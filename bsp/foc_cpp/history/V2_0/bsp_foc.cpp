/**
 * @file bsp_foc.cpp
 * @brief foc板级支持包
 * @author Tony_Wang
 * @version 2.0 Beta
 * @date 2023-9-18
 * @copyright
 * @par 日志:
 *   V1.0 		基本构建，完成： 1.开环速度控制 2.基于编码器的位置闭环控制
 * 	 V1.1 		重写运行逻辑，改为模式运行
 *	 V2.0 Beta  新增电流采样和电流闭环，但是没有调通
 */

#include "bsp_foc.hpp"
#include "tim.h"

#include "stm32f1xx_ll_cortex.h"
#include <math.h>

#define WEAK __attribute__((weak)) // 使用WEAK类型是方便特殊电机来重构特定函数

/* 默认声明变量样例 */
pwmio pwm_u(&htim2, TIM_CHANNEL_1);
pwmio pwm_v(&htim2, TIM_CHANNEL_2);
pwmio pwm_w(&htim2, TIM_CHANNEL_3);
// foc motor_1612(&pwm_u, &pwm_v, &pwm_w, 7, REVERSE);
foc motor_1612(&pwm_u, &pwm_v, &pwm_w, 7, FORWARD, M_EN_GPIO_Port, M_EN_Pin);
pid pid_out_1612(5.0f, 0.0f, 0.0f, 0.0f, 300.0f);
// pid pid_in_1612(0.003, 0.0001, 0, 0.5, 0.5);
pid pid_in_1612(0.003f, 0.0001f, 0.0f, 0.3f, 0.3f);
pid pid_current_1612(-5.0f, -0.5f, 0.0f, 4.0f, 4.0f);

current_sensor foc_current_sensor = current_sensor(&bsp_adc_dma1);

/* pwmio 构造函数 */
WEAK pwmio::pwmio(TIM_HandleTypeDef *htim, uint32_t TIM_CHANNEL)
{
	this->htim = htim;
	this->TIM_CHANNEL = TIM_CHANNEL;
	this->ccr = 0;
}

/**
 * @brief  设置 pwmio 的 比较值
 * @details
 * @param  ccr :比较值
 * @retval
 */
void pwmio::set_ccr(uint16_t ccr)
{
	this->ccr = ccr;
	__HAL_TIM_SET_COMPARE(this->htim, this->TIM_CHANNEL, this->ccr);
}

/**
 * @brief  开启 pwm 输出
 * @details
 * @param  state :enable disable
 * @retval
 */
void pwmio::set_state(PWM_STATE state)
{
	this->state = state;
	if (state == PWM_ENABLE)
	{
		HAL_TIM_PWM_Start(this->htim, TIM_CHANNEL);
	}
	else if (state == PWM_DISABLE)
	{
		HAL_TIM_PWM_Stop(this->htim, this->TIM_CHANNEL);
	}
}

////******************************************* foc 电机类*************************************************************************////
/**
	* @brief  foc 电机类
	* @param [in]   *pwm_u			U 通道 pwm
	* @param [in]	*pwm_v			V 通道 pwm
	* @param [in]	*pwm_w			W 通道 pwm
	* @param [in]	*EN_GPIO_Port	电机使能端口号
	* @param [in]	EN_GPIO_Pin		电机使能引脚号
	* @param [in]	pole_pairs		电机极对数

	*
*/
WEAK foc::foc(pwmio *pwm_u, pwmio *pwm_v, pwmio *pwm_w, int pole_pairs, DIR_STATE dir, GPIO_TypeDef *EN_GPIO_Port, uint16_t EN_GPIO_Pin)
{
	this->pwm_u = pwm_u;
	this->pwm_v = pwm_v;
	this->pwm_w = pwm_w;
	this->pole_pairs = pole_pairs;
	this->dir = dir;

	/* 电机使能IO配置 */
	this->EN_GPIO_Port = EN_GPIO_Port;
	this->EN_GPIO_Pin = EN_GPIO_Pin;

	// 全幅限制，保护电机不转
	this->voltage_limit = 0;
	this->voltage_power_supply = 5;
}

/**
 * @brief  foc 初始化函数
 * @details	包括开启使用的三个pwm定时器，读取定时器的 重装载值
 * @param
 * @retval
 */
void foc::init(void)
{
	this->pwm_u->set_state(PWM_ENABLE);
	this->pwm_v->set_state(PWM_ENABLE);
	this->pwm_w->set_state(PWM_ENABLE);

	this->pwm_u->set_ccr(0);
	this->pwm_v->set_ccr(0);
	this->pwm_w->set_ccr(0);
	_tim_autoreload = __HAL_TIM_GetAutoreload(this->pwm_u->htim);

	/* 给一个50%的占空比，让adc校准有下降沿触发 */
	motor_1612.run_UVW(0.5 * motor_1612.voltage_power_supply, 0.5 * motor_1612.voltage_power_supply, 0.5 * motor_1612.voltage_power_supply);
}

/**
 * @brief  foc 上电
 * @details	使能foc
 * @param
 * @retval
 */
void foc::enable(void)
{
	HAL_GPIO_WritePin(this->EN_GPIO_Port, this->EN_GPIO_Pin, GPIO_PIN_SET);
}

/**
 * @brief  foc 下电
 * @details	使能foc
 * @param
 * @retval
 */
void foc::disable(void)
{
	HAL_GPIO_WritePin(this->EN_GPIO_Port, this->EN_GPIO_Pin, GPIO_PIN_RESET);
}

/**
 * @brief  foc 输出限制配置函数
 * @details
 * @param  	voltage_limit :限制的输出电压
 * @param  	voltage_power_supply :提供的电源电压
 * @retval
 */
void foc::set_voltage_limit(float voltage_limit, float voltage_power_supply)
{
	this->voltage_limit = _constrain(voltage_limit, 0.0f, voltage_power_supply);
	this->voltage_power_supply = voltage_power_supply;
}

/**
 * @brief  foc 编码器设置函数
 * @details
 * @param  	_encoder :链接配置的编码器，这是一个软核，输出要求为弧度值[0,2PI]
 * @retval
 */
void foc::set_encoder(encoder *_encoder)
{
	this->_encoder = _encoder;
}

/**
 * @brief  foc 编码器设置滤波器函数
 * @details	定义里面 new 了一个 LowPassFilter 类，调用设定函数的 delete 默认的
 * @param  	_encoder :链接配置的编码器，这是一个软核，输出要求为弧度值[0,2PI]
 * @retval
 */
void foc::set_SPEED_LowPassFilter(LowPassFilter *_SPEED_LowPassFilter)
{
	delete this->_SPEED_LowPassFilter; // 释放默认的滤波器
	this->_SPEED_LowPassFilter = _SPEED_LowPassFilter;
}

/**
 * @brief  foc q电流环设置滤波器函数
 * @details	定义里面 new 了一个 LowPassFilter 类，调用设定函数的 delete 默认的
 * @param  	_encoder :链接配置的编码器，这是一个软核，输出要求为弧度值[0,2PI]
 * @retval
 */
void foc::set_Iq_LowPassFilter(LowPassFilter *_Iq_LowPassFilter)
{
	delete this->_Iq_LowPassFilter; // 释放默认的滤波器
	this->_Iq_LowPassFilter = _Iq_LowPassFilter;
}

/**
 * @brief  foc d电流环设置滤波器函数
 * @details	定义里面 new 了一个 LowPassFilter 类，调用设定函数的 delete 默认的
 * @param  	_encoder :链接配置的编码器，这是一个软核，输出要求为弧度值[0,2PI]
 * @retval
 */
void foc::set_Id_LowPassFilter(LowPassFilter *_Id_LowPassFilter)
{
	delete this->_Id_LowPassFilter; // 释放默认的滤波器
	this->_Id_LowPassFilter = _Id_LowPassFilter;
}

/**
 * @brief  foc 外环设置函数
 * @details
 * @param  	_PID_OUT :链接配置的 pid 类，用于控制位置闭环的，属于外环
 * @retval
 */
void foc::set_PID_OUT(pid *_PID_OUT)
{
	this->_PID_OUT = _PID_OUT;
}

/**
 * @brief  foc 内环设置函数
 * @details
 * @param  	_PID_IN :链接配置的 pid 类，用于控制速度闭环的，属于内环
 * @retval
 */
void foc::set_PID_IN(pid *_PID_IN)
{
	this->_PID_IN = _PID_IN;
}

/**
 * @brief  foc 输出函数
 * @details
 * @param  	Uu :u相输出电压
 * @param  	Uv :v相输出电压
 * @param  	Uw :w相输出电压
 * @retval
 */
void foc::run_UVW(float Uu, float Uv, float Uw)
{
	// 设置上限
	Uu = _constrain(Uu, 0.6f, voltage_limit);
	Uv = _constrain(Uv, 0.6f, voltage_limit);
	Uw = _constrain(Uw, 0.6f, voltage_limit);
	this->Uu = Uu;
	this->Uv = Uv;
	this->Uw = Uw;

	// 计算占空比，写入到pwm
	this->pwm_u->set_ccr(Uu / voltage_power_supply * _tim_autoreload);
	this->pwm_v->set_ccr(Uv / voltage_power_supply * _tim_autoreload);
	this->pwm_w->set_ccr(Uw / voltage_power_supply * _tim_autoreload);
}

/**
 * @brief  原始电角度设定
 * @details
 * @param  zero_electrical_angle :设定的 原始电角度
 * @retval
 */
float foc::set_ZeroElectricalAngle(float zero_electrical_angle)
{
	this->zero_electrical_angle = zero_electrical_angle;
	return zero_electrical_angle;
}

/**
 * @brief  自动检测初始化电角度偏差值函数
 * @details
 * @param  delaytime :等待电机给一个等特定电压的时间，用于稳定电机
 * @retval
 */
float foc::init_ZeroElectricalAngle(uint16_t delaytime)
{
	/* 压根没有霍尔，测个p的角度 */
	if (this->_encoder == nullptr)
		return 0;

	/* 有霍尔 */
	/* 上电到特定角度 */
	Clark_Park_Inverse(voltage_limit / 2, 0, 3 * PI / 2);
	/* 等待电机转到该位置 */
	HAL_Delay(delaytime);
	/* 编码器读取该位置的机械角度 */
	shaft_angle = _encoder->get_count();
	/* 转换为该位置绝对电角度 */
	set_ZeroElectricalAngle(0);
	shaftAngle_2_electricalAngle();
	/* 该位置的电角度设置为 初始零电角度 */
	set_ZeroElectricalAngle(electrical_angle);
	Clark_Park_Inverse(0, 0, 3 * PI / 2);

	return zero_electrical_angle;
}

/* 功能性函数 */
/**
 * @brief  电角度计算函数
 * @details
 * @param  shaft_angle :机械角度
 * @retval
 */
float foc::shaftAngle_2_electricalAngle(void)
{
	shaftAngle_2_electricalAngle(this->shaft_angle);
}

/**
 * @brief  电角度计算函数
 * @details
 * @param  shaft_angle :机械角度
 * @retval
 */
float foc::shaftAngle_2_electricalAngle(float shaft_angle_putin)
{
	electrical_angle = _normalizeAngle((float)(dir * pole_pairs) * shaft_angle_putin - zero_electrical_angle);
	// electrical_angle = _normalizeAngle((float)(dir * pole_pairs) * shaft_angle);
	return electrical_angle;
}

// 归一化角度到 [0,2PI]
float foc::_normalizeAngle(float angle)
{
	float a = fmod(angle, 2 * PI); // 取余运算可以用于归一化，列出特殊值例子算便知
	return a >= 0 ? a : (a + 2 * PI);
	// 三目运算符。格式：condition ? expr1 : expr2
	// 其中，condition 是要求值的条件表达式，如果条件成立，则返回 expr1 的值，否则返回 expr2 的值。可以将三目运算符视为 if-else 语句的简化形式。
	// fmod 函数的余数的符号与除数相同。因此，当 angle 的值为负数时，余数的符号将与 _2PI 的符号相反。也就是说，如果 angle 的值小于 0 且 _2PI 的值为正数，则 fmod(angle, _2PI) 的余数将为负数。
	// 例如，当 angle 的值为 -PI/2，_2PI 的值为 2PI 时，fmod(angle, _2PI) 将返回一个负数。在这种情况下，可以通过将负数的余数加上 _2PI 来将角度归一化到 [0, 2PI] 的范围内，以确保角度的值始终为正数。
}

/**
 * @brief  克拉克与帕克逆变换 后的运行函数
 * @details
 * @param  I_q :主要的输入电压目标
 * @param  I_d :没用的需要忽略
 * @param  angle_el :电角度
 * @retval
 */
void foc::Clark_Park_Inverse(float U_q_input, float U_d_input, float angle_el)
{
	this->U_q = _constrain(U_q_input, -(voltage_power_supply) / 2, (voltage_power_supply) / 2);
	this->U_d = _constrain(U_d, -(voltage_power_supply) / 2, (voltage_power_supply) / 2);
	angle_el = _normalizeAngle(angle_el + zero_electrical_angle);

	// 帕克逆变换
	U_alpha = U_d * cos(angle_el) - U_q * sin(angle_el);
	U_beta = U_q * cos(angle_el) + U_d * sin(angle_el);
	// 忽略 I_d 项，减少计算量
	// U_alpha = -U_q * sin(angle_el);
	// U_beta = U_q * cos(angle_el);

	float temp_Uu, temp_Uv, temp_Uw; // 中间暂存的 u v w 值，之后输入到 foc 类中
	// 克拉克逆变换
	temp_Uu = U_alpha + voltage_power_supply / 2;
	temp_Uv = (sqrt(3) * U_beta - U_alpha) / 2 + voltage_power_supply / 2;
	temp_Uw = (-U_alpha - sqrt(3) * U_beta) / 2 + voltage_power_supply / 2;
	run_UVW(temp_Uu, temp_Uv, temp_Uw);
}

/**
 * @brief  目标速度设置函数
 * @details
 * @param  target_speed :目标速度，单位 rpm
 * @retval
 */
void foc::set_speed(float _target_speed)
{
	target_speed = _target_speed;
	/* 判断是否有编码器 */
	if (this->_encoder != nullptr)
	{
		/* 有编码器，速度闭环模式 */
		run_mode = speedMode;
	}
	else
	{
		/* 无编码器，开环速度模式 */
		run_mode = openloop;
	}
}

/**
 * @brief  带模式的 目标速度设置函数
 * @details
 * @param  target_speed :目标速度，单位 rpm
 * @param	mode:运行模式
 * @retval
 */
void foc::set_speed(float _target_speed, foc_run_mode mode)
{
	target_speed = _target_speed;
	run_mode = mode;
}

/**
 * @brief  目标角度设置函数
 * @details
 * @param  target_speed :目标角度，单位 度°
 * @retval
 */
void foc::set_angle(float _target_angle)
{
	target_angle = _target_angle;
	run_mode = angleMode;
}

/**
 * @brief  运行函数
 * @details 放在周期循环中自动更新运行状态
 * @param
 * @retval
 */
void foc::run(void)
{
	/* 速度采集放在最开始 */
	this->speed = this->encoder_dir * this->_encoder->get_speed();
	this->speed = Rad2Rot(this->speed);
	this->speed = _SPEED_LowPassFilter->run(this->speed);
	this->shaft_angle = this->encoder_dir * this->_encoder->date;
	shaftAngle_2_electricalAngle(this->encoder_dir * this->shaft_angle);
	/* 电流转换 */
	this->_current_sensor->update(); // 放到定时器中运行了
	this->Clark_Park(this->_current_sensor->phase_u.data,
					 this->_current_sensor->phase_v.data,
					 this->_current_sensor->phase_w.data,
					 this->electrical_angle);
	/* 电流低通滤波 */
	this->I_q = this->_Iq_LowPassFilter->run(this->I_q);
	this->I_d = this->_Id_LowPassFilter->run(this->I_d);
	/* 开环运行模式 */
	if (this->run_mode == openloop)
	{
		uint64_t now_us = MICROS_us(); // 获得从芯片启动开始的微秒时间
		/* 计算每个 loop 的运行时间 */
		static uint32_t openloop_timestamp; // 用于计算时间间隔
		float Ts = (now_us - openloop_timestamp) * 1e-6f;

		/* now_us 会在大约 70min 后跳变到 0 ，因此需要进行修正 */
		/* Ts 过大直接修正为一个较小的值 */
		// Ts = Ts > 0.5f ? 1e-3f : Ts;
		if (Ts <= 0 || Ts > 0.5f)
			Ts = 1e-3f;
		/* 通过时间的目标速度虚拟的角度，需要对机械角度归一化为 [0,2PI] */
		static float openloop_shaft_angle;
		openloop_shaft_angle = _normalizeAngle(openloop_shaft_angle + Rot2Rad(target_speed) * Ts);
		/* 计算电角度 */
		shaftAngle_2_electricalAngle(openloop_shaft_angle);

		/* 电流闭环运行 */
		// this->target_current = voltage_limit / 3;
		// this->U_q = _PID_CURRENT->pid_run(this->target_current - this->I_q);
		// this->U_d = _PID_CURRENT->pid_run(0 - this->I_d);
		/* 直接设置 I_q 为电压上限，进行输出 */
		this->U_q = voltage_limit / 3;
		this->U_d = 0;
		/* 放在最后了 */
		// Clark_Park_Inverse(I_q, 0, electrical_angle);
		openloop_timestamp = now_us;
	}
	/* 速度闭环运行模式 */
	else if (this->run_mode == speedMode)
	{
		/* 放在最上面了 */
		/* 速度环部分 */
		// /* 转换输入的速度值 角度值 变为弧度值 */
		// target_speed = (target_speed / 180.0f) * PI;
		/* 获取速度角度 */
		// speed = _encoder->get_speed();
		// speed = Rad2Rot(speed);
		/* 速度通过低通滤波器 */
		// speed = _SPEED_LowPassFilter->run(speed);
		/* 获得角度值 */
		// shaft_angle = _encoder->date;
		/* 转化为电角度 */
		// shaftAngle_2_electricalAngle();

		/* 控制死区 */
		float err_temp = this->target_speed - this->speed;
		if (err_temp > -0.3 && err_temp < 0.3)
			err_temp = 0;
		/* 运行pid */
		this->target_current = _PID_IN->pid_run(err_temp);
		/* 电流环部分 */
		this->U_q = _PID_CURRENT->pid_run(this->target_current - this->I_q);
		this->U_d = _PID_CURRENT->pid_run(0 - this->I_d);
	}
	/* 位置闭环运行模式 */
	else if (this->run_mode == angleMode)
	{
		/* 位置环部分 */
		/* 获取速度角度 */
		// speed = _encoder->get_speed();
		// speed = Rad2Rot(speed);
		/* 速度通过低通滤波器 */
		// speed = _SPEED_LowPassFilter->run(speed);
		/* 转换输入的角度值变为弧度值 */
		// target_angle = (target_angle / 180.0f) * PI;
		/* 获取机械角度 */
		// shaft_angle = _encoder->date;
		/* 转化为电角度 */
		// shaftAngle_2_electricalAngle();
		/* 运行pid,这里是串级pid，先位置外环再速度内环 */
		this->target_speed = _PID_OUT->pid_run(this->target_angle - Rad2Angle(shaft_angle));
		/* 速度环部分 */
		this->target_current = _PID_IN->pid_run(this->target_speed - speed);
		/* 电流环部分 */
		this->U_q = _PID_CURRENT->pid_run(this->target_current - this->I_q);
		this->U_d = _PID_CURRENT->pid_run(0 - this->I_d);
	}
	/* 电流闭环运行 */
	else if (this->run_mode == currentMode)
	{
		/* 获取机械角度 */
		// shaft_angle = this->_encoder->get_date();
		/* 转化为电角度 */
		// shaftAngle_2_electricalAngle();
		/* 电流采样 */
		// this->_current_sensor->update();
		/* 运行 克拉克和帕克 变换 */
		// this->Clark_Park(this->_current_sensor->phase_u.data,
		// 				 this->_current_sensor->phase_v.data,
		// 				 this->_current_sensor->phase_w.data,
		// 				 this->electrical_angle);
		/* 运行pid,这里是串级pid，运行电流环 */
		this->U_q = _PID_CURRENT->pid_run(this->target_current - this->I_q);
		this->U_d = _PID_CURRENT->pid_run(0 - this->I_d);
	}

	Clark_Park_Inverse(U_q, U_d, electrical_angle);
}

/* ------------------------------------------------------------------------------------------------------------------------------------------ */
/**
 * @brief  foc 电流采样设置函数
 * @details
 * @param  	_current_sensor :链接配置的电流采样，这是一个软核
 * @retval
 */
void foc::set_current_sensor(current_sensor *_current_sensor)
{
	this->_current_sensor = _current_sensor;
}

/**
 * @brief  目标电流设置函数
 * @details
 * @param  target_current :目标角度，单位 度°
 * @retval
 */
void foc::set_current(float _target_current)
{
	target_current = _target_current;
	run_mode = currentMode;
}

/**
 * @brief  foc 电流环设置函数
 * @details
 * @param  	_PID_CURRENT :链接配置的 pid 类，用于控制电流闭环的，属于电流环
 * @retval
 */
void foc::set_PID_CURRENT(pid *_PID_CURRENT)
{
	this->_PID_CURRENT = _PID_CURRENT;
}

/**
 * @brief  foc 电流闭环使用 克拉克和帕克变换
 * @details	 自动更新 I_alpha,I_beta,I_q,I_d
 * @param  	Uu,I_q,Uw
 * @retval
 */
void foc::Clark_Park(float Uu_in, float Uv_in, float Uw_in, float angle_el)
{
	this->_current_sensor->update();
	this->I_alpha = Uu_in;
	this->I_beta = Uu_in / sqrt(3) + 2 * Uv_in / sqrt(3);

	this->I_q = I_beta * cos(angle_el) - I_alpha * sin(angle_el);
	this->I_d = I_beta * sin(angle_el) + I_alpha * cos(angle_el);
}

////******************************************* current_sensor 电流采样类*************************************************************************////
/**
 * @brief  current_sensor 电机类
 * @param [in]   *pwm_u			U 通道 pwm
 *
 */
WEAK current_sensor::current_sensor(bsp_ADC_DMA *adc_dma)
{
	this->adc_dma = adc_dma;
}

/**
 * @brief  current_sensor phase 设置函数
 * @details
 * @param
 * @retval
 */
void current_sensor::set_phase(phase *_phase, int8_t channel, float ratio)
{
	_phase->channel = channel;
	_phase->ratio = ratio;
}

/**
 * @brief  current phase 设置函数
 * @details
 * @param	三个通道的序号 电流计算系数
 * @retval
 */
void current_sensor::init(int8_t channel_u, int8_t channel_v, int8_t channel_w, float ratio)
{
	init(channel_u, channel_v, channel_w, ratio, ratio, ratio);
}

void current_sensor::init(int8_t channel_u, int8_t channel_v, int8_t channel_w, float ratio_u, float ratio_v, float ratio_w)
{
	set_phase(&phase_u, channel_u, ratio_u);
	set_phase(&phase_v, channel_v, ratio_v);
	set_phase(&phase_w, channel_w, ratio_w);
}

/**
 * @brief  current phase 校准函数
 * @details 校准三个通道补偿值，一定放在定时器之后
 * @param
 * @retval
 */
void current_sensor::calibration(uint16_t times)
{
	float calibration_buffer_u = 0, calibration_buffer_v = 0, calibration_buffer_w = 0;
	this->phase_u.offset = 0;
	this->phase_v.offset = 0;
	this->phase_w.offset = 0;
	/* 巨坑！！！ 实测这里再开始采样出来的数据默认就是 2.725120 先给他采样出来滤掉 */
	for (uint16_t i = 0; i < 10; i++)
	{
		this->adc_dma->getonce();
		this->update();
		HAL_Delay(1);
	}
	for (uint16_t i = 0; i < times; i++)
	{
		this->adc_dma->getonce();
		this->update();
		calibration_buffer_u += this->phase_u.data;
		calibration_buffer_v += this->phase_v.data;
		calibration_buffer_w += this->phase_w.data;
		HAL_Delay(1);
	}
	this->phase_u.offset = calibration_buffer_u / times;
	this->phase_v.offset = calibration_buffer_v / times;
	this->phase_w.offset = calibration_buffer_w / times;
}

/**
 * @brief  current 数据更新函数
 * @details 这个函数需要根据需要调整
 * @param
 * @retval
 */
void current_sensor::update(void)
{
	adc_dma->update();
	if (phase_u.channel != -1)
	{
		/* u 相是有采样的 */
		phase_u.data = adc_dma->data_list[phase_u.channel];
		if (phase_v.channel != -1)
		{
			/* v 相是有采样的 */
			phase_v.data = adc_dma->data_list[phase_v.channel];
			if (phase_w.channel != -1)
			{
				/* w 相是有采样的 */
				phase_w.data = adc_dma->data_list[phase_w.channel];
				/* last: u,w,v */
				/* 三相均采样，精确化 */
			}
			else
			{
				/* last: u,v : w */
				/* 添加 w 的计算 */
			}
		}
		else
		{
			/* v 相没采样 */
			if (phase_w.channel != -1)
			{
				/* w 相是有采样的 */
				phase_w.data = adc_dma->data_list[phase_w.channel];
				/* last: u,w : v */
				/* 添加 v 的计算 */
			}
			else
			{
				/* w 相没采样 */
				/* last: u : v,w */
				/* 单电阻，后续优化 */
			}
		}
	}
	else
	{
		/* u 相没采样 */
		if (phase_v.channel != -1)
		{
			/* v 相是有采样的 */
			phase_v.data = adc_dma->data_list[phase_v.channel];
			if (phase_w.channel != -1)
			{
				/* w 相是有采样的 */
				phase_w.data = adc_dma->data_list[phase_w.channel];
				/* last: w,v ：u */
				/* 添加 u 的计算 */
			}
			else
			{
				/* last: v : w,u */
				/* 单电阻，后续优化 */
			}
		}
		else
		{
			/* v 相没采样 */
			if (phase_w.channel != -1)
			{
				/* w 相是有采样的 */
				phase_w.data = adc_dma->data_list[phase_w.channel];
				/* last: w : v,u */
				/* 单电阻，后续优化 */
			}
			else
			{
				/* w 相没采样 */
				/* last: 压根没采样 */
				/* 这里需要报错 */
			}
		}
	}
	/* 系数计算 */
	phase_u.data = phase_u.ratio * ((11 / 10.39 * 0.39 * 3.3) - (phase_u.data * 3.3 / 4095)) * 0.1 - phase_u.offset;
	phase_v.data = phase_v.ratio * ((11 / 10.39 * 0.39 * 3.3) - (phase_v.data * 3.3 / 4095)) * 0.1 - phase_v.offset;
	// phase_w.data = phase_w.ratio * ((11 / 10.39 * 0.39 * 3.3) - (phase_w.data * 3.3 / 4095)) * 0.1 - phase_w.offset;
	phase_w.data = -(phase_u.data + phase_v.data);
}

/* 以下内容放在定时器中断里 */
/* 中断定时器直接使用 pwm 的 中心采样，具体见 readme */
// if (htim == &htim2)
//     {
//         static uint8_t Tick1 = 0;
//         static uint8_t Tick2 = 0;
//         if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2)) // 50us 20k Hz
//         {
//             if (Tick1 < 1) // 1: 50us
//             {
//                 Tick1++;
//             }
//             else
//             {
//                 Tick1 = 0;
//                 motor_1612._current_sensor->adc_dma->getonce();
//             }

//             if (Tick2 < 19) // 19:1000us
//             {
//                 Tick2++;
//             }
//             else
//             {
//                 Tick2 = 0;
//                 motor_1612.run();
//                 // app_motor_can_Basic_postback(&motor_1612);
//             }
//         }
//     }
