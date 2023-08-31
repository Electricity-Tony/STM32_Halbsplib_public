# ADC库使用教程

## 1 日志

 * @外设相关：<font color=Red>ADC</font >

   @版本：<font color=Red>1.1</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                          功能                           |
   | :--------------------------------- | :-----------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> |   <font color=DeepSkyBlue>基本Cube配置DMA配置</font>    |
   | <font color=DeepSkyBlue>1.1</font> | <font color=DeepSkyBlue>增加测不准修改的配置文件</font> |


 ## 2 文件介绍

> | bsp_adc.c      | OLED主文件       |
> | -------------- | ---------------- |
> | **bsp_adc.h**  | **OLED头文件**   |

 ## 3 重要函数介绍

### 3.1 ADC初始化函数

```c
void bsp_adc_init(void)
```

* adc初始化，使能dmp开始采样adc

### 3.2 数据清除

```c
void bsp_adc_date_clear(uint32_t adc_date[])
```

* 清除当前暂存adc数值，便于下一次进行滤波

### 3.2 数据采样

```c
void bsp_adc_date_update(uint32_t adc_date[])
```

* 采样当前adc数值，存放于暂存值<font color='DeepSkyBlue'>bsp_adc_date</font>



 ## 4 自定义修改参数

1. adc采样相关数据

```c
//设置重定向adc通道
#define fadc &hadc1               //重定向至串口1 

//adc关键数据设置
enum  __adc_date
{
	Sampling_number = 4,			//采样通道数量
	Sampling_times = 12,	//每个通道采样次数
};	
```



## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>ADC</font>开启
* 将<font color='DeepSkyBlue'>Number Of Conversion</font>数量改为实际使用的ADC通道的数量
* 使能<font color='DeepSkyBlue'>Scan Conversion Mode</font>和<font color='DeepSkyBlue'>Continuous Conversion Mode</font>和<font color='DeepSkyBlue'>Enable Regular Coversions</font>
* 在使用的若干个Rank里面分别按顺序配置采样的通道，<font color='red'>按需修改Sampling Time,不该大可能会测不准，并且通道间相互影响</font>
* 在<font color='DeepSkyBlue'>DMA Settings</font>中增加一个DAM，默认设置，Mode设置为<font color='DeepSkyBlue'>Circular</font>，Increment Address设置为Memory，Data Width均设置为<font color='DeepSkyBlue'>Word</font>
* 使用<font color='DeepSkyBlue'>HAL_ADC_Start_DMA(fadc, (uint32_t*)&bsp_adc_Value, Sampling_number\*Sampling_times)</font>开启DAM对ADC采样，数据存在bsp_adc_Value数组中，Sampling_number\*Sampling_times为这个数组存储的数据长度
* 调用<font color='DeepSkyBlue'>bsp_adc_date</font>，其中存放每一通道的adc采样值

 ## 6 其他注意

* 发现Sampling Time默认1.5Cycle会出现测不准并且通道间相互影响，修改为55.5Cycle时基本不再出现，但仍有部分静态误差，adc初始化时添加  HAL_ADCEx_Calibration_Start(fadc); 基本修正了静态误差

