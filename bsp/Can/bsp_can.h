#ifndef __BSP_CAN_H
#define __BSP_CAN_H
#include "stm32f4xx.h"
//#include "stm32f1xx.h"

#include "can.h"

//芯片型号宏定义选择
//#define USE_F1XX
#define USE_F4XX

//是否使用标准电机库宏定义选择
#define USE_bsp_motor

//条件编译开关宏定义，不需要开启的就注释掉相关宏定义
//#define	BSP_CAN_USE_FREERTOS


//外设相关宏定义,移植时如果修改了外设请在这里修改
#ifdef USE_F4XX
#define BSP_CAN_USE_CAN1					hcan1
#define BSP_CAN_USE_CAN2					hcan2
#endif // USE_F4XX

#ifdef USE_F1XX
#define BSP_CAN_USE_CAN					hcan
#endif // USE_F1XX

void bsp_can_Init(void);	//CAN总线初始化函数
HAL_StatusTypeDef bsp_can_Sendmessage(CAN_HandleTypeDef* hcan,int16_t StdId,int16_t* Can_Send_Data);	//CAN总线数据发送函数
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);	//重定义的CAN中断回调函数
#endif
