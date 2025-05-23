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
/* EMW3031 PIN
PIN	FUN1	FUN2	FUN3	FUN4
1/3	SWD_CLK	GPIO_7		
2/4	SWD_DIO	GPIO_8		
5	WAKE_UP0	GPIO_22		
6	WAKE_UP1	GPIO_23		
7	UART2_RTS	GPIO_47	SPI2_CS	ADC0_5
8	UART2_CTS	GPIO_46	SPI2_CLK	ADC0_4
9	UART2_TXD	GPIO_48	SPI2_TXD	ADC0_6
10	UART2_RXD	GPIO_49	SPI2_RXD	ADC0_7
11	RESET			
12	SPI0_CS	GPIO_1	PWM0_1	UART0_RTS
13	SPI0_MISO	GPIO_3	PWM0_3	UART0_RXD
14	SPI0_MOSI	GPIO_2	PWM0_2	UART0_TXD
15	SPI0_CLK	GPIO_0	PWM0_0	UART0_CTS
16	VCC_3V3			
17	GND			
18	GPIO_41			
19	I2C0_SDA	GPIO_4	PWM0_4	
20	I2C0_CLK	GPIO_5	PWM0_5	
21/24	UART1_TXD	GPIO_44		ADC0_2
22/25	UART1_RXD	GPIO_45		ADC0_3
23	GPIO_6			
26A2	I2C1_SDA	GPIO_9		
27A1	I2C1_SCL	GPIO_10		
28A3	GPIO_26			32K_OUT
29A4	GND			
30B1	GPIO_43			ADC0_1
31B2	GPIO_42			ADC0_0
32B3	GPIO_40			
33B4	GPIO_39		
MICO_SYS_LED GPIO_16 
*/

typedef enum
{
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
    MICO_GPIO_15,
    MICO_GPIO_16,
    MICO_GPIO_17,
    MICO_GPIO_18,
    MICO_GPIO_19,
    MICO_GPIO_20,
    MICO_GPIO_21,
    MICO_GPIO_22,
    MICO_GPIO_23,
    MICO_GPIO_24,
    MICO_GPIO_25,
    MICO_GPIO_26,
    MICO_GPIO_27,
    MICO_GPIO_28,
    MICO_GPIO_29,
    MICO_GPIO_30,
    MICO_GPIO_31,
    MICO_GPIO_32,
    MICO_GPIO_33,
    MICO_SYS_LED,
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
    MICO_I2C_2,
    MICO_I2C_MAX, /* Denotes the total number of I2C port aliases. Not a valid I2C alias */
    MICO_I2C_NONE,
} mico_i2c_t;

typedef enum
{
    MICO_IIS_1,
	MICO_IIS_2,
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
    MICO_ADC_1,
    MICO_ADC_2,
    MICO_ADC_3,
    MICO_ADC_4,
    MICO_ADC_5,
    MICO_ADC_6,
    MICO_ADC_7,
    MICO_ADC_8,
    MICO_ADC_MAX, /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
    MICO_ADC_NONE,
} mico_adc_t;

typedef enum
{
    MICO_UART_1,
    MICO_UART_2,
    MICO_UART_3,
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
} mico_user_partition_t;

#define STDIO_UART          MICO_UART_1
#define STDIO_UART_BAUDRATE (115200) 

#define UART_FOR_APP     MICO_UART_2
#define MFG_TEST         MICO_UART_2
#define CLI_UART         MICO_UART_1

/* Components connected to external I/Os*/
#define Standby_SEL      (MICO_GPIO_29)

/* I/O connection <-> Peripheral Connections */
#define BOOT_SEL        MICO_GPIO_19
#define MFG_SEL         MICO_GPIO_20
#define MICO_RF_LED     MICO_GPIO_30
#define EasyLink_BUTTON MICO_GPIO_23

typedef struct {
	int country_code;
	int enable_healthmon;
	int dhcp_arp_check;
} mico_system_config_t;

/* Arduino extention connector */
#define Arduino_RXD         (MICO_GPIO_10)
#define Arduino_TXD         (MICO_GPIO_9)
#define Arduino_D2          (MICO_GPIO_NONE)
#define Arduino_D3          (MICO_GPIO_18)
#define Arduino_D4          (MICO_GPIO_5) 
#define Arduino_D5          (MICO_GPIO_6)  
#define Arduino_D6          (MICO_GPIO_32) 
#define Arduino_D7          (MICO_GPIO_NONE)

#define Arduino_D8          (MICO_GPIO_28)
#define Arduino_D9          (MICO_GPIO_33)
#define Arduino_CS          (MICO_GPIO_12)
#define Arduino_SI          (MICO_GPIO_14)
#define Arduino_SO          (MICO_GPIO_13)
#define Arduino_SCK         (MICO_GPIO_15)
#define Arduino_SDA         (MICO_GPIO_26)
#define Arduino_SCL         (MICO_GPIO_27)

#define Arduino_A0          (MICO_ADC_NONE)
#define Arduino_A1          (MICO_ADC_NONE)
#define Arduino_A2          (MICO_ADC_6)
#define Arduino_A3          (MICO_ADC_5)
#define Arduino_A4          (MICO_ADC_NONE)
#define Arduino_A5          (MICO_ADC_NONE)

#define Arduino_I2C         (MICO_I2C_2)
#define Arduino_SPI         (MICO_SPI_1)
#define Arduino_UART        (MICO_UART_2)

#ifdef USE_MiCOKit_EXT
#define MICO_I2C_CP         (Arduino_I2C)
#include "micokit_ext_def.h"
#else
#define MICO_I2C_CP         (MICO_I2C_NONE)
#endif //USE_MiCOKit_EXT

#ifdef __cplusplus
} /*extern "C" */
#endif

