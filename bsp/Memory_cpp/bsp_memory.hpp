/**
 * @file bsp_memory.cpp
 * @brief memory板级支持包
 * @author Tony_Wang
 * @version 1.1
 * @date 2023-8-24
 * @copyright
 * @par 日志:
 *   V1.0 基本配置相关说明
 *   V1.1 新增加环形缓冲空间fifo
 */

#ifndef __BSP_MEMORY_HPP__
#define __BSP_MEMORY_HPP__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "dep.hpp"
#include "main.h"
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/

typedef enum
{
	FIFO_NORMAL,
	FIFO_EMPTY,
	FIFO_FULL,
} fifo_State;

typedef enum
{
	POP_PUSH_SUCCESS,
	POP_PUSH_ERROR,
} pop_push_state;

// fifo 表示先进先出队列
class fifo
{
public:
	uint8_t *buffer_ptr; // 链接数组地址
	uint16_t size;		 // 环形缓存区大小
	// 公共函数
	fifo(void){};
	fifo(uint8_t *data_ptr, uint16_t size);

	uint16_t check_free_size(void);
	fifo_State check_state(void);
	pop_push_state push(uint8_t data);			   // 写入一个数据
	pop_push_state pop(uint8_t *out_data);		   // 读一个数据至 out_data
	uint16_t write(uint8_t *ptr, uint16_t length); // 写入指定长度数据
	uint16_t read(uint8_t *data, uint16_t length); // 读取指定长度数据
	uint16_t read(uint16_t length);				   // 释放指定长度数据

	// 运算储存区
protected:
	uint16_t free_size;	  // 空余的可用空间
	uint16_t write_index; // 写入位置
	uint16_t read_index;  // 读取位置

private:
};

#endif
