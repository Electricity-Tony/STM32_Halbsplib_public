
#include "bsp_led.h"


_LedTypedef Board_Led = { BORAD_LED_GPIO_Port,BORAD_LED_Pin,0,0,0,200,0,3 };



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


/**
* @brief  LED初始化
* @details  关闭所有led
* @param
* @retval

*/
void bsp_led_Init(void)
{
	bsp_led_Operate(&Board_Led, LED_OFF);
}



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


