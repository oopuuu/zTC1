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
EMW3265 on EMB-3265-A platform pin definitions ...
+-------------------------------------------------------------------------+
| Enum ID       |Pin | STM32| Peripheral  |    Board     |   Peripheral   |
|               | #  | Port | Available   |  Connection  |     Alias      |
|---------------+----+------+-------------+--------------+----------------|
|               | 1  | NC   |             |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_2   | 2  | B  2 |   GPIO      |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 3  |  NC  |             |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_4   | 4  | A  7 | TIM1_CH1N   |              |                |
|               |    |      | TIM3_CH2    |              |                |
|               |    |      | SPI1_MOSI   |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_5   | 5  | A  15| JTDI        |              |                |
|               |    |      | TIM2_CH1    |              |                |        
|               |    |      | TIM2_ETR    |              |                |
|               |    |      | SPI1_NSS    |              |                |
|               |    |      | SPI3_NSS    |              |                |
|               |    |      | USART1_TX   |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_6   | 6  | B  3 | TIM2_CH2    |              |                |
|               |    |      | GPIO        |              |                |        
|               |    |      | SPI1_SCK    |              |                |
|               |    |      | USART1_RX   |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_7   | 7  | B  4 | JTRST       |              |                |
|               |    |      | GPIO        |              |                |
|               |    |      | SDIO_D0     |              |                |
|               |    |      | TIM3_CH1    |              |                |
|               |    |      | SPI1_MISO   |              |                |
|               |    |      | SPI3_MISO   |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_8   | 8  | A  2 | TIM2_CH3    |              | MICO_UART_1_TX |
|               |    |      | TIM5_CH3    |              |                |
|               |    |      | TIM9_CH1    |              |                |
|               |    |      | USART2_TX   |              |                |
|               |    |      | GPIO        |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_9   | 9  | A  1 | TIM2_CH2    |EasyLink_BUTTON|               |
|               |    |      | TIM5_CH2    |              |                |
|               |    |      | USART2_RTS  |              |                |
|               |    |      | GPIO        |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 10 | VBAT |             |
|---------------+----+------+-------------+--------------+----------------|
|               | 11 | NC   |             |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_12  | 12 | A  3 | TIM2_CH4    |              | MICO_UART_1_RX |
|               |    |      | TIM5_CH4    |              |                |
|               |    |      | TIM9_CH2    |              |                |
|               |    |      | USART2_RX   |              |                |
|               |    |      | GPIO        |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 13 | NRST |             |              |  MICRO_RST_N   |
|---------------+----+------+-------------+--------------+----------------|
|               | 14 |WAKE_UP             |              |                |
|---------------+----+------+-------------+--------------+----------------|
|               | 15 | NC   |             |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_16  | 16 | C 13 |     -       |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_17  | 17 | B 10 |  TIM2_CH3   | MICO_SYS_LED |                |
|               |    |      |  I2C2_SCL   |              |                |
|               |    |      |  GPIO       |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_18  | 18 | B  9 | TIM4_CH4    |              |                |
|               |    |      | TIM11_CH1   |              |                |
|               |    |      | I2C1_SDA    |              |                |
|               |    |      | GPIO        |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_19  | 19 | B 12 | GPIO        |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 20 | GND  |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 21 | GND  |             |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_22  | 22 | B  3 |             |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_23  | 23 | A 15 | GPIO        |              |  JTAG_TDI      |
|               |    |      | USART1_TX   |STDIO_UART_RX |  SPI1_SSN      |
|               |    |      | TIM2_CH1    |              |                |
|               |    |      | TIM2_ETR    |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_24  | 24 | B  4 |             |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_25  | 25 | A 14 | JTCK-SWCLK  |  SWCLK       |                |
|               |    |      |  GPIO       |              |                |  
+---------------+----+--------------------+--------------+----------------+
|MICO_GPIO_26   | 26 | A 13 | JTMS-SWDIO  |  SWDIO       |                |
|               |    |      |  GPIO       |              |                |    
+---------------+----+--------------------+--------------+----------------+
|MICO_GPIO_27   | 27 | A 12 | TIM1_ETR    |              | USART1_RTS     |
|               |    |      | USART1_RTS  |              |                |    
|               |    |      | USART6_RX   |              |                |    
|               |    |      | USB_FS_DP   |              |                |    
|               |    |      | GPIO        |              |                |    
+---------------+----+--------------------+--------------+----------------+
|               | 28 | NC   |             |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_29  | 29 | A 10 | GPIO        |              |                |
|               |    |      | TIM1_CH3    |              |                |
|               |    |      | USART1_RX   |              |                |
|               |    |      | USB_FS_ID   |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_30  | 30 | B  6 | GPIO        |              |                |
|               |    |      | TIM4_CH1    |              |                |
|               |    |      | USART1_TX   |              |                |
|               |    |      | I2C1_SCL    |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_SYS_LED  | 31 | B  8 | GPIO        |              |                |
|               |    |      | TIM4_CH3    |              |                |
|               |    |      | TIM10_CH1   |              |                |  
+---------------+----+--------------------+--------------+----------------+
|               | 32 |  NC  |             |              |                | 
+---------------+----+--------------------+--------------+----------------+  
| MICO_GPIO_33  | 33 | B 13 | TIM1_CH1N   |              |                |  
|               |    |      | SPI2_SCK    |              |                |  
|               |    |      | GPIO        |              |                | 
+---------------+----+--------------------+--------------+----------------+  
| MICO_GPIO_34  | 34 | A  5 | TIM2_CH1    |              |                |  
|               |    |      | GPIO        |              |                | 
+---------------+----+--------------------+--------------+----------------+  
| MICO_GPIO_35  | 35 | A  11| TIM1_CH4    |              |  USART1_CTS    |  
|               |    |      | SPI4_MISO   |              |                |  
|               |    |      | USART1_CTS  |              |                |  
|               |    |      | USART6_TX   |              |                |  
|               |    |      | USB_FS_DM   |              |                |  
|               |    |      | GPIO        |              |                |
+---------------+----+--------------------+--------------+----------------+  
| MICO_GPIO_36  | 36 | B  1 | TIM1_CH3N   | BOOT_SEL     |                |  
|               |    |      | TIM3_CH4    |              |                |  
|               |    |      | GPIO        |              |                | 
+---------------+----+--------------------+--------------+----------------+  
| MICO_GPIO_37  | 37 | B  0 | TIM1_CH2N   | MFG_SEL      |                |  
|               |    |      | TIM3_CH3    |              |                | 
|               |    |      | GPIO        |              |                | 
+---------------+----+--------------------+--------------+----------------+  
| MICO_GPIO_38  | 38 | A  4 | USART2_CK   | MICO_RF_LED |                | 
|               |    |      | GPIO        |              |                | 
+---------------+----+--------------------+--------------+----------------+  
|               | 39 | VDD  |             |              |                | 
+---------------+----+--------------------+--------------+----------------+  
|               | 40 | VDD  |             |              |                |
+---------------+----+--------------------+--------------+----------------+  
|               | 41 | ANT  |             |              |                |
+---------------+----+--------------------+--------------+----------------+  
Notes
1. These mappings are defined in <MICO-SDK>/Platform/BCM943362WCD4/platform.c
2. STM32F2xx Datasheet  -> http://www.st.com/web/en/resource/technical/document/datasheet/CD00237391.pdf
3. STM32F2xx Ref Manual -> http://www.st.com/web/en/resource/technical/document/reference_manual/CD00225773.pdf
*/

typedef enum
{
    BOOT_SEL, 
    MFG_SEL, 
    EasyLink_BUTTON, 
    FLASH_PIN_SPI_CS,
    FLASH_PIN_SPI_CLK,
    FLASH_PIN_SPI_MOSI,
    FLASH_PIN_SPI_MISO,
    
    MICO_GPIO_2,
    MICO_GPIO_8,
    MICO_GPIO_9,
    MICO_GPIO_12,
    MICO_GPIO_14,
    //MICO_GPIO_16,  //????
    MICO_GPIO_17,
    MICO_GPIO_18,
    MICO_GPIO_19,
//    MICO_GPIO_27,  
//    MICO_GPIO_29,
//    MICO_GPIO_30,
    MICO_GPIO_31,
//    MICO_GPIO_33,
//    MICO_GPIO_34,
//    MICO_GPIO_35,
    MICO_GPIO_36,
    MICO_GPIO_37,
    //MICO_GPIO_38,
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
    MICO_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    MICO_PWM_NONE,
} mico_pwm_t;

typedef enum
{
    MICO_ADC_2,
    MICO_ADC_MAX, /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
    MICO_ADC_NONE,
} mico_adc_t;


typedef enum
{
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

#ifdef BOOTLOADER
#define STDIO_UART       (MICO_UART_1)
#define STDIO_UART_BAUDRATE (921600)
#else
#define STDIO_UART       (MICO_UART_1)
#define STDIO_UART_BAUDRATE (115200) 
#endif

#define UART_FOR_APP     (MICO_UART_1)
#define MFG_TEST         (MICO_UART_1)
#define CLI_UART         (MICO_UART_1)

/* Components connected to external I/Os*/
#define USE_MICO_SPI_FLASH
#define SFLASH_SUPPORT_MACRONIX_PARTS 
//#define SFLASH_SUPPORT_SST_PARTS
//#define SFLASH_SUPPORT_WINBOND_PARTS

#define MICO_SYS_LED    (MICO_GPIO_19)
#define MICO_RF_LED     (MICO_GPIO_31)

/* Arduino extention connector */
#define Arduino_RXD         (MICO_GPIO_NONE)
#define Arduino_TXD         (MICO_GPIO_NONE)
#define Arduino_D2          (MICO_GPIO_NONE)
#define Arduino_D3          (MICO_GPIO_NONE)
#define Arduino_D4          (MICO_GPIO_NONE) 
#define Arduino_D5          (MICO_GPIO_NONE)  
#define Arduino_D6          (MICO_GPIO_14) 
#define Arduino_D7          (MICO_GPIO_NONE)

#define Arduino_D8          (MICO_GPIO_31)
#define Arduino_D9          (MICO_GPIO_NONE)
#define Arduino_CS          (MICO_GPIO_2)
#define Arduino_SI          (FLASH_PIN_SPI_MOSI)
#define Arduino_SO          (FLASH_PIN_SPI_MISO)
#define Arduino_SCK         (FLASH_PIN_SPI_CLK)
#define Arduino_SDA         (MICO_GPIO_18)
#define Arduino_SCL         (MICO_GPIO_17)

#define Arduino_A0          (MICO_ADC_NONE)
#define Arduino_A1          (MICO_ADC_NONE)
#define Arduino_A2          (MICO_ADC_NONE)
#define Arduino_A3          (MICO_ADC_2)
#define Arduino_A4          (MICO_ADC_NONE)
#define Arduino_A5          (MICO_ADC_NONE)

#define Arduino_I2C         (MICO_I2C_1)
#define Arduino_SPI         (MICO_SPI_1)
#define Arduino_UART        (MICO_UART_2)

#ifdef USE_MiCOKit_EXT
#define MICO_I2C_CP         (MICO_I2C_NONE)
#include "micokit_ext_def.h"
#else
#define MICO_I2C_CP         (MICO_I2C_NONE)
#endif //USE_MiCOKit_EXT



#ifdef __cplusplus
} /*extern "C" */
#endif


