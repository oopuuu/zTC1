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
#include <stdarg.h>
#include "common.h"
#include "moc_api.h"


/******************************************************
 *                      Macros
 ******************************************************/

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

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/
extern int mico_debug_enabled;
extern const mico_api_t *lib_api_p;

/******************************************************
 *               Function Definitions
 ******************************************************/

int socket(int domain, int type, int protocol)
{
    return lib_api_p->lwip_apis->lwip_socket(domain, type, protocol);
}

int setsockopt (int socket, int level, int optname, void *optval, socklen_t optlen)
{
    return lib_api_p->lwip_apis->lwip_setsockopt(socket, level, optname,optval, optlen);
}

int getsockopt (int socket, int level, int optname, void *optval, socklen_t *optlen_ptr)
{
    return lib_api_p->lwip_apis->lwip_getsockopt(socket, level, optname, optval, optlen_ptr);
}

int bind (int socket, struct sockaddr *addr, socklen_t length)
{
    return lib_api_p->lwip_apis->lwip_bind(socket, addr, length);
}

int connect (int socket, struct sockaddr *addr, socklen_t length)
{
    return lib_api_p->lwip_apis->lwip_connect(socket, addr, length);
}

int listen(int sockfd, int backlog)
{
    return lib_api_p->lwip_apis->lwip_listen(sockfd, backlog);
}

int accept (int socket, struct sockaddr *addr, socklen_t *length_ptr)
{
    return lib_api_p->lwip_apis->lwip_accept(socket, addr, length_ptr);
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	nfds = 64;
    return lib_api_p->lwip_apis->lwip_select(nfds, readfds, writefds, exceptfds, timeout);
}

int poll(struct pollfd *fds, int nfds, int timeout)
{
	int maxfd=0;
	int i, n;
	fd_set rfds, wfds, efds;
	struct timeval t;
	int ret = 0, got;

	if (nfds <= 0)
		return 0;
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
	}
	if (timeout < 0) {
		n = select(maxfd+1, &rfds, &wfds, &efds, NULL);
	} else {
		t.tv_sec = timeout / 1000;
		t.tv_usec = (timeout % 1000) * 1000;
		n = select(maxfd+1, &rfds, &wfds, &efds, &t);
	}

	if (n <= 0) {
		return n;
	}
	for(i=0; i<nfds; i++) {
		got=0;
		if (FD_ISSET(fds[i].fd, &rfds)) {
			fds[i].revents |= fds[i].events & (POLLIN|POLLPRI);
			got = 1;
		}
		if (FD_ISSET(fds[i].fd, &wfds)) {
			fds[i].revents |= fds[i].events & POLLOUT;
			got = 1;
		}
		if (FD_ISSET(fds[i].fd, &efds)) {
			fds[i].revents |= fds[i].events & (POLLERR|POLLHUP|POLLNVAL);
			got = 1;
		}
		if (got == 1) {
			ret++;
		}
	}

	return ret;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
    return lib_api_p->lwip_apis->lwip_send(sockfd, buf, len, flags);
}

ssize_t write(int filedes, const void *buffer, size_t size)
{
    return lib_api_p->lwip_apis->lwip_write(filedes, buffer, size);
}

int sendto (int socket, const void *buffer, size_t size, int flags, const struct sockaddr *addr, socklen_t length)
{
    return lib_api_p->lwip_apis->lwip_sendto(socket, buffer, size, flags, addr, length);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
    return lib_api_p->lwip_apis->lwip_recv(sockfd, buf, len, flags);
}

int read(int filedes, void *buf, size_t len)
{
    return lib_api_p->lwip_apis->lwip_read(filedes, buf, len);
}

int recvfrom (int socket, void *buffer, size_t size, int flags, struct sockaddr *addr, socklen_t *length_ptr)
{
    return lib_api_p->lwip_apis->lwip_recvfrom(socket, buffer, size, flags, addr, length_ptr);
}

int close(int filedes)
{
    return lib_api_p->lwip_apis->lwip_close(filedes);
}

uint32_t inet_addr (const char *name)
{
    return lib_api_p->lwip_apis->ipaddr_addr(name);
}

char *inet_ntoa (struct in_addr addr)
{
    return lib_api_p->lwip_apis->ipaddr_ntoa((ip_addr_t*) &(addr) );
}

struct hostent* gethostbyname(const char *name)
{
    return lib_api_p->lwip_apis->lwip_gethostbyname(name);
}

int getaddrinfo(const char *nodename,
       const char *servname,
       const struct addrinfo *hints,
       struct addrinfo **res)
{
	return lib_api_p->lwip_apis->lwip_getaddrinfo(nodename,servname,hints,res);
}

void freeaddrinfo(struct addrinfo *ai)
{
	lib_api_p->lwip_apis->lwip_freeaddrinfo(ai);
}
int getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
	return lib_api_p->lwip_apis->lwip_getpeername (s, name, namelen);
}

int getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
	return lib_api_p->lwip_apis->lwip_getsockname (s, name, namelen);
}

int shutdown(int s, int how)
{
	return lib_api_p->lwip_apis->lwip_shutdown(s, how);
}

int ioctl(int s, int cmd, ...)
{
	va_list ap;
	va_start( ap, cmd );
	void *para = va_arg( ap, void *);
	va_end( ap );
    return lib_api_p->lwip_apis->lwip_ioctl(s, cmd, para);
}

int fcntl(int s, int cmd, ...)
{
	va_list ap;
	va_start( ap, cmd );
	int para = va_arg( ap, int);
	va_end( ap );
    return lib_api_p->lwip_apis->lwip_fcntl(s, cmd, para);
}

char *sethostname( char *name )
{
	return lib_api_p->lwip_apis->sethostname(name);
}

char* get_dhcp_classid( void )
{
	return lib_api_p->lwip_apis->get_dhcp_classid();
}

char* set_dhcp_classid( char *classid )
{
	return lib_api_p->lwip_apis->set_dhcp_classid(classid);
}

const char * inet_ntop (int af, const void *cp, char *buf, socklen_t len)
{
    return lib_api_p->lwip_apis->inet_ntop(af, cp, buf, len);
}

int inet_pton (int af, const char *cp, void *buf)
{
    return lib_api_p->lwip_apis->inet_pton (af, cp, buf);
}


