/**
 * @file bsp_servo.h
 * @author Tony_Wang， Asn
 * @version 1.1
 * @date 2020-10-25
 * @copyright
 */
#ifndef __BSP_SERVO_HPP
#define __BSP_SERVO_HPP
#include "stm32f1xx.h"



using namespace std;

#define Positive 1
#define negative -1


//engine特指使用pwm控制设备，例如空心杯电机
class engine
{
public:
	TIM_HandleTypeDef htim;		//使用的时钟
	uint32_t TIM_CHANNEL;			//使用的时钟通道计数值
	uint8_t speed;			//作为电机的速度（百分比）

	uint16_t frequency;//时钟设置的频率，单位Hz,默认50Hz
	uint16_t autoreload;	//此pwm的重装载值

		//公共函数
	void init(TIM_HandleTypeDef htim, uint32_t TIM_CHANNEL, uint16_t frequency = 0);//构造pwm电机，设置频率为50Hz，为0时不进行修改
	void compare_set(uint16_t compare_value);
	void speed_set(uint16_t speed);

	//运算储存区
private:
	uint16_t compare;		//pwm比较计数值
};





//servo特指50Hz pwm控制的舵机
class servo:public engine
{
public:
	uint16_t AngleModel;		//舵机机械性能上可旋转的角度
	uint16_t Zero_Pulse;			//作为0度的标准角度计数值
	uint16_t Std_Pulse;			//没有命令常规状态的标准角度计数值
	uint16_t Pulse;					//控制舵机的计数值
	uint16_t Angle;					//控制舵机的角度值
	int8_t Direction;			//正方向及现性放大倍数
	uint16_t UPlimit_Pulse;				//舵机计数值上限
	uint16_t DOWNlimit_Pulse;		//舵机计数值下限


		//公共函数
	void init(TIM_HandleTypeDef htim, uint32_t TIM_CHANNEL,
		uint16_t AngleModel, uint16_t Zero_Pulse, uint16_t Std_Pulse, int8_t Direction = Positive, uint16_t UPlimit_Pulse = 50, uint16_t DOWNlimit_Pulse = 250);
	void pulse_set(uint16_t pulse_value);
	void angle_set(int16_t angle_value);
	//运算储存区
private:

};




#endif
