# SERVO库使用教程

## 1 日志

 * @外设相关：<font color=Red>SERVO  舵机</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             功能                             |
   | :--------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> |   <font color=DeepSkyBlue>建立伺服（舵机）bsp级驱动</font>   |
   | <font color=DeepSkyBlue>1.1</font> | <font color=DeepSkyBlue>对角度，初始化等进行修正，现可直接使用</font> |

 ## 2 文件介绍

> | **bsp_servo.c** | 舵机库主文件     |
> | --------------- | ---------------- |
> | **bsp_servo.h** | **舵机库头文件** |

 ## 3 重要函数介绍

 ### 3.1 初始化函数
* 单个舵机初始化
```c
/**
 * @brief 单独servo_Bottom初始化
* @param 
 */
void bsp_Servo_Bottom_Init(void)
{
    //舵机标准结构体各个参数设置
	Servo_Bottom.htim=htim2;
	Servo_Bottom.TIM_CHANNEL=TIM_CHANNEL_3;
	Servo_Bottom.Rotatable_angle=270;
	Servo_Bottom.Zero_Pulse=140;
	Servo_Bottom.Std_Pulse=140;
	Servo_Bottom.Pulse=Servo_Bottom.Std_Pulse;
	Servo_Bottom.Direction=1;
	Servo_Bottom.UPlimit_Pulse=140;
	Servo_Bottom.DOWNlimit_Pulse=50;
	//舵机开启使能，初始角度命令
	HAL_TIM_PWM_Start(&Servo_Bottom.htim,Servo_Bottom.TIM_CHANNEL);
	bsp_Servo_Set_Pulse(&Servo_Bottom);
}
```

* servo初始化

 ```c
void bsp_All_Servo_Init(void)
{
    //中间放各个舵机初始化的函数
	bsp_Servo_Bottom_Init();
	bsp_Servo_Middle_Init();
	bsp_Servo_Claw_Init();
}					//使能舵机时钟及通道，转至标准位置
    
 ```




 ### 3.2 工作函数
* servo角度通过自身结构体数据更新

```c
/**
 * @brief servo角度通过自身结构体数据计数值更新
* @param 标准舵机结构体
 */
void bsp_Servo_Set_Pulse(ServoTypeDef* Servo)
{
	if(Servo->Pulse>=Servo->DOWNlimit_Pulse && Servo->Pulse<=Servo->UPlimit_Pulse)	//判断目标值是否在限幅范围内
	{
        //在限幅范围内允许舵机更新角度
		__HAL_TIM_SET_COMPARE(&(Servo->htim), Servo->TIM_CHANNEL,Servo->Pulse);	
	}
}			//自身数据输出至舵机
```

* 舵机绝对角度值控制

```c
/**
* @brief 设定舵机旋转绝对角度值，原点为标准结构够Zero_Pulse
* @param 标准舵机结构体，旋转至绝对角度,单位0.1度
 */
void bsp_Servo_Set_AbsoluteAngle(ServoTypeDef* Servo,int16_t Angle)
{
	float PulsePerAngle=20/(float)(Servo->Rotatable_angle);			//每0.1度的计数值
	Servo->Pulse=Angle*PulsePerAngle*Servo->Direction + Servo->Zero_Pulse;
	bsp_Servo_Set_Pulse(Servo);
	
}
```



 ## 4 自定义修改参数

### 4.1 芯片头文件

 * 添加芯片头文件修改

* 各个舵机的基本参数修改


 ## 5 基本使用方法

1. Cube配置TIM，IO相关设置，设置<font color='DeepSkyBlue'>周期为20ms</font>，计数值为2000
2. 添加bsp_servo.c，bsp_servo.h文件至工程
3. 声明<font color='DeepSkyBlue'>ServoTypeDef</font>作为一个标准舵机，新写设置舵机标准角度位置，上下限幅，使用时钟通道等的函数
4. 将新增的单个舵机初始化函数放至<font color='DeepSkyBlue'>bsp_All_Servo_Init</font>
5. 主函数前使用<font color='DeepSkyBlue'>bsp_All_Servo_Init</font>，对所有舵机进行初始化
6. 通过<font color='DeepSkyBlue'>bsp_Servo_Set_AbsoluteAngle</font>，使用绝对角度控制舵机

 

 ## 6 其他注意

* 结构体作为输入变量进入函数并需要对结构体进行修改，须传入地址进行结构体操作

 
