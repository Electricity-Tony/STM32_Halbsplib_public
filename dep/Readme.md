# dep库使用教程

## 1 日志

 * @外设相关：<font color=Red>最底层依赖库</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             更新时间                             |功能|
   | :--------------------------------- | :----------------------------------------------------------: | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>2023-7-24</font> |<font color=DeepSkyBlue>为保证所有库需要依赖的文件都包含，将所有依赖文件独立成dep库</font>|


 ## 2 文件介绍

> | dep.cpp     | dep主文件     |
> | ----------- | ------------- |
> | **dep.hpp** | **dep头文件** |

 ## 3 重要函数介绍

### 3.1 宏定义介绍

* 宏定义均全部放在 dep.hpp 文件中  

```cpp
/* 宏定义 --------------------------------------------------------------------*/
#define WEAK __attribute__((weak)) // 使用WEAK类型是方便重构特定函数

#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt))) // 限幅函数
// 宏定义实现的一个约束函数,用于限制一个值的范围。
// 具体来说，该宏定义的名称为 _constrain，接受三个参数 amt、low 和 high，分别表示要限制的值、最小值和最大值。该宏定义的实现使用了三元运算符，根据 amt 是否小于 low 或大于 high，返回其中的最大或最小值，或者返回原值。
// 换句话说，如果 amt 小于 low，则返回 low；如果 amt 大于 high，则返回 high；否则返回 amt。这样，_constrain(amt, low, high) 就会将 amt 约束在 [low, high] 的范围内。

#define PI 3.1415926535f // 调用PI的计算

#define MICROS_ms() HAL_GetTick() // 计时，单位ms,除1000是秒

#define ABS(x) ((x) > 0 ? (x) : -(x)) // 快速取绝对值
```

### 3.2 函数定义

* 函数主题放在 dep.cpp 中，函数声明放在 dep.hpp 中

### 3.3 功能汇总

* 可以调用的功能


| 宏定义名                   | 作用                  |
| -------------------------- | --------------------- |
| WEAK                       | 弱定义声明            |
| _constrain(amt, low, high) | 将amt限幅在[low,high] |
| PI                         | 3.1415926535f 的声明  |
| MICROS_ms()                | 获取毫秒计时          |
| MICROS_us()                | 获取微秒计时          |
| ABS(x)                     | 快速取绝对值          |

* 避免 bug 重构的功能
| 功能名 | 作用                     |
| ------ | ------------------------ |
| new    | 重定义，避免原始的爆内存 |
| delete | 重定义，避免原始的爆内存 |




 ## 4 自定义修改参数






 ## 5 基本使用方法

* 导入 dep 包即可






 ## 6 其他注意




