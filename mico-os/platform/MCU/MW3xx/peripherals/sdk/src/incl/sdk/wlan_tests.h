/*! \file wlan_tests.h
 *  \brief WLAN Connection Manager
 *
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __WLAN_TESTS_H__
#define __WLAN_TESTS_H__

void test_wlan_scan(int argc, char **argv);
void test_wlan_disconnect(int argc, char **argv);
int get_security(int argc, char **argv, enum wlan_security_type type,
			struct wlan_network_security *sec);
void cmd_wlan_get_rf_channel(int argc, char **argv);
void cmd_wlan_set_rf_channel(int argc, char **argv);
void cmd_wlan_get_tx_power(int argc, char **argv);
void cmd_wlan_deepsleep(int argc, char **argv);
void cmd_wlan_pscfg(int argc, char **argv);
#endif /* WLAN_TESTS_H */

