/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * This is a board specific configuration file for
 * Saratoga Validation (Development) Board V2.0.
 * The name/version is printed on the board.
 */

#include <wmtypes.h>
#include <wmerrno.h>
#include <wm_os.h>
#include <board.h>
#include <lowlevel_drivers.h>
#include <generic_io.h>

int board_main_xtal()
{
	return 32000000;
}

int board_main_osc()
{
	return -WM_FAIL;
}

int board_cpu_freq()
{
	return 200000000;
}

int board_32k_xtal()
{
	return true;
}

int board_32k_osc()
{
	return false;
}

int board_rc32k_calib()
{
	return false;
}

int board_card_detect()
{
	GPIO_IO_Type cdlvl;

	GPIO_PinMuxFun(GPIO_9, GPIO9_GPIO9);
	GPIO_SetPinDir(GPIO_9, GPIO_INPUT);
	cdlvl = GPIO_ReadPinLevel(GPIO_9);
	if (cdlvl == GPIO_IO_HIGH)
		return false;

	return true;
}

void board_sdio_pdn()
{
	GPIO_PinMuxFun(GPIO_29, GPIO29_GPIO29);
	GPIO_SetPinDir(GPIO_29, GPIO_OUTPUT);
	GPIO_WritePinOutput(GPIO_29, GPIO_IO_LOW);
}

void board_sdio_pwr()
{
	GPIO_WritePinOutput(GPIO_29, GPIO_IO_HIGH);
}

void board_sdio_reset()
{
	board_sdio_pdn();
	_os_delay(200);
	board_sdio_pwr();
	_os_delay(200);
}

int board_sdio_pdn_support()
{
	return true;
}

int board_button_3()
{
	return GPIO_30;
}

int board_button_pressed(int pin)
{
	if (pin < 0)
		return false;

	GPIO_PinMuxFun(pin, PINMUX_FUNCTION_0);
	GPIO_SetPinDir(pin, GPIO_INPUT);
	if (GPIO_ReadPinLevel(pin) == GPIO_IO_HIGH)
		return true;

	return false;
}

void board_gpio_power_on()
{
	/* Enable the Boot override button */
	PMU_PowerOnVDDIO(PMU_VDDIO_D1);

	/* Turn On SYS_3V3_EN */
	PMU_PowerOnVDDIO(PMU_VDDIO_D0);
	GPIO_PinMuxFun(GPIO_17, GPIO17_GPIO17);
	GPIO_SetPinDir(GPIO_17, GPIO_OUTPUT);
	GPIO_WritePinOutput(GPIO_17, GPIO_IO_LOW);
}

void board_uart_pin_config(int id)
{
	switch (id) {
	case UART0_ID:
		GPIO_PinMuxFun(GPIO_74, GPIO74_UART0_TXD);
		GPIO_PinMuxFun(GPIO_75, GPIO75_UART0_RXD);
		break;
	case UART1_ID:
		GPIO_PinMuxFun(GPIO_65, GPIO65_UART1_TXD);
		GPIO_PinMuxFun(GPIO_66, GPIO66_UART1_RXD);
		break;
	case UART2_ID:
	case UART3_ID:
		/* Not implemented */
		break;
	}
}

void board_i2c_pin_config(int id)
{
	switch (id) {
	case I2C0_PORT:
		/* Not implemented */
		break;
	case I2C1_PORT:
		GPIO_PinMuxFun(GPIO_4, GPIO4_I2C1_SDA);
		GPIO_PinMuxFun(GPIO_5, GPIO5_I2C1_SCL);
		break;
	case I2C2_PORT:
		/* Not implemented */
		break;
	}
}

void board_usb_pin_config()
{
	GPIO_PinMuxFun(GPIO_57, GPIO57_USB_DP);
	GPIO_PinMuxFun(GPIO_58, GPIO58_USB_DM);
	GPIO_PinMuxFun(GPIO_45, GPIO45_USB2_DRVVBUS);
}

void board_ssp_pin_config(int id, bool cs)
{
	switch (id) {
	case SSP0_ID:
		GPIO_PinMuxFun(GPIO_32, GPIO32_SSP0_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_33, GPIO33_SSP0_FRM);
		GPIO_PinMuxFun(GPIO_34, GPIO34_SSP0_RXD);
		GPIO_PinMuxFun(GPIO_35, GPIO35_SSP0_TXD);
		break;
	case SSP1_ID:
		/* Not implemented */
		break;
	case SSP2_ID:
		GPIO_PinMuxFun(GPIO_40, GPIO40_SSP2_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_41, GPIO41_SSP2_FRM);
		GPIO_PinMuxFun(GPIO_42, GPIO42_SSP2_RXD);
		GPIO_PinMuxFun(GPIO_43, GPIO43_SSP2_TXD);
		break;
	}
}

void board_sdio_pin_config()
{
	GPIO_PinMuxFun(GPIO_50, GPIO50_SDIO_LED);
	GPIO_PinMuxFun(GPIO_51, GPIO51_SDIO_CLK);
	GPIO_PinMuxFun(GPIO_52, GPIO52_SDIO_3);
	GPIO_PinMuxFun(GPIO_53, GPIO53_SDIO_2);
	GPIO_PinMuxFun(GPIO_54, GPIO54_SDIO_1);
	GPIO_PinMuxFun(GPIO_55, GPIO55_SDIO_0);
	GPIO_PinMuxFun(GPIO_56, GPIO56_SDIO_CMD);
}

/*
 * Application Specific APIs
 * Define these only if your application needs/uses them.
 */

output_gpio_cfg_t board_led_1()
{
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_11,
		.type = GPIO_ACTIVE_LOW,
	};

	return gcfg;
}

output_gpio_cfg_t board_led_2()
{
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_64,
		.type = GPIO_ACTIVE_LOW,
	};

	return gcfg;
}

output_gpio_cfg_t board_led_3()
{
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_68,
		.type = GPIO_ACTIVE_LOW,
	};

	return gcfg;
}

output_gpio_cfg_t board_led_4()
{
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_63,
		.type = GPIO_ACTIVE_LOW,
	};

	return gcfg;
}

int board_button_1()
{
	return GPIO_28;
}

int board_button_2()
{
	return GPIO_30;
}

void board_lcd_backlight_on()
{
	GPIO_PinMuxFun(GPIO_8, PINMUX_FUNCTION_0);
	GPIO_SetPinDir(GPIO_8, GPIO_OUTPUT);
	GPIO_WritePinOutput(GPIO_8, GPIO_IO_HIGH);
}
void board_lcd_backlight_off()
{
	GPIO_PinMuxFun(GPIO_8, PINMUX_FUNCTION_0);
	GPIO_SetPinDir(GPIO_8, GPIO_OUTPUT);
	GPIO_WritePinOutput(GPIO_8, GPIO_IO_LOW);
}

void board_lcd_reset()
{
	GPIO_PinMuxFun(GPIO_7, PINMUX_FUNCTION_0);
	GPIO_SetPinDir(GPIO_7, GPIO_OUTPUT);
	GPIO_WritePinOutput(GPIO_7, GPIO_IO_LOW);
	_os_delay(200);
	GPIO_WritePinOutput(GPIO_7, GPIO_IO_HIGH);
}

int board_wifi_host_wakeup()
{
	return -WM_FAIL;
}

int board_wakeup0_functional()
{
	return true;
}

int  board_wakeup1_functional()
{
	return false;
}

int board_wakeup0_wifi()
{
	return false;
}

int board_wakeup1_wifi()
{
	return false;
}

unsigned int board_antenna_select()
{
	return 1;
}
