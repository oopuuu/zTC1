/**
 ******************************************************************************
 * @file    platform_config.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provides common configuration for current platform.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************
*                      Macros
******************************************************/

/******************************************************
*                    Constants
******************************************************/

#define HARDWARE_REVISION   "1.0"
#define DEFAULT_NAME        "NUCLEO"
#define MODEL               "NUCLEO-F412ZG"


/* MICO RTOS tick rate in Hz */
#define MICO_DEFAULT_TICK_RATE_HZ                   (1000)                                          //RTOS閺冨爼妫块悧锟� 1ms

  /************************************************************************
 * Uncomment to disable watchdog. For debugging only */
//#define MICO_DISABLE_WATCHDOG

/************************************************************************
 * Uncomment to disable standard IO, i.e. printf(), etc. */
//#define MICO_DISABLE_STDIO

/************************************************************************
 * Enable press space go to boot */
#define MICO_ENABLE_STDIO_TO_BOOT

/************************************************************************
 * Uncomment to disable MCU powersave API functions */
//#define MICO_DISABLE_MCU_POWERSAVE

/************************************************************************
 * Uncomment to enable MCU real time clock */
#define MICO_ENABLE_MCU_RTC

/************************************************************************
 * Restore default and start easylink after press down EasyLink button for 3 seconds. */
#define RestoreDefault_TimeOut                      (3000)

/************************************************************************
 * CPU clock 100MHZ*/
#define MCU_CLOCK_HZ            (100000000)

/************************************************************************
 * How many bits are used in NVIC priority configuration */
#define CORTEX_NVIC_PRIO_BITS   (4)

/************************************************************************
 * Enable write protection to write-disabled embedded flash sectors */
//#define MCU_EBANLE_FLASH_PROTECT 

#define HSE_SOURCE              RCC_HSE_OFF               /* Use internal crystal                */
#define AHB_CLOCK_DIVIDER       RCC_SYSCLK_Div1          /* AHB clock = System clock             */
#define APB1_CLOCK_DIVIDER      RCC_HCLK_Div2            /* APB1 clock = AHB clock / 2           */
#define APB2_CLOCK_DIVIDER      RCC_HCLK_Div1            /* APB2 clock = AHB clock / 1           */
#define PLL_SOURCE              RCC_PLLSource_HSI        /* PLL source = external crystal        */
#define PLL_M_CONSTANT          16                       /* PLLM = 16                            */
#define PLL_N_CONSTANT          400                      /* PLLN = 400                           */
#define PLL_P_CONSTANT          4                        /* PLLP = 4                             */
#define PPL_Q_CONSTANT          7                        /* PLLQ = 7                             */
#define PPL_R_CONSTANT          2                        /* PLLR = 2                             */
#define SYSTEM_CLOCK_SOURCE     RCC_SYSCLKSource_PLLCLK  /* System clock source = PLL clock      */
#define SYSTICK_CLOCK_SOURCE    SysTick_CLKSource_HCLK   /* SysTick clock source = AHB clock     */
#define INT_FLASH_WAIT_STATE    FLASH_Latency_3          /* Internal flash wait state = 3 cycles */
#define PWR_WakeUp_Pin          PWR_WakeUp_Pin2          /* PWR_Wake_Up_Pin                      */

/******************************************************
*  EMW1062 Options
******************************************************/
/*  Wi-Fi chip module */
#define EMW1062

/*  WiFi driver use gSPI mode, and share spi bus with other devices by different /cs pins */
#define MICO_WIFI_SHARE_SPI_BUS
  
/*  GPIO pins are used to bootstrap Wi-Fi to SDIO or gSPI mode */
//#define MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP

/*  Wi-Fi GPIO0 pin is used for out-of-band interrupt */
#define MICO_WIFI_OOB_IRQ_GPIO_PIN  ( 0 )

/*  Wi-Fi power pin is present */
//#define MICO_USE_WIFI_POWER_PIN

/*  Wi-Fi reset pin is present */
#define MICO_USE_WIFI_RESET_PIN

/*  Wi-Fi 32K pin is present */
#define MICO_USE_WIFI_32K_PIN                                                                       //WiFi濡�虫健閺冨爼鎸撻悽鍗烆樆闁劍褰佹笟锟�

/*  USE SDIO 1bit mode. */
//#define SDIO_1_BIT

/* Wi-Fi power pin is active high */
//#define MICO_USE_WIFI_POWER_PIN_ACTIVE_HIGH

/*  WLAN Powersave Clock Source
 *  The WLAN sleep clock can be driven from one of two sources:
 *  1. MCO (MCU Clock Output) - default
 *  2. WLAN 32K internal oscillator (30% inaccuracy)
 */
//#define MICO_USE_WIFI_32K_CLOCK_MCO

//#define MICO_USE_BUILTIN_RF_DRIVER                                                                //鐞涖劎銇氶悽銊ュ敶闁劍鏆熼幑顕嗙礉閼板奔绗夐弰顖氼樆闁劎娈慠F driver

#ifdef __cplusplus
} /*extern "C" */
#endif

 
