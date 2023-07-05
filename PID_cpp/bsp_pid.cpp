
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
#include "bsp_pid.hpp"
#include <string.h>
#include <math.h>

#define WEAK __attribute__((weak)) // 使用WEAK类型是方便特殊电机来重构特定函数
#define ABS(x) ((x) > 0 ? (x) : -(x))
using namespace std;

////*******************************************传统PID类*************************************************************************////
/**
 * @brief  传统pid构造函数
 * @param [in]   P 比例系数
 * @param [in]	 I 积分系数
 * @param [in]	 D 微分系数
 * @param [in]	 IMax 积分限幅
 * @param [in]	 PIDMax 输出限幅
 * @param [in]	 I_Time 积分时间
 *	@param [in]	 D_Time 积分时间
 * @param [in]	 I_Limited 进行积分调节的误差区间限制
 * @par 日志
 *
 */
WEAK pid::pid(float P, float I, float D, float IMax, float PIDMax, uint16_t I_Time, uint16_t D_Time, uint16_t I_Limited)
{
	this->P = P;
	this->I = I;
	this->D = D;
	this->IMax = IMax;
	this->PIDMax = PIDMax;
	this->I_Time = I_Time;
	this->D_Time = D_Time;
	this->I_Limited = I_Limited;
}
/**
 * @brief  非线性pid构造函数
 * @param [in]   Kp ∈(ap,ap+bp),err=0时Kp最小,bp为变化区间,cp调整Kp变化速率,偶函数开口向上
 * @param [in]   Ki ∈(0,ai),err=0时ki最大,ci调整Ki变化速率,偶函数开口向下
 * @param [in]   Kd ∈(ad,ad+bd),err=0时Kd=ad+bd/(1+cd),dd调整Kd变化速率,单调递减
 * @param [in]	 IMax 积分限幅
 * @param [in]	 PIDMax 输出限幅
 * @param [in]	 T 积分时间
 * @param [in]	 I_Limited 进行积分调节的误差区间限制
 * @par 日志
 *
 */
pid::pid(float ap, float bp, float cp,
		 float ai, float ci,
		 float ad, float bd, float cd, float dd,
		 float IMax, float PIDMax, uint16_t I_Time, uint16_t D_Time, uint16_t I_Limited)
{
	this->ap = ap;
	this->bp = bp;
	this->cp = cp;
	this->ai = ai;
	this->ci = ci;
	this->ad = ad;
	this->bd = bd;
	this->cd = cd;
	this->dd = dd;
	this->IMax = IMax;
	this->PIDMax = PIDMax;
	this->I_Time = I_Time;
	this->D_Time = D_Time;
	this->I_Limited = I_Limited;
}
/**
 * @brief  pid运行函数
 * @param [in]   err 传入pid环的误差
 * @retval  pid的运行输出
 * @par 日志
 *				2019年12月1日15:00:00 移除积分时间不设置的兼容性改动，加入微分时间
 */
WEAK float pid::pid_run(float err)
{
	CurrentError = err;
	Pout = CurrentError * P;

	// 积分分离
	if (HAL_GetTick() - I_start_time >= I_Time) // 如果达到了时间区间的话则进行积分输出
	{
		if (ABS(CurrentError) < I_Limited) // 仅在小于误差区间时进行I积分
			Iout += I * CurrentError;
		else
			Iout = 0;				  // 误差区间外边积分清0
		I_start_time = HAL_GetTick(); // 重新定义积分开始时间
	}

	if (Custom_Diff != NULL) // 存在自定义微分数据
		Dout_Accumulative = (*Custom_Diff) * D;
	else
		Dout_Accumulative = (CurrentError - LastError) * D;

	if (HAL_GetTick() - D_start_time > D_Time) // 如果达到了时间区间的话则进行微分输出
	{
		Dout = Dout_Accumulative;
		Dout_Accumulative = 0;
		D_start_time = HAL_GetTick(); // 重新定义微分开始时间
	}

	if (Iout >= IMax)
		Iout = IMax;
	if ((Iout) <= -(IMax))
		Iout = -(IMax); // 积分限幅

	PIDout = Pout + Iout + Dout; // Pid输出计算
	if (PIDout >= PIDMax)
		PIDout = PIDMax;
	if (PIDout <= -(PIDMax))
		PIDout = -(PIDMax); // 输出限幅

	LastError = CurrentError;
	return PIDout;
}
/**
 * @brief  非线性pid运行函数
 * @param [in]   err 传入pid环的误差
 * @retval  pid的运行输出
 * @par 日志
 *
 */
float pid::nonlinear_pid_run(float err)
{
	P = ap + bp * (1 - sech(cp * err));
	I = ai * sech(ci * err);

	// 非线性pid增益调节参数曲线——kd曲线为单调递减函数，貌似只支持一种变化方向，此处处理保持输入kd(err)的err>0
	if (err > 0)
		D = ad + bd / (1 + cd * exp(dd * err));
	else
		D = ad + bd / (1 + cd * exp(dd * (-err)));

	return pid_run(err);
}
float pid::sech(float in)
{
	return 1 / cosh(in);
}

// 卸载宏定义
#undef ABS
