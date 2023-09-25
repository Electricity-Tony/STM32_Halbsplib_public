
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

#include "bsp_flash.h"
static FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t PAGEError = 0;

/* 用于 flash 的32位 int 和 float 公用内存地址 */
union bsp_flash_datetemp_TypeDef
{
	float float_date;
	uint32_t uint32_t_date;
} bsp_flash_datetemp;

/**
 * @brief  擦除当前页面
 * @details
 * @param start_SECTOR:擦除的起始扇区编号
 * @param erase_size:擦除的扇区数量
 * @retval
 */
void bsp_flash_Erase(uint32_t start_SECTOR, uint32_t erase_size)
{
	HAL_FLASH_Unlock(); // 解锁

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;									 // 选择页擦除模式
	EraseInitStruct.PageAddress = (STM32_FLASH_BASE + STM32_SECTOR_SIZE * start_SECTOR); // 设置擦除起始地址
	EraseInitStruct.NbPages = erase_size;												 // 设置擦除扇区数量

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		/* 擦除失败，锁住flash */
		HAL_FLASH_Lock();
		Error_Handler();
	}
}

/**
 * @brief  数据写入
 * @details
 * @param write_SECTOR_index:擦除的起始扇区编号
 * @param write_byte_index:写入的地址偏移
 * @param *pBuffer:写入数据地址
 * @param NumTowrite:写入数据量
 * @retval
 */
void bsp_flash_Write(uint32_t write_SECTOR_index, uint32_t write_byte_index, uint32_t *pBuffer, uint32_t NumTowrite)
{
	HAL_FLASH_Unlock(); // 解锁

	uint32_t Address = (STM32_FLASH_BASE + STM32_SECTOR_SIZE * write_SECTOR_index); // 设置擦除地址;
	// 再写入
	Address = Address + write_byte_index; // 设置到写入地址
	uint16_t i;
	for (i = 0; i < NumTowrite; i++)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, pBuffer[i]) == HAL_OK)
		{
			Address = Address + 4; // 地址后移4个字节
		}
		else
		{
			/* 这里放置写入错误代码，擦除后写入第二次及以后都会进入这里 */
			// Error_Handler();
		}
	}
	HAL_FLASH_Lock();
}

void bsp_flash_Write_float(uint32_t write_SECTOR_index, uint32_t write_byte_index, float *pBuffer, uint32_t NumTowrite)
{

	HAL_FLASH_Unlock(); // 解锁

	uint32_t Address = (STM32_FLASH_BASE + STM32_SECTOR_SIZE * write_SECTOR_index); // 设置擦除地址;
	// 再写入
	Address = Address + write_byte_index; // 设置到写入地址
	uint16_t i;
	for (i = 0; i < NumTowrite; i++)
	{
		bsp_flash_datetemp.float_date = pBuffer[i];
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, bsp_flash_datetemp.uint32_t_date) == HAL_OK)
		{
			Address = Address + 4; // 地址后移4个字节
		}
		else
		{
			/* 这里放置写入错误代码，擦除后写入第二次及以后都会进入这里 */
			// Error_Handler();
		}
	}
	HAL_FLASH_Lock();
}

/**
 * @brief  数据读取
 * @details
 * @param read_SECTOR_index:擦除的起始扇区编号
 * @param read_byte_index:写入的地址偏移
 * @param *pBuffer:存入的数据地址
 * @param NumTowrite:读入数据量
 * @retval
 */
void bsp_flash_read(uint32_t read_SECTOR_index, uint32_t read_byte_index, uint32_t *pBuffer, uint32_t NumTowrite)
{

	uint16_t i = 0;
	uint32_t Address = (STM32_FLASH_BASE + STM32_SECTOR_SIZE * read_SECTOR_index + read_byte_index); // 设置读取地址;

	for (i = 0; i < NumTowrite; i++)
	{
		pBuffer[i++] = *(__IO uint32_t *)Address;
		Address = Address + 4; // 地址后移4个字节}
	}
}
void bsp_flash_read_float(uint32_t read_SECTOR_index, uint32_t read_byte_index, float *pBuffer, uint32_t NumTowrite)
{
	uint16_t i = 0;
	uint32_t Address = (STM32_FLASH_BASE + STM32_SECTOR_SIZE * read_SECTOR_index + read_byte_index); // 设置读取地址;

	for (i = 0; i < NumTowrite; i++)
	{
		bsp_flash_datetemp.uint32_t_date = *(__IO uint32_t *)Address;
		pBuffer[i++] = bsp_flash_datetemp.float_date;
		Address = Address + 4; // 地址后移4个字节}
	}
}
