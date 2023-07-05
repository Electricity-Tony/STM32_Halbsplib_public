# Encoder库使用教程

## 1 日志

 * @外设相关：<font color=Red>Encoder</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                                 |                             功能                             |
   | :----------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font>   |    <font color=DeepSkyBlue>基本Cube配置与数值获取</font>     |
   | <font color='DeepSkyBlue'>1.1</font> | <font color='DeepSkyBlue'>结构化编码器结构体文件，扩展Cube计数范围不足的情况，简化数据获取方式</font> |

 ## 2 文件介绍

> | bsp_Encoder.c     | 编码器主文件     |
> | ----------------- | ---------------- |
> | **bsp_Encoder.h** | **编码器头文件** |

 ## 3 重要函数介绍

### 3.1 编码器初始化函数

```c
void bsp_Encoder_Init(void);
```

* 编码器初始化，包含开启时钟和编码器结构体与对应时钟关联，若需增添编码器须增加初始化内容

### 3.2 数据获取

```c
bsp_Encoder bsp_Encoder_Getdate(bsp_Encoder Encoder);
//使用例如
Encoder2=bsp_Encoder_Getdate(Encoder2);
```

* 单一编码器通道计数值获取，会获取输入编码器结构体的关联时钟并返回存入新值得编码器结构体

### 3.3 数据更新

```c
void bsp_Encoder_Update(void);
```

* 本质是将若干数据获取函数封装在一起，简化使用，实际使用时循环调用该函数即可

 ## 4 自定义修改参数

1. 基本额定参数

```c
#define Counter_Period 0xFFFF		//计数最大值（临界值）
#define Dir_range_check 5000    //设置过临界点使能正反转有效的范围值
```

* <font color='DeepSkyBlue'>Counter_Period：</font>由Cube中芯片确定，表示编码器模式计数位数，一般为16或者32位
* <font color='DeepSkyBlue'>Dir_range_check：</font>对正反转过计数最大的临界值得判断区间，由Counter_Period相对确定，小于Counter_Period/3即可；

2. 预设编码器使用宏定义

```c
//编码器使用定时器通道
#define bsp_Use_Encoder_TIM1 htim3
#define bsp_Use_Encoder_TIM2 htim4
```

* 已预设使用两个编码器，至使用其一可直接将其注释
* 使用是时钟通道不同可直接修改其后的时钟通道



 ## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>Tim—>Combined Channels</font>开启<font color='DeepSkyBlue'>Encoder Mod</font>
* 在<font color='DeepSkyBlue'>NVIC Settings</font>中开启时钟全局中断
* 修改芯片头文件
* 使用2个以内编码器，开关时钟宏定义及修改定时器通道
* 初始化使用<font color='DeepSkyBlue'>bsp_Encoder_Init();</font>
* 循环调用<font color='DeepSkyBlue'>bsp_Encoder_Update();</font>
* 读取全局变量<font color='DeepSkyBlue'>Encoder1</font>或<font color='DeepSkyBlue'>Encoder2</font>

### 5.2 新增编码器使用方法

* <font color='DeepSkyBlue'>bsp_Encoder.c</font>声明新的编码器结构体

```c
  //例
  bsp_Encoder Encoder3;		//声明使用编码器3的结构体
```

* <font color='DeepSkyBlue'>bsp_Encoder.h</font>中新增宏定义内容与外部声明

```c
#define bsp_Use_Encoder_TIM3 htim1
extern bsp_Encoder Encoder3;
```

* 初始化函数中新增初始化内容

```c
	HAL_TIM_Encoder_Start(&bsp_Use_Encoder_TIM3, TIM_CHANNEL_ALL);
	Encoder3.Use_TIM=bsp_Use_Encoder_TIM3;
```

* 数据更新函数中新增数据更新

```c
	Encoder3=bsp_Encoder_Getdate(Encoder3);
```



 ## 6 其他注意



 
