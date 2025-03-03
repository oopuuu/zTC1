/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

/* Wakelock debug functionality */

#include <wmstdio.h>
#include <string.h>
#include <wmlog.h>
#include <cli.h>
#include <pwrmgr.h>
#include "wakelock_debug.h"

static struct wl_debug_info wl_dbg[MAX_LOGICAL_WAKELOCKS];


void wakelock_put_debug(const char *id_str)
{
	int i;

	/* Check if wakelock_get is for currently tracked wakelock */
	for (i = 0; i < MAX_LOGICAL_WAKELOCKS; i++) {
		if (!wl_dbg[i].id_str)
			continue;

		if (strcmp(id_str, wl_dbg[i].id_str))
			continue;

		if (wl_dbg[i].count <= 0) {
			wl_dbg[i].underflow++;
			wmlog_e("Wakelock", "Mismatching wakelock put");
		} else
			wl_dbg[i].count--;

		break;
	}

	if (i == MAX_LOGICAL_WAKELOCKS) {
		wmlog_e("Wakelock", "Mismatching wakelock put");
	}
}

void wakelock_get_debug(const char *id_str)
{
	int i;

	/* Check if wakelock_get is for currently tracked wakelock */
	for (i = 0; i < MAX_LOGICAL_WAKELOCKS; i++) {
		if (!wl_dbg[i].id_str)
			continue;

		if (strcmp(id_str, wl_dbg[i].id_str))
			continue;

		if (wl_dbg[i].count == 0xefffffff) {
			wmlog_e("Wakelock", "Possible wakelock overflow");
		}

		wl_dbg[i].count++;
		break;
	}

	/* New wakelock. Find a place for it */
	if (i == MAX_LOGICAL_WAKELOCKS) {
		for (i = 0; i < MAX_LOGICAL_WAKELOCKS; i++) {
			if (wl_dbg[i].id_str)
				continue;
			wl_dbg[i].id_str = id_str;
			wl_dbg[i].count++;
			break;
		}
	}

	if (i == MAX_LOGICAL_WAKELOCKS) {
		wmlog_e("Wakelock",
			"Maximum count for logical wakelocks reached.");
	}
}

static void dump_wakelocks(int argc, char *argv[])
{
	int i;

	wmprintf("System wakelock count: %d\r\n\n", wakelock_isheld());

	wmprintf("Wakelock\tCount\tExtraPut\r\n");
	for (i = 0; i < MAX_LOGICAL_WAKELOCKS; i++) {
		if (!wl_dbg[i].id_str)
			continue;
		wmprintf("%s\t%d\t%d\r\n", wl_dbg[i].id_str,
			 wl_dbg[i].count, wl_dbg[i].underflow);
	}

	return;
}

static struct cli_command wldbg_cmd = { "wakelock-dbg", NULL, dump_wakelocks };

int wakelock_cli_init(void)
{
	return cli_register_command(&wldbg_cmd);
}
