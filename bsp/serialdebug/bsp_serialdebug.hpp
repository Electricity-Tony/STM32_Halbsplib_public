/**
 * @file bsp_serialdebug.cpp
 * @brief serialdebug 应用级支持包
 * @author Tony_Wang
 * @version 2.0
 * @date 2023-12-19
 * @copyright
 * @par 日志:
 *   V1.0 成功解读 serialstudio 通讯协议，移植到stm单片机
 *   V1.1 修改缓存区为连接外部数组，改为串口DMA发送
 *
 * 	 V2.0 增加适配Vofa工具组功能，可选适配FireWater和JustFloat协议，更名为bsp_serialdebug,注：索引号发生改变！！！
 *
 */

#ifndef __BSP_SERIALDEBUG_HPP__
#define __BSP_SERIALDEBUG_HPP__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "dep.hpp"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
// #define FireWater // 选择使用FireWater协议（所有数据均使用字符串形式）
/* 注！！！ vofo中 FireWater 帧头要求最后一位为: 帧间隔为,  尾帧要求回车 0D 0A */
/* 上面注释了直接选择 JustFloat 协议（所有按照长度组合为4字节浮点数） */
#ifndef FireWater
#define JustFloat // 选择使用 JustFloat 协议（所有按照长度组合为4字节浮点数）
/* 注！！！ vofo中 JustFloat 无帧头、帧间隔 尾帧要求 00 00 80 7f */
#endif // !FireWater

/* serialdebug 串口绘图类声明---------------------------------------------------------------- */
class serialdebug
{
public:
	/* 声明链表类，用来管理数据 */
	struct data_LinkedNode
	{
		Shared_Struct_32bit data;						  // 节点上存储的元素
		data_LinkedNode *next;							  // 指向下一个节点的指针
		data_LinkedNode(float x) : data{x}, next(NULL) {} // 节点的构造函数
		data_LinkedNode(float x, data_LinkedNode *next) : data{x}, next(next) {}
	};

	UART_HandleTypeDef *_huart; // 使用的串口通道
	uint8_t data_size;			// 一帧数据长度

	// 成员函数
	serialdebug(UART_HandleTypeDef *_huart, uint8_t data_size);
	// ~serialdebug();
	void check_frame_length(void);				// 帧长度检查
	void add_LinkedNode_AtTail(int val);		// 在末尾添加一个节点
	void config_data(float data_in);			// 转接发送的数据至data[]数组
	void config_data(float data_in, int index); // 转接发送的数据至data[]数组
	void send_frame(void);						// 发送一帧数据

protected:
	// 保护成员变量
	/* 自定义协议 */
	// uint8_t frame_start[2] = {0x2f, 0x2a}; // 帧头为 "/*"
	// uint8_t frame_separator[1] = {0x20};   // 间隔符为 " "
	// uint8_t frame_end[2] = {0x2a, 0x2f};   // 帧尾为 "*/"

	/* vofa-FireWater 协议 */
	// uint8_t frame_start[0] = {};		 // 帧头为 ""
	// uint8_t frame_separator[1] = {','};	 // 间隔符为 ","
	// uint8_t frame_end[2] = {0x0d, 0x0a}; // 帧尾为 "\r\n"

	/* vofa-JustFloat 协议 */
	uint8_t frame_start[0] = {};					 // 帧头为 ""
	uint8_t frame_separator[0] = {};				 // 间隔符为 ","
	uint8_t frame_end[4] = {0x00, 0x00, 0x80, 0x7F}; // 帧尾为 "00 00 80 7f"

	uint8_t frame[102];	  // 发送的帧，由于DMA中必须使用全局变量，使用变长数组太麻烦了，因此必须使用一个定长数组，根据需求调整大小
	uint8_t decimals = 8; // 小数位数

private:
	data_LinkedNode *_datadummyHead; // ；链表虚拟头结点
	uint8_t frame_length;			 // 发送一帧数据的字符串长度
	uint8_t data_index;				 // 当前最后一个数据索引
};

/* 外部声明变量 */

extern serialdebug serial_test;

#endif
