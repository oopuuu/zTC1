/*
 *
 * ============================================================================
 * Copyright (c) 2008-2015   Marvell International Ltd. All Rights Reserved.
 *
 *                         Marvell Confidential
 * ============================================================================
 */

#include <mc200.h>
#include <mc200_driver.h>
#include <mc200_pinmux.h>
#include <mc200_clock.h>
#include <mc200_gpio.h>
#include <mc200_pmu.h>
#include <board.h>

/****************************************************************************
 * @brief      Initialize the USB device hardware
 *
 * @param[in]  none
 *
 * @return none
 *
 ***************************************************************************/
void USB_HwInit(void)
{
	uint32_t XtalFreq, Fref;
	CLK_AupllConfig_Type usbClkCfg;

	XtalFreq = board_main_xtal();

	/* Fvco clock is 540MHz, for getting USB clock 54MHz */
	uint32_t FVco = 540000000;

	/* 2MHz < (XtalFreq / refDiv) < 4MHz */
	usbClkCfg.refDiv = (uint32_t) (XtalFreq / 2000000);
	Fref = (uint32_t) (XtalFreq / usbClkCfg.refDiv);

	/* FVco = (XtalFreq * fdDiv) / refDiv */
	usbClkCfg.fbDiv =(uint32_t) (((double) FVco *
			(double) usbClkCfg.refDiv) / (double) XtalFreq);
	usbClkCfg.usbPostDiv = 0;

	/*!< Select icp and updateRate per Fref */
	/*!< icp = 4, updateRate = 1, if 2 MHz <= Fref <= 2.35 MHz */
	/*!< icp = 3, updateRate = 1, if 2.35 MHz < Fref <= 2.75 MHz */
	/*!< icp = 6, updateRate = 0, if 2.75 MHz < Fref <= 3.25 MHz */
	/*!< icp = 5, updateRate = 0, if 3.25 MHz < Fref <= 4 MHz */
	if (2000000 <= Fref && Fref <= 2350000) {
		usbClkCfg.icp = 4;
		usbClkCfg.updateRate = 1;
	} else if (2350000 < Fref && Fref <= 2750000) {
		usbClkCfg.icp = 3;
		usbClkCfg.updateRate = 1;
	} else if (2750000 < Fref && Fref <= 3250000) {
		usbClkCfg.icp = 6;
		usbClkCfg.updateRate = 0;
	} else if (3250000 < Fref && Fref <= 4000000) {
		usbClkCfg.icp = 5;
		usbClkCfg.updateRate = 0;
	}

	/* Power on the USB phy */
	SYS_CTRL->USB_PHY_CTRL.BF.REG_PU_USB = 1;

	/* Set the USB clock */
	CLK_AupllEnable(&usbClkCfg);

	/* Enable the USB clock */
	CLK_ModuleClkEnable(CLK_USBC);
	CLK_AupllClkOutEnable(CLK_AUPLL_USB);

	/* Set pinmux as usb function */
	board_usb_pin_config();
}
