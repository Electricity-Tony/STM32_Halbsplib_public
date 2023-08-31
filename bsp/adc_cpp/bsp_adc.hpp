/**
 * @file bsp_adc.cpp
 * @brief adc支持包
 * @author Tony_Wang
 * @version 2.0
 * @date 2022-8-31
 * @copyright
 * @par 日志:
 *   V1.0 基本Cube配置DMA配置
 *	 V1.1 增加测不准修改的配置文件
 *
 *	 V2.0 修改为cpp文件，区分普通adc与dmaadc
 */

#ifndef __BSP_ADC_HPP
#define __BSP_ADC_HPP

#include "stm32f1xx.h"
#include "dep.hpp"
#include "main.h"

/* ******************  采样的 ADC 类 ****************************** */
class bsp_ADC
{
public:
	ADC_HandleTypeDef *hadc; // 使用的adc通道
	uint8_t NUMs_channel;	 // 通道数量

	uint16_t data_list[16]; // 数据存放列表

	// 成员函数
	bsp_ADC(void){};
	bsp_ADC(ADC_HandleTypeDef *hadc, uint8_t NUMs_channel);
	// ~bsp_ADC();
	void init(void);   // 初始化使能adc相关功能
	void update(void); // 数据更新

protected:
private:
};

/* ******************* 派生用于 DMA 采样的 ADC 类 ********************* */
class bsp_ADC_DMA : public bsp_ADC
{
public:
	uint16_t *data_DMA_buffer; // 连接到的用于存储DMA的数组

	// 成员函数
	bsp_ADC_DMA(ADC_HandleTypeDef *hadc, uint8_t NUMs_channel, uint16_t *data_DMA_buffer);
	// ~bsp_ADC_DMA();
	void init(void);   // 初始化使能adc相关功能
	void update(void); // 数据更新

protected:
private:
};

/* 外部声明 */
extern bsp_ADC bsp_ADC2;
extern bsp_ADC_DMA bsp_adc_dma1;

#endif
