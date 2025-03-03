/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include <wm_os.h>
#include <wmtime.h>
#include <rtc.h>
#include <mdev_rtc.h>

/**
 * Global variable to avoid multiple initialization of rtc
 */
static int hwrtc_init_flag;
static uint64_t rtc_ticks __attribute__((section(".nvram_uninit")));
static uint32_t rtc_sig __attribute__((section(".nvram_uninit")));
static mdev_t *rtc_dev;
/*
 * It is observed that it takes ~5ms for update, reset, start
 * cycle of the RTC. So we need to add the lost time everytime.
 * NOTE: This is an approximate compensation, so some inaccuracy
 * will persist in the time kept by the RTC.
 */
uint8_t tcalib = 5;

void hwrtc_time_update(void)
{
	rtc_ticks += (uint64_t)rtc_drv_get(rtc_dev) + (uint64_t)tcalib;
}

static void hwrtc_cb(void)
{
	rtc_ticks += (uint64_t)rtc_drv_get_uppval(rtc_dev) + (uint64_t)1;
	rtc_drv_set(rtc_dev, 0xffffffff);
	rtc_drv_reset(rtc_dev);
}

void hwrtc_init(void)
{
	if (hwrtc_init_flag)
		return;
	hwrtc_init_flag = 1;

	/* Initialize if nvram is empty */
	if (rtc_sig != 0xdeadbeef) {
		rtc_ticks = 0;
		rtc_sig = 0xdeadbeef;
	}

	rtc_drv_set_cb(hwrtc_cb);
	rtc_drv_init();
	rtc_dev = rtc_drv_open("MDEV_RTC");
	/* Update the time before reseting RTC */
	hwrtc_time_update();
	rtc_drv_reset(rtc_dev);
	rtc_drv_start(rtc_dev);
}

int hwrtc_time_set(time_t time)
{
	rtc_drv_reset(rtc_dev);
	/* RTC runs at 1024 Hz */
	rtc_ticks = (uint64_t)time << 10;
	return 0;
}

time_t hwrtc_time_get(void)
{
	/* Convert to seconds before returning */
	return (rtc_ticks + (uint64_t)rtc_drv_get(rtc_dev)) >> 10;
}
