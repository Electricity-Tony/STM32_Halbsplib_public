# STM32_Halbsplib

stm32Cubemx 驱动库

- 包含的现存的所有 stm32 库，基于 Hal 库版本编写，便于针对所有 stm32 项目能快速搭建框架进行使用

**所有库快速链接**

## 1. 最基本依赖库

* 目前很多库都可能用到依赖，建议每次调用库报错的时候，先检查是否调用了依赖

> [dep](./dep/) " v1.2"

## 2. task 功能库

* 应用于整合全局的功能库，避免重发查阅资料

> [timer](./task/timer/) " v1.0"

## 3 app 应用库

* 应用于集成了相关 bsp 功能的库，实现相应的应用

  > [app_usartcmd](./app/usartcmd) " v1.0" {基于串口实现串口控制}

## 4. bsp 驱动库

### 不依赖的库

> [bsp_flash](./bsp/flash) " v1.0"


### 基于普通 io 的库

> [bsp_key](./bsp/Key/) " v1.0"
>
> [bsp_key_cpp](./bsp/Key_cpp/) " v1.3"
>
> [bsp_Led](./bsp/Led/) " v1.0"
>
> [bsp_Led_cpp](./bsp/Led_cpp/) " v1.1"

### 基于 adc 的库

> [bsp_adc](./bsp/adc_cpp/) " v2.0"

### 基于定时器的库


> [bsp_StepMotor](./bsp/StepMotor/) " v1.0"
>
> [bsp_StepMotor_cpp](./bsp/StepMotor_cpp/) " v1.2"
>
> * bsp_StepMotor.cpp 内含 app 层库
>
> [bsp_servo](./bsp/Servo/) " v1.1"
>
> [bsp_servo_cpp](./bsp/Servo_cpp/) " v1.2"
>
> [bsp_foc](./bsp/foc_cpp/) " v2.1" "修改驱动方式为SVPWM，并优化调用函数"

### 基于 i2c 的库

> [bsp_oled_i2c](./bsp/Oled_i2c/) " v1.0"

### 基于 spi 的库

> [bsp_bmp280](./bsp/Bmp280/) " v1.0"
>
> [bsp_tle5012b](./bsp/tle5012b/) " v1.0"	"spi霍尔编码器"
>
> [bsp_as5048a](./bsp/as5048a/) " v1.0"	"spi霍尔编码器"
>
> [bsp_NRF24L01](./bsp/NRF24L01/) " v1.1"
>
> [bsp_st7735s](./bsp/st7735s/) " v1.0"



### 基于 usart 的库

> [bsp_usart](./bsp/usart_cpp/) " v2.0"
>
> [bsp_serialdebug](./bsp/serialdebug/) " v2.0"
>
> [bsp_DYSV](./bsp/DYSV/) " v1.0"



### 基于 Can 的库

> [bsp_can](./bsp/Can/) " v2.2"
>




### 基于 usb 的库

> [bsp_myusbd_hid](./bsp/Keyboard/) " v1.0 这个还是个半成品"



### 纯算法的库

> [bsp_memory_cpp](./bsp/Memory_cpp/) " v1.1"
>
> [bsp_pid](./bsp/PID.cpp/) " v1.0"
