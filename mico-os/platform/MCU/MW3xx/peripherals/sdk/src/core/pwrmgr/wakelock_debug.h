/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */
#ifndef _WAKELOCK_DEBUG_H_
#define _WAKELOCK_DEBUG_H_

#define MAX_LOGICAL_WAKELOCKS	16

struct wl_debug_info {
	const char *id_str;
	int count;
	unsigned underflow;
};

#ifndef CONFIG_WAKELOCK_DEBUG
inline void wakelock_put_debug(const char *id_str) { }
inline void wakelock_get_debug(const char *id_str) { }

/*
 * Enable wakelock CLI
 * This function registers the CLI for the wakelock
 * wakelock-dbg command is used to get the additional
 * debug information about it
 */

#define wakelock_cli_init(...)
#else
extern void wakelock_put_debug(const char *id_str);
extern void wakelock_get_debug(const char *id_str);
extern int wakelock_cli_init(void);
#endif

#endif
