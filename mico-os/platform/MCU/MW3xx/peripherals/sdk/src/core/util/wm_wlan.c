/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include <wlan.h>
#include <wmstdio.h>
#include <cli.h>
#include <healthmon.h>
#include <wmtime.h>
#include <boot_flags.h>
#include <partition.h>

#define init_e(...)	\
	wmlog_e("init", ##__VA_ARGS__)

static char wlan_init_done;
static char core_init_done;

int wm_wlan_init()
{
	struct partition_entry *p;
	short history = 0;
	struct partition_entry *f1, *f2;

	if (wlan_init_done)
		return WM_SUCCESS;

	int ret = part_init();
	if (ret != WM_SUCCESS) {
		init_e("wm_wlan_init: could not read partition table");
		return ret;
	}
	f1 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);
	f2 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);

	if (f1 && f2)
		p = part_get_active_partition(f1, f2);
	else if (!f1 && f2)
		p = f2;
	else if (!f2 && f1)
		p = f1;
	else
		return -WLAN_ERROR_FW_NOT_DETECTED;

	flash_desc_t fl;
	part_to_flash_desc(p, &fl);

	/* Initialize wlan */
	ret = wlan_init(&fl);
	if (ret != WM_SUCCESS)
		return ret;

	wlan_init_done = 1;
	return WM_SUCCESS;
}

int wm_core_init(void)
{
	int ret = 0;

	if (core_init_done)
		return WM_SUCCESS;

	wmstdio_init(UART0_ID, 0);

	ret = cli_init();
	if (ret != WM_SUCCESS) {
		init_e("Cli init failed.");
		goto out;
	}

	ret = wmtime_init();
	if (ret != WM_SUCCESS) {
		init_e("Wmtime init failed.");
		goto out;
	}

	ret = pm_init();
	if (ret != WM_SUCCESS) {
		init_e("Power manager init failed.");
		goto out;
	}
	ret = healthmon_init();
	if (ret != WM_SUCCESS) {
		init_e("Healthmon init failed.");
		goto out;
	}

	/* Read partition table layout from flash */
	part_init();
out:
	if (ret == WM_SUCCESS)
		core_init_done = 1;
	return ret;
}

int wm_core_and_wlan_init()
{
	int ret = wm_core_init();
	if (ret != WM_SUCCESS) {
		init_e("Error: Core init failed");
		return ret;
	}

	ret = wm_wlan_init();
	if (ret != WM_SUCCESS)
		init_e("Error: Unable to initialize wlan");
	return ret;
}

void wm_wlan_deinit()
{
	/* De-Initialize wlan */
	wlan_deinit();

	wlan_init_done = 0;
}
