/**
 * @file bsp_DYSV.h
 * @author Tony_Wang
 * @version 1.0
 * @date 2020-10-5
 * @copyright 
 */
#ifndef __BSP_DYSV_H
#define __BSP_DYSV_H
#include "stm32f1xx.h"
#include <stdio.h>
#include "string.h"
//设置重定向串口通道
#define fusart_DYSV &huart2               //重定向至串口2
#define USART_DYSV USART2

//设置初始化发送数组大小
#define aTx_DYSV_size 26

//设置中断接收数组大小，及每一帧接收数据
#define aRx_DYSV_size 8									//设置最大接收数组大小为8

//是否还是用了其他串口并同时使用串口中断回调函数
//#define EXTERN_USART  //有多个串口同时使用中断回调请打开，并进行相关合并

#ifdef EXTERN_USART
extern uint8_t Rx_DYSV_Buffer[aRx_DYSV_size + 2], Rx_DYSV_Date[aRx_DYSV_size + 2];	//设置接收中断数组大小
extern uint8_t aRx_DYSV_Buffer;	//设置接收中断数组大小
extern uint8_t Rx_Cnt_DYSV = 0;        //接收缓冲计数
extern uint8_t  cAmStr_DYSV[] = "数据溢出\r\n";
#endif // EXTERN_USART




//设置功放相关控制命令
extern uint8_t PlayFromBegin[4];
extern uint8_t Playmusic01[6];
extern uint8_t Stop[4];

//外部声明相关数组
/* Buffer used for reception */
extern uint8_t Rx2Date[aRx2_size+2];


void USART_DYSV_Interrupt_Init(void);
void USART_DYSV_TX_Music(uint8_t *TxDate);
#endif
