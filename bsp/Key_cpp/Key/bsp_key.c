
#include "bsp_key.h"


_KeyTypedef key_up = {key_up_GPIO_Port,key_up_Pin,0,0,1,0,1000};
_KeyTypedef key_down = {key_down_GPIO_Port,key_down_Pin,0,0,1,0,1000};

/**
* @brief  读取当前按键的按下状态
* @details  
* @param  _KeyTypedef* Key(按键标准结构体)
* @retval  更新按键结构体状态
*/
void bsp_key_Read(_KeyTypedef* Key)
{
	//按键当前状态输出，在Now_State中存放，表示电平情况
	Key->Now_State = HAL_GPIO_ReadPin(Key->GPIO_Port,Key->Pin);
}


/**
* @brief  按键按下时长检测
* @details  按键按下，松开视为结束
* @param  _KeyTypedef* Key(按键标准结构体)
*/
void bsp_key_State_Update(_KeyTypedef* Key)
{
	bsp_key_Read(Key);


	//按键状态判断，长按判断中间态输出，在Ture_State中存放
	//0表示没有按下或处于长按中间态中，1表示短按成立，2表示长按成立
	//默认Now_State为低电平表示按下
	if(Key->time_flag > Key->time_check)
	{
		//长按时间标志位成立，长按成立
		Key->time_flag=0;
		Key->Ture_State = 2;
	}
	if(Key->Now_State==0) 
	{
		//长按中间态，长按时间标志位累增
		Key->time_flag++;
		Key->Ture_State = 0;
	}
	else if(10< Key->time_flag && Key->time_flag < Key->time_check)
	{
		//短按成立
		Key->time_flag=0;
		Key->Ture_State = 1;
	}
	else Key->Ture_State = 0;	//一直没有按下

	//判断按键状态变化，默认二值态，短按反相
	if (Key->State != 1 && Key.Ture_State == 1)
	{
		Key->State = 1;
	}
	else if (Key->State != 0 && Key.Ture_State == 1)
	{
		Key->State = 0;
	}
	else if (Key->State.Ture_State == 2)
	{
		Key->State = 2;
	}


	
}


