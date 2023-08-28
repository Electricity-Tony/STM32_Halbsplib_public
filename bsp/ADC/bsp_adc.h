/**
 * @file bsp_adc.c
 * @brief ��������֧�ְ�
 * @author Tony_Wang
 * @version 1.0
 * @date 2022-3-21
 * @copyright 
 * @par ��־:
 *   V1.0 ����Cube����DMA����
 *	 V1.1 ���Ӳⲻ׼�޸ĵ������ļ�
 */

 
#ifndef __BSP_ADC_H
#define __BSP_ADC_H			  	 

#include "stm32f1xx.h"

#include "main.h"

//�����ض���adcͨ��
#define fadc &hadc1               //�ض�����adc1 

//adc�ؼ���������
enum  __adc_date
{
	Sampling_number = 2,			//����ͨ������
	Sampling_times = 12,	//ÿ��ͨ����������
	
};	

extern uint32_t bsp_adc_date[Sampling_number];

void bsp_adc_init(void);
void bsp_adc_date_clear(uint32_t adc_date[]);
void bsp_adc_date_update(uint32_t adc_date[]);

#endif  
	 



