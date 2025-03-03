/**
 ******************************************************************************
 * @file    platform.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provides all MICO Peripherals mapping table and platform
 *          specific functions.
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

#include "mico_platform.h"
#include "platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_config.h"
#include "platform_logging.h"
#include "CheckSumUtils.h"
#include "keypad/gpio_button/button.h"

#ifdef USE_MiCOKit_EXT
#include "MiCOKit_EXT/micokit_ext.h"
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

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
extern WEAK void PlatformEasyLinkButtonClickedCallback(void);
extern WEAK void PlatformEasyLinkButtonLongPressedCallback(void);
extern WEAK void bootloader_start(void);

/******************************************************
 *               Variables Definitions
 ******************************************************/

const platform_gpio_t platform_gpio_pins[] =
{
	/* Common GPIOs for internal use */
	[MICO_SYS_LED   ] = {HAL_GPIO_38},
	[MICO_RF_LED	] = {HAL_GPIO_35},
	[BOOT_SEL		] = {HAL_GPIO_34},
	[MFG_SEL		] = {HAL_GPIO_33},
	[EasyLink_BUTTON] = {HAL_GPIO_6 },
	[STDIO_UART_RX	] = {HAL_GPIO_36},
	[STDIO_UART_TX	] = {HAL_GPIO_37},
	/* GPIOs for external use */
	[MICO_GPIO_59] = {HAL_GPIO_59}, //01,SWDIO
	[MICO_GPIO_60] = {HAL_GPIO_60},	//02,SWCLK
	[MICO_GPIO_39] = {HAL_GPIO_39}, //23
	[MICO_GPIO_0 ] = {HAL_GPIO_0 }, //07,UART0_RST
	[MICO_GPIO_32] = {HAL_GPIO_32}, //12,SPI_CS
	[MICO_GPIO_30] = {HAL_GPIO_30}, //13,SPI_MISO
	[MICO_GPIO_29] = {HAL_GPIO_29}, //14,SPI_MOSI
	[MICO_GPIO_31] = {HAL_GPIO_31}, //15,SPI_CLK
	[MICO_GPIO_35] = {HAL_GPIO_35}, //18
	[MICO_GPIO_57] = {HAL_GPIO_57}, //B1,ADC_IN0
	[MICO_GPIO_58] = {HAL_GPIO_58}, //B2,ADC_IN1	
	[MICO_GPIO_27] = {HAL_GPIO_27}, //20,I2C_SCL
	[MICO_GPIO_28] = {HAL_GPIO_28}, //19,I2C_SDA

	[MICO_GPIO_2 ] = {HAL_GPIO_2 }, //10,UART2_RX
	[MICO_GPIO_3 ] = {HAL_GPIO_3 }, //09,UART2_TX
};

const platform_i2c_t platform_i2c_peripherals[] =
{
	[MICO_I2C_1] =
	{
	},
};

platform_i2c_driver_t platform_i2c_drivers[MICO_I2C_MAX];

const platform_uart_t platform_uart_peripherals[] =
{
	[MICO_UART_1] =
	{
		.port = HAL_UART_1,
	},
	[MICO_UART_2] =
	{
		.port = HAL_UART_0,
	},
};
platform_uart_driver_t platform_uart_drivers[MICO_UART_MAX];

const platform_spi_t platform_spi_peripherals[] =
{
	[	MICO_SPI_1	] =
	{
	}
};

platform_spi_driver_t platform_spi_drivers[MICO_SPI_MAX];

const platform_adc_t platform_adc_peripherals[] =
{
	[	MICO_ADC_1	] =
	{
		HAL_ADC_CHANNEL_0
	},
	[	MICO_ADC_2	] =
	{
		HAL_ADC_CHANNEL_1
	},
};

const platform_pwm_t platform_pwm_peripherals[] =
{
  [MICO_PWM_1]  =
  {
  },
};

/* Flash memory devices */
const platform_flash_t platform_flash_peripherals[] =
{
	[MICO_FLASH_EMBEDDED] =
	{
		.flash_type = FLASH_TYPE_EMBEDDED,
		.flash_start_addr = 0x00,
		.flash_length = 0x400000,
	},
};

platform_flash_driver_t platform_flash_drivers[MICO_FLASH_MAX];

/* Logic partition on flash devices */
const mico_logic_partition_t mico_partitions[] =
{
	[MICO_PARTITION_BOOTLOADER] =
	{
		.partition_owner = MICO_FLASH_EMBEDDED,
		.partition_description = "Bootloader",
		.partition_start_addr = 0x00,
		.partition_length = 0x8000,    //32k bytes
		.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[MICO_PARTITION_RF_FIRMWARE] =
	{
		.partition_owner = MICO_FLASH_EMBEDDED,
		.partition_description = "RF Firmware",
		.partition_start_addr = 0x8000,
		.partition_length = 0x71000,  //452k bytes
		.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[MICO_PARTITION_ATE] =
	{
		.partition_owner = MICO_FLASH_EMBEDDED,
		.partition_description = "ATE",
		.partition_start_addr = 0x79000,
		.partition_length = 0x55000, //340k bytes
		.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[MICO_PARTITION_APPLICATION] =
	{
		.partition_owner = MICO_FLASH_EMBEDDED,
		.partition_description = "Application",
		.partition_start_addr = 0xCE000,
		.partition_length = 0x100000, //1M bytes
		.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[MICO_PARTITION_OTA_TEMP] =
	{
		.partition_owner = MICO_FLASH_EMBEDDED,
		.partition_description = "OTA Storage",
		.partition_start_addr = 0x1CE000,
		.partition_length = 0x100000, //1M bytes
		.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[MICO_PARTITION_PARAMETER_1] =
	{
		.partition_owner = MICO_FLASH_EMBEDDED,
		.partition_description = "PARAMETER1",
		.partition_start_addr = 0x2CE000,
		.partition_length = 0x4000, //16k bytes
		.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[MICO_PARTITION_PARAMETER_2] =
	{
		.partition_owner = MICO_FLASH_EMBEDDED,
		.partition_description = "PARAMETER1",
		.partition_start_addr = 0x2D2000,
		.partition_length = 0x4000, //16k bytes
		.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[MICO_PARTITION_FILESYS] =
	{
		.partition_owner = MICO_FLASH_EMBEDDED,
		.partition_description = "FILESYS",
		.partition_start_addr = 0x2D6000,
		.partition_length = 0x100000, //1M bytes
		.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	}
};

/******************************************************
 *               Function Definitions
 ******************************************************/

void platform_init_peripheral_irq_priorities(void)
{
}

void init_platform(void)
{
	button_init_t init;

	MicoGpioInitialize((mico_gpio_t) MICO_SYS_LED, OUTPUT_PUSH_PULL);
	MicoGpioOutputLow((mico_gpio_t) MICO_SYS_LED);
	MicoGpioInitialize((mico_gpio_t) MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL);
	MicoGpioOutputHigh((mico_gpio_t) MICO_RF_LED);

	MicoGpioInitialize((mico_gpio_t) BOOT_SEL, INPUT_PULL_UP);
	MicoGpioInitialize((mico_gpio_t) MFG_SEL, INPUT_PULL_UP);

	init.gpio = EasyLink_BUTTON;
	init.pressed_func = PlatformEasyLinkButtonClickedCallback;
	init.long_pressed_func = PlatformEasyLinkButtonLongPressedCallback;
	init.long_pressed_timeout = 5000;

	button_init(IOBUTTON_EASYLINK, init);

#ifdef USE_MiCOKit_EXT
	dc_motor_init();
	dc_motor_set(0);

	rgb_led_init();
	rgb_led_open(0, 0, 0);
#endif

	platform_rtc_init();

}

#ifdef BOOTLOADER

//#define BOOTLOADER_DEBUG

void init_platform_bootloader( void )
{

	MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
	MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
	MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_PUSH_PULL );
	MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );

	MicoGpioInitialize((mico_gpio_t)BOOT_SEL, INPUT_PULL_UP);
	MicoGpioInitialize((mico_gpio_t)MFG_SEL, INPUT_PULL_UP);

#ifdef USE_MiCOKit_EXT
	dc_motor_init( );
	dc_motor_set( 0 );

	rgb_led_init();
	rgb_led_open(0, 0, 0);
#endif

#ifdef BOOTLOADER_DEBUG
	static const platform_uart_config_t bootloader_debug_uart_config =
	{	.baud_rate = 921600, .data_width = DATA_WIDTH_8BIT, .parity =
		NO_PARITY, .stop_bits = STOP_BITS_1, .flow_control =
		FLOW_CONTROL_DISABLED, .flags = 0,};
	platform_uart_init(&platform_uart_drivers[UART_FOR_APP],
			&platform_uart_peripherals[UART_FOR_APP], &bootloader_debug_uart_config, NULL);
#endif /* #ifndef BOOTLOADER_DEBUG */

	return;
}

#ifdef BOOTLOADER_DEBUG
#include <stdarg.h>
int bootloader_log( const char *format, ... )
{
	static uint8_t log_buf[1024];
	va_list ap;
	va_start(ap, format);
	int size = vsnprintf(log_buf, sizeof(log_buf) - 1, format, ap);
	va_end(ap);
	MicoUartSend(UART_FOR_APP, log_buf, size);
	return size;
}
#endif /* #ifndef BOOTLOADER_DEBUG */

#endif

void MicoSysLed(bool onoff)
{
	if (onoff) {
		MicoGpioOutputLow((mico_gpio_t) MICO_SYS_LED);
	}
	else {
		MicoGpioOutputHigh((mico_gpio_t) MICO_SYS_LED);
	}
}

void MicoRfLed(bool onoff)
{
	if (onoff) {
		MicoGpioOutputLow((mico_gpio_t) MICO_RF_LED);
	}
	else {
		MicoGpioOutputHigh((mico_gpio_t) MICO_RF_LED);
	}
}

#ifdef USE_MiCOKit_EXT
// add test mode for MiCOKit-EXT board,check Arduino_D5 pin when system startup
bool MicoExtShouldEnterTestMode(void)
{
	if ( MicoGpioInputGet((mico_gpio_t) Arduino_D5) == false) {
		return true;
	}
	else {
		return false;
	}
}
#endif

bool MicoShouldEnterMFGMode(void)
{
	if (MicoGpioInputGet((mico_gpio_t) BOOT_SEL) == false
			&& MicoGpioInputGet((mico_gpio_t) MFG_SEL) == false)
		return true;
	else
		return false;
}

bool MicoShouldEnterBootloader(void)
{
	if (MicoGpioInputGet((mico_gpio_t) BOOT_SEL) == false
			&& MicoGpioInputGet((mico_gpio_t) MFG_SEL) == true)
		return true;
	else
		return false;
}

bool MicoShouldEnterATEMode(void)
{
	if (MicoGpioInputGet((mico_gpio_t) BOOT_SEL) == false
			&& MicoGpioInputGet((mico_gpio_t) EasyLink_BUTTON) == false)
		return true;
	else
		return false;
}
