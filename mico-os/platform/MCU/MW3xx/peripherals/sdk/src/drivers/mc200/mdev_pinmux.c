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

/* Please note this depends on mc200_pinmux.h */
int pinmux_drv_get_gpio_func(int pin)
{
	if (pin < GPIO_0 || pin > GPIO_79)
		return -WM_E_INVAL;

	if (((pin >= GPIO_20) && (pin <= GPIO_26))
		|| (pin == GPIO_57) || (pin == GPIO_58)) {
		return PINMUX_FUNCTION_1;
	} else {
		return PINMUX_FUNCTION_0;
	}
}
