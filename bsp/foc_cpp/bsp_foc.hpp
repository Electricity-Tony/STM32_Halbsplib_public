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

#ifndef __BSP_FOC_HPP
#define __BSP_FOC_HPP
#include "stm32f1xx.h"
#include "main.h"

#include "PID_cpp/bsp_pid.hpp"
#include "app_encoder/app_encoder.hpp"
#include "systick/bsp_systick.h"

/* pwm 勾选 fastmode */
/* gpio 速度设置为最大 */

/* pwm 使能的枚举类型 */
typedef enum
{
	PWM_DISABLE = 0,
	PWM_ENABLE = 1
} PWM_STATE;

/* pwmio 类 定义 */
class pwmio
{
public:
	TIM_HandleTypeDef *htim; // 使用的时钟
	uint32_t TIM_CHANNEL;	 // 使用的时钟通道计数值

	// 成员函数
	pwmio(void){};
	pwmio(TIM_HandleTypeDef *htim, uint32_t TIM_CHANNEL);
	void set_ccr(uint16_t ccr);		 // 设置 pwmio 的 比较值
	void set_state(PWM_STATE state); // pwm 输出控制函数

protected:
	uint16_t ccr;	 // 通道的比较值
	PWM_STATE state; // pwm输出状态
};

/* 用来设定 foc 正转的旋转方向 */
typedef enum
{
	FORWARD = 1,
	REVERSE = -1
} DIR_STATE;

/*****			foc 构造函数				******/
class foc
{
public:
	pwmio *pwm_u, *pwm_v, *pwm_w; // 三个pwm控制电机三相

	DIR_STATE dir;			// 正转的旋转方向
	uint8_t pole_pairs;		// 极对数
	float shaft_angle;		// 机械角度
	float electrical_angle; // 电角度

	pid *_PID_OUT; // 外环位置环
	void set_PID_OUT(pid *_PID_OUT);

	encoder *_encoder;					 // 使用的编码器
	void set_encoder(encoder *_encoder); // 编码器设置函数

	// 成员函数
	foc(void){};
	foc(pwmio *pwm_u, pwmio *pwm_v, pwmio *pwm_w, int pole_pairs, DIR_STATE dir = FORWARD);
	void init(void);														 // foc 初始化函数
	void set_voltage_limit(float voltage_limit, float voltage_power_supply); // 电压限制设置函数
	void run_speed_Openloop(float target_velocity);							 // 开环运行函数
	void run_angle(float target_angle);										 // 角度闭环运行函数

	float voltage_limit;		// 输出限制电压
	float voltage_power_supply; // 电源电压

	/* 原始电角度偏差值 */
	float zero_electrical_angle = 0.0f;							// 原始电角度偏差值
	float init_ZeroElectricalAngle(uint16_t delaytime);			// 自动检测初始化电角度偏差值函数
	float set_ZeroElectricalAngle(float zero_electrical_angle); // 原始电角度设定

	/* 输入控制参数 */
	float Uq, Ud;

	/* 帕克逆变换后的中间量 */
	float Ualpha;
	float Ubeta;
	/* 克拉克逆变换后的中间量 */
	float Uu, Uv, Uw;

	// 保护成员函数
	float shaftAngle_2_electricalAngle(void);			  // 电角度转换函数
	float _normalizeAngle(float angle);					  // 角度标准化为[0,2PI]
	void run_QDangle(float Uq, float Ud, float angle_el); // 输入Uq，Ud，和电角度，通过克拉克与帕克逆变换
	void run_UVW(float Uu, float Uv, float Uw);			  // 根据最后电压运行函数

protected:
	uint16_t _tim_autoreload; // 当前时钟的重装载值
};

extern foc motor_1612;
extern pwmio pwm_u;
extern pid pid_1612;

#endif