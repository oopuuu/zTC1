/**
******************************************************************************
* @file    platform.h
* @author  William Xu
* @version V1.0.0
* @date    05-Oct-2016
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
    MICO_GPIO_1,
    MICO_GPIO_2,
    MICO_GPIO_7,
    MICO_GPIO_8,
    MICO_GPIO_9,
    MICO_GPIO_10,
    MICO_GPIO_12,
    MICO_GPIO_13,
    MICO_GPIO_14,
    MICO_GPIO_19,
    MICO_GPIO_21,
    MICO_GPIO_22,
    MICO_GPIO_23,
    MICO_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    MICO_GPIO_NONE,
} mico_gpio_t;

typedef enum
{
    MICO_SPI_1,
    MICO_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
    MICO_SPI_NONE,
} mico_spi_t;

typedef enum
{
    MICO_I2C_1,
    MICO_I2C_2,
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
    MICO_PWM_1,
    MICO_PWM_2,
    MICO_PWM_3,
    MICO_PWM_4,
    MICO_PWM_5,
    MICO_PWM_6,
    MICO_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    MICO_PWM_NONE,
} mico_pwm_t;

typedef enum
{
    MICO_GTIMER_1,
    MICO_GTIMER_2,
    MICO_GTIMER_3,
    MICO_GTIMER_4,
    MICO_GTIMER_5,
    MICO_GTIMER_MAX, /* Denotes the total number of GTIMER port aliases. Not a valid GTIMER alias */
    MICO_GTIMER_NONE,
} mico_gtimer_t;

typedef enum
{
    MICO_ADC_1,
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
  MICO_FLASH_SPI,
  MICO_FLASH_MAX,
  MICO_FLASH_NONE,
} mico_flash_t;

/* Donot change MICO_PARTITION_USER_MAX!! */
typedef enum
{
    MICO_PARTITION_USER_MAX = 0,
    MICO_PARTITION_USER = 7,
    MICO_PARTITION_SDS,
} mico_user_partition_t;

typedef struct
{
  mico_gpio_t pin;
  unsigned char config; /* @ref mico_gpio_config_t */
  unsigned char out; /* 0: low, 1: high */
}mico_gpio_init_t;

typedef struct
{
  mico_gpio_t pin;
} mico_pwm_pinmap_t;

typedef struct
{
  mico_gpio_t mosi; 
  mico_gpio_t miso; 
  mico_gpio_t sclk; 
  mico_gpio_t ssel; 
} mico_spi_pinmap_t;

typedef struct
{
  mico_gpio_t tx;
  mico_gpio_t rx;
  mico_gpio_t rts;
  mico_gpio_t cts;
} mico_uart_pinmap_t;

typedef struct
{
  mico_gpio_t sda;  
  mico_gpio_t scl;
} mico_i2c_pinmap_t;
    
typedef struct
{
  const mico_pwm_pinmap_t *pwm_pinmap;
  const mico_spi_pinmap_t *spi_pinmap;
  const mico_uart_pinmap_t *uart_pinmap;
  const mico_i2c_pinmap_t *i2c_pinmap;
} platform_peripherals_pinmap_t;

#define STDIO_UART      MICO_UART_2
#define STDIO_UART_BAUDRATE (115200) 

#define UART_FOR_APP    MICO_UART_1
#define MFG_TEST        MICO_UART_1
#define CLI_UART        MICO_UART_2

/* Components connected to external I/Os*/
#define Standby_SEL      (MICO_GPIO_29)

/* I/O connection <-> Peripheral Connections */
#define BOOT_SEL        MICO_GPIO_19
#define MFG_SEL         MICO_GPIO_NONE
#define MICO_RF_LED     MICO_GPIO_NONE
#define MICO_SYS_LED    MICO_GPIO_19
#define EasyLink_BUTTON MICO_GPIO_23

#define MICO_GPIO_NC    0xFF

typedef struct {
	int country_code;
	int enable_healthmon;
	int dhcp_arp_check;
} mico_system_config_t;

/* Arduino extention connector */
#define Arduino_RXD         (MICO_GPIO_23)
#define Arduino_TXD         (MICO_GPIO_18)
#define Arduino_D2          (MICO_GPIO_NONE)
#define Arduino_D3          (MICO_GPIO_NONE)
#define Arduino_D4          (MICO_GPIO_NONE) 
#define Arduino_D5          (MICO_GPIO_NONE)  
#define Arduino_D6          (MICO_GPIO_NONE) 
#define Arduino_D7          (MICO_GPIO_NONE)

#define Arduino_D8          (MICO_GPIO_NONE)
#define Arduino_D9          (MICO_GPIO_NONE)
#define Arduino_CS          (MICO_GPIO_8)
#define Arduino_SI          (MICO_GPIO_9)
#define Arduino_SO          (MICO_GPIO_7)
#define Arduino_SCK         (MICO_GPIO_10)
#define Arduino_SDA         (MICO_GPIO_8)
#define Arduino_SCL         (MICO_GPIO_7)

#define Arduino_A0          (MICO_ADC_NONE)
#define Arduino_A1          (MICO_ADC_NONE)
#define Arduino_A2          (MICO_ADC_NONE)
#define Arduino_A3          (MICO_ADC_NONE)
#define Arduino_A4          (MICO_ADC_NONE)
#define Arduino_A5          (MICO_ADC_NONE)

#define Arduino_I2C         (MICO_I2C_1)
#define Arduino_SPI         (MICO_SPI_1)
#define Arduino_UART        (MICO_UART_1)

#ifdef USE_MiCOKit_EXT
#define MICO_I2C_CP         (Arduino_I2C)
#include "micokit_ext_def.h"
#else
#define MICO_I2C_CP         (MICO_I2C_NONE)
#endif //USE_MiCOKit_EXT

#ifdef __cplusplus
} /*extern "C" */
#endif

