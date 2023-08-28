
/**
 * @file bsp_Key.cpp
 * @brief Key板级支持包
 * @author Tony_Wang
 * @version 1.2
 * @date 2022-3-11
 * @copyright
 * @par 日志:
 *   V1.0 基本Cube配置与基本设置
 *	 V1.1 重构为C++库，派生各类Key，并可用系统时钟调用长按
 *	 V1.2 Button长按修改为不松开仍然生效
 */

#include "bsp_key.hpp"

#define WEAK __attribute__((weak)) // 使用WEAK类型是方便特殊电机来重构特定函数
#define MICROS_ms() HAL_GetTick()  // 计时，单位ms,除1000是秒

Button USER_Button(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, 3000);

/*****			自锁类按键构造函数				******/
WEAK Key::Key(GPIO_TypeDef *GPIO_Port, uint16_t Pin)
{
	this->GPIO_Port = GPIO_Port;
	this->Pin = Pin;
}
/**
 * @brief  读取当前按键的按下状态
 * @details
 * @param
 * @retval  读取自锁类按键状态
 */
void Key::read()
{
	Key::State = HAL_GPIO_ReadPin(Key::GPIO_Port, Key::Pin);
}

/*****			派生类按键构造函数				******/
WEAK Button::Button(GPIO_TypeDef *GPIO_Port, uint16_t Pin, uint16_t time_check)
{
	this->GPIO_Port = GPIO_Port;
	this->Pin = Pin;
	this->time_check = time_check;
}

/**
 * @brief  按键按下时长检测
 * @details  按键按下，松开视为结束
 * @param  更新Button类输出状态State：1、0（二值类），2（长按）
 */
void Button::update()
{
	this->read(); // Button::read();这个函数也可

#ifdef MICROS_ms								   // 使用内置时钟控制长按判断
	if (this->State == 0 && this->Last_State == 1) // 表示按键按下瞬间
	{
		this->time_flag = MICROS_ms(); // 记录按下时的时刻
		this->Last_State = this->State;
	}
	else if (this->State == 1 && this->Last_State == 0) // 表示按键松开瞬间
	{
		this->time_flag = MICROS_ms() - this->time_flag; // 计算按下期间的时间
		this->Last_State = this->State;

		if (this->time_check < this->time_flag) // 长按成立
		{
			this->Button_State = 2;
		}

		else if (this->time_check > this->time_flag && this->time_flag > 10) // 短按成立
		{
			if (this->Button_State == 0)
				this->Button_State = 1;
			else
				this->Button_State = 0;
		}

		else
		{
		} // 抖动情况，不处理
	}

#else
	// 按键状态判断，长按判断中间态输出，在Ture_State中存放
	// 0表示没有按下或处于长按中间态中，1表示短按成立，2表示长按成立
	// 默认Now_State为低电平表示按下
	if (this->time_flag > this->time_check)
	{
		// 长按时间标志位成立，长按成立
		if (this->State == 1)
			this->time_flag = 0;
		this->Ture_State = 2;
	}
	else if (this->State == 0)
	{
		// 长按中间态，长按时间标志位累增
		this->time_flag++;
		this->Ture_State = 0;
	}
	else if (10 < this->time_flag && this->time_flag < this->time_check)
	{
		// 短按成立
		this->time_flag = 0;
		this->Ture_State = 1;
	}
	else
	{
		this->time_flag = 0;  // 清空长按标志位
		this->Ture_State = 0; // 一直没有按下
	}

	// 判断按键状态变化，默认二值态，短按反相
	if (this->Button_State != 1 && this->Ture_State == 1)
	{
		this->Button_State = 1;
	}
	else if (this->Button_State != 0 && this->Ture_State == 1)
	{
		this->Button_State = 0;
	}
	else if (this->Ture_State == 2)
	{
		this->Button_State = 2;
	}
#endif
}
