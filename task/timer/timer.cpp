/**
 * @file timer.cpp
 * @brief timer
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-8-28
 * @copyright
 * @par 日志:
 *   V1.0 建立定时器调用库，避免每次都要翻阅资料
 *
 */

#include "timer.hpp"

/* 添加的功能函数头文件 */
#include "foc/bsp_foc.hpp"

void timer_init(void)
{
    /* 这里放需要启动定时器中断的定时器 */
    HAL_TIM_Base_Start_IT(&htim1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* 依次放进入中断的定时器 */
    if (htim == (&htim1))
    {
        /* 依次放该定时器下不同功能的触发间隔变量 */
        static uint8_t foc_run_flag = 0;

        /* 以上每个触发间隔变量 ++  */
        foc_run_flag += 1;

        /* 编写触发功能函数 */
        if (foc_run_flag == 10)
        {
            HallEncoder.get_speed();
            // HallEncoder.get_date();
            // motor_1612.run_speed_Openloop(6.28);
            // motor_1612.run_angle(180);
            motor_1612.run_speed(6.28);
            
            /* 一定要添加间隔变量复位！！ */
            foc_run_flag = 0;
        }
    }
}
