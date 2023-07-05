# SERVO库使用教程

## 1 日志

 * @外设相关：<font color=Red>SERVO  舵机</font >

   @版本：<font color=Red>1.2</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             功能                             |
   | :--------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> |   <font color=DeepSkyBlue>建立伺服（舵机）bsp级驱动</font>   |
   | <font color=DeepSkyBlue>1.1</font> | <font color=DeepSkyBlue>对角度，初始化等进行修正，现可直接使用</font> |
   | <font color=DeepSkyBlue>1.2</font> | <font color=DeepSkyBlue>修正为c++库，建立基类pwm单向电机</font> |

 ## 2 文件介绍

> | **bsp_servo.cpp** | 舵机库主文件     |
> | ----------------- | ---------------- |
> | **bsp_servo.hpp** | **舵机库头文件** |

 ## 3 重要函数介绍

### 3.1 engine基类

#### 3.1.1 初始化

* 包含时钟及端口初始化，并可以实现自适应频率调整（无需进入cube进行修改），初始化是输入frequency变量即可，不输入时不进行修改

```c++
////******************************************* pwm电机类（engine）*************************************************************************////
/**
	* @brief  pwm电机类（engine）
	* @param [in]   htim			使用时钟端号
	* @param [in]	TIM_CHANNEL		使用时钟通道
	* @param [in]	frequency		调整时钟频率，为0时程序内不会进行调整
	*
*/
WEAK void engine::init(TIM_HandleTypeDef htim, uint32_t TIM_CHANNEL, uint16_t frequency)
{
	this->htim = htim;
	this->TIM_CHANNEL = TIM_CHANNEL;
	this->frequency = frequency;

	uint32_t timclockfreq = 0;
	timclockfreq = HAL_RCC_GetHCLKFreq();	//获得当前时钟主频率
	autoreload = __HAL_TIM_GET_AUTORELOAD(&htim);	//获取当前重装载值
	if (frequency != 0)	//表示cube端没有进行更改 且 设置对频率进行了修改
	{
		if (htim.Instance == TIM1)
		{
			timclockfreq = timclockfreq / 2;
		}

		uint32_t timtemp = timclockfreq / frequency;	//分频数与计数值乘机的暂存数
		float fretemp = frequency / 5;				//频率计算暂存数
		uint32_t pretemp = 0;						//分频系数暂存值

		//获取适合的分频系数
		if (fretemp < 1)
		{
			pretemp = timclockfreq / 10000;
		}
		else if (fretemp < 100)
		{
			pretemp = timclockfreq / 100000;
		}
		else
		{
			pretemp = timclockfreq / 1000000;
		}
		__HAL_TIM_SET_PRESCALER(&htim, pretemp - 1);
		autoreload = timtemp / pretemp;

		__HAL_TIM_SET_AUTORELOAD(&htim, autoreload);
	}
	HAL_TIM_PWM_Start(&htim, TIM_CHANNEL);
}
```

#### 3.1.2 pwm比较值输入函数

* 直接更改该时钟通道比较值，及TIMx-->CRR1

```c++
/**
	* @brief  engine设置比较计数值
	* @param [in]   比较值计数值
	* @retval
	* @par 日志
*
*/
WEAK void engine::compare_set(uint16_t compare_value)
{
	compare = compare_value;
	if (compare <= autoreload)
	{
		__HAL_TIM_SET_COMPARE(&htim, TIM_CHANNEL, compare);
	}
}
```

#### 3.1.3 以千分制形式更改占空比

```c++
/**
	* @brief  engine设置比较值函数
	* @param [in]   目标比较值(千分比)
	* @retval
	* @par 日志
*
*/
WEAK void engine::speed_set(uint16_t speed)
{
	this->speed = speed;
	this->compare = (float)this->autoreload * speed / 1000;
	//if (compare <= autoreload)
	//{
	//	__HAL_TIM_SET_COMPARE(&htim, TIM_CHANNEL, this->compare);
	//}
	this->compare_set(compare);

}
```



 ### 3.2 servo舵机派生类

#### 3.2.1 servo初始化

* 舵机初始化强制frequency为50Hz

```c++
////******************************************* Servo电机类*************************************************************************////
/**
	* @brief  Servo电机类
	* @param [in]   htim			使用时钟端号
	* @param [in]	TIM_CHANNEL		使用的时钟通道
	* @param [in]	AngleModel		舵机机械性能上可旋转的角度
	* @param [in]	Zero_Pulse		作为0度的标准角度计数值
	* @param [in]	Std_Pulse		没有命令常规状态的标准角度计数值
	* @param [in]	Direction		正方向及现性放大倍数
	* @param [in]	UPlimit_Pulse		舵机计数值上限
	* @param [in]	DOWNlimit_Pulse		舵机计数值下限
	*
*/
WEAK void servo::init(TIM_HandleTypeDef htim, uint32_t TIM_CHANNEL, 
	uint16_t AngleModel, uint16_t Zero_Pulse, uint16_t Std_Pulse, int8_t Direction,  uint16_t UPlimit_Pulse, uint16_t DOWNlimit_Pulse)
{
	this->htim = htim;
	this->TIM_CHANNEL = TIM_CHANNEL;

	this->AngleModel = AngleModel;
	this->Zero_Pulse = Zero_Pulse;
	this->Std_Pulse = Std_Pulse;
	this->Direction = Direction;
	this->UPlimit_Pulse = UPlimit_Pulse;
	this->DOWNlimit_Pulse = DOWNlimit_Pulse;

	frequency = 50;

	uint32_t timclockfreq = 0;
	timclockfreq = HAL_RCC_GetHCLKFreq();	//获得当前时钟主频率
	autoreload = __HAL_TIM_GET_AUTORELOAD(&htim);	//获取当前重装载值
	if (frequency != 0)	//表示cube端没有进行更改 且 设置对频率进行了修改
	{
		if (htim.Instance == TIM1)
		{
			timclockfreq = timclockfreq / 2;
		}

		uint32_t timtemp = timclockfreq / frequency;	//分频数与计数值乘机的暂存数
		float fretemp = frequency / 5;				//频率计算暂存数
		uint32_t pretemp = 0;						//分频系数暂存值

		//获取适合的分频系数
		if (fretemp < 1)
		{
			pretemp = timclockfreq / 10000;
		}
		else if (fretemp < 100)
		{
			pretemp = timclockfreq / 100000;
		}
		else
		{
			pretemp = timclockfreq / 1000000;
		}
		__HAL_TIM_SET_PRESCALER(&htim, pretemp - 1);
		autoreload = timtemp / pretemp;

		__HAL_TIM_SET_AUTORELOAD(&htim, autoreload);
	}
	HAL_TIM_PWM_Start(&htim, TIM_CHANNEL);
}
```

#### 3.2.2 设置舵机占空比数

* 50~250表示0.5ms-2.5ms

 ```c++
 /**
	 * @brief  servo设置计数值
	 * @param [in]   50~250
	 * @retval
	 * @par 日志
 *
 */
WEAK void servo::pulse_set(uint16_t pulse_value)
{
	if (pulse_value >= DOWNlimit_Pulse && pulse_value <= UPlimit_Pulse)
	{
		this->compare_set(pulse_value);
	}
}
 ```




 #### 3.2.2 工作函数
* servo角度控制

```c++
/**
	* @brief  servo旋转绝对角度值
	* @param [in]   旋转至绝对角度,单位0.1度
	* @retval
	* @par 日志
*
*/
WEAK void servo::angle_set(int16_t angle_value)
{
	float PulsePerAngle = 20 / (float)(AngleModel);			//每0.1度的计数值
	Pulse = angle_value * PulsePerAngle * Direction + Zero_Pulse;
	this->pulse_set(Pulse);
}
```





 ## 4 自定义修改参数

### 4.1 芯片头文件

 * 添加芯片头文件修改

* 各个舵机的基本参数修改


 ## 5 基本使用方法

1. Cube配置TIM，IO相关设置，设置<font color='DeepSkyBlue'>周期为20ms</font>，计数值为2000
2. 添加bsp_servo.cpp，bsp_servo.hpp文件至工程
3. 常规设置为适配C++文件
4. 声明变量<font color='DeepSkyBlue'>engine</font>或者<font color='DeepSkyBlue'>servo</font>，调用相关函数
5. 将各个电机进行初始化
6. 将各个，使用绝对角度控制舵机
7. 通过<font color='DeepSkyBlue'>bsp_Servo_Set_AbsoluteAngle</font>，使用绝对角度控制舵机

 

 ## 6 其他注意

* 每个电机需要进行初始化，切必须在MX_TIM初始化之后
