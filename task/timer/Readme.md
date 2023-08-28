# Key库使用教程

## 1 日志

 * @外设相关：<font color=Red>timer</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             功能                             |
   | :--------------------------------- | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>建立定时器调用库，避免每次都要翻阅资料</font> |
   |                                    |                                                              |
   |                                    |                                                              |
   |                                    |                                                              |


 ## 2 文件介绍

> | bsp_timer.cpp     | 定时器主文件     |
> | ----------------- | ---------------- |
> | **bsp_timer.hpp** | **定时器头文件** |

 ## 3 重要函数介绍

### 3.1 开启定时器

```c++
void timer_init(void)
{
    /* 这里放需要启动定时器中断的定时器 */
    HAL_TIM_Base_Start_IT(&htim1);
}
```

* 即封装了一个定时器初始化函数

#### 3.2 定时器中断回调

```c++
void Key::read()
```

* 直接更新当前按键当前状态值

### 3.2 派生类Button

```c++
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* 依次放进入中断的定时器 */
    if (htim == (&htim1))
    {
        /* 依次放该定时器下不同功能的触发间隔变量 */
        static uint8_t foc_run_flag = 0;

        /* 以上每个触发间隔变量 ++  */
        foc_run_flag += 1;

        /* 编写触发功能函数 */
        if (foc_run_flag == 10)
        {
            HallEncoder.get_speed();
            motor_1612.run_speed(6.28);
            /* 一定要添加间隔变量复位！！ */
            foc_run_flag = 0;
        }
    }
}
```

* 根据需求编写功能实现函数即可

 ## 4 自定义修改参数




 ## 5 基本使用方法

### 5.1 默认使用方法

* TIMx的Clock Sourse 中设置<font color='DeepSkyBlue'>Internal Clock</font>，设置预分频系数(Prescaler)和计数值(Counter Period),并注意时钟源中的频率来计算
* NVIC 中打开定时器中断
* 初始化时调用 timer_init()
* 中断回调中编写功能函数




 ## 6 其他注意



 
