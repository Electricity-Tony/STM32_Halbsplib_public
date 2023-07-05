/**
 * @file bsp_Encoder.c
 * @brief Encoder板级支持包
 * @author Tony_Wang
 * @version 1.1
 * @date 2020-10-10
 * @copyright 
 * @par 日志:
 *   V1.0 基本Cube配置与数值获取
 *	 V1.1 结构化编码器结构体文件，扩展Cube计数范围不足的情况，简化数据获取方式
 */		



#ifndef __BSP_Encoder_H__
#define __BSP_Encoder_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/

#define Counter_Period 0xFFFF		//计数最大值（临界值）
#define Dir_range_check 5000    //设置过临界点使能正反转有效的范围值

#define Enable 1
#define Disable 0
                             
//编码器使用定时器通道
//#define bsp_Use_Encoder_TIM1 htim3
#define bsp_Use_Encoder_TIM2 htim4

typedef struct
{
	TIM_HandleTypeDef Use_TIM;	//该结构体对应使用的时钟
	uint8_t Forward_Check;	//正转过临界标志位
	uint8_t Reverse_Check;	//反转过临界标志位
	int16_t Turn;						//过临界周转圈数
	uint16_t CaptureNumber;	//当前临界内计数值
	int32_t Encoder_date;		//总计数值，作为全局定位编码器值
}bsp_Encoder;

#ifdef bsp_Use_Encoder_TIM1
extern bsp_Encoder Encoder1;
#endif // bsp_Use_Encoder_TIM1

#ifdef bsp_Use_Encoder_TIM2
extern bsp_Encoder Encoder2;
#endif // bsp_Use_Encoder_TIM2

void bsp_Encoder_Init(void);
bsp_Encoder bsp_Encoder_Getdate(bsp_Encoder Encoder);
void bsp_Encoder_Update(void);

#endif 
