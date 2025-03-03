/*
 * Copyright (C) 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <lowlevel_drivers.h>
#include <board.h>
#include <wm_os.h>

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
	uint32_t tempRegValue;

	/* Enable USB clock */
	CLK_RefClkEnable(CLK_REFCLK_USB);
	/* Wait for USB clock ready */
	while(CLK_GetClkStatus(CLK_REFCLK_USB) == RESET);

	/* Power on the USB phy */
	/* No longer for 32M, it's for USB PHY power */
	PMU->ANA_GRP_CTRL0.WORDVAL |= 0x7;

	SYS_CTRL->USB_CTRL.BF.PHY_RESET_SEL = 0x1;
	SYS_CTRL->USB_CTRL.BF.SOFT_PHY_RESET = 0x0;

	_os_delay(1);

	SYS_CTRL->USB_CTRL.BF.PHY_RESET_SEL = 0x0;

	/* Added on Jun 10. Power up OTG PHY */
	USBC->PHY_REG_OTG_CONTROL.BF.PU_OTG = 1;

	SYS_CTRL->USB_CTRL.WORDVAL |= 0x7;
	USBC->ANA_CONTRL_1.BF.PU_ANA = 0x1;  /* Addr: 234 */

	_os_delay(1);

	/* PLL VCO calibaration */
	tempRegValue = USBC->PLL_CONTROL_0.WORDVAL & 0xFFFFC000;
	/* REFDIV = 0x8, FBDIV = 0x64*/;
	tempRegValue |= 0x1064;
	/* default: REFDIV = 0x8, FBDIV = 0x64 */
	USBC->PLL_CONTROL_0.WORDVAL = tempRegValue;

	USBC->PLL_CONTROL_1.BF.VCOCAL_START = 0x1;   /* Addr: 208 */
	/* Wait for at least 400uS */
	_os_delay(1);
	/* Check USB PLL ready flag */
	while((USBC->PLL_CONTROL_1.WORDVAL & 0x00008000) != 0x00008000)
	{
		//Sleep
	}
	/* RCAL */
	USBC->TX_CHANNEL_CONTRL_0.BF.RCAL_START = 0x1;
	/* Wait for at least 400uS */
	_os_delay(1);
	USBC->TX_CHANNEL_CONTRL_0.BF.RCAL_START = 0x0;

	/* Set pinmux as usb function */
	board_usb_pin_config();
}
