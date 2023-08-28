/**
 * @file bsp_StepMotor.cpp
 * @brief StepMotor�弶֧�ְ�
 * @author Tony_Wang
 * @version 1.2
 * @date 2022-3-11
 * @copyright 
 * @par ��־:
 *   V1.0 ����Cube�������˵��
 *	 V1.1 ��PWM������ʽ�޸�Ϊ����ȽϷ�ʽ������ʹͬһʱ���²�ͬ�˿�ʵ�ֲ�ͬƵ�ʵ�����
 *	 V1.2 ά�������ж�1������bug������Completed_pulse�������ھ���λ��
 */		
 
 
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_StepMotor.hpp"
#include "tim.h"
#include "math.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/

#define WEAK __attribute__((weak)) //ʹ��WEAK�����Ƿ������ع��ض�����
#define	ABS(x)   ((x)>0?(x):-(x))

using namespace std;


stepmotor stepmotor1 (StepM1_Dir_GPIO_Port,StepM1_Dir_Pin,&htim2,TIM_CHANNEL_1,1600,200,StepM1_EN_GPIO_Port,StepM1_EN_Pin);
stepmotor stepmotor2 (StepM2_Dir_GPIO_Port,StepM2_Dir_Pin,&htim2,TIM_CHANNEL_2,1600,500,StepM2_EN_GPIO_Port,StepM2_EN_Pin);


/////**************************************	����Ƚ��жϺ���	*****************************************************************/////
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	stepmotor1.TIMupdate(htim);
	stepmotor2.TIMupdate(htim);
}



////******************************************* stepmotor����� *************************************************************************////
/**
	* @brief  stepmotor�����
	* @param [in] DIR_Port			����IO�˺�
	* @param [in]	DIR_Pin				����IOͨ����
	* @param [in]	htim					ʹ��ʱ�Ӷ˺�
	* @param [in]	TIM_CHANNEL		ʹ��ʱ��ͨ��
	* @param [in]	Pulse_Equivalent			���嵱��
	* @param [in]	CCR_Per			����Ƚ�ÿ�����ӵ�CCRֵ
	*
*/
WEAK stepmotor::stepmotor(GPIO_TypeDef* DIR_Port,uint16_t DIR_Pin,
													TIM_HandleTypeDef* htim, uint32_t TIM_CHANNEL,
													uint16_t Pulse_Equivalent,uint16_t Pulse_Per,
													GPIO_TypeDef* EN_Port,uint16_t EN_Pin)
{
	this->DIR_Port = DIR_Port;
	this->DIR_Pin = DIR_Pin;
	this->EN_Port = EN_Port;
	this->EN_Pin = EN_Pin;
	this->Pulse_Equivalent = Pulse_Equivalent;
	this->Pulse_Per = Pulse_Per;
	this->htim = htim;
	this->TIM_CHANNEL = TIM_CHANNEL;
}


WEAK void stepmotor::step_init(void)
{
	HAL_TIM_Base_Start_IT(htim);//ʹ��ʹ�õĶ�ʱ��
	HAL_TIM_OC_Start_IT(htim,TIM_CHANNEL);//ʹ������Ƚϵ�ͨ��
	HAL_TIM_OC_Stop(htim, TIM_CHANNEL);
	setEN(Enable);
	setPulse();
//	setDIR(Positive);
//	start(Disable);
}

/**
  * ��������: ���ò������ת��
  * �������:DIR	Positive/Reverse
  * �� �� ֵ: 
  * ˵    ����
  */
WEAK void stepmotor::setDIR(uint8_t DIR)
{	
	if(DIR) HAL_GPIO_WritePin(DIR_Port,DIR_Pin,GPIO_PIN_SET);
	else HAL_GPIO_WritePin(DIR_Port,DIR_Pin,GPIO_PIN_RESET);
	
	this->DIR = DIR;
}



/**
  * ��������: ���ò������ʹ��
  * �������: EN	Enable/Disable
  * �� �� ֵ: 
  * ˵    ����
  */
WEAK void stepmotor::setEN(uint8_t EN)
{
	if(EN) HAL_GPIO_WritePin(EN_Port,EN_Pin,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(EN_Port,EN_Pin,GPIO_PIN_SET);
}


/**
  * ��������: ���ز������pwm���
  * �������: start Enable/Disable
  * �� �� ֵ: 
  * ˵    ����
  */
WEAK void stepmotor::start(uint8_t start)
{
	//if(start) HAL_TIM_PWM_Start_IT(htim, TIM_CHANNEL);//�޸�����Ƚ�ɾȥ
	if(start) HAL_TIM_OC_Start(htim, TIM_CHANNEL);	//��������Ƚ�ͨ��
	else HAL_TIM_OC_Stop(htim, TIM_CHANNEL);
	
}




/**
  * ��������: ���ò������ÿ�������CCR�ж�ֵ
  * �������: 
  * �� �� ֵ: 
  * ˵    �����ú������ڷ����ж����Զ�ִ�У�ÿ���жϺ������һ���жϵ�CCRֵ
  */
WEAK void stepmotor::setPulse(void)
{
		Pulse =__HAL_TIM_GET_COMPARE(htim,TIM_CHANNEL);	//��ȡ��ǰͨ��CCRֵֵ
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL,Pulse+Pulse_Per);
}



/**
  * ��������: ���ò��������ĳһ����ת�ض�������
  * �������: Direction������		Pulse��������
  * �� �� ֵ: 
  * ˵    ����
  */
WEAK void stepmotor::rotate_with_Pulse(uint8_t Direction,uint64_t Pulse)
{
	setDIR(Direction);
	if(Pulse != 0) start(Enable);
	Remaining_pulse = Pulse*2;
}

/**
  * ��������: ���ò��������ĳһ����תָ���Ƕ�
  * �������: Completed_pulse()
  * �� �� ֵ: 
  * ˵    ����
  */
WEAK void stepmotor::set_Completed_pulse(int64_t Completed_pulse)
{
	this->Completed_pulse = Completed_pulse;
}


/**
  * ��������: ���ò���������е����������
  * �������: (��λ����)
  * �� �� ֵ: 
  * ˵    ����
  */
WEAK void stepmotor::rotate_with_angle(uint8_t Direction,float angle)
{
	rotate_with_Pulse(Direction,abs(angle/360*Pulse_Equivalent));
}

/**
  * ��������: ��ʱ�������ڸ���ʣ��������
  * �������: 
  * �� �� ֵ: 
  * ˵    ��������PWM��ʱ��ѭ����
  */
WEAK void stepmotor::TIMupdate(TIM_HandleTypeDef *htim)
{
	static uint8_t Trigger_flag=0;
	if(this->htim==htim)
	{
		switch (this->TIM_CHANNEL)
		{
		case TIM_CHANNEL_1:
			if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			Trigger_flag = 1;
			break;
		case TIM_CHANNEL_2:
			if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
			Trigger_flag = 1;
			break;
		case TIM_CHANNEL_3:
			if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
			Trigger_flag = 1;
			break;
		case TIM_CHANNEL_4:
			if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
			Trigger_flag = 1;
			break;
		default:
			break;
		}
		if(Trigger_flag == 1)
		{
			if(this->Remaining_pulse != 0)
			{
				this->Remaining_pulse--;
				if(DIR == Positive) Completed_pulse++;
				else Completed_pulse--;
			}
			if(this->Remaining_pulse == 0)
			{
					start(Disable);
			}
			setPulse();
			Trigger_flag = 0;
		}
	}
}



