# dep库使用教程

## 1 日志

 * @外设相关：<font color=Red>最底层依赖库</font >

   @版本：<font color=Red>1.1</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             更新时间                             |功能|
   | :--------------------------------- | :----------------------------------------------------------: | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>2023-7-24</font> |<font color=DeepSkyBlue>为保证所有库需要依赖的文件都包含，将所有依赖文件独立成dep库</font>|
   | <font color=DeepSkyBlue>1.1</font> | <font color=DeepSkyBlue>2023-8-31</font> |<font color=DeepSkyBlue>增加角度单位转换</font>|
   |                                    |                                          |                                                              |
   |                                    |                                          |                                                              |
   |                                    |                                          |                                                              |


 ## 2 文件介绍

> | dep.cpp     | dep主文件     |
> | ----------- | ------------- |
> | **dep.hpp** | **dep头文件** |

 ## 3 重要函数介绍

### 3.1 宏定义介绍

* 宏定义均全部放在 dep.hpp 文件中  

### 3.2 函数定义

* 函数主题放在 dep.cpp 中，函数声明放在 dep.hpp 中

### 3.3 功能汇总

* 可以调用的功能
* \#define Rad2Rot(rad) ((rad) / PI * 30)    // 弧度(rad/s) 转 转速(rpm)


| 宏定义名                   | 作用                  |
| -------------------------- | --------------------- |
| WEAK                       | 弱定义声明            |
| Rad2Rot(rad) 等            | 角度单位转换          |
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





