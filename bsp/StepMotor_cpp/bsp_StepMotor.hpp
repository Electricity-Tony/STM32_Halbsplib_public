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



#ifndef __BSP_StepMotor_HPP__
#define __BSP_StepMotor_HPP__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "main.h"
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/

#define Positive 1   //设置步进电机正转
#define Reverse 0  //设置步进电机反转
#define Enable 1   //设置步进电使能，开关
#define Disable 0  //设置步进电机失能，开关


//stepmotor表示为步进电机
class stepmotor
{
public:
  GPIO_TypeDef* DIR_Port;	    //方向控制io结构体端口号
	uint16_t DIR_Pin;		    //方向控制io结构体引脚号

	TIM_HandleTypeDef* htim;		//使用的时钟
	uint32_t TIM_CHANNEL;		//使用的时钟通道计数值
	uint16_t Pulse_Per;			//输出比较每次增加的CCR值

  GPIO_TypeDef* EN_Port;	    //使能io结构体端口号
	uint16_t EN_Pin;		    //使能io结构体引脚号

	uint16_t Pulse_Equivalent;	//脉冲当量：多少个脉冲电机转动一周

	int64_t Completed_pulse;		//完成的(半)脉冲数，app层位置确定,注意此处两个值代表一个脉冲
	
		//公共函数
	stepmotor(void){};
	//stepmotor(GPIO_TypeDef* DIR_Port,uint16_t DIR_Pin,TIM_HandleTypeDef* htim,uint32_t TIM_CHANNEL, uint16_t Period);//构造stepmotor
	stepmotor(GPIO_TypeDef* DIR_Port,uint16_t DIR_Pin,
						TIM_HandleTypeDef* htim, uint32_t TIM_CHANNEL,
						uint16_t Pulse_Equivalent,	uint16_t Pulse_Per,
						GPIO_TypeDef* EN_Port = NULL,uint16_t EN_Pin = NULL);//含使能构造stepmotor
	void step_init(void);
	void setDIR(uint8_t DIR);														//步进方向设置
	void setEN(uint8_t EN);															//步进使能设置
	void start(uint8_t start);													//步进时钟使能设置
	void setPulse(void);														//步进脉冲占空比设置
	
	void set_Completed_pulse(int64_t Completed_pulse);//完成脉冲占空比设置
	
	void rotate_with_Pulse(uint8_t Direction,uint64_t Pulse);		//步进目标方向转动脉冲数设置
	void rotate_with_angle(uint8_t Direction,float angle);
	void TIMupdate(TIM_HandleTypeDef *now_htim);
		
	//运算储存区
protected:
	uint16_t Pulse;			//通道CCR值
	uint8_t DIR;	//当前的方向
	uint64_t Remaining_pulse;		//剩余需要执行的脉冲数
	
	
	
private:
	
};


///*****			轴类步进电机构造函数				******/
////需配合key.cpp调用光电门判断初始位置
//class stepaxis:public stepmotor
//{
//public:
//		float Pulse_equivalent;	//脉冲当量，转一度所需的脉冲数
//		float Absolute_angle;		//转轴的绝对角度
//		Key limit_key;					//判断初始化的开关
//			//公共函数
//		
//		stepaxis(void){};
//		stepaxis(GPIO_TypeDef* DIR_Port,uint16_t DIR_Pin,
//							TIM_HandleTypeDef* htim, uint32_t TIM_CHANNEL,
//							float Pulse_equivalent,Key limit_key,
//							GPIO_TypeDef* EN_Port = NULL,uint16_t EN_Pin = NULL);
//		uint8_t axis_init(uint8_t Direction);
//		void setorigin(float ab_angle);		//当前绝对角度设定
//		void setangle(float angle);				//目标绝对角度设定
//		void axis_TIMupdate(TIM_HandleTypeDef *now_htim);
//	//运算储存区
//private:
//};

//extern stepaxis axis1;
//extern stepaxis axis3;
//extern stepaxis axis4;
//extern stepaxis axis5;
//extern stepaxis axis6;
//**********************	外部声明变量	***************************//
extern stepmotor stepmotor1 ;
extern stepmotor stepmotor2 ;
#endif 
