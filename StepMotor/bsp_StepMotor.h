/**
 * @file bsp_StepMotor.c
 * @brief StepMotor板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2020-10-14
 * @copyright 
 * @par 日志:
 *   V1.0 基本Cube配置相关说明
 *	 
 */		



#ifndef __BSP_StepMotor_H__
#define __BSP_StepMotor_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/


#define StepMotor_Forward 1
#define StepMotor_Reversal 0
#define StepMotor_Error 1

#define StepMotot1_SetPulsa(x) __HAL_TIM_SET_COMPARE(&StepMotor1_TIM,StepMotor1_CH,x)  

#define StepMotor1_DIR_Forward() HAL_GPIO_WritePin(StepMotor1_DIR_GPIO_Port,StepMotor1_DIR_Pin,GPIO_PIN_SET);
#define StepMotor1_DIR_Reversal() HAL_GPIO_WritePin(StepMotor1_DIR_GPIO_Port,StepMotor1_DIR_Pin,GPIO_PIN_RESET);

#define StepMotor1_ENABLE() HAL_GPIO_WritePin(StepMotor1_DIR_GPIO_Port,StepMotor1_DIR_Pin,GPIO_PIN_RESET);
#define StepMotor1_DISABLE() HAL_GPIO_WritePin(StepMotor1_EN_GPIO_Port,StepMotor1_DIR_Pin,GPIO_PIN_SET);

//自定义宏定义
#define StepMotor1_ID 0x01						//步进电机1的ID号
#define StepMotor1_TIM htim1					//步进电机1的时钟号
#define StepMotor1_CH TIM_CHANNEL_3		//步进电机1的时钟通道
#define StepMotor1_Period 1000				//步进电机1的每周期计数值








void bsp_StepMotor_Init(void);
uint8_t bsp_StepMotor_TurnPulse(uint8_t StepMotor_ID,uint8_t Direction,uint16_t Pulse);

#endif 
