/*! \file wifi-decl.h
 *  \brief Wifi structure declarations
 */

#ifndef __WIFI_DECL_H__
#define __WIFI_DECL_H__

#include <stdint.h>
#include <stdbool.h>
#include <wm_utils.h>

/* fixme: remove these after complete integration with mlan */
#define MLAN_MAC_ADDR_LENGTH     (6)
/** Version string buffer length */
#define MLAN_MAX_VER_STR_LEN    128

#define PMK_BIN_LEN 32
#define PMK_HEX_LEN 64

#ifndef INCLUDE_FROM_MLAN
typedef char t_s8;
typedef unsigned char t_u8;
#endif /* INCLUDE_FROM_MLAN */


typedef struct {
	t_u8 mac[MLAN_MAC_ADDR_LENGTH];
	/*
	 * Power management status
	 * 0 = active (not in power save)
	 * 1 = in power save status
	 */
	t_u8 power_mgmt_status;
	/** RSSI: dBm */
	t_s8 rssi;
} sta_info_t;

/*
 *  Note: This is variable length structure. The size of array mac_list is
 *  equal to count. The caller of the API which returns this structure does
 *  not need to separately free the array mac_list. It only needs to free
 *  the sta_list_t object after use.
 */
typedef struct {
	int count;
	/*
	 * Variable length array. Max size is MAX_NUM_CLIENTS.
	 */
	/* sta_info_t *list; */
} sta_list_t;

/** BSS type : STA */
#define BSS_TYPE_STA 0
/** BSS type : UAP */
#define BSS_TYPE_UAP 1

#define UAP_DEFAULT_CHANNEL 0

enum wifi_bss_security {
	WIFI_SECURITY_NONE = 0,
	WIFI_SECURITY_WEP_STATIC,
	WIFI_SECURITY_WEP_DYNAMIC,
	WIFI_SECURITY_WPA,
	WIFI_SECURITY_WPA2,
};

enum wifi_bss_features {
	WIFI_BSS_FEATURE_WMM = 0,
	WIFI_BSS_FEATURE_WPS = 1,
};

struct wifi_message {
	uint16_t event;
	uint16_t reason;
	void *data;
};

#ifdef CONFIG_P2P
struct wifi_wfd_event {
	bool peer_event;
	void *data;
};
#endif

typedef struct {
	uint8_t   wep40:1;
	uint8_t   wep104:1;
	uint8_t   tkip:1;
	uint8_t   ccmp:1;
	uint8_t   rsvd:4;
} _Cipher_t;

typedef struct {
	uint16_t     noRsn:1;
	uint16_t     wepStatic:1;
	uint16_t     wepDynamic:1;
	uint16_t     wpa:1;
	uint16_t     wpaNone:1;
	uint16_t     wpa2:1;
	uint16_t     rsvd:10;
} _SecurityMode_t;

/* TODO: clean up the parts brought over from the Host SME BSSDescriptor_t,
 * remove ifdefs, consolidate security info */

#ifndef INCLUDE_FROM_MLAN
/* fixme: remove this during cleanup */
typedef unsigned short t_u16;
typedef unsigned int t_u32;
#endif /* INCLUDE_FROM_MLAN */

/** MLAN Maximum SSID Length */
#define MLAN_MAX_SSID_LENGTH     (32)
#ifdef CONFIG_WLAN_BRIDGE
/** MLAN Maximum PASSPHRASE Length */
#define MLAN_MAX_PASS_LENGTH     (64)
#endif

struct wifi_scan_result {
	uint8_t bssid[MLAN_MAC_ADDR_LENGTH];
	bool is_ibss_bit_set;

	uint8_t ssid[MLAN_MAX_SSID_LENGTH];
	int ssid_len;
	uint8_t Channel;		/*!< Channel associated to the BSSID */
	uint8_t RSSI;		/*!< Received signal strength */
	_SecurityMode_t WPA_WPA2_WEP;
	_Cipher_t wpa_mcstCipher;
	_Cipher_t wpa_ucstCipher;
	_Cipher_t rsn_mcstCipher;
	_Cipher_t rsn_ucstCipher;
	bool is_pmf_required;

	/*!<
	 **  WPA_WPA2 = 0 => Security not enabled
	 **  = 1 => WPA mode
	 **  = 2 => WPA2 mode
	 **  = 3 => WEP mode
	 */
	bool phtcap_ie_present;
	bool phtinfo_ie_present;

	bool wmm_ie_present;
	uint8_t band;

	bool wps_IE_exist;
	uint16_t wps_session;
	bool wpa2_entp_IE_exist;
};

typedef struct {
	char mac[MLAN_MAC_ADDR_LENGTH];
} mac_addr_t;

typedef struct {
	char version_str[MLAN_MAX_VER_STR_LEN];
} wifi_fw_version_t;

typedef struct {
	char version_str[MLAN_MAX_VER_STR_LEN];
} wifi_fw_version_ext_t;


enum wlan_type {
	WLAN_TYPE_NORMAL = 0,
	WLAN_TYPE_WIFI_CALIB,
	WLAN_TYPE_FCC_CERTIFICATION,
};

enum wlan_fw_storage_type {
	WLAN_FW_IN_FLASH = 0,
	WLAN_FW_IN_RAM,
};

typedef struct {
	uint16_t current_level;
	uint8_t max_power;
	uint8_t min_power;

} wifi_tx_power_t;

typedef struct {
	uint16_t current_channel;
	uint16_t rf_type;
} wifi_rf_channel_t;

typedef struct {
	uint16_t remove;
	uint8_t status;
	uint8_t bandcfg;
	uint8_t channel;
	uint32_t remain_period;
} wifi_remain_on_channel_t;

typedef struct {
	uint8_t tx_rate;
	uint8_t rx_rate;
} wifi_tx_rate_t;

typedef struct {
	int16_t data_rssi_last;
	int16_t data_nf_last;
	int16_t data_rssi_avg;
	int16_t data_nf_avg;

	int16_t bcn_snr_last;
	int16_t bcn_snr_avg;
	int16_t data_snr_last;
	int16_t data_snr_avg;

	int16_t bcn_rssi_last;
	int16_t bcn_nf_last;
	int16_t bcn_rssi_avg;
	int16_t bcn_nf_avg;
} wifi_rssi_info_t;

/**
 * Data structure for subband set
 *
 * For uAP 11d support
 */
typedef struct {
	/** First channel */
	t_u8 first_chan;
	/** Number of channels */
	t_u8 no_of_chan;
	/** Maximum Tx power in dBm */
	t_u8 max_tx_pwr;

} wifi_sub_band_set_t;

#define COUNTRY_CODE_LEN    3

/**
 * Data structure for domain parameters
 *
 * This structure is accepted by wlan_uap_set_domain_params() API. This
 * information is used to generate the country info IE.
 */
typedef struct {
	/** Country code */
	t_u8 country_code[COUNTRY_CODE_LEN];
	/** subbands count */
	int no_of_sub_band;
	/** Set of subbands of no_of_sub_band number of elements */
	wifi_sub_band_set_t sub_band[1];
} wifi_domain_param_t;

/**
 * Data structure for Channel descriptor
 *
 * Set CFG data for Tx power limitation
 *
 * start_freq: Starting Frequency of the band for this channel
 *                  2407, 2414 or 2400 for 2.4 GHz
 *                  5000
 *                  4000
 * chan_width: Channel Width
 *                  20
 * chan_num  : Channel Number
 *
 */
typedef PACK_START struct {
	/** Starting frequency of the band for this channel */
	t_u16 start_freq;
	/** Channel width */
	t_u8 chan_width;
	/** Channel Number */
	t_u8 chan_num;
} PACK_END wifi_channel_desc_t;

/**
 * Data structure for Modulation Group
 *
 * mod_group : ModulationGroup
 *                  0: CCK (1,2,5.5,11 Mbps)
 *                  1: OFDM (6,9,12,18 Mbps)
 *                  2: OFDM (24,36 Mbps)
 *                  3: OFDM (48,54 Mbps)
 *                  4: HT20 (0,1,2)
 *                  5: HT20 (3,4)
 *                  6: HT20 (5,6,7)
 *                  7: HT40 (0,1,2)
 *                  8: HT40 (3,4)
 *                  9: HT40 (5,6,7)
 *                  10: HT2_20 (8,9,10)
 *                  11: HT2_20 (11,12)
 *                  12: HT2_20 (13,14,15)
 *                  13: HT2_40 (8,9,10)
 *                  14: HT2_40 (11,12)
 *                  15: HT2_40 (13,14,15)
 *tx_power   : Power Limit in dBm
 *
 */
typedef PACK_START struct {
	/** Modulation group */
	t_u8 mod_group;
	/** Tx Power */
	t_u8 tx_power;
} PACK_END wifi_chan_trpc_entry_t;

/**
 * Data structure for TRPC config
 *
 * For TRPC support
 */
typedef PACK_START struct {
	/** Number of modulation groups */
	t_u8 num_mod_grps;
	/** Chnannel descriptor */
	wifi_channel_desc_t chan_desc;
	/** Channel Modulation groups */
	wifi_chan_trpc_entry_t chan_trpc_entry[10];
} PACK_END chan_trpc_config_t;

/**
 * Data structure for Channel TRPC config
 *
 * For TRPC support
 */
typedef PACK_START struct {
	/** Number of Channels */
	t_u8 num_chans;
	/** TRPC config */
	chan_trpc_config_t chan_trpc_config[40];
} PACK_END chan_trpc_t;

#ifdef CONFIG_WLAN_BRIDGE
/**
 * Data structure for Bridge Autolink Configuration
 */
typedef struct {
	/** Auto Link Periodical scan interval */
	uint32_t scan_timer_interval;
	/** The condition triggers Auto Link periodical scan
	 *  0: trigger scan when current link is not good
	 *  1: trigger scan by host setting(always periodical scan)
	 */
	uint8_t scan_timer_condition;
	/** Auto Link periodical scan channel list:
	 *  0: only scan with previous In-STA associated channel
	 *  1: 2.4G all channels
	 */
	uint8_t scan_channel_list;
} wifi_autolink_cfg_t;

/**
 * Data structure for Bridge Configuration
 */
#define ENABLE_AUTOLINK_BIT		1
#define HIDDEN_SSID_BIT			2
typedef struct {
	/** Bit 0: Enable/Disable bridge mode,
	 *  Bit 1: Enable/Disable auto link,
	 *  Bit 2: Enable/Disable hidden ssid
	 */
	uint8_t enable;
	/** Auto Link */
	bool auto_link;
	/** Hideen Bridge SSID */
	bool hidden_ssid;
	/** EX-AP SSID Length */
	uint8_t ex_ap_ssid_len;
	/** EX-AP SSID */
	char ex_ap_ssid[MLAN_MAX_SSID_LENGTH];
	/** EX-AP Passphrase length */
	uint8_t ex_ap_pass_len;
	/** EX-AP Passphrase */
	char ex_ap_pass[MLAN_MAX_PASS_LENGTH];
	/** Bridge SSID Length */
	uint8_t bridge_ssid_len;
	/** Bridge SSID */
	char bridge_ssid[MLAN_MAX_SSID_LENGTH];
	/** Bridge Passphrase length */
	uint8_t bridge_pass_len;
	/** Bridge Passphrase */
	char bridge_pass[MLAN_MAX_PASS_LENGTH];
	/**auto link configuration*/
	wifi_autolink_cfg_t autolink;
} wifi_bridge_cfg_t;
#endif

typedef struct {
	uint16_t monitor_activity;
	uint16_t filter_flags;
	uint8_t radio_type;
	uint8_t chan_number;
} wifi_net_monitor_t;

typedef PACK_START struct {
	uint8_t frame_ctrl_flags;
	uint16_t duration;
	char dest[MLAN_MAC_ADDR_LENGTH];
	char src[MLAN_MAC_ADDR_LENGTH];
	char bssid[MLAN_MAC_ADDR_LENGTH];
	uint16_t seq_frag_num;
	uint8_t timestamp[8];
	uint16_t beacon_interval;
	uint16_t cap_info;
	uint8_t ssid_element_id;
	uint8_t ssid_len;
	char ssid[MLAN_MAX_SSID_LENGTH];
}  PACK_END wifi_beacon_info_t;

typedef PACK_START struct {
	uint8_t frame_ctrl_flags;
	uint16_t duration;
	char bssid[MLAN_MAC_ADDR_LENGTH];
	char src[MLAN_MAC_ADDR_LENGTH];
	char dest[MLAN_MAC_ADDR_LENGTH];
	uint16_t seq_frag_num;
	uint16_t qos_ctrl;
} PACK_END wifi_data_info_t;

 typedef enum {
	ASSOC_REQ_FRAME = 0x00,
	ASSOC_RESP_FRAME = 0x10,
	REASSOC_REQ_FRAME = 0x20,
	REASSOC_RESP_FRAME = 0x30,
	PROBE_REQ_FRAME = 0x40,
	PROBE_RESP_FRAME = 0x50,
	BEACON_FRAME = 0x80,
	DISASSOC_FRAME = 0xA0,
	AUTH_FRAME = 0xB0,
	DEAUTH_FRAME = 0xC0,
	ACTION_FRAME = 0xD0,
	DATA_FRAME = 0x08,
	QOS_DATA_FRAME = 0x88,
 } wifi_frame_type_t;
 

typedef PACK_START struct {
	wifi_frame_type_t frame_type;
	union {
		wifi_beacon_info_t beacon_info;
		wifi_data_info_t data_info;
	} frame_data;
} PACK_END wifi_frame_t;

typedef struct {
	t_u8 chan_number;
	t_u16 min_scan_time;
	t_u16 max_scan_time;
} wifi_chan_scan_param_set_t;

typedef struct {
	int no_of_channels;
	wifi_chan_scan_param_set_t chan_scan_param[1];
} wifi_chan_list_param_set_t;

/** 802_11_header packet */
typedef PACK_START struct _wifi_mgmt_frame_t {
	/** Packet Length */
	t_u16 frm_len;
	/** Frame Type */
	wifi_frame_type_t frame_type;
	/** Frame Control flags */
	t_u8 frame_ctrl_flags;
	/** Duration ID */
	t_u16 duration_id;
	/** Address1 */
	t_u8 addr1[MLAN_MAC_ADDR_LENGTH];
	/** Address2 */
	t_u8 addr2[MLAN_MAC_ADDR_LENGTH];
	/** Address3 */
	t_u8 addr3[MLAN_MAC_ADDR_LENGTH];
	/** Sequence Control */
	t_u16 seq_ctl;
	/** Address4 */
	t_u8 addr4[MLAN_MAC_ADDR_LENGTH];
	/** Frame payload */
	t_u8 payload[0];
} PACK_END wifi_mgmt_frame_t;


/** Calibration Data */
typedef PACK_START struct _wifi_cal_data_t {
	/** Calibration data length */
	t_u16 data_len;
	/** Calibration data */
	t_u8 *data;
} PACK_END wifi_cal_data_t;

typedef PACK_START struct _wifi_auto_reconnect_config_t {
	/** Reconnect counter */
	t_u8 reconnect_counter;
	/** Reconnect interval */
	t_u8 reconnect_interval;
	/** Flags */
	t_u16 flags;
} PACK_END wifi_auto_reconnect_config_t;

typedef PACK_START struct _wifi_scan_channel_list_t {
	t_u8 chan_number;
	t_u8 scan_type;
	t_u16 scan_time;
} PACK_END wifi_scan_channel_list_t;

/* Configuration for wireless scanning */
#define MAX_CHANNEL_LIST 5

typedef PACK_START struct _wifi_scan_params_v2_t {
	t_u8 bssid[MLAN_MAC_ADDR_LENGTH];
	char ssid[MLAN_MAX_SSID_LENGTH + 1];
	t_u8 num_channels;
	wifi_scan_channel_list_t chan_list[MAX_CHANNEL_LIST];
	t_u8 num_probes;
	int (*cb) (unsigned int count);
} PACK_END wifi_scan_params_v2_t;

#endif /* __WIFI_DECL_H__ */
