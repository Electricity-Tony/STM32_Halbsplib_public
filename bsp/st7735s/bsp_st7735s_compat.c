#include "bsp_st7735s_compat.h"
#include "spi.h"
#include "tim.h"

uint32_t tim_period = 10000;
uint32_t tim_pulse;

#include "stm32f4xx_hal_gpio.h"

#define ST_BLK_TIM htim2
#define ST_BLK_TIM_CHANNEL TIM_CHANNEL_1
#define ST_HSPI hspi3

#ifndef _Pin_Init
// #define _Pin_Init(name) LL_GPIO_SetPinMode(name ## _GPIO_Port, name ## _Pin, LL_GPIO_MODE_OUTPUT)
#define _Pin_Toggle(name) HAL_GPIO_TogglePin(name##_GPIO_Port, name##_Pin)
#define _Pin_High(name) HAL_GPIO_WritePin(name##_GPIO_Port, name##_Pin, GPIO_PIN_SET);
#define _Pin_Low(name) HAL_GPIO_WritePin(name##_GPIO_Port, name##_Pin, GPIO_PIN_RESET);
#endif

void SPI_Init(void)
{

    // Pin_Low(CS);
}

void TIM_BLK_Init(void)
{
    HAL_TIM_PWM_Start(&ST_BLK_TIM, ST_BLK_TIM_CHANNEL);
}

void Pin_CS_Low(void)
{
    _Pin_Low(SPI3_CS);
}

void Pin_CS_High(void)
{
    _Pin_High(SPI3_CS);
}

void Pin_RES_High(void)
{
    _Pin_High(LCD_RES);
}

void Pin_RES_Low(void)
{
    _Pin_Low(LCD_RES);
}

void Pin_DC_High(void)
{
    _Pin_High(LCD_DC);
}

void Pin_DC_Low(void)
{
    _Pin_Low(LCD_DC);
}

extern uint8_t backlight_pct;
void Pin_BLK_Pct(uint8_t pct)
{
    backlight_pct = pct;
    tim_pulse = pct * tim_period / 100;
    __HAL_TIM_SET_COMPARE(&ST_BLK_TIM, ST_BLK_TIM_CHANNEL, tim_pulse);
}

void SPI_send(uint16_t len, uint8_t *data)
{
    Pin_CS_Low();
#if 0
	while (len--)
		HAL_SPI_Transmit(&hspi3, data++, 1, 0xF000);
#else
    HAL_SPI_Transmit(&ST_HSPI, data, len, 0xF000);
    Pin_CS_High();
#endif
}

void SPI_TransmitCmd(uint16_t len, uint8_t *data)
{
    Pin_DC_Low();
    SPI_send(len, data);
}

void SPI_TransmitData(uint16_t len, uint8_t *data)
{
    Pin_DC_High();
    SPI_send(len, data);
}

void SPI_Transmit(uint16_t len, uint8_t *data)
{
    SPI_TransmitCmd(1, data++);
    if (--len)
        SPI_TransmitData(len, data);
}

void _Delay(uint32_t d)
{
    HAL_Delay(d);
}
