# as5048a库使用教程

## 1 日志

 * @外设相关：<font color=Red>as5048a</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                             更新时间                             |功能|
   | :--------------------------------- | :----------------------------------------------------------: | :----------------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>2024-3-26</font> |<font color=DeepSkyBlue>基本Cube配置与基本设置</font>|



 ## 2 文件介绍

> | bsp_as5048a.c     | 主文件     |
> | ----------------- | ---------- |
> | **bsp_as5048a.h** | **头文件** |

 ## 3 重要函数介绍

### 3.1 读角度函数



```c
/* 读取角度值 */
float bsp_as5048a_ReadAngle(void)
{
    // return (ReadValue(READ_ANGLE_VALUE) * 360.0 / 0x10000);/* 返回 角度值 [0,360] */
    // return (bsp_as5048a_ReadValue(READ_ANGLE_VALUE) * 2.0 * PI / 0x10000); /* 返回 弧度值 [0,2*PI] */
    return (bsp_as5048a_ReadValue(CMD_ANGLE) * 2.0 * PI / 0x3fff); /* 返回 弧度值 [0,2*PI] */
}

/* 调用的公用函数 */
uint16_t bsp_as5048a_ReadValue(uint16_t u16RegValue)
{
    uint16_t u16Data;

    SPI_CS_ENABLE;
    HAL_SPI_TransmitReceive(&AS5048A_SPI, (uint8_t *)&u16RegValue, (uint8_t *)&u16Data, 1, 0xff);
    SPI_CS_DISABLE;
    return (u16Data & 0x3FFF);
}
```

* 已经进行数据转换，转换为 double 的浮点类型数据，单位为弧度
* 注意：读取与接受中采用了 写入读取公用的 HAL库 。单独写入然后读取 需要在中间加一个延迟，实测吧 cs 分别拉高拉低的时间就行了
* 注意：这个芯片的特殊性，本次发送命令读取的是上一次的命令值，所有必须高频快速的读取，否则需要加入 nop 空指令函数



 ## 4 自定义修改参数

* 使用的 spi 总线
* 使用的片选引脚

```c
// 需要自己转接的3个宏定义
#define AS5048A_SPI hspi1
#define GPIO_CS_Pin_Name SPI1_CS_Pin
#define GPIO_CS_Pin_Type SPI1_CS_GPIO_Port
```


 ## 5 基本使用方法

### 5.1 默认使用方法

* Cube配置中在<font color='DeepSkyBlue'>SPI_CS</font>，输出 GPIO
* 配置 spi ， 目前测试 4.5M 速率无问题，据说可以18M以上
* 配置一帧数据为<font color="deepskyblue">16Bits,CPOL = 0 , CPHA = 1(2 Edge)</font>
* 循环调用<font color='DeepSkyBlue'>bsp_as5048a_ReadAngle</font>




 ## 6 其他注意

 
