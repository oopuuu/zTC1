/**
 ******************************************************************************
 * @file    mico_socket.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-Aug-2018
 * @brief   This file provide the MiCO Socket abstract layer convert functions.
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */


#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

/******************************************************
 *                      Macros
 ******************************************************/

#ifdef inet_addr
#undef inet_addr
#endif


#ifdef inet_ntoa
#undef inet_ntoa
#endif

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
struct pollfd {
	int fd; /**< fd related to */
	short events; /**< which POLL... events to respond to */
	short revents; /**< which POLL... events occurred */
};
#define POLLIN		0x0001
#define POLLPRI		0x0002
#define POLLOUT		0x0004
#define POLLERR		0x0008
#define POLLHUP		0x0010
#define POLLNVAL	0x0020

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

int socket(int domain, int type, int protocol)
{
    return lwip_socket( domain, type, protocol );
}

int setsockopt (int socket, int level, int optname, void *optval, socklen_t optlen)
{
    return lwip_setsockopt( socket, level, optname, optval, optlen );
}

int getsockopt (int socket, int level, int optname, void *optval, socklen_t *optlen_ptr)
{
    return lwip_getsockopt( socket, level, optname, optval, optlen_ptr );
}

int bind (int socket, struct sockaddr *addr, socklen_t length)
{
    return lwip_bind ( socket, addr, length);
}

int connect (int socket, struct sockaddr *addr, socklen_t length)
{
    return lwip_connect( socket, addr, length );
}

int listen (int socket, int n)
{
    return lwip_listen( socket, n );
}

int accept (int socket, struct sockaddr *addr, socklen_t *length_ptr)
{
    return lwip_accept( socket, addr, length_ptr );
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    nfds = 64;

    if ((timeout->tv_sec == 0) && (timeout->tv_usec < 1000)) // timeout must bigger than 1ms.
        timeout->tv_usec = 1000;

    return lwip_select( nfds, readfds, writefds, exceptfds, timeout );
}

int poll(struct pollfd *fds, int nfds, int timeout)
{
	int maxfd=0;
	int i, n;
	fd_set rfds, wfds, efds;
	struct timeval t;
	int ret = 0, got;

	//printf("poll nfds=%d, timeout = %d\r\n", nfds, timeout);
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);
	for(i=0; i<nfds; i++) {
		if (fds[i].fd > maxfd)
			maxfd = fds[i].fd;
		if (fds[i].events & (POLLIN|POLLPRI))
			FD_SET(fds[i].fd, &rfds); 
		if (fds[i].events & (POLLOUT))
			FD_SET(fds[i].fd, &wfds); 
		if (fds[i].events & (POLLERR|POLLHUP|POLLNVAL))
			FD_SET(fds[i].fd, &efds); 
		fds[i].revents = 0;
		//printf("<%d> fd=%d, evetns = %x\r\n", i, fds[i].fd, fds[i].events);
	}
	if (timeout < 0) {
		n = lwip_select(maxfd+1, &rfds, &wfds, &efds, NULL);
	} else {
		t.tv_sec = timeout / 1000;
		t.tv_usec = (timeout % 1000) * 1000;
		n = lwip_select(maxfd+1, &rfds, &wfds, &efds, &t);
	}
	if (n <= 0)
		return n;
	for(i=0; i<nfds; i++) {
		got=0;
		if (FD_ISSET(fds[i].fd, &rfds)) {
			fds[i].revents = fds[i].events & (POLLIN|POLLPRI);
			got = 1;
		}
		if (FD_ISSET(fds[i].fd, &wfds)) {
			fds[i].revents = fds[i].events & POLLOUT;
			got = 1;
		}
		if (FD_ISSET(fds[i].fd, &efds)) {
			fds[i].revents = fds[i].events & (POLLERR|POLLHUP|POLLNVAL);
			got = 1;
		}
		if (got == 1) {
			//printf("fd=%d, revetns = %x\r\n", fds[i].fd, fds[i].revents);
			ret++;
		}
	}

	return ret;
}

int send (int socket, const void *buffer, size_t size, int flags)
{
    return lwip_send( socket, buffer, size, flags );
}

int sendto (int socket, const void *buffer, size_t size, int flags, const struct sockaddr *addr, socklen_t length)
{
    return lwip_sendto( socket, buffer, size, flags, addr, length);
}

int recv (int socket, void *buffer, size_t size, int flags)
{
    return lwip_recv( socket, buffer, size, flags );
}

int recvfrom (int socket, void *buffer, size_t size, int flags, struct sockaddr *addr, socklen_t *length_ptr)
{
    return lwip_recvfrom( socket, buffer, size, flags, addr, length_ptr );
}

ssize_t read (int filedes, void *buffer, size_t size)
{
    return recv(filedes, buffer, size, 0);
}

ssize_t write (int filedes, const void *buffer, size_t size)
{
    return send(filedes, buffer, size, 0);
}

int close (int filedes)
{
    return lwip_close( filedes );
}
/*
int shutdown(int s, int how)
{
	return lwip_shutdown(s, how);
}
*/
struct hostent * gethostbyname (const char *name)
{
    return lwip_gethostbyname( name );
}

int getaddrinfo(const char *nodename,
       const char *servname,
       const struct addrinfo *hints,
       struct addrinfo **res)
{
	printf("%s, nodename %s, servname %s\r\n", __FUNCTION__, nodename, servname);
	return lwip_getaddrinfo(nodename,servname,hints,res);
}

void freeaddrinfo(struct addrinfo *ai)
{
	lwip_freeaddrinfo(ai);
}
int getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
	return lwip_getpeername (s, name, namelen);
}
int getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
	return lwip_getsockname (s, name, namelen);
}




uint32_t inet_addr (const char *name)
{
    return ipaddr_addr( name );
}


char *inet_ntoa (struct in_addr addr)
{
#if LwIP_VERSION_MAJOR == 1 && LwIP_VERSION_MINOR < 5
    return ipaddr_ntoa( (ip_addr_t*) &(addr) );
#elif LwIP_VERSION_MAJOR == 1 && LwIP_VERSION_MINOR >= 5
    return ipaddr_ntoa( (ip_addr_t*) &(addr) );
#elif LwIP_VERSION_MAJOR == 2
    return ipaddr_ntoa( (ip_addr_t*) &(addr) );
#else
#error LwIP version not supported
#endif
}

