/**
 * @file bsp_adc.c
 * @brief 基本串口支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2022-3-21
 * @copyright 
 * @par 日志:
 *   V1.0 基本Cube配置DMA配置
 *	 V1.1 增加测不准修改的配置文件
 */

 
#ifndef __BSP_ADC_H
#define __BSP_ADC_H			  	 

#include "stm32f1xx.h"

#include "main.h"

//设置重定向adc通道
#define fadc &hadc1               //重定向至adc1 

//adc关键数据设置
enum  __adc_date
{
	Sampling_number = 2,			//采样通道数量
	Sampling_times = 12,	//每个通道采样次数
	
};	

extern uint32_t bsp_adc_date[Sampling_number];

void bsp_adc_init(void);
void bsp_adc_date_clear(uint32_t adc_date[]);
void bsp_adc_date_update(uint32_t adc_date[]);

#endif  
	 



