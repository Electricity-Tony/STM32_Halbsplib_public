/**
 * @file bsp_usart.c
 * @brief 基本串口支持包
 * @author Tony_Wang
 * @version 1.2
 * @date 2023-6-19
 * @copyright
 * @par 日志:
 *   V1.0 建立基本串口重定向,中断接收回调初始化
	V1.1 更改中断回调函数触发方式，改为每次数据传输均触发，传送最终数组至RxDebugDate
	V1.2 将该库设定为与PC通信的直接函数，单独识别中断回到通道
	V1.3 更改变量命名更独立，新增在c++编译下的不使用维库选项
  V1.4 重构重定向，新增使用 platformio 时的新的重定向功能
 */
#include "bsp_usart.hpp"

#include <stdarg.h>

// 设置中断接收和发送数组
uint8_t aTxDebugBuffer[aTxDebug_size + 2] = "USART-DMA Success\r\n";
/* Buffer used for reception */
uint8_t RxDebugBuffer[aRxDebug_size + 2], RxDebugDate[aRxDebug_size + 2]; // 设置接收中断数组大小
uint8_t aRxDebugBuffer;													  // 设置接收中断数组大小
uint8_t RxDebug_Cnt = 0;												  // 接收缓冲计数
uint8_t cAlmDebugStr[] = "数据溢出\r\n";

/* 使用DMA的方式需要额外一个 fifo 空间 */
fifo RxDebug_fifo(RxDebugDate, sizeof(RxDebugDate) / sizeof(*RxDebugDate));

/**
 * 函数功能: 初始化串口DMA，并测试串口DMA发送是否成功
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：成功后串口端发送  "USART-DMA Success\r\n"
 */
void USART_Debug_DMA_Init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(fusart_Debug, RxDebugBuffer, sizeof(RxDebugBuffer));
	__HAL_UART_ENABLE_IT(fusart_Debug, UART_IT_IDLE); // 使能串口空闲中断
	HAL_UART_Transmit_DMA(fusart_Debug, (uint8_t *)&aTxDebugBuffer, sizeof(aTxDebugBuffer) / sizeof(aTxDebugBuffer[0]));
}

/**
 * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
 * @param  huart UART handle
 * @param  Size  Number of data available in application reception buffer (indicates a position in
 *               reception buffer until which, data are available)
 * @retval None
 */
/* DMA 接受处理的中断回调，实现乒乓缓存的中断，包含串口空闲中断，半/全溢出中断 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART_Debug)
	{
		/* size 指整个缓冲区中被占用的大小 */
		static uint8_t Rx_buf_pos; // 本次回调接收的数据在缓冲区的起点
		static uint8_t Rx_length;  // 本次回调接收数据的长度
		Rx_length = Size - Rx_buf_pos;
		/* 调用一个自写 fifo 函数实现数据转存的自己的 fifo 中 */
		RxDebug_fifo.write(&RxDebugBuffer[Rx_buf_pos], Rx_length); // 数据填入 FIFO
		Rx_buf_pos += Rx_length;
		if (Rx_buf_pos >= (aRxDebug_size + 2))
		{
			Rx_buf_pos = 0;
		} // 缓冲区用完后，返回 0 处重新开始
		/* 以下程序是自动将接受的内容转存到发送，测试可以回发后注释 */
		RxDebug_fifo.read(aTxDebugBuffer, Rx_length);
		// 这里用dma的方式回发在半溢出和全溢出的时候会出问题，用普通方式发送可以实现
		// 无法实现 HAL_UART_Transmit_DMA(fusart_Debug, (uint8_t *)aTxDebugBuffer, Rx_length);
		HAL_UART_Transmit(fusart_Debug, (uint8_t *)aTxDebugBuffer, Rx_length, 0xFFFF);
		/* 这里写把 fifo 中内容读取出来的程序 */
		/* UART Callback Code begin */

		/* UART Callback Code end */
	}
}

/* 以下为实现 DMA 串口 printf 重定向的函数 */
volatile uint8_t USART_DMA_TX_OVER = 1; // 定义全局变量指示DMA发送完成
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART_Debug)
	{
		USART_DMA_TX_OVER = 1;
	}
}
int Myprintf(const char *format, ...)
{
	va_list arg;
	static char SendBuff[200] = {0}; // 200为发送的缓存
	int rv;
	while (!USART_DMA_TX_OVER)
		; // 等待前一次DMA发送完成

	va_start(arg, format);
	rv = vsnprintf((char *)SendBuff, sizeof(SendBuff) + 1, (char *)format, arg);
	va_end(arg);

	HAL_UART_Transmit_DMA(fusart_Debug, (uint8_t *)SendBuff, rv);
	USART_DMA_TX_OVER = 0;

	return rv;
}
