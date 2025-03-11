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



/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    NXP_GPIO_0_27,
	NXP_GPIO_0_28,
	NXP_GPIO_0_29,
	NXP_GPIO_0_30,
	NXP_GPIO_0_20,

	NXP_GPIO_1_12,
	NXP_GPIO_1_13,
	NXP_GPIO_0_7 ,
	NXP_GPIO_1_15,
	NXP_GPIO_1_14,
	NXP_GPIO_0_10,
	NXP_GPIO_0_5 ,
	NXP_GPIO_0_6 ,

	NXP_GPIO_0_19,
	NXP_GPIO_0_18,
	NXP_GPIO_0_15,
	NXP_GPIO_0_12,
	NXP_GPIO_0_13,
	NXP_GPIO_0_11,
	NXP_GPIO_0_24,
	NXP_GPIO_0_23,

	NXP_GPIO_1_0,
	NXP_GPIO_1_1,
	NXP_GPIO_1_2,
	NXP_GPIO_1_3,
	NXP_GPIO_1_4,
	NXP_GPIO_1_5,

	FLASH_PIN_SPI_CS,
    MICO_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    MICO_GPIO_NONE,
} mico_gpio_t;

typedef enum
{
    CLKOUT      ,
    CLKIN       ,

    STDIO_UART_RX,
    STDIO_UART_TX,

    SPI0_PIN_CLK ,
    SPI0_PIN_MOSI,
    SPI0_PIN_MISO,
} sys_gpio_t;

typedef enum
{
    MICO_SPI_0,
    MICO_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
    MICO_SPI_NONE,
} mico_spi_t;

typedef enum
{
    MICO_I2C_0,
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
    MICO_PWM_0,
    MICO_PWM_1,
    MICO_PWM_2,
    MICO_PWM_3,
    MICO_PWM_4,
    MICO_PWM_5,

    MICO_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    MICO_PWM_NONE,
} mico_pwm_t;

typedef enum
{
    MICO_ADC_0,
    MICO_ADC_1,
    MICO_ADC_2,
    MICO_ADC_MAX, /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
    MICO_ADC_NONE,
} mico_adc_t;

typedef enum
{
    MICO_UART_0,
    MICO_UART_1,
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

#define USE_MICO_SPI_FLASH
//#define SFLASH_SUPPORT_MACRONIX_PARTS
//#define SFLASH_SUPPORT_SST_PARTS
//#define SFLASH_SUPPORT_WINBOND_PARTS

#ifdef BOOTLOADER
#define STDIO_UART       MICO_UART_0
#define STDIO_UART_BAUDRATE (921600)
#else
#define STDIO_UART       MICO_UART_0
#define STDIO_UART_BAUDRATE (115200)
#endif

#define UART_FOR_APP     MICO_UART_1
#define MFG_TEST         MICO_UART_0
#define CLI_UART         MICO_UART_0

/* I/O connection <-> Peripheral Connections */
/* Mother board connection */
#define BOOT_SEL            (NXP_GPIO_0_27)
#define MFG_SEL             (NXP_GPIO_0_28)
#define MICO_SYS_LED        (NXP_GPIO_0_29)
#define MICO_RF_LED         (NXP_GPIO_0_30)
#define EasyLink_BUTTON     (NXP_GPIO_0_20)

/* Arduino extention connector */
#define Arduino_RXD         (NXP_GPIO_1_12)
#define Arduino_TXD         (NXP_GPIO_1_13)
#define Arduino_D2          (NXP_GPIO_0_7)
#define Arduino_D3          (NXP_GPIO_1_15)
#define Arduino_D4          (NXP_GPIO_1_14)
#define Arduino_D5          (NXP_GPIO_0_10)
#define Arduino_D6          (NXP_GPIO_0_5)
#define Arduino_D7          (NXP_GPIO_0_6)

#define Arduino_D8          (NXP_GPIO_0_19)
#define Arduino_D9          (NXP_GPIO_0_18)
#define Arduino_CS          (NXP_GPIO_0_15)
#define Arduino_SI          (NXP_GPIO_0_12)
#define Arduino_SO          (NXP_GPIO_0_13)
#define Arduino_SCK         (NXP_GPIO_0_11)
#define Arduino_SDA         (NXP_GPIO_0_24)
#define Arduino_SCL         (NXP_GPIO_0_23)

#define Arduino_A0          (MICO_ADC_NONE)
#define Arduino_A1          (MICO_ADC_NONE)
#define Arduino_A2          (MICO_ADC_0)
#define Arduino_A3          (MICO_ADC_1)
#define Arduino_A4          (MICO_ADC_NONE)
#define Arduino_A5          (MICO_ADC_NONE)


#define Arduino_I2C         (MICO_I2C_0)
#define Arduino_SPI         (MICO_SPI_0)
#define Arduino_UART        (MICO_UART_1)

#define USE_MiCOKit_EXT

#ifdef USE_MiCOKit_EXT
#define USE_RGB_LED_DRIVER_P9813
#define MICO_I2C_CP         (Arduino_I2C)
#include "micokit_ext_def.h"
#else
#define MICO_I2C_CP         (MICO_I2C_NONE)
#endif //USE_MiCOKit_EXT

#ifdef __cplusplus
} /*extern "C" */
#endif

