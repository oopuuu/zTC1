/**
 ******************************************************************************
 * @file    platform_uart.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide UART driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "platform.h"
#include "platform_core.h"
#include "platform_peripheral.h"
#include "debug.h"

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
#define VFIFO_SIZE (1024)

static uint8_t uart0_tx_fifo[VFIFO_SIZE];
static uint8_t uart0_rx_fifo[VFIFO_SIZE];
static uint8_t uart1_tx_fifo[VFIFO_SIZE];
static uint8_t uart1_rx_fifo[VFIFO_SIZE];

typedef struct
{
	hal_gpio_pin_t tx_pin;
	uint8_t tx_func;
	uint8_t *tx_fifo_buf;
	uint16_t tx_size;
	mico_semaphore_t tx_semphr;
	mico_mutex_t tx_mutex;
	void (*tx_callback)(void);

	hal_gpio_pin_t rx_pin;
	uint8_t rx_func;
	uint8_t *rx_fifo_buf;
	uint16_t rx_size;
	mico_semaphore_t rx_semphr;
	void (*rx_callback)(void);
} uart_config_t;
/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/
extern uint32_t mico_rtos_get_time(void);
/******************************************************
 *        Static Function Declarations
 ******************************************************/
static void uart0_rx_callback(void);
static void uart0_tx_callback(void);
static void uart1_rx_callback(void);
static void uart1_tx_callback(void);
/******************************************************
 *               Function Definitions
 ******************************************************/
uart_config_t uart_config[HAL_UART_MAX] =
{
		[HAL_UART_0] =
		{
				.rx_pin = HAL_GPIO_2,
				.rx_func = HAL_GPIO_2_UART1_RX_CM4,
				.rx_size = 0,
				.rx_fifo_buf = uart0_rx_fifo,
				.rx_callback = uart0_rx_callback,
				.tx_pin = HAL_GPIO_3,
				.tx_func = HAL_GPIO_3_UART1_TX_CM4,
				.tx_size = 0,
				.tx_fifo_buf = uart0_tx_fifo,
				.tx_callback = uart0_tx_callback,
		},
		[HAL_UART_1] =
		{
				.rx_pin = HAL_GPIO_36,
				.rx_func = HAL_GPIO_36_UART2_RX_CM4,
				.rx_size = 0,
				.rx_fifo_buf = uart1_rx_fifo,
				.rx_callback = uart1_rx_callback,
				.tx_pin = HAL_GPIO_37,
				.tx_func = HAL_GPIO_37_UART2_TX_CM4,
				.tx_size = 0,
				.tx_fifo_buf = uart1_tx_fifo,
				.tx_callback = uart1_tx_callback,
		},
};

/**
 * Initialise the specified UART port
 *
 * @return @ref OSStatus
 */

OSStatus platform_uart_init(platform_uart_driver_t* driver,
		const platform_uart_t* peripheral, const platform_uart_config_t* config,
		ring_buffer_t* optional_ring_buffer)
{
	driver->peripheral = (platform_uart_t*) peripheral;
	if (driver->initialized == false) {
		mico_rtos_init_semaphore(&uart_config[peripheral->port].rx_semphr, 1);
		mico_rtos_init_semaphore(&uart_config[peripheral->port].tx_semphr, 1);
		mico_rtos_init_mutex(&uart_config[peripheral->port].tx_mutex);

		hal_gpio_init(uart_config[peripheral->port].rx_pin);
		hal_pinmux_set_function(uart_config[peripheral->port].rx_pin,
				uart_config[peripheral->port].rx_func);
		hal_gpio_init(uart_config[peripheral->port].tx_pin);
		hal_pinmux_set_function(uart_config[peripheral->port].tx_pin,
				uart_config[peripheral->port].tx_func);

		/* Configure UART port to dma mode */
		hal_uart_dma_config_t dma_config;
		dma_config.receive_vfifo_alert_size = 0;
		dma_config.receive_vfifo_buffer = uart_config[peripheral->port].rx_fifo_buf;
		dma_config.receive_vfifo_buffer_size = VFIFO_SIZE;
		dma_config.receive_vfifo_threshold_size = VFIFO_SIZE;
		dma_config.send_vfifo_buffer = uart_config[peripheral->port].tx_fifo_buf;
		dma_config.send_vfifo_buffer_size = VFIFO_SIZE;
		dma_config.send_vfifo_threshold_size = 0;
		hal_uart_dma_init(peripheral->port, &dma_config);

		hal_uart_dma_register_rx_callback(peripheral->port,
				uart_config[peripheral->port].rx_callback);
		hal_uart_dma_register_tx_callback(peripheral->port,
				uart_config[peripheral->port].tx_callback);
	}

	/* Configure UART port with basic function */
	hal_uart_config_t basic_config;
	switch (config->baud_rate) {
	case 9600:
		basic_config.baudrate = HAL_UART_BAUDRATE_9600;
		break;
	case 38400:
		basic_config.baudrate = HAL_UART_BAUDRATE_38400;
		break;
	case 57600:
		basic_config.baudrate = HAL_UART_BAUDRATE_57600;
		break;
	case 115200:
		basic_config.baudrate = HAL_UART_BAUDRATE_115200;
		break;
	case 230400:
		basic_config.baudrate = HAL_UART_BAUDRATE_230400;
		break;
	case 460800:
		basic_config.baudrate = HAL_UART_BAUDRATE_460800;
		break;
	case 921600:
		basic_config.baudrate = HAL_UART_BAUDRATE_921600;
		break;
	default:
		return kParamErr;
	}
	switch (config->parity) {
	case NO_PARITY:
		basic_config.parity = HAL_UART_PARITY_NONE;
		break;
	case ODD_PARITY:
		basic_config.parity = HAL_UART_PARITY_ODD;
		break;
	case EVEN_PARITY:
		basic_config.parity = HAL_UART_PARITY_EVEN;
		break;
	default:
		return kParamErr;
	}
	switch (config->stop_bits) {
	case STOP_BITS_1:
		basic_config.stop_bit = HAL_UART_STOP_BIT_1;
		break;
	case STOP_BITS_2:
		basic_config.stop_bit = HAL_UART_STOP_BIT_2;
		break;
	default:
		return kParamErr;
	}
	switch (config->data_width) {
	case DATA_WIDTH_5BIT:
		basic_config.word_length = HAL_UART_WORD_LENGTH_5;
		break;
	case DATA_WIDTH_6BIT:
		basic_config.word_length = HAL_UART_WORD_LENGTH_6;
		break;
	case DATA_WIDTH_7BIT:
		basic_config.word_length = HAL_UART_WORD_LENGTH_7;
		break;
	case DATA_WIDTH_8BIT:
		basic_config.word_length = HAL_UART_WORD_LENGTH_8;
		break;
	default:
		return kParamErr;
	}
	hal_uart_init(peripheral->port, &basic_config);

	driver->initialized = true;

	return kNoErr;
}

/**
 * Deinitialise the specified UART port
 *
 * @return @ref OSStatus
 */
OSStatus platform_uart_deinit(platform_uart_driver_t* driver)
{
	hal_uart_deinit(driver->peripheral->port);
	//Disable DMA interrupt, note that we use the DMA interrupt to process UART data
	NVIC_DisableIRQ((IRQn_Type) CM4_DMA_IRQ);

	mico_rtos_deinit_semaphore(&uart_config[driver->peripheral->port].rx_semphr);
	mico_rtos_deinit_semaphore(&uart_config[driver->peripheral->port].tx_semphr);
	mico_rtos_deinit_mutex(&uart_config[driver->peripheral->port].tx_mutex);
	driver->initialized = false;
	return kNoErr;
}

/**
 * Transmit data over the specified UART port
 *
 * @return @ref OSStatus
 */
OSStatus platform_uart_transmit_bytes(platform_uart_driver_t* driver,
		const uint8_t* data_out, uint32_t size)
{
	platform_uart_port_t port = driver->peripheral->port;
	uart_config_t *config = &uart_config[port];

	uint8_t in_interrupt = platform_is_in_interrupt_context() == MICO_TRUE ? 1 : 0;

	if(in_interrupt == 0){
		mico_rtos_lock_mutex(&config->tx_mutex);
	}

	uint8_t *send_start = (uint8_t*) data_out;
	uint32_t need_to_send = size;
	uint32_t send_size;
	if (need_to_send > hal_uart_get_available_send_space(port)) {
		while (1) {
			if (need_to_send > VFIFO_SIZE) {
				send_size = VFIFO_SIZE;
			}
			else {
				send_size = need_to_send;
			}

			if(in_interrupt == 0){
				hal_uart_dma_set_tx_threshold(port, VFIFO_SIZE - send_size + 1);
				hal_uart_dma_set_tx_interrupt(port, 1);
				mico_rtos_get_semaphore(&config->tx_semphr, MICO_NEVER_TIMEOUT);
			}else{
				while(!(hal_uart_get_available_send_space(port) > send_size));
			}
			send_size = hal_uart_dma_tx(port, send_start, send_size);
			send_start += send_size;
			need_to_send -= send_size;

			if (need_to_send == 0) {
				break;
			}
		}
	}
	else {
		hal_uart_dma_tx(port, send_start, need_to_send);
	}

	if(in_interrupt == 0){
		mico_rtos_unlock_mutex(&config->tx_mutex);
	}
	return kNoErr;
}

/**
 * Receive data over the specified UART port
 *
 * @return @ref OSStatus
 */
OSStatus platform_uart_receive_bytes(platform_uart_driver_t* driver,
		uint8_t* data_in, uint32_t expected_data_size, uint32_t timeout_ms)
{
	platform_uart_port_t port = driver->peripheral->port;
	uart_config_t *config = &uart_config[port];

	uint8_t *receive_start = data_in;
	uint32_t need_to_receive = expected_data_size;
	uint32_t receive_size;

	if (need_to_receive > hal_uart_get_available_receive_bytes(port)) {
		uint32_t start_time = mico_rtos_get_time();
		uint32_t wait_time = timeout_ms;
		mico_rtos_get_semaphore(&config->rx_semphr, MICO_NO_WAIT);
		while (1) {
			if (need_to_receive > VFIFO_SIZE) {
				receive_size = VFIFO_SIZE;
			}
			else {
				receive_size = need_to_receive;
			}

			hal_uart_dma_set_rx_threshold(port, receive_size - 1);
			hal_uart_dma_set_rx_interrupt(port, 1);
			if (mico_rtos_get_semaphore(&config->rx_semphr, wait_time) == kTimeoutErr) {
				return kTimeoutErr;
			}
			receive_size = hal_uart_dma_rx(port, receive_start, receive_size);
			if (timeout_ms != MICO_WAIT_FOREVER) {
				wait_time -= mico_rtos_get_time() - start_time;
			}
			receive_start += receive_size;
			need_to_receive -= receive_size;

			if (need_to_receive == 0) {
				break;
			}
		}
	}
	else {
		hal_uart_dma_rx(port, receive_start, need_to_receive);
	}

	return kNoErr;
}

/**
 * Get the received data length in ring buffer over the specified UART port
 *
 * @return
 */
uint32_t platform_uart_get_length_in_buffer(platform_uart_driver_t* driver)
{
	return hal_uart_get_available_receive_bytes(driver->peripheral->port);
}

static void uart0_rx_callback(void)
{
	mico_rtos_set_semaphore(&uart_config[HAL_UART_0].rx_semphr);
	hal_uart_dma_set_rx_interrupt(HAL_UART_0, 0);
}

static void uart0_tx_callback(void)
{
	mico_rtos_set_semaphore(&uart_config[HAL_UART_0].tx_semphr);
	hal_uart_dma_set_tx_interrupt(HAL_UART_0, 0);
}

static void uart1_rx_callback(void)
{
	mico_rtos_set_semaphore(&uart_config[HAL_UART_1].rx_semphr);
	hal_uart_dma_set_rx_interrupt(HAL_UART_1, 0);
}

static void uart1_tx_callback(void)
{
	mico_rtos_set_semaphore(&uart_config[HAL_UART_1].tx_semphr);
	hal_uart_dma_set_tx_interrupt(HAL_UART_1, 0);
}
