/**
 ******************************************************************************
 * @file    platform.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provides all MICO Peripherals defined for current platform.
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

#ifndef __PLATFORM_H_
#define __PLATFORM_H_

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
  
/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
//    MFG_SEL, 
    EasyLink_BUTTON, 
    MICO_SYS_LED,
    
    MICO_GPIO_1,
    MICO_GPIO_2,
    MICO_GPIO_3,
    MICO_GPIO_4,
    MICO_GPIO_5,
    MICO_GPIO_6,
    MICO_GPIO_7,
    MICO_GPIO_8,
    
    MICO_GPIO_9,
    MICO_GPIO_10,
    MICO_GPIO_11,
    MICO_GPIO_12,
    MICO_GPIO_13,
    MICO_GPIO_14,

    MICO_GPIO_17,
    MICO_GPIO_18,
    
    MICO_GPIO_19,

    MICO_GPIO_20,
    MICO_GPIO_21,
    MICO_GPIO_22,
    MICO_GPIO_23,
    MICO_GPIO_24,
    
    MICO_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    MICO_GPIO_NONE,
} mico_gpio_t;

#define MFG_SEL               (MICO_GPIO_NONE)
#define MICO_RF_LED           (MICO_GPIO_NONE)
#define BOOT_SEL              (MICO_GPIO_NONE)

typedef enum
{
  MICO_SPI_1,
  MICO_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
  MICO_SPI_NONE,
} mico_spi_t;

typedef enum
{
    MICO_I2C_1,
    MICO_I2C_MAX, /* Denotes the total number of I2C port aliases. Not a valid I2C alias */
    MICO_I2C_NONE,
} mico_i2c_t;

typedef enum
{
    MICO_IIS_MAX, /* Denotes the total number of IIS port aliases. Not a valid IIS alias */
    MICO_IIS_NONE,
} mico_iis_t;

typedef enum
{
    MICO_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    MICO_PWM_NONE,
} mico_pwm_t;

typedef enum
{
    MICO_ADC_1,
    MICO_ADC_2,
    MICO_ADC_3,
    MICO_ADC_MAX, /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
    MICO_ADC_NONE,
} mico_adc_t;

typedef enum
{
    MICO_UART_1,
    MICO_UART_2,
    MICO_UART_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
    MICO_UART_NONE,
} mico_uart_t;

typedef enum
{
  MICO_FLASH_EMBEDDED,
  MICO_FLASH_SPI,
  MICO_FLASH_MAX,
  MICO_FLASH_NONE,
} mico_flash_t;

typedef enum
{
  MICO_PARTITION_USER_MAX
} mico_user_partition_t;

#ifdef BOOTLOADER
#define STDIO_UART       (MICO_UART_1)
#define STDIO_UART_BAUDRATE (115200) 
#else
#define STDIO_UART       (MICO_UART_1)
#define STDIO_UART_BAUDRATE (115200) 
#endif

#define UART_FOR_APP     (MICO_UART_1)
#define MFG_TEST         (MICO_UART_1)
#define CLI_UART         (MICO_UART_1)

/* Components connected to external I/Os*/
#define USE_MICO_SPI_FLASH
//#define SFLASH_SUPPORT_MACRONIX_PARTS 
//#define SFLASH_SUPPORT_SST_PARTS
#define SFLASH_SUPPORT_WINBOND_PARTS

/* Arduino extention connector */
#define Arduino_RXD         (MICO_GPIO_1)
#define Arduino_TXD         (MICO_GPIO_2)
#define Arduino_D2          (MICO_GPIO_3)
#define Arduino_D3          (MICO_GPIO_4)
#define Arduino_D4          (MICO_GPIO_5) 
#define Arduino_D5          (MICO_GPIO_6)  
#define Arduino_D6          (MICO_GPIO_7) 
#define Arduino_D7          (MICO_GPIO_8)

#define Arduino_D8          (MICO_GPIO_9)
#define Arduino_D9          (MICO_GPIO_10)
#define Arduino_CS          (MICO_GPIO_11)
#define Arduino_SI          (MICO_GPIO_12)
#define Arduino_SO          (MICO_GPIO_13)
#define Arduino_SCK         (MICO_GPIO_14)

#define Arduino_SDA         (MICO_GPIO_17)
#define Arduino_SCL         (MICO_GPIO_18)

#define Arduino_A0          (MICO_ADC_1)
#define Arduino_A1          (MICO_ADC_2)
#define Arduino_A2          (MICO_ADC_3)
#define Arduino_A3          (MICO_ADC_NONE)
#define Arduino_A4          (MICO_ADC_NONE)
#define Arduino_A5          (MICO_ADC_3)

#define Arduino_I2C         (MICO_I2C_1)
#define Arduino_SPI         (MICO_SPI_1)
#define Arduino_UART        (MICO_UART_2)

#define USE_MiCOKit_STMEMS

#ifdef USE_MiCOKit_STMEMS
#define MICO_I2C_CP         (Arduino_I2C)
#include "MiCOKit_STmems/MiCOKit_STmems_def.h" 
#else
#define MICO_I2C_CP         (MICO_I2C_NONE)
#endif //USE_MiCOKit_STMEMS

#endif



