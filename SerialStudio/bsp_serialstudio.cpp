/**
 * @file bsp_serialstudio.cpp
 * @brief serialstudio 应用级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-7-17
 * @copyright
 * @par 日志:
 *   V1.0 成功解读 serialstudio 通讯协议，移植到stm单片机
 *
 */

/* 协议内容
 * 协议包括：帧头，分割字符，帧尾
 * 由于串口重定向会占用很多flash ，所以需要编写一个不利用 printf 的库
 * 帧样例 /*1.34572892 7654.9835 -25.7865368 0.0003871*/
 /**/

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_serialstudio.hpp"
// #include "Usart/bsp_usart.h"
#include "usart.h"
#include <string.h>
#include <math.h>

/* 私有类型定义 --------------------------------------------------------------*/

#define WEAK __attribute__((weak)) // 使用WEAK类型是方便特殊电机来重构特定函数

serialdebug serial_test(&huart1, 2);

////******************************************* serialdebug 串口绘图类声明*************************************************************************////
/**
 * @brief  serialdebug 串口绘图类声明
 * @param [in]   *_huart	上位机的串口
 * @param [in]	data_size	发送的数据个数
 *
 */

WEAK serialdebug::serialdebug(UART_HandleTypeDef *_huart, uint16_t data_size)
{
	this->_huart = _huart;
	this->data_size = data_size;
	this->_datadummyHead = new data_LinkedNode(0);
	/* 根据设定的数据个数，创建链表，默认值为0 */
	for (uint8_t i = 0; i < data_size; i++)
	{
		add_LinkedNode_AtTail(0);
	}
	/* 计算一帧数据的长度 */
	frame_length =
		+data_size * (decimals + 2)												 // 发送数据占用的长度,包含一个正负号和小数点
		+ (data_size - 1) * sizeof(frame_separator) / sizeof(frame_separator[0]) // 分隔符占得长度
		+ sizeof(frame_start) / sizeof(frame_start[0])							 // 帧头占得长度
		+ sizeof(frame_end) / sizeof(frame_end[0]);								 // 帧尾占得长度
}

/**
 * @brief    在链表尾部添加节点
 * @details
 * @param    val     添加链表节点数据
 * @return
 */
void serialdebug::add_LinkedNode_AtTail(int val)
{
	data_LinkedNode *addHead = new data_LinkedNode(val);
	data_LinkedNode *curHead = _datadummyHead;
	while (curHead->next != NULL)
	{
		curHead = curHead->next;
	}
	curHead->next = addHead;
}

/**
 * @brief  转接数据至一帧函数
 * @details
 * @param	输入的数据，和数据在的索引，注意！！！索引从0开始
 * @retval
 */
void serialdebug::config_data(float data_in, int index)
{
	data_LinkedNode *curHead = _datadummyHead;
	for (uint8_t i = 0; i <= index; i++)
	{
		curHead = curHead->next;
	}
	curHead->data = data_in;
}

/**
 * @brief  发送一帧数据
 * @details
 * @param
 * @retval 本质是先创建好一帧的数据，把数据值全部转化为一帧的字符串
 */
void serialdebug::send_frame(void)
{
	data_LinkedNode *curHead = _datadummyHead;
	uint8_t frame[frame_length]; // 准备用来发送的一帧
	uint8_t frame_index = 0;	 // 用于记录frame的索引
	/* 添加帧头 */
	memset(frame, 0x00, sizeof(frame));
	memcpy(frame, frame_start, sizeof(frame_start));
	frame_index = frame_index + sizeof(frame_start);
	/* 大循环一次添加链表里面有的数据 转化为传输数据 */
	for (int i = 0; i < data_size; i++)
	{
		curHead = curHead->next;
		uint8_t integer_decimals = 0;
		/* 这一个数据小于0的情况 */
		if (curHead->data < 0)
		{
			/* 添加一个负号 */
			frame[frame_index] = '-';
			frame_index++;
		}
		float data_use = abs(curHead->data); // 转化为正数

		/* 提取正数为位数 */
		for (uint8_t i = 0; i < decimals; i++)
		{
			if ((uint64_t)(data_use / pow(10, i)) != 0)
			{
				integer_decimals++;
			}
		}
		/* 整数位是0的情况 */
		if (integer_decimals == 0)
		{
			frame[frame_index] = '0';
			frame_index++;
			frame[frame_index] = '.';
			frame_index++;
			for (uint8_t i = 1; i < decimals; i++)
			{
				frame[frame_index] = ((uint64_t)(data_use * pow(10, i)) % 10) + 0x30;
				frame_index++;
			}
		}
		/* 整数位有值的情况 */
		else
		{
			/* 因为已经知道了整数占得位数 直接转化为收尾为0，第一小数部位零的标准数 */
			data_use = (data_use / pow(10, integer_decimals));
			/* 提取整数位的个数作为整数 */
			for (uint8_t i = 1; i < integer_decimals + 1; i++)
			{
				frame[frame_index] = ((uint64_t)(data_use * pow(10, i)) % 10) + 0x30;
				frame_index++;
			}
			/* 整数位个数没有超过设定的位数精度，添加小数部分 */
			if (integer_decimals < decimals)
			{
				frame[frame_index] = '.';
				frame_index++;
			}
			/* 转换小数的部分 */
			for (uint8_t i = integer_decimals + 1; i < decimals + 1; i++)
			{
				frame[frame_index] = ((uint64_t)(data_use * pow(10, i)) % 10) + 0x30;
				frame_index++;
			}
		}
		/* 如果是最后一个数据，不添加间隔符 */
		if (curHead->next == NULL)
		{
			break;
		}
		else
		{
			/* 否则添加间隔符 */
			memcpy(frame + frame_index, frame_separator, sizeof(frame_separator) / sizeof(frame_separator[0]));
			frame_index = frame_index + sizeof(frame_separator) / sizeof(frame_separator[0]);
		}
	}
	/* 添加尾帧 */
	memcpy(frame + frame_index, frame_end, sizeof(frame_end) / sizeof(frame_end[0]));
	frame_index = frame_index + sizeof(frame_end) / sizeof(frame_end[0]);

	HAL_UART_Transmit(_huart, (uint8_t *)&frame, sizeof(frame) / sizeof(frame[0]), 0xFFFF);
}
