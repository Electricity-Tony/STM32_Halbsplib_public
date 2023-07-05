/**
 * @file bsp_adc.c
 * @brief 基本串口支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2022-3-21
 * @copyright 
 * @par 日志:
 *   V1.0 基本Cube配置DMA配置
 *	 V1.1 增加测不准修改的配置文件
 */


#include "bsp_adc.h"
#include "adc.h"


uint32_t bsp_adc_Value[Sampling_number*Sampling_times];
uint32_t bsp_adc_date[Sampling_number];



/**
* @brief  初始化adc dmp采样开始
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
* @brief  将所有已采样adc数据清除
* @details  
* @param  uint32_t adc_date[] 需要清除的adc数据
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
* @brief  获取采样adc数据
* @details  
* @param  uint32_t adc_date[] 存入的adc获取数据
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












