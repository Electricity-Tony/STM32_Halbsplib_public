# Encoder库使用教程

## 1 日志

 * @外设相关：<font color=Red>StepMotor</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                                 |                        功能                         |
   | :----------------------------------- | :-------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font>   | <font color=DeepSkyBlue>基本Cube配置相关说明</font> |
   | <font color='DeepSkyBlue'>1.1</font> |          <font color='DeepSkyBlue'></font>          |

 ## 2 文件介绍

> | bsp_StepMotor.c     | 步进电机主文件     |
> | ------------------- | ------------------ |
> | **bsp_StepMotor.h** | **步进电机头文件** |

 ## 3 重要函数介绍

### 3.1  步进电机初始化函数

```c
void bsp_StepMotor_Init(void)
```

* 初始设定步进电机方向，50%占空比，开启使能，不命令转动

### 3.2  步进电机脉冲命令转动函数

```c
uint8_t bsp_StepMotor_TurnPulse(uint8_t StepMotor_ID,uint8_t Direction,uint16_t Pulse)
```

* 输入需要转动步进电机ID号、转动方向、脉冲步数，此后该步进电机步进电机向该方向转动目标脉冲度数，仅发送一次即可，循环发送回一直转动

### 3.3  步进电机PWM中断回调函数

```c
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
```

* PWM中断回调函数，内部封装对目标脉冲的命令，对买一次脉冲计数，达到目标脉冲后关闭PWM输出

 ## 4 自定义修改参数

1. 步进电机电器相关

```c
#define StepMotor1_ID 0x01						//步进电机1的ID号
#define StepMotor1_TIM htim1					//步进电机1的时钟号
#define StepMotor1_CH TIM_CHANNEL_3		//步进电机1的时钟通道
#define StepMotor1_Period 1000				//步进电机1的每周期计数值
```

* 使用步进电机1的相关宏定义



 ## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>Tim—>ChannelX</font>开启<font color='DeepSkyBlue'>PWM Generation CHX</font>
* <font color='DeepSkyBlue'>Trigger Event Selection</font>选择<font color='DeepSkyBlue'>Update Event</font>
* <font color='DeepSkyBlue'>Output compare preload</font>选择<font color='DeepSkyBlue'>Enable</font>
* 在<font color='DeepSkyBlue'>NVIC Settings</font>中开启时钟全局中断，高级定时器开启<font color='DeepSkyBlue'>capture compare interrupt</font>
* 修改芯片头文件
* 根据使用的时钟，IO等修改IO名及时钟使用，见**4 自定义修改参数**
* 初始化使用<font color='DeepSkyBlue'>bsp_StepMotor_Init();</font>
* 单次调用<font color='DeepSkyBlue'>bsp_StepMotor_TurnPulse();</font>



 ## 6 其他注意

* AT2100驱动标明O1B、O1A、O2B、O2A。其中<font color='red'>一定是数字决定相！！！</font><font color='DeepSkyBlue'>即2相4线步进电机A+与A-可接在O1B、O1A，B+与B-可接在O2B、O2A。</font><font color='red'>绝不是字母决定相！！！</font>
* AT2100驱动发热严重，建议最好增加散热片

 
