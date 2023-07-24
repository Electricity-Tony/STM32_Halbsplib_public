# STM32_Halbsplib_public

stm32Cubemx驱动库

* 包含的现存的所有stm32库，基于Hal库版本编写，便于针对所有stm32项目能快速搭建框架进行使用，该版本是可以公开的版本

## 所有库快速链接

### 最基本依赖库

* 目前很多库都可能用到依赖，建议每次调用库报错的时候，先检查是否调用了依赖

> [dep](./dep/) " v1.0"

### 基于普通 io 的库

> [bsp_key](./Key/) " v1.0"
>
> [bsp_key_cpp](./Key_cpp/) " v1.3"
>
> [bsp_Led](./Led/) " v1.0"
>
> [bsp_Led_cpp](./Led_cpp/) " v1.1"

### 基于 adc 的库

> [bsp_adc](./ADC/) " v1.1"

### 基于定时器的库

> [bsp_StepMotor](./StepMotor/) " v1.0"
>
> [bsp_StepMotor_cpp](./StepMotor_cpp/) " v1.2"
>
> * bsp_StepMotor.cpp 内含 app 层库
>
> [bsp_servo](./Servo/) " v1.1"
>
> [bsp_servo_cpp](./Servo_cpp/) " v1.2"
>
> [bsp_foc](./foc_cpp/) " v1.0"

### 基于 i2c 的库

> [bsp_oled_i2c](./Oled_i2c/) " v1.0"

### 基于 spi 的库

> [bsp_bmp280](./Bmp280/) " v1.0"
>
> [bsp_tle5012b](./tle5012b/) " v1.0"
>
> [bsp_NRF24L01](./NRF24L01/) " v1.1"



### 基于 usart 的库

> [bsp_usart](./Usart/) " v1.4"
>
> [bsp_serialstudio](./SerialStudio/) " v1.1"
>
> [bsp_DYSV](./DYSV/) " v1.0"



### 基于 Can 的库

> [bsp_can](./Can/) " v2.2"
>



### 基于 usb 的库

> [bsp_myusbd_hid](./Keyboard/) " v1.0 这个还是个半成品"



### 纯算法的库

> [bsp_memory_cpp](./Memory_cpp/) " v1.0"
>
> [bsp_pid](./PID.cpp/) " v1.0"





