# USART库使用教程



## 1 日志

* @外设相关：<font color=Red>USART</font> 

  @版本：<font color=Red>1.4</font> 

  @维护：<font color=Red>Tony_Wang</font> 

  @代码规范：<font color=Red>暂无</font>

  | 版本                               |                             功能                             |
  | :--------------------------------- | :----------------------------------------------------------: |
  | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>建立基本串口重定向,中断接收回调初始化</font> |
  |  <font color=DeepSkyBlue>1.1</font>  |<font color=DeepSkyBlue>更改中断回调函数触发方式，改为每次数据传输均触发，传送最终数组至RxDate</font> |
  |  <font color=DeepSkyBlue>1.2</font>  |<font color=DeepSkyBlue>将该库设定为与PC通信的直接函数，单独识别中断回到通道</font> |
  |  <font color=DeepSkyBlue>1.3</font>  |<font color=DeepSkyBlue>更改变量命名更独立，新增在c++编译下的不使用维库选项</font> |
  |  <font color=DeepSkyBlue>1.4</font>  |<font color=DeepSkyBlue>重构重定向，新增使用 platformio 时的新的重定向功能</font> |

## 2 文件介绍

>> bsp_usart.c            usart主文件
>>
>> bsp_usart.h            usart头文件

## 3 重要函数介绍

### 3.1 初始化函数 

```c
void USART_Debug_Interrupt_Init(void)
{
	__HAL_UART_DISABLE_IT(fusart_Debug,UART_IT_RXNE);
	HAL_UART_Transmit(fusart_Debug, (uint8_t *)&aTxDebugStartMessage, sizeof(aTxDebugStartMessage), 0xffff);
	__HAL_UART_ENABLE_IT(fusart_Debug,UART_IT_RXNE);
	HAL_UART_Receive_IT(fusart_Debug, (uint8_t *)&aRxDebugBuffer, 1);
}
```

* 验证串口是否可用，初始化成功串口端发送：USART-interrupt Success

### 3.2  keil 重定向函数

```c
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
```

* 重定向到基本输入输出到串口端，现可以通过宏定义切换是否使用维库，不使用维库版本在触发中断回调若干次（很多很多次后）会无法进入中断回调，在哪不清楚问题

### 3.3 platformio 重定向函数

```c
// 使用 PlatformIO 的重定向
#ifdef USE_PlatformIO
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
#endif // USE_PlatformIO
```

* 通过 h 文件中的宏定义开关修改使用不同开发平台的重定向
* 使用 platformio 重定向时还需要<font color=red> 添加 syscalls.c</font> 至目标路径，该文件位于C:\Users\admin\STM32Cube\Repository\STM32Cube_FW_F1_V1.8.4\Projects\STM3210C_EVAL\Examples\UART\UART_Printf\SW4STM32
* 听说同时还需要在 platformio.ini 中添加以下代码实现输出浮点数，<font color = red> 但是不加发现也能用</font>

```ini
build_flags = 
	-Wl,-u,_printf_float
```



### 3.4 接收中断回调函数

```c
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
```

* 主要串口函数，每次有数据接收便进入中断，将单个字符缓冲（aRxDebugBuffer）存入数组缓冲（RxDebugBuffer），存储完毕后拷贝到全局变量（RxDebugDate）一遍随时调用，随后数组缓冲（RxDebugBuffer）清空。
* 通过**if (huart->Instance == USART_Debug)**单独判断是否接收是通过PC串口端，避免串口干扰。同时其余串口通道与此串口同时使用时需将其他串口中断回调函数合并在此串口中断回调中



## 4 自定义修改参数

* **fusart_Debug****：串口通道，设置所有串口操作使用的通道
* **USART_Debug**：串口中断回调通道，设置接收在哪个通道上
* **aTxDebugStartMessage**：初始化串口后的发送数组，需配套修改**aTxDebug_size**，可根据编译后数组报错自动得到修改后的数组大小
* **aRxDebug_size**：每帧数据长度，代表接收数据长度。直接决定可使用数据全局变量（RxDebugDate）的大小。【实际大小应最后停止位会+2】



## 5 基本使用方法

1. Cube配置串口相关设置，开启全局串口中断

2. 添加bsp_usart.c与bsp_usart.h文件至工程

3. 按照需求对以下内容进行修改

   ```c
   #include "stm32f1xx.h"
   
   /* ************************************************************************************* */
   // 使用keil调试使能
   // #define USE_KEIL
   
   // 使用 PlatformIO 调试使能
   #define USE_PlatformIO
   
   // 是否使用维库宏定义
   // #define USE_Microlib
   /* ************************************************************************************* */
   
   // 设置重定向串口通道
   #define fusart_Debug &huart1 // 重定向至串口1
   #define USART_Debug USART1
   
   // 设置初始化发送数组大小
   #define aTxDebug_size 26
   
   // 设置中断接收数组大小，及每一帧接收数据
   #define aRxDebug_size 8 // 设置最大接收数组大小为8
   ```

4. 初始化使用USART_Interrupt_Init（）函数

5. 自由使用printf与scanf等标准输入输出函数

6. 通过接收的数据**RxDebugDate**，进行相关操作



## 6 其他注意

* 所有串口打印操作使用的**HAL_UART_Transmit**会导致**HAL_UART_Receive_IT**中断接收重新使能函数失效，导致无法再次接收数据。使用维库版本已在所有发送前后分别失能中断和使能中断并重新使能中断接收函数。
* 可接受小于**aRx_size**的数据，但需要重新拷贝数据，易受到停止位对数据影响

