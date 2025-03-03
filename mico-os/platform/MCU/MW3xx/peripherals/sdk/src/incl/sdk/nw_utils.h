/*! \file nw_utils.h
 * \brief Networking CLIs
 *
 * Collection of networking CLIs like ping etc.
 */

/*
 * Copyright (C) 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef _NW_UTILS_H_
#define _NW_UTILS_H_

#include <wmlog.h>

#define nw_utils_e(...)				\
	wmlog_e("nw_utils", ##__VA_ARGS__)
#define nw_utils_w(...)				\
	wmlog_w("nw_utils", ##__VA_ARGS__)


/* #define CONFIG_NW_UTILS_DEBUG */
#ifdef CONFIG_NW_UTILS_DEBUG
#define nw_utils_d(...)				\
	wmlog("nw_utils", ##__VA_ARGS__)
#else
#define nw_utils_d(...)
#endif /* ! CONFIG_NW_UTILS_DEBUG */

#define PING_ID				0xAFAF
#define PING_INTERVAL			1000
#define PING_DEFAULT_TIMEOUT_SEC	2
#define PING_DEFAULT_COUNT		5
#define PING_DEFAULT_SIZE		56
#define PING_MAX_SIZE			65507


/** Register Network Utility CLI commands.
 *
 *  Register the Network Utility CLI commands. Currently, only ping command is
 *  supported.
 *
 *  \note This function can only be called by the application after \ref
 *  wlan_init() called.
 *
 *  \return WM_SUCCESS if the CLI commands are registered
 *  \return -WM_FAIL otherwise (for example if this function
 *          was called while the CLI commands were already registered)
 */

int nw_utils_cli_init();


/** Unregister Network Utility CLI commands.
 *
 *  Unregister the Network Utility CLI commands.
 *
 *  \return WM_SUCCESS if the CLI commands are unregistered
 *  \return -WM_FAIL otherwise
 */

int nw_utils_cli_deinit();
#endif  /*_NW_UTILS_H_ */
