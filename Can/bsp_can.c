/**
 * @brief    CAN板级支持包
 * @details  CAN总线相关设置，数据接收解析函数
 * @author   Tony_Wang
 * @version  2.2
 * @par Copyright (c):  RM2020电控
 * @par 日志
 * @date      2020.10.7
 *				2020.10.7		|		2.1		|		重新编写ReadME，便于从零开始库的使用
 *				2020.10.8		|		2.2		|		进行宏定义设定，可用于F1使用，并可开关使用电机库单独使用CAN
 *
 */
#include "bsp_can.hpp"

#ifdef USE_bsp_motor
#include "app_motor.h"
#endif // USE_bsp_motor

/**
 * @brief  CAN总线配置初始化
 * @details  初始化滤波器，根据宏定义的开启来初始化CAN总线
 * @param  NULL
 * @retval  NULL
 */
void bsp_can_Init(void)
{
	// CAN滤波器设置，此部分不需要修改，直接用就行
	CAN_FilterTypeDef CAN_FilterConfig;

	CAN_FilterConfig.SlaveStartFilterBank = 0;
	CAN_FilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfig.FilterIdHigh = 0X0000;
	CAN_FilterConfig.FilterIdLow = 0X0000;
	CAN_FilterConfig.FilterMaskIdHigh = 0X0000;
	CAN_FilterConfig.FilterMaskIdLow = 0X0000;
	CAN_FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	CAN_FilterConfig.FilterActivation = ENABLE;
#ifdef BSP_CAN_USE_CAN1
	CAN_FilterConfig.FilterBank = 0;
	if (HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterConfig) != HAL_OK)
	{
		while (1)
			; // 初始化没有成功则暴露错误
	}
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); /*开启中断*/
#endif

#ifdef BSP_CAN_USE_CAN2
	CAN_FilterConfig.FilterBank = 14; // CAN2滤波器用14号
	if (HAL_CAN_ConfigFilter(&hcan2, &CAN_FilterConfig) != HAL_OK)
	{
		while (1)
			; // 初始化没有成功则暴露错误
	}
	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING); /*开启中断*/
#endif

#ifdef BSP_CAN_USE_CAN
	CAN_FilterConfig.FilterBank = 0; // CAN2滤波器用14号
	if (HAL_CAN_ConfigFilter(&hcan, &CAN_FilterConfig) != HAL_OK)
	{
		while (1)
			; // 初始化没有成功则暴露错误
	}
	HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING); /*开启中断*/
#endif																  // BSP_CAN_USE_CAN
}

/**
 * @brief  CAN发送数据
 * @details  通过CAN总线发送控制数据
 * @param  CAN_HandleTypeDef* hcan 发送使用的CAN总线,int16_t StdId CAN线发送ID,int16_t* Can_Send_Data 需要发送的数据
 * @retval  HAL_RESULT 发送结果 HAL_OK 成功，HAL_ERROR 失败
 */
HAL_StatusTypeDef bsp_can_Sendmessage(CAN_HandleTypeDef *hcan, int16_t StdId, int16_t *Can_Send_Data)
{
	uint32_t MailBox;
	CAN_TxHeaderTypeDef bsp_can_Tx;
	HAL_StatusTypeDef HAL_RESULT;

	// 将传入的数据转换为标准CAN帧数据
	uint8_t Data[8];
	Data[0] = (uint8_t)((*(Can_Send_Data + 0) >> 8));
	Data[1] = (uint8_t)(*(Can_Send_Data + 0)) & 0XFF;
	Data[2] = (uint8_t)((*(Can_Send_Data + 1) >> 8));
	Data[3] = (uint8_t)(*(Can_Send_Data + 1)) & 0XFF;
	Data[4] = (uint8_t)((*(Can_Send_Data + 2) >> 8));
	Data[5] = (uint8_t)(*(Can_Send_Data + 2)) & 0XFF;
	Data[6] = (uint8_t)((*(Can_Send_Data + 3) >> 8));
	Data[7] = (uint8_t)(*(Can_Send_Data + 3)) & 0XFF;

	// 设置CAN帧配置
	bsp_can_Tx.StdId = StdId;
	bsp_can_Tx.RTR = CAN_RTR_DATA;
	bsp_can_Tx.IDE = CAN_ID_STD;
	bsp_can_Tx.DLC = 8;
	HAL_RESULT = HAL_CAN_AddTxMessage(hcan, &bsp_can_Tx, Data, &MailBox);
#ifndef BSP_CAN_USE_FREERTOS
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3)
		; // 等待发送完成，如果是使用FreeRTOS则可以不需要这句,因为任务调度本身是需要延时的
#endif

	return HAL_RESULT;
}

/**
 * @brief  CAN接收中断
 * @details  重新定义接收中断，会自动在CAN中断中调用，不需要手动添加,使用的时候自行在此函数中替换解析函数
 * @param  NULL
 * @retval  NULL
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	static CAN_RxHeaderTypeDef bsp_can_Rx;
	uint8_t CAN_RxData[8];

	if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) != 0) // 判断中断产生
	{
		HAL_CAN_GetRxMessage(hcan, 0, &bsp_can_Rx, CAN_RxData); // 获取CAN报文

// 使用电机库须返回相对应的电机值
#ifdef USE_bsp_motor
		motor::CANUpdate(hcan, &bsp_can_Rx, (uint8_t *)CAN_RxData);
#endif // USE_bsp_motor

// CAN1收到数据的解析函数
#ifdef BSP_CAN_USE_CAN1
		if (hcan == &hcan1)
		{
		}
#endif

// CAN2收到数据的解析函数
#ifdef BSP_CAN_USE_CAN2
		if (hcan == &hcan2)
		{
		}
#endif

// 使用F1时CAN收到数据的解析函数
#ifdef BSP_CAN_USE_CAN
#endif
	}
}
