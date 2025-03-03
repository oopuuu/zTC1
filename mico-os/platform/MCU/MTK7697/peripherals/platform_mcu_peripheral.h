/**
 ******************************************************************************
 * @file    platform_mcu_peripheral.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide all the headers of functions for stm32f2xx platform
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#pragma once

//#include "mt7687.h"
#include "hal.h"
#include "mico_rtos.h"
#include "RingBufferUtils.h"

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

/******************************************************
 *                 Type Definitions
 ******************************************************/

/* GPIO port */
typedef hal_gpio_pin_t platform_gpio_port_t;

/* UART port */
typedef hal_uart_port_t platform_uart_port_t;

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
	platform_gpio_port_t pin;
} platform_gpio_t;

typedef struct
{
	platform_uart_port_t port;
} platform_uart_t;

typedef struct
{
	platform_uart_t* peripheral;
	volatile bool initialized;
} platform_uart_driver_t;

typedef struct
{
    uint32_t flash_type;
	uint32_t flash_start_addr;
	uint32_t flash_length;
	uint32_t flash_protect_opt;
} platform_flash_t;

typedef struct
{
	const platform_flash_t* peripheral;
	mico_mutex_t flash_mutex;
	volatile bool initialized;
} platform_flash_driver_t;

typedef struct
{
	uint8_t	channel;
} platform_adc_t;

typedef struct
{
	uint8_t unimplemented;
} platform_pwm_t;

typedef struct
{
	uint8_t unimplemented;
} platform_spi_t;

typedef struct
{
	platform_spi_t* peripheral;
	mico_mutex_t spi_mutex;
} platform_spi_driver_t;

typedef struct
{
	uint8_t unimplemented;
} platform_spi_slave_driver_t;

typedef struct
{
	uint8_t unimplemented;
} platform_i2c_t;

typedef struct
{
	mico_mutex_t i2c_mutex;
} platform_i2c_driver_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus platform_rtc_init(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

