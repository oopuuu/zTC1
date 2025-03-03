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

/*
EMW3162 on EMB-380-S platform pin definitions ...
+-------------------------------------------------------------------------+
| Enum ID       |Pin | STM32| Peripheral  |    Board     |   Peripheral   |
|               | #  | Port | Available   |  Connection  |     Alias      |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_1   | 1  | B  6 | GPIO        |              |                |
|               |    |      | TIM4_CH1    |              |                |
|               |    |      | CAN2_TX     |              |                |
|               |    |      | USART1_TX   |              |                |
|               |    |      | I2C1_SCL    |              | MICO_I2C1_SCL  |
|               |    |      | CAN2_TX     |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_2   | 2  | B  7 | GPIO        |              |                |
|               |    |      | I2C1_SCL    |              | MICO_I2C1_SDA  |
|               |    |      | USART1_RX   |              |                |
|               |    |      | TIM4_CH2    |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 3  | A  13| SWDIO       |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_4   | 4  | C  7 | USART2_RX   |              | MICO_UART_2_RX |
|               |    |      | GPIO        |              |                |
|               |    |      | TIM8_CH2    |              |                |
|               |    |      | TIM3_CH2    |              |                |
|               |    |      | I2S3_MCK    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_5   | 5  | A  3 | ADC123_IN3  |              | MICO_ADC_1     |
|               |    |      | GPIO        |              |                |
|               |    |      | TIM2_CH4    |              |                |
|               |    |      | TIM5_CH4    |              |                |
|               |    |      | TIM9_CH2    |              |                |
|               |    |      | UART2_RX    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_6   | 6  | A  4 | SPI1_NSS    |              | MICO_SPI_1_NSS |
|               |    |      | GPIO        |              |                |
|               |    |      | SPI3_NSS    |              |                |
|               |    |      | I2S3_WS     |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_7   | 7  | B  3 | JTDO        |              |                |
|               |    |      | GPIO        |              |                |
|               |    |      | I2S3_SCK    |              |                |
|               |    |      | TIM2_CH2    |              |                |
|               |    |      | SPI1_SCK    |              | MICO_SPI_1_SCK |
|               |    |      | SPI3_SCK    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_8   | 8  | B  4 | NJRST       |              |                |
|               |    |      | GPIO        |              |                |
|               |    |      | SPI3_MISO   |              |                |
|               |    |      | SPI1_MISO   |              | MICO_SPI_1_MISO|
|               |    |      | TIM3_CH1    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_9   | 9  | B  5 | I2C1_SMBA   |              |                |
|               |    |      | GPIO        |              |                |
|               |    |      | CAN2_RX     |              |                |
|               |    |      | SPI1_MOSI   |              |                |
|               |    |      | SPI3_MOSI   |              | MICO_SPI_1_MOSI|
|               |    |      | TIM3_CH2    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_10  | 10 | B  8 | GPIO        |              |                |
|               |    |      | TIM4_CH3    |              | MICO_PWM_1     |
|               |    |      | TIM10_CH1   |              |                |
|               |    |      | I2C1_SCL    |              |                |
|               |    |      | CAN1_RX     |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_11  | 11 | A  1 | GPIO        |EasyLink_BUTTON |              |
|               |    |      | TIM5_CH2    |              |                |
|               |    |      | TIM2_CH2    |              |                |
|               |    |      | ADC123_IN1  |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_12  | 12 | C  2 | GPIO        |              |                |
|               |    |      | ADC123_ IN12|              | MICO_ADC_2     |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_13  | 13 | B 14 | GPIO        |              |                |
|               |    |      | TIM1_CH2N   |              |                |
|               |    |      | TIM12_CH1   |              | MICO_PWM_2     |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_14  | 14 | C  6 | GPIO        |              |                |
|               |    |      | TIM3_CH1    |              |                |
|               |    |      | TIM8_CH1    |              |                |
|               |    |      | USART6_TX   |              | MICO_UART_2_TX |
|---------------+----+------+-------------+--------------+----------------|
|               | 15 | GND  |             |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_16  | 16 | B 1  | GPIO        |  RF_LED      |                |
|               |    |      | TIM3_CH4    |              |                |
|               |    |      | TIM8_CH3N   |              |                |
|               |    |      | TIM1_CH4N   |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 17 |nReset|             |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_18  | 18 | A 15 | GPIO        |              |                |
|               |    |      | JTDI        |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_19  | 19 | B 11 | GPIO        |              |                |
|               |    |      | TIM2_CH4    |              | MICO_PWM_3     |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_20  | 20 | A 12 | GPIO        |              |                |
|               |    |      | USART1_RTS  |              |                |
|               |    |      | CAN1_TX     |              |                |
|               |    |      | TIM1_ETR    |              |                |
|               |    |      | OTG_FS_DP   |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_21  | 21 | A 11 | GPIO        |              |                |
|               |    |      | USART1_CTS  |              |                |
|               |    |      | CAN1_RX     |              |                |
|               |    |      | TIM1_CH4    |              |                |
|               |    |      | OTG_FS_DM   |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_22  | 22 | A  9 | GPIO        |              |                |
|               |    |      | USART1_TX   |STDIO_UART_TX |                |
|               |    |      | TIM1_CH2    |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_23  | 23 | A 10 | GPIO        |              |                |
|               |    |      | USART1_RX   |STDIO_UART_RX |                |
|               |    |      | TIM1_CH3    |              |                |
|               |    |      | OTG_FS_ID   |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 24 | VCC  |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 25 | GND  |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 26 | NC   |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 27 | BOOT0|             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 28 | A 14 | JTCK-SWCLK  |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_29  | 29 | A  0 | GPIO        |StandBy/WakeUp|                |
|               |    |      | TIM2_CH1_ETR|              |                |
|               |    |      | TIM5_CH1    |              |                |
|               |    |      | TIM8_ETR    |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_30  | 30 | B  9 | GPIO        | Status_Sel   |                |
|               |    |      | TIM4_CH4    |              |                |
|               |    |      | TIM11_CH1   |              |                |
|               |    |      | I2C1_SDA    |              |                |
|               |    |      | CAN1_TX     |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_SYS_LED  |    | B  0 | GPIO        |              |                |
+---------------+----+--------------------+--------------+----------------+
*
*/
  
#define MICO_UNUSED 0xFF

typedef enum
{
    MICO_SYS_LED,
    MICO_RF_LED,
    BOOT_SEL,
    MFG_SEL,
    EasyLink_BUTTON,
    MICO_GPIO_1 ,
    MICO_GPIO_2,
    //MICO_GPIO_3,
    MICO_GPIO_4,
    MICO_GPIO_5,
    MICO_GPIO_6,
    MICO_GPIO_7,
    MICO_GPIO_8,
    MICO_GPIO_9,
    MICO_GPIO_10,
    //MICO_GPIO_11,
    MICO_GPIO_12,
    MICO_GPIO_13,
    MICO_GPIO_14,
    //MICO_GPIO_15,
    //MICO_GPIO_16,
    //MICO_GPIO_17,
    MICO_GPIO_18,
    MICO_GPIO_19,
    MICO_GPIO_20,
    MICO_GPIO_21,
    MICO_GPIO_22,
    MICO_GPIO_23,
    //MICO_GPIO_24,
    //MICO_GPIO_25,
    //MICO_GPIO_26,
    //MICO_GPIO_27,
    //MICO_GPIO_28,
    MICO_GPIO_29,
    //MICO_GPIO_30,
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
  MICO_FLASH_MAX,
  MICO_FLASH_NONE,
} mico_flash_t;

typedef enum
{
  MICO_PARTITION_USER_MAX
} mico_user_partition_t;

#ifdef BOOTLOADER
#define STDIO_UART          MICO_UART_1
#define STDIO_UART_BAUDRATE (921600) 
#else
#define STDIO_UART          MICO_UART_2
#define STDIO_UART_BAUDRATE (115200) 
#endif

#define UART_FOR_APP     MICO_UART_1
#define MFG_TEST         MICO_UART_1
#define CLI_UART         MICO_UART_2

/* Components connected to external I/Os*/
#define Standby_SEL      (MICO_GPIO_29)

/* I/O connection <-> Peripheral Connections */
#define MICO_I2C_CP      (MICO_I2C_1)


#ifdef __cplusplus
} /*extern "C" */
#endif

