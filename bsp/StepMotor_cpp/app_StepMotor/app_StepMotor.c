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



#define TORADIAN   0.0174533f                //转换为弧度制，四元数的姿态解算需要使用 π/180

#include "app_StepMotor.h"
#include "bsp_StepMotor.hpp"
#include "bsp_key.hpp"
#include "math.h"



#define Gear_ratio_1 0.0888889	//	80mm对应步进电机1旋转360度，另加外置减速比2.5
#define Gear_ratio_2 0.2222222			//	80mm对应步进电机1旋转360度

#define x_max 760
#define x_min 0
#define y_max 0
#define y_min -500

Position_Typedef position;

/** 
	* @brief  步进电机完成脉冲数转化为坐标
	* @warning 必须通过
	* @retval  void
	* @note void
	* @par 日志   
	*  
	*
*/
void app_StepMotor_position_convert(void)
{
	position.y = float(stepmotor1.Completed_pulse)/2/stepmotor1.Pulse_Equivalent*Gear_ratio_1*360;
	position.x = float(stepmotor2.Completed_pulse)/2/stepmotor2.Pulse_Equivalent*Gear_ratio_2*360;
	
}




/** 
	* @brief  步进电机初始化
	* @warning 必须通过
	* @retval  void
	* @note 进行步进电机需要的初始化
	* @par 日志   
	*  
	*
*/
void app_StepMotor_init(void)
{
	stepmotor1.step_init();
	stepmotor2.step_init();
}


/** 
	* @brief  单个步进电机位置复位
	* @warning 必须通过
	* @retval  步进电机类，旋转方向，光电门
	* @note 通过光电门对步进电机进行位置复位
	* @par 日志   
	*  
	*
*/
uint8_t app_StepMotor_reset(stepmotor* stepmotor_in,uint8_t DIR,Key* Key_in)
{
	Key_in->read();
	if(Key_in->State == 1) 
	{
		stepmotor_in->rotate_with_Pulse(DIR,10);
		return 1;	//光电门未被触发
	}
	else return 0;									//光电门触发
	
}

/** 
	* @brief  所以步进电机位置复位
	* @warning 必须通过
	* @retval  void
	* @note 对所有步进电机复位，复位结束前为阻塞状态
	* @par 日志   
	*  
	*
*/
void app_StepMotor_allreset(void)
{
	uint8_t all_reset_flag=1;
	while(all_reset_flag)
	{
		all_reset_flag = 0;
		all_reset_flag += app_StepMotor_reset(&stepmotor1,Positive,&photogate3);
		all_reset_flag += app_StepMotor_reset(&stepmotor2,Reverse,&photogate4);
	}
	stepmotor1.set_Completed_pulse(0);
	stepmotor2.set_Completed_pulse(0);
	app_StepMotor_position_convert();
}

/** 
	* @brief  控制步进电机带动同步带移动相对距离
	* @warning 必须通过
	* @retval  步进电机类，同步带传动比,移动距离（mm，含正负）
	* @note 
	* @par 日志   
	*  
	*
*/
void app_StepMotor_rotate_with_Length(stepmotor* stepmotor_in,float Gear_ratio,float Length)
{
	float rotate_angle = Length/Gear_ratio;
	if(fabs(rotate_angle)>0.5)
	{
		if(rotate_angle > 0)
		{
			stepmotor_in->rotate_with_angle(Positive,rotate_angle);
		}
		else
		{
			stepmotor_in->rotate_with_angle(Reverse,fabs(rotate_angle));
		}
	}
	app_StepMotor_position_convert();
}



/** 
	* @brief  控制步进电机带动同步带移动 至绝对位置
	* @warning 必须通过
	* @retval  Target_x,Target_y
	* @note 
	* @par 日志   
	*  
	*
*/
void app_StepMotor_move_with_position(float Target_x,float Target_y)
{
	if(Target_x > x_max) Target_x = x_max;
	else if(Target_x < x_min) Target_x = x_min;
	if(Target_y > y_max) Target_y = y_max;
	else if(Target_y < y_min) Target_y = y_min;
	
	app_StepMotor_position_convert();
	float err_x = Target_x - position.x
		,err_y = Target_y-position.y;
	if(fabs(err_x) > 0.1) 
	{app_StepMotor_rotate_with_Length(&stepmotor2,Gear_ratio_2,err_x);
	}
	if(fabs(err_y) > 0.1)
	{app_StepMotor_rotate_with_Length(&stepmotor1,Gear_ratio_1,err_y);
	}
}




/** 
	* @brief  按键控制同步带移动
	* @warning 必须通过
	* @retval  步进电机类，同步带传动比,移动距离（mm）
	* @note 
	* @par 日志   
	*  
	*
*/
void app_StepMotor_button_Ctrl_position(void)
{
	KEY_UP.read();
	KEY_DOWN.read();
	KEY_LEFT.read();
	KEY_RIGHT.read();
	
	int8_t UP_DOWN_flag=0,LEFT_RIGHT_flag=0;
	
	if(KEY_UP.State != KEY_DOWN.State)
	{
		if(KEY_UP.State == 0)	UP_DOWN_flag=1;
		else UP_DOWN_flag = -1;
	}
	if(KEY_LEFT.State != KEY_RIGHT.State)
	{
		if(KEY_LEFT.State == 0)	LEFT_RIGHT_flag=1;
		else LEFT_RIGHT_flag = -1;
	}
	app_StepMotor_move_with_position(position.x+LEFT_RIGHT_flag , position.y+UP_DOWN_flag);
}

