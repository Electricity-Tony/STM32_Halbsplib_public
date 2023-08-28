/**
 * @file bsp_usart.h
 * @author Tony_Wang
 * @version 1.4
 * @date 2023-6-19
 * @copyright
 */
#ifndef __BSP_USART_HPP
#define __BSP_USART_HPP
#include "stm32f1xx.h"
#include "Memory_cpp/bsp_memory.hpp"

/* 重定向头文件 */
#include <stdio.h>

/* ************************************************************************************* */
// 使用 printf 调试使能
#define printf Myprintf
// 关闭 printf 调试
// #define printf
/* ************************************************************************************* */

// 设置重定向串口通道
#define fusart_Debug &huart1 // 重定向至串口1
#define USART_Debug USART1

// 设置初始化发送数组大小
#define aTxDebug_size 30

// 设置中断接收数组大小，及每一帧接收数据
#define aRxDebug_size 50 // 设置最大接收数组大小

// 外部声明相关数组
/* Buffer used for reception */
extern uint8_t RxDebugDate[aRxDebug_size + 2];
extern fifo RxDebug_fifo;

void USART_Debug_DMA_Init(void);
int Myprintf(const char *format, ...);

#endif
