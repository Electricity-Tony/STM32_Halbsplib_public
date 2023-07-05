/**
 * @file bsp_memory.cpp
 * @brief memory板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2022-4-7
 * @copyright 
 * @par 日志:
 *   V1.0 基本配置相关说明
 */		
 
 
// 局部数组大小受限 请修改 Stack_Size（栈）大小 
 
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_memory.hpp"

/* 私有类型定义 --------------------------------------------------------------*/

#define WEAK __attribute__((weak)) //使用WEAK类型是方便来重构特定函数
#define	ABS(x)   ((x)>0?(x):-(x))

using namespace std;

int16_t motor_speed_date[2500];
queue motor_speed_FIFO(motor_speed_date,2500);

int16_t R_ADC_date[2500];
queue R_ADC_FIFO(R_ADC_date,2500);




////******************************************* 队列queue(FIFO) 类 *************************************************************************////
/**
	* @brief  queue类
	* @param [in] maxsize			int16_t类型数据存储最大值

	*
*/
WEAK queue::queue(int16_t data_link[],uint16_t maxsize)
{
	this->data = data_link;
	this->maxsize = maxsize;
	this->front = 0;
	this->rear = 0;
	this->counter = 0;
}

/**
  * 函数功能: 判断队列是否为空
  * 输入参数:
	*	返 回 值: 1:空   0:非空
  * 说    明：
  */
WEAK uint8_t queue::check_empty(void)
{
	if(this->counter == 0)
	{
		return 1;
	}
	else return 0;
}


/**
  * 函数功能: 判断队列是否为满
  * 输入参数:
	*	返 回 值: 1:满   0:非满
  * 说    明：
  */
WEAK uint8_t queue::check_full(void)
{
	if(this->counter >= maxsize)
	{
		return 1;
	}
	else return 0;
}

/**
  * 函数功能: 元素入队列
  * 输入参数:	入队列元素
	*	返 回 值: 1:满   0:非满
  * 说    明：
  */
WEAK uint8_t queue::push(int16_t element)
{
	if(check_full())
	{
		return 1;
	}
	else
	{
		this->data[this->rear] = element;
		this->rear = (this->rear + 1) % this->maxsize;
		this->counter++;
		return 0;
	}
}

/**
  * 函数功能: 元素出队列
  * 输入参数:	获得出元素值的存储变量
	*	返 回 值: 1:空   0:非空
  * 说    明：
  */
WEAK uint8_t queue::pop(int16_t* out_element)
{
	if(check_empty())
	{
		return 1;
	}
	else
	{
		*out_element = this->data[this->front];
		this->front = (this->front + 1) % this->maxsize;
		this->counter--;
		return 0;
	}
}






