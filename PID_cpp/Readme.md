# motor_withCan库使用教程

## 1 日志

 * @外设相关：<font color=Red>bsp_pid</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
   | 版本                               |                          功能                           |
   | :--------------------------------- | :-----------------------------------------------------: |
  | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>单独将电机库分离独立成库</font> |

 ## 2 文件介绍

> | **bsp_pid.cpp**        | **配套pid库主文件**                    |
> | ---------------------- | -------------------------------------- |
> | **bsp_pid.hpp**        | **配套pid库头文件**                    |

 ## 3 重要函数介绍


### 3.1 pid重要函数

#### 3.1.1 pid相关变量声明

* 目前pid使用类分为线性pid和非线性pid两种，目前主使用线性pid

```c++
//例
pid my_pid(0,0,0,0,0);//创建一个PID结构体，将P、I、D、IMax、PIDMax均设为0
//具体上面输入变量含义
pid（线性的传统PID） ：
    * @param [in]     P 比例系数
	* @param [in]	 I 积分系数
	* @param [in]	 D 微分系数
	* @param [in]	 IMax 积分限幅
	* @param [in]	 PIDMax 输出限幅
	* @param [in]	 I_Time 积分时间
	*@param [in]	 D_Time 积分时间
	* @param [in]	 I_Limited 进行积分调节的误差区间限制
motor :
	* @param [in]  can_num	第几个CAN 只允许输入1或2
	* @param [in]  _can_id		该电机的CAN_ID,如:0x201
	* @param [in]  *motor_type 电机类型结构体指针
	* @param [in]  _PID_In	内环PID 输入pid对象指针
	* @param [in]	 _PID_Out 外环PID	输入pid对象指针 
```




 ## 4 自定义修改参数




 ## 5 基本使用方法

   3. 在Option—>C/C++—>Misc Contrils添加<font color='DeepSkyBlue'>--cpp11</font>
   4. 在Option—>Target—><font color='DeepSkyBlue'>取消勾选Use MicroLIB</font>
        5. 右键<font color='DeepSkyBlue'>main.c</font>，选择<font color='DeepSkyBlue'>Options for File 'main.c'...</font>，<font color='DeepSkyBlue'>File Type</font>选择<font color='DeepSkyBlue'>c++Source file</font>
   6. 声明pid参数



 ## 6 其他注意


