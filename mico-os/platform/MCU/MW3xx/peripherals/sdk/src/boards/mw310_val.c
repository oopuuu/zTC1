/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * This is a board specific configuration file for
 * the RD-88MW300 RD and AB-88MW300 baseboard
 * based on schematic dated 19th June 2014.
 * the MW310-88QFN Validation board
 * Based on schematic dated 23 April 2014
 *  Details:
 * By default the board file configures the CPU frequncy to 128MHz

 * The dev board has:
 * MAINXTAL 38.4MHz
 * Two push buttons being used as wakeup0(GPIO22) and wakeup1(GPIO23)
 */

#include <wmtypes.h>
#include <wmerrno.h>
#include <wm_os.h>
#include <board.h>
#include <lowlevel_drivers.h>
#ifdef CONFIG_ENABLE_MXCHIP
#include "MicoPlatform.h"
#endif
int board_main_xtal()
{
	/* MAINXTAL: 38.4MHZ */
	return 38400000;
}

int board_main_osc()
{
	return -WM_FAIL;
}

WEAK int board_cpu_freq()
{
	return 100000000;
}

WEAK int board_32k_xtal()
{
	return false;
}

WEAK int board_32k_osc()
{
	return false;
}

WEAK int board_rc32k_calib()
{
	return true;
}

void board_sdio_pdn()
{
/*
 * This cannot be done so easily
 * as MCU XTAL clock is tied
 * to WLAN
 * PMU_PowerDownWLAN();
 */
}

void board_sdio_pwr()
{
/*	PMU_PowerOnWLAN();*/
}

void board_sdio_reset()
{
	board_sdio_pdn();
	_os_delay(20);
	board_sdio_pwr();
	_os_delay(20);
}

int board_sdio_pdn_support()
{
	return true;
}

WEAK int board_button_pressed(int pin)
{
	if (pin < 0)
		return false;
	switch (pin) {
	/* GPIO_22 & GPIO_23 wakeup are
	 * configured as ACTIVE_HIGH */
	case GPIO_22:
	case GPIO_23:
		GPIO_PinMuxFun(pin, PINMUX_FUNCTION_0);
		GPIO_SetPinDir(pin, GPIO_INPUT);
		if (GPIO_ReadPinLevel(pin) == GPIO_IO_HIGH)
			return true;
		break;
	default:
		GPIO_PinMuxFun(pin, PINMUX_FUNCTION_0);
		GPIO_SetPinDir(pin, GPIO_INPUT);
		if (GPIO_ReadPinLevel(pin) == GPIO_IO_LOW)
			return true;
	}

	return false;
}

WEAK void board_gpio_power_on()
{
	/* Temp fix to get FW DNLD working */
	GPIO_PinMuxFun(GPIO_47, PINMUX_FUNCTION_7);
	/* Wakeup can be configured by adjusting the jumpers
	 * Setting it to ACTIVE_HIGH for now
	 */
	PMU_ConfigWakeupPin(PMU_GPIO22_INT, PMU_WAKEUP_LEVEL_HIGH);
	/* WAKEUP0 is Active low */
	PMU_ConfigWakeupPin(PMU_GPIO23_INT, PMU_WAKEUP_LEVEL_HIGH);
}

WEAK void board_uart_pin_config(int id)
{
	switch (id) {
	case UART0_ID:
		GPIO_PinMuxFun(GPIO_2, GPIO2_UART0_TXD);
		GPIO_PinMuxFun(GPIO_3, GPIO3_UART0_RXD);
		break;
	case UART1_ID:
		GPIO_PinMuxFun(GPIO_44, GPIO44_UART1_TXD);
		GPIO_PinMuxFun(GPIO_45, GPIO45_UART1_RXD);
	case UART2_ID:
		GPIO_PinMuxFun(GPIO_48, GPIO48_UART2_TXD);
		GPIO_PinMuxFun(GPIO_49, GPIO49_UART2_RXD);
		break;
	}
}

WEAK void board_i2c_pin_config(int id)
{
	switch (id) {
	case I2C0_PORT:
		/* Used for I2C EEPROM */
		GPIO_PinMuxFun(GPIO_4, GPIO4_I2C0_SDA);
		GPIO_PinMuxFun(GPIO_5, GPIO5_I2C0_SCL);
		break;
	case I2C1_PORT:
		/* Used for CODEC */
		GPIO_PinMuxFun(GPIO_25, GPIO25_I2C1_SDA);
		GPIO_PinMuxFun(GPIO_26, GPIO26_I2C1_SCL);
		break;
	}
}

WEAK void board_usb_pin_config()
{
	GPIO_PinMuxFun(GPIO_27, GPIO27_DRVVBUS);
}

WEAK void board_ssp_pin_config(int id, bool cs)
{
	/* To do */
	switch (id) {
	case SSP0_ID:
		GPIO_PinMuxFun(GPIO_0, GPIO0_SSP0_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_1, GPIO1_SSP0_FRM);
		GPIO_PinMuxFun(GPIO_2, GPIO2_SSP0_TXD);
		GPIO_PinMuxFun(GPIO_3, GPIO3_SSP0_RXD);
		break;
	case SSP1_ID:
		GPIO_PinMuxFun(GPIO_11, GPIO11_SSP1_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_12, GPIO12_SSP1_FRM);
		else {
			GPIO_PinMuxFun(GPIO_12, GPIO12_GPIO12);
			GPIO_SetPinDir(GPIO_12, GPIO_INPUT);
		}
		GPIO_PinMuxFun(GPIO_13, GPIO13_SSP1_TXD);
		GPIO_PinMuxFun(GPIO_14, GPIO14_SSP1_RXD);
		break;
	case SSP2_ID:
		break;
	}
}

WEAK output_gpio_cfg_t board_led_1()
{
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_40,
		.type = GPIO_ACTIVE_LOW,
	};

	return gcfg;
}

WEAK output_gpio_cfg_t board_led_2()
{
	/* LED2_OUT : GPIO45 */
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_45,
		.type = GPIO_ACTIVE_LOW,
	};

	return gcfg;
}

WEAK output_gpio_cfg_t board_led_3()
{
	output_gpio_cfg_t gcfg = {
		.gpio = -1,
	};

	return gcfg;
}

WEAK output_gpio_cfg_t board_led_4()
{
	output_gpio_cfg_t gcfg = {
		.gpio = -1,
	};

	return gcfg;
}

WEAK int board_button_1()
{
	/* SW1_IN: GPIO39 */
	GPIO_PinMuxFun(GPIO_39, PINMUX_FUNCTION_0);
	GPIO_SetPinDir(GPIO_39, GPIO_INPUT);
	return GPIO_39;
}

WEAK int board_button_2()
{
	/* SW2_IN : GPIO40 */
	GPIO_PinMuxFun(GPIO_40, PINMUX_FUNCTION_0);
	GPIO_SetPinDir(GPIO_40, GPIO_INPUT);
	return GPIO_40;
}

WEAK int board_button_3()
{
	return -WM_FAIL;
}

int board_wifi_host_wakeup()
{
	return 16;
}

int board_wakeup0_functional()
{
	return true;
}

int board_wakeup1_functional()
{
	return true;
}

int board_antenna_switch_ctrl()
{
	return false;
}

unsigned int board_antenna_select()
{
	return 1;
}
#ifdef CONFIG_ENABLE_MXCHIP

/*
FC_COMP_BOOT2       0x0 0x6000 0 boot2
FC_COMP_FW            0x6000 0x68000 0 mcufw
FC_COMP_FW            0x6E000 0x68000 0 mcufw
FC_COMP_PSM          0xD6000 0x4000 0 psm
FC_COMP_PSM          0xDA000 0x4000 0 psm
FC_COMP_ATE           0xDE000 0x4C000 0 ATE
FC_COMP_WLAN_FW 0x12a000 0x49000 0 wififw
FC_COMP_WLAN_FW 0x173000 0x49000 0 wififw
*/

const mico_logic_partition_t mico_partitions[] =
{
  [phy_PARTITION_BOOTLOADER] = // bootloader move to application
  {
    .partition_owner           = MICO_FLASH_NONE,
  },
  [phy_PARTITION_APPLICATION1] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "Application",
    .partition_start_addr      = 0x6000,
    .partition_length          = 0x88000,   //544k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_APPLICATION2] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "Application",
    .partition_start_addr      = 0x8E000,
    .partition_length          = 0x88000,   //544k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  
  [phy_PARTITION_PARAMETER_1] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER1",
    .partition_start_addr      = 0x116000,
    .partition_length          = 0x4000, // 16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_PARAMETER_2] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER2",
    .partition_start_addr      = 0x11A000,
    .partition_length          = 0x4000, //16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_ATE] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "ATE",
    .partition_start_addr      = 0x11E000,
    .partition_length          = 0x4C000, //304k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_RF_FIRMWARE1] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "RF Firmware",
    .partition_start_addr      = 0x16A000,
    .partition_length          = 0x49000, //292k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_RF_FIRMWARE2] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "RF Firmware",
    .partition_start_addr      = 0x1B3000,
    .partition_length          = 0x49000, //292k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
};


#define MXCHIP_GPIO(x) {.pin=GPIO_##x, .pinmux=GPIO##x##_GPIO##x,}
#if OLD_BOARD
/* MXCHIP board */
const platform_gpio_t platform_gpio_pins[] =
{
    [BOOT_SEL]        = MXCHIP_GPIO(39),
  	[MFG_SEL]         = MXCHIP_GPIO(40),
  	[EasyLink_BUTTON] = MXCHIP_GPIO(1),
  	[MICO_SYS_LED]    = MXCHIP_GPIO(23),
  	[MICO_RF_LED]     = MXCHIP_GPIO(24),
};
#else
/* MXCHIP board */
const platform_gpio_t platform_gpio_pins[] =
{
    [BOOT_SEL]        = MXCHIP_GPIO(4),
  	[MFG_SEL]         = MXCHIP_GPIO(5),
  	[EasyLink_BUTTON] = MXCHIP_GPIO(6),
  	[MICO_SYS_LED]    = MXCHIP_GPIO(16),
  	//[MICO_RF_LED]     = MXCHIP_GPIO(24),
  	[MICO_GPIO_12]    = MXCHIP_GPIO(0),
  	[MICO_GPIO_13]    = MXCHIP_GPIO(1),
  	[MICO_GPIO_14]    = MXCHIP_GPIO(2),
  	[MICO_GPIO_15]    = MXCHIP_GPIO(3),
};
#endif
const platform_uart_t platform_uart[] =
{
	[MICO_UART_1] = {.id = UART1_ID,},
	[MICO_UART_2] = {.id = UART2_ID,},
};

bool MicoShouldEnterBootloader( void )
{
  if ((MicoGpioInputGet((mico_gpio_t)BOOT_SEL) == false) &&
	  (MicoGpioInputGet((mico_gpio_t)EasyLink_BUTTON) == true) &&
	  (MicoGpioInputGet((mico_gpio_t)MFG_SEL) == true))
	  return true;
  else
  	return false;
}

bool MicoShouldEnterMFGMode( void )
{
  if ((MicoGpioInputGet((mico_gpio_t)BOOT_SEL) == false) &&
	  (MicoGpioInputGet((mico_gpio_t)MFG_SEL) == false))
	  return true;
  else
  	return false;
}

bool MicoShouldEnterATEMode( void )
{
  if ((MicoGpioInputGet((mico_gpio_t)BOOT_SEL) == false) &&
	  (MicoGpioInputGet((mico_gpio_t)EasyLink_BUTTON) == false) &&
	  (MicoGpioInputGet((mico_gpio_t)MFG_SEL) == true))
	  return true;
  else
  	return false;
}

void MicoRfLed(bool onoff)
{
    
}


#endif

