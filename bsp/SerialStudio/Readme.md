# serialstudio库使用教程

## 1 日志

 * @外设相关：<font color=Red>serialstudio</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             更新时间                             |功能|
   | :--------------------------------- | :----------------------------------------------------------: | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>2023-7-17</font> |<font color=DeepSkyBlue>成功解读 serialstudio 通讯协议，移植到stm单片机</font>|
   | <font color=DeepSkyBlue>1.1</font> | <font color=DeepSkyBlue>2023-8-31</font> |<font color=DeepSkyBlue>修改缓存区为连接外部数组，改为串口DMA发送</font>|

 ## 2 文件介绍

> | bsp_serialstudio.cpp     | 串口绘图工具主文件     |
> | ------------------------ | ---------------------- |
> | **bsp_serialstudio.hpp** | **串口绘图工具头文件** |
> > **依赖**
> >
> > | 依赖库名称    | 版本 | 依赖功能 |
> > | ------------- | -------- |-------- |
> > | bsp_usart.hpp | V2.0 | DMA 发送 |
> > | dep。hpp | V1.0 |          |
> > |               |          |          |


 ## 3 重要函数介绍

### 3.1 类介绍 serialdebug

```cpp
class serialdebug
{
public:
	/* 声明链表类，用来管理数据 */
	struct data_LinkedNode
	{
		float data;										  // 节点上存储的元素
		data_LinkedNode *next;							  // 指向下一个节点的指针
		data_LinkedNode(float x) : data(x), next(NULL) {} // 节点的构造函数
		data_LinkedNode(float x, data_LinkedNode *next) : data(x), next(next) {}
	};

	UART_HandleTypeDef *_huart; // 使用的串口通道
	uint16_t data_size;			// 一帧数据长度

	// 成员函数
	serialdebug(UART_HandleTypeDef *_huart, uint16_t data_size);
	// ~serialdebug();
	void check_frame_length(void);				// 帧长度检查
	void add_LinkedNode_AtTail(int val);		// 在末尾添加一个节点
	void config_data(float data_in, int index); // 转接发送的数据至data[]数组
	void send_frame(void);						// 发送一帧数据

protected:
	// 保护成员变量
	uint8_t frame_start[2] = {0x2f, 0x2a}; // 帧头为 "/*"
	uint8_t frame_separator[1] = {0x20};   // 间隔符为 " "
	uint8_t frame_end[2] = {0x2a, 0x2f};   // 帧尾为 "*/"

private:
	data_LinkedNode *_datadummyHead; // ；链表虚拟头结点
	uint8_t frame_length;			 // 发送一帧数据的字符串长度
	uint8_t frame[102];				 // 发送的帧，由于DMA中必须使用全局变量，使用变长数组太麻烦了，因此必须使用一个定长数组，根据需求调整大小
	uint8_t decimals = 8;			 // 小数位数
};

```

* 创建了链表类，通过链表链接外部数据，在通过函数将数据转换为字符串协议用于输出显示绘图
* 主要使用 serialstudio 开源工具，便于配置通讯协议

### 3.2 变量声明

```cpp
////******************************************* serialdebug 串口绘图类声明*************************************************************************////
/**
 * @brief  serialdebug 串口绘图类声明
 * @param [in]   *_huart	上位机的串口
 * @param [in]	data_size	发送的数据个数
 *
 */

WEAK serialdebug::serialdebug(UART_HandleTypeDef *_huart, uint16_t data_size)
{
	this->_huart = _huart;
	this->data_size = data_size;
	this->_datadummyHead = new data_LinkedNode(0);
	/* 根据设定的数据个数，创建链表，默认值为0 */
	for (uint8_t i = 0; i < data_size; i++)
	{
		add_LinkedNode_AtTail(0);
	}
	/* 计算一帧数据的长度 */
	frame_length =
		+data_size * (decimals + 2)												 // 发送数据占用的长度,包含一个正负号和小数点
		+ (data_size - 1) * sizeof(frame_separator) / sizeof(frame_separator[0]) // 分隔符占得长度
		+ sizeof(frame_start) / sizeof(frame_start[0])							 // 帧头占得长度
		+ sizeof(frame_end) / sizeof(frame_end[0]);								 // 帧尾占得长度
}
```

* 输入参数为输出的串口通道，并输入每帧数据的数据量
* 词汇会根据数据量，自动创建数据链表，计算一帧的字符个数

### 3.3 帧长度检查

```cpp
/**
 * @brief    帧长度检查
 * @details  串口输出帧长度情况
 * @param
 * @return
 */
void serialdebug::check_frame_length(void)
{
	HAL_Delay(100);
	printf("the total frame_length is %d\r\n", this->frame_length);
	HAL_Delay(100);
	printf("set the length of the frame is %d\r\n", sizeof(frame) / sizeof(frame[0]));
	if (frame_length > sizeof(frame) / sizeof(frame[0]))
	{
		// frame 长度超限准备做的事
		HAL_Delay(100);
		printf("Warning! the length mast be higher\r\n");
	}
}
```

* 检查实际调用的帧数据长度和连接的数组长度，是否超量
* 需开放使用 printf，并使用串口接受

### 3.4 数据获取

```cpp
/**
 * @brief  转接数据至一帧函数
 * @details
 * @param	输入的数据，和数据在的索引，注意！！！索引从0开始
 * @retval
 */
void serialdebug::config_data(float data_in, int index)
{
	data_LinkedNode *curHead = _datadummyHead;
	for (uint8_t i = 0; i <= index; i++)
	{
		curHead = curHead->next;
	}
	curHead->data = data_in;
}
```

* 通过该函数，获取数据转移至链表中
* 输入需要转存的数据，并写明要放在的顺序索引中，即这个数据是一帧里面的第几个数据

### 3.5 发送数据

```cpp
/**
 * @brief  发送一帧数据
 * @details
 * @param
 * @retval 本质是先创建好一帧的数据，把数据值全部转化为一帧的字符串
 */
void serialdebug::send_frame(void)
{
	data_LinkedNode *curHead = _datadummyHead;
	// uint8_t frame[frame_length]; // 准备用来发送的一帧
	uint8_t frame_index = 0; // 用于记录frame的索引
	/* 添加帧头 */
	memset(frame, 0x00, sizeof(frame));
	memcpy(frame, frame_start, sizeof(frame_start));
	frame_index = frame_index + sizeof(frame_start);
	/* 大循环一次添加链表里面有的数据 转化为传输数据 */
	for (int i = 0; i < data_size; i++)
	{
		curHead = curHead->next;
		uint8_t integer_decimals = 0;
		/* 这一个数据小于0的情况 */
		if (curHead->data < 0)
		{
			/* 添加一个负号 */
			frame[frame_index] = '-';
			frame_index++;
		}
		float data_use = abs(curHead->data); // 转化为正数

		/* 提取正数为位数 */
		for (uint8_t i = 0; i < decimals; i++)
		{
			if ((uint64_t)(data_use / pow(10, i)) != 0)
			{
				integer_decimals++;
			}
		}
		/* 整数位是0的情况 */
		if (integer_decimals == 0)
		{
			frame[frame_index] = '0';
			frame_index++;
			frame[frame_index] = '.';
			frame_index++;
			for (uint8_t i = 1; i < decimals; i++)
			{
				frame[frame_index] = ((uint64_t)(data_use * pow(10, i)) % 10) + 0x30;
				frame_index++;
			}
		}
		/* 整数位有值的情况 */
		else
		{
			/* 因为已经知道了整数占得位数 直接转化为收尾为0，第一小数部位零的标准数 */
			data_use = (data_use / pow(10, integer_decimals));
			/* 提取整数位的个数作为整数 */
			for (uint8_t i = 1; i < integer_decimals + 1; i++)
			{
				frame[frame_index] = ((uint64_t)(data_use * pow(10, i)) % 10) + 0x30;
				frame_index++;
			}
			/* 整数位个数没有超过设定的位数精度，添加小数部分 */
			if (integer_decimals < decimals)
			{
				frame[frame_index] = '.';
				frame_index++;
			}
			/* 转换小数的部分 */
			for (uint8_t i = integer_decimals + 1; i < decimals + 1; i++)
			{
				frame[frame_index] = ((uint64_t)(data_use * pow(10, i)) % 10) + 0x30;
				frame_index++;
			}
		}
		/* 如果是最后一个数据，不添加间隔符 */
		if (curHead->next == NULL)
		{
			break;
		}
		else
		{
			/* 否则添加间隔符 */
			memcpy(frame + frame_index, frame_separator, sizeof(frame_separator) / sizeof(frame_separator[0]));
			frame_index = frame_index + sizeof(frame_separator) / sizeof(frame_separator[0]);
		}
	}
	/* 添加尾帧 */
	memcpy(frame + frame_index, frame_end, sizeof(frame_end) / sizeof(frame_end[0]));
	frame_index = frame_index + sizeof(frame_end) / sizeof(frame_end[0]);

	// HAL_UART_Transmit(_huart, (uint8_t *)&frame, sizeof(frame) / sizeof(frame[0]), 0xFFFF);
	HAL_UART_Transmit_DMA(_huart, (uint8_t *)&frame, sizeof(frame) / sizeof(frame[0]));
}
```

* 该函数包含了手写的将 float 转换为要求精度的字符串的算法，并生成一个发送的整体帧，最后发送
* 最后修改为 DMA 发送，取消最开始 RAM 中申明发送数组

 ## 4 自定义修改参数

1. 帧协议要求

```cpp
	uint8_t frame_start[2] = {0x2f, 0x2a}; // 帧头为 "/*"
	uint8_t frame_separator[1] = {0x20};   // 间隔符为 " "
	uint8_t frame_end[2] = {0x2a, 0x2f};   // 帧尾为 "*/"
```

2. 数据精度

```cpp
uint8_t decimals = 8;			 // 小数位数
```

* 这个是一个数据的包含数字的字符串长度，例如这里是8，如果是负数的小数，会补充为10，添加 '-' 和 '.' 




 ## 5 基本使用方法

### 5.1 默认使用方法

* 声明一个 serialdebug 类变量

```cpp
serialdebug serial_test(&huart1, 2);
```

* 包含 <font color='DeepSkyBlue'>串口通道</font>值 和<font color='DeepSkyBlue'> 数据长度</font>
* 初始化中检查数组长度是否足够

```cpp
serial_test.check_frame_length();
```

* 链接数据到链表

```cpp
serial_test.config_data(motor_1612.speed, 0);
serial_test.config_data(motor_1612.shaft_angle, 1);
```
* 生成数据帧并发送

```cpp
serial_test.send_frame();
```



 ## 6 其他注意

* 本来通过 pringf 或者 c++ 的 string 可以很方便的对浮点型转化为字符串和数据帧拼接，但是实测发现，<font color='yellow'>重定向串口后会占用20kb的flash空间，调用sting后直接占用60kb，对于f103c8来说存储根本不够</font>，因此整个过程只能手写程序完成。

 
