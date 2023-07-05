/**
 * @file bsp_StepMotor.c
 * @brief StepMotor�弶֧�ְ�
 * @author Tony_Wang
 * @version 1.0
 * @date 2020-10-14
 * @copyright 
 * @par ��־:
 *   V1.0 ����Cube�������˵��
 *	 
 */		
 
 
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "tim.h"
#include "bsp_StepMotor.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/

uint16_t StepMotor1_SetPulse;
uint16_t StepMotor1_Cnt;



/**
  * ��������: ��ʼ���������
  * �������: 
  * �� �� ֵ: 
  * ˵    �������ò��������ת��ʹ�ܣ�ռ�ձ�50%��������ת��
  */
void bsp_StepMotor_Init(void)
{
	StepMotor1_DIR_Forward();
	StepMotor1_ENABLE();
	StepMotot1_SetPulsa(StepMotor1_Period/2);
}


/**
  * ��������: ���ò��������ĳһ����ת�ض�������
  * �������: 
  * �� �� ֵ: 
  * ˵    ����
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
  * ��������: �������PWM�жϻص�����
  * �������: 
  * �� �� ֵ: 
  * ˵    ����
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



