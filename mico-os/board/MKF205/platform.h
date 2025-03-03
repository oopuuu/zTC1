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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

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

/*
  MICO-EVB-1 platform pin definitions ...
+-------------------------------------------------------------------------+
| Enum ID       |Pin | STM32| Peripheral  |    Board     |   Peripheral   |
|               | #  | Port | Available   |  Connection  |     Alias      |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_0   | 0  | C  6 | GPIO        |              | MICO_UART_2_TX |
|               |    |      | TIM8_CH1    |              |                |
|               |    |      | I2S2_MCK    |              |                |
|               |    |      | USART6_TX   |              |                |
|               |    |      | TIM3_CH1    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_1   | 1  | C  7 | GPIO        |              | MICO_UART_2_RX |
|               |    |      | I2S2_MCK    |              |                |
|               |    |      | TIM8_CH2    |              |                |
|               |    |      | SDIO_D7     |              |                |
|               |    |      | USART6_RX   |              |                |
|               |    |      | TIM3_CH2    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_2   | 2  | C  8 | GPIO        |              |                |
|               |    |      | TIM8_CH3    |              |                |
|               |    |      | SDIO_D0     |              |                |
|               |    |      | TIM3_CH3    |              |                |
|               |    |      | USART6_CK   |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_3   | 3  | C  9 | GPIO        |              | MICO_PWM_1     |
|               |    |      | I2S2_CKIN   |              |                |
|               |    |      | TIM8_CH4    |              |                |
|               |    |      | SDIO_D1     |              |                |
|               |    |      | I2C3_SDA    |              |                |
|               |    |      | TIM3_CH4    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_4   | 4  | C  12| GPIO        |              |                |
|               |    |      | UART5_TX    |              |                |
|               |    |      | SDIO_CK     |              |                |
|               |    |      | SPI3_MOSI   |              |                |
|               |    |      | I2S3_SD     |              |                |
|               |    |      | UART3_CK    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_5   | 5  | C  13| GPIO        |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_6   | 6  | C  10| GPIO        |              |                |
|               |    |      | SPI3_SCK    |              |                |
|               |    |      | I2S3_SCK    |              |                |
|               |    |      | UART4_TX    |              |                |
|               |    |      | SDIO_D2     |              |                |
|               |    |      | UART3_TX    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_7   | 7  | C  11| GPIO        |              |                |
|               |    |      | UART4_RX    |              |                |
|               |    |      | SPI3_MISO   |              |                |
|               |    |      | SDIO_D3     |              |                |
|               |    |      | UART3_RX    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_8   | 8  | D  2 | GPIO        |              |                |
|               |    |      | TIM3_ETR    |              |                |
|               |    |      | USART5_RX   |              |                |
|               |    |      | SDIO_CMD    |              |                |
+---------------+----+------+-------------+--------------+----------------+
|               | 9  | BOOT |             |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_10  | 10 | A  4 | GPIO        |              | MICO_SPI_1_NSS |
|               |    |      | SPI1_NSS    |              |                |
|               |    |      | SPI3_NSS    |              |                |
|               |    |      | USART2_CK   |              |                |
|               |    |      | I2S3_WS     |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_11  | 11 | A  7 | GPIO        |              | MICO_SPI_1_MOSI|
|               |    |      | SPI1_MOSI   |              |                |
|               |    |      | TIM8_CH1N   |              |                |
|               |    |      | TIM14_CH1   |              |                |
|               |    |      | TIM3_CH2    |              |                |
|               |    |      | TIM1_CH1N   |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_12  | 12 | A  6 | GPIO        |              | MICO_SPI_1_MISO|
|               |    |      | SPI1_MISO   |              |                |
|               |    |      | TIM8_BKIN   |              |                |
|               |    |      | TIM13_CH1   |              |                |
|               |    |      | TIM1_BKIN   |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_13  | 13 | A  5 | GPIO        |              | MICO_SPI_1_CLK |
|               |    |      | SPI1_CLK    |              |                |
|               |    |      | TIM2_CH1_ETR|              |                |
|               |    |      | TIM8_CH1N   |              |                |
+---------------+----+------+-------------+--------------+----------------+
|               | 14 | GND  |             |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_15  | 15 | B  2 | GPIO        |              |                |
|               |    |      | TIM4_CH4    |              |                |
|               |    |      | TIM11_CH1   |              |                |
|               |    |      | I2C1_SDA    |              |                |
|               |    |      | CAN1_TX     |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_16  | 16 | B  10| GPIO        |              | MICO_I2C_1_SCL |
|               |    |      | SPI2_SCK    |              |                |
|               |    |      | I2S2_SCK    |              |                |
|               |    |      | I2C2_SCL    |              |                |
|               |    |      | USART3_TX   |              |                |
|               |    |      | TIM2_CH3    |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_17  | 17 | B  11| GPIO        |              | MICO_I2C_1_SDA |
|               |    |      | I2C2_SDA    |              |                |
|               |    |      | USART3_RX   |              |                |
|               |    |      | TIM2_CH4    |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_18  | 18 | C  5 | GPIO        |              |  MICO_ADC_6    |
|               |    |      | ADC123_IN15 |              |                |
+---------------+----+------+-------------+--------------+----------------+  
| MICO_GPIO_19  | 19 | C  4 | GPIO        |              |  MICO_ADC_5    |
|               |    |      | ADC123_IN14 |              |                |
+---------------+----+------+-------------+--------------+----------------+  
| MICO_GPIO_20  | 20 | C  3 | GPIO        |              |  MICO_ADC_4    |
|               |    |      | SPI2_MOSI   |              |                |
|               |    |      | ADC123_IN13 |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_21  | 21 | C  2 | GPIO        |              |  MICO_ADC_3    |
|               |    |      | SPI2_MISO   |              |                |
|               |    |      | ADC123_IN12 |              |                |
+---------------+----+------+-------------+--------------+----------------+  
| MICO_GPIO_22  | 22 | C  1 | GPIO        |              |  MICO_ADC_2    |
|               |    |      | ADC123_IN11 |              |                |
+---------------+----+------+-------------+--------------+----------------+
| MICO_GPIO_23  | 23 | C  0 | GPIO        |              |  MICO_ADC_1    |
|               |    |      | ADC123_IN10 |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 24 |  -   |             |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 25 | GND  |             |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 26 | GND  |             |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 27 | VDD  |     5V      |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 28 | VDD  |     3.3V    |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 29 | RESET|             |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 30 | VDD  |     5V      |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 31 |  -   |             |              |                |
|-------------------------------------------------------------------------|

*/

typedef enum
{
    MICO_SYS_LED,
    MICO_RF_LED,
    BOOT_SEL,
    MFG_SEL,
    Standby_SEL,
    EasyLink_BUTTON,
    STDIO_UART_RX,
    STDIO_UART_TX,  
    STDIO_UART_CTS,  
    STDIO_UART_RTS,
    FLASH_PIN_SPI_CS,
    FLASH_PIN_SPI_CLK,
    FLASH_PIN_SPI_MOSI,
    FLASH_PIN_SPI_MISO,

    MICO_GPIO_0,
    MICO_GPIO_1, 
    MICO_GPIO_2,
    MICO_GPIO_3,
    MICO_GPIO_4,
    MICO_GPIO_5,
    MICO_GPIO_6,
    MICO_GPIO_7,
    MICO_GPIO_8,
//    MICO_GPIO_9, BOOT
    MICO_GPIO_10,
    MICO_GPIO_11,
    MICO_GPIO_12,
    MICO_GPIO_13,
//    MICO_GPIO_14, GND
    MICO_GPIO_15,
    MICO_GPIO_16,
    MICO_GPIO_17,
    MICO_GPIO_18,
    MICO_GPIO_19,
    MICO_GPIO_20,
    MICO_GPIO_21,
    MICO_GPIO_22,
    MICO_GPIO_23,
//    MICO_GPIO_24  
//    MICO_GPIO_25,
//    MICO_GPIO_26,
//    MICO_GPIO_27,
//    MICO_GPIO_28,
//    MICO_GPIO_29,
//    MICO_GPIO_30,
//    MICO_GPIO_31,
    MICO_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    MICO_GPIO_NONE,
} mico_gpio_t;

typedef enum
{
    MICO_SPI_1,
    MICO_SPI_2,
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
    MICO_PWM_1,
    MICO_PWM_2,
    MICO_PWM_3,
    MICO_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    MICO_PWM_NONE,
} mico_pwm_t;

typedef enum
{
    MICO_ADC_1,
    MICO_ADC_2,
    MICO_ADC_3,
    MICO_ADC_4,
    MICO_ADC_5,
    MICO_ADC_6,
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
#define STDIO_UART       MICO_UART_1
#define STDIO_UART_BAUDRATE (115200) 
#else
#define STDIO_UART       MICO_UART_1
#define STDIO_UART_BAUDRATE (115200) 
#endif

#define UART_FOR_APP     MICO_UART_2
#define MFG_TEST         MICO_UART_1
#define CLI_UART         MICO_UART_1

#define USE_MICO_SPI_FLASH
//#define SFLASH_SUPPORT_MACRONIX_PARTS 
//#define SFLASH_SUPPORT_SST_PARTS
#define SFLASH_SUPPORT_WINBOND_PARTS

/* I/O connection <-> Peripheral Connections */
#define MICO_I2C_CP      (MICO_I2C_1)


#ifdef __cplusplus
} /*extern "C" */
#endif

#endif // __PLATFORM__H__

