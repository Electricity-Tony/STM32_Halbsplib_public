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

#ifndef __DEP_HPP__
#define __DEP_HPP__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx.h"
#include <string.h>
#include <math.h>
/* 类型定义 ------------------------------------------------------------------*/
/* 这里放一下如果调用c文件时需要放进去的编译声明 */
// #ifdef __cplusplus
// extern "C"
// {
// #endif // __cplusplus
// #ifdef __cplusplus
// }
// #endif // __cplusplus

/* 宏定义 --------------------------------------------------------------------*/
#define WEAK __attribute__((weak)) // 使用WEAK类型是方便重构特定函数

#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt))) // 限幅函数
// 宏定义实现的一个约束函数,用于限制一个值的范围。
// 具体来说，该宏定义的名称为 _constrain，接受三个参数 amt、low 和 high，分别表示要限制的值、最小值和最大值。该宏定义的实现使用了三元运算符，根据 amt 是否小于 low 或大于 high，返回其中的最大或最小值，或者返回原值。
// 换句话说，如果 amt 小于 low，则返回 low；如果 amt 大于 high，则返回 high；否则返回 amt。这样，_constrain(amt, low, high) 就会将 amt 约束在 [low, high] 的范围内。

#define PI 3.1415926535f // 调用PI的计算

#define MICROS_ms() HAL_GetTick() // 计时，单位ms,除1000是秒

#define ABS(x) ((x) > 0 ? (x) : -(x)) // 快速取绝对值

/* 角度单位转换 */
#define Rad2Rot(rad) ((rad) / PI * 30)      // 弧度(rad/s) 转 转速(rpm)
#define Rad2Angle(rad) ((rad) / PI * 180)   // 弧度(rad) 转 角度(°)
#define Angle2Rad(angle) ((angle)*PI / 180) // 角度(°) 转 弧度(rad)
#define Angle2Rot(angle) ((angle) / 6)      // 角度(°/s) 转 转速(rpm)
#define Rot2Rad(rot) ((rot)*PI / 30)        // 转速(rpm) 转 弧度(rad/s)
#define Rot2Angle(rot) ((rot)*6)            // 转速(rpm) 转 角度(°/s)

/* 函数定义 ---------------------------------------------------------------------------------- */
/* new 与 delete 重定义 */
void *operator new(size_t size);
void operator delete(void *p);

/* 利用滴答定时器获取微秒计时 */
uint64_t MICROS_us(void);

#endif
