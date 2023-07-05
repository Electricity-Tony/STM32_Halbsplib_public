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



#ifndef __BSP_StepMotor_HPP__
#define __BSP_StepMotor_HPP__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "main.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/

#define Positive 1   //���ò��������ת
#define Reverse 0  //���ò��������ת
#define Enable 1   //���ò�����ʹ�ܣ�����
#define Disable 0  //���ò������ʧ�ܣ�����


//stepmotor��ʾΪ�������
class stepmotor
{
public:
  GPIO_TypeDef* DIR_Port;	    //�������io�ṹ��˿ں�
	uint16_t DIR_Pin;		    //�������io�ṹ�����ź�

	TIM_HandleTypeDef* htim;		//ʹ�õ�ʱ��
	uint32_t TIM_CHANNEL;		//ʹ�õ�ʱ��ͨ������ֵ
	uint16_t Pulse_Per;			//����Ƚ�ÿ�����ӵ�CCRֵ

  GPIO_TypeDef* EN_Port;	    //ʹ��io�ṹ��˿ں�
	uint16_t EN_Pin;		    //ʹ��io�ṹ�����ź�

	uint16_t Pulse_Equivalent;	//���嵱�������ٸ�������ת��һ��

	int64_t Completed_pulse;		//��ɵ�(��)��������app��λ��ȷ��,ע��˴�����ֵ����һ������
	
		//��������
	stepmotor(void){};
	//stepmotor(GPIO_TypeDef* DIR_Port,uint16_t DIR_Pin,TIM_HandleTypeDef* htim,uint32_t TIM_CHANNEL, uint16_t Period);//����stepmotor
	stepmotor(GPIO_TypeDef* DIR_Port,uint16_t DIR_Pin,
						TIM_HandleTypeDef* htim, uint32_t TIM_CHANNEL,
						uint16_t Pulse_Equivalent,	uint16_t Pulse_Per,
						GPIO_TypeDef* EN_Port = NULL,uint16_t EN_Pin = NULL);//��ʹ�ܹ���stepmotor
	void step_init(void);
	void setDIR(uint8_t DIR);														//������������
	void setEN(uint8_t EN);															//����ʹ������
	void start(uint8_t start);													//����ʱ��ʹ������
	void setPulse(void);														//��������ռ�ձ�����
	
	void set_Completed_pulse(int64_t Completed_pulse);//�������ռ�ձ�����
	
	void rotate_with_Pulse(uint8_t Direction,uint64_t Pulse);		//����Ŀ�귽��ת������������
	void rotate_with_angle(uint8_t Direction,float angle);
	void TIMupdate(TIM_HandleTypeDef *now_htim);
		
	//���㴢����
protected:
	uint16_t Pulse;			//ͨ��CCRֵ
	uint8_t DIR;	//��ǰ�ķ���
	uint64_t Remaining_pulse;		//ʣ����Ҫִ�е�������
	
	
	
private:
	
};


///*****			���ಽ��������캯��				******/
////�����key.cpp���ù�����жϳ�ʼλ��
//class stepaxis:public stepmotor
//{
//public:
//		float Pulse_equivalent;	//���嵱����תһ�������������
//		float Absolute_angle;		//ת��ľ��ԽǶ�
//		Key limit_key;					//�жϳ�ʼ���Ŀ���
//			//��������
//		
//		stepaxis(void){};
//		stepaxis(GPIO_TypeDef* DIR_Port,uint16_t DIR_Pin,
//							TIM_HandleTypeDef* htim, uint32_t TIM_CHANNEL,
//							float Pulse_equivalent,Key limit_key,
//							GPIO_TypeDef* EN_Port = NULL,uint16_t EN_Pin = NULL);
//		uint8_t axis_init(uint8_t Direction);
//		void setorigin(float ab_angle);		//��ǰ���ԽǶ��趨
//		void setangle(float angle);				//Ŀ����ԽǶ��趨
//		void axis_TIMupdate(TIM_HandleTypeDef *now_htim);
//	//���㴢����
//private:
//};

//extern stepaxis axis1;
//extern stepaxis axis3;
//extern stepaxis axis4;
//extern stepaxis axis5;
//extern stepaxis axis6;
//**********************	�ⲿ��������	***************************//
extern stepmotor stepmotor1 ;
extern stepmotor stepmotor2 ;
#endif 
