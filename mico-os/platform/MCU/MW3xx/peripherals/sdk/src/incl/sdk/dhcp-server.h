/*! \file dhcp-server.h
 *  \brief DHCP server
 *
 * The DHCP Server is required in the provisioning mode of the application to
 * assign IP Address to Wireless Clients that connect to the WM.
 */
/*  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __DHCP_SERVER_H__
#define __DHCP_SERVER_H__

#include <wmerrno.h>

/** DHCPD Error Codes
 */

enum wm_dhcpd_errno {
	WM_E_DHCPD_ERRNO_BASE = MOD_ERROR_START(MOD_DHCPD),
	/** Dhcp server is already running */
	WM_E_DHCPD_SERVER_RUNNING,
	/** Failed to create dhcp thread */
	WM_E_DHCPD_THREAD_CREATE,
	/** Failed to create dhcp mutex */
	WM_E_DHCPD_MUTEX_CREATE,
	/** Failed to register dhcp commands */
	WM_E_DHCPD_REGISTER_CMDS,
	/** Failed to send dhcp response */
	WM_E_DHCPD_RESP_SEND,
	/** Ignore as msg is not a valid dns query */
	WM_E_DHCPD_DNS_IGNORE,
	/** Buffer overflow occurred */
	WM_E_DHCPD_BUFFER_FULL,
	/** The input message is NULL or has incorrect length */
	WM_E_DHCPD_INVALID_INPUT,
	/** Invalid opcode in the dhcp message */
	WM_E_DHCPD_INVALID_OPCODE,
	/** Invalid header type or incorrect header length */
	WM_E_DHCPD_INCORRECT_HEADER,
	/** Spoof length is either NULL or it exceeds max length */
	WM_E_DHCPD_SPOOF_NAME,
	/** Failed to get broadcast address */
	WM_E_DHCPD_BCAST_ADDR,
	/** Failed to look up requested IP address from the interface */
	WM_E_DHCPD_IP_ADDR,
	/** Failed to look up requested netmask from the interface */
	WM_E_DHCPD_NETMASK,
	/** Failed to create the socket */
	WM_E_DHCPD_SOCKET,
	/** Failed to send Gratuitous ARP */
	WM_E_DHCPD_ARP_SEND,
	/** Error in ioctl call */
	WM_E_DHCPD_IOCTL_CALL,
	/** Failed to init dhcp server */
	WM_E_DHCPD_INIT,

};

/* Maximum length of the name_to_spoof for the DNS spoofer (see
 * dhcp_server_start below)
 */
#define MAX_QNAME_SIZE		32

 /** Register DHCP server commands
 *
 * This function registers the CLI dhcp-stat for the DHCP server.
 * dhcp-stat command displays ip to associated client mac mapping.
 *
 * @return -WM_E_DHCPD_REGISTER_CMDS if cli init operation failed.
 * @return WM_SUCCESS if cli init operation success.
 */

int dhcpd_cli_init(void);
void dhcp_stat(int argc, char **argv);

/** Start DHCP server
 *
 * This starts the DHCP server on the interface specified. Typically DHCP server
 * should be running on the micro-AP interface but it can also run on wifi
 * direct interface if configured as group owner. Use net_get_uap_handle() to
 * get micro-AP interface handle and net_get_wfd_handle() to get wifi-direct
 * interface handle.
 *
 * \param[in] intrfc_handle The interface handle on which DHCP server will start
 *
 * \return WM_SUCCESS on success or error code
 */
int dhcp_server_start(void *intrfc_handle);

/** Start DNS server
 *
 * This starts the DNS server on the interface specified for dhcp server. This
 * function needs to be used before dhcp_server_start() function.
 *
 * If dns queries are not resolved, some of the client devices do not show WiFi
 * signal strength symbol when connected to micro-AP in open mode.
 * With dns server support enabled, dns server responds with ERROR_REFUSED
 * indicating that the DNS server refuses to provide whatever data client is
 * asking for.
 */
void dhcp_enable_nack_dns_server();

/** Stop DHCP server
 */
void dhcp_server_stop(void);

/** Configure the DHCP dynamic IP lease time
 *
 * This API configures the dynamic IP lease time, which
 * should be invoked before DHCP server initialization
 *
 * \param[in] val Number of seconds, use (60U*60U*number of hours)
 *             for clarity. Max value is (60U*60U*24U*49700U)
 *
 * \return Error status code
 */
int dhcp_server_lease_timeout(uint32_t val);

/** Get IP address corresponding to MAC address from dhcpd ip-mac mapping
 *
 * This API returns IP address mapping to the MAC address present in cache.
 * IP-MAC cache stores MAC to IP mapping of previously or currently connected
 * clients.
 *
 * \param[in] client_mac Pointer to a six byte array containing the MAC address
 * of the client
 * \param[out] client_ip Pointer to IP address of the client
 *
 * \return WM_SUCCESS on success or -WM_FAIL.
 */
int dhcp_get_ip_from_mac(uint8_t *client_mac, uint32_t *client_ip);
#endif
