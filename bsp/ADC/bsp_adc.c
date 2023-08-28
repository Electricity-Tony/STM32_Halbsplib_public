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


#include "bsp_adc.h"
#include "adc.h"


uint32_t bsp_adc_Value[Sampling_number*Sampling_times];
uint32_t bsp_adc_date[Sampling_number];



/**
* @brief  ��ʼ��adc dmp������ʼ
* @details  
* @param  
* @retval  
*/
void bsp_adc_init(void)
{
	HAL_ADCEx_Calibration_Start(fadc);
	HAL_ADC_Start_DMA(fadc, (uint32_t*)&bsp_adc_Value, Sampling_number*Sampling_times);
}


/**
* @brief  �������Ѳ���adc�������
* @details  
* @param  uint32_t adc_date[] ��Ҫ�����adc����
* @retval  
*/
void bsp_adc_date_clear(uint32_t adc_date[])
{
	uint16_t i;
	for(i=0;i<Sampling_number;i++)
	{
		adc_date[i] = 0; 
	}
}



/**
* @brief  ��ȡ����adc����
* @details  
* @param  uint32_t adc_date[] �����adc��ȡ����
* @retval  
*/
void bsp_adc_date_update(uint32_t adc_date[])
{
	uint16_t i;
	uint8_t j;
	bsp_adc_date_clear(adc_date);
	for(i = 0;i < Sampling_number*Sampling_times;)
	{
		for(j=0;j<Sampling_number;j++)
		{
			adc_date[j] += bsp_adc_Value[i++];
		}
  }
	
	for(i=0;i<Sampling_number;i++)
	{
		adc_date[i] = adc_date[i]/Sampling_times;
	}


}












