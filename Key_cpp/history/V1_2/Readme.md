# Key库使用教程

## 1 日志

 * @外设相关：<font color=Red>Key</font >

   @版本：<font color=Red>1.2</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             功能                             |
   | :--------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> |    <font color=DeepSkyBlue>基本Cube配置与基本设置</font>     |
   | <font color=DeepSkyBlue>1.1</font> | <font color=DeepSkyBlue>重构为C++库，派生各类Key，并可用系统时钟调用长按</font> |
   | <font color=DeepSkyBlue>1.2</font> | <font color=DeepSkyBlue>Button长按修改为不松开仍然生效</font> |


 ## 2 文件介绍

> | bsp_Key.cpp     | 按键主文件     |
> | --------------- | -------------- |
> | **bsp_Key.hpp** | **按键头文件** |

 ## 3 重要函数介绍

### 3.1 基类 Key

#### 3.1.1 Key结构大纲

```c++
/*****			自锁类按键构造函数				******/
class Key
{
	public:
		GPIO_TypeDef* GPIO_Port;	//按键结构体端口号
		uint16_t Pin;				//按键结构体引脚号
    
   		uint8_t State;			//实时当时的按键按下情况
    
		//成员函数
		Key(void){};
		Key(GPIO_TypeDef* GPIO_Port, uint16_t Pin);	
		void read(void);
	protected:
};
```

* 对每一个按键进行设置构造，内置IO端口和引脚号，可直接使用构造函数构造
* 设定为可以自锁类开关，只有按下与没按下两种状态

#### 3.1.2 按键当前值获取

```c++
void Key::read()
```

* 直接更新当前按键当前状态值

### 3.2 派生类Button

```c++
/*****			派生类按键构造函数				******/
class Button:public Key
{
	public:
		uint16_t time_check;			//长按时间判断域
		uint8_t Button_State;				//实际需要调用的状态	
		//成员函数
		Button(GPIO_TypeDef* GPIO_Port, uint16_t Pin,uint16_t time_check);
		void update(void);
	protected:	
	private:
		//运算储存区
		uint32_t time_flag;		//长按判断标志位
		uint8_t Ture_State;		//按键表示的状况标致,用于长按时间段内标致判断,目前仅由未使用内置时钟使用
		uint8_t Last_State;		//上一次检测状态
};
```

* 设定为不可自锁类按键开关，出现短按反转状态与长按 共三种状态

#### 3.2.3 逻辑可用状态值更新

```c++
void Button::update()
```

* 对按键可用值进行更新，存放在State中
* 长按检测函数使用的是内置 systick 时钟，单位ms

 ## 4 自定义修改参数




 ## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>GPIO_INPUT</font>，设置为上拉。pull_up
* 设置用户自定义名称
* 声明需要的按键结构体并进行初始赋值
* 循环调用<font color='DeepSkyBlue'>Key.read() 或 Button.update</font>
* 需要对逻辑值进行判断时调用变量的<font color='DeepSkyBlue'>State 或 Button_State</font>




 ## 6 其他注意



 
