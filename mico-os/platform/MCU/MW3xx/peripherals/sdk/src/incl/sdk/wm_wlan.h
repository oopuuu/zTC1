/*! \file wm_wlan.h
 *  \brief Architecture specific system include file
 *
 * Copyright (C) 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef _WM_WLAN_H_
#define _WM_WLAN_H_

#include <stdint.h>


/** Initialize WLAN
 *
 * This function is responsible for initializing only wireless.
 *
 * \return Returns 0 on success, error otherwise.
 * */

int wm_wlan_init();

/** De Initialize WLAN
 *
 * This function is responsible for deinitializing only wireless.
 *
 */
void wm_wlan_deinit();

/** Initialize SDK core components plus WLAN
 *
 * This function is responsible for initializing core SDK components like
 * uart, console, time, power management, health monitor, flash partitions
 * and wireless. Internally it calls the following apis:
 *  - \ref wmstdio_init()
 *  - \ref cli_init()
 *  - \ref wmtime_init()
 *  - \ref pm_init()
 *  - \ref healthmon_init()
 *  - \ref part_init()
 *  - \ref wlan_init()
 *
 * \return Returns 0 on success, error otherwise.
 * */
int wm_core_and_wlan_init(void);

#endif /* ! _WM_WLAN_H_ */
