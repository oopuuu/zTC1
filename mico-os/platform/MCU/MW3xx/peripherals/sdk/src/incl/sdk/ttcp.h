/*! \file ttcp.h
 *  \brief ttcp network traffic source and sink utility
 *
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef _TTCP_H_
#define _TTCP_H_

#include <wmerrno.h>
#include <wmlog.h>

#define ttcp_e(...)				\
	wmlog_e("ttcp", ##__VA_ARGS__)
#define ttcp_w(...)				\
	wmlog_w("ttcp", ##__VA_ARGS__)

/** TTCP Error Codes
*/
enum wm_ttcp_errno {
	WM_E_TTCP_ERRNO_BASE = MOD_ERROR_START(MOD_TTCP),
	/** TTCP command registration failed*/
	WM_E_TTCP_REGISTER_CMDS,
	/** Received failed in mread*/
	WM_E_TTCP_MREAD_RECV,
	/** nread -b option received*/
	WM_E_TTCP_NREAD_B_OPTION,
	/** Rx thread already running*/
	WM_E_TTCP_THREAD_RUNNING,
	/** Failed to create Rx thread*/
	WM_E_TTCP_RX_THREAD_CREATE,
	/* Failed to create Tx thread */
	WM_E_TTCP_TX_THREAD_CREATE,
	/** Failed to create socket*/
	WM_E_TTCP_SOCKET,
};

int ttcp_init(void);

#endif
