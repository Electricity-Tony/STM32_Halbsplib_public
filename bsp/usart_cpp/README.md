# USART库使用教程



## 1 日志

* @外设相关：<font color=Red>USART</font> 

  @版本：<font color=Red>2.0</font> 

  @维护：<font color=Red>Tony_Wang</font> 

  @代码规范：<font color=Red>暂无</font>

  | 版本                               |                             功能                             |
  | :--------------------------------- | :----------------------------------------------------------: |
  | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>建立基本串口重定向,中断接收回调初始化</font> |
  |  <font color=DeepSkyBlue>1.1</font>  |<font color=DeepSkyBlue>更改中断回调函数触发方式，改为每次数据传输均触发，传送最终数组至RxDate</font> |
  |  <font color=DeepSkyBlue>1.2</font>  |<font color=DeepSkyBlue>将该库设定为与PC通信的直接函数，单独识别中断回到通道</font> |
  |  <font color=DeepSkyBlue>1.3</font>  |<font color=DeepSkyBlue>更改变量命名更独立，新增在c++编译下的不使用维库选项</font> |
  |  <font color=DeepSkyBlue>1.4</font>  |<font color=DeepSkyBlue>重构重定向，新增使用 platformio 时的新的重定向功能</font> |
  
  | 版本                               |                             功能                             |
  | :--------------------------------- | :----------------------------------------------------------: |
  | <font color=DeepSkyBlue>2.0</font> | <font color=DeepSkyBlue>重构中断触发为DMA方式，需要调用V1_1以上的memory库</font> |
  |                                    |                                                              |
  |                                    |                                                              |
  |                                    |                                                              |
  |                                    |                                                              |

## 2 文件介绍

> | bsp_usart.cpp     | 主文件     |
> | ----------------- | ---------- |
> | **bsp_usart.hpp** | **头文件** |
>
> 需要依托的文件
> > | memery.hpp | V1_1 以上的存储库，使用环形 fifo |
> > | ---------- | -------------------------------- |
> > |            |                                  |

## 3 重要函数介绍

### 3.1 初始化函数 

```cpp
void USART_Debug_DMA_Init(void)
{
	__HAL_UART_ENABLE_IT(fusart_Debug, UART_IT_IDLE); // 使能串口空闲中断
	HAL_UARTEx_ReceiveToIdle_DMA(fusart_Debug, RxDebugBuffer, sizeof(RxDebugBuffer));
	HAL_UART_Transmit_DMA(fusart_Debug, (uint8_t *)&aTxDebugBuffer, sizeof(aTxDebugBuffer) / sizeof(aTxDebugBuffer[0]));
}
```

* 使能串口空闲IDLE中断，开启DMA，并发送串口显示测试结果



### 3.2 接收中断回调函数

```cpp
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
```

* <font color=DeepSkyBlue>中断触发</font>：串口空闲 IDLE、DMA fifo半溢出/全溢出 均会触发中断。回调函数内自动根据中断类型实现数据转移
* <font color=DeepSkyBlue>接受回发</font>：实测通过 DMA 的方式回发会出现半溢出和全溢出时程序运行了第二次回发但是接受不到的情况，怀疑是由于连续两次的DMA发送TX_fifo中的数据没有发送完，第二次不发送，因此这个位置仍然采用中断的方式发送，调试结束后将这句话注释
* 通过**if (huart->Instance == USART_Debug)**单独判断是否接收是通过PC串口端，避免串口干扰。同时其余串口通道与此串口同时使用时需将其他串口中断回调函数合并在此串口中断回调中
* <font color=DeepSkyBlue>自行调用数据时，修改 RxDebug_fifo.read()，读取到的地址为自己的数组，进行后续处理即可</font>

### 3.3 platformio DMA 重定向函数

```cpp
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
```

* 通过 hpp 文件中的宏定义开关修改是否打开 printf 重定向

* ```cpp
  /* ************************************************************************************* */
  // 使用 printf 调试使能
  #define printf Myprintf
  // 关闭 printf 调试
  // #define printf
  /* ************************************************************************************* */
  ```

* 使用 platformio 重定向时还需要<font color=red> 添加 syscalls.c</font> 至目标路径，该文件位于C:\Users\admin\STM32Cube\Repository\STM32Cube_FW_F1_V1.8.4\Projects\STM3210C_EVAL\Examples\UART\UART_Printf\SW4STM32

* 听说同时还需要在 platformio.ini 中添加以下代码实现输出浮点数，<font color = red> 但是不加发现也能用</font>

```ini
build_flags = 
	-Wl,-u,_printf_float
```

* 这个是全新的使用 DMA 的重定向函数，2.0 之前的重定向不可使用


## 4 自定义修改参数

* **fusart_Debug****：串口通道，设置所有串口操作使用的通道
* **USART_Debug**：串口中断回调通道，设置接收在哪个通道上
* **aTxDebugStartMessage**：初始化串口后的发送数组，需配套修改**aTxDebug_size**，可根据编译后数组报错自动得到修改后的数组大小
* **aRxDebug_size**：每帧数据长度，代表接收数据长度。直接决定可使用数据全局变量（RxDebugDate）的大小。【实际大小应最后停止位会+2】



## 5 基本使用方法

1. Cube配置串口相关设置，开启全局串口中断
2. <font color=DeepSkyBlue>DMA Settings 中添加 TX 和 RX 的 DMA，TX的mode为Normal，RX的mode为Circular</font>
3. 添加bsp_usart.cpp与bsp_usart.hpp文件至工程,并添加 memory.hpp
4. 按照需求对相关宏定义

5. 初始化使用USART_Interrupt_Init（）函数
6. 自由使用printf输出函数
7. 通过接收的数据**RxDebugDate**，进行相关操作



## 6 其他注意

* 使用DMA 后的发送函数为 HAL_UART_Transmit_DMA()

