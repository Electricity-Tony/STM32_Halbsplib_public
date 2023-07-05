/**
 * @file bsp_bmp280.c
 * @brief NRF24L01板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2021-03-19
 * @copyright 
 * @par 日志:
 *   V1.0 
 */


#ifndef __BSP_BMP280_H__
#define __BSP_BMP280_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"

/* 类型定义 ------------------------------------------------------------------*/
//#define USE_FIXED_POINT_COMPENSATE	//声明算法选用浮点补偿


//微调参数声明
typedef struct
{
	//采样数据
	uint16_t t1;
	int16_t t2;
	int16_t t3;
	uint16_t p1;
	int16_t p2;
	int16_t p3;
	int16_t p4;
	int16_t p5;
	int16_t p6;
	int16_t p7;
	int16_t p8;
	int16_t p9;
}BMP280_Trimming;

//typedef struct
//{
//	uint8_t P_Osample:3;
//	uint8_t T_Osample:3;
//	uint8_t	POWMODE:2;
//} BMP_MODE;
//	
//typedef struct
//{
//	uint8_t T_SB:3;
//	uint8_t CONFIG:3;
//	uint8_t SPI3W:1;
//} BMP_CONFIG;


/* 宏定义 --------------------------------------------------------------------*/

//bmp280地址定义
enum BMP280_REGISTER
{
	BMP280_REGISTER_DIG_T1              = 0x88,
	BMP280_REGISTER_DIG_T2              = 0x8A,
	BMP280_REGISTER_DIG_T3              = 0x8C,

	BMP280_REGISTER_DIG_P1              = 0x8E,
	BMP280_REGISTER_DIG_P2              = 0x90,
	BMP280_REGISTER_DIG_P3              = 0x92,
	BMP280_REGISTER_DIG_P4              = 0x94,
	BMP280_REGISTER_DIG_P5              = 0x96,
	BMP280_REGISTER_DIG_P6              = 0x98,
	BMP280_REGISTER_DIG_P7              = 0x9A,
	BMP280_REGISTER_DIG_P8              = 0x9C,
	BMP280_REGISTER_DIG_P9              = 0x9E,

	BMP280_REGISTER_CHIPID             = 0xD0,
	BMP280_REGISTER_VERSION            = 0xD1,
	BMP280_REGISTER_SOFTRESET          = 0xE0,

	BMP280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

	BMP280_REGISTER_STATUS			   = 0xF3,	//BMP当前状态寄存器
	BMP280_REGISTER_CONTROL            = 0xF4,	//控制地址1
	BMP280_REGISTER_CONFIG             = 0xF5,	//控制地址2
	BMP280_REGISTER_PRESSUREDATA       = 0xF7,
	BMP280_REGISTER_TEMPDATA    	   = 0xFA,
	
//	BMP280_REGISTER_PRESSUREDATA	      		 = 0xF7,
//	BMP280_REGISTER_TEMPDATA       = 0xFA,
	
};

//数据位声明
enum status_flag
{
	BMP280_MEASURING = 0x08,
	BMP280_IM_UPDATE = 0x01,
	
};


/************* bmp280控制数据设置定义**********************/
//压力超采样倍率（0XF4[7:5]）
#define	BMP280_BITS_PRESS_FS_1			0x20	//16bits/2.62Pa 
#define	BMP280_BITS_PRESS_FS_2      0x40	//17bits/1.31a
#define	BMP280_BITS_PRESS_FS_4      0x60	//18bits/0.66Pa
#define	BMP280_BITS_PRESS_FS_8      0x80	//19bits/0.33Pa
#define	BMP280_BITS_PRESS_FS_16     0xA0	//20bits/0.16Pa


//温度超采样倍率（0XF4[4:2]）
#define	BMP280_BITS_TEMP_FS_1				0x04	//16bits/0.005度 
#define	BMP280_BITS_TEMP_FS_2       0x08	//17bits/0.0025度
#define	BMP280_BITS_TEMP_FS_4       0x0C	//18bits/0.0012度
#define	BMP280_BITS_TEMP_FS_8       0x10	//19bits/0.0006度
#define	BMP280_BITS_TEMP_FS_16      0x14	//20bits/0.0003度

//功耗模式选择（0XF4[1:0]）
#define	BMP280_BITS_POWMOD_SLEEP		0x00	//睡眠模式
#define	BMP280_BITS_POWMOD_FORCED		0x01	//触发模式
#define	BMP280_BITS_POWMOD_NORMAL		0x03	//正常模式

//保持时间选择（0XF5[7:5]）
#define	BMP280_BITS_T_SB1						0x00	//0.5ms
#define	BMP280_BITS_T_SB2						0x20	//62.5ms
#define	BMP280_BITS_T_SB3						0x40	//125ms
#define	BMP280_BITS_T_SB4						0x60	//250ms
#define	BMP280_BITS_T_SB5						0x80	//500ms
#define	BMP280_BITS_T_SB6						0xA0	//1000ms
#define	BMP280_BITS_T_SB7						0xC0	//2000ms
#define	BMP280_BITS_T_SB8						0xE0	//4000ms


//IIR filter(滤波器)系数（0XF5[4:2]）
#define	BMP280_BITS_CONFIG_FS_0			0x00	//不使用滤波器
#define	BMP280_BITS_CONFIG_FS_2			0x04	//滤波器系数2，带宽0.223
#define	BMP280_BITS_CONFIG_FS_4			0x08	//滤波器系数4，带宽0.092
#define	BMP280_BITS_CONFIG_FS_8			0x0C	//滤波器系数8，带宽0.042
#define	BMP280_BITS_CONFIG_FS_16		0x14	//滤波器系数16，带宽0.024

//3线spi使能（0XF5[0]）
#define	BMP280_BITS_SPI3W_ENABLE		0x01	//使用3线SPI
#define	BMP280_BITS_SPI3W_DISANLE		0x00	//不使用3线SPI


static void bsp_bmp280_WriteReg(uint8_t regAddr, uint8_t data);
uint16_t bsp_bmp280_ReadReg_trimming(uint8_t regAddr);
int32_t bsp_bmp280_ReadReg_measure(uint8_t regAddr);
void bsp_bmp280_ReadRegs(uint8_t regAddr,uint8_t *pBuff,uint8_t length);
uint8_t bsp_bmp280_GetStatus(uint8_t status_flag);
#ifdef USE_FIXED_POINT_COMPENSATE
int32_t bsp_bmp280_GetTemperature(void);
uint32_t bsp_bmp280_GetPressure(void);
#else
double bsp_bmp280_GetTemperature(void);
double bsp_bmp280_GetPressure(void);
#endif

void bsp_bmp280_Reset(void);
uint8_t bsp_bmp280_Check(void);
void bsp_bmp280_init(void);


#endif 
