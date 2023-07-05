/**
 * @file bsp_Encoder.c
 * @brief Encoder�弶֧�ְ�
 * @author Tony_Wang
 * @version 1.1
 * @date 2020-10-10
 * @copyright 
 * @par ��־:
 *   V1.0 ����Cube��������ֵ��ȡ
 *	 V1.1 �ṹ���������ṹ���ļ�����չCube������Χ���������������ݻ�ȡ��ʽ
 */		



#ifndef __BSP_Encoder_H__
#define __BSP_Encoder_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/

#define Counter_Period 0xFFFF		//�������ֵ���ٽ�ֵ��
#define Dir_range_check 5000    //���ù��ٽ��ʹ������ת��Ч�ķ�Χֵ

#define Enable 1
#define Disable 0
                             
//������ʹ�ö�ʱ��ͨ��
//#define bsp_Use_Encoder_TIM1 htim3
#define bsp_Use_Encoder_TIM2 htim4

typedef struct
{
	TIM_HandleTypeDef Use_TIM;	//�ýṹ���Ӧʹ�õ�ʱ��
	uint8_t Forward_Check;	//��ת���ٽ��־λ
	uint8_t Reverse_Check;	//��ת���ٽ��־λ
	int16_t Turn;						//���ٽ���תȦ��
	uint16_t CaptureNumber;	//��ǰ�ٽ��ڼ���ֵ
	int32_t Encoder_date;		//�ܼ���ֵ����Ϊȫ�ֶ�λ������ֵ
}bsp_Encoder;

#ifdef bsp_Use_Encoder_TIM1
extern bsp_Encoder Encoder1;
#endif // bsp_Use_Encoder_TIM1

#ifdef bsp_Use_Encoder_TIM2
extern bsp_Encoder Encoder2;
#endif // bsp_Use_Encoder_TIM2

void bsp_Encoder_Init(void);
bsp_Encoder bsp_Encoder_Getdate(bsp_Encoder Encoder);
void bsp_Encoder_Update(void);

#endif 
