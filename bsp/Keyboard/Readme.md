# Keyboard库使用教程

## 1 日志

 * @外设相关：<font color=Red>Keyboard</font >

   @版本：<font color=Red>1.0</font >

   @维护：<font color=Red>Tony_Wang</font >

   @代码规范：<font color=Red>暂无</font>
   
    
   
  
   | 版本                               |                         功能                          |
   | :--------------------------------- | :---------------------------------------------------: |
   | <font color=DeepSkyBlue>1.0</font> | <font color=DeepSkyBlue>基本Cube配置与基本设置</font> |


 ## 2 文件介绍

> | bsp_keyboard.c     | 按键主文件     |
> | ------------------ | -------------- |
> | **bsp_keyboard.h** | **按键头文件** |

 ## 3 重要函数介绍

### 3.1 按键结构体构造



 ## 4 自定义修改参数




 ## 5 基本使用方法

### 5.1 默认使用方法

* Cube选项<font color='DeepSkyBlue'>Connectivity</font>font>中开启<font color='DeepSkyBlue'>USB（FS）</font>
* <font color='DeepSkyBlue'>Middleware</font>开启<font color='DeepSkyBlue'>USB_DEVICE</font>,选择<font color='DeepSkyBlue'>Human Interface Device Class(HID)</font>
* <font color='DeepSkyBlue'>HID_FS_BINTERVAL</font>设置为<font color='DeepSkyBlue'>0xA</font>
* 将文件中的所有内容覆盖usbd_hid.c
* 调用<font color='DeepSkyBlue'>USBD_HID_SendReport(&hUsbDeviceFS,发送数组,发送长度); </font>函数




 ## 6 其他注意



 

