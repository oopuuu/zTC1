/*
 * Copyright (C) 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <wmtypes.h>
#include <lowlevel_drivers.h>

void arch_start_cyccnt()
{
	/* Enable Cortex-M3 DWT unit */
	CoreDebug->DEMCR |= ((1 << 24));
	/* Enable cpu cycle counter */
	DWT->CTRL |= (0x1);
	/* Reset cpu cycle counter */
	DWT->CYCCNT = 0;
}

uint32_t arch_get_cyccnt()
{
	/* 32 bit counter will overflow based on frequency at which cpu is
	 * running, e.g. approx 21 seconds for 200MHz.
	 */
	return DWT->CYCCNT;
}

void arch_stop_cyccnt()
{
	/* Disable cpu cycle counter */
	DWT->CTRL &= ~(0x1);
	/* Disable Cortex-M3 DWT unit */
	CoreDebug->DEMCR &= (~(1 << 24));
}
