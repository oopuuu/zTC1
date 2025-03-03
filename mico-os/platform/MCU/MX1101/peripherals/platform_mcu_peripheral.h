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

#include "gpio.h"
#include "uart.h"
#include "irqs.h"
#include "clk.h"
#include "uart.h"
#include "gpio.h"
#include "spi_flash.h"
#include "watchdog.h"
#include "timeout.h"
#include "cache.h"
#include "delay.h"
#include "RingBufferUtils.h"
#include "mico_rtos.h"
#include "platform_config.h"
#include "debug.h"
#include "common.h"
#include "platform_logging.h"
#include "wakeup.h"
#include "rtc.h"


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

#define NUMBER_OF_GPIO_IRQ_LINES  (5)

/* Invalid UART port number */
#define INVALID_UART_PORT_NUMBER  (0xff)

 /* SPI1 to SPI3 */
#define NUMBER_OF_SPI_PORTS       (3)

#define		GPIOA					(0x00)
#define		GPIOB					(0x0A)
#define		GPIOC					(0x14)

#define		FUART					(0x00)
#define		BUART					(0x01)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef enum
{
    FLASH_TYPE_EMBEDDED, 
    FLASH_TYPE_SPI,
} platform_flash_type_t;

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    uint8_t                   port;
    uint8_t                   pin;
} platform_gpio_t;

typedef struct
{
    const platform_gpio_t*    pin_adc;
} platform_adc_t;

typedef struct
{
    const platform_gpio_t*    pin_pwm;
} platform_pwm_t;


/* DMA can be enabled by setting SPI_USE_DMA */
typedef struct
{
    const platform_gpio_t*    pin_mosi;
    const platform_gpio_t*    pin_miso;
    const platform_gpio_t*    pin_clock;
} platform_spi_t;

typedef struct
{
    platform_spi_t*           peripheral;
    mico_mutex_t              spi_mutex;
} platform_spi_driver_t;

typedef struct
{
    uint8_t unimplemented;
} platform_spi_slave_driver_t;

typedef struct
{
    const platform_gpio_t*    pin_scl;
    const platform_gpio_t*    pin_sda;
} platform_i2c_t;

typedef struct
{
    mico_mutex_t              i2c_mutex;
} platform_i2c_driver_t;

typedef struct
{
    uint8_t                   uart;
    const platform_gpio_t*    pin_tx;
    const platform_gpio_t*    pin_rx;
    const platform_gpio_t*    pin_cts;
    const platform_gpio_t*    pin_rts;
} platform_uart_t;

typedef struct
{
    platform_uart_t*           peripheral;
    ring_buffer_t*             rx_buffer;
    volatile int               buart_fifo_head;
#ifndef NO_MICO_RTOS
    mico_semaphore_t           rx_complete;
    mico_semaphore_t           tx_complete;
    mico_mutex_t               tx_mutex;
    mico_semaphore_t           sem_wakeup;
#else
    volatile bool              rx_complete;
    volatile bool              tx_complete;
#endif
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    volatile OSStatus          last_receive_result;
    volatile OSStatus          last_transmit_result;
} platform_uart_driver_t;

typedef struct
{
    platform_flash_type_t      flash_type;
    uint32_t                   flash_start_addr;
    uint32_t                   flash_length;
    uint32_t                   flash_protect_opt;
} platform_flash_t;

typedef struct
{
    const platform_flash_t*    peripheral;
    mico_mutex_t               flash_mutex;
    volatile bool              initialized;
} platform_flash_driver_t;

/******************************************************
 *                 Global Variables
 ******************************************************/


/******************************************************
 *               Function Declarations
 ******************************************************/
OSStatus platform_gpio_irq_manager_init      ( void );

OSStatus platform_mcu_powersave_init         ( void );

OSStatus platform_rtc_init                   ( void );

void     platform_uart_irq                   ( platform_uart_driver_t* driver );


#ifdef __cplusplus
} /* extern "C" */
#endif





