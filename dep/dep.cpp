/**
 * @file dep.cpp
 * @brief dep
 * @author Tony_Wang
 * @version 1.1
 * @date 2023-8-31
 * @copyright
 * @par 日志:
 *   V1.0 为保证所有库需要依赖的文件都包含，将所有依赖文件独立成dep库
 *   V1.1 增加角度单位转换
 *
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "dep.hpp"

/* 私有类型定义 --------------------------------------------------------------*/

/**
 * @brief  new 与 delete 重定义
 * @details	放置自带的 new 占用过大，爆flash
 * @param
 * @retval
 */
/* 使用 cpp 特性 new 与 delete 需要调用的代码 */
void *operator new(size_t size)
{
	/* 声明返回的指针 */
	void *res;
	/* 输入是指针强行带一个地址 */
	if (size == 0)
	{
		size = 1;
	}
	/* 拷贝地址 */
	res = malloc(size);
	// 检查是否拷贝完全
	while (1)
	{
		if (res)
		{
			break;
		}
	}
	return res;
}
void operator delete(void *p)
{
	free(p);
}

/**
 * @brief  获取滴答定时器的微秒级定时
 * @details
 * @param
 * @retval
 */
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
	NewTimeVal = (MICROS_ms() * 1000) + (SysTick->VAL * 1000 / SysTick->LOAD);

	// 当计算出来的时间值小于上一个时间值的时候，说明在函数计算的期间发生了SysTick中断，此时应该补正时间值。
	if (NewTimeVal < OldTimeVal)
		NewTimeVal += 1000;
	OldTimeVal = NewTimeVal;

	// 返回正确的时间值
	return NewTimeVal;
}