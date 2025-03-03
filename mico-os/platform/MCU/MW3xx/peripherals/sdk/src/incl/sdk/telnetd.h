/*! \file telnetd.h
 *  \brief The Telnet Server
 */
/* Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved
 *
 */
#ifndef _WMSDK_TELNETD_
#define _WMSDK_TELNETD_

#include <wmstdio.h>
#include <wmerrno.h>

typedef struct telnetd_s {
	/* The listening socket */
	int socket;
	/* The connected socket */
	int conn;
	/* Flag to instruct the thread to stop on 0 */
	int run;
} telnetd_t;

extern telnetd_t telnetd;

/** TELNETD Error Codes
*/
enum wm_telnetd_errno {
	WM_E_TELNETD_ERRNO_BASE = MOD_ERROR_START(MOD_TELNETD),
	/** Telnet server is already running */
	WM_E_TELNETD_SERVER_RUNNING,
	/** Failed to create telnet thread */
	WM_E_TLENETD_THREAD_CREATE,
	/** Failed to create the socket */
	WM_E_TELNETD_SOCKET,
	/** Failed to bind the socket */
	WM_E_TELNETD_BIND_SOCKET,
	/** Failed to listen on socket */
	WM_E_TELNETD_LISTEN_SOCKET,
	/** Failed to accept new socket */
	WM_E_TELNETD_ACCEPT_SOCKET,
	/** Failed to make socket non-blocking */
	WM_E_TELNETD_NONBLOCKING_SOCKET,
};
/** Start the telnet daemon
 *
 * This function starts the telnetd daemon. Remote clients can connect
 * to the telnet server and execute commands on the CLI.
 *
 * \param port The port number to listen for connections.
 *
 * \return -WM_E_TELNETD_SERVER_RUNNING if server is already running.
 * \return -WM_E_TLENETD_THREAD_CREATE if telnet start failed.
 * \return WM_SUCCESS if telnetd start was successful.
 */
int telnetd_start(int port);

/** Stop the telnet daemon
 *
 * This function stops the telnet daemon.
 *
 * \note This function is usually called on a network disconnect/link
 * lost event.
 *
 */
void telnetd_stop(void);

#endif /* ! _WMSDK_TELNETD_ */
