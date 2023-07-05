
/**
 * @file bsp_pid.cpp
 * @brief pid板级支持包
 * @author Tony_Wang
 * @version 1.1
 * @date 2023-6-19
 * @copyright
 * @par 日志:
 *   V1.0 单独将电机库分离独立成库
 *
 */
#ifndef __BSP_PID_HPP
#define __BSP_PID_HPP

/// 选定使用的STM32的头文件
#include "stm32f1xx_hal.h"

using namespace std;

// PID可以算pid输出 可以配置pid
class pid
{
public:
	float *Custom_Diff = NULL; //!< 自定义的外界浮点型微分数值 常用于路程环的微分环节(即速度值)
	uint16_t I_Time;		   //!< pid时间参数 以ms为单位 plus专属,积分时间
	uint16_t D_Time;		   //!< 微分时间，ms为单位
	uint16_t I_Limited;		   //!< 当误差小于I_Limited时才进行I输出 plus专属
	float ap = 0, bp = 0, cp;  // ap==0 && bp==0说明不是非线性pid
	float ai = 0, ci;
	float ad = 0, bd = 0, cd, dd;
	float P;
	float I;
	float D;
	float IMax;
	float PIDMax;

	// 公共函数
	pid(float P, float I, float D, float IMax, float PIDMax, uint16_t I_Time = 1, uint16_t D_Time = 1, uint16_t I_Limited = 9999); // 传统pid构造函数
	pid(float ap, float bp, float cp,
		float ai, float ci,
		float ad, float bd, float cd, float dd,
		float IMax, float PIDMax, uint16_t I_Time = 1, uint16_t D_Time = 1, uint16_t I_Limited = 9999); // 非线性pid构造函数
	float pid_run(float err);
	float nonlinear_pid_run(float err);
	float sech(float in);

private:
	// 运算储存区
	float Pout;
	float Iout;
	float Dout;
	float Dout_Accumulative; // 因为有微分时间重定义,因此加入一个变量真正用于输出D,源Dout用作累积误差
	float PIDout;
	float CurrentError;
	float LastError;
	uint32_t I_start_time; //!< 积分开始时间戳，用于带时间参数的pid   plus专属
	uint32_t D_start_time; //!< 微分开始时间戳，用于带时间参数的pid
};

#endif
