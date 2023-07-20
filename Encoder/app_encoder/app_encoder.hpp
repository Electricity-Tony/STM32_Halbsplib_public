/**
 * @file app_Encoder.cpp
 * @brief Encoder 应用级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-7-6
 * @copyright
 * @par 日志:
 * 		V1.0 分离原本脉冲编码器bsp_Encoder库，脱离为不受硬件显示的计算库
 * 		V1.1 新增速度获取函数
 */

#ifndef __APP_Encoder_HPP__
#define __APP_Encoder_HPP__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "systick/bsp_systick.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/

/* encoder 类函数声明 ----------------------------------------------------------------*/
class encoder
{
public:
	float count;		  // 当前临界内计数值,这个值必须恒为正数
	float date;			  // 总计数值，作为全局定位编码器值
	float speed;		  // 编码器速度 单位 弧度/s
	float counter_period; // 完整一圈的计数值

	typedef float (*float_func_none)(void); // d定义一类无输入，float 输出的函数
	float_func_none get_count_link;			// 获取计数值连接函数

	// 成员函数
	encoder(void){};
	encoder(float counter_period, float_func_none get_count_link);
	float get_count(void);							 // 获得计数值函数
	float get_date(void);							 // 获得总计数值
	void set_TurnCheckRange(float Turn_Check_range); // 临界范围设置
	float get_speed(void);							 // 获取速度

protected:
	// 保护成员函数
	int8_t Turn_Check;			  // 转过临界标志位
	int16_t Turn;				  // 过临界周转圈数
	float Turn_Check_range = 0.1; // 转过临界的判断范围
	float time_prev;			  // 速度计算的上一次暂存时间
	float date_prev;			  // 速度计算上一次的暂存数据
};

/* 外部声明变量 */
extern encoder HallEncoder;

#endif
