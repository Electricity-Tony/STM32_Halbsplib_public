# 独立CAN库使用教程

## 1 日志

 * @外设相关：<font color=Red>CAN</font >

   @版本：<font color=Red>2.2</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本 |                             功能                             |
   | :--- | :----------------------------------------------------------: |
   | 2.1  | <font color=DeepSkyBlue>重新编写ReadME，便于从零开始库的使用</font> |
   | 2.2  | <font color='DeepSkyBlue'>进行宏定义设定，可用于F1使用，并可开关使用电机库单独使用CAN，暂未验证并融合进motor_withCan</font> |
   
   

 ## 2 文件介绍

> | **bsp_can.c** | **can主文件** |
> | ------------- | ------------- |
> | **bsp_can.h** | **can头文件** |

 ## 3 重要函数介绍

 ### 3.1 CAN初始化函数
* CAN初始化函数，未成功会一直循环初始化
 ```c
void bsp_can_Init(void);						//CAN初始化
 ```

### 3.2 CAN发送函数

* 标准CAN发送函数

```c
HAL_StatusTypeDef bsp_can_Sendmessage(CAN_HandleTypeDef* hcan,int16_t StdId,int16_t* Can_Send_Data)
    //hcan为选择CAN发送通道
    //StdId为发送ID号
    //Can_Send_Data为发送数据，创建4数据长度的整型数组
```



 ### 3.3 CAN接收中断回调函数
 * 对于接受信息需要进行解析在对应位置写相关函数

```c
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
```



 ## 4 自定义修改参数

### 4.1 芯片头文件

 * 添加芯片头文件修改：

 * > <font color=DeepSkyBlue>bsp_can.h</font>添加头文件下修改为自己芯片型号的头文件

### 4.2 使用芯片型号宏定义修改

* <font color=DeepSkyBlue>bsp_can.h</font>下修改F1和F4使用的宏定义，主要由于F1只有CAN，没有CAN1这个东西，要单独处理

```c#
//芯片型号宏定义选择
//#define USE_F1XX
#define USE_F4XX
```

### 4.3 标准电机库宏定义选择

* <font color=DeepSkyBlue>bsp_can.c</font>下选择是否使用标准电机库

```c
//是否使用标准电机库宏定义选择
#define USE_bsp_motor
```

### 4.4 FreeRtos使用设定

* <font color=DeepSkyBlue>bsp_can.h</font>下修改对应使用操作系统宏定义

```c#
//条件编译开关宏定义，不需要开启的就注释掉相关宏定义
//#define	BSP_CAN_USE_FREERTOS
```

### 4.5 F4的CAN通道使用选择

* <font color=DeepSkyBlue>bsp_can.h</font>下对使用CAN1和CAN2进行选择

```c
//外设相关宏定义,移植时如果修改了外设请在这里修改
#ifdef USE_F4XX
#define BSP_CAN_USE_CAN1					hcan1
#define BSP_CAN_USE_CAN2					hcan2
#endif // USE_F4XX
```



 ## 5 基本使用方法

1. Cube配置CAN，波特率设为1M
2. 添加<font color='DeepSkyBlue'>bsp_can.cpp，bsp_can.hpp</font>文件至工程
3. 在Option—>C/C++—>Misc Contrils添加<font color='DeepSkyBlue'>--cpp11</font>
4. 在Option—>Target—><font color='DeepSkyBlue'>取消勾选Use MicroLIB</font>
5. 右键<font color='DeepSkyBlue'>main.c</font>，选择<font color='DeepSkyBlue'>Options for File 'main.c'...</font>，<font color='DeepSkyBlue'>File Type</font>选择<font color='DeepSkyBlue'>c++Source file</font>
5. 初始化使用<font color='DeepSkyBlue'>bsp_can_Init();</font>，进行CAN初始化
6. 调用发送函数进行相关发送，在main.h中将需要的全局变量声明，然后在中断回调函数中解析使用

 

 ## 6 其他注意

* 当前独立CAN库仅完成重构，还未对所有功能进行验证，因此暂时未更新motor_withCan的内容，验证完成会须进行融合

* 目前F1测试会卡死才发送函数<font color='DeepSkyBlue'>HAL_CAN_GetTxMailboxesFreeLevel</font>中，暂不清楚什么问题，发送若干次后会卡住————该问题已解决，将CAN波特率调整至1M后解决，猜想是波特率过小导致邮箱堵塞，具体原因暂不清楚。总之留下一个坑点，CAN波特率不能过小



