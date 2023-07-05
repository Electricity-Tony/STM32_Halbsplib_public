# DY-SV音响魔块库使用教程



## 1 日志

* @外设相关：DY-SV

  @版本：<font color=Red>1.0</font> 

  @维护：<font color=Red>Tony_Wang</font> 

  @代码规范：<font color=Red>暂无</font>

  | 版本                               |                             功能                             |
  | :--------------------------------- | :----------------------------------------------------------: |
  | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>建立基本串口发送功能，可进行通信操作</font> |
  |  <font color=DeepSkyBlue>1.1</font>  |<font color=DeepSkyBlue></font> |

## 2 文件介绍

>> bsp_DYSV.c            DYSV主文件
>>
>> bsp_DYSV.h            DYSV头文件

## 3 重要函数介绍

### 3.1 初始化函数 

```c
void USART_DYSV_Interrupt_Init(void)
{
	__HAL_UART_DISABLE_IT(fusart_DYSV,UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(fusart_DYSV,UART_IT_RXNE);
	HAL_UART_Receive_IT(fusart_DYSV, (uint8_t *)&aRx_DYSV_Buffer, 1);
}
```

* 初始化DYSV串口端接收，目前未使用接收，<font color=Red>暂时没有作用</font>

### 3.2 接收中断回调函数

```c
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
				memcpy(RxDate, Rx_DYSV_Buffer, aRx_DYSV_size + 2);
				Rx_Cnt_DYSV = 0;
				memset(Rx_DYSV_Buffer, 0x00, sizeof(Rx_DYSV_Buffer)); //清空数组
			}
		}
		HAL_UART_Receive_IT(fusart_DYSV, (uint8_t*)& aRx_DYSV_Buffer, 1);   //再开启接收中断
	}
}
```

* 对DYSV返回值进行处理，完成后返回值保存在Rx_DYSV_Date中

### 3.3 命令发送函数

```c#
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
```

* 直接对DYSV模块发送命令，进行相关操作

## 4 自定义修改参数

> 主文件中

* 按照串口通讯协议增加新的控制命令

```c#
//例如
//设置功放相关控制命令
uint8_t PlayFromBegin[4]={0xAA,0X02,0x00,0xAC};
uint8_t Playmusic01[6]={0xAA,0X07,0x02,0x00,0x01,0xB4};
uint8_t Stop[4]={0xAA,0x03,0x00,0xAD};
```

* 若有使用主要USART通道，将中断回调函数（HAL_UART_RxCpltCallback）内容复制到主USART的中断回调中并删除此中断回调函数
* **aTxStartMessage**：初始化串口后的发送数组，需配套修改**aTx_size**，可根据编译后数组报错自动得到修改后的数组大小，<font color=Red>暂时没有作用</font>

> 头文件中

* 对新增加的命令进行外部声明

```c#
//例如：
//设置功放相关控制命令
extern uint8_t PlayFromBegin[4];
extern uint8_t Playmusic01[6];
extern uint8_t Stop[4];
```

* **fusart_DYSV**：串口通道，设置所有串口操作使用的通道
* **USART_DYSV**：中断回调中断串口通道，设置此串口接受数据通道
* **#define EXTERN_USART**：是否有外部的串口同时使用，若有请开启并进行终端回调函数的合并
* **aTx_DYSV_size**:初始化后发送的数组大小，<font color=Red>暂时没有作用</font>
* **aRx_DYSV_size**：每帧数据长度，代表接收数据长度。直接决定可使用数据全局变量（RxDate）的大小。【实际大小应最后停止位会+2】



## 5 基本使用方法

> 1. Cube配置串口相关设置，开启串口全局中断
> 2. 添加bsp_DYSV.c与bsp_DYSV.h文件至工程
> 3. 进行相关自定义内容的修改
> 4. 初始化使用USART_Interrupt_Init（）函数
> 5. 在外部调用USART_DYSV_TX_Music进行命令发送
> 6. 通过接收的数据**Rx_DYSV_Date**，识别返回内容



## 6 其他注意

* DYSV必须接受16进制数，发送端必须单独以0x声明每一个数据的值
* HAL_UART_Transmit可以直接发送数组，但是会在0x00出跳出，不发送该值及之后的值，经调试单独发送一个数据均可以发送，所以**USART_DYSV_TX_Music**中采用循环的方式单独发送每一位

