/**
 * @file bsp_StepMotor.cpp
 * @brief StepMotor板级支持包
 * @author Tony_Wang
 * @version 1.2
 * @date 2022-3-11
 * @copyright 
 * @par 日志:
 *   V1.0 基本Cube配置相关说明
 *	 V1.1 将PWM触发方式修改为输出比较方式，可以使同一时钟下不同端口实现不同频率的脉冲
 *	 V1.2 维护两次中断1次脉冲bug，新增Completed_pulse变量用于就算位置
 */		
 
 
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_StepMotor.hpp"
#include "tim.h"
#include "math.h"
/* 私有类型定义 --------------------------------------------------------------*/

#define WEAK __attribute__((weak)) //使用WEAK类型是方便来重构特定函数
#define	ABS(x)   ((x)>0?(x):-(x))

using namespace std;


stepmotor stepmotor1 (StepM1_Dir_GPIO_Port,StepM1_Dir_Pin,&htim2,TIM_CHANNEL_1,1600,200,StepM1_EN_GPIO_Port,StepM1_EN_Pin);
stepmotor stepmotor2 (StepM2_Dir_GPIO_Port,StepM2_Dir_Pin,&htim2,TIM_CHANNEL_2,1600,500,StepM2_EN_GPIO_Port,StepM2_EN_Pin);


/////**************************************	输出比较中断函数	*****************************************************************/////
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	stepmotor1.TIMupdate(htim);
	stepmotor2.TIMupdate(htim);
}



////******************************************* stepmotor电机类 *************************************************************************////
/**
	* @brief  stepmotor电机类
	* @param [in] DIR_Port			方向IO端号
	* @param [in]	DIR_Pin				方向IO通道号
	* @param [in]	htim					使用时钟端号
	* @param [in]	TIM_CHANNEL		使用时钟通道
	* @param [in]	Pulse_Equivalent			脉冲当量
	* @param [in]	CCR_Per			输出比较每次增加的CCR值
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
	HAL_TIM_Base_Start_IT(htim);//使能使用的定时器
	HAL_TIM_OC_Start_IT(htim,TIM_CHANNEL);//使能输出比较的通道
	HAL_TIM_OC_Stop(htim, TIM_CHANNEL);
	setEN(Enable);
	setPulse();
//	setDIR(Positive);
//	start(Disable);
}

/**
  * 函数功能: 设置步进电机转向
  * 输入参数:DIR	Positive/Reverse
  * 返 回 值: 
  * 说    明：
  */
WEAK void stepmotor::setDIR(uint8_t DIR)
{	
	if(DIR) HAL_GPIO_WritePin(DIR_Port,DIR_Pin,GPIO_PIN_SET);
	else HAL_GPIO_WritePin(DIR_Port,DIR_Pin,GPIO_PIN_RESET);
	
	this->DIR = DIR;
}



/**
  * 函数功能: 设置步进电机使能
  * 输入参数: EN	Enable/Disable
  * 返 回 值: 
  * 说    明：
  */
WEAK void stepmotor::setEN(uint8_t EN)
{
	if(EN) HAL_GPIO_WritePin(EN_Port,EN_Pin,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(EN_Port,EN_Pin,GPIO_PIN_SET);
}


/**
  * 函数功能: 开关步进电机pwm输出
  * 输入参数: start Enable/Disable
  * 返 回 值: 
  * 说    明：
  */
WEAK void stepmotor::start(uint8_t start)
{
	//if(start) HAL_TIM_PWM_Start_IT(htim, TIM_CHANNEL);//修改输出比较删去
	if(start) HAL_TIM_OC_Start(htim, TIM_CHANNEL);	//开启输出比较通道
	else HAL_TIM_OC_Stop(htim, TIM_CHANNEL);
	
}




/**
  * 函数功能: 设置步进电机每个脉冲的CCR中断值
  * 输入参数: 
  * 返 回 值: 
  * 说    明：该函数用于放在中断中自动执行，每次中断后更新下一次中断的CCR值
  */
WEAK void stepmotor::setPulse(void)
{
		Pulse =__HAL_TIM_GET_COMPARE(htim,TIM_CHANNEL);	//获取当前通道CCR值值
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL,Pulse+Pulse_Per);
}



/**
  * 函数功能: 设置步进电机向某一方向转特定脉冲数
  * 输入参数: Direction：方向		Pulse：脉冲数
  * 返 回 值: 
  * 说    明：
  */
WEAK void stepmotor::rotate_with_Pulse(uint8_t Direction,uint64_t Pulse)
{
	setDIR(Direction);
	if(Pulse != 0) start(Enable);
	Remaining_pulse = Pulse*2;
}

/**
  * 函数功能: 设置步进电机向某一方向转指定角度
  * 输入参数: Completed_pulse()
  * 返 回 值: 
  * 说    明：
  */
WEAK void stepmotor::set_Completed_pulse(int64_t Completed_pulse)
{
	this->Completed_pulse = Completed_pulse;
}


/**
  * 函数功能: 设置步进电机运行的完成脉冲数
  * 输入参数: (单位：度)
  * 返 回 值: 
  * 说    明：
  */
WEAK void stepmotor::rotate_with_angle(uint8_t Direction,float angle)
{
	rotate_with_Pulse(Direction,abs(angle/360*Pulse_Equivalent));
}

/**
  * 函数功能: 定时器中周期更新剩余脉冲数
  * 输入参数: 
  * 返 回 值: 
  * 说    明：放在PWM定时器循环中
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



