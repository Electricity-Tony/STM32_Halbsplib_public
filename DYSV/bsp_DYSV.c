/**
 * @file bsp_DYSV.c
 * @brief 基本DYSV功放支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2020-10-5
 * @copyright 
 * @par 日志:
 *   V1.0 建立基本串口发送功能，可进行通信操作
 */
#include "bsp_DYSV.h"
#include "usart.h"




//设置中断接收和发送数组
uint8_t aTx_DYSV_StartMassage[aTx_DYSV_size] = "Dysv Success\r\n";
/* Buffer used for reception */
uint8_t Rx_DYSV_Buffer[aRx_DYSV_size+2],Rx_DYSV_Date[aRx_DYSV_size+2];	//设置接收中断数组大小
uint8_t aRx_DYSV_Buffer;	//设置接收中断数组大小
uint8_t Rx_Cnt_DYSV_DYSV = 0;        //接收缓冲计数
uint8_t  cAmStr_DYSV[] = "数据溢出\r\n";



//设置功放相关控制命令
uint8_t PlayFromBegin[4]={0xAA,0X02,0x00,0xAC};
uint8_t Playmusic01[6]={0xAA,0X07,0x02,0x00,0x01,0xB4};
uint8_t Stop[4]={0xAA,0x03,0x00,0xAD};



/**
* 函数功能: 中断接受函数使能初始化,失能中断发送函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：成功后串口端发送  "USART-interrupt Success"
  */
void USART_DYSV_Interrupt_Init(void)
{
	__HAL_UART_DISABLE_IT(fusart_DYSV_DYSV,UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(fusart_DYSV_DYSV,UART_IT_RXNE);
	HAL_UART_Receive_IT(fusart_DYSV_DYSV, (uint8_t *)&aRx_DYSV_Buffer, 1);
}




/**
* 函数功能: 串口命令发送函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：
  */
void USART_DYSV_TX_Music(uint8_t TxDate[8])
{
	//uint8_t length=sizeof(TxDate);
	uint8_t i;
	for(i=0;i<8;i++)
	{
	HAL_UART_Transmit(fusart_DYSV_DYSV,(uint8_t *)TxDate+i , 1,0xFFFF);
	//HAL_UART_Transmit(&huart1,(uint8_t *)TxDate+i , 1,0xFFFF);
	}
	
}


#ifdef EXTERN_USART
/**
  * @brief Rx Transfer completed callbacks
  * @param huart: uart handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);
	/* NOTE : This function should not be modified, when the callback is needed,
			  the HAL_UART_RxCpltCallback can be implemented in the user file
	*/
	if (huart->Instance == USART_DYSV)	// 判断是由哪个串口触发的中断
	{
		if (Rx_Cnt_DYSV >= aRx_DYSV_size + 2)  //溢出判断
		{
			Rx_Cnt_DYSV = 0;
			memset(Rx_DYSV_Buffer, 0x00, sizeof(Rx_DYSV_Buffer));
			//HAL_UART_Transmit(fusart_DYSV, (uint8_t*)& cAmStr_DYSV, sizeof(cAmStr_DYSV), 0xFFFF);
		}
		else
		{
			Rx_DYSV_Buffer[Rx_Cnt_DYSV++] = aRx_DYSV_Buffer;   //接收数据转存

			if ((Rx_DYSV_Buffer[Rx_Cnt_DYSV - 1] == 0x0A) && (Rx_DYSV_Buffer[Rx_Cnt_DYSV - 2] == 0x0D)) //判断结束位
			{
				//HAL_UART_Transmit(fusart_DYSV, (uint8_t*)& Rx_DYSV_Buffer, Rx_Cnt_DYSV, 0xFFFF); //将收到的信息发送出去	
				memcpy(Rx_DYSV_Date, Rx_DYSV_Buffer, aRx_DYSV_size + 2);
				Rx_Cnt_DYSV = 0;
				memset(Rx_DYSV_Buffer, 0x00, sizeof(Rx_DYSV_Buffer)); //清空数组
			}
		}
		HAL_UART_Receive_IT(fusart_DYSV, (uint8_t*)& aRx_DYSV_Buffer, 1);   //再开启接收中断
	}
}
#endif // EXTERN_USART


