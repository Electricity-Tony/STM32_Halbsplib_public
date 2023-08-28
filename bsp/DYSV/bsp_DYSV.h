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
//�����ض��򴮿�ͨ��
#define fusart_DYSV &huart2               //�ض���������2
#define USART_DYSV USART2

//���ó�ʼ�����������С
#define aTx_DYSV_size 26

//�����жϽ��������С����ÿһ֡��������
#define aRx_DYSV_size 8									//���������������СΪ8

//�Ƿ��������������ڲ�ͬʱʹ�ô����жϻص�����
//#define EXTERN_USART  //�ж������ͬʱʹ���жϻص���򿪣���������غϲ�

#ifdef EXTERN_USART
extern uint8_t Rx_DYSV_Buffer[aRx_DYSV_size + 2], Rx_DYSV_Date[aRx_DYSV_size + 2];	//���ý����ж������С
extern uint8_t aRx_DYSV_Buffer;	//���ý����ж������С
extern uint8_t Rx_Cnt_DYSV = 0;        //���ջ������
extern uint8_t  cAmStr_DYSV[] = "�������\r\n";
#endif // EXTERN_USART




//���ù�����ؿ�������
extern uint8_t PlayFromBegin[4];
extern uint8_t Playmusic01[6];
extern uint8_t Stop[4];

//�ⲿ�����������
/* Buffer used for reception */
extern uint8_t Rx2Date[aRx2_size+2];


void USART_DYSV_Interrupt_Init(void);
void USART_DYSV_TX_Music(uint8_t *TxDate);
#endif
