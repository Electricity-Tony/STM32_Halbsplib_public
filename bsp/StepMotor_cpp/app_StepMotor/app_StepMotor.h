/**
 * @file app_StepMotor.c
 * @brief StepMotor板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2022-3-11
 * @copyright 
 * @par 日志:
 *   V1.0 基于bsp_StepMotor开发的应用层
 *	 V1.1 
 *	 V1.2 
 */		



#ifndef __APP_MOTOR_H
#define __APP_MOTOR_H
//#include "stm32f4xx.h"
#include "stm32f1xx.h"
#include "bsp_StepMotor.hpp"
#include "bsp_key.hpp"

typedef struct
{
	float x;
	float y;
}Position_Typedef;


void app_StepMotor_position_convert(void);
void app_StepMotor_init(void);
uint8_t app_StepMotor_reset(stepmotor* stepmotor_in,uint8_t DIR,Key* Key_in);
void app_StepMotor_allreset(void);
void app_StepMotor_rotate_with_Length(stepmotor* stepmotor_in,float Gear_ratio,float Length);
void app_StepMotor_move_with_position(float Target_x,float Target_y);
void app_StepMotor_button_Ctrl_position(void);


extern Position_Typedef position;

#endif
