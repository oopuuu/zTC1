/*! \file cyccnt.h
 * \brief CPU cycles counter module
 */

/*
 * Copyright (C) 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef _ARCH_H_
#define _ARCH_H_

/** Start CPU cycles counter
 *
 * This function enables DWT unit in Cortex-M3 core and starts CPU cycles
 * counter, resetting its value to 0.
 */
void arch_start_cyccnt();

/** Get CPU cycles counter value
 * This function returns current cpu cycle counter (32 bit) value.
 *
 * \return uint32_t value
 * \note CPU cycle counter in Cortex-M3 core is 32 bit and hence it will
 * overflow based on configured frequency at which cpu is running, e.g. approx
 * 21 seconds for 200MHz frequency. This should be taken into account.
 */
uint32_t arch_get_cyccnt();

/** Stop CPU cycles counter
 *
 * This function disables DWT unit in Cortex-M3 core and stops CPU cycles
 * counter.
 */
void arch_stop_cyccnt();

#endif /* _ARCH_H_ */
