/*
 * @Author: Tony_Wang
 * @Date: 2024-03-05 18:40:54
 * @LastEditors: Tony_Wang
 * @LastEditTime: 2024-07-26 15:36:59
 * @FilePath: \st7735s\bsp_st7735s.h
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */
/**
 * @file bsp_st7735s.c
 * @brief st7735s显示驱动板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2024-3-5
 * @copyright
 * @par 日志:
 *   V1.0 移植github开源工程，实现基本功能
 */

#ifndef __bsp_st7735s_h__
#define __bsp_st7735s_h__

#include "bsp_st7735s_driver.h"
#include "bsp_st7735s_fonts.h"
#include "bsp_st7735s_gfx.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void bsp_st7735s_init(rotation_t R);
    void bsp_st7735s_runtimet(uint32_t time_ms);
    void bsp_st7735s_example(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif
