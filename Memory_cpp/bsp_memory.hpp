/**
 * @file bsp_memory.cpp
 * @brief memory板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2022-4-7
 * @copyright 
 * @par 日志:
 *   V1.0 基本配置相关说明
 */		
 


#ifndef __BSP_MEMORY_HPP__
#define __BSP_MEMORY_HPP__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "main.h"
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/



//stepmotor表示为步进电机
class queue
{
public:
	uint16_t maxsize;
	int16_t* data;
	
		//公共函数
	queue(void){};
	queue(int16_t data_link[],uint16_t maxsize);
		
	uint8_t check_empty(void);
	uint8_t check_full(void);
	uint8_t	push(int16_t element);
	uint8_t pop(int16_t* out_element);
		
	//运算储存区
protected:
	uint16_t front;
	uint16_t rear;
	uint16_t counter;
	
private:
	
};


extern queue motor_speed_FIFO;
extern queue R_ADC_FIFO;

#endif 
