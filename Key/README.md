# Key库使用教程

## 1 日志

 * @外设相关：<font color=Red>Key</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                         功能                          |
   | :--------------------------------- | :---------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>基本Cube配置与基本设置</font> |


 ## 2 文件介绍

> | bsp_Key.c     | 按键主文件     |
> | ------------- | -------------- |
> | **bsp_Key.h** | **按键头文件** |

 ## 3 重要函数介绍

### 3.1 按键结构体构造

```c
typedef struct {
	GPIO_TypeDef* GPIO_Port;	//按键结构体端口号
	uint16_t Pin;				//按键结构体引脚号
	uint8_t State				//实际需要调用的状态
	uint8_t Ture_State;			//按键表示的状况标致,用于长按时间段内标致判断
	uint8_t Now_State;			//实时当时的按键按下情况
	uint16_t time_flag;			//长按判断标志位
	uint16_t time_check			//长按时间判断域
}_KeyTypedef;
```

* 对每一个按键进行设置构造

### 3.2 按键当前值获取

```c
void bsp_key_Read(_KeyTypedef* Key)
```

* 直接更新当前按键结构体当前值

### 3.3 逻辑可用状态值更新

```c
void bsp_key_State_Update(_KeyTypedef* Key)
```

* 对按键可用值进行更新，存放在State中

 ## 4 自定义修改参数




 ## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>GPIO_OUTPUT</font>
* 设置用户自定义名称
* 声明需要的按键结构体并进行初始赋值
* 循环调用<font color='DeepSkyBlue'>bsp_key_State_Update();</font>
* 需要对逻辑值进行判断时调用结构体的<font color='DeepSkyBlue'>State</font>




 ## 6 其他注意



 
