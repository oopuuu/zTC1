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

#ifndef __CONNSYS_PROFILE_H__
#define __CONNSYS_PROFILE_H__

#include <stdint.h>

#pragma pack(push, 1)

#define WIFI_PROFILE_PDA_HEADER_LEN     (12)

// copy from N9 wifi_uni_mac_7636/rom/include/iot/rt_bora.h
#define MAX_LEN_OF_SSID                 (32)

typedef struct {
    unsigned char first_channel;
    unsigned char num_of_ch;
    unsigned char channel_prop; //0: Active, 1: Passive
    unsigned char reserve; // 4-align and could be used to extend channel_prop
} ch_desc_t;

typedef struct {
    unsigned char num_bg_band_triplet;
    unsigned char num_a_band_triplet;
    ch_desc_t triplet[]; // BGBand Triplet followed by Aband Triplet
} ch_list_t;

typedef struct {
    uint8_t opmode;
    uint8_t country_region;
    uint8_t country_region_a_band;
    uint8_t country_code[4];
    uint8_t radio_off;
    uint8_t dbg_level;
    uint16_t rts_threshold;
    uint16_t frag_threshold;

    uint8_t sta_local_admin_mac;
    uint8_t sta_ip_addr[4];
    uint8_t sta_mac_addr[6];
    uint8_t sta_ssid[32];
    uint8_t sta_ssid_len;
    uint8_t sta_bss_type;
    uint8_t sta_channel;
    uint8_t sta_bw;
    uint8_t sta_wireless_mode;
    uint8_t sta_ba_decline;
    uint8_t sta_auto_ba;
    uint8_t sta_ht_mcs;
    uint8_t sta_ht_ba_win_size;
    uint8_t sta_ht_gi;
    uint8_t sta_ht_protect;
    uint8_t sta_ht_ext_ch;
    uint8_t sta_wmm_capable;
    uint8_t sta_listen_interval;
    uint8_t sta_auth_mode;
    uint8_t sta_encryp_type;
    uint8_t sta_wpa_psk[64];
    uint8_t sta_wpa_psk_len;
    uint8_t sta_pmk[32];
    uint8_t sta_pair_cipher;
    uint8_t sta_group_cipher;
    uint8_t sta_default_key_id;
    uint8_t sta_ps_mode;
    uint8_t sta_keep_alive_period;

    uint8_t ap_local_admin_mac;
    uint8_t ap_ip_addr[4];
    uint8_t ap_mac_addr[6];
    uint8_t ap_ssid[32];
    uint8_t ap_ssid_len;
    uint8_t ap_channel;
    uint8_t ap_bw;
    uint8_t ap_wireless_mode;
    uint8_t ap_auto_ba;
    uint8_t ap_ht_mcs;
    uint8_t ap_ht_ba_win_size;
    uint8_t ap_ht_gi;
    uint8_t ap_ht_protect;
    uint8_t ap_ht_ext_ch;
    uint8_t ap_wmm_capable;
    uint8_t ap_dtim_period;
    uint8_t ap_hide_ssid;
    uint8_t ap_auto_channel_select;
    uint8_t ap_auth_mode;
    uint8_t ap_encryp_type;
    uint8_t ap_wpa_psk[64];
    uint8_t ap_wpa_psk_len;
    uint8_t ap_pmk[32];
    uint8_t ap_pair_cipher;
    uint8_t ap_group_cipher;
    uint8_t ap_default_key_id;

    // "scan channel table" and "regulatory table"
    unsigned char bg_band_entry_num;
    ch_desc_t bg_band_triple[10];
    unsigned char a_band_entry_num;
    ch_desc_t a_band_triple[10];
    uint8_t ap_beacon_disable;
    uint8_t forwarding_zero_copy;

    /* These are for MBSS support, but not exist trunk (it's customer feature), however,
            we have to add them here due to N9 FW has them (only one version of N9 FW)
         */
    // TODO: How to solve it in the future...Michael
    uint8_t mbss_enable;
    uint8_t mbss_ssid1[32];
    uint8_t mbss_ssid_len1;
    uint8_t mbss_ssid2[32];
    uint8_t mbss_ssid_len2;

    uint8_t config_free_ready;
    uint8_t config_free_enable;
    uint8_t sta_fast_link;
} sys_cfg_t;

#pragma pack(pop)

#endif // #ifndef __CONNSYS_PROFILE_H__

