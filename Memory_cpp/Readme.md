# Led库使用教程

## 1 日志

 * @外设相关：<font color=Red>memory</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                      功能                       |
   | :--------------------------------- | :---------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>基本配置相关说明</font> |



 ## 2 文件介绍

> | bsp_memory.cmemory | 主文件           |
> | ------------------ | ---------------- |
> | **bsp_memory.h**   | **memory头文件** |

 ## 3 重要函数介绍

### 3.1 queue 类构造

```cpp
//queue 表示为一个队列
class queue
{
public:
	uint16_t maxsize;
	int16_t* data;
	
		//公共函数
	queue(void){};
	queue(int16_t data_link[],uint16_t maxsize);
		
	uint8_t check_empty(void);
	uint8_t check_full(void);
	uint8_t	push(int16_t element);
	uint8_t pop(int16_t* out_element);
		
	//运算储存区
protected:
	uint16_t front;
	uint16_t rear;
	uint16_t counter;
	
private:
	
};
```

* 对每一个 queue 进行设置构造

### 3.2 对 queue 进行判断

```cpp
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
```

* 判断 queue 输出状态

### 3.3 queue 入队和出

```cpp
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
```

* 通过调用入队和出队函数进行控制







## 4 自定义修改参数



## 5 基本使用方法

### 5.1 默认使用方法

* 声明 queue 类，进行操作




 ## 6 其他注意



 
