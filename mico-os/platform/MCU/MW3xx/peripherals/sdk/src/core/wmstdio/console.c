/* 
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/** console.c: Functions for mapping wmstdio to the serial console.
 */
#include <string.h>

#include <wmstdio.h>
#include <mdev_uart.h>

static mdev_t * mdev_uart_ = NULL;

stdio_funcs_t serial_console_funcs;

mdev_t *serial_console_init(UART_ID_Type uart_port, int baud)
{
	mdev_uart_ = uart_drv_open(uart_port, baud);
	c_stdio_funcs = &serial_console_funcs;
	return mdev_uart_;
}

int serial_console_get_portid(int *port_id)
{
	*port_id = uart_drv_get_portid(mdev_uart_);
	return 0;
}

int serial_console_printf(char *str)
{
	int len;

	if (! mdev_uart_)
		return 0;

	len = strlen(str);
	uart_drv_write(mdev_uart_, (uint8_t *)str, len);
	return len;
}

int serial_console_flush()
{
	uart_drv_tx_flush(mdev_uart_);
	return 0;
}

int serial_console_getchar(uint8_t *inbyte_p)
{
	return uart_drv_read(mdev_uart_, inbyte_p, 1);
}

int serial_console_putchar(char *inbyte_p)
{
	if (!mdev_uart_)
		return 0;
	return uart_drv_write(mdev_uart_, (uint8_t *)inbyte_p, 1);
}

stdio_funcs_t serial_console_funcs = { 
	serial_console_printf, 
	serial_console_flush,
	serial_console_getchar,
	serial_console_putchar,
};
