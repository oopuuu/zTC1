/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include <wmstdio.h>
#include <wm_os.h>
#include <lowlevel_drivers.h>
#include <mdev_pinmux.h>
#include <board.h>
#include <led_indicator.h>
#include <wmlog.h>

/*-----------------------Global declarations----------------------*/
struct led_private_data {
	output_gpio_cfg_t led_cfg;
	uint8_t curr_state;
	os_timer_t timer;
	int on_duty_cycle;
	int off_duty_cycle;
} led_data[LED_COUNT];

static int decide_led_array_index(output_gpio_cfg_t led_cfg)
{
	int i;
	for (i = 0; i < LED_COUNT; i++) {
		if (led_data[i].led_cfg.gpio == led_cfg.gpio) {
			return i;
		} else if (led_data[i].led_cfg.gpio == 0) {
			led_data[i].led_cfg = led_cfg;
			return i;
		}
	}
	return -WM_FAIL;
}

static void __led_on(output_gpio_cfg_t led)
{
	GPIO_PinMuxFun(led.gpio, pinmux_drv_get_gpio_func(led.gpio));
	GPIO_SetPinDir(led.gpio, GPIO_OUTPUT);
	if (led.type == GPIO_ACTIVE_LOW)
		GPIO_WritePinOutput(led.gpio, GPIO_IO_LOW);
	else
		GPIO_WritePinOutput(led.gpio, GPIO_IO_HIGH);
}
void led_on(output_gpio_cfg_t led)
{
	if (led.gpio < 0)
		return;

	int idx = decide_led_array_index(led);
	int ret;

	if (idx == -WM_FAIL)
		return;

	if (os_timer_is_running(&led_data[idx].timer)) {
		ret = os_timer_delete(&led_data[idx].timer);
		if (ret != WM_SUCCESS) {
			wmlog_e("led", "Unable to delete LED timer");
			return;
		}
	}
	led_data[idx].curr_state = LED_ON;
	__led_on(led);
}

static void __led_off(output_gpio_cfg_t led)
{
	GPIO_PinMuxFun(led.gpio, pinmux_drv_get_gpio_func(led.gpio));
	GPIO_SetPinDir(led.gpio, GPIO_OUTPUT);
	if (led.type == GPIO_ACTIVE_LOW)
		GPIO_WritePinOutput(led.gpio, GPIO_IO_HIGH);
	else
		GPIO_WritePinOutput(led.gpio, GPIO_IO_LOW);
}
void led_off(output_gpio_cfg_t led)
{
	if (led.gpio < 0)
		return;

	int idx = decide_led_array_index(led);
	int err;

	if (idx == -WM_FAIL)
		return;
	if (os_timer_is_running(&led_data[idx].timer)) {
		err = os_timer_delete(&led_data[idx].timer);
		if (err != WM_SUCCESS) {
			wmlog_e("led", "Unable to delete LED timer");
			return;
		}
	}
	led_data[idx].curr_state = LED_OFF;
	__led_off(led);
}

static void led_cb(os_timer_arg_t handle)
{
	int tid = (int) os_timer_get_context(&handle);
	if (tid >= LED_COUNT)
		return;
	if (led_data[tid].curr_state == LED_ON) {
		__led_off(led_data[tid].led_cfg);
		led_data[tid].curr_state = LED_OFF;
		os_timer_change(&led_data[tid].timer,
				led_data[tid].off_duty_cycle, -1);
		os_timer_activate(&led_data[tid].timer);
	} else {
		__led_on(led_data[tid].led_cfg);
		led_data[tid].curr_state = LED_ON;
		os_timer_change(&led_data[tid].timer,
				led_data[tid].on_duty_cycle, -1);
		os_timer_activate(&led_data[tid].timer);
	}
}

void led_blink(output_gpio_cfg_t led, int on_duty_cycle, int off_duty_cycle)
{
	if (led.gpio < 0)
		return;

	int err, idx;

	idx = decide_led_array_index(led);

	if (idx == -WM_FAIL)
		return;

	if (os_timer_is_running(&led_data[idx].timer)) {
		err = os_timer_delete(&led_data[idx].timer);
		if (err != WM_SUCCESS)
			return;
	}
	led_data[idx].on_duty_cycle = on_duty_cycle;
	led_data[idx].off_duty_cycle = off_duty_cycle;

	__led_off(led);
	led_data[idx].curr_state = LED_OFF;

	err = os_timer_create(&led_data[idx].timer,
			      "led-timer",
			      os_msec_to_ticks(led_data[idx].on_duty_cycle),
			      led_cb,
			      (void *)idx,
			      OS_TIMER_ONE_SHOT,
			      OS_TIMER_AUTO_ACTIVATE);
	if (err != WM_SUCCESS)
		return;
}

int led_get_state(output_gpio_cfg_t led)
{
	if (led.gpio < 0)
		return -WM_FAIL;

	GPIO_IO_Type led_status = GPIO_ReadPinLevel(led.gpio);
	if (led.type == GPIO_ACTIVE_LOW) {
		if (led_status == GPIO_IO_LOW)
			return LED_ON;
		else
			return LED_OFF;
	} else if (led.type == GPIO_ACTIVE_HIGH) {
		if (led_status == GPIO_IO_LOW)
			return LED_OFF;
		else
			return LED_ON;

	}
	return -WM_FAIL;
}
