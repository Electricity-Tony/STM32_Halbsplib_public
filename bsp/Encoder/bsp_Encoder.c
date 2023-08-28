/**
 * @file bsp_Encoder.c
 * @brief Encoder板级支持包
 * @author Tony_Wang
 * @version 1.1
 * @date 2020-10-10
 * @copyright 
 * @par 日志:
 *   V1.0 基本Cube配置与数值获取
 *	 V1.1 结构化编码器结构体文件，扩展Cube计数范围不足的情况，简化数据获取方式
 */		
 
 
/* 包含头文件 ----------------------------------------------------------------*/
#include "tim.h"
#include "bsp_Encoder.h"
/* 私有类型定义 --------------------------------------------------------------*/

#ifdef bsp_Use_Encoder_TIM1
bsp_Encoder Encoder1;		//声明使用编码器1的结构体
#endif // bsp_Use_Encoder_TIM1

#ifdef bsp_Use_Encoder_TIM2
bsp_Encoder Encoder2;		//声明使用编码器2的结构体
#endif // bsp_Use_Encoder_TIM2

/**
  * 函数功能: 初始化编码器
  * 输入参数: 
  * 返 回 值: 
  * 说    明：无
  */
void bsp_Encoder_Init(void)
{
	#ifdef bsp_Use_Encoder_TIM1
	HAL_TIM_Encoder_Start(&bsp_Use_Encoder_TIM1, TIM_CHANNEL_ALL);
	Encoder1.Use_TIM=bsp_Use_Encoder_TIM1;
	#endif // bsp_Use_Encoder_TIM1

	#ifdef bsp_Use_Encoder_TIM2
	HAL_TIM_Encoder_Start(&bsp_Use_Encoder_TIM2, TIM_CHANNEL_ALL);
	Encoder2.Use_TIM=bsp_Use_Encoder_TIM2;
	#endif // bsp_Use_Encoder_TIM2
}


bsp_Encoder bsp_Encoder_Getdate(bsp_Encoder Encoder)
{
	Encoder.CaptureNumber = __HAL_TIM_GET_COUNTER(&Encoder.Use_TIM);

	if (Encoder.CaptureNumber >= 0 && Encoder.CaptureNumber <= Dir_range_check)
	{
		Encoder.Reverse_Check = Enable;
		if (Encoder.Forward_Check == Enable) Encoder.Turn++;
	}
	if (Encoder.CaptureNumber >= Counter_Period - Dir_range_check && Encoder.CaptureNumber <= Counter_Period)
	{
		Encoder.Forward_Check = Enable;
		if (Encoder.Reverse_Check == Enable) Encoder.Turn--;
	}
	else Encoder.Forward_Check = Disable;
	if(Encoder.CaptureNumber <= Counter_Period && Encoder.CaptureNumber > Dir_range_check) Encoder.Reverse_Check=Disable;

	Encoder.Encoder_date = Encoder.Turn * Counter_Period + Encoder.CaptureNumber;
	
	return Encoder;
}

void bsp_Encoder_Update(void)
{
	#ifdef bsp_Use_Encoder_TIM1
	Encoder1=bsp_Encoder_Getdate(Encoder1);
	#endif	//bsp_Use_Encoder_TIM1
	
	#ifdef bsp_Use_Encoder_TIM2
	Encoder2=bsp_Encoder_Getdate(Encoder2);
	#endif	//bsp_Use_Encoder_TIM2
}




