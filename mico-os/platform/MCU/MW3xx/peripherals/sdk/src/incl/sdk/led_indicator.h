/*! \file led_indicator.h
 * \brief LED indicator helper functions
 */

/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __LED_INDICATOR_H__
#define __LED_INDICATOR_H__

#include <generic_io.h>
#define LED_COUNT 8

/** State of the LED */
enum led_state {
	/** LED is Off */
	LED_OFF = 0,
	/** LED is On */
	LED_ON,
};

/** Switch ON LED
 *
 * \param[in] led The output LED GPIO configuration of type
 * \ref output_gpio_cfg_t
 */
void led_on(output_gpio_cfg_t led);

/** Switch OFF LED
 *
 * \param[in] led The output LED GPIO configuration of type
 * \ref output_gpio_cfg_t
 */
void led_off(output_gpio_cfg_t led);

/** Blink the LED with given duty cycle
 *
 * \param[in] led The output LED GPIO configuration of type
 * \ref output_gpio_cfg_t
 * \param[in] on_duty_cycle Time in millisec for which LED will be
 * ON in blinking cycle
 * \param[in] off_duty_cycle Time in millisec for which LED will be
 * OFF in blinking cycle
 */
void led_blink(output_gpio_cfg_t led, int on_duty_cycle, int off_duty_cycle);

/** Get the current LED state
 *
 * \param[in] led The output LED GPIO configuration of type
 * \ref output_gpio_cfg_t
 *
 * \return \ref LED_ON if the LED is On
 * \return \ref LED_OFF if the LED is Off
 * \return -WM_FAIL if invalid parameter specified
 */
int led_get_state(output_gpio_cfg_t led);

/** Fast Blink the LED
 *
 *  Blink LED with on_duty_cycle = 200ms and off_duty_cycle = 200ms
 *
 *  \param[in] led The output LED GPIO configuration of type
 *  \ref output_gpio_cfg_t
 */
static inline void led_fast_blink(output_gpio_cfg_t led)
{
	led_blink(led, 200, 200);
}

/** Slow Blink the LED
 *
 *  Blink LED with on_duty_cycle = 1000ms and off_duty_cycle = 1000ms
 *
 *  \param[in] led The output LED GPIO configuration of type
 *  \ref output_gpio_cfg_t
 */
static inline void led_slow_blink(output_gpio_cfg_t led)
{
	led_blink(led, 1000, 1000);
}
#endif /* ! __LED_INDICATOR_H__ */
