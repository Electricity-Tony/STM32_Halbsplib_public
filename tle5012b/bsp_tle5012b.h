
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

#ifndef _BSP_TLE5012B_H_
#define _BSP_TLE5012B_H_

#ifdef __cplusplus
extern "C"
{
#endif // cplusplus

#include "main.h"




/* SPI command for TLE5012 */
#define READ_STATUS 0x8001      // 8000
#define READ_ANGLE_VALUE 0x8021 // 8020
#define READ_SPEED_VALUE 0x8031 // 8030

#define WRITE_MOD1_VALUE 0x5060 // 0_1010_0_000110_0001
#define MOD1_VALUE 0x0001

#define WRITE_MOD2_VALUE 0x5080 // 0_1010_0_001000_0001
#define MOD2_VALUE 0x0801

#define WRITE_MOD3_VALUE 0x5091 // 0_1010_0_001001_0001
#define MOD3_VALUE 0x0000

#define WRITE_MOD4_VALUE 0x50E0 // 0_1010_0_001110_0001
#define MOD4_VALUE 0x0098       // 9bit 512

#define WRITE_IFAB_VALUE 0x50B1
#define IFAB_VALUE 0x000D
/* Functionality mode */
#define REFERESH_ANGLE 0

    float ReadAngle(void);
    uint16_t ReadSpeed(void);
    uint16_t ReadValue(uint16_t u16RegValue);
    uint16_t SPIx_ReadWriteByte(uint16_t byte);

#ifdef __cplusplus
}
#endif // cplusplus

#endif