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
 
 
/* 包含头文件 ----------------------------------------------------------------*/
#include "tim.h"
#include "bsp_StepMotor.h"
/* 私有类型定义 --------------------------------------------------------------*/

uint16_t StepMotor1_SetPulse;
uint16_t StepMotor1_Cnt;



/**
  * 函数功能: 初始化步进电机
  * 输入参数: 
  * 返 回 值: 
  * 说    明：设置步进电机正转，使能，占空比50%，不开启转动
  */
void bsp_StepMotor_Init(void)
{
	StepMotor1_DIR_Forward();
	StepMotor1_ENABLE();
	StepMotot1_SetPulsa(StepMotor1_Period/2);
}


/**
  * 函数功能: 设置步进电机向某一方向转特定脉冲数
  * 输入参数: 
  * 返 回 值: 
  * 说    明：
  */
uint8_t bsp_StepMotor_TurnPulse(uint8_t StepMotor_ID,uint8_t Direction,uint16_t Pulse)
{
	if(Direction==StepMotor_Forward) 
	{
		switch(StepMotor_ID)
		{
			case 0x01:
			{
				StepMotor1_DIR_Forward();
				HAL_TIM_PWM_Start_IT(&StepMotor1_TIM, StepMotor1_CH);
				StepMotor1_SetPulse=Pulse;
				break;
			}
		}
		
	}
	else if(Direction==StepMotor_Reversal) 
	{
		switch(StepMotor_ID)
		{
			case 0x01:
			{
				StepMotor1_DIR_Reversal();
				HAL_TIM_PWM_Start_IT(&StepMotor1_TIM, StepMotor1_CH);
				StepMotor1_SetPulse=Pulse;
				break;
			}
		}
		
	}	
	else {return(StepMotor_Error);}
	
	

}



/**
  * 函数功能: 步进电机PWM中断回调函数
  * 输入参数: 
  * 返 回 值: 
  * 说    明：
  */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{

     if(htim==&htim1) 
    {             
         StepMotor1_Cnt++;
        if(StepMotor1_Cnt == StepMotor1_SetPulse)
       {
         HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_3);
					StepMotor1_SetPulse = 0;
          StepMotor1_Cnt = 0;
        }
    }
}



