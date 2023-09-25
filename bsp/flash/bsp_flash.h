
/**
 * @file bsp_flash.c
 * @brief flash 板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-9-25
 * @copyright
 * @par 日志:
 *   V1.0 flash 操作的基本库函数
 */

#ifndef __BSP_FLASH_HPP
#define __BSP_FLASH_HPP

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx.h"
#include "main.h"

#define STM32_FLASH_SIZE 64 // 所选 STM32 的 falsh 大小(KB)

#if STM32_FLASH_SIZE < 256     // 大小判断
#define STM32_SECTOR_SIZE 1024 // 扇区 1K
#else
#define STM32_SECTOR_SIZE 2048 // 扇区 2K
#endif

#define STM32_FLASH_BASE 0x08000000 // STM32 的起始地址
    /* 其中 F1 一共是 64k 的量，即有 64 个扇区 */

    void bsp_flash_Erase(uint32_t start_SECTOR, uint32_t erase_size);
    void bsp_flash_Write(uint32_t write_SECTOR_index, uint32_t write_byte_index, uint32_t *pBuffer, uint32_t NumTowrite);
    void bsp_flash_Write_float(uint32_t write_SECTOR_index, uint32_t write_byte_index, float *pBuffer, uint32_t NumTowrite);
    void bsp_flash_read(uint32_t read_SECTOR_index, uint32_t read_byte_index, uint32_t *pBuffer, uint32_t NumTowrite);
    void bsp_flash_read_float(uint32_t read_SECTOR_index, uint32_t read_byte_index, float *pBuffer, uint32_t NumTowrite);

#ifdef __cplusplus
}
#endif

#endif
