/*
 * @Author: Tony_Wang
 * @Date: 2024-03-26 15:20:49
 * @LastEditors: Tony_Wang
 * @LastEditTime: 2024-03-26 16:39:58
 * @FilePath: \F4programe\cubemx\User_Code\bsp\as5048a\bsp_as5048a.h
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

#ifndef _BSP_AS5048A_H_
#define _BSP_AS5048A_H_

#ifdef __cplusplus
extern "C"
{
#endif // cplusplus

#include "main.h"

/* SPI command for TLE5012 */
#define CMD_ANGLE 0xFFFF //
#define CMD_AGC 0x7FFD   //
#define CMD_MAG 0x7FFE   //
#define CMD_CLEAR 0x4001 //
#define CMD_NOP 0xC000   //

/* Functionality mode */
#define REFERESH_ANGLE 0

    float bsp_as5048a_ReadAngle(void);
    uint16_t bsp_as5048a_ReadSpeed(void);
    uint16_t bsp_as5048a_ReadValue(uint16_t u16RegValue);
    uint16_t bsp_as5048a_ReadNOP(void);
    uint16_t bsp_as5048a_CLEARerr(void);
    // uint16_t bsp_as5048a_SPIx_ReadWriteByte(uint16_t byte);

#ifdef __cplusplus
}
#endif // cplusplus

#endif