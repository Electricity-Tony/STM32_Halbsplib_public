# Led库使用教程

## 1 日志

 * @外设相关：<font color=Red>memory</font >

   @版本：<font color=Red>1.1</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                            功能                             |
   | :--------------------------------- | :---------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> |       <font color=DeepSkyBlue>基本配置相关说明</font>       |
   | <font color=DeepSkyBlue>1.1</font> | <font color=DeepSkyBlue>重构，新增加环形缓冲空间fifo</font> |



 ## 2 文件介绍

> | bsp_memory.c     | 主文件           |
> | ---------------- | ---------------- |
> | **bsp_memory.h** | **memory头文件** |

 ## 3 重要函数介绍

### 3.1 fifo 类构造

```cpp
// fifo 表示先进先出队列
class fifo
{
public:
	// 公共函数
	fifo(void){};
	fifo(uint8_t *data_ptr);

	uint16_t check_free_size(void);
	fifo_State check_empty(void);
	fifo_State check_full(void);
	pop_push_state push(uint8_t data);			   // 写入一个数据
	pop_push_state pop(uint8_t *out_data);		   // 读一个数据至 out_data
	uint16_t write(uint8_t *ptr, uint16_t length); // 写入指定长度数据
	uint16_t read(uint8_t *data, uint16_t length); // 读取指定长度数据

	// 运算储存区
protected:
	uint16_t size;		  // 环形缓存区大小
	uint16_t free_size;	  // 空余的可用空间
	uint16_t write_index; // 写入位置
	uint16_t read_index;  // 读取位置
	uint8_t *buffer_ptr;  // 链接数组地址

private:
};
```

* 对每一个 fifo 进行设置构造

### 3.2 对  fifo 进行判断

```cpp
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
	else
	{
		return FIFO_NORMAL;
	}
}
```

* 判断 fifo 输出状态

### 3.3 fifo 入队和出

```cpp
/**
 * 函数功能: 元素入队列
 * 输入参数:	入队列元素
 * 返 回 值: pop_push_state 入队是否成功
 * 说    明：
 */
pop_push_state fifo::push(uint8_t data)
{
	if (check_full() == FIFO_FULL)
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
	if (check_empty() == FIFO_EMPTY)
	{
		return POP_PUSH_ERROR;
	}
	else
	{
		*out_data = this->buffer_ptr[this->read_index];
		this->read_index = (this->read_index + 1) % this->read_index;
		this->free_size--;
		return POP_PUSH_SUCCESS;
	}
}
```

* 通过调用入队和出队函数进行控制

### 3.4 写入和读取数据

```cpp
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
			*(data + i) = buffer_ptr[write_index++];
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
```







## 4 自定义修改参数



## 5 基本使用方法

### 5.1 默认使用方法

* 声明 fifo 类，进行操作




 ## 6 其他注意



 
