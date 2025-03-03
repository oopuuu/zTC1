/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include <json_parser.h>
#include <psm.h>
#include <psm-v2.h>
#include <wlan.h>
#include <wm_utils.h>
#include <wmtime.h>
#include <wm_net.h>
#include <diagnostics.h>

#define DIAG_STATS_LENGTH	64

#define J_NAME_DIAG		"diag"
#define VAR_REBOOT_REASON	"stats.reboot_reason"
#define VAR_CONN_STATS		"stats.conn"
#define VAR_DHCP_STATS		"stats.dhcp"
#define VAR_HTTP_CLIENT_STATS	"stats.httpc"
#define VAR_CLOUD_STATS		"stats.cloud"
#define VAR_CLOUD_CUMUL_STATS	"stats.cloud_cumul"
#define VAR_HTTPD_STATS		"stats.httpd"
#define VAR_NET_STATS		"stats.net"
#define VAR_IP_ADDR		"stats.ip_addr"
#define VAR_TIME		"stats.time"

#define J_NAME_REBOOT_REASON		"reboot_reason"
#define J_NAME_CONN_STATS		"conn"
#define J_NAME_DHCP_STATS		"dhcp"
#define J_NAME_HTTP_CLIENT_STATS	"httpc"
#define J_NAME_CLOUD_CUMUL_STATS	"cloud_cumul"
#define J_NAME_CLOUD_STATS		"cloud"
#define J_NAME_HTTPD_STATS		"httpd"
#define J_NAME_NET_STATS		"net"
#define J_NAME_IP_ADDR			"ip_addr"
#define J_NAME_TIME			"time"
#define J_NAME_PROV_TYPE		"prov_type"

#ifdef ENABLE_DBG
#define dbg(...)	wmprintf(__VA_ARGS__)
#else
#define dbg(...)
#endif

static int get_stats(psm_hnd_t psm_hnd, char *string, int len,
	wm_stat_type_t stat_type)
{
	struct wlan_network network;
	struct in_addr ip;
	char *psm_val;
	short rssi;

	if (string == NULL)
		return -1;

	switch (stat_type) {
	case CONN_STATS:
		wlan_get_current_rssi(&rssi);
		snprintf(string, len, "%u %u %u %u %u %u %d",
			 g_wm_stats.wm_lloss, g_wm_stats.wm_conn_att,
			 g_wm_stats.wm_conn_succ, g_wm_stats.wm_conn_fail,
			 g_wm_stats.wm_auth_fail, g_wm_stats.wm_nwnt_found,
			 rssi);
		break;
	case DHCP_STATS:
		snprintf(string, len, "%u %u %u %u %u", g_wm_stats.wm_dhcp_succ,
			 g_wm_stats.wm_dhcp_fail, g_wm_stats.wm_leas_succ,
			 g_wm_stats.wm_leas_fail, g_wm_stats.wm_addr_type);
		break;
	case HTTP_CLIENT_STATS:
		snprintf(string, len, "%u %u %u %u %u %u",
			 g_wm_stats.wm_ht_dns_fail, g_wm_stats.wm_ht_sock_fail,
			 g_wm_stats.wm_ht_conn_no_route,
			 g_wm_stats.wm_ht_conn_timeout,
			 g_wm_stats.wm_ht_conn_reset,
			 g_wm_stats.wm_ht_conn_other);
		break;
	case CLOUD_STATS:
		g_wm_stats.wm_cl_total = g_wm_stats.wm_cl_post_succ +
			g_wm_stats.wm_cl_post_fail;
		snprintf(string, len, "%lu %lu %lu", g_wm_stats.wm_cl_post_succ,
			g_wm_stats.wm_cl_post_fail, g_wm_stats.wm_cl_total);
		break;
	/* fixme: Currently, cloud cumulative stats are not reported in
	 * diagnostics */
	case CLOUD_CUMUL_STATS:
		psm_val = string;
		g_wm_stats.wm_cl_cum_total = 0;
		if (psm_get_variable_str(psm_hnd, VAR_CLOUD_CUMUL_STATS,
			psm_val, len) > 0) {
			g_wm_stats.wm_cl_cum_total = atoi(psm_val);
		}
		g_wm_stats.wm_cl_cum_total =
		    g_wm_stats.wm_cl_cum_total + g_wm_stats.wm_cl_total;
		snprintf(string, len, "%u", g_wm_stats.wm_cl_cum_total);
		break;
#ifdef CONFIG_ENABLE_HTTPD_STATS
	case HTTPD_STATS:
		snprintf(string, len, "%u %u %u %s-%s",
			 g_wm_stats.wm_hd_wsgi_call, g_wm_stats.wm_hd_file,
			 g_wm_stats.wm_hd_time,
			 g_wm_stats.wm_hd_useragent.product,
			 g_wm_stats.wm_hd_useragent.version);
		break;
#endif /* CONFIG_ENABLE_HTTPD_STATS */
	case NET_STATS:
		net_diag_stats(string, len);
		break;
	case IP_ADDR:
		if (wlan_get_current_network(&network)) {
			string[0] = 0;
			return -1;
		}
		ip.s_addr = network.ip.ipv4.address;
		snprintf(string, len, "%s", inet_ntoa(ip));
		break;
	case TIME:
		snprintf(string, len, "%ld", (long)wmtime_time_get_posix());
		break;
	case PROV_TYPE:
		snprintf(string, len, "%u", g_wm_stats.wm_prov_type);
		break;
		break;
	default:
		snprintf(string, len, "Invalid");
	}
	return 0;
}
int diagnostics_read_stats(struct json_str *jptr, psm_hnd_t psm_hnd)
{
	char temp[DIAG_STATS_LENGTH];
	int json_int;

	if (jptr == NULL)
		return -1;

	get_stats(psm_hnd, temp, sizeof(temp), CONN_STATS);
	json_set_val_str(jptr, J_NAME_CONN_STATS, temp);

	get_stats(psm_hnd, temp, sizeof(temp), DHCP_STATS);
	json_set_val_str(jptr, J_NAME_DHCP_STATS, temp);

	get_stats(psm_hnd, temp, sizeof(temp), HTTP_CLIENT_STATS);
	json_set_val_str(jptr, J_NAME_HTTP_CLIENT_STATS, temp);

	get_stats(psm_hnd, temp, sizeof(temp), CLOUD_STATS);
	json_set_val_str(jptr, J_NAME_CLOUD_STATS, temp);

	/* fixme: Removing cloud cumulative stats from the output */
#if 0
	get_stats(psm_hnd, temp, sizeof(temp), CLOUD_CUMUL_STATS);
	json_set_val_str(jptr, J_NAME_CLOUD_CUMUL_STATS, temp);
#endif

	get_stats(psm_hnd, temp, sizeof(temp), HTTPD_STATS);
	json_set_val_str(jptr, J_NAME_HTTPD_STATS, temp);

	get_stats(psm_hnd, temp, sizeof(temp), NET_STATS);
	json_set_val_str(jptr, J_NAME_NET_STATS, temp);

	get_stats(psm_hnd, temp, sizeof(temp), IP_ADDR);
	json_set_val_str(jptr, J_NAME_IP_ADDR, temp);

	get_stats(psm_hnd, temp, sizeof(temp), TIME);
	json_int = atoi(temp);
	json_set_val_int(jptr, J_NAME_TIME, json_int);

	get_stats(psm_hnd, temp, sizeof(temp), PROV_TYPE);
	json_int = atoi(temp);
	json_set_val_int(jptr, J_NAME_PROV_TYPE, json_int);

	return 0;
}

int diagnostics_write_stats(psm_hnd_t psm_hnd)
{
	char psm_val[DIAG_STATS_LENGTH];

	/* Query for cumulative statistics first, since this has to open psm
	 * itself.
	 */
	get_stats(psm_hnd, psm_val, sizeof(psm_val), CLOUD_CUMUL_STATS);

	psm_set_variable_str(psm_hnd, VAR_CLOUD_CUMUL_STATS, psm_val);

	snprintf(psm_val, sizeof(psm_val), "%d", g_wm_stats.reboot_reason);
	psm_set_variable_str(psm_hnd, VAR_REBOOT_REASON, psm_val);

	get_stats(psm_hnd, psm_val, sizeof(psm_val), CONN_STATS);
	psm_set_variable_str(psm_hnd, VAR_CONN_STATS, psm_val);

	get_stats(psm_hnd, psm_val, sizeof(psm_val), DHCP_STATS);
	psm_set_variable_str(psm_hnd, VAR_DHCP_STATS, psm_val);

	get_stats(psm_hnd, psm_val, sizeof(psm_val), HTTP_CLIENT_STATS);
	psm_set_variable_str(psm_hnd, VAR_HTTP_CLIENT_STATS, psm_val);

	get_stats(psm_hnd, psm_val, sizeof(psm_val), CLOUD_STATS);
	psm_set_variable_str(psm_hnd, VAR_CLOUD_STATS, psm_val);

	get_stats(psm_hnd, psm_val, sizeof(psm_val), HTTPD_STATS);
	psm_set_variable_str(psm_hnd, VAR_HTTPD_STATS, psm_val);

	get_stats(psm_hnd, psm_val, sizeof(psm_val), NET_STATS);
	psm_set_variable_str(psm_hnd, VAR_NET_STATS, psm_val);

	get_stats(psm_hnd, psm_val, sizeof(psm_val), IP_ADDR);
	psm_set_variable_str(psm_hnd, VAR_IP_ADDR, psm_val);

	get_stats(psm_hnd, psm_val, sizeof(psm_val), TIME);
	psm_set_variable_str(psm_hnd, VAR_TIME, psm_val);

	return 0;
}

int diagnostics_read_stats_psm(struct json_str *jptr, psm_hnd_t psm_hnd)
{
	int json_int;
	int ret;
	char temp[DIAG_STATS_LENGTH];

	ret = psm_get_variable_str(psm_hnd, VAR_REBOOT_REASON,
		temp, sizeof(temp));
	if (ret > 0) {
		json_int = atoi(temp);
		json_set_val_int(jptr, J_NAME_REBOOT_REASON, json_int);
	}

	ret = psm_get_variable_str(psm_hnd, VAR_CONN_STATS, temp, sizeof(temp));
	if (ret > 0)
		json_set_val_str(jptr, J_NAME_CONN_STATS, temp);

	ret = psm_get_variable_str(psm_hnd, VAR_DHCP_STATS, temp, sizeof(temp));
	if (ret > 0)
		json_set_val_str(jptr, J_NAME_DHCP_STATS, temp);

	ret = psm_get_variable_str(psm_hnd, VAR_HTTP_CLIENT_STATS,
		temp, sizeof(temp));
	if (ret > 0)
		json_set_val_str(jptr, J_NAME_HTTP_CLIENT_STATS, temp);

	ret = psm_get_variable_str(psm_hnd, VAR_CLOUD_STATS, temp,
		sizeof(temp));
	if (ret > 0)
		json_set_val_str(jptr, J_NAME_CLOUD_STATS, temp);

	/* fixme: Removing cloud cumulative stats from the output */
#if 0
	ret = psm_get_variable_str(psm_hnd, VAR_CLOUD_CUMUL_STATS,
		temp, sizeof(temp));
	if (ret > 0) {
		json_set_val_str(jptr, J_NAME_CLOUD_CUMUL_STATS, temp);
#endif

	ret = psm_get_variable_str(psm_hnd, VAR_HTTPD_STATS, temp,
		sizeof(temp));
	if (ret > 0)
		json_set_val_str(jptr, J_NAME_HTTPD_STATS, temp);

	ret = psm_get_variable_str(psm_hnd, VAR_NET_STATS, temp, sizeof(temp));
	if (ret > 0)
		json_set_val_str(jptr, J_NAME_NET_STATS, temp);

	ret = psm_get_variable_str(psm_hnd, VAR_IP_ADDR, temp, sizeof(temp));
	if (ret > 0)
		json_set_val_str(jptr, J_NAME_IP_ADDR, temp);

	ret = psm_get_variable_str(psm_hnd, VAR_TIME, temp, sizeof(temp));
	if (ret > 0) {
		json_int = atoi(temp);
		json_set_val_int(jptr, J_NAME_TIME, json_int);
	}

	return 0;
}

void diagnostics_set_reboot_reason(wm_reboot_reason_t reason)
{
	g_wm_stats.reboot_reason = reason;
}

void diagnostics_write_stats_with_reboot_reason(psm_hnd_t psm_hnd,
	wm_reboot_reason_t reason)
{
	diagnostics_set_reboot_reason(reason);
	diagnostics_write_stats(psm_hnd);
}

diagnostics_write_cb diag_write_cb;

int diagnostics_init()
{
	diag_write_cb = diagnostics_write_stats_with_reboot_reason;
	return 0;
}
