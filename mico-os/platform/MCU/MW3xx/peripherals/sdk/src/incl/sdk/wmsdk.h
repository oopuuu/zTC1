/*! \file wmsdk.h
 *  \brief WMSDK APIs
 *
 * This modules provides APIs for the various features offered by the WMSDK.
 */

/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */
#ifndef _WMSDK_H_
#define _WMSDK_H_

#include <wmstdio.h>
#include <wm_utils.h>

/** Connect to an Access Point using the ssid and passphrase
 *
 * This function initializes the Wi-Fi subsystem and connects to an
 * Access Point using the ssid and psk.
 *
 * \param[in] ssid SSID of the Access Point
 * \param[in] psk passphrase of the Access Point. Pass NULL for open networks.
 *
 * \return WM_SUCCESS on success
 * \return negative error code otherwise
 */
int wm_wlan_connect(char *ssid, char *psk);

/** Start configuration app, and connect to Access Point if configured
 *
 * - If the network configuration settings are done, this function initializes
 * the Wi-Fi subsystem to connect to the Access Point as configured.
 * - If the network configuration settings are not done, this function starts a
 * Wi-Fi network (micro-AP) of its own, with the my_ssid/my_wpa2_passphrase. An
 * end-user can connect to this micro-AP network and enter home network
 * configuration. Once the configuration is received, it then connects to the
 * Access Point that is configured.
 *
 * \param[in] my_ssid The SSID of the micro-AP network that should be hosted
 * \param[in] my_wpa2_passphrase The passphrase of the micro-AP network that
 * should be hosted
 *
 * \return WM_SUCCESS on success
 * \return negative error code otherwise
 */
int wm_wlan_start(char *my_ssid, char *my_wpa2_passphrase);

/** Callback function when Wi-Fi station is connected
 *
 * This function gets called when the station interface of the Wi-Fi network
 * successfully associates with an Access Point.
 */
WEAK void wlan_event_normal_connected(void *data);

/** Callback function when Wi-Fi station connection fails
 *
 * This function gets called when the station interface of the Wi-Fi network
 * fails to associate with an Access Point. The station will continue to make
 * connection attempts infinitely. This function will be called for connection
 * failure.
 */
WEAK void wlan_event_connect_failed();

/** Callback function when Wi-Fi station link is lost
 *
 * This function gets called when the station interface of the Wi-Fi network
 * loses the connection that was previously established with the Access
 * Point. The station will continue to make connection attempts to the Access
 * Point, after this callback returns.
 */
WEAK void wlan_event_normal_link_lost(void *data);

/** Reset the device to factory settings
 *
 * Erase all the configured information in this device, including network
 * credentials and any other configuration data. Once all the settings are erase
 * reboot the device to start fresh.
 */
int invoke_reset_to_factory();

/** These APIs are left undocumented for now */
void wm_wlan_set_httpd_handlers(void *hdlrs, int no);

#endif /* ! _WMSDK_H_ */
