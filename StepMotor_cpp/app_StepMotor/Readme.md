# Encoder库使用教程

## 1 日志

 * @外设相关：<font color=Red>app_StepMotor</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             功能                             |
   | :--------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>基于bsp_StepMotor开发的应用层</font> |
   |                                    |                                                              |
   |                                    |                                                              |

 ## 2 文件介绍

> | app_StepMotor.cpp     | 步进电机主文件     |
> | --------------------- | ------------------ |
> | **app_StepMotor.hpp** | **步进电机头文件** |

* 调用此app库时须搭配Key.cpp库使用

 ## 3 重要函数介绍

### 3.1  二维平台控制

#### 3.1.1 坐标转换

```c
//电机减速比宏定义
#define Gear_ratio_1 0.0888889	//	80mm对应步进电机1旋转360度，另加外置减速比2.5
#define Gear_ratio_2 0.2222222			//	80mm对应步进电机1旋转360度

void app_StepMotor_position_convert(void)//利用 Completed_pulse 变量实现的坐标变换
```

#### 3.1.2 光电门复位触发

```c
uint8_t app_StepMotor_reset(stepmotor* stepmotor_in,uint8_t DIR,Key* Key_in)
    //进行复位操做时须通过 while(function) 进行阻塞式执行
● stepmotor_in(stepmotor*)://步进电机类
● DIR(enum)://复位时电机转动方向
 	Positive:1;
	Reverse:0
● Key_in(Key*): //判断复位情况的光电门Key类
> output: uint8_t
    1://光电门未被触发,复位未完成
	2：//光电门触发，重复执行电机不会再转动
```

#### 3.1.3 转动距离控制

```c
void app_StepMotor_rotate_with_Length(stepmotor* stepmotor_in,float Gear_ratio,float Length)
    //此处为转动相对距离，若重复执行则会不停旋转，该函数为非阻塞式
● stepmotor_in(stepmotor*)://步进电机类
● Gear_ratio(float)://电机对应直线上的减速比
● Length(float): //距离参数（mm），区分正负，代表方向
```

#### 3.1.4 坐标闭环控制

```c
void app_StepMotor_move_with_position(float Target_x,float Target_y)
    //此为绝对坐标控制，复位后须完成 Completed_pulse 清零 
● Target_x(float)://x坐标位置
● Target_y(float)://y坐标位置
```

#### 3.1.5 按键手动控制

```c
void app_StepMotor_button_Ctrl_position(void)
    //通过读取按键状态调用 app_StepMotor_move_with_position 进行移动
    //建议在定时器中循环调用该函数
```



 ## 4 自定义修改参数

### 4.1 行程软限位

```c
#define x_max 760
#define x_min 0
#define y_max 0
#define y_min -500
```



 ## 5 基本使用方法

### 5.1 默认使用方法

> 以下为 bsp_StepMotor 配置文件

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
