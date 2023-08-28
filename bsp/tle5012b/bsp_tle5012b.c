
/**
 * @file bsp_tle5012.c
 * @brief 霍尔传感器tle5012板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-6-24
 * @copyright
 * @par 日志:
 *   V1.0 基本Cube配置与基本设置
 *
 */

#include "bsp_tle5012b.h"
#include "spi.h"

#define PI 3.1415926535f
#define SPI_CS_ENABLE HAL_GPIO_WritePin(GPIO_CS_Pin_Type, GPIO_CS_Pin_Name, GPIO_PIN_RESET)
#define SPI_CS_DISABLE HAL_GPIO_WritePin(GPIO_CS_Pin_Type, GPIO_CS_Pin_Name, GPIO_PIN_SET)

// 需要自己转接的3个宏定义
#define TLE5012_SPI hspi2
#define GPIO_CS_Pin_Name TLE5012_CS_Pin
#define GPIO_CS_Pin_Type TLE5012_CS_GPIO_Port

/* 读取角度值 */
float ReadAngle(void)
{
    // return (ReadValue(READ_ANGLE_VALUE) * 360.0 / 0x10000);/* 返回 角度值 [0,360] */
    return (ReadValue(READ_ANGLE_VALUE) * 2.0 * PI / 0x10000); /* 返回 弧度值 [0,2*PI] */
}

/* 读取速度函数，内置的不知道好不好用 */
uint16_t ReadSpeed(void)
{
    return ReadValue(READ_SPEED_VALUE);
}

/* 调用的公用函数 */
uint16_t ReadValue(uint16_t u16RegValue)
{
    uint16_t u16Data;

    SPI_CS_ENABLE;

    HAL_SPI_Transmit(&TLE5012_SPI, (uint8_t *)(&u16RegValue), sizeof(u16RegValue) / sizeof(uint16_t), 0xff);
    HAL_SPI_Receive(&TLE5012_SPI, (uint8_t *)(&u16Data), sizeof(u16Data) / sizeof(uint16_t), 0xff);
    SPI_CS_DISABLE;

    return ((u16Data & 0x7FFF) << 1);
}