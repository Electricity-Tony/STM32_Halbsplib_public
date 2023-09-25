# flash库使用教程

## 1 日志

 * @外设相关：<font color=Red>flash</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                         功能                          |
   | :--------------------------------- | :---------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>flash 操作的基本库函数</font> |
   |                                    |                                                       |
   |                                    |                                                       |
   |                                    |                                                       |


 ## 2 文件介绍

> | bsp_flash.c     | 主文件     |
> | --------------- | ---------- |
> | **bsp_flash.h** | **头文件** |
>
> 可能需要依托的文件.
> > 
> > | 依赖库名称    | 版本 | 依赖功能 |
> > | ------------- | -------- |-------- |
> > |            |      |          |
> > |            |      |          |
> > |            |      |          |
> > |            |      |          |
> > |            |      |          |
> > |               |          |          |
> > |               |          |          |
> > |               |          |          |

 ## 3 重要函数介绍

### 3.1 基板参数声明

```c
#include "stm32f1xx.h"
#include "main.h"

#define STM32_FLASH_SIZE 64 // 所选 STM32 的 falsh 大小(KB)

#if STM32_FLASH_SIZE < 256     // 大小判断
#define STM32_SECTOR_SIZE 1024 // 扇区 1K
#else
#define STM32_SECTOR_SIZE 2048 // 扇区 2K
#endif

#define STM32_FLASH_BASE 0x08000000 // STM32 的起始地址
```

* 一定注意声明对应的 stm32 型号时写明对应的扇区大小等参数



### 3.2 具体操作

* !!!!!!!!!!!!!!!!!!!

* 注意，基本流程是：

* > 1. 解锁flash
  > 2. 擦除对应扇区
  > 3. 写入对应数据
  > 4. 锁住flash

#### 3.2.1 擦除扇区

```c
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
```

* 设定开始擦除的扇区编号和擦除数量



#### 3.2.2 写入 flash 数据

```c
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
			Error_Handler();
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
			Error_Handler();
		}
	}
	HAL_FLASH_Lock();
}
```

* 写入数据，输入参数为对应扇区号，便偏移字节数（一个数据是4字节，至少是4的倍数），写入数据的地址，写入数据数量
* 这里有两个函数，分别是 uint32_t 和 float 类型的实现，通过 union 共用地址的方式实现

### 3.2.3 读取 flash 数据

* 读取 flash 数据不需要解锁 flash

```c
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
```

* 读取数据，输入参数为对应扇区号，便偏移字节数（一个数据是4字节，至少是4的倍数），读取数据的地址，读取数据数量
* 这里有两个函数，分别是 uint32_t 和 float 类型的实现，通过 union 共用地址的方式实现



 ## 4 自定义修改参数

* flash 大小 ，扇区大小 等


 ## 5 基本使用方法

### 5.1 默认使用方法

* 调用 <font color=DeepSkyBlue>bsp_flash_Erase</font> 擦除对应扇区
* 调用 <font color=DeepSkyBlue>bsp_flash_Write 或者 bsp_flash_Write_float</font> 写入数据
* 调用 <font color=DeepSkyBlue>bsp_flash_read 或者 bsp_flash_read_float</font> 读取数据




 ## 6 其他注意

* <font color=DeepSkyBlue>非常重要！！！</font> 擦除一次后，同一个地址区域不能写入两次，只有第一次写入才有效

 
