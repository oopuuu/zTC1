/*! \file wm_supplicant.h
 * \brief WMSDK Host Supplicant
 */
/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 *
 */

#ifndef __SUPPLICANT_H__
#define __SUPPLICANT_H__

#include <wlan.h>

typedef PACK_START enum
{
	SUPP_STA_INIT = 1,
	SUPP_STA_ALLOC,
	SUPP_STA_FREE,
	SUPP_STA_ENABLE,
	SUPP_STA_SET_PASSPHRASE,
	SUPP_STA_REMOVE_PASSPHRASE,
	SUPP_STA_SET_PMK,
	SUPP_STA_SESS_INIT,
	SUPP_STA_SESS_FAILURE,
	SUPP_STA_DISABLE,
	SUPP_STA_DEAUTH_DATA,
	SUPP_UAP_INIT,
	SUPP_UAP_ALLOC,
	SUPP_UAP_FREE,
	SUPP_UAP_ENABLE,
	SUPP_UAP_SET_PASSPHRASE,
	SUPP_UAP_SESS_INIT,
	SUPP_UAP_DISABLE,
	SUPP_UAP_DEINIT,
	SUPP_UAP_ASSOC_DATA,
	SUPP_UAP_DEAUTH_DATA,
	SUPP_DATA,
} PACK_END supplicant_cmd_id_t;

/** enum : SUPP events */
enum supp_event {
	/** SUPP thread started */
	SUPP_STARTED = 0,
	/** SUPP PBC/PIN Session started */
	SUPP_SESSION_STARTED,
	/** SUPP Session registration timeout */
	SUPP_SESSION_TIMEOUT,
	/** SUPP Session attempt successful */
	SUPP_SESSION_SUCCESSFUL,
	/** SUPP Session failed */
	SUPP_SESSION_FAILED,
	/** SUPP start failed */
	SUPP_FAILED,
	/** SUPP thread stopped */
	SUPP_FINISHED
};

/* Supplicant APIs */

/** Start supplicant thread
 *
 * \param[in] supp_callback supplicant callback function pointer.
 *
 * \return WM_SUCCESS on success or -WM_FAIL on error.
 *
 */
int supplicant_start(int (*supp_callback)
		(enum supp_event event, void *data, uint16_t len));


/** Send command to supplicant thread.
 *
 * \param[in] cmd_id Command ID.
 * \param[in] ssid SSID of known wlan network.
 * \param[in] security Security of known wlan network.
 * \param[in] mcstCipher Multicast cipher of known wlan network.
 * \param[in] ucstCipher Unicast cipher of known wlan network.
 * \param[in] passphrase Passphrase of known wlan network.
 * \param[in] bssid BSSID of known wlan network.
 * \param[in] sta_mac MAC address of station associated to uAP network.
 * \param[in] is_pmf_required PMF support is required or not.
 *
 * \return WM_SUCCESS on success or -WM_FAIL on error.
 *
 */
int supplicant_send_cmd(supplicant_cmd_id_t cmd_id, char *ssid, int security,
		struct wlan_cipher *mcstCipher, struct wlan_cipher *ucstCipher,
		char *passphrase, char *bssid, uint8_t *sta_mac,
		bool is_pmf_required);

/** Stop supplicant thread */
void supplicant_finish(void);

#endif
