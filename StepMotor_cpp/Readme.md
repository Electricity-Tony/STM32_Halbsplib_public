# Encoder库使用教程

## 1 日志

 * @外设相关：<font color=Red>bsp_StepMotor</font >

   @版本：<font color=Red>1.2</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                                 |                             功能                             |
   | :----------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font>   |     <font color=DeepSkyBlue>基本Cube配置相关说明</font>      |
   | <font color='DeepSkyBlue'>1.1</font> | <font color='DeepSkyBlue'>将PWM触发方式修改为输出比较方式，可以使同一时钟下不同端口实现不同频率的脉冲</font> |
   | <font color='DeepSkyBlue'>1.2</font> | <font color='DeepSkyBlue'>维护两次中断1次脉冲bug，新增Completed_pulse变量用于就算位置</font> |

 ## 2 文件介绍

> | bsp_StepMotor.cpp     | 步进电机主文件     |
> | --------------------- | ------------------ |
> | **bsp_StepMotor.hpp** | **步进电机头文件** |

 ## 3 重要函数介绍

### 3.1  步进电机类

```c
////******************************************* stepmotor电机类 *************************************************************************////
/**
	* @brief  stepmotor电机类
	* @param [in] DIR_Port			方向IO端号
	* @param [in]	DIR_Pin				方向IO通道号
	* @param [in]	htim					使用时钟端号
	* @param [in]	TIM_CHANNEL		使用时钟通道
	* @param [in]	Pulse_Equivalent			脉冲当量
	* @param [in]	CCR_Per			输出比较每次增加的CCR值
	*
*/
类对象声明：
stepmotor stepmotor1 (StepM1_Dir_GPIO_Port,StepM1_Dir_Pin,&htim2,TIM_CHANNEL_1,1600,1000,StepM1_EN_GPIO_Port,StepM1_EN_Pin);
```

* 对步进电机声明方向：使能端口，时钟号，通道号，脉冲当量，每次增加CCR值（用于控制频率）等

### 3.2  步进电机设置转动方向函数

```c++
WEAK void stepmotor::setDIR(uint8_t DIR)
 ●DIR(enum):
 	Positive:1;
	Reverse:0
```

### 3.3 步进电机转动函数

* 脉冲数控制

  ```c++
  WEAK void stepmotor::rotate_with_Pulse(uint8_t Direction,uint64_t Pulse)
  ●Direction(enum)://方向
   	Positive:1;
  	Reverse:0
  ● Pulse(uint64_t): //脉冲数
  ```

* 角度控制

  ```c++
  WEAK void stepmotor::rotate_with_angle(uint8_t Direction,float angle)
      //须含有脉冲当量情况下计算
  ●Direction(enum)://方向
   	Positive:1;
  	Reverse:0
  ● angle(float): //角度，°
  ```

  

### 3.4  步进电机输出比较中断更新函数

```c++
WEAK void stepmotor::TIMupdate(TIM_HandleTypeDef *htim)	//输出比较更新函数
    //将更新函数放在输出比较中断函数中
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)	//输出比较触发源中断函数
{
	stepmotor1.TIMupdate(htim);
}
```



 ## 4 自定义修改参数

### 4.1 反馈位置控制

* Completed_pulse 变量会在产生输出比较中断时自动更新，及Completed_pulse +2或者-2表示步进电机向指定方向旋转了一个脉冲
* Completed_pulse 手动赋值，用于系统复位后设置原点

```c++
WEAK void stepmotor::set_Completed_pulse(int64_t Completed_pulse)
●Completed_pulse(int64_t)://赋值半脉冲数，大部分情况是偶数
```



 ## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>Tim—>Clock Sourse</font>选择<font color='DeepSkyBlue'>Internal Clock内部时钟源</font>
* Cube配置中在<font color='DeepSkyBlue'>Tim—>ChannelX</font>开启<font color='DeepSkyBlue'>Output Compare CHX</font>
* <font color='DeepSkyBlue'>Prescaler分频系数</font>按需设置，直接影响该时钟上所有通道的周期情况
* <font color='DeepSkyBlue'>Counter Mode计数方式</font>选择<font color='DeepSkyBlue'>Up向上计数</font>
* <font color='DeepSkyBlue'>Counter Period重装载数</font>设置为16位上限65535，该值大于单个通道CCR增加值的最大值即可
* <font color='DeepSkyBlue'>通道模式Mode</font>选择<font color='DeepSkyBlue'>Toggle on match</font>,该选项在每次输出比较中断时会反转io电平
* 在<font color='DeepSkyBlue'>NVIC Settings</font>中开启时钟全局中断，高级定时器开启<font color='DeepSkyBlue'>capture compare interrupt</font>
* 修改芯片头文件
* 根据使用的时钟，IO等修改IO名及时钟使用
* 程序开始运行初始化函数step_init()
* 在void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)中添加更新函数TIMupdate(htim)
* 自由调用旋转函数即可



 ## 6 其他注意

 ### 6.1 app应用层

* 本次（V1.2）更新新增 app_motor 应用层 ，可通过bsp层进行相关操作
