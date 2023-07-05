# OLED库使用教程

## 1 日志

 * @外设相关：<font color=Red>OLED</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                          功能                           |
   | :--------------------------------- | :-----------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>基本Cube配置基本显示函数</font> |


 ## 2 文件介绍

> | bsp_OLEDc      | OLED主文件       |
> | -------------- | ---------------- |
> | **bsp_OLED.h** | **OLED头文件**   |
> | **oledfont.h** | **OLED字库文件** |

 ## 3 重要函数介绍

### 3.1 OLED初始化函数

```c
void OLED_Init(void);
```

* OLED初始化，包含各类设置，清空缓存等

### 3.2 命令发送

```c
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void OLED_WR_Byte(unsigned dat, unsigned cmd);
```

* 向OLED发送数据或命令，会根据使用硬件或者软件I2C自行切换



 ## 4 自定义修改参数

1. 硬件和软件I2C切换

```c
#define USE_HARDWARE_I2C	//使用硬件I2C，若使用模拟I2C请注释
```

> (1). 硬件I2C使用通道
>
> ```c#
> #define OLED_hi2c hi2c1	//OLED使用的硬件I2C通道
> ```
>
> (2). 软件I2C使用引脚
>
> ```c#
> #define OLED_SCLK_Clr() HAL_GPIO_WritePin(I2C_SCL_GPIO_Port,I2C_SCL_Pin,GPIO_PIN_RESET);//SCL
> #define OLED_SCLK_Set() HAL_GPIO_WritePin(I2C_SCL_GPIO_Port,I2C_SCL_Pin,GPIO_PIN_SET);
> 
> #define OLED_SDIN_Clr() HAL_GPIO_WritePin(I2C1_SDA_GPIO_Port,I2C1_SDA_Pin,GPIO_PIN_RESET)//SDA
> #define OLED_SDIN_Set() HAL_GPIO_WritePin(I2C1_SDA_GPIO_Port,I2C1_SDA_Pin,GPIO_PIN_SET)
> ```
>
> * 可直接在main.h中将对应的IO名称复制过来，或者直接在Cube中修改为程序中的名称

2. 可能的参数更改

```c
#define Oled_address 0x78	//OLED的地址可能会变
```



## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>I2C</font>开启
* 在<font color='DeepSkyBlue'>NVIC Settings</font>中开启<font color='DeepSkyBlue'>I2C</font>全局中断
* 修改芯片头文件
* 初始化使用<font color='DeepSkyBlue'>OLED_Init();</font>

 ## 6 其他注意

* 目前可使用的0.96的OLED，其他型号需要更新相关库



 
