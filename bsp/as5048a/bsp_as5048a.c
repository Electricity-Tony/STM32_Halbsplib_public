/*
 * @Author: Tony_Wang
 * @Date: 2024-03-26 15:20:49
 * @LastEditors: Tony_Wang
 * @LastEditTime: 2024-03-26 19:36:45
 * @FilePath: \F4programe\cubemx\User_Code\bsp\as5048a\bsp_as5048a.c
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */

/**
 * @file bsp_as5048a.c
 * @brief 霍尔传感器 as5048a 板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2024-3-26
 * @copyright
 * @par 日志:
 *   V1.0 基本Cube配置与基本设置
 *
 */

#include "bsp_as5048a.h"
#include "spi.h"

#define PI 3.1415926535f
#define SPI_CS_ENABLE HAL_GPIO_WritePin(GPIO_CS_Pin_Type, GPIO_CS_Pin_Name, GPIO_PIN_RESET)
#define SPI_CS_DISABLE HAL_GPIO_WritePin(GPIO_CS_Pin_Type, GPIO_CS_Pin_Name, GPIO_PIN_SET)

// 需要自己转接的3个宏定义
#define AS5048A_SPI hspi1
#define GPIO_CS_Pin_Name SPI1_CS_Pin
#define GPIO_CS_Pin_Type SPI1_CS_GPIO_Port

/* 读取角度值 */
float bsp_as5048a_ReadAngle(void)
{
    // return (ReadValue(READ_ANGLE_VALUE) * 360.0 / 0x10000);/* 返回 角度值 [0,360] */
    // return (bsp_as5048a_ReadValue(READ_ANGLE_VALUE) * 2.0 * PI / 0x10000); /* 返回 弧度值 [0,2*PI] */
    return (bsp_as5048a_ReadValue(CMD_ANGLE) * 2.0 * PI / 0x3fff); /* 返回 弧度值 [0,2*PI] */
}

/* 空功能函数，用于正常读取值 */
uint16_t bsp_as5048a_ReadNOP(void)
{
    bsp_as5048a_ReadValue(CMD_NOP);
}

/* 清除错误标志位函数函数，用于正常读取值 */
uint16_t bsp_as5048a_CLEARerr(void)
{
    bsp_as5048a_ReadValue(CMD_CLEAR);
}

/* 调用的公用函数 */
uint16_t bsp_as5048a_ReadValue(uint16_t u16RegValue)
{
    uint16_t u16Data;

    SPI_CS_ENABLE;
    HAL_SPI_TransmitReceive(&AS5048A_SPI, (uint8_t *)&u16RegValue, (uint8_t *)&u16Data, 1, 0xff);
    SPI_CS_DISABLE;
    return (u16Data & 0x3FFF);
    // return u16Data;
}