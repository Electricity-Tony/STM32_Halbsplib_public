# Led库使用教程

## 1 日志

 * @外设相关：<font color=Red>Led</font >

   @版本：<font color=Red>1.1</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             功能                             |
   | :--------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>基本Cube配置与基本设置,使用C结构体编写</font> |
   | <font color=DeepSkyBlue>1.1</font> | <font color=DeepSkyBlue>重构为C++库，使用系统时钟调用闪烁</font> |


 ## 2 文件介绍

> | bsp_led.cpp     | led主文件     |
> | --------------- | ------------- |
> | **bsp_led.hpp** | **led头文件** |

 ## 3 重要函数介绍

### 3.1 led 类 构造

```cpp
/*****			GPIO_OUT led 构造函数				******/
class Led
{
public:
	GPIO_TypeDef *GPIO_Port; // 按键结构体端口号
	uint16_t Pin;			 // 按键结构体引脚号

	LED_State State; // 实时当时的按键按下情况

	// 成员函数
	Led(void){};
	Led(GPIO_TypeDef *GPIO_Port, uint16_t Pin);
	void twinkle_Config(uint16_t twinkle_time, uint16_t twinkle_times);
	void ctrl(LED_State State);
	void twinkle_update();

protected:
	uint16_t time_flag_start; // 闪烁时间标志位开始
	uint16_t time_flag_now;	  // 闪烁时间标志位当前
	uint16_t twinkle_time;	  // 单词LED亮起的时间ms
	uint16_t twinkle_count;	  // 闪烁次数判断域
	uint16_t twinkle_times;	  // 闪烁次数设置
};
```

* 对每一个led进行设置构造

### 3.2 对 led 进行开关

```cpp
/**
 * @brief  设置LED亮灭
 * @details
 * @param  State :状态设置
 * @retval
 */
void Led::ctrl(LED_State State)
{
	// 按键当前状态输出，在Now_State中存放，表示电平情况
	this->State = State;
	if (this->State == LED_ON)
	{
		HAL_GPIO_WritePin(this->GPIO_Port, this->Pin, GPIO_PIN_RESET);
	}
	else if (this->State == LED_OFF)
	{
		HAL_GPIO_WritePin(this->GPIO_Port, this->Pin, GPIO_PIN_SET);
	}
}
```

* 改变 led 输出状态

### 3.3 led 闪烁参数配置函数

```cpp
/**
 * @brief  设置 Led 闪烁参数
 * @details
 * @param	twinkle_time	:闪烁时间设置ms （不能超过20s）
 * @param	twinkle_times	:闪烁次数设置
 * @retval
 */
void Led::twinkle_Config(uint16_t twinkle_time, uint16_t twinkle_times)
{
#if twinkle_time > 20000
#warning twinkle_time cant greater than 20000
#endif
	this->twinkle_time = twinkle_time;
	this->twinkle_times = twinkle_times;
}
```

* 设定单次闪烁时间和闪烁次数



### 3.4 闪烁更新函数

```cpp
/**
 * @brief  LED 闪烁更新函数
 * @details
 * @param
 * @retval
 */
void Led::twinkle_update()
{
	this->time_flag_now = MICROS_ms(); // 记录按下时的时刻

	int32_t time_err = this->time_flag_now - this->time_flag_start;
	// 此时移除爆值
	if (time_err < 0)
	{
		time_err = time_err + 65535;
	}

	if (time_err > this->twinkle_time)
	{ // 进入一次时间周期
		if (this->twinkle_count < this->twinkle_times)
		{ // 处于闪烁次数内
			if (this->State == LED_ON)
			{ // 当前led为亮
				ctrl(LED_OFF);
				this->twinkle_count++;
			}
			else
			{ // 当前状态为灭
				ctrl(LED_ON);
			}
		}
		else if (this->twinkle_count < this->twinkle_times * 2.5)
		{ // 处于熄灭次数内
			this->twinkle_count++;
		}
		else
		{ // 总周期完成
			this->twinkle_count = 0;
		}
		this->time_flag_start = this->time_flag_now;
	}
}		
```

* 该函数放置在一个重复调用函数中，周期远小于闪烁时间就行

 ## 4 自定义修改参数




 ## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>GPIO_OUTPUT</font>
* 设置用户自定义名称
* 声明需要的 led 类并进行初始赋值
* 循环调用<font color='DeepSkyBlue'>Led::twinkle_update();</font>




 ## 6 其他注意



 
