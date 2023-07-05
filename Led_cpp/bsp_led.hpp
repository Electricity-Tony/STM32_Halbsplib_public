/**
 * @file bsp_led.cpp
 * @brief led板级支持包
 * @author Tony_Wang
 * @version 1.1
 * @date 2023-6-19
 * @copyright
 * @par 日志:
 *   V1.0 基本Cube配置与基本设置,使用C结构体编写
 *	 V1.1 重构为C++库，使用系统时钟调用闪烁
 *
 */

#ifndef __BSP_LED_HPP
#define __BSP_LED_HPP
#include "stm32f1xx.h"
#include "main.h"

typedef enum
{
	LED_ON = GPIO_PIN_RESET,
	LED_OFF = GPIO_PIN_SET

} LED_State;

/*****			GPIO_OUT led 构造函数				******/
class Led
{
public:
	GPIO_TypeDef *GPIO_Port; // 按键结构体端口号
	uint16_t Pin;			 // 按键结构体引脚号

	LED_State State; // 实时当时的按键按下情况

	// 成员函数
	Led(void){};
	Led(GPIO_TypeDef *GPIO_Port, uint16_t Pin);
	void twinkle_Config(uint16_t twinkle_time, uint16_t twinkle_times);
	void ctrl(LED_State State);
	void twinkle_update();

protected:
	uint16_t time_flag_start; // 闪烁时间标志位开始
	uint16_t time_flag_now;	  // 闪烁时间标志位当前
	uint16_t twinkle_time;	  // 单词LED亮起的时间ms
	uint16_t twinkle_count;	  // 闪烁次数判断域
	uint16_t twinkle_times;	  // 闪烁次数设置
};

extern Led BORAD_LED;

#endif