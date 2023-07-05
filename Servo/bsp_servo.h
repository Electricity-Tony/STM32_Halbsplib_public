/**
 * @file bsp_servo.h
 * @author Tony_Wang， Asn
 * @version 1.1
 * @date 2020-10-25
 * @copyright 
 */
#ifndef __BSP_SERVO_H
#define __BSP_SERVO_H
#include "stm32f1xx.h"
   


typedef struct{
	TIM_HandleTypeDef htim;		//使用的时钟
	
	uint32_t TIM_CHANNEL;			//使用的时钟通道计数值
	
	uint16_t Rotatable_angle;	//舵机可旋转的角度
	
	uint16_t Zero_Pulse;			//作为0度的标准角度计数值
	
	uint16_t Std_Pulse;			//没有命令常规状态的标准角度计数值
	
	uint16_t Pulse;					//控制舵机的角度计数值
	
	int8_t Direction;			//正方向及现性放大倍数
	
	uint16_t UPlimit_Pulse;				//舵机计数值上限
	
	uint16_t DOWNlimit_Pulse;		//舵机计数值下限
	
}ServoTypeDef;


extern ServoTypeDef Servo_Bottom;
extern ServoTypeDef Servo_Middle;
extern ServoTypeDef Servo_Claw;

//void bsp_Servo_Bottom_Init(void);
//void bsp_Servo_Middle_Init(void);
//void bsp_Servo_Claw_Init(void);

void bsp_All_Servo_Init(void);
void bsp_Servo_Set_AbsoluteAngle(ServoTypeDef* Servo,int16_t Angle);
void bsp_Servo_Set_Pulse(ServoTypeDef* Servo);


#endif
