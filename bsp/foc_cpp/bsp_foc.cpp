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

// #include "stm32f4xx_ll_cortex.h"
#include <math.h>

#define WEAK __attribute__((weak)) // 使用WEAK类型是方便特殊电机来重构特定函数

/* 默认声明变量样例 */
pwmio pwm_u(&htim1, TIM_CHANNEL_1, PWM_ENABLE);
pwmio pwm_v(&htim1, TIM_CHANNEL_2, PWM_ENABLE);
pwmio pwm_w(&htim1, TIM_CHANNEL_3, PWM_ENABLE);
foc motor(&pwm_u, &pwm_v, &pwm_w, 7, REVERSE);
// foc motor(&pwm_u, &pwm_v, &pwm_w, 7, FORWARD);
pid motor_pid_out(5.0f, 0.0f, 100.0f, 0.0f, 300.0f);
pid motor_pid_in(0.03f, 0.0f, 0.0f, 3.0f, 3.0f);
/* 这个是不进入电流环直接输出电压的 */
// float p = 0.03f;
// float i = 0.003f;
// float d = 0.05f;
// pid motor_pid_in(p, i, 0, 2.0f, 2.0f);
// pid motor_pid_in(0, 0, d, 2.0f, 2.0f);

float current_p = -0.02f;
// float current_i = -0.001f;
float current_i = current_p / 10;
// float current_i = -0.0f;
float current_i_max = 2.0f;
float current_pid_max = 2.0f;
pid motor_pid_IQ(current_p, current_i, 0.0f, current_i_max, current_pid_max);
pid motor_pid_ID(current_p, current_i, 0.0f, current_i_max, current_pid_max);

current_sensor foc_current_sensor = current_sensor(&bsp_adc_dma1);

/* pwmio 构造函数 */
WEAK pwmio::pwmio(TIM_HandleTypeDef *htim, uint32_t TIM_CHANNEL, PWM_STATE __TIM_EX_FLAG)
{
	this->htim = htim;
	this->TIM_CHANNEL = TIM_CHANNEL;
	this->ccr = 0;

	this->__TIM_EX_FLAG = __TIM_EX_FLAG;
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
		if (this->__TIM_EX_FLAG == PWM_ENABLE)
		{
			HAL_TIMEx_PWMN_Start(this->htim, TIM_CHANNEL);
		}
		HAL_TIM_PWM_Start(this->htim, TIM_CHANNEL);
	}
	else if (state == PWM_DISABLE)
	{
		if (this->__TIM_EX_FLAG == PWM_ENABLE)
		{
			HAL_TIMEx_PWMN_Stop(this->htim, TIM_CHANNEL);
		}
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
	// this->pwm_u->set_state(PWM_ENABLE);
	// this->pwm_v->set_state(PWM_ENABLE);
	// this->pwm_w->set_state(PWM_ENABLE);

	this->pwm_u->set_ccr(0);
	this->pwm_v->set_ccr(0);
	this->pwm_w->set_ccr(0);
	_tim_autoreload = __HAL_TIM_GetAutoreload(this->pwm_u->htim);

	/* 给一个50%的占空比，让adc校准有下降沿触发 */
	this->run_UVW(0.5 * this->voltage_power_supply, 0.5 * this->voltage_power_supply, 0.5 * this->voltage_power_supply);
}

/**
 * @brief  foc 上电
 * @details	使能foc
 * @param
 * @retval
 */
void foc::enable(void)
{
	if (this->motor_state != FOC_ENABLE)
	{
		if (this->EN_GPIO_Port != nullptr)
		{
			HAL_GPIO_WritePin(this->EN_GPIO_Port, this->EN_GPIO_Pin, GPIO_PIN_SET);
		}
		this->pwm_u->set_state(PWM_ENABLE);
		this->pwm_v->set_state(PWM_ENABLE);
		this->pwm_w->set_state(PWM_ENABLE);
		motor_state = FOC_ENABLE;
	}
}

/**
 * @brief  foc 下电
 * @details	使能foc
 * @param
 * @retval
 */
void foc::disable(void)
{
	if (this->motor_state != FOC_DISABLE)
	{
		if (this->EN_GPIO_Port != nullptr)
		{
			HAL_GPIO_WritePin(this->EN_GPIO_Port, this->EN_GPIO_Pin, GPIO_PIN_RESET);
		}
		this->pwm_u->set_state(PWM_DISABLE);
		this->pwm_v->set_state(PWM_DISABLE);
		this->pwm_w->set_state(PWM_DISABLE);
		motor_state = FOC_DISABLE;
		__HAL_TIM_ENABLE(&htim1); // ！！关闭PWM输出会自动失能定时器，需要手动打开
	}
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
 * @details 默认以中间计算的三相占空比输出
 * @param
 * @retval
 */
void foc::run_UVW(void)
{
	/* 跳转至带输入参数的 */
	this->run_UVW(this->Uu, this->Uv, this->Uw);
}

/**
 * @brief  foc 输出函数
 * @details
 * @param  	Uu :u相输出电压
 * @param  	Uv :v相输出电压
 * @param  	Uw :w相输出电压
 * @retval
 */
void foc::run_UVW(float Uu_in, float Uv_in, float Uw_in)
{
	// 设置上限
	Uu_in = _constrain(Uu_in, 0.6f, voltage_limit);
	Uv_in = _constrain(Uv_in, 0.6f, voltage_limit);
	Uw_in = _constrain(Uw_in, 0.6f, voltage_limit);
	this->Uu = Uu_in;
	this->Uv = Uv_in;
	this->Uw = Uw_in;

	// 计算占空比，写入到pwm
	this->pwm_u->set_ccr(Uu / voltage_power_supply * _tim_autoreload);
	this->pwm_v->set_ccr(Uv / voltage_power_supply * _tim_autoreload);
	this->pwm_w->set_ccr(Uw / voltage_power_supply * _tim_autoreload);
}

/**
 * @brief  foc 時間输出函数(ccr计数值)
 * @details
 * @param  	Tu :u相输出计数值
 * @param  	Tv :v相输出计数值
 * @param  	Tw :w相输出计数值
 * @retval
 */
void foc::run_UVW_T(void)
{
	// 计算占空比，写入到pwm
	run_UVW_T((uint16_t)this->Svpwm_Mod.ta,
			  (uint16_t)this->Svpwm_Mod.tb,
			  (uint16_t)this->Svpwm_Mod.tc);
}

/**
 * @brief  foc 時間输出函数(ccr计数值)
 * @details
 * @param  	Tu :u相输出计数值
 * @param  	Tv :v相输出计数值
 * @param  	Tw :w相输出计数值
 * @retval
 */
void foc::run_UVW_T(uint16_t Tu, uint16_t Tv, uint16_t Tw)
{
	// 计算占空比，写入到pwm
	this->pwm_u->set_ccr(Tu);
	this->pwm_v->set_ccr(Tv);
	this->pwm_w->set_ccr(Tw);
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
	set_ZeroElectricalAngle(0.0f);
	/* 上电到特定角度 */
	foc_run_mode runMode_temp = this->run_mode;
	this->run_mode = calMode;
	this->enable();
	// Clark_Park_Inverse(voltage_limit / 2, 0, 3 * PI / 2);
	// Clark_Park_Inverse(0, voltage_limit / 4, 0);
	this->U_q = 0;
	this->U_d = voltage_limit / 10;
	// this->electrical_angle = 0;
	SvpwmCtrl();
	run_UVW_T();

	// SvpwmCtrl();
	// run_UVW_T();
	/* 等待电机转到该位置 */
	while (delaytime--)
	{
		HAL_Delay(1);
		/* 编码器读取该位置的机械角度 */
		shaft_angle = _encoder->get_count();
	}

	/* 转换为该位置绝对电角度 */
	shaftAngle_2_electricalAngle();
	/* 该位置的电角度设置为 初始零电角度 */
	set_ZeroElectricalAngle(electrical_angle);
	Clark_Park_Inverse(0, 0, 3 * PI / 2);
	SvpwmCtrl();
	run_UVW_T();

	this->run_mode = runMode_temp;
	this->disable();

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
	return shaftAngle_2_electricalAngle(this->shaft_angle);
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
	this->U_q = _constrain(U_q_input, -(voltage_limit) / 2, (voltage_limit) / 2);
	this->U_d = _constrain(U_d_input, -(voltage_limit) / 2, (voltage_limit) / 2);
	// angle_el = _normalizeAngle(angle_el + zero_electrical_angle);

	// 帕克逆变换
	U_alpha = U_d * cos(angle_el) - U_q * sin(angle_el);
	U_beta = U_q * cos(angle_el) + U_d * sin(angle_el);
	// 电流闭环后不可忽略 I_d 项
	// U_alpha = -U_q * sin(angle_el);
	// U_beta = U_q * cos(angle_el);
}

/**
 * @brief  Spwm 控制方式，输出到pwm占空比
 * @details
 * @param
 * @retval
 */
void foc::SpwmCtrl(void)
{
	this->Uu = U_alpha + voltage_power_supply / 2;
	this->Uv = (sqrt(3) * U_beta - U_alpha) / 2 + voltage_power_supply / 2;
	this->Uw = (-U_alpha - sqrt(3) * U_beta) / 2 + voltage_power_supply / 2;
}

/**
 * @brief  Svpwm 控制方式，输出到pwm占空比
 * @details
 * @param
 * @retval
 */
void foc::SvpwmCtrl(void)
{
	/* 变量转移，Svpwm结构体可独立使用 */

	this->Svpwm_Mod.uAlpha = this->U_alpha;
	this->Svpwm_Mod.uBeta = this->U_beta;
	this->Svpwm_Mod.ts = this->_tim_autoreload;		  // 时钟周期
	this->Svpwm_Mod.udc = this->voltage_power_supply; // 输出的母线电压
	// this->Svpwm_Mod.udc = this->voltage_limit;	// 输出的母线电压--这里放limit是错误的，都被用limit归一化了电压限幅就没有了

	this->Svpwm_Mod.K = sqrt(3) * this->Svpwm_Mod.ts / this->Svpwm_Mod.udc; // Svpwm系数
	// this->Svpwm_Mod.K = sqrt(3); // 这里我实际上不适用 ts 和 Udc，最后在run_UVW，在调用

	/* 扇区判断 */
	uint8_t a, b, c, n;
	this->Svpwm_Mod.u1 = this->Svpwm_Mod.uBeta;
	this->Svpwm_Mod.u2 = sqrt(3) / 2 * this->Svpwm_Mod.uAlpha - this->Svpwm_Mod.uBeta / 2;
	this->Svpwm_Mod.u3 = -sqrt(3) / 2 * this->Svpwm_Mod.uAlpha - this->Svpwm_Mod.uBeta / 2;

	if (this->Svpwm_Mod.u1 > 0)
	{
		a = 1;
	}
	else
	{
		a = 0;
	}
	if (this->Svpwm_Mod.u2 > 0)
	{
		b = 1;
	}
	else
	{
		b = 0;
	}
	if (this->Svpwm_Mod.u3 > 0)
	{
		c = 1;
	}
	else
	{
		c = 0;
	}

	n = 4 * c + 2 * b + a;
	switch (n)
	{
	case 3:
		this->Svpwm_Mod.sector = 1;
		break;
	case 1:
		this->Svpwm_Mod.sector = 2;
		break;
	case 5:
		this->Svpwm_Mod.sector = 3;
		break;
	case 4:
		this->Svpwm_Mod.sector = 4;
		break;
	case 6:
		this->Svpwm_Mod.sector = 5;
		break;
	case 2:
		this->Svpwm_Mod.sector = 6;
		break;
	}

	/* 计算矢量作用时长 */
	switch (this->Svpwm_Mod.sector)
	{
	case 1:
		this->Svpwm_Mod.t4 = this->Svpwm_Mod.K * this->Svpwm_Mod.u2;
		this->Svpwm_Mod.t6 = this->Svpwm_Mod.K * this->Svpwm_Mod.u1;
		this->Svpwm_Mod.t0 = this->Svpwm_Mod.t7 = (this->Svpwm_Mod.ts - this->Svpwm_Mod.t4 - this->Svpwm_Mod.t6) / 2;
		break;
	case 2:
		this->Svpwm_Mod.t2 = -this->Svpwm_Mod.K * this->Svpwm_Mod.u2;
		this->Svpwm_Mod.t6 = -this->Svpwm_Mod.K * this->Svpwm_Mod.u3;
		this->Svpwm_Mod.t0 = this->Svpwm_Mod.t7 = (this->Svpwm_Mod.ts - this->Svpwm_Mod.t2 - this->Svpwm_Mod.t6) / 2;
		break;
	case 3:
		this->Svpwm_Mod.t2 = this->Svpwm_Mod.K * this->Svpwm_Mod.u1;
		this->Svpwm_Mod.t3 = this->Svpwm_Mod.K * this->Svpwm_Mod.u3;
		this->Svpwm_Mod.t0 = this->Svpwm_Mod.t7 = (this->Svpwm_Mod.ts - this->Svpwm_Mod.t2 - this->Svpwm_Mod.t3) / 2;
		break;
	case 4:
		this->Svpwm_Mod.t1 = -this->Svpwm_Mod.K * this->Svpwm_Mod.u1;
		this->Svpwm_Mod.t3 = -this->Svpwm_Mod.K * this->Svpwm_Mod.u2;
		this->Svpwm_Mod.t0 = this->Svpwm_Mod.t7 = (this->Svpwm_Mod.ts - this->Svpwm_Mod.t1 - this->Svpwm_Mod.t3) / 2;
		break;
	case 5:
		this->Svpwm_Mod.t1 = this->Svpwm_Mod.K * this->Svpwm_Mod.u3;
		this->Svpwm_Mod.t5 = this->Svpwm_Mod.K * this->Svpwm_Mod.u2;
		this->Svpwm_Mod.t0 = this->Svpwm_Mod.t7 = (this->Svpwm_Mod.ts - this->Svpwm_Mod.t1 - this->Svpwm_Mod.t5) / 2;
		break;
	case 6:
		this->Svpwm_Mod.t4 = -this->Svpwm_Mod.K * this->Svpwm_Mod.u3;
		this->Svpwm_Mod.t5 = -this->Svpwm_Mod.K * this->Svpwm_Mod.u1;
		this->Svpwm_Mod.t0 = this->Svpwm_Mod.t7 = (this->Svpwm_Mod.ts - this->Svpwm_Mod.t4 - this->Svpwm_Mod.t5) / 2;
		break;
	default:
		break;
	}

	/* Svpwm 生成 */
	switch (this->Svpwm_Mod.sector)
	{
	case 1:
		this->Svpwm_Mod.ta = this->Svpwm_Mod.t4 + this->Svpwm_Mod.t6 + this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tb = this->Svpwm_Mod.t6 + this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tc = this->Svpwm_Mod.t7;
		break;
	case 2:
		this->Svpwm_Mod.ta = this->Svpwm_Mod.t6 + this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tb = this->Svpwm_Mod.t2 + this->Svpwm_Mod.t6 + this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tc = this->Svpwm_Mod.t7;
		break;
	case 3:
		this->Svpwm_Mod.ta = this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tb = this->Svpwm_Mod.t2 + this->Svpwm_Mod.t3 + this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tc = this->Svpwm_Mod.t3 + this->Svpwm_Mod.t7;
		break;
	case 4:
		this->Svpwm_Mod.ta = this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tb = this->Svpwm_Mod.t3 + this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tc = this->Svpwm_Mod.t1 + this->Svpwm_Mod.t3 + this->Svpwm_Mod.t7;
		break;
	case 5:
		this->Svpwm_Mod.ta = this->Svpwm_Mod.t5 + this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tb = this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tc = this->Svpwm_Mod.t1 + this->Svpwm_Mod.t5 + this->Svpwm_Mod.t7;
		break;
	case 6:
		this->Svpwm_Mod.ta = this->Svpwm_Mod.t4 + this->Svpwm_Mod.t5 + this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tb = this->Svpwm_Mod.t7;
		this->Svpwm_Mod.tc = this->Svpwm_Mod.t5 + this->Svpwm_Mod.t7;
		break;
	}

	// /* 不带入时间和母线电压的归一化占空比，在最后运行函数里面算 */
	// this->Uu = this->Svpwm_Mod.ta;
	// this->Uv = this->Svpwm_Mod.tb;
	// this->Uw = this->Svpwm_Mod.tc;
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
void foc::set_degree(float _target_degree)
{
	target_degree = _target_degree;
	run_mode = angleMode;
}

/* ************************** 采样功能函数 **************************************** */

/**
 * @brief  编码器采样更新函数
 * @details
 * @param
 * @retval
 */
void foc::encoder_update(void)
{
	if (_encoder != nullptr)
	{
		/* 过溢出点速度改变判断 */
		float speed_temp = this->encoder_dir * this->_encoder->get_speed();
		speed_temp = Rad2Rot(speed_temp);
		if (ABS(speed_temp) < 5000)
		{
			this->speed = speed_temp;
		}
		else
		{
		}
		this->speed = _SPEED_LowPassFilter->run(this->speed);
		this->shaft_angle = this->encoder_dir * this->_encoder->date;
		shaftAngle_2_electricalAngle(this->encoder_dir * this->shaft_angle);
	}
}

/**
 * @brief  运行函数
 * @details 放在周期循环中自动更新运行状态
 * @param
 * @retval
 */
void foc::run(void)
{
	// /* 编码器数据更新在最开始 */
	// this->encoder_update();
	// /* 电流转换 */
	// if (_current_sensor != nullptr)
	// {
	// 	this->_current_sensor->update(); // 放到定时器中运行了
	// 	this->Clark_Park(this->_current_sensor->phase_u.data,
	// 					 this->_current_sensor->phase_v.data,
	// 					 this->_current_sensor->phase_w.data,
	// 					 this->electrical_angle);

	// 	/* 电流低通滤波 */
	// 	this->I_q = this->_Iq_LowPassFilter->run(this->I_q);
	// 	this->I_d = this->_Id_LowPassFilter->run(this->I_d);
	// }
	// /* 校准模式 */
	// if (this->run_mode == calMode)
	// {
	// 	this->electrical_angle = 0;
	// 	this->U_q = 0;
	// 	this->U_d = voltage_limit / 4;
	// 	set_ZeroElectricalAngle(0.0f);
	// }
	// /* 开环运行模式 */
	// else if (this->run_mode == openloop)
	// {
	// 	uint64_t now_us = MICROS_us(); // 获得从芯片启动开始的微秒时间
	// 	/* 计算每个 loop 的运行时间 */
	// 	static uint32_t openloop_timestamp; // 用于计算时间间隔
	// 	float Ts = (now_us - openloop_timestamp) * 1e-6f;

	// 	/* now_us 会在大约 70min 后跳变到 0 ，因此需要进行修正 */
	// 	/* Ts 过大直接修正为一个较小的值 */
	// 	// Ts = Ts > 0.5f ? 1e-3f : Ts;
	// 	if (Ts <= 0 || Ts > 0.5f)
	// 		Ts = 1e-3f;
	// 	/* 通过时间的目标速度虚拟的角度，需要对机械角度归一化为 [0,2PI] */
	// 	static float openloop_shaft_angle;
	// 	openloop_shaft_angle = _normalizeAngle(openloop_shaft_angle + Rot2Rad(target_speed) * Ts);
	// 	/* 计算电角度 */
	// 	shaftAngle_2_electricalAngle(openloop_shaft_angle);

	// 	/* 电流闭环运行 */
	// 	// this->target_current = voltage_limit / 3;
	// 	// this->U_q = _PID_CURRENT->pid_run(this->target_current - this->I_q);
	// 	// this->U_d = _PID_CURRENT->pid_run(0 - this->I_d);
	// 	/* 直接设置 I_q 为电压上限，进行输出 */
	// 	this->U_q = voltage_limit / 3;
	// 	this->U_d = 0;
	// 	/* 放在最后了 */
	// 	// Clark_Park_Inverse(I_q, 0, electrical_angle);
	// 	openloop_timestamp = now_us;
	// }
	// /* 速度闭环运行模式 */
	// else if (this->run_mode == speedMode)
	// {
	// 	/* 放在最上面了 */
	// 	/* 速度环部分 */
	// 	// /* 转换输入的速度值 角度值 变为弧度值 */
	// 	// target_speed = (target_speed / 180.0f) * PI;
	// 	/* 获取速度角度 */
	// 	// speed = _encoder->get_speed();
	// 	// speed = Rad2Rot(speed);
	// 	/* 速度通过低通滤波器 */
	// 	// speed = _SPEED_LowPassFilter->run(speed);
	// 	/* 获得角度值 */
	// 	// shaft_angle = _encoder->date;
	// 	/* 转化为电角度 */
	// 	// shaftAngle_2_electricalAngle();

	// 	/* 控制死区 */
	// 	float err_temp = this->target_speed - this->speed;
	// 	if (err_temp > -0.3 && err_temp < 0.3)
	// 		err_temp = 0;
	// 	/* 运行pid */
	// 	this->target_current = _PID_IN->pid_run(err_temp);
	// 	/* 电流环部分 */
	// 	this->U_q = _PID_IQ->pid_run(this->target_current - this->I_q);
	// 	this->U_d = _PID_ID->pid_run(0 - this->I_d);
	// }
	// /* 位置闭环运行模式 */
	// else if (this->run_mode == angleMode)
	// {

	// 	/* 运行pid,这里是串级pid，先位置外环再速度内环 */
	// 	this->degree = Rad2Angle(shaft_angle);
	// 	this->target_speed = _PID_OUT->pid_run(this->target_degree - this->degree);
	// 	/* 速度环部分 */
	// 	/* 可以绕过速度环 */
	// 	// this->target_current = _PID_IN->pid_run(this->target_speed - speed);
	// 	this->target_current = _PID_OUT->pid_run(this->target_degree - this->degree);
	// 	/* 电流环部分 */
	// 	this->U_q = _PID_IQ->pid_run(this->target_current - this->I_q);
	// 	this->U_d = _PID_ID->pid_run(0 - this->I_d);
	// }
	// /* 电流闭环运行 */
	// else if (this->run_mode == currentMode)
	// {
	// 	/* 获取机械角度 */
	// 	// shaft_angle = this->_encoder->get_date();
	// 	/* 转化为电角度 */
	// 	// shaftAngle_2_electricalAngle();
	// 	/* 电流采样 */
	// 	// this->_current_sensor->update();
	// 	/* 运行 克拉克和帕克 变换 */
	// 	// this->Clark_Park(this->_current_sensor->phase_u.data,
	// 	// 				 this->_current_sensor->phase_v.data,
	// 	// 				 this->_current_sensor->phase_w.data,
	// 	// 				 this->electrical_angle);
	// 	/* 运行pid,这里是串级pid，运行电流环 */
	// 	this->U_q = _PID_IQ->pid_run(this->target_current - this->I_q);
	// 	this->U_d = _PID_ID->pid_run(0 - this->I_d);
	// 	/* 仅电流环d轴调试使用 */
	// 	// this->U_q = 0;
	// 	// this->U_d = _PID_ID->pid_run(this->target_current - this->I_d);
	// }

	// Clark_Park_Inverse(U_q, U_d, electrical_angle); // 克拉克帕克变换
	// // Clark_Park_Inverse(0, 4, 0); // 零电角度调试
	// // SpwmCtrl();										// 计算Spwm方式的占空比
	// // run_UVW();										// 输出占空比

	// // Clark_Park_Inverse(voltage_limit / 2, 0, 3 * PI / 2);

	// SvpwmCtrl();
	// run_UVW_T();
}

/**
 * @brief  位置环运行函数
 * @details 放在周期循环中自动更新运行状态
 * @param
 * @retval
 */
void foc::angle_run(void)
{
	/* 不进入模式也把数据算出来 */
	this->degree = Rad2Angle(shaft_angle);

	if (this->run_mode == angleMode)
	{
		/* 运行pid,这里是串级pid，先位置外环再速度内环 */
		this->target_speed = _PID_OUT->pid_run(this->target_degree - this->degree);
	}
}

/**
 * @brief  速度环运行函数
 * @details 放在周期循环中自动更新运行状态
 * @param
 * @retval
 */
void foc::speed_run(void)
{
	/* 速度频率不能高了，只能放在这里 */
	this->encoder_update();
	/* 不进入模式也把数据算出来 */
	if (this->run_mode == speedMode || this->run_mode == angleMode)
	{
		float err_temp = this->target_speed - this->speed;
		if (err_temp > -0.3 && err_temp < 0.3)
			err_temp = 0;
		/* 运行pid */
		this->target_current = _PID_IN->pid_run(err_temp);
	}
	if (this->run_mode == openloop)
	{
	}
}

/**
 * @brief  电流环运行函数
 * @details 放在周期循环中自动更新运行状态
 * @param
 * @retval
 */
void foc::current_run(void)
{
	/* 编码器数据更新在最开始 */
	// this->_encoder->get_date();	//这个位置采样编码器频率跟不上
	/* 电流转换 */
	if (_current_sensor != nullptr)
	{
		this->_current_sensor->update(); // 放到定时器中运行了
		this->Clark_Park(this->_current_sensor->phase_u.data,
						 this->_current_sensor->phase_v.data,
						 this->_current_sensor->phase_w.data,
						 this->electrical_angle);

		/* 电流低通滤波 */
		// this->I_q = this->_Iq_LowPassFilter->run(this->I_q);
		// this->I_d = this->_Id_LowPassFilter->run(this->I_d);

		/* 电流闭环 */
		if (this->run_mode == calMode)
		{
			this->U_q = 0;
			this->U_d = voltage_limit / 10;
			set_ZeroElectricalAngle(0.0f);

			Clark_Park_Inverse(U_q, U_d, 0); // 克拉克帕克变换
			SvpwmCtrl();
			run_UVW_T();
			return;
		}
		else if (this->run_mode == openloop) // 开环强拖
		{
			static uint16_t openloop_flag = 0;
			static float openloop_shaft_angle;
			openloop_flag++;
			if (openloop_flag == 10)
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

				openloop_shaft_angle = _normalizeAngle(openloop_shaft_angle + Rot2Rad(target_speed) * Ts);
				/* 计算电角度 */
				shaftAngle_2_electricalAngle(openloop_shaft_angle);

				/* 电流闭环运行 */
				// this->target_current = voltage_limit / 3;
				// this->U_q = _PID_CURRENT->pid_run(this->target_current - this->I_q);
				// this->U_d = _PID_CURRENT->pid_run(0 - this->I_d);
				/* 直接设置 I_q 为电压上限，进行输出 */
				this->U_q = voltage_limit / 4;
				this->U_d = 0;
				/* 放在最后了 */
				// Clark_Park_Inverse(I_q, 0, electrical_angle);
				openloop_timestamp = now_us;
				openloop_flag = 0;
			}
			shaftAngle_2_electricalAngle(openloop_shaft_angle);
		}
		else // 需要电流闭环下
		{
			/* 运行pid,这里是串级pid，运行电流环 */
			this->U_q = _PID_IQ->pid_run(this->target_current - this->I_q);
			this->U_d = _PID_ID->pid_run(0 - this->I_d);

			/* 避开电流环，直接速度环进来 */
			// this->U_q = this->target_current;
			// this->U_d = 0;

			/* 仅电流环d轴调试使用 */
			// this->U_q = 0;
			// this->U_d = _PID_ID->pid_run(this->target_current - this->I_d);
		}

		Clark_Park_Inverse(U_q, U_d, electrical_angle); // 克拉克帕克变换
		// Clark_Park_Inverse(0, 4, 0); // 零电角度调试

		SvpwmCtrl();
		run_UVW_T();
	}
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
 * @brief  foc 电流Q环设置函数
 * @details
 * @param  	_PID_CURRENT :链接配置的 pid 类，用于控制电流闭环的，属于电流环
 * @retval
 */
void foc::set_PID_IQ(pid *_PID_IQ)
{
	this->_PID_IQ = _PID_IQ;
}

/**
 * @brief  foc 电流D环设置函数
 * @details
 * @param  	_PID_CURRENT :链接配置的 pid 类，用于控制电流闭环的，属于电流环
 * @retval
 */
void foc::set_PID_ID(pid *_PID_ID)
{
	this->_PID_ID = _PID_ID;
}

/**
 * @brief  foc 电流闭环使用 克拉克和帕克变换
 * @details	 自动更新 I_alpha,I_beta,I_q,I_d
 * @param  	Uu,I_q,Uw
 * @retval
 */
void foc::Clark_Park(float Uu_in, float Uv_in, float Uw_in, float angle_el)
{
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
void current_sensor::set_phase(phase *_phase, int8_t channel)
{
	_phase->channel = channel;
}

/**
 * @brief  current phase 设置函数
 * @details
 * @param	三个通道的序号
 * @retval
 */
void current_sensor::init(int8_t channel_u, int8_t channel_v, int8_t channel_w)
{
	set_phase(&phase_u, channel_u);
	set_phase(&phase_v, channel_v);
	set_phase(&phase_w, channel_w);
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
	// phase_u.data = phase_u.ratio * ((11 / 10.39 * 0.39 * 3.3) - (phase_u.data * 3.3 / 4095)) * 0.1 - phase_u.offset;
	// phase_v.data = phase_v.ratio * ((11 / 10.39 * 0.39 * 3.3) - (phase_v.data * 3.3 / 4095)) * 0.1 - phase_v.offset;
	// // phase_w.data = phase_w.ratio * ((11 / 10.39 * 0.39 * 3.3) - (phase_w.data * 3.3 / 4095)) * 0.1 - phase_w.offset;
	// phase_w.data = -(phase_u.data + phase_v.data);

	// phase_u.data = phase_u.data;
	// phase_v.data = phase_v.data;
	// phase_w.data = phase_w.data;

	phase_u.data = (phase_u.data - 2048) * 0.008056640625f - phase_u.offset;
	phase_v.data = (phase_v.data - 2048) * 0.008056640625f - phase_v.offset;
	phase_w.data = (phase_w.data - 2048) * 0.008056640625f - phase_w.offset;
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
