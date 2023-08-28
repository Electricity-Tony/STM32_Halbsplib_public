
#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "stm32f1xx.h"
#include "main.h"

typedef struct {
	GPIO_TypeDef* GPIO_Port;	//�����ṹ��˿ں�
	uint16_t Pin;				//�����ṹ�����ź�
	uint8_t State				//ʵ����Ҫ���õ�״̬
	uint8_t Ture_State;			//������ʾ��״������,���ڳ���ʱ����ڱ����ж�
	uint8_t Now_State;			//ʵʱ��ʱ�İ����������
	uint16_t time_flag;			//�����жϱ�־λ
	uint16_t time_check			//����ʱ���ж���
}_KeyTypedef;



extern _KeyTypedef key_up;
extern _KeyTypedef key_down;


void bsp_key_Read(_KeyTypedef* Key);
void bsp_key_State_Update(_KeyTypedef* Key);





#endif
