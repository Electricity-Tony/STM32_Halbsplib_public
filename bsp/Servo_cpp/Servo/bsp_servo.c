/**
 * @file bsp_servo.c
 * @brief servo板级支持包
 * @author Tony_Wang, Asn
 * @version 1.1
 * @date 2020-10-25
 * @copyright 
 * @par 日志:
 *   V1.0 建立伺服（舵机）bsp级驱动
 *	 V1.1 对角度，初始化等进行修正，现可直接使用
 */
#include "bsp_servo.h"
#include "tim.h"   

//Servo计数值设定
#define Period 2000		//暂时没得用

ServoTypeDef Servo_Bottom;
ServoTypeDef Servo_Middle;
ServoTypeDef Servo_Claw;




/**
 * @brief servo角度通过自身结构体数据计数值更新
* @param 标准舵机结构体
 */
void bsp_Servo_Set_Pulse(ServoTypeDef* Servo)
{
	if(Servo->Pulse>=Servo->DOWNlimit_Pulse && Servo->Pulse<=Servo->UPlimit_Pulse)
	{
		__HAL_TIM_SET_COMPARE(&(Servo->htim), Servo->TIM_CHANNEL,Servo->Pulse);
	}
}

/**
* @brief 设定舵机旋转绝对角度值，原点为标准结构够Zero_Pulse
* @param 标准舵机结构体，旋转至绝对角度,单位0.1度
 */
void bsp_Servo_Set_AbsoluteAngle(ServoTypeDef* Servo,int16_t Angle)
{
	float PulsePerAngle=20/(float)(Servo->Rotatable_angle);			//每0.1度的计数值
	Servo->Pulse=Angle*PulsePerAngle*Servo->Direction + Servo->Zero_Pulse;
	bsp_Servo_Set_Pulse(Servo);
	
}

/**
 * @brief 单独servo_Bottom初始化
* @param 
 */
void bsp_Servo_Bottom_Init(void)
{
	Servo_Bottom.htim=htim2;
	Servo_Bottom.TIM_CHANNEL=TIM_CHANNEL_3;
	Servo_Bottom.Rotatable_angle=270;
	Servo_Bottom.Zero_Pulse=140;
	Servo_Bottom.Std_Pulse=140;
	Servo_Bottom.Pulse=Servo_Bottom.Std_Pulse;
	Servo_Bottom.Direction=1;
	Servo_Bottom.UPlimit_Pulse=140;
	Servo_Bottom.DOWNlimit_Pulse=50;
	
	HAL_TIM_PWM_Start(&Servo_Bottom.htim,Servo_Bottom.TIM_CHANNEL);
	bsp_Servo_Set_Pulse(&Servo_Bottom);
}

/**
 * @brief 单独servo_Middle初始化
* @param 
 */
void bsp_Servo_Middle_Init(void)
{
	Servo_Middle.htim=htim2;
	Servo_Middle.TIM_CHANNEL=TIM_CHANNEL_4 ;
	Servo_Middle.Rotatable_angle=270;
	Servo_Middle.Zero_Pulse=100;
	Servo_Middle.Std_Pulse=150;
	Servo_Middle.Pulse=Servo_Middle.Std_Pulse;
	Servo_Middle.Direction=-1;
	Servo_Middle.UPlimit_Pulse=170;
	Servo_Middle.DOWNlimit_Pulse=100;
	
	HAL_TIM_PWM_Start(&Servo_Middle.htim,Servo_Middle.TIM_CHANNEL);
	bsp_Servo_Set_Pulse(&Servo_Middle);
}


/**
 * @brief 单独servo_Claw初始化
* @param 
 */
void bsp_Servo_Claw_Init(void)
{
	Servo_Claw.htim=htim2;
	Servo_Claw.TIM_CHANNEL=TIM_CHANNEL_2;
	Servo_Claw.Rotatable_angle=180;
	Servo_Claw.Zero_Pulse=150;
	Servo_Claw.Std_Pulse=150;
	Servo_Claw.Pulse=Servo_Claw.Std_Pulse;
	Servo_Claw.Direction=1;
	Servo_Claw.UPlimit_Pulse=250;
	Servo_Claw.DOWNlimit_Pulse=50;
	
	HAL_TIM_PWM_Start(&Servo_Claw.htim,Servo_Claw.TIM_CHANNEL);
	bsp_Servo_Set_Pulse(&Servo_Claw);
}

/**
 * @brief 所有servo初始化
* @param 
 */
void bsp_All_Servo_Init(void)
{
	bsp_Servo_Bottom_Init();
	bsp_Servo_Middle_Init();
	bsp_Servo_Claw_Init();
}







