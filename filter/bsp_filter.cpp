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
#include "bsp_filter.hpp"

/* *****************************低通滤波器************************************ */
/**
 * @brief  低通滤波器初始化配置
 * @details 时间常数是用于调控 滤波的频率
 * @param  Tf :低通滤波时间常数，单位是 us
 * @retval
 */
LowPassFilter::LowPassFilter(float Tf)
{
    this->Tf = Tf;
}

/**
 * @brief  低通滤波器运行函数
 * @details 输入本次的数值，自动与上一次的数值对比没实现滤波
 * @param  data_in：输入的数值
 * @retval
 */
float LowPassFilter::run(float data_in)
{
    /* 获取两次滤波的时间差 */
    uint32_t time_now = MICROS_us();
    float dt = (time_now - time_prev) * 1e-6f;

    /* 溢出时归零 */
    if (dt < 0.0f)
        dt = 1e-3f;
    /* 两次调用时间过长 取消滤波 */
    else if (dt > 0.3f)
    {
        data_prev = data_in;
        time_prev = time_now;
        data = data_in;
        return data;
    }
    /* 加权前后两次的值，实现低通滤波 */
    float alpha = Tf / (Tf + dt);
    data = alpha * data_prev + (1.0f - alpha) * data_in;
    data_prev = data;
    time_prev = time_now;
    return data;
}
