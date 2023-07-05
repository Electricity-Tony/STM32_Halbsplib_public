# Led库使用教程

## 1 日志

 * @外设相关：<font color=Red>Led</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             功能                             |
   | :--------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>基本Cube配置与基本设置,使用C结构体编写</font> |



 ## 2 文件介绍

> | bsp_led.c     | led主文件     |
> | ------------- | ------------- |
> | **bsp_led.h** | **led头文件** |

 ## 3 重要函数介绍

### 3.1 led 结构体构造

```c
typedef struct
{
	GPIO_TypeDef *GPIO_Port;  // LED结构体端口号
	uint16_t Pin;			  // LED结构体引脚号
	uint8_t State;			  // 实时的LED情况
	uint16_t time_flag_start; // 闪烁时间标志位开始
	uint16_t time_flag_now;	  // 闪烁时间标志位当前
	uint16_t time_on;		  // 单词LED亮起的时间ms
	uint16_t check_times;	  // 闪烁次数判断域
	uint16_t on_times;		  // 闪烁次数设置
} _LedTypedef;

```

* 对每一个led进行设置构造

### 3.2 对 led 进行开关

```c
/**
* @brief  设置LED亮灭
* @details
* @param  _LedTypedef* Led(LED标准结构体)，uint8_t State（状态设置）
* @retval  
*/
void bsp_led_Operate(_LedTypedef* Led, uint8_t State)
{
	//按键当前状态输出，在Now_State中存放，表示电平情况
	Led->State = State;
	HAL_GPIO_WritePin(Led->GPIO_Port, Led->Pin, State);
}
```

* 改变 led 输出状态

### 3.3 led 闪烁函数

```c
/**
* @brief  LED闪烁设置
* @details  
* @param  _LedTypedef* Led(LED标准结构体)
*/
void bsp_led_twinkle(_LedTypedef* Led)
{
	
	if (Led->time_flag_now > Led->time_on)
	{//进入一次时间周期
		if (Led->check_times < Led->on_times)
		{//处于闪烁次数内
			if (Led->State == LED_ON)
			{//当前led为亮
				bsp_led_Operate(Led, LED_OFF);
				Led->check_times++;
			}
			else
			{//当前状态为灭
				bsp_led_Operate(Led, LED_ON);
			}
		}
		else if (Led->check_times < Led->on_times * 2.5)
		{//处于熄灭次数内
			Led->check_times++;
		}
		else
		{//总周期完成
			Led->check_times = 0;
		}
		Led->time_flag_now = 0;
	}
	Led->time_flag_now++;
}
```

* 通过结构体内的参数，设置闪烁参数
* 本函数是通过不断循环运行进行闪烁时间估计，通过循环数判定的





 ## 4 自定义修改参数




 ## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>GPIO_OUTPUT</font>
* 设置用户自定义名称
* 声明需要的 led 结构体并进行初始赋值
* 循环调用<font color='DeepSkyBlue'>bsp_led_twinkle</font>




 ## 6 其他注意



 
