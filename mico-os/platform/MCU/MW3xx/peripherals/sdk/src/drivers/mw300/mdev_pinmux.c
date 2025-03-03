/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * mdev_pinmux.c: generic Pinmux helper functions
 */

#include <wmtypes.h>
#include <wmstdio.h>
#include <mdev_pinmux.h>
#include <wm_os.h>

/* Please note this depends on mw300_pinmux.h */
int pinmux_drv_get_gpio_func(int pin)
{
	if (pin < GPIO_0 || pin > GPIO_49)
		return -WM_E_INVAL;

	if (((pin >= GPIO_6) && (pin <= GPIO_10))
	    || ((pin >= GPIO_22) && (pin <= GPIO_26))
	    || ((pin >= GPIO_28) && (pin <= GPIO_33))) {
		return PINMUX_FUNCTION_1;
	} else {
		return PINMUX_FUNCTION_0;
	}
}
