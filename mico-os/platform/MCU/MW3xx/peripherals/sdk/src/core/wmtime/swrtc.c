/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/** swrtc.c: Software rtc implementation
 */

#include <wm_os.h>
#include <wmtime.h>
#include <rtc.h>

static unsigned long n_ticks_per_sec;

/**
 * Global variable to avoid multiple initialization of rtc
 */
static int swrtc_init_flag = 0;

time_t cur_posix_time = 0;
unsigned int abs_tick_count = 0;
static unsigned int tick_count_ = 0;

/**
 * Update RTC counters on a system clock tick
 */
void swrtc_tick(void)
{
	abs_tick_count++;

	/* No protection is required as tick_count_ is not used anywhere else */
	if (++tick_count_ >= n_ticks_per_sec) {
		tick_count_ = 0;
		cur_posix_time++;
	}
}

/**
 * Initialize Software Real Time Clock
 */
void swrtc_init(void)
{
	if (swrtc_init_flag)
		return;
	swrtc_init_flag = 1;

	/* Set initial date to 1/1/1970 00:00 */
	cur_posix_time = 0;
	abs_tick_count = 0;

	n_ticks_per_sec = os_msec_to_ticks(1000);
	os_setup_tick_function(swrtc_tick);
}

int swrtc_time_set(time_t time)
{
    cur_posix_time = time;
    return 0;
}

time_t swrtc_time_get(void)
{
    return cur_posix_time;
}
