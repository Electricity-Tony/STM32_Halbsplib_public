/**
 * @file bsp_bmp280.c
 * @brief NRF24L01�弶֧�ְ�
 * @author Tony_Wang
 * @version 1.0
 * @date 2021-03-19
 * @copyright 
 * @par ��־:
 *   V1.0 
 */


#ifndef __BSP_BMP280_H__
#define __BSP_BMP280_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
//#define USE_FIXED_POINT_COMPENSATE	//�����㷨ѡ�ø��㲹��


//΢����������
typedef struct
{
	//��������
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


/* �궨�� --------------------------------------------------------------------*/

//bmp280��ַ����
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

	BMP280_REGISTER_STATUS			   = 0xF3,	//BMP��ǰ״̬�Ĵ���
	BMP280_REGISTER_CONTROL            = 0xF4,	//���Ƶ�ַ1
	BMP280_REGISTER_CONFIG             = 0xF5,	//���Ƶ�ַ2
	BMP280_REGISTER_PRESSUREDATA       = 0xF7,
	BMP280_REGISTER_TEMPDATA    	   = 0xFA,
	
//	BMP280_REGISTER_PRESSUREDATA	      		 = 0xF7,
//	BMP280_REGISTER_TEMPDATA       = 0xFA,
	
};

//����λ����
enum status_flag
{
	BMP280_MEASURING = 0x08,
	BMP280_IM_UPDATE = 0x01,
	
};


/************* bmp280�����������ö���**********************/
//ѹ�����������ʣ�0XF4[7:5]��
#define	BMP280_BITS_PRESS_FS_1			0x20	//16bits/2.62Pa 
#define	BMP280_BITS_PRESS_FS_2      0x40	//17bits/1.31a
#define	BMP280_BITS_PRESS_FS_4      0x60	//18bits/0.66Pa
#define	BMP280_BITS_PRESS_FS_8      0x80	//19bits/0.33Pa
#define	BMP280_BITS_PRESS_FS_16     0xA0	//20bits/0.16Pa


//�¶ȳ��������ʣ�0XF4[4:2]��
#define	BMP280_BITS_TEMP_FS_1				0x04	//16bits/0.005�� 
#define	BMP280_BITS_TEMP_FS_2       0x08	//17bits/0.0025��
#define	BMP280_BITS_TEMP_FS_4       0x0C	//18bits/0.0012��
#define	BMP280_BITS_TEMP_FS_8       0x10	//19bits/0.0006��
#define	BMP280_BITS_TEMP_FS_16      0x14	//20bits/0.0003��

//����ģʽѡ��0XF4[1:0]��
#define	BMP280_BITS_POWMOD_SLEEP		0x00	//˯��ģʽ
#define	BMP280_BITS_POWMOD_FORCED		0x01	//����ģʽ
#define	BMP280_BITS_POWMOD_NORMAL		0x03	//����ģʽ

//����ʱ��ѡ��0XF5[7:5]��
#define	BMP280_BITS_T_SB1						0x00	//0.5ms
#define	BMP280_BITS_T_SB2						0x20	//62.5ms
#define	BMP280_BITS_T_SB3						0x40	//125ms
#define	BMP280_BITS_T_SB4						0x60	//250ms
#define	BMP280_BITS_T_SB5						0x80	//500ms
#define	BMP280_BITS_T_SB6						0xA0	//1000ms
#define	BMP280_BITS_T_SB7						0xC0	//2000ms
#define	BMP280_BITS_T_SB8						0xE0	//4000ms


//IIR filter(�˲���)ϵ����0XF5[4:2]��
#define	BMP280_BITS_CONFIG_FS_0			0x00	//��ʹ���˲���
#define	BMP280_BITS_CONFIG_FS_2			0x04	//�˲���ϵ��2������0.223
#define	BMP280_BITS_CONFIG_FS_4			0x08	//�˲���ϵ��4������0.092
#define	BMP280_BITS_CONFIG_FS_8			0x0C	//�˲���ϵ��8������0.042
#define	BMP280_BITS_CONFIG_FS_16		0x14	//�˲���ϵ��16������0.024

//3��spiʹ�ܣ�0XF5[0]��
#define	BMP280_BITS_SPI3W_ENABLE		0x01	//ʹ��3��SPI
#define	BMP280_BITS_SPI3W_DISANLE		0x00	//��ʹ��3��SPI


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
