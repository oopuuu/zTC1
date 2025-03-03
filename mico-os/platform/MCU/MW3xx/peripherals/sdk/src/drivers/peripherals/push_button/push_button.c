/* The push_button_set_cb API provided here enables applications to
 * handle push button events without having to worry about any debouncing
 * or other timing stuff like detection of short press, long press, etc.
 *
 * The way it is done is as follows:
 *
 * When the application calls push_button_set_cb, we first check if some
 * callback was already registered for the given button. If not found, we
 * allocate memory for that particular push button and fill up the
 * associated data structures appropriately. An interrupt callback is
 * registered depending on whether the button is active high or low.
 *
 * If an entry is found, the new callback is placed in an array as per
 * the timing value associated with it, such that the array is sorted in
 * descending order of timing.
 *
 * When the button is pressed, we get a callback.
 * We save the current time of the system in the push button's private
 * data structure and then disable the interrupt so that we do not get any
 * more false triggers due to key debouncing.
 * We also start a debounce timer as per the debounce time programmed for
 * the button, the default being 100msec.
 * After the debounce timer times out, we check if the button is still pressed.
 * If not, we consider that the button is released and then invoke the
 * appropriate callback registered by the application based on the total time
 * for which the button was pressed.
 * If it is found that the button is still pressed, we register interrupt
 * callback for the button "release" event.
 * When the button is released, the callback is invoked.
 * We check the total time for which the button was pressed and then invoke
 * the appropriate callback registered by the application.
 *
 * After the button release event, we wait for debounce time and only then
 * register back the callback handlers so that there are no false triggers
 * because of debouncing.
 *
 * Another additional timer is also used for advanced purposes.
 * Normally, the callbacks would be invoked only after a button release event.
 * However, the callbacks will be invoked even if the button is pressed and
 * held for the maximum timeout configured for that button.
 * If a repeat_time has been configured for the maximum timeout, the timer
 * gives periodic interrupts and the application's callback is invoked
 * repeatedly, till the button is released.
 */
#include <wmlist.h>
#include <wm_os.h>
#include <wmlog.h>
#include <generic_io.h>
#include <mdev_pinmux.h>
#include <mdev_gpio.h>
#include <push_button.h>
#include <pwrmgr.h>
#include <system-work-queue.h>

#define	PB_WAKELOCK		"pb_wakelock"
#ifdef CONFIG_CPU_MW300
#define WAKEUP0_PIN		GPIO_22
#define WAKEUP1_PIN		GPIO_23
#else
#define WAKEUP0_PIN		GPIO_25
#endif

static list_head_t pb_io_lh;

typedef struct push_button_timer_cfg {
	/* Time in msecs for which the push button should be pressed for pb_cb
	 * to be invoked
	 */
	int time;
	/* Time period for which pb_cb should be repeated (if required).
	 * Refer refman for details.
	 */
	int repeat_time;
	/* The push button callback */
	gpio_irq_cb pb_cb;
	/* Data to be sent to the callback */
	void *data;
} push_button_timer_cfg_t;

typedef struct push_button_cfg {
	/* A flag to denote if a button press even was handled */
	bool press_handled;
	/* A flag to denote if the push button handling is currently
	 * active. This is to safeguard against false triggers due to
	 * debouncing.
	 */
	bool active;
	/* The GPIO configuration (gpio number and active high/low)
	 */
	input_gpio_cfg_t input;
	/* Total number of callbacks registered */
	uint8_t num_cb;
	/* Current index in timer_cfg below that has been queued for
	 * handling. Useful when executing the callback from the
	 * work queue callback.
	 */
	uint8_t cur_cb;
	/* Timing information for the push button press */
	uint32_t timing_info;
	/* Debounce time configured for the button.
	 * Currently, it is hard-coded to PB_DEF_DEBOUNCE_TIME
	 */
	uint16_t debounce_time;
	/* Timer for handling different timing related events for
	 * the push button.
	 */
	os_timer_t timer;
	/* Advanced timer required for advanced features like long press
	 * and repeat events.
	 */
	os_timer_t adv_timer;
	/* Array of configured callbacks */
	push_button_timer_cfg_t timer_cfg[MAX_PB_CB];
	/* Identifier in the link list of push button configs */
	list_head_t lh;
} push_button_cfg_t;

/* Generic work queue callback for the push button module.
 * This intermediate callback is required because work queue jobs
 * have a single paramater, whereas push button callbacks require
 * 2 paramaters. So, we cannot directly register a push button
 * callback with the work queue.
 */
static int wq_pb_cb(void *param)
{
	push_button_cfg_t *cfg = (push_button_cfg_t *)param;
	int i = cfg->cur_cb;
	cfg->timer_cfg[i].pb_cb(cfg->input.gpio, cfg->timer_cfg[i].data);

	return WM_SUCCESS;
}

/* Enqueue the push button callback so that it executes in a thread
 * context and not an interrupt context.
 */
static void pb_enqueue(push_button_cfg_t *cfg, int index)
{
	cfg->cur_cb = index;
	wq_job_t job = {
		.job_func = wq_pb_cb,
		.owner[0] = 0,
		.param = cfg,
		.periodic_ms = 0,
		.initial_delay_ms = 0,
	};

	wq_handle_t wq_handle;

	wq_handle = sys_work_queue_get_handle();
	if (wq_handle)
		work_enqueue(wq_handle, &job, NULL);
}

void _push_button_press_cb(int pin, void *data);

/* Depending on the type of the push button (active high/low),
 * this function tells if the button is pressed or not.
 * return true of pressed and false otherwise.
 */
static bool pb_pressed(input_gpio_cfg_t input)
{
	if (GPIO_ReadPinLevel(input.gpio) == input.type)
		return true;
	else
		return false;
}
static void release_debounce_cb(os_timer_arg_t handle)
{
	push_button_cfg_t *cfg =
		(push_button_cfg_t *)os_timer_get_context(&handle);
	if (cfg) {
		os_timer_delete(&cfg->timer);
		/* Setting this to false means that the entire button handling
		 * operation has finished.
		 */
		cfg->active = false;
		/* Set the GPIO callbacks to the original values */
		if (cfg->input.type)
			gpio_drv_set_cb(NULL, cfg->input.gpio,
					GPIO_INT_RISING_EDGE, cfg,
					_push_button_press_cb);
		else
			gpio_drv_set_cb(NULL, cfg->input.gpio,
					GPIO_INT_FALLING_EDGE, cfg,
					_push_button_press_cb);
		wakelock_put(PB_WAKELOCK);
	}
}

/* Handle a push button press */
static void handle_pb_press(push_button_cfg_t *cfg, bool released)
{

	/* Handle the press only if it is not already handled.
	 * This check is required for cases in which the button has been
	 * pressed for a long time and the callback for the highest timeout has
	 * already been called. This also means that this function has been
	 * called as a result of a button release event and we must just
	 * restore the button interrupts to original values after a debounce
	 * timer.
	 */
	if (!cfg->press_handled) {
		/* Calculate the time between push button press and release */
		uint32_t total_time = os_get_timestamp() - cfg->timing_info;
		int i;
		total_time /= 1000;
		/* Depending on the time for which the push button was pressed,
		 * search for the appropriate callback in the list and invoke it
		 * Note that the callbacks are stored in the array in the
		 * descending order of timing. Hence we just check if the total
		 * time is greater than the programed time starting from
		 * index 0.
		 * If say 2 callbacks are registered, one for 5sec and second
		 * for 10sec.
		 * For total_time >= 10sec, the second callback will be invoked
		 * For total_time >= 5sec but <=10sec, the first callback will
		 * be invoked. For total_time < 5sec, no callback will be
		 * invoked.
		 */
		for (i = 0; i < cfg->num_cb; i++) {
			if (total_time >= cfg->timer_cfg[i].time) {
				if (cfg->timer_cfg[i].pb_cb) {
					pb_enqueue(cfg, i);
					break;
				}
			}
		}
		cfg->press_handled = true;
	}

	/* If the button has been released, reset the button to its
	 * normal state after a debounce time.
	 */
	if (released) {
		/* Reset the timing information */
		cfg->timing_info = 0;
		if (cfg->adv_timer) {
			if (os_timer_is_running(&cfg->adv_timer))
				os_timer_deactivate(&cfg->adv_timer);
			os_timer_delete(&cfg->adv_timer);
		}
		/* Start the button release debounce timer and register the
		 * button press callbacks after the timeout so that there
		 * are no false triggers because of the debouncing.
		 */
		if (cfg->timer) {
			if (os_timer_is_running(&cfg->timer))
				os_timer_deactivate(&cfg->timer);
			os_timer_delete(&cfg->timer);
		}
		os_timer_create(&cfg->timer,
				"pb_release_timer",
				os_msec_to_ticks(cfg->debounce_time),
				release_debounce_cb,
				(void *)cfg,
				OS_TIMER_ONE_SHOT,
				OS_TIMER_AUTO_ACTIVATE);
	}
}

/* This function has importance only in cases where the button configuration
 * also has a repeat_time set. In that case, we just invoke the application
 * callback and reset the timer so that it keeps repeating.
 */
static void repeat_press_cb(os_timer_arg_t handle)
{
	push_button_cfg_t *cfg =
		(push_button_cfg_t *)os_timer_get_context(&handle);
	if (cfg) {
		/* The timer_cfg index 0 is hard coded here because this
		 * implementation assumes that if a repeat timer has been
		 * set, it has been set for the maximum value of timeout.
		 * Since the timer_cfg list is sorted, the max timeout
		 * is at index 0
		 */
		pb_enqueue(cfg, 0);
		os_timer_reset(&cfg->adv_timer);
	}
}

/* Normally, the application callbacks are invoked after the button is
 * released. However, this function is called if the button is pressed and held
 * for a time >= the max timeout configured for the button. This has been done
 * for convenience since it can be hard for a user to know how long the button
 * has been pressed and when to release it.
 */
static void max_press_cb(os_timer_arg_t handle)
{
	push_button_cfg_t *cfg =
		(push_button_cfg_t *)os_timer_get_context(&handle);
	if (cfg) {
		/* Invoke the callback for the max timeout */
		cfg->press_handled = true;
		if (cfg->timer_cfg[0].pb_cb) {
			pb_enqueue(cfg, 0);
		}

		/* This is set to true as we do not want to invoke the
		 * application callback once again after the button
		 * release event.
		 */
		cfg->press_handled = true;
		/* If the max timeout value has a repeat_time configured, then
		 * start another timer for repeating the callback.
		 */
		if (cfg->timer_cfg[0].repeat_time) {
			os_timer_delete(&cfg->adv_timer);
			os_timer_create(&cfg->adv_timer,
					"pb_adv_timer",
					os_msec_to_ticks
					(cfg->timer_cfg[0]
					 .repeat_time),
					repeat_press_cb,
					(void *)cfg,
					OS_TIMER_ONE_SHOT,
					OS_TIMER_AUTO_ACTIVATE);
		}
	}
}
/* Callback for button release event.
 * This will be called in a worq queue's context
 */
static int push_button_release_cb(void *data)
{
	push_button_cfg_t *cfg = (push_button_cfg_t *)data;
	if (cfg) {
		/* Disable the interrupt to avoid false triggers */
		gpio_drv_set_cb(NULL, cfg->input.gpio,
				GPIO_INT_DISABLE, NULL,
				NULL);
		handle_pb_press(cfg, true);
	}

	return WM_SUCCESS;
}

/* This is the actual GPIO interrupt callback being registered
 * Since we cannot do much stuff here, we use the work queue and
 * perform the actual actions there.
 */
static void _push_button_release_cb(int pin, void *data)
{
	wq_job_t job = {
		.job_func = push_button_release_cb,
		.owner[0] = 0,
		.param = data,
		.periodic_ms = 0,
		.initial_delay_ms = 0,
	};

	wq_handle_t wq_handle;

	wq_handle = sys_work_queue_get_handle();
	if (wq_handle)
		work_enqueue(wq_handle, &job, NULL);
}
static void press_debounce_cb(os_timer_arg_t handle)
{

	push_button_cfg_t *cfg =
		(push_button_cfg_t *)os_timer_get_context(&handle);
	if (cfg) {
		/* If it is found that after the debounce time, the push button
		 * is no more pressed, we consider it as a noise.
		 * The handle_pb_press() function will then take care of
		 * re-configuring the timers and interrupts for the push button
		 * so that it comes back to the default state.
		 * We set cfg->press_handled to 'true' before calling
		 * handle_pb_press() so that no application registered
		 * callbacks are invoked
		 *
		 * TODO: Handle this cleanly instead of setting
		 * cfg->press_handled and then calling handle_pb_press()
		 */
		if (!pb_pressed(cfg->input)) {
			cfg->press_handled = true;
			handle_pb_press(cfg, true);
		} else {
			/* Else we reverse the interrupt type so that we get an
			 * interrupt on button release.
			 */
			os_timer_delete(&cfg->timer);
			if (cfg->input.type) {
				gpio_drv_set_cb(NULL, cfg->input.gpio,
						GPIO_INT_FALLING_EDGE, cfg,
						_push_button_release_cb);
			} else {
				gpio_drv_set_cb(NULL, cfg->input.gpio,
						GPIO_INT_RISING_EDGE, cfg,
						_push_button_release_cb);
			}
		}
	}
}
/* Callback for button press event.
 * This will be called in a worq queue's context
 */
static int push_button_press_cb(void *data)
{
	push_button_cfg_t *cfg = (push_button_cfg_t *)data;
	if (cfg) {
		/* If the cfg-> active value is true, it means that we got a
		 * false trigger for button press as the previous press event
		 * is still active.
		 */
		if (cfg->active)
			return WM_SUCCESS;

		wakelock_get(PB_WAKELOCK);
		cfg->active = true;
		/* Disable the interrupt callback to avoid false triggers */
		gpio_drv_set_cb(NULL, cfg->input.gpio,
				GPIO_INT_DISABLE, NULL,
				NULL);
		/* Store the current time so that after the button release
		 * event, we can know the time for which the button was
		 * pressed.
		 */
		cfg->timing_info = os_get_timestamp();
		cfg->press_handled = false;
		if (cfg->adv_timer) {
			if (os_timer_is_running(&cfg->adv_timer))
				os_timer_deactivate(&cfg->adv_timer);
			os_timer_delete(&cfg->adv_timer);
		}
		if (cfg->timer) {
			if (os_timer_is_running(&cfg->timer))
				os_timer_deactivate(&cfg->timer);
			os_timer_delete(&cfg->timer);
		}
		/* Start the key debounce timer */
		os_timer_create(&cfg->timer,
				"pb_press_timer",
				os_msec_to_ticks(cfg->debounce_time),
				press_debounce_cb,
				(void *)cfg,
				OS_TIMER_ONE_SHOT,
				OS_TIMER_AUTO_ACTIVATE);
		/* It is assumed that if the max timeout is less than
		 * 1 second, the user will not press and hold the key
		 * for much longer time.
		 * The purpose of this timer is for a case like say
		 * the max timeout is 10sec. It is hard for the user
		 * to know if the button has indeed been pressed for
		 * 10sec. So, instead of waiting for the release event,
		 * we will just invoke the callback after this
		 * timeout.
		 * Another use case for this is the repeat_time usage.
		 * Consider a push button for a dimmer, for which it
		 * is required that if the button is pressed for more
		 * than 5sec, a callback should be invoked for every
		 * 500msec thereafter. This timer can be used even in
		 * that case.
		 */
		if ((cfg->timer_cfg[0].time >= 1000)) {
			os_timer_create(&cfg->adv_timer,
					"pb_adv_timer",
					os_msec_to_ticks
					(cfg->timer_cfg[0]
					 .time),
					max_press_cb,
					(void *)cfg,
					OS_TIMER_ONE_SHOT,
					OS_TIMER_AUTO_ACTIVATE);
		}
	}

	return WM_SUCCESS;
}

/* This is the actual GPIO interrupt callback being registered
 * Since we cannot do much stuff here, we use the work queue and
 * perform the actual actions there.
 */
void _push_button_press_cb(int pin, void *data)
{
	wq_job_t job = {
		.job_func = push_button_press_cb,
		.owner[0] = 0,
		.param = data,
		.periodic_ms = 0,
		.initial_delay_ms = 0,
	};

	wq_handle_t wq_handle;

	wq_handle = sys_work_queue_get_handle();
	if (wq_handle)
		work_enqueue(wq_handle, &job, NULL);
}
static int pb_init()
{
	static bool pb_init_done;
	if (!pb_init_done) {
		if (sys_work_queue_init() != WM_SUCCESS)
			return -WM_FAIL;
		INIT_LIST_HEAD(&pb_io_lh);
		pb_init_done = true;
	}
	return WM_SUCCESS;
}

static push_button_cfg_t *get_push_button_cfg(input_gpio_cfg_t input)
{
	push_button_cfg_t *cfg;
	list_for_each_entry(cfg, &pb_io_lh, lh) {
		if (cfg->input.gpio == input.gpio)
			return cfg;
	}
	return NULL;
}

static void pb_extpin0_cb(void)
{
	input_gpio_cfg_t input;
	input.gpio = WAKEUP0_PIN;
	push_button_cfg_t *cfg = get_push_button_cfg(input);
	if (cfg)
		_push_button_press_cb(WAKEUP0_PIN, cfg);
}

#ifdef CONFIG_CPU_MW300
static void pb_extpin1_cb(void)
{
	input_gpio_cfg_t input;
	input.gpio = WAKEUP1_PIN;
	push_button_cfg_t *cfg = get_push_button_cfg(input);
	if (cfg)
		_push_button_press_cb(WAKEUP1_PIN, cfg);
}
#endif

int push_button_set_cb(input_gpio_cfg_t input, gpio_irq_cb pb_cb,
		int time, int repeat_time, void *data)
{
	if (pb_init() != WM_SUCCESS)
		return -WM_FAIL;
	if (input.gpio < 0)
		return -WM_FAIL;
	pinmux_drv_init();
	gpio_drv_init();
	/* First we check if some callback is already registered for the
	 * given push button.
	 */
	push_button_cfg_t *cfg = get_push_button_cfg(input);
	/* If a previous entry is found, first do some sanity
	 * checks.
	 */
	if (cfg) {
		if (cfg->num_cb >= MAX_PB_CB) {
			wmlog_e("push_button",
					"Cannot add any more callbacks for "
					"the given GPIO");
			return -WM_FAIL;
		}
		if (cfg->input.type != input.type) {
			wmlog_e("push_button", "Type mismatch in push button "
					"(Active High/Low)");
			return -WM_FAIL;
		}
	} else {
		/* If an entry is not already found, add it to the list */
		cfg = os_mem_calloc(sizeof(push_button_cfg_t));
		if (!cfg)
			return -WM_FAIL;
		cfg->input = input;
		cfg->debounce_time = PB_DEF_DEBOUNCE_TIME;
		/* Set appropriate interrupt callbacks */
		int ret;
		if (input.type == GPIO_ACTIVE_HIGH) {
			ret = gpio_drv_set_cb(NULL, input.gpio,
					GPIO_INT_RISING_EDGE, cfg,
					_push_button_press_cb);
		} else {
			ret = gpio_drv_set_cb(NULL, input.gpio,
					GPIO_INT_FALLING_EDGE, cfg,
					_push_button_press_cb);
		}
		if (ret != WM_SUCCESS) {
			os_mem_free(cfg);
			return -WM_FAIL;
		}
		if (input.gpio == WAKEUP0_PIN) {
			install_int_callback(INT_EXTPIN0,
					0, pb_extpin0_cb);
		}
#ifdef CONFIG_CPU_MW300
		else if (input.gpio == WAKEUP1_PIN) {
			install_int_callback(INT_EXTPIN1,
					0, pb_extpin1_cb);
		}
#endif
		INIT_LIST_HEAD(&cfg->lh);
		list_add_tail(&cfg->lh, &pb_io_lh);
	}
	/* Configure the pin as a GPIO and set it as an input.
	 * This is being done every time just to safeguard against cases
	 * wherein someone has changed the GPIO configuration between two
	 * push_button_set_cb() calls for the same pin.
	 */
	mdev_t *pinmux_dev, *gpio_dev;

	pinmux_dev = pinmux_drv_open("MDEV_PINMUX");
	gpio_dev = gpio_drv_open("MDEV_GPIO");
	pinmux_drv_setfunc(pinmux_dev, input.gpio,
			pinmux_drv_get_gpio_func(input.gpio));
	gpio_drv_setdir(gpio_dev, input.gpio, GPIO_INPUT);
	pinmux_drv_close(pinmux_dev);
	gpio_drv_close(gpio_dev);

	/* Add this new callback entry such that all entries
	 * are in descending order of timing.
	 */
	int i, j;
	for (i = 0; i < cfg->num_cb; i++) {
		if (time >= cfg->timer_cfg[i].time)
			break;
	}
	for (j = cfg->num_cb; j > i; j--)
		cfg->timer_cfg[j] = cfg->timer_cfg[j-1];
	cfg->timer_cfg[i].time = time;
	cfg->timer_cfg[i].repeat_time = repeat_time;
	cfg->timer_cfg[i].pb_cb = pb_cb;
	cfg->timer_cfg[i].data = data;
	cfg->num_cb++;
	return WM_SUCCESS;
}

int push_button_reset_cb(input_gpio_cfg_t input, int time)
{
	pb_init();
	if (input.gpio < 0)
		return -WM_FAIL;

	push_button_cfg_t *cfg = get_push_button_cfg(input);
	if (!cfg) {
		/* Return success and not error because after invoking
		 * this API, it is expected that the callback for this
		 * push button should be de-registered. Even if it is
		 * found that there was no callback registered anyways,
		 * the end result is the same.
		 */
		return WM_SUCCESS;
	}
	/* If the button is active i.e. pressed, return failure to avoid
	 * unexpected behavior.
	 */
	if (cfg->active)
		return -WM_FAIL;
	int i;
	for (i = 0; i < cfg->num_cb; i++) {
		if (cfg->timer_cfg[i].time == time) {
			int j;
			/* Re-arrange the entries in ascending order of time
			*/
			for (j = i; j < cfg->num_cb; j++)
				cfg->timer_cfg[j] = cfg->timer_cfg[j+1];
			cfg->num_cb--;
			break;
		}
	}
	/* If there are no more callbacks registered for that particular
	 * button, delete it from the list */
	if (!cfg->num_cb) {
		/* The timers will anyways be in-active.
		 * Still, better to check and take appropriate action.
		 */
		if (cfg->adv_timer) {
			if (os_timer_is_running(&cfg->adv_timer))
				os_timer_deactivate(&cfg->adv_timer);
			os_timer_delete(&cfg->adv_timer);
		}
		if (cfg->timer) {
			if (os_timer_is_running(&cfg->timer))
				os_timer_deactivate(&cfg->timer);
			os_timer_delete(&cfg->timer);
		}
		/* Interrupt handler no more required */
		gpio_drv_set_cb(NULL, cfg->input.gpio,
				GPIO_INT_DISABLE, NULL,
				NULL);
		list_del(&cfg->lh);
		os_mem_free(cfg);
	}
	return WM_SUCCESS;
}

int push_button_set_debounce_time(input_gpio_cfg_t input, uint16_t db_time)
{
	push_button_cfg_t *cfg = get_push_button_cfg(input);
	if (!cfg) {
		wmlog_e("push_button",
				"No entry for push button found");
		return -WM_FAIL;
	}
	cfg->debounce_time = db_time;
	return WM_SUCCESS;
}
