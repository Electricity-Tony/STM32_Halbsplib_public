# filter库使用教程

## 1 日志

 * @外设相关：<font color=Red>filter</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             更新时间                             |功能|
   | :--------------------------------- | :----------------------------------------------------------: | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>2023-7-11</font> |<font color=DeepSkyBlue>建立基本的滤波器库</font>|



 ## 2 文件介绍

> | bsp_filter.cpp     | 滤波器主文件     |
> | ------------------ | ---------------- |
> | **bsp_filter.hpp** | **滤波器头文件** |
>
> > 需要嵌入的库
> > | bsp_systick.cpp     | 系统时钟主文件     |
> > | ------------------- | ------------------ |
> > | **bsp_systick.hpp** | **系统时钟头文件** |
> > 

 ## 3 重要函数介绍

### 3.1 低通滤波器 LowPassFilter

```cpp
class LowPassFilter
{
public:
    float Tf;   // 低通滤波时间常数，单位是 us
    float data; // 滤波后的结果

    /* 成员函数 */
    LowPassFilter(float Tf); // 输入时间常数初始化
    ~LowPassFilter() = default;
    float run(float data_in); // 滤波运行函数

private:
    /* data */
protected:
    float data_prev;    // 上一次滤波后的结果
    uint32_t time_prev; // 上一次滤波后的结果
};
```

* 实现低通滤波的功能变量
* 初始化设置时间常数 Tf ， 这个值与每次调用的dt有关

### 3.2 低通滤波器运行函数

```cpp
/**
 * @brief  低通滤波器运行函数
 * @details 输入本次的数值，自动与上一次的数值对比没实现滤波
 * @param  data_in：输入的数值
 * @retval
 */
float LowPassFilter::run(float data_in)
{
    /* 获取两次滤波的时间差 */
    uint32_t time_now = MICROS_us();
    float dt = (time_now - time_prev) * 1e-6f;

    /* 溢出时归零 */
    if (dt < 0.0f)
        dt = 1e-3f;
    /* 两次调用时间过长 取消滤波 */
    else if (dt > 0.3f)
    {
        data_prev = data_in;
        time_prev = time_now;
        data = data_in;
        return data;
    }
    /* 加权前后两次的值，实现低通滤波 */
    float alpha = Tf / (Tf + dt);
    data = alpha * data_prev + (1.0f - alpha) * data_in;
    data_prev = data;
    time_prev = time_now;
    return data;
}
```

* 实现低通滤波



 ## 4 自定义修改参数



 ## 5 基本使用方法

### 5.1 默认使用方法

* 导入 <font color='DeepSkyBlue'>bsp_filter</font> 包后，调用对应的滤波函数即可




 ## 6 其他注意



