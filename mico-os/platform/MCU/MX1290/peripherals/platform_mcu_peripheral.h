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
#include "platform_cmsis.h"


#include "mico_rtos.h"
#include "RingBufferUtils.h"

//#include "lowlevel_drivers.h"
//#include "boot_flags.h"

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


/**
 * UART flow control for driver
 */
typedef enum
{
    FLOW_CONTROL_DISABLED_DRV,
    FLOW_CONTROL_CTS_DRV,
    FLOW_CONTROL_RTS_DRV,
    FLOW_CONTROL_CTS_RTS_DRV
} platform_uart_driver_flow_control_t;

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    uint8_t unimplemented;
} platform_gpio_t;

typedef struct
{
    uint8_t unimplemented;
} platform_adc_t;

typedef struct
{
    uint8_t unimplemented;
} platform_pwm_t;


/* DMA can be enabled by setting SPI_USE_DMA */
typedef struct
{
    uint8_t unimplemented;
} platform_spi_t;

typedef struct
{
    platform_spi_t*     peripheral;
    mico_mutex_t        spi_mutex;
    mico_bool_t         initialized;
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
    mico_mutex_t              i2c_mutex;
} platform_i2c_driver_t;

typedef void (* wakeup_irq_handler_t)(void *arg);

typedef struct
{
    uint32_t  port_id;
} platform_uart_t;

typedef struct
{
    uint8_t                    id;
    ring_buffer_t*             rx_ring_buffer;
    mico_semaphore_t           rx_complete;
    mico_semaphore_t           tx_complete;
    mico_mutex_t               tx_mutex;
    mico_semaphore_t           sem_wakeup;
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    volatile OSStatus          last_receive_result;
    volatile OSStatus          last_transmit_result;
    platform_uart_driver_flow_control_t   flow_control;
} platform_uart_driver_t;


typedef struct
{
    uint32_t                   flash_type;
    uint32_t                   flash_start_addr;
    uint32_t                   flash_length;
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
//int get_passive_firmware(void);
//int part_write_layout(void);



//OSStatus platform_gpio_irq_manager_init      ( void );
//uint8_t  platform_gpio_get_port_number       ( platform_gpio_port_t* gpio_port );
//OSStatus platform_gpio_enable_clock          ( const platform_gpio_t* gpio );
//OSStatus platform_gpio_set_alternate_function( platform_gpio_port_t* gpio_port, uint8_t pin_number, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pull_up_down_type, uint8_t alternation_function );
//
//OSStatus platform_mcu_powersave_init         ( void );
//
//OSStatus platform_rtc_init                   ( void );
//OSStatus platform_rtc_enter_powersave        ( void );
//OSStatus platform_rtc_abort_powersave        ( void );
//OSStatus platform_rtc_exit_powersave         ( uint32_t requested_sleep_time, uint32_t *cpu_sleep_time );
//
//uint8_t  platform_uart_get_port_number       ( platform_uart_port_t* uart );
//void     platform_uart_irq                   ( platform_uart_driver_t* driver );
//void     platform_uart_tx_dma_irq            ( platform_uart_driver_t* driver );
//void     platform_uart_rx_dma_irq            ( platform_uart_driver_t* driver );
//
//uint8_t  platform_spi_get_port_number        ( platform_spi_port_t* spi );

#ifdef __cplusplus
} /* extern "C" */
#endif





