/**
 ******************************************************************************
 * @file    platform_gpio.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide GPIO driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "platform.h"
#include "platform_peripheral.h"
#include "platform_logging.h"

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
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/**
 * Initialise the specified GPIO pin
 *
 * @param[in] gpio   : gpio pin
 * @param[in] config : pin configuration
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_init(const platform_gpio_t* gpio,
		platform_pin_config_t config)
{
	hal_pinmux_set_function(gpio->pin, 8); //gpio
	switch (config)
	{
	case INPUT_PULL_UP:
		hal_gpio_set_direction(gpio->pin, HAL_GPIO_DIRECTION_INPUT);
		hal_gpio_pull_up(gpio->pin);
		break;
	case INPUT_PULL_DOWN:
		hal_gpio_set_direction(gpio->pin, HAL_GPIO_DIRECTION_INPUT);
		hal_gpio_pull_down(gpio->pin);
		break;
	case INPUT_HIGH_IMPEDANCE:
		hal_gpio_set_direction(gpio->pin, HAL_GPIO_DIRECTION_INPUT);
		hal_gpio_disable_pull(gpio->pin);
		break;
	case OUTPUT_PUSH_PULL:
		hal_gpio_set_direction(gpio->pin, HAL_GPIO_DIRECTION_OUTPUT);
		break;
	case OUTPUT_OPEN_DRAIN_NO_PULL:
	case OUTPUT_OPEN_DRAIN_PULL_UP:
	default:
		return kParamErr;
	}

	return kNoErr;
}

/**
 * Deinitialise the specified GPIO pin
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_deinit(const platform_gpio_t* gpio)
{
	return kNoErr;
}

/**
 * Toggle the specified GPIO pin output high
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_output_high(const platform_gpio_t* gpio)
{
	hal_gpio_set_output(gpio->pin, HAL_GPIO_DATA_HIGH);
	return kNoErr;
}

/**
 * Toggle the specified GPIO pin output low
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_output_low(const platform_gpio_t* gpio)
{
	hal_gpio_set_output(gpio->pin, HAL_GPIO_DATA_LOW);
	return kNoErr;
}

/**
 * Toggle the specified GPIO pin
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_output_trigger(const platform_gpio_t* gpio)
{
	hal_gpio_toggle_pin(gpio->pin);
	return kNoErr;
}

/**
 * Retrieve logic level of the GPIO input pin specified
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
bool platform_gpio_input_get(const platform_gpio_t* gpio)
{
	hal_gpio_data_t in;
	hal_gpio_get_input(gpio->pin, &in);
	return in;
}

/**
 * Enable interrupt on the GPIO input pin specified
 *
 * @param[in] gpio    : gpio pin
 * @param[in] trigger : interrupt trigger type
 * @param[in] handler : callback function that will be called when an interrupt occurs
 * @param[in] arg     : argument that will be passed into the callback function
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_irq_enable(const platform_gpio_t* gpio,
		platform_gpio_irq_trigger_t trigger,
		platform_gpio_irq_callback_t handler, void* arg)
{
	hal_pinmux_set_function(HAL_GPIO_6, HAL_GPIO_6_EINT5);
  hal_eint_config_t eint_config;
  switch(trigger){
  case IRQ_TRIGGER_RISING_EDGE:
  	eint_config.trigger_mode = HAL_EINT_EDGE_RISING;
	break;
  case IRQ_TRIGGER_FALLING_EDGE:
  	eint_config.trigger_mode = HAL_EINT_EDGE_FALLING;
	break;
  case IRQ_TRIGGER_BOTH_EDGES:
  	eint_config.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;
	break;
  }
  eint_config.debounce_time = 5;
  hal_eint_number_t irq_num = HAL_EINT_NUMBER_5;
  hal_eint_init(irq_num, &eint_config);
  hal_eint_register_callback(irq_num, handler, arg);
	return kNoErr;
}

/**
 * Disable interrupt on the GPIO input pin specified
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_irq_disable(const platform_gpio_t* gpio)
{
	return kNoErr;
}
