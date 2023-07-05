/**
 * @file app_StepMotor.c
 * @brief StepMotor�弶֧�ְ�
 * @author Tony_Wang
 * @version 1.0
 * @date 2022-3-11
 * @copyright 
 * @par ��־:
 *   V1.0 ����bsp_StepMotor������Ӧ�ò�
 *	 V1.1 
 *	 V1.2 
 */		



#define TORADIAN   0.0174533f                //ת��Ϊ�����ƣ���Ԫ������̬������Ҫʹ�� ��/180

#include "app_StepMotor.h"
#include "bsp_StepMotor.hpp"
#include "bsp_key.hpp"
#include "math.h"



#define Gear_ratio_1 0.0888889	//	80mm��Ӧ�������1��ת360�ȣ�������ü��ٱ�2.5
#define Gear_ratio_2 0.2222222			//	80mm��Ӧ�������1��ת360��

#define x_max 760
#define x_min 0
#define y_max 0
#define y_min -500

Position_Typedef position;

/** 
	* @brief  ����������������ת��Ϊ����
	* @warning ����ͨ��
	* @retval  void
	* @note void
	* @par ��־   
	*  
	*
*/
void app_StepMotor_position_convert(void)
{
	position.y = float(stepmotor1.Completed_pulse)/2/stepmotor1.Pulse_Equivalent*Gear_ratio_1*360;
	position.x = float(stepmotor2.Completed_pulse)/2/stepmotor2.Pulse_Equivalent*Gear_ratio_2*360;
	
}




/** 
	* @brief  ���������ʼ��
	* @warning ����ͨ��
	* @retval  void
	* @note ���в��������Ҫ�ĳ�ʼ��
	* @par ��־   
	*  
	*
*/
void app_StepMotor_init(void)
{
	stepmotor1.step_init();
	stepmotor2.step_init();
}


/** 
	* @brief  �����������λ�ø�λ
	* @warning ����ͨ��
	* @retval  ��������࣬��ת���򣬹����
	* @note ͨ������ŶԲ����������λ�ø�λ
	* @par ��־   
	*  
	*
*/
uint8_t app_StepMotor_reset(stepmotor* stepmotor_in,uint8_t DIR,Key* Key_in)
{
	Key_in->read();
	if(Key_in->State == 1) 
	{
		stepmotor_in->rotate_with_Pulse(DIR,10);
		return 1;	//�����δ������
	}
	else return 0;									//����Ŵ���
	
}

/** 
	* @brief  ���Բ������λ�ø�λ
	* @warning ����ͨ��
	* @retval  void
	* @note �����в��������λ����λ����ǰΪ����״̬
	* @par ��־   
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
	* @brief  ���Ʋ����������ͬ�����ƶ���Ծ���
	* @warning ����ͨ��
	* @retval  ��������࣬ͬ����������,�ƶ����루mm����������
	* @note 
	* @par ��־   
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
	* @brief  ���Ʋ����������ͬ�����ƶ� ������λ��
	* @warning ����ͨ��
	* @retval  Target_x,Target_y
	* @note 
	* @par ��־   
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
	* @brief  ��������ͬ�����ƶ�
	* @warning ����ͨ��
	* @retval  ��������࣬ͬ����������,�ƶ����루mm��
	* @note 
	* @par ��־   
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

