/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef __DIAGNOSTICS_H__
#define __DIAGNOSTICS_H__

#include <wmstats.h>
#include <wmerrno.h>
#include <psm-v2.h>
#include <json_parser.h>
#include <json_generator.h>

int diagnostics_read_stats(struct json_str *jptr, psm_hnd_t psm_hnd);
int diagnostics_write_stats(psm_hnd_t psm_hnd);
int diagnostics_read_stats_psm(struct json_str *jptr, psm_hnd_t psm_hnd);
void diagnostics_set_reboot_reason(wm_reboot_reason_t reason);
typedef void (*diagnostics_write_cb)(psm_hnd_t psm_hnd,
	wm_reboot_reason_t reason);

/** Initialize the diagnostics module
 *
 * \return WM_SUCCESS on success
 * \return error code otherwise
 *
 */
int diagnostics_init();
#endif
