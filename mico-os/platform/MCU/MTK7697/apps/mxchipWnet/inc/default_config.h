/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __DEFAULT_CONFIG_H__
#define __DEFAULT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

// start of wifi profile
#define DEF_WIFI_STA_MAC_ADDR   "00:0C:43:76:87:28"
#define DEF_WIFI_AP_MAC_ADDR    "00:0C:43:76:87:30"

#define DEF_WIFI_SSID           "DontConnectMe2G"

#if 1
// open-none
#define DEF_WIFI_AUTH_MODE      (0) //0:Ndis802_11AuthModeOpen, 7:Ndis802_11AuthModeWPA2PSK
#define DEF_WIFI_ENCRYPT_TYPE   (1) //1: Ndis802_11EncryptionDisabled 5:Ndis802_11Encryption3Enabled
#else
// wpa2-psk
#define DEF_WIFI_AUTH_MODE      (7) //0:Ndis802_11AuthModeOpen, 7:Ndis802_11AuthModeWPA2PSK
#define DEF_WIFI_ENCRYPT_TYPE   (5) //1: Ndis802_11EncryptionDisabled 5:Ndis802_11Encryption3Enabled
#endif

#define DEF_WIFI_WIRELESS_MODE  (9) //PHY_11BGN_MIXED,  // if check 802.11b.      9
#define DEF_WIFI_CHANNEL        (1)

#define DEF_WIFI_BSS_TYPE       (1) //BSS_INFRA
#define DEF_WIFI_BW             (0) //BW_20
#define DEF_WIFI_MCS            (33) //MCS_AUTO
#define DEF_WIFI_COUNTRY_REGION (5) //REGION_5_BG_BAND
#define DEF_WIFI_COUNTRY_REGION_A_BAND  (7) //REGION_7_A_BAND
#define DEF_WIFI_DBG_LEVEL      (1) //RT_DEBUG_ERROR
// end of wifi profile

// start of system config
#define DEF_STA_IP_ADDR         "192.168.0.28"
#define DEF_STA_IP_NETMASK      "255.255.255.0"
#define DEF_STA_IP_GATEWAY      "192.168.0.1"

#define DEF_AP_IP_ADDR          "192.168.1.12"
#define DEF_AP_IP_NETMASK       "255.255.255.0"
#define DEF_AP_IP_GATEWAY       "192.168.1.254"
// end of system config

// start of wpa_supplicant config
#define DEF_SUPP_KEY_MGMT       (0x100) // 0x100: WPA_KEY_MGMT_NONE, 0x10: WPA_KEY_MGMT_PSK
#define DEF_SUPP_SSID           "DontConnectMe2G"
#define DEF_SUPP_PASSPHRASE     "12345678"
// end of wpa_supplicant config

#ifdef __cplusplus
}
#endif

#endif // #ifndef __DEFAULT_CONFIG_H__

