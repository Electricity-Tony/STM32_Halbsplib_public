/**
 * @file bsp_bmp280.c
 * @brief BMP280板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2021-03-23
 * @copyright 
 * @par 日志:
 *   V1.0 
 */
 
 
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_bmp280.h"
#include "spi.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/                              


/* 私有变量定义 ----------------------------------------------------------------*/                              
BMP280_Trimming dig;




//需要自己转接的3个宏定义
#define BMP280_SPI hspi1
#define BMP280_Disable()   HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_SET)
#define BMP280_Enable()    HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_RESET)


/** 
* @brief 气压计写函数   
*/
static void bsp_bmp280_WriteReg(uint8_t regAddr, uint8_t data)
{
	regAddr &= 0x7f;  //首位0位写
	BMP280_Enable();   
	HAL_SPI_Transmit(&BMP280_SPI, &regAddr,1,100);
	HAL_SPI_Transmit(&BMP280_SPI, &data,1,100);
	BMP280_Disable();
}	


/** 
* @brief 气压计单次读取微调数据
*/
uint16_t bsp_bmp280_ReadReg_trimming(uint8_t regAddr)
{
	uint8_t temp_MSB,temp_LSB;
	uint16_t temp;
	bsp_bmp280_ReadRegs(regAddr,&temp_LSB,1);
	bsp_bmp280_ReadRegs(regAddr+1,&temp_MSB,1);
	temp = (uint16_t)(temp_MSB << 8)| temp_LSB;
	return temp;	
}


/** 
* @brief 气压计单次读取测量数据
*/
int32_t bsp_bmp280_ReadReg_measure(uint8_t regAddr)
{
	uint8_t temp_MSB,temp_LSB,temp_XLSB;
	int32_t temp;
	bsp_bmp280_ReadRegs(regAddr,&temp_MSB,1);
	bsp_bmp280_ReadRegs(regAddr+1,&temp_LSB,1);
	bsp_bmp280_ReadRegs(regAddr+2,&temp_XLSB,1);
	temp = ((int32_t)(temp_MSB << 12)|(int32_t)(temp_LSB << 4)|(temp_XLSB >>4));
	return temp;	
}

/**
* @brief 气压计读取函数
*/
void bsp_bmp280_ReadRegs(uint8_t regAddr,uint8_t *pBuff,uint8_t length)
{
  regAddr |= 0x80;
  BMP280_Enable();
	HAL_SPI_Transmit(&BMP280_SPI, &regAddr, 1, 100);
	HAL_SPI_Receive(&BMP280_SPI, pBuff, length, 100);
	BMP280_Disable();	
}



/**
* @brief 气压计获取当前状态
* @retval 0：空闲 1：占用  
* @in 		状态地址
* @return  
*/
uint8_t bsp_bmp280_GetStatus(uint8_t status_flag)
{
  uint8_t Status;
	bsp_bmp280_ReadRegs(BMP280_REGISTER_STATUS,&Status,1);
	//获取的数据bit3：measuring	bit0：im_update
	if(Status&status_flag) return 1;
	else return 0;
}


/**
* @brief 气压计复位
* @retval   
* @in 		
* @return  
*/
void bsp_bmp280_Reset(void)
{
	bsp_bmp280_WriteReg(BMP280_REGISTER_SOFTRESET,0xB6);
}

/**
* @brief 气压自检
* @retval
* @in
* @return
*/
uint8_t bsp_bmp280_Check(void)
{
	uint8_t bmp_ID;
	bsp_bmp280_ReadRegs(BMP280_REGISTER_CHIPID,&bmp_ID, 1);
	if (bmp_ID == 0x58) return 0;
	else return 1;
}


//补偿计算（此处为直接复制）
/**************************传感器值转定点值*************************************/
int64_t t_fine;			//用于计算补偿
int32_t adc_T,adc_P;

//我用浮点补偿
#ifdef USE_FIXED_POINT_COMPENSATE
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC. 
// t_fine carries fine temperature as global value
int32_t bsp_bmp280_GetTemperature(void)
{
	int32_t var1, var2, Temperature;
	adc_T = bsp_bmp280_ReadReg_measure(BMP280_REGISTER_TEMPDATA);
	var1 = ((((adc_T>>3) - ((int32_t)dig.t1<<1))) * ((int32_t)dig.t2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)dig.t1)) * ((adc_T>>4) - ((int32_t)dig.t1))) >> 12) * 
	((int32_t)dig.t3)) >> 14;
	t_fine = var1 + var2;
	Temperature = (t_fine * 5 + 128) >> 8;
	return Temperature;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t bsp_bmp280_GetPressure(void)
{
	int64_t var1, var2, Pressure;
	adc_P = bsp_bmp280_ReadReg_measure(BMP280_REGISTER_PRESSUREDATA); 
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig.p6;
	var2 = var2 + ((var1*(int64_t)dig.p5)<<17);
	var2 = var2 + (((int64_t)dig.p4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig.p3)>>8) + ((var1 * (int64_t)dig.p2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig.p1)>>33;
	if (var1 == 0)
	{
	return 0; // avoid exception caused by division by zero
	}
	Pressure = 1048576-adc_P;
	Pressure = (((Pressure<<31)-var2)*3125)/var1;
	var1 = (((int64_t)dig.p9) * (Pressure>>13) * (Pressure>>13)) >> 25;
	var2 = (((int64_t)dig.p8) * Pressure) >> 19;
	Pressure = ((Pressure + var1 + var2) >> 8) + (((int64_t)dig.p7)<<4);
	return (uint32_t)Pressure;
}


/***********************************CUT*************************************/
#else
/**************************传感器值转定点值*************************************/
// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
	
	
double bsp_bmp280_GetTemperature(void)
{
	double var1, var2, Temperature;
	adc_T = bsp_bmp280_ReadReg_measure(BMP280_REGISTER_TEMPDATA);
//		adc_T = 519888;//样例数据检验
	var1 = (((double)adc_T)/16384.0 - ((double)dig.t1)/1024.0) * ((double)dig.t2);
	var2 = ((((double)adc_T)/131072.0 - ((double)dig.t1)/8192.0) *
	(((double)adc_T)/131072.0 - ((double) dig.t1)/8192.0)) * ((double)dig.t3);
	t_fine = (int32_t)(var1 + var2);
	Temperature = (var1 + var2) / 5120.0;
	return Temperature;
}

// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
double bsp_bmp280_GetPressure(void)
{
	double var1, var2, Pressure;
	adc_P = bsp_bmp280_ReadReg_measure(BMP280_REGISTER_PRESSUREDATA); 
//	adc_P = 415148;//样例数据检验
	var1 = ((double)t_fine/2.0) - 64000.0;
	var2 = var1 * var1 * ((double)dig.p6) / 32768.0;
	var2 = var2 + var1 * ((double)dig.p5) * 2.0;
	var2 = (var2/4.0)+(((double)dig.p4) * 65536.0);
	var1 = (((double)dig.p3) * var1 * var1 / 524288.0 + ((double)dig.p2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double)dig.p1);
	if (var1 == 0.0)
	{
	return 0; // avoid exception caused by division by zero
	}
	Pressure = 1048576.0 - (double)adc_P;
	Pressure = (Pressure - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)dig.p9) * Pressure * Pressure / 2147483648.0;
	var2 = Pressure * ((double)dig.p8) / 32768.0;
	Pressure = Pressure + (var1 + var2 + ((double)dig.p7)) / 16.0;
	return Pressure;
}
#endif





/**
* @brief 气压计初始化函数
*/
void bsp_bmp280_init(void)
{
	//微调数据测量
	dig.t1 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_T1);
	dig.t2 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_T2);
	dig.t3 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_T3);
	dig.p1 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P1);
	dig.p2 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P2);
	dig.p3 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P3);
	dig.p4 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P4);
	dig.p5 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P5);
	dig.p6 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P6);
	dig.p7 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P7);
	dig.p8 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P8);
	dig.p9 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P9);
	
//	dig.t1 = 27504;
//	dig.t2 = 26435;
//	dig.t3 = -1000;
//	dig.p1 = 36477;
//	dig.p2 = -10685;
//	dig.p3 = 3024;
//	dig.p4 = 2855;
//	dig.p5 = 140;
//	dig.p6 = -7;
//	dig.p7 = 15500;
//	dig.p8 = -14600;
//	dig.p9 = 6000;
	
	//bsp_bmp280_Reset();
	//控制数据写入
	uint8_t BMP280_CONTROL,BMP280_CONFIG;
	//0XF4控制地址写入数据，设置压力分辨率，温度分辨率，运行模式
	BMP280_CONTROL = BMP280_BITS_PRESS_FS_4|BMP280_BITS_TEMP_FS_1|BMP280_BITS_POWMOD_NORMAL;
	//0XF4控制地址写入数据，设置维持时间，滤波器系数，SPI3/4线模式
	BMP280_CONFIG = BMP280_BITS_T_SB1|BMP280_BITS_CONFIG_FS_0|BMP280_BITS_SPI3W_DISANLE;
	
	//写入
	bsp_bmp280_WriteReg(BMP280_REGISTER_CONTROL,BMP280_CONTROL);
	bsp_bmp280_WriteReg(BMP280_REGISTER_CONTROL,BMP280_CONFIG);
}




