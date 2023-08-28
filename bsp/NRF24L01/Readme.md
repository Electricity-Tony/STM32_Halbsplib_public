# NRF24L01库使用教程

## 1 日志

 * @外设相关：<font color=Red>NRF24L01</font >

   @版本：<font color=Red>1.1</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                                 |                             功能                             |
   | :----------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font>   | <font color=DeepSkyBlue>规范化NRF24L01收发驱动库，实现基本功能</font> |
   | <font color='DeepSkyBlue'>1.1</font> | <font color='DeepSkyBlue'>修正检测是否在线多次检测，修改RX获得包的状态位判断，修改IO设定方式</font> |

 ## 2 文件介绍

> | **bsp_NRF24L01.c** | **NRF24L01的spi读写与初始化**          |
> | ------------------ | -------------------------------------- |
> | **bsp_NRF24L01.h** | **NRF24L01相关寄存器define与函数声明** |

 ## 3 重要函数介绍

 ### 3.1 自检函数 
* NRF24L01自检函数，返回1成功
 ```c
uint8_t NRF24L01_Check(void);						//检查24L01是否存在
 ```

* NRF24L01工作模式配置，两模式只能同时存在一种情况

```c
void NRF24L01_RX_Mode(void);					//配置为接收模式
void NRF24L01_TX_Mode(void);					//配置为发送模式
```



 ### 3.2 工作函数
 * NRF24L01接收/发送数据包函数，输入变量均为32长度的uint8_t的数组，接收已声明全局变量 uint8_t NRF_RX_date[33]

```c
uint8_t NRF24L01_TxPacket(uint8_t *txbuf);				//发送一个包的数据
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf);				//接收一个包的数据
```



 ## 4 自定义修改参数

### 4.1 芯片头文件

 * 添加芯片头文件修改：

 * > <font color=DeepSkyBlue>bsp_NRF24L01.h</font>添加头文件下修改为自己芯片型号的头文件

### 4.2 IO修改

* <font color=DeepSkyBlue>bsp_NRF24L01.h</font>下修改NRF24L01配置与使能IO配置

```c#
#define NRF24L01_SPIx                                 SPI2
#define NRF24L01_SPIx_RCC_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define NRF24L01_SPIx_RCC_CLK_DISABLE()               __HAL_RCC_SPI2_CLK_DISABLE()
    
#define NRF24L01_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_RESET)
#define NRF24L01_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_SET)
    
#define NRF24L01_CE_LOW()                             HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET)
#define NRF24L01_CE_HIGH()                            HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET)
    
#define NRF24L01_IRQ_PIN_READ()                       HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port,NRF_IRQ_Pin)
```

### 4.3 自动应答设定

* <font color=DeepSkyBlue>bsp_NRF24L01.c</font>下选择是否注释使用自动应答宏定义

```c#
//自动应答，不使用自动应答时请注释
#define EN_NRF_ACK
```

### 4.4 设置通讯地址

* <font color=DeepSkyBlue>bsp_NRF24L01.c</font>下修改静态声明函数内的值设定需要的地址，目前已声明通道0地址。
* 注：通道地址0和通道地址1可以为任意地址，通道地址2~5高4字节必须与通道地址1相同，即最左的一个字节，且通道2~5只能做接收

```c#
const uint8_t TX0_ADDRESS[TX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01}; //设置发送接收地址
const uint8_t RX0_ADDRESS[RX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01};


//例如：
const u8 RX0_ADDRESS[5]={0x10,0x10,0x10,0x10,0xAA}; //接收地址0
const u8 RX1_ADDRESS[5]={0x11,0x10,0x10,0x10,0xFF}; //接收地址1
const u8 RX2_ADDRESS[5]={0x12,0x10,0x10,0x10,0xFF}; //接收地址2
const u8 RX3_ADDRESS[5]={0x13,0x10,0x10,0x10,0xFF}; //接收地址3
const u8 RX4_ADDRESS[5]={0x14,0x10,0x10,0x10,0xFF}; //接收地址4
const u8 RX5_ADDRESS[5]={0x15}; //接收地址5
```



 ## 5 基本使用方法

  1. Cube配置spi，IO相关设置，<font color='red'>SPI设定最大速度不能超过10M，IRQ设置为浮空</font>
 2. 添加bsp_NRF24L01.c，bsp_NRF24L01.h文件至工程
  3. 使用NRF24L01_Check(void)对NRF24L01进行自检，返回0则成功
 4. 设定NRF24L01工作模式：NRF24L01_TX_Mode(void)，NRF24L01_RX_Mode(void)
   5. 使用对应的发送/接收函数包完成工作，NRF24L01_TxPacket（）输入长度不超过32的uchar型数组，NRF24L01_RxPacket（）输入已声明变量NRF_RX_date[]，不断获取更新的接收值。

 

 ## 6 其他注意

* 两个模块相互通信是只有地址完全相同，频道完全相同才可以实现通讯
* 某一模块只能同时处于接收或者发送状态，因此主程序需要轮训才能实现半双工通讯，且容易丢包
* 目前暂未开发通过中断方式从发送模式进入接收模式，目前资料显示<font color=yellow>IRQ的下拉能力太弱了会被io控偶的上拉电阻强制拉高，也就是说产生IRQ中断由于上拉电阻影响IRQ回一直置高（巨坑）。补救方法：将io配合为浮空输入即可完美解决</font> ，具体中断方式实现待维护。
* <font color='red'>切换为接收模式后需要有一定间隔时间</font>（测试为100ms以上），在进行接收数据包获取，留足24L01获得包的时间

 
