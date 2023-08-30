/**
 * @file bsp_memory.cpp
 * @brief memory板级支持包
 * @author Tony_Wang
 * @version 1.1
 * @date 2023-8-24
 * @copyright
 * @par 日志:
 *   V1.0 基本配置相关说明
 *   V1.1 新增加环形缓冲空间fifo
 */

// 局部数组大小受限 请修改 Stack_Size（栈）大小

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_memory.hpp"

/* 私有类型定义 --------------------------------------------------------------*/

#define WEAK __attribute__((weak)) // 使用WEAK类型是方便来重构特定函数
#define ABS(x) ((x) > 0 ? (x) : -(x))

using namespace std;

// uint8_t data[50];
// fifo fifo_data(data);

////******************************************* fifo(FIFO) 类 *************************************************************************////
/**
	* @brief  fifo
	* @param [in] *data_ptr			链接数组地址
	* @param [in] size 数组大小

	*
*/
WEAK fifo::fifo(uint8_t *data_ptr, uint16_t size)
{

	this->write_index = 0;
	this->read_index = 0;
	/* 链接数组 */
	this->buffer_ptr = data_ptr;
	this->size = size;
	this->free_size = size;
}

/**
 * @brief  fifo 状态判断函数
 * @details
 * @param
 * @retval	返回空余空间大小
 */
uint16_t fifo::check_free_size(void)
{
	return this->free_size;
}

/**
 * 函数功能: 判断队列状态
 * 输入参数:
 *	返 回 值: fifo 状态
 * 说    明：
 */
fifo_State fifo::check_state(void)
{
	if (check_free_size() == size)
	{
		return FIFO_EMPTY;
	}
	else if (check_free_size() == 0)
	{
		return FIFO_FULL;
	}
	return FIFO_NORMAL;
}

/**
 * 函数功能: 元素入队列
 * 输入参数:	入队列元素
 * 返 回 值: pop_push_state 入队是否成功
 * 说    明：
 */
pop_push_state fifo::push(uint8_t data)
{
	if (check_state() == FIFO_FULL)
	{
		return POP_PUSH_ERROR;
	}
	else
	{
		this->buffer_ptr[this->write_index] = data;
		this->write_index = (this->write_index + 1) % this->size;
		this->free_size--;
		return POP_PUSH_SUCCESS;
	}
}

/**
 * 函数功能: 元素出队列
 * 输入参数:	获得出元素值的存储变量
 *	返 回 值: pop_push_state 出队是否成功
 * 说    明：
 */
pop_push_state fifo::pop(uint8_t *out_data)
{
	if (check_state() == FIFO_EMPTY)
	{
		return POP_PUSH_ERROR;
	}
	else
	{
		*out_data = this->buffer_ptr[this->read_index];
		this->read_index = (this->read_index + 1) % this->size;
		this->free_size++;
		return POP_PUSH_SUCCESS;
	}
}

/**
 * @brief  fifo 写数据
 * @details
 * @param	写入的数组地址和长度
 * @example write(&date,sizeof(data)/sizeof(data[0]))
 * @retval
 */
uint16_t fifo::write(uint8_t *ptr, uint16_t length)
{
	/* cheak free_size */
	if (free_size < length)
	{
		return 0;
	}
	else
	{
		for (uint8_t i = 0; i < length; i++)
		{
			buffer_ptr[write_index++] = *(ptr + i);
			/* 判断一个环结束 */
			if (write_index == size)
			{
				write_index = 0;
			}
		}
		free_size -= length;
	}
	return length;
}

/**
 * @brief  fifo 读取数据
 * @details
 * @param	读取到的数组地址和长度
 * @example read(&date,20)
 * @retval
 */
uint16_t fifo::read(uint8_t *data, uint16_t length)
{
	/* cheak free_size */
	if (free_size == size)
	{
		return 0;
	}
	else
	{
		for (uint8_t i = 0; i < length; i++)
		{
			*(data + i) = buffer_ptr[read_index++];
			/* 判断一个环结束 */
			if (read_index == size)
			{
				read_index = 0;
			}
			free_size++;
			if (free_size == size)
			{
				return i + 1;
			}
		}
	}
	return length;
}

/**
 * @brief  fifo 清除数据
 * @details
 * @param	释放指定长度空间
 * @example read(&date,20)
 * @retval
 */
uint16_t fifo::read(uint16_t length)
{
	/* cheak free_size */
	if (free_size == size)
	{
		return 0;
	}
	else
	{
		for (uint8_t i = 0; i < length; i++)
		{
			/* 本质就是删除读取这一句话 */
			// *(data + i) = buffer_ptr[read_index++];
			read_index++;
			/* 判断一个环结束 */
			if (read_index == size)
			{
				read_index = 0;
			}
			free_size++;
			if (free_size == size)
			{
				return i + 1;
			}
		}
	}
	return length;
}
