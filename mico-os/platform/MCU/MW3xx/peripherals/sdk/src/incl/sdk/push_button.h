/*! \file push_button.h
 * \brief Push button helper functions
 */

/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __PUSH_BUTTON_H__
#define __PUSH_BUTTON_H__

#include <generic_io.h>

/** Maximum callbacks that can be registered for a given push button */
#define	MAX_PB_CB		4

/** Default key debounce time in milliseconds */
#define PB_DEF_DEBOUNCE_TIME	50

/** Set a callback function for push button operations
 *
 * This API can be used to register different callbacks for the same push
 * button based on the time for which the button is pressed. Consider following
 * use cases to understand the API.
 *
 * -# Single short press like a simple on/off toggle switch.
 *  \code
 *  void button1_cb(int pin, void *data)
 *  {
 *	<short press action>
 *  }
 *  {
 *	input_gpio_cfg_t button_1;
 *	button_1.gpio = GPIO_25;
 *	button_1.type = GPIO_ACTIVE_LOW;
 *	push_button_set_cb(button_1, button1_cb, 0, 0, NULL);
 *  }
 *  \endcode
 *  Note that the time specified is 0. This is the most common use case.
 * -# Press and hold for a specified time and release
 *  \code
 *  void button1_5sec_cb(int pin, void *data)
 *  {
 *	<5 sec press and release action>
 *  }
 *  {
 *	push_button_set_cb(button_1, button1_5sec_cb, 5000, 0, NULL);
 *  }
 *  \endcode
 *  The callback will be invoked if the button is pressed for more than 5
 *  seconds and then released.
 * -# Press and hold for a specified time and then invoke a callback
 *  repeatedly.
 *  \code
 *  void button1_10sec_repeat_cb(int pin, void *data)
 *  {
 *	<repeat action to invoke>
 *  }
 *  {
 *	push_button_set_cb(button_1, button1_10sec_repeat_cb, 10000,
 *			200, NULL);
 *  }
 *  \endcode
 *  With this, if a button is pressed and held for more than 10 seconds, after
 *  the 10 second timeout, the callback is invoked for every 200 msec until
 *  the button is released.
 *  Note that if the repeat time is going to be used, the "time" specified
 *  for that callback should be maximum for that particular button. So, if
 *  say another callback is registered with a time of 11 seconds, the repeat
 *  callback defined above will not take effect.
 *
 * A single push button can have multiple such callbacks registered, with a
 * maximum limit of \ref MAX_PB_CB.
 * Consider another example.
 * \code
 * {
 *	push_button_set_cb(button_1, button1_cb, 0, 0, NULL);
 *	push_button_set_cb(button_1, button1_5sec_cb, 5000, 0, NULL);
 *	push_button_set_cb(button_1, button1_10sec_cb, 10000, 0, NULL);
 * }
 * \endcode
 * - In this case, for a short press, button1_cb will be invoked.
 * button1_cb will be invoked even if the button is pressed for a longer
 * duration but released before 5 seconds.
 * - If the button is pressed for more than 5 seconds but released before 10
 * seconds, button1_5sec_cb will be invoked.
 * - Since 10 seconds is the maximum time configured for this button, if the
 * button is pressed for 10+ seconds, the button1_10sec_cb will be invoked
 * even if the button is not released.
 *
 * If some pointer is specified as the last argument for push_button_set_cb,
 * it will be received in the pb_cb.
 *
 * This API takes care of key debouncing (oscillations because of the hardware
 * button press) as well. Hence, the application will not get any false
 * triggers.
 *
 * If multiple callbacks are registered for the same timeout value, the
 * call registered the latest will be invoked.
 *
 * \param[in] input The push button configuration as per \ref input_gpio_cfg_t
 * \param[in] pb_cb The push button callback to be invoked for this event.
 * Refer \ref gpio_irq_cb
 * \param[in] time The time in milliseconds for which the button should be
 * pressed to invoke the callback
 * \param[in] repeat_time The time in milliseconds for invoking the callbacks
 * repeatedly as described above.
 * \param[in] data A pointer to the data that the applications wants to
 * receive in the callback.
 *
 * \return WM_SUCCESS on success
 * \return -WM_FAIL in case of an error
 */
int push_button_set_cb(input_gpio_cfg_t input, gpio_irq_cb pb_cb,
		int time, int repeat_time, void *data);

/** Reset callback function registered for a push button
 *
 * This function resets the callbacks registered using push_button_set_cb(),
 * one at a time.
 *
 * \param[in] input The push button configuration as per \ref input_gpio_cfg_t.
 * Should be the same as passed to push_button_set_cb()
 * \param[in] time The time as specified while registering the callback using
 * push_button_set_cb()
 *
 * \return WM_SUCCESS on success
 * \return -WM_FAIL on failure. This will occur if the button is already
 * pressed when this function is invoked. This is done to avoid unexpected
 * behavior.
 */
int push_button_reset_cb(input_gpio_cfg_t input, int time);

/** Set push button debounce time
 *
 * When physical push buttons are pressed or released, they do not go to
 * the desired logic level directly. Instead, there are multiple oscillations
 * as the contacts bounce. This results in multiple press/release events
 * instead of a single one. To avoid this, the push button module implements
 * a key debouncing logic. However, the debounce time (time it takes
 * for the push button to settle) may vary from button to button. By default,
 * it is set to \ref PB_DEF_DEBOUNCE_TIME. If this is not suitable for a
 * particular push button, this API can be used to configure the time.
 *
 * \pre push_button_set_cb()
 *
 * \note *Guideline for deciding the value of time*: If a single button
 * press causes multiple events, try increasing the value. If some push
 * button press events are getting missed, try reducing the value.
 *
 * \param[in] input The push button configuration as per \ref input_gpio_cfg_t.
 * Should be the same as passed to push_button_set_cb().
 * \param[in] db_time The desired debounce time in msecs.
 *
 * \return WM_SUCCESS on success
 * \return -WM_FAIL on failure
 */
int push_button_set_debounce_time(input_gpio_cfg_t input, uint16_t db_time);
#endif /* ! __PUSH_BUTTON_H__ */
