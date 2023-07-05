/**
 * @file bsp_led.cpp
 * @brief led板级支持包
 * @author Tony_Wang
 * @version 1.1
 * @date 2023-6-19
 * @copyright
 * @par 日志:
 *   V1.0 基本Cube配置与基本设置,使用C结构体编写
 *	 V1.1 重构为C++库，使用系统时钟调用闪烁
 *
 */

#include "bsp_led.hpp"

#define WEAK __attribute__((weak)) // 使用WEAK类型是方便特殊电机来重构特定函数
#define MICROS_ms() HAL_GetTick()  // 计时，单位ms,除1000是秒

Led BORAD_LED(BORAD_LED_GPIO_Port, BORAD_LED_Pin);

/*****			自锁类按键构造函数				******/
WEAK Led::Led(GPIO_TypeDef *GPIO_Port, uint16_t Pin)
{
	this->GPIO_Port = GPIO_Port;
	this->Pin = Pin;
}
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
