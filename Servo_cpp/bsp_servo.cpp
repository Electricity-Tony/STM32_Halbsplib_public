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
#include "bsp_servo.hpp"
#include "tim.h"   


#define WEAK __attribute__((weak)) //使用WEAK类型是方便来重构特定函数
#define	ABS(x)   ((x)>0?(x):-(x))

using namespace std;



////******************************************* pwm电机类（engine）*************************************************************************////
/**
	* @brief  pwm电机类（engine）
	* @param [in]   htim			使用时钟端号
	* @param [in]	TIM_CHANNEL		使用时钟通道
	* @param [in]	frequency		调整时钟频率，为0时程序内不会进行调整
	*
*/
WEAK void engine::init(TIM_HandleTypeDef htim, uint32_t TIM_CHANNEL, uint16_t frequency)
{
	this->htim = htim;
	this->TIM_CHANNEL = TIM_CHANNEL;
	this->frequency = frequency;


	uint32_t timclockfreq = 0;
	timclockfreq = HAL_RCC_GetHCLKFreq();	//获得当前时钟主频率
	autoreload = __HAL_TIM_GET_AUTORELOAD(&htim);	//获取当前重装载值
	if (frequency != 0)	//表示cube端没有进行更改 且 设置对频率进行了修改
	{
		if (htim.Instance == TIM1)
		{
			timclockfreq = timclockfreq / 2;
		}

		uint32_t timtemp = timclockfreq / frequency;	//分频数与计数值乘机的暂存数
		float fretemp = frequency / 5;				//频率计算暂存数
		uint32_t pretemp = 0;						//分频系数暂存值

		//获取适合的分频系数
		if (fretemp < 1)
		{
			pretemp = timclockfreq / 10000;
		}
		else if (fretemp < 100)
		{
			pretemp = timclockfreq / 100000;
		}
		else
		{
			pretemp = timclockfreq / 1000000;
		}
		__HAL_TIM_SET_PRESCALER(&htim, pretemp - 1);
		autoreload = timtemp / pretemp;

		__HAL_TIM_SET_AUTORELOAD(&htim, autoreload);
	}
	HAL_TIM_PWM_Start(&htim, TIM_CHANNEL);
}


/**
	* @brief  engine设置比较计数值
	* @param [in]   比较值计数值
	* @retval
	* @par 日志
*
*/
WEAK void engine::compare_set(uint16_t compare_value)
{
	compare = compare_value;
	if (compare <= autoreload)
	{
		__HAL_TIM_SET_COMPARE(&htim, TIM_CHANNEL, compare);
	}
}

/**
	* @brief  engine设置比较值函数
	* @param [in]   目标比较值(千分比)
	* @retval
	* @par 日志
*
*/
WEAK void engine::speed_set(uint16_t speed)
{
	this->speed = speed;
	this->compare = (float)this->autoreload * speed / 1000;
	//if (compare <= autoreload)
	//{
	//	__HAL_TIM_SET_COMPARE(&htim, TIM_CHANNEL, this->compare);
	//}
	this->compare_set(compare);

}


////******************************************* Servo电机类*************************************************************************////
/**
	* @brief  Servo电机类
	* @param [in]   htim			使用时钟端号
	* @param [in]	TIM_CHANNEL		使用的时钟通道
	* @param [in]	AngleModel		舵机机械性能上可旋转的角度
	* @param [in]	Zero_Pulse		作为0度的标准角度计数值
	* @param [in]	Std_Pulse		没有命令常规状态的标准角度计数值
	* @param [in]	Direction		正方向及现性放大倍数
	* @param [in]	UPlimit_Pulse		舵机计数值上限
	* @param [in]	DOWNlimit_Pulse		舵机计数值下限
	*
*/
WEAK void servo::init(TIM_HandleTypeDef htim, uint32_t TIM_CHANNEL, 
	uint16_t AngleModel, uint16_t Zero_Pulse, uint16_t Std_Pulse, int8_t Direction,  uint16_t UPlimit_Pulse, uint16_t DOWNlimit_Pulse)
{
	this->htim = htim;
	this->TIM_CHANNEL = TIM_CHANNEL;

	this->AngleModel = AngleModel;
	this->Zero_Pulse = Zero_Pulse;
	this->Std_Pulse = Std_Pulse;
	this->Direction = Direction;
	this->UPlimit_Pulse = UPlimit_Pulse;
	this->DOWNlimit_Pulse = DOWNlimit_Pulse;

	frequency = 50;

	uint32_t timclockfreq = 0;
	timclockfreq = HAL_RCC_GetHCLKFreq();	//获得当前时钟主频率
	autoreload = __HAL_TIM_GET_AUTORELOAD(&htim);	//获取当前重装载值
	if (frequency != 0)	//表示cube端没有进行更改 且 设置对频率进行了修改
	{
		if (htim.Instance == TIM1)
		{
			timclockfreq = timclockfreq / 2;
		}

		uint32_t timtemp = timclockfreq / frequency;	//分频数与计数值乘机的暂存数
		float fretemp = frequency / 5;				//频率计算暂存数
		uint32_t pretemp = 0;						//分频系数暂存值

		//获取适合的分频系数
		if (fretemp < 1)
		{
			pretemp = timclockfreq / 10000;
		}
		else if (fretemp < 100)
		{
			pretemp = timclockfreq / 100000;
		}
		else
		{
			pretemp = timclockfreq / 1000000;
		}
		__HAL_TIM_SET_PRESCALER(&htim, pretemp - 1);
		autoreload = timtemp / pretemp;

		__HAL_TIM_SET_AUTORELOAD(&htim, autoreload);
	}
	HAL_TIM_PWM_Start(&htim, TIM_CHANNEL);
}



 /**
	 * @brief  servo设置计数值
	 * @param [in]   50~250
	 * @retval
	 * @par 日志
 *
 */
WEAK void servo::pulse_set(uint16_t pulse_value)
{
	if (pulse_value >= DOWNlimit_Pulse && pulse_value <= UPlimit_Pulse)	//超过上下限不进行控制
	{
		this->compare_set(pulse_value);
	}
}

/**
	* @brief  servo旋转绝对角度值
	* @param [in]   旋转至绝对角度,单位0.1度
	* @retval
	* @par 日志
*
*/
WEAK void servo::angle_set(int16_t angle_value)
{
	float PulsePerAngle = 20 / (float)(AngleModel);			//每0.1度的计数值
	Pulse = angle_value * PulsePerAngle * Direction + Zero_Pulse;
	this->pulse_set(Pulse);

}



//卸载宏定义
#undef ABS




