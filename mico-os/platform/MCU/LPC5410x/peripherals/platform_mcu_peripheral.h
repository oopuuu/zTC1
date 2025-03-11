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

#include "chip.h"   // rocky: this include makes almost all sources depend on chip.h, should try to remove
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

#define DMA_MAX_XFER_CNT		1024
/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/* GPIO port */
typedef LPC_GPIO_T      platform_gpio_port_t;

/* UART port */
typedef LPC_USART_T     platform_uart_port_t;

/* SPI port */
typedef LPC_SPI_T       platform_spi_port_t;

/* I2C port */
typedef LPC_I2C_T       platform_i2c_port_t;

/* GPIO alternate function */
typedef uint8_t       platform_gpio_alternate_function_t;

/* Peripheral clock function */
typedef void (*platform_peripheral_clock_function_t)(uint32_t clock, FunctionalState state );

typedef LPC_DMA_T       dma_registers_t;
typedef FunctionalState functional_state_t;
typedef uint32_t        peripheral_clock_t;

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct
{
    LPC_DMA_CHANNEL_T*  stream;         // Magicoe DMA_Stream_TypeDef* stream;
    uint32_t            channel;
    IRQn_Type           irq_vector;
    uint32_t            complete_flags;
    uint32_t            error_flags;
} platform_dma_config_t;

typedef struct
{
    uint8_t               port;
    uint8_t               pin_number;
} platform_gpio_t;

typedef struct
{
    LPC_ADC_T*             port;        // Magicoe ADC_TypeDef*           port;
    uint8_t                channel;
    uint32_t               adc_peripheral_clock;
    ADC_SEQ_IDX_T          seqIndex;    /* Wecan Modify 2015.07.08 */
    const platform_gpio_t* pin;
} platform_adc_t;


#define PWMNDX_SCTL_BASE		0x80UL
#define PWMNDX_SCTH_BASE		0xC0UL

typedef struct
{
	union
	{
    	LPC_SCT_T*          port;         // Magicoe TIM_TypeDef*           tim;
    	LPC_SCT_T* 			pSCT;
    	LPC_TIMER_T* 		pTMR;
    };
   	uint8_t pwmNdx;	   	// 0-7F : Timer ; 80-FF : SCT
   	uint8_t portNdx;	// port & pin
   	uint8_t pinNdx;
   	uint8_t pinMux;
} platform_pwm_t;


/* DMA can be enabled by setting SPI_USE_DMA */
typedef struct
{
    LPC_SPI_T*                 			 port;
    const platform_gpio_t*               pin_mosi;
    const platform_gpio_t*               pin_miso;
    const platform_gpio_t*               pin_clock;
    uint8_t dmaRxChnNdx;
    uint8_t dmaTxChnNdx;
} platform_spi_t;

typedef struct
{
    const platform_spi_t*               peripheral;

    mico_semaphore_t		        sem_xfer_done;
    mico_mutex_t                        spi_mutex;

    uint8_t					  isTxDone;
    uint8_t					  isRxDone;
    uint8_t					  isRx;
    uint8_t					  xferErr;		// 0 = no err, bit0:txErr, bit1:RxErr
} platform_spi_driver_t;

typedef struct
{
    uint8_t unimplemented;
} platform_spi_slave_driver_t;


typedef struct _platform_i2c_cb_t
{
	const uint8_t *txBuff;		/*!< Pointer to array of bytes to be transmitted */
	uint8_t *rxBuff;			/*!< Pointer memory where bytes received from I2C be stored */

	volatile uint16_t txSz; 	/*!< Number of bytes in transmit array,
									 if 0 only receive transfer will be carried on */
	volatile uint16_t rxSz; 	/*!< Number of bytes to received,
									 if 0 only transmission we be carried on */
	volatile uint16_t status;	/*!< Status of the current I2C transfer */
	uint8_t isProbe;

	uint8_t slaveAddr;			/*!< 7-bit I2C Slave address */
	uint16_t	 retries;	 /* Number of times to retry the message */
	const void *pDev;

    #ifndef BOOTLOADER
    mico_semaphore_t semXfer;
    mico_mutex_t mtxXfer;
    #endif
}platform_i2c_cb_t;

typedef struct
{
    LPC_I2C_T*   port;

    uint8_t irqNdx;
    uint8_t hwNdx;

	platform_i2c_cb_t *pCtx;

} platform_i2c_t;

typedef struct
{
    mico_mutex_t              i2c_mutex;
} platform_i2c_driver_t;

typedef void (* wakeup_irq_handler_t)(void *arg);

typedef struct
{
	uint8_t dmaTxChnNdx;
	uint8_t irqNdx;
	uint8_t hwNdx;
	uint8_t isRxFifoEn;
    platform_uart_port_t*  port;
    const platform_gpio_t* pin_tx;
    const platform_gpio_t* pin_rx;
    const platform_gpio_t* pin_cts;
    const platform_gpio_t* pin_rts;
} platform_uart_t;

typedef struct
{
    platform_uart_t*           peripheral;
    ring_buffer_t*             pRxRing;
	uint32_t bufFillThsld;		// when ring buffer used size >= bufFillThsld, signal RX complete

    uint8_t *pBuf;				// if pRxRing is empty, then use this liner buffer, given by caller
    uint8_t *pBufEnd;


//#ifndef BOOTLOADER
    mico_semaphore_t           rx_complete;
    mico_semaphore_t           tx_complete;
    mico_mutex_t               tx_mutex;
    mico_semaphore_t           sem_wakeup;
//#else
//    volatile bool              rx_complete;
//    volatile bool              tx_complete;
//#endif
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    volatile OSStatus          last_receive_result;
    volatile OSStatus          last_transmit_result;

} platform_uart_driver_t;

typedef struct
{
    uint32_t                   flash_type;
    uint32_t                   flash_start_addr;
    uint32_t                   flash_length;
} platform_flash_t;

typedef struct
{
    platform_flash_t*          peripheral;
    volatile bool              initialized;
    mico_mutex_t               flash_mutex;
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

uint8_t  platform_uart_get_port_number       ( platform_uart_port_t* uart );
void     platform_uart_irq                   ( platform_uart_driver_t* driver );
void     platform_uart_tx_dma_irq            ( platform_uart_driver_t* driver );
void     platform_uart_rx_dma_irq            ( platform_uart_driver_t* driver );

uint8_t  platform_spi_get_port_number        ( platform_spi_port_t* spi );


extern void disable_interrupts(void);
extern void enable_interrupts(void);

#ifdef __cplusplus
} /* extern "C" */
#endif





