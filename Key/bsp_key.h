
#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "stm32f1xx.h"
#include "main.h"

typedef struct {
	GPIO_TypeDef* GPIO_Port;	//按键结构体端口号
	uint16_t Pin;				//按键结构体引脚号
	uint8_t State				//实际需要调用的状态
	uint8_t Ture_State;			//按键表示的状况标致,用于长按时间段内标致判断
	uint8_t Now_State;			//实时当时的按键按下情况
	uint16_t time_flag;			//长按判断标志位
	uint16_t time_check			//长按时间判断域
}_KeyTypedef;



extern _KeyTypedef key_up;
extern _KeyTypedef key_down;


void bsp_key_Read(_KeyTypedef* Key);
void bsp_key_State_Update(_KeyTypedef* Key);





#endif
