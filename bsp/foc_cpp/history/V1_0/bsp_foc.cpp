/**
 * @file bsp_foc.cpp
 * @brief foc板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-7-8
 * @copyright
 * @par 日志:
 *   V1.0 基本构建，完成： 1.开环速度控制 2.基于编码器的位置闭环控制
 *
 */

#include "bsp_foc.hpp"
#include "tim.h"

#include "stm32f1xx_ll_cortex.h"
#include <math.h>

#define WEAK __attribute__((weak)) // 使用WEAK类型是方便特殊电机来重构特定函数
#define BSP_FOC_PI 3.1415926535f

// 初始变量及函数定义
#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
// 宏定义实现的一个约束函数,用于限制一个值的范围。
// 具体来说，该宏定义的名称为 _constrain，接受三个参数 amt、low 和 high，分别表示要限制的值、最小值和最大值。该宏定义的实现使用了三元运算符，根据 amt 是否小于 low 或大于 high，返回其中的最大或最小值，或者返回原值。
// 换句话说，如果 amt 小于 low，则返回 low；如果 amt 大于 high，则返回 high；否则返回 amt。这样，_constrain(amt, low, high) 就会将 amt 约束在 [low, high] 的范围内。

/* 默认声明变量样例 */
pwmio pwm_u(&htim2, TIM_CHANNEL_1);
pwmio pwm_v(&htim2, TIM_CHANNEL_2);
pwmio pwm_w(&htim2, TIM_CHANNEL_3);
foc motor_1612(&pwm_u, &pwm_v, &pwm_w, 7, REVERSE);
pid pid_1612(-30.0f, 0, 0, 0, 4);
// foc motor_1612(&pwm_u, &pwm_v, &pwm_w, 4);

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
	* @param [in]	pole_pairs		电机极对数

	*
*/
WEAK foc::foc(pwmio *pwm_u, pwmio *pwm_v, pwmio *pwm_w, int pole_pairs, DIR_STATE dir)
{
	this->pwm_u = pwm_u;
	this->pwm_v = pwm_v;
	this->pwm_w = pwm_w;
	this->pole_pairs = pole_pairs;
	this->dir = dir;

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
	Uu = _constrain(Uu, 0.0f, voltage_limit);
	Uv = _constrain(Uv, 0.0f, voltage_limit);
	Uw = _constrain(Uw, 0.0f, voltage_limit);
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
	/* 上电到特定角度 */
	run_QDangle(voltage_limit / 2, 0, 3 * BSP_FOC_PI / 2);
	/* 等待电机转到该位置 */
	HAL_Delay(delaytime);
	/* 编码器读取该位置的机械角度 */
	shaft_angle = _encoder->get_count();
	/* 转换为该位置绝对电角度 */
	set_ZeroElectricalAngle(0);
	shaftAngle_2_electricalAngle();
	/* 该位置的电角度设置为 初始零电角度 */
	set_ZeroElectricalAngle(electrical_angle);
	run_QDangle(0, 0, 3 * BSP_FOC_PI / 2);

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
	electrical_angle = _normalizeAngle((float)(dir * pole_pairs) * shaft_angle - zero_electrical_angle);
	// electrical_angle = _normalizeAngle((float)(dir * pole_pairs) * shaft_angle);
	return electrical_angle;
}

// 归一化角度到 [0,2PI]
float foc::_normalizeAngle(float angle)
{
	float a = fmod(angle, 2 * BSP_FOC_PI); // 取余运算可以用于归一化，列出特殊值例子算便知
	return a >= 0 ? a : (a + 2 * BSP_FOC_PI);
	// 三目运算符。格式：condition ? expr1 : expr2
	// 其中，condition 是要求值的条件表达式，如果条件成立，则返回 expr1 的值，否则返回 expr2 的值。可以将三目运算符视为 if-else 语句的简化形式。
	// fmod 函数的余数的符号与除数相同。因此，当 angle 的值为负数时，余数的符号将与 _2PI 的符号相反。也就是说，如果 angle 的值小于 0 且 _2PI 的值为正数，则 fmod(angle, _2PI) 的余数将为负数。
	// 例如，当 angle 的值为 -PI/2，_2PI 的值为 2PI 时，fmod(angle, _2PI) 将返回一个负数。在这种情况下，可以通过将负数的余数加上 _2PI 来将角度归一化到 [0, 2PI] 的范围内，以确保角度的值始终为正数。
}

/**
 * @brief  克拉克与帕克逆变换 后的运行函数
 * @details
 * @param  Uq :主要的输入电压目标
 * @param  Ud :没用的需要忽略
 * @param  angle_el :电角度
 * @retval
 */
void foc::run_QDangle(float Uq_input, float Ud_input, float angle_el)
{
	this->Uq = _constrain(Uq_input, -(voltage_power_supply) / 2, (voltage_power_supply) / 2);
	this->Ud = Ud_input;
	angle_el = _normalizeAngle(angle_el + zero_electrical_angle);

	// 帕克逆变换
	// Ualpha = Ud * cos(angle_el) - Uq * sin(angle_el);
	// Ubeta = Uq * cos(angle_el) + Ud * sin(angle_el);
	// 忽略 Ud 项，减少计算量
	Ualpha = -Uq * sin(angle_el);
	Ubeta = Uq * cos(angle_el);

	float temp_Uu, temp_Uv, temp_Uw; // 中间暂存的 u v w 值，之后输入到 foc 类中
	// 克拉克逆变换
	temp_Uu = Ualpha + voltage_power_supply / 2;
	temp_Uv = (sqrt(3) * Ubeta - Ualpha) / 2 + voltage_power_supply / 2;
	temp_Uw = (-Ualpha - sqrt(3) * Ubeta) / 2 + voltage_power_supply / 2;
	run_UVW(temp_Uu, temp_Uv, temp_Uw);
}

/**
 * @brief  开环虚拟速度运行函数
 * @details
 * @param  target_velocity :目标速度
 * @retval
 */
void foc::run_speed_Openloop(float target_velocity)
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
	shaft_angle = _normalizeAngle(shaft_angle + target_velocity * Ts);
	/* 计算电角度 */
	shaftAngle_2_electricalAngle();

	/* 直接设置 Uq 为电压上限，进行输出 */
	Uq = voltage_limit / 3;
	run_QDangle(Uq, 0, electrical_angle);
	openloop_timestamp = now_us;
}

/**
 * @brief  闭环位置运行函数
 * @details
 * @param  target_angle :目标角度，由于过程中计算均使用弧度值，这里输入角度制，方便理解
 * @retval
 */
void foc::run_angle(float target_angle)
{
	/* 转换输入的角度值变为弧度值 */
	target_angle = (target_angle / 180.0f) * BSP_FOC_PI;
	/* 获取机械角度 */
	shaft_angle = _encoder->get_count();
	/* 转化为电角度 */
	shaftAngle_2_electricalAngle();
	/* 运行pid */
	run_QDangle(_PID_OUT->pid_run(target_angle - shaft_angle), 0, electrical_angle);
	// run_QDangle(3, 0, electrical_angle);
}
