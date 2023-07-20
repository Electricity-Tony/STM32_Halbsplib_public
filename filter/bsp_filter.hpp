/**
 * @file bsp_filter.c
 * @brief 滤波器支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-7-11
 * @copyright
 * @par 日志:
 *   V1.0 建立基本的滤波器库
 *
 */
#ifndef __BSP_FILTER_HPP
#define __BSP_FILTER_HPP

#include "systick/bsp_systick.h"

/* 低通滤波器 类 */
class LowPassFilter
{
public:
    float Tf;   // 低通滤波时间常数，单位是 us
    float data; // 滤波后的结果

    /* 成员函数 */
    LowPassFilter(float Tf); // 输入时间常数初始化
    ~LowPassFilter() = default;
    float run(float data_in); // 滤波运行函数

private:
    /* data */
protected:
    float data_prev;    // 上一次滤波后的结果
    uint32_t time_prev; // 上一次滤波后的结果
};

#endif
