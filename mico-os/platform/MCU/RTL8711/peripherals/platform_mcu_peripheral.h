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

#include "rtl8195a.h"
#include "PinNames.h"
#include "objects.h"

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

 /* GPIOA to I */
#define NUMBER_OF_GPIO_PORTS      (8)

/* Interrupt line 0 to 15. Each line is shared among the same numbered pins across all GPIO ports */
#define NUMBER_OF_GPIO_IRQ_LINES  (16)

/* USART1 to 6 */
#define NUMBER_OF_UART_PORTS      (6)


/* Invalid UART port number */
#define INVALID_UART_PORT_NUMBER  (0xff)

 /* SPI1 to SPI3 */
#define NUMBER_OF_SPI_PORTS       (3)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/* GPIO port */
//typedef GPIO_TypeDef  platform_gpio_port_t;

/* UART port */
//typedef USART_TypeDef platform_uart_port_t;

/* SPI port */
//typedef SPI_TypeDef   platform_spi_port_t;

/* I2C port */
//typedef I2C_TypeDef   platform_i2c_port_t;

/* GPIO alternate function */
//typedef uint8_t       platform_gpio_alternate_function_t;

/* Peripheral clock function */
//typedef void (*platform_peripheral_clock_function_t)(uint32_t clock, FunctionalState state );

//typedef DMA_TypeDef     dma_registers_t;
//typedef FunctionalState functional_state_t;
//typedef uint32_t        peripheral_clock_t;


/******************************************************
 *                    Structures
 ******************************************************/
//typedef struct gtimer_s gtimer_t;

typedef struct
{
//    DMA_TypeDef*        controller;
//    DMA_Stream_TypeDef* stream;
    uint32_t            channel;
//    IRQn_Type           irq_vector;
    uint32_t            complete_flags;
    uint32_t            error_flags;
} platform_dma_config_t;

typedef struct
{
    PinName pin;	
    HAL_GPIO_PIN hal_pin;
} platform_gpio_t;

typedef struct analogin_s analogin_t;

typedef struct
{
    PinName pin;
    analogin_t obj; 
} platform_adc_t;

typedef struct
{
    PinName pin;
    uint8_t pwm_idx;
    uint8_t pin_sel;
    uint32_t period;
    uint32_t pulse;
} platform_pwm_t;


/* DMA can be enabled by setting SPI_USE_DMA */
typedef struct
{
    HAL_SSI_ADAPTOR spi_adp;
    HAL_SSI_OP      spi_op;
    u32 irq_handler;
    u32 irq_id;
    u32 state;
    PinName mosi;
    PinName miso;
    PinName sclk;
    PinName ssel;	
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
    PinName sda;
    PinName scl;
    SAL_I2C_MNGT_ADPT   SalI2CMngtAdpt;        
    SAL_I2C_HND_PRIV    SalI2CHndPriv;    
    HAL_I2C_INIT_DAT    HalI2CInitData;
    HAL_I2C_OP          HalI2COp;   
    IRQ_HANDLE          I2CIrqHandleDat;    
    HAL_GDMA_ADAPTER    HalI2CTxGdmaAdpt;   
    HAL_GDMA_ADAPTER    HalI2CRxGdmaAdpt;     
    HAL_GDMA_OP         HalI2CGdmaOp;
    IRQ_HANDLE          I2CTxGdmaIrqHandleDat;    
    IRQ_HANDLE          I2CRxGdmaIrqHandleDat;        
    SAL_I2C_USER_CB     SalI2CUserCB;   
    SAL_I2C_USERCB_ADPT SalI2CUserCBAdpt[SAL_USER_CB_NUM];    
    SAL_I2C_DMA_USER_DEF    SalI2CDmaUserDef;
} platform_i2c_t;

typedef struct
{
    mico_mutex_t              i2c_mutex;
} platform_i2c_driver_t;

typedef void (* wakeup_irq_handler_t)(void *arg);

typedef struct
{
    PinName tx;
    PinName rx;
    HAL_RUART_OP hal_uart_op;
    HAL_RUART_ADAPTER hal_uart_adp;
#ifdef CONFIG_GDMA_EN    
    UART_DMA_CONFIG   uart_gdma_cfg;
    HAL_GDMA_ADAPTER uart_gdma_adp_tx;
    HAL_GDMA_ADAPTER uart_gdma_adp_rx;
#endif    
} platform_uart_t;

typedef struct
{
    platform_uart_t*           peripheral;
    ring_buffer_t*             rx_buffer;
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


typedef struct flash_s flash_t;

typedef struct
{
    uint32_t                   flash_type;
    flash_t                    flash_obj;	
    uint32_t                   flash_start_addr;
    uint32_t                   flash_length;
    uint32_t                   flash_protect_opt;
    uint32_t                   flash_readonly_start; // can't erase and write
    uint32_t                   flash_readonly_len;
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
//uint8_t  platform_gpio_get_port_number       ( platform_gpio_port_t* gpio_port );
OSStatus platform_gpio_enable_clock          ( const platform_gpio_t* gpio );
//OSStatus platform_gpio_set_alternate_function( platform_gpio_port_t* gpio_port, uint8_t pin_number, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pull_up_down_type, uint8_t alternation_function );

OSStatus platform_mcu_powersave_init         ( void );

OSStatus platform_rtc_init                   ( void );
OSStatus platform_rtc_enter_powersave        ( void );
OSStatus platform_rtc_abort_powersave        ( void );
OSStatus platform_rtc_exit_powersave         ( uint32_t requested_sleep_time, uint32_t *cpu_sleep_time );

//uint8_t  platform_uart_get_port_number       ( platform_uart_port_t* uart );
void     platform_uart_irq                   ( platform_uart_driver_t* driver );
void     platform_uart_tx_dma_irq            ( platform_uart_driver_t* driver );
void     platform_uart_rx_dma_irq            ( platform_uart_driver_t* driver );

//uint8_t  platform_spi_get_port_number        ( platform_spi_port_t* spi );

#ifdef __cplusplus
} /* extern "C" */
#endif





