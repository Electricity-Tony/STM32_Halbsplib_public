
#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "stm32f1xx.h"
#include "main.h"

#define LED_ON GPIO_PIN_RESET
#define LED_OFF GPIO_PIN_SET

typedef struct
{
	GPIO_TypeDef *GPIO_Port;  // LED结构体端口号
	uint16_t Pin;			  // LED结构体引脚号
	uint8_t State;			  // 实时的LED情况
	uint16_t time_flag_start; // 闪烁时间标志位开始
	uint16_t time_flag_now;	  // 闪烁时间标志位当前
	uint16_t time_on;		  // 单词LED亮起的时间ms
	uint16_t check_times;	  // 闪烁次数判断域
	uint16_t on_times;		  // 闪烁次数设置
} _LedTypedef;

// Cpp 调用声明
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	extern _LedTypedef Board_Led;

	void bsp_led_Init(void);
	void bsp_led_Operate(_LedTypedef *Led, uint8_t State);
	void bsp_led_twinkle(_LedTypedef *Led);

// Cpp 调用声明结束
#ifdef __cplusplus
}
#endif // __cplusplus

#endif