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

#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "stm32f1xx.h"

/* 获取 ms 级别的定时时间 */
#define MICROS_ms() HAL_GetTick() // 计时，单位ms,除1000是秒
	/* 获取 us 级别的定时时间 */
	uint64_t MICROS_us(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif