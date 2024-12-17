/**
 * @file bsp_st7735s_api.c
 * @brief st7735s显示驱动板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2024-3-5
 * @copyright
 * @par 日志:
 *   V1.0 移植github开源工程，实现基本功能
 */

#include "bsp_st7735s.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief  st7735s 初始化函数
 * @details
 * @param
 * @retval
 */
void bsp_st7735s_init(rotation_t R)
{
    ST7735S_Init();
    /* 设置显示方向 */
    setOrientation(R);
    /* 清屏 */
    Delay(100);
    setColor(0, 0, 0);
    fillScreen();
}

extern void gcvtf();
/**
 * @brief  显示运行时间
 * @details
 * @param uint32_t time_ms 需要显示的运行时间
 * @retval
 */
void bsp_st7735s_runtimet(uint32_t time_ms)
{
    static uint32_t breath_counts = 0;
    char show_array[10] = {0};
    float time64i = time_ms * 0.001; // s

    sprintf(show_array, "%.3f", time64i);
    // show_array[-1] = 's';
    // for (uint8_t i = 0; i < 3; i++) // 提取小数
    // {
    //     show_array[-(i + 2)] = time64i % 10;
    //     time64i = time64i / 10;
    // }
    // show_array[-5] = '.';
    // uint8_t integer_length = 0;
    // while (time64i != 0)
    // {
    //     show_array[-(integer_length + 6)] = time64i % 10;
    //     time64i = time64i / 10;
    //     integer_length++;

    //     if (integer_length >= 5)
    //         break;
    // }
    // setColor(0, 0, 0);
    // fillScreen();
    setColor(255, 100, 100);
    setFont(ter_u24b);
    drawText(0, 0, (const char *)show_array);
    // drawText(0, 0, "111");
    flushBuffer();
}

/* test function */
void bsp_st7735s_example(void)
{

    ST7735S_Init();

    /* 清屏 */
    setColor(0, 0, 0);
    fillScreen();

    setOrientation(R0);

    while (1)
    {

        for (int i = 0; i < 32; i++)
        {
            setColor(i, i, i);
            drawLine(0, i, WIDTH - 1, i);
        }

        Delay(500);
        setColor(0, 0, 0);
        fillScreen();

        setColor(31, 63, 31);
        setbgColor(0, 0, 0);
        setFont(ter_u24b);
        drawText(4, 33, "Hi World!");

        Delay(500);
        setColor(0, 0, 0);
        fillScreen();

        for (int i = 0; i < 64; i++)
        {
            setColor(i / 2, 63 - i, i / 2);
            drawLine(WIDTH - 1 - i, 0, WIDTH - 1 - i, HEIGHT - 1);
        }

        Delay(500);
        setColor(0, 0, 0);
        fillScreen();

        for (int i = 0; i < 64; i++)
        {
            setColor(i / 2, i, i / 2);
            drawLine(0, HEIGHT - 1 - i, WIDTH - 1, HEIGHT - 1 - i);
        }

        Delay(500);
        setColor(0, 0, 0);
        fillScreen();

        setColor(10, 20, 30);
        drawCircle(80, 50, 30);

        Delay(500);
        setColor(0, 0, 0);
        fillScreen();

        setColor(30, 10, 30);
        drawCircle(40, 10, 90);

        Delay(500);
        setColor(0, 0, 0);
        fillScreen();

        setColor(30, 0, 30);
        drawLine(0, 0, WIDTH - 1, HEIGHT - 1);

        flushBuffer();

        Delay(500);
        setColor(0, 0, 0);
        fillScreen();

        for (uint8_t i = 0; i < 40; i++)
        {
            uint8_t r, g, b;
            r = (i * 3) % 255;
            g = (i * 3) % 255;
            b = (i * 7) % 255;
            setColor(r, g, b);
            setbgColor((255 - r), (255 - g), (255 - b));
            setFont(ter_u24b);
            drawText(i, 33, "LOVE MIMI!");

            setColor(0, 0, 0);
            drawLine(i, 0, i, HEIGHT - 1);
            Delay(50);
        }
        Delay(50);
        setColor(0, 0, 0);
        fillScreen();
    }
}