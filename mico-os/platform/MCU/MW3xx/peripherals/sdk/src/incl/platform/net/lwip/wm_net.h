/*! \file wm_net.h
 *\brief Network Abstraction Layer
 *
 * This provides the calls related to the network layer. The SDK uses lwIP as
 * the network stack.
 *
 * Here we document the network utility functions provided by the SDK. The
 * detailed lwIP API documentation can be found at:
 * http://lwip.wikia.com/wiki/Application_API_layers
 *
 */
/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef _WM_NET_H_
#define _WM_NET_H_

#include <string.h>
#include <errno.h>

#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/stats.h>
#include <lwip/icmp.h>
#include <lwip/ip.h>
#include <lwip/inet_chksum.h>
#include "lwip/pbuf.h"

#include <wm_os.h>
#include <wmtypes.h>

/*
 * fixme: This dependancy of wm_net on wlc manager header should be
 * removed. This is the lowest level file used to access lwip
 * functionality and should not contain higher layer dependancies.
 */
#include <wlan.h>

#define NET_SUCCESS WM_SUCCESS
#define NET_ERROR (-WM_FAIL)
#define NET_ENOBUFS ENOBUFS

#define NET_BLOCKING_OFF 1
#define NET_BLOCKING_ON	0

/* Error Codes
 * lwIP provides all standard errnos defined in arch.h, hence no need to
 * redefine them here.
 * */

/* To be consistent with naming convention */
#define net_socket(domain, type, protocol) socket(domain, type, protocol)
#define net_select(nfd, read, write, except, timeout) \
			select(nfd, read, write, except, timeout)
#define net_bind(sock, addr, len) bind(sock, addr, len)
#define net_listen(sock, backlog) listen(sock, backlog)
#define net_close(c) close((c))
#define net_accept(sock, addr, len) accept(sock, addr, len)
#define net_shutdown(c, b) shutdown(c, b)
#define net_connect(sock, addr, len) connect(sock, addr, len)
#define net_read(sock, data, len) read(sock, data, len)
#define net_write(sock, data, len) write(sock, data, len)

int net_dhcp_hostname_set(char *);
#if LWIP_STATS
extern struct stats_ lwip_stats;

static inline int net_get_tx()
{
	return lwip_stats.link.xmit;
}

static inline int net_get_rx()
{
	return lwip_stats.link.recv;
}

static inline int net_get_error()
{
	int temp;
	temp = lwip_stats.link.drop + lwip_stats.link.chkerr +
		lwip_stats.link.lenerr + lwip_stats.link.memerr+
		lwip_stats.link.rterr + lwip_stats.link.proterr+
		lwip_stats.link.opterr + lwip_stats.link.err;
	return temp;
}
#else
static inline int net_get_tx()
{
	return 0;
}

static inline int net_get_rx()
{
	return 0;
}

static inline int net_get_error()
{
	return 0;
}

#endif
static inline int net_set_mcast_interface()
{
	return WM_SUCCESS;
}

static inline int net_socket_blocking(int sock, int state)
{
	return lwip_ioctl(sock, FIONBIO, &state);
}

static inline int net_get_sock_error(int sock)
{
	switch (errno) {
	case EWOULDBLOCK:
		return -WM_E_AGAIN;
	case EBADF:
		return -WM_E_BADF;
	case ENOBUFS:
		return -WM_E_NOMEM;
	default:
		return errno;
	}
}

static inline uint32_t net_inet_aton(const char *cp)
{
	struct in_addr addr;
	inet_aton(cp, &addr);
	return addr.s_addr;
}

/**
 * Get network host entry
 *
 * @param[in] cp Hostname or an IPv4 address in the standard dot notation.
 * @param[in] hentry Pointer to pointer of host entry structure.
 *
 * @note This function is not thread safe. If thread safety is required
 * please use lwip_getaddrinfo() - lwip_freeaddrinfo() combination.
 *
 * @return WM_SUCESS if operation successful.
 * @return -WM_FAIL if operation fails.
 */
static inline int net_gethostbyname(const char *cp, struct hostent **hentry)
{
	struct hostent *he;
	if ((he = gethostbyname(cp)) == NULL)
		return -WM_FAIL;

	*hentry = he;
	return WM_SUCCESS;
}

static inline void net_inet_ntoa(unsigned long addr, char *cp)
{
	struct in_addr saddr;
	saddr.s_addr = addr;
	/* No length, sigh! */
	strcpy(cp, inet_ntoa(saddr));
}

/** Add application defined UDP broadcast filters
 *
 * By default, UDP broadcast packets received on ports DHCPD(0x43), DHCPC(0x44)
 * and DNS(0x35) are accepted and others are discarded. This is done in order to
 * save on extra processing time and memory requirement in networking stack for
 * un-necessary UDP broadcast packets. Application needs to call this API to
 * allow and process UDP broadcast packet coming on specific port other than
 * ports mentioned above before \ref wlan_init().
 *
 * It will add filter in process_data_packet() function which will check if the
 * packet received is UDP broadcast packet with destination port matching with
 * the port_number. If yes, it will accept the packet, otherwise discard it.
 *
 * Currently, this API allows IPv4 as well as IPv6 UDP broadcast traffic coming
 * on given port. In future, an additional parameter needs to be added which
 * will specify IPv4, IPv6 or both.
 *
 * \param[in] port_number Port number on which UDP broadcast packets need to be
 * accepted
 *
 * \return WM_SUCCESS on success
 * \return -WM_FAIL otherwise
 */
int netif_add_udp_broadcast_filter(uint16_t port_number);

/** Remove UDP broadcast packet filter
 *
 * This API will remove UDP broadcast packet filter for a port. And hence,
 * UDP broadcast packets coming on this port will be dropped here onwards.
 *
 * \param[in] port_number Port number on which filter was applied and needs to
 * be removed.
 *
 * \return WM_SUCCESS on success
 * \return -WM_FAIL otherwise
 */
int netif_remove_udp_broadcast_filter(uint16_t port_number);

void *net_ip_to_iface(uint32_t ipaddr);

/** Get interface handle from socket descriptor
 *
 * Given a socket descriptor this API returns which interface it is bound with.
 *
 * \param[in] sock socket descriptor
 *
 * \return[out] interface handle
 */
void *net_sock_to_interface(int sock);

void net_wlan_init(void);

/** Get station interface handle
 *
 * Some APIs require the interface handle to be passed to them. The handle can
 * be retrieved using this API.
 *
 * \return station interface handle
 */
void *net_get_sta_handle(void);
#define net_get_mlan_handle() net_get_sta_handle()

/** Get micro-AP interface handle
 *
 * Some APIs require the interface handle to be passed to them. The handle can
 * be retrieved using this API.
 *
 * \return micro-AP interface handle
 */
void *net_get_uap_handle(void);

/** Get WiFi Direct (P2P) interface handle
 *
 * Some APIs require the interface handle to be passed to them. The handle can
 * be retrieved using this API.
 *
 * \return WiFi Direct (P2P) interface handle
 */
void *net_get_wfd_handle(void);

/** Take interface up
 *
 * Change interface state to up. Use net_get_sta_handle(),
 * net_get_uap_handle() or net_get_wfd_handle() to get interface handle.
 *
 * \param[in] intrfc_handle interface handle
 *
 * \return void
 */
void net_interface_up(void *intrfc_handle);

/** Take interface down
 *
 * Change interface state to down. Use net_get_sta_handle(),
 * net_get_uap_handle() or net_get_wfd_handle() to get interface handle.
 *
 * \param[in] intrfc_handle interface handle
 *
 * \return void
 */
void net_interface_down(void *intrfc_handle);

/** Stop DHCP client on given interface
 *
 * Stop the DHCP client on given interface state. Use net_get_sta_handle(),
 * net_get_uap_handle() or net_get_wfd_handle() to get interface handle.
 *
 * \param[in] intrfc_handle interface handle
 *
 * \return void
 */
void net_interface_dhcp_stop(void *intrfc_handle);

int net_configure_address(struct wlan_ip_config *addr, void *intrfc_handle);

void net_configure_dns(struct wlan_ip_config *ip, enum wlan_bss_role role);

/** Get interface IP Address in \ref wlan_ip_config
 *
 * This function will get the IP address of a given interface. Use
 * net_get_sta_handle(), net_get_uap_handle() or net_get_wfd_handle() to get
 * interface handle.
 *
 * \param[out] addr \ref wlan_ip_config
 * \param[in] intrfc_handle interface handle
 *
 * \return WM_SUCCESS on success or error code.
 */
int net_get_if_addr(struct wlan_ip_config *addr, void *intrfc_handle);

#ifdef CONFIG_IPV6
/** Get interface IPv6 Addresses & their states in \ref wlan_ip_config
 *
 * This function will get the IPv6 addresses & address states of a given
 * interface. Use net_get_sta_handle() to get interface handle.
 *
 * \param[out] addr \ref wlan_ip_config
 * \param[in] intrfc_handle interface handle
 *
 * \return WM_SUCCESS on success or error code.
 */
int net_get_if_ipv6_addr(struct wlan_ip_config *addr, void *intrfc_handle);

/** Get list of preferred IPv6 Addresses of a given interface
 * in \ref wlan_ip_config
 *
 * This function will get the list of IPv6 addresses whose address state
 * is Preferred.
 * Use net_get_sta_handle() to get interface handle.
 *
 * \param[out] addr \ref wlan_ip_config
 * \param[in] intrfc_handle interface handle
 *
 * \return Number of IPv6 addresses whose address state is Preferred
 */
int net_get_if_ipv6_pref_addr(struct wlan_ip_config *addr, void *intrfc_handle);

/** Get the description of IPv6 address state
 *
 * This function will get the IPv6 address state description like -
 * Invalid, Preferred, Deprecated
 *
 * \param[in] addr_state Address state
 *
 * \return IPv6 address state description
 */
char *ipv6_addr_state_to_desc(unsigned char addr_state);

/** Get the description of IPv6 address type
 *
 * This function will get the IPv6 address type description like -
 * Linklocal, Global, Sitelocal, Uniquelocal
 *
 * \param[in] ipv6_conf Pointer to IPv6 configuration of type \ref ipv6_config
 *
 * \return IPv6 address type description
 */
char *ipv6_addr_type_to_desc(struct ipv6_config *ipv6_conf);

/** Deregister ipv6 callback for given interface handle.
 *
 * Deregisters ipv6 callback on given interface state.
 * Use net_get_sta_handle(), net_get_uap_handle()
 * or net_get_wfd_handle() to get interface handle.
 *
 * \param[in] intrfc_handle interface handle
 *
 *\return void
 */
void net_interface_deregister_ipv6_callback(void *intrfc_handle);
#endif /* CONFIG_IPV6 */

/** Get interface IP Address
 *
 * This function will get the IP Address of a given interface. Use
 * net_get_sta_handle(), net_get_uap_handle() or net_get_wfd_handle() to get
 * interface handle.
 *
 * \param[out] ip ip address pointer
 * \param[in] intrfc_handle interface handle
 *
 * \return WM_SUCCESS on success or error code.
 */
int net_get_if_ip_addr(uint32_t *ip, void *intrfc_handle);

/** Get interface IP Subnet-Mask
 *
 * This function will get the Subnet-Mask of a given interface. Use
 * net_get_sta_handle(), net_get_uap_handle() or net_get_wfd_handle() to get
 * interface handle.
 *
 * \param[in] mask Subnet Mask pointer
 * \param[in] intrfc_handle interface
 *
 * \return WM_SUCCESS on success or error code.
 */
int net_get_if_ip_mask(uint32_t *mask, void *intrfc_handle);

/** Initialize the network stack
 *
 *
 *  This function initializes the network stack. This function is
 *  called by wlan_start().
 *
 *  Applications may optionally call this function directly:
 *  # if they wish to use the networking stack (loopback interface)
 *  without the wlan functionality.
 *  # if they wish to initialize the networking stack even before wlan
 *  comes up.
 *
 * \note This function may safely be called multiple times.
 */
void net_ipv4stack_init(void);
void net_ipv6stack_init(struct netif *netif);

/**
 * Frame Tx - Injecting Wireless frames from Host
 *
 * This function is used to Inject Wireless frames from application
 * directly.
 *
 * \param[in] interface Interface on which frame to be injected.
 * \param[in] buf Buffer holding 802.11 Wireless frame (Header + Data).
 * \param[in] len Length of the 802.11 Wireless frame.
 *
 * \return WM_SUCCESS on success or error code.
 *
 */
int raw_low_level_output(const u8_t interface, const u8_t *buf, t_u32 len);


void net_sockinfo_dump();

void net_stat(char *name);
#ifdef CONFIG_P2P
int netif_get_bss_type();
#endif

static inline void net_diag_stats(char *str, int len)
{
	/* Report stats */
	strncpy(str, "", len);
}

#endif /* _WM_NET_H_ */
