# encoder库使用教程

## 1 日志

 * @外设相关：<font color=Red>encoder</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             功能                             |
   | :--------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>分离原本脉冲编码器bsp_Encoder库，脱离为不受硬件显示的计算库</font> |
   | <font color=DeepSkyBlue>1.1</font> |       <font color=DeepSkyBlue>新增速度获取函数</font>        |

 ## 2 文件介绍

> | app_encoder.cpp     | 编码器主文件     |
> | ------------------- | ---------------- |
> | **app_encoder.hpp** | **编码器头文件** |

 ## 3 重要函数介绍

### 3.1 编码器类 encoder

```cpp
class encoder
{
public:
	float count;		  // 当前临界内计数值,这个值必须恒为正数
	float date;			  // 总计数值，作为全局定位编码器值
	float counter_period; // 完整一圈的计数值

	typedef float (*float_func_none)(void); // d定义一类无输入，float 输出的函数
	float_func_none get_count_link;			// 获取计数值连接函数

	// 成员函数
	encoder(void){};
	encoder(float counter_period, float_func_none get_count_link);
	float get_count(void); // 获得计数值函数
	float get_data(void);  // 获得总计数值

	

protected:
	// 保护成员函数
	int8_t Turn_Check; // 转过临界标志位
	int16_t Turn;	   // 过临界周转圈数
	float Turn_Check_range = 0.1; // 转过临界的判断范围
};

/**
 * @brief  encoder 编码器类
 * @param [in]   counter_period	完整一圈的计数值
 * @param [in]	get_count_link	获取计数值连接函数
 *
 */
WEAK encoder::encoder(float counter_period, float_func_none get_count_link)
{
	this->counter_period = counter_period;
	this->get_count_link = get_count_link;
}
```

* 整合编码器内变量，其中 count 当前编码器读取的值， date 表示计算得到的从运行开始一直累计的编码器值，换言之，count 一个周期后就归零了，date会一直记录
* 中间的链接函数为定义的一种输入函数，通过链接到 bsp 库中的读取函数来更新类的值，做到更高的兼容性
* **Turn_Check_range** 为判读正反转的区间范围，取值为(0 , 0.5) ，发现两次数据更新时间过长，导致没识别到周期改变，适当增大范围

### 3.2 数据获取

```cpp
 * @brief  get_count 获取计数值函数
 * @param 	[in]   	counter_period    	完整一圈的计数值
 * @param 	[in]    get_count_link    	获取计数值连接函数
 * @retval	[out]	count				当前计数值
 */
float encoder::get_count(void)
{
	count = get_count_link();
	return count;
}
```

* 通过链接函数获取 count，链接函数满足 无输入变量 ， float 输出

### 3.3 数据更新

```cpp
/**
 * @brief  get_data 获取计数值函数
 * @param
 * @param
 * @retval	[out]	date				总计数值
 */
float encoder::get_date(void)
{
	/* 读取一下现在的 count */
	get_count();
	/* 判断过临界周转标志 */
	if (count <= (counter_period * Turn_Check_range))
	{
		/* 判断是否正转成立 */
		if (Turn_Check == 1)
		{
			Turn++; // 圈数加一
		}
		Turn_Check = -1;
	}
	else if (count >= (counter_period * (1 - Turn_Check_range)))
	{
		/* 判断是否正转成立 */
		if (Turn_Check == -1)
		{
			Turn--; // 圈数加一
		}
		Turn_Check = 1;
	}
	else
	{
		Turn_Check = 0;
	}
	date = Turn * counter_period + count;
	return date;
}
```

* 通过读取 count 计算 date

### 3.4 速度获取

```cpp
/**
 * @brief  获取速度函数
 * @details	获得转速值，单位 弧度/s
 * @param
 * @retval 转速值
 */
float encoder::get_speed(void)
{
	get_date();
	/* 计算采样时间 */
	float Ts = (MICROS_us() - time_prev) * 1e-6; // seconds
	/* 修复移除的情况 */
	if (Ts <= 0)
		Ts = 1e-3f;
	/* 速度计算 */
	speed = (date - date_prev) / Ts;
	/* 更新暂存变量 */
	date_prev = date;
	time_prev = Ts;
	return speed;
}
```

* 调用该函数获取计算的角速度，单位  弧度/s

 ## 4 自定义修改参数

1. 基本额定参数

```cpp
counter_period; // 完整一圈的计数值
get_count_link;			// 获取计数值连接函数
Turn_Check_range = 0.1; // 转过临界的判断范围
```

* <font color='DeepSkyBlue'>counter_period：</font>编码器一个周期的值，底层封装为角度值就是 360 ，弧度值就是 2Pi ， 16位编码器不进行数据解析就是 65535 等等
* <font color='DeepSkyBlue'>get_count_link：</font>链接函数 ， 声明的时候把链接的函数名输入就行
* <font color='DeepSkyBlue'>Turn_Check_range：</font>临界范围设置，通过 <font color = "deepskyblue">set_TurnCheckRange</font> 设置




 ## 5 基本使用方法

### 5.1 默认使用方法

* 声明一个 encoder 类变量

```cpp
encoder HallEncoder(360, ReadAngle);
```

* 包含 <font color='DeepSkyBlue'>周期</font>值 和<font color='DeepSkyBlue'> 链接函数</font>
* 单独可以使用 <font color='DeepSkyBlue'>get_count</font> 获取周期内采样值
* 获取 <font color='DeepSkyBlue'>date</font> 计算值

```cpp
HallEncoder.get_date();
```
* 获取 <font color='DeepSkyBlue'>speed</font> 角速度值

```cpp
HallEncoder.get_date();
```



 ## 6 其他注意



 
