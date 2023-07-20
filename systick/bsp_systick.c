/**
 * @file bsp_systick.c
 * @brief systick板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-7-11
 * @copyright
 * @par 日志:
 *   V1.0 基本构建，用于实现单片机内部滴答定时器时间计算
 *
 */

#include "bsp_systick.h"

/* getCurrentMicros()函数用于获取自MCU复位以来的运行时间，单位微秒，
	这个函数的实质还是使用了系统滴答定时器，
	只是在HAL_GetTick()函数的基础上使用了SysTick的当前计数值SysTick->VAL，
	根据这个数据和SysTick->LOAD重装载值的关系即可计算出不足1毫秒的那一部分时间，
	从而得到准确的以微秒为单位的时间。 */
/* 计时时间移除后会归零 */
uint64_t MICROS_us(void)
{
	// 我们需要记录老的时间值，因为不保证这个函数被调用的期间SysTick的中断不会被触发。
	static uint64_t OldTimeVal;
	uint64_t NewTimeVal;

	// 新的时间值以Tick计数为毫秒部分，以SysTick的计数器值换算为微秒部分，获得精确的时间。
	NewTimeVal = (HAL_GetTick() * 1000) + (SysTick->VAL * 1000 / SysTick->LOAD);

	// 当计算出来的时间值小于上一个时间值的时候，说明在函数计算的期间发生了SysTick中断，此时应该补正时间值。
	if (NewTimeVal < OldTimeVal)
		NewTimeVal += 1000;
	OldTimeVal = NewTimeVal;

	// 返回正确的时间值
	return NewTimeVal;
}
