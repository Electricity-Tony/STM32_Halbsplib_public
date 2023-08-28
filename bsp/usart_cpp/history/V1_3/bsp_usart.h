/**
 * @file bsp_usart.h
 * @author Tony_Wang
 * @version 1.2
 * @date 2020-10-5
 * @copyright 
 */
#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "stm32f1xx.h"
#include <stdio.h>
#include "string.h"

//是否使用维库宏定义
//#define USE_Microlib


//设置重定向串口通道
#define fusart_Debug &huart1               //重定向至串口1 
#define USART_Debug USART1

//设置初始化发送数组大小
#define aTxDebug_size 26

//设置中断接收数组大小，及每一帧接收数据
#define aRxDebug_size 8									//设置最大接收数组大小为8


//外部声明相关数组
/* Buffer used for reception */
extern uint8_t RxDebugDate[aRxDebug_size +2];


void USART_Debug_Interrupt_Init(void);

#endif
