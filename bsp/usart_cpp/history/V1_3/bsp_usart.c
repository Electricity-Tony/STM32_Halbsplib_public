/**
 * @file bsp_usart.c
 * @brief 基本串口支持包
 * @author Tony_Wang
 * @version 1.2
 * @date 2020-10-5
 * @copyright 
 * @par 日志:
 *   V1.0 建立基本串口重定向,中断接收回调初始化
	V1.1 更改中断回调函数触发方式，改为每次数据传输均触发，传送最终数组至RxDebugDate
	V1.2 将该库设定为与PC通信的直接函数，单独识别中断回到通道
	V1.3 更改变量命名更独立，新增在c++编译下的不使用维库选项
 */
#include "bsp_usart.h"
#include "usart.h"




//设置中断接收和发送数组
uint8_t aTxDebugStartMessage[aTxDebug_size] = "USART-interrupt Success\r\n";
/* Buffer used for reception */
uint8_t RxDebugBuffer[aRxDebug_size+2],RxDebugDate[aRxDebug_size+2];	//设置接收中断数组大小
uint8_t aRxDebugBuffer;	//设置接收中断数组大小
uint8_t RxDebug_Cnt = 0;        //接收缓冲计数
uint8_t  cAlmDebugStr[] = "数据溢出\r\n";


/**
* 函数功能: 中断接受函数使能初始化,失能中断发送函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：成功后串口端发送  "USART-interrupt Success"
  */
void USART_Debug_Interrupt_Init(void)
{
	__HAL_UART_DISABLE_IT(fusart_Debug,UART_IT_RXNE);
	HAL_UART_Transmit(fusart_Debug, (uint8_t *)&aTxDebugStartMessage, sizeof(aTxDebugStartMessage), 0xffff);
	__HAL_UART_ENABLE_IT(fusart_Debug,UART_IT_RXNE);
	HAL_UART_Receive_IT(fusart_Debug, (uint8_t *)&aRxDebugBuffer, 1);
}


/**
  * @brief Rx Transfer completed callbacks
  * @param huart: uart handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file
  */
  if (huart->Instance == USART_Debug)	// 判断是由哪个串口触发的中断
  {
	  if (RxDebug_Cnt >= aRxDebug_size + 2)  //溢出判断
	  {
		  RxDebug_Cnt = 0;
		  memset(RxDebugBuffer, 0x00, sizeof(RxDebugBuffer));
		  HAL_UART_Transmit(fusart_Debug, (uint8_t*)& cAlmDebugStr, sizeof(cAlmDebugStr), 0xFFFF);
	  }
	  else
	  {
		  RxDebugBuffer[RxDebug_Cnt++] = aRxDebugBuffer;   //接收数据转存

		  if ((RxDebugBuffer[RxDebug_Cnt - 1] == 0x0A) && (RxDebugBuffer[RxDebug_Cnt - 2] == 0x0D)) //判断结束位
		  {
			  HAL_UART_Transmit(fusart_Debug, (uint8_t*)& RxDebugBuffer, RxDebug_Cnt, 0xFFFF); //将收到的信息发送出去	
			  memcpy(RxDebugDate, RxDebugBuffer, aRxDebug_size + 2);
			  RxDebug_Cnt = 0;
			  memset(RxDebugBuffer, 0x00, sizeof(RxDebugBuffer)); //清空数组
		  }
	  }
	  HAL_UART_Receive_IT(fusart_Debug, (uint8_t*)& aRxDebugBuffer, 1);   //再开启接收中断
  }
}

#ifndef USE_Microlib
/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：须使用维库
  */
int fputc(int ch, FILE *f)
{
	__HAL_UART_DISABLE_IT(fusart_Debug,UART_IT_RXNE);
  HAL_UART_Transmit(fusart_Debug, (uint8_t *)&ch, 1, 0xffff);
	__HAL_UART_ENABLE_IT(fusart_Debug,UART_IT_RXNE);

	HAL_UART_Receive_IT(fusart_Debug, (uint8_t *)&aRxDebugBuffer, 1);
  return ch;
}
/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：须使用维库
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
	__HAL_UART_DISABLE_IT(fusart_Debug,UART_IT_RXNE);
  HAL_UART_Receive(fusart_Debug, &ch, 1, 0xffff);
	__HAL_UART_ENABLE_IT(fusart_Debug,UART_IT_RXNE);
	HAL_UART_Receive_IT(fusart_Debug, (uint8_t *)&aRxDebugBuffer, 1);
  return ch;
}

#else
/**
  * 函数功能: 不使用维库的重定向函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：关闭半主机模式等
  */
extern "C"
{
    void _sys_exit(int returncode)
    {
        printf("Exited! returncode=%d\n", returncode);
        while (1);
    }

    void _ttywrch(int ch)
    {
        if (ch == '\n')
            HAL_UART_Transmit(fusart_Debug, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
        else
            HAL_UART_Transmit(fusart_Debug, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    }
}
namespace std
{
    struct __FILE
    {
        int handle;
        /* Whatever you require here. If the only file you are using is */
        /* standard output using printf() for debugging, no file handling */
        /* is required. */
    };
    FILE __stdin = { 0 };
    FILE __stdout = { 1 };
    FILE __stderr = { 2 };

    int fgetc(FILE* stream)
    {
        int c = 0;

        if (stream->handle == 0)
        {
            while (__HAL_UART_GET_FLAG(fusart_Debug, UART_FLAG_RXNE) == RESET);
            HAL_UART_Receive(fusart_Debug, (uint8_t*)&c, 1, HAL_MAX_DELAY);

            _ttywrch((c == '\r') ? '\n' : c); // 回显
            return c;
        }
        return EOF;
    }

    int fputc(int c, FILE* stream)
    {
        if (stream->handle == 1 || stream->handle == 2)
        {
            _ttywrch(c);
            return c;
        }
        return EOF;
    }

    int fclose(FILE* stream)
    {
        return 0;
    }

    int fseek(FILE* stream, long int offset, int whence)
    {
        return -1;
    }

    int fflush(FILE* stream)
    {
        if (stream->handle == 1 || stream->handle == 2)
            while (__HAL_UART_GET_FLAG(fusart_Debug, UART_FLAG_TC) == RESET);
        return 0;
    }
}

#endif // USE_Microlib


