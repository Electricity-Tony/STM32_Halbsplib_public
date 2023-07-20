/**
 * @file bsp_serialstudio.hpp
 * @brief serialstudio 应用级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2023-7-17
 * @copyright
 * @par 日志:
 *   V1.0 成功解读 serialstudio 通讯协议，移植到stm单片机
 *
 */

#ifndef __BSP_SERIALSTUDIO_HPP__
#define __BSP_SERIALSTUDIO_HPP__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/

/* serialdebug 串口绘图类声明---------------------------------------------------------------- */
class serialdebug
{

public:
	/* 声明链表类，用来管理数据 */
	struct data_LinkedNode
	{
		float data;										  // 节点上存储的元素
		data_LinkedNode *next;							  // 指向下一个节点的指针
		data_LinkedNode(float x) : data(x), next(NULL) {} // 节点的构造函数
		data_LinkedNode(float x, data_LinkedNode *next) : data(x), next(next) {}
	};

	UART_HandleTypeDef *_huart; // 使用的串口通道
	uint16_t data_size;			// 一帧数据长度

	// 成员函数
	serialdebug(UART_HandleTypeDef *_huart, uint16_t data_size);
	// ~serialdebug();
	void add_LinkedNode_AtTail(int val);		// 在末尾添加一个节点
	void config_data(float data_in, int index); // 转接发送的数据至data[]数组
	void send_frame(void);						// 发送一帧数据

protected:
	// 保护成员变量
	uint8_t frame_start[2] = {0x2f, 0x2a}; // 帧头为 "/*"
	uint8_t frame_separator[1] = {0x20};   // 间隔符为 " "
	uint8_t frame_end[2] = {0x2a, 0x2f};   // 帧尾为 "*/"

private:
	data_LinkedNode *_datadummyHead; // ；链表虚拟头结点
	uint8_t frame_length;			 // 发送一帧数据的字符串长度
	uint8_t decimals = 8;			 // 小数位数
};

/* 外部声明变量 */

extern serialdebug serial_test;

#endif
