# BMP280库使用教程

## 1 日志

 * @外设相关：<font color=Red>BMP280</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                            功能                            |
   | :--------------------------------- | :--------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>构建bmp280spi计算气压温度库</font> |


 ## 2 文件介绍

> | **bsp_bmp280.c** | **bmp280的spi读写与计算**            |
> | ---------------- | ------------------------------------ |
> | **bsp_bmp280.h** | **bmp280相关寄存器define与函数声明** |

 ## 3 重要函数介绍

 ### 3.1 自检函数 
* bmp280自检函数，返回0成功
 ```c
uint8_t bsp_bmp280_Check(void);						//检查bmp280ID是否正确，正确为0x58，返回0
 ```

### 3.2 计算函数

* bmp280计算函数

 ```c
double bsp_bmp280_GetTemperature(void);				//计算温度值（0.01度）
double bsp_bmp280_GetPressure(void);				//计算气压值（帕）
 ```

 ### 3.3 初始化函数
 * 初始化话包含两部分
 * 第一部分包括读取bmp280出厂数据，用于计算相关计算值
 * 第二部分包括设置bmp280相关模式，通过修改<font color='DeepSkyBlue'>BMP280_CONTROL,BMP280_CONFIG</font>，更改其设置

 ```c
/**
* @brief 气压计初始化函数
*/
void bsp_bmp280_init(void)
{
	//微调数据测量
	dig.t1 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_T1);
	dig.t2 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_T2);
	dig.t3 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_T3);
	dig.p1 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P1);
	dig.p2 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P2);
	dig.p3 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P3);
	dig.p4 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P4);
	dig.p5 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P5);
	dig.p6 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P6);
	dig.p7 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P7);
	dig.p8 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P8);
	dig.p9 = bsp_bmp280_ReadReg_trimming(BMP280_REGISTER_DIG_P9);
	
	//bsp_bmp280_Reset();
	//控制数据写入
	uint8_t BMP280_CONTROL,BMP280_CONFIG;
	//0XF4控制地址写入数据，设置压力分辨率，温度分辨率，运行模式
	BMP280_CONTROL = BMP280_BITS_PRESS_FS_4|BMP280_BITS_TEMP_FS_1|BMP280_BITS_POWMOD_NORMAL;
	//0XF4控制地址写入数据，设置维持时间，滤波器系数，SPI3/4线模式
	BMP280_CONFIG = BMP280_BITS_T_SB1|BMP280_BITS_CONFIG_FS_0|BMP280_BITS_SPI3W_DISANLE;
	
	//写入
	bsp_bmp280_WriteReg(BMP280_REGISTER_CONTROL,BMP280_CONTROL);
	bsp_bmp280_WriteReg(BMP280_REGISTER_CONTROL,BMP280_CONFIG);
 ```


 ## 4 自定义修改参数

### 4.1 芯片头文件

 * 添加芯片头文件修改：

 * > <font color=DeepSkyBlue>bsp_bmp280.h</font>添加头文件下修改为自己芯片型号的头文件

### 4.2 IO修改

* <font color=DeepSkyBlue>bsp_bmp280.c</font>下修改spi通道和片选io

```c#
//需要自己转接的3个宏定义
#define BMP280_SPI hspi1
#define BMP280_Disable()   HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_SET)
#define BMP280_Enable()    HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_RESET)
```



 ## 5 基本使用方法

  1. Cube配置spi，IO相关设置，<font color='red'>SPI设定最大速度不能超过10M</font>
 2. 添加bsp_bmp280.c，bsp_bmp280.h文件至工程
  3. 使用bsp_bmp280_Check(void)对bmp280进行自检，返回0则成功
   5. 初始化bmp280，并设置需要的相关模式等
     5. 循环调用<font color='DeepSkyBlue'>bsp_bmp280_GetTemperature</font>和<font color='DeepSkyBlue'>bsp_bmp280_GetPressure</font>更新气压值与温度值

 

 ## 6 其他注意

* 目前两种计算方式，位移计算和浮点计算，浮点计算已确定可用，位移计算还未验证
* 气压计算需要使用温度计算的中间值，因此调用气压值时一定要更新温度值

 
