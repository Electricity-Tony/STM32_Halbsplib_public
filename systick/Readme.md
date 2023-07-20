# systick库使用教程

## 1 日志

 * @外设相关：<font color=Red>systick</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             更新时间                             |功能|
   | :--------------------------------- | :----------------------------------------------------------: | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>2023-7-11</font> |<font color=DeepSkyBlue>基本构建，用于实现单片机内部滴答定时器时间计算</font>|



 ## 2 文件介绍

> | bsp_systick.c     | 主文件     |
> | ----------------- | ---------- |
> | **bsp_systick.h** | **头文件** |

 ## 3 重要函数介绍

### 3.1 ms 获取

```cpp
/* 获取 ms 级别的定时时间 */
#define MICROS_ms() HAL_GetTick() // 计时，单位ms,除1000是秒
```

* 获取毫秒时间

### 3.2 foc 类介绍

```cpp
/* getCurrentMicros()函数用于获取自MCU复位以来的运行时间，单位微秒，
	这个函数的实质还是使用了系统滴答定时器，
	只是在HAL_GetTick()函数的基础上使用了SysTick的当前计数值SysTick->VAL，
	根据这个数据和SysTick->LOAD重装载值的关系即可计算出不足1毫秒的那一部分时间，
	从而得到准确的以微秒为单位的时间。 */
/* 计时时间移除后会归零 */
uint64_t MICROS_us(void)
{
	// 我们需要记录老的时间值，因为不保证这个函数被调用的期间SysTick的中断不会被触发。
	static uint64_t OldTimeVal;
	uint64_t NewTimeVal;

	// 新的时间值以Tick计数为毫秒部分，以SysTick的计数器值换算为微秒部分，获得精确的时间。
	NewTimeVal = (HAL_GetTick() * 1000) + (SysTick->VAL * 1000 / SysTick->LOAD);

	// 当计算出来的时间值小于上一个时间值的时候，说明在函数计算的期间发生了SysTick中断，此时应该补正时间值。
	if (NewTimeVal < OldTimeVal)
		NewTimeVal += 1000;
	OldTimeVal = NewTimeVal;

	// 返回正确的时间值
	return NewTimeVal;
}
```

* 获取 us 时间



 ## 4 自定义修改参数



 ## 5 基本使用方法

### 5.1 默认使用方法

* 导入 <font color='DeepSkyBlue'>bsp_systick</font> 包后，调用时间获取函数即可




 ## 6 其他注意



