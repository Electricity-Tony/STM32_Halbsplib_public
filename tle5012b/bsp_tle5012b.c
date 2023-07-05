
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

#define TLE5012_SPI hspi2

uint16_t SPIx_ReadWriteByte(uint16_t byte)
{

    uint16_t read_value = 0;

    HAL_SPI_Transmit(&TLE5012_SPI, (uint8_t *)(&byte), sizeof(byte) / sizeof(uint16_t), 0xff);

    HAL_SPI_Transmit(&TLE5012_SPI, (uint8_t *)(&read_value), sizeof(read_value) / sizeof(uint16_t), 0xff);
    return read_value;
}

double ReadAngle(void)
{
    return (ReadValue(READ_ANGLE_VALUE) * 360.0 / 0x10000);
}

uint16_t ReadSpeed(void)
{
    return ReadValue(READ_SPEED_VALUE);
}

uint16_t ReadValue(uint16_t u16RegValue)
{
    uint16_t u16Data;

    SPI_CS_ENABLE;

    HAL_SPI_Transmit(&TLE5012_SPI, (uint8_t *)(&u16RegValue), sizeof(u16RegValue) / sizeof(uint16_t), 0xff);
    HAL_SPI_Receive(&TLE5012_SPI, (uint8_t *)(&u16Data), sizeof(u16Data) / sizeof(uint16_t), 0xff);
    SPI_CS_DISABLE;

    return ((u16Data & 0x7FFF) << 1);
}