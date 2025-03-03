/*! \file generic_io.h
 * \brief Input/Output GPIO related data structures
 */

/*
 * Copyright (C) 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */
#ifndef __GENERIC_IO_H__
#define __GENERIC_IO_H__

#include <mdev_gpio.h>

/** GPIO needs to be pulled low to activate */
#define GPIO_ACTIVE_LOW		0
/** GPIO needs to be pulled high to activate */
#define GPIO_ACTIVE_HIGH	1

/** Input GPIO Configuration type.
 * Normally used for push buttons. */
typedef struct {
	/** The GPIO number associated with this input. */
	int8_t gpio;
	/** The type of the input. Can be either \ref GPIO_ACTIVE_LOW
	 * or \ref GPIO_ACTIVE_HIGH. This indicates what the state of
	 * the GPIO will be when the button is pressed.
	 */
	bool type;
} input_gpio_cfg_t;

/** Output GPIO Configuration type.
 * Normally used for LED indicators or for driving loads. */
typedef struct {
	/** The GPIO number associated with this output. */
	int8_t gpio;
	/** The type of the output. Can be either \ref GPIO_ACTIVE_LOW
	 * or \ref GPIO_ACTIVE_HIGH.
	 */
	bool type;
} output_gpio_cfg_t;

#endif /* __HAP_IO_H__ */
