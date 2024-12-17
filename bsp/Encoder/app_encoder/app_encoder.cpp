/**
 * @file app_Encoder.cpp
 * @brief Encoder 应用级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-7-6
 * @copyright
 * @par 日志:
 * 		V1.0 分离原本脉冲编码器bsp_Encoder库，脱离为不受硬件显示的计算库
 * 		V1.1 新增速度获取函数
 *
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "app_encoder.hpp"

#include "tle5012/bsp_tle5012b.h"
/* 私有类型定义 --------------------------------------------------------------*/

#define WEAK __attribute__((weak)) // 使用WEAK类型是方便特殊电机来重构特定函数

#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
// 宏定义实现的一个约束函数,用于限制一个值的范围。
// 具体来说，该宏定义的名称为 _constrain，接受三个参数 amt、low 和 high，分别表示要限制的值、最小值和最大值。该宏定义的实现使用了三元运算符，根据 amt 是否小于 low 或大于 high，返回其中的最大或最小值，或者返回原值。
// 换句话说，如果 amt 小于 low，则返回 low；如果 amt 大于 high，则返回 high；否则返回 amt。这样，_constrain(amt, low, high) 就会将 amt 约束在 [low, high] 的范围内。

encoder HallEncoder(2 * PI, ReadAngle);

////******************************************* encoder 编码器类*************************************************************************////
/**
 * @brief  encoder 编码器类
 * @param [in]   counter_period	完整一圈的计数值
 * @param [in]	get_count_link	获取计数值连接函数
 *
 */
WEAK encoder::encoder(float counter_period, float_func_none get_count_link)
{
	this->counter_period = counter_period;
	this->get_count_link = get_count_link;
}

/**
 * @brief  set_TurnCheckRange 临界范围设置
 * @param [in]   Turn_Check_range	转过临界的判断范围
 *
 */
void encoder::set_TurnCheckRange(float Turn_Check_range)
{
	Turn_Check_range = _constrain(Turn_Check_range, 0, 0.5); // 限制允许的范围
	this->Turn_Check_range = Turn_Check_range;
}

/**
 * @brief  get_count 获取计数值函数
 * @param 	[in]   	counter_period    	完整一圈的计数值
 * @param 	[in]    get_count_link    	获取计数值连接函数
 * @retval	[out]	count				当前计数值
 */
float encoder::get_count(void)
{
	count = get_count_link();
	return count;
}

/**
 * @brief  get_data 获取计数值函数
 * @param
 * @param
 * @retval	[out]	date				总计数值
 */
float encoder::get_date(void)
{
	/* 读取一下现在的 count */
	get_count();
	/* 判断过临界周转标志 */
	if (count <= (counter_period * Turn_Check_range))
	{
		/* 判断是否正转成立 */
		if (Turn_Check == 1)
		{
			Turn++; // 圈数加一
		}
		Turn_Check = -1;
	}
	else if (count >= (counter_period * (1 - Turn_Check_range)))
	{
		/* 判断是否正转成立 */
		if (Turn_Check == -1)
		{
			Turn--; // 圈数加一
		}
		Turn_Check = 1;
	}
	else
	{
		Turn_Check = 0;
	}
	date = Turn * counter_period + count;
	return date;
}

/**
 * @brief  获取速度函数
 * @details	获得转速值，单位 弧度/s
 * @param
 * @retval 转速值
 */
float encoder::get_speed(void)
{
	get_date();
	/* 计算采样时间 */
	uint64_t time_now = MICROS_us();
	float Ts = (float)(MICROS_us() - time_prev) * 1e-6f; // seconds
	// float Ts = (float)(aaa - time_prev) * 1e-6f; // seconds

	/* 修复溢出的情况 */
	if (Ts <= 0)
		Ts = 1e-3f;
	/* 速度计算 */
	speed = (date - date_prev) / Ts;
	/* 更新暂存变量 */
	date_prev = date;
	time_prev = time_now;
	return speed;
}
