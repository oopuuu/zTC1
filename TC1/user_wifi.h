#ifndef __USER_WIFI_H_
#define __USER_WIFI_H_

#include "mico.h"
#include "mico_wlan.h"
#include "micokit_ext.h"

enum
{
   WIFI_STATE_FAIL,
   WIFI_STATE_NOCONNECT,
   WIFI_STATE_CONNECTING,
   WIFI_STATE_CONNECTED,
};

#define ZZ_AP_NAME       "TC1-AP-%s"
#define ZZ_AP_KEY        "12345678"
#define ZZ_AP_LOCAL_IP   "192.168.0.1"
#define ZZ_AP_DNS_SERVER "192.168.0.1"
#define ZZ_AP_NET_MASK   "255.255.255.0"

#define WIFI_SCAN_RESULT_JSON "{'success':%d,'ssids':[%s],'secs':[%s]}"
extern bool scaned;
extern char* wifi_ret;
extern char wifi_status;

typedef struct {
    int  mode;  //0:AP, 1:Station
    char ip[16];
    char gateway[16];
    char mask[16];
} IpStatus;

typedef struct {
    char ssid[32];
    char bssid[6];
    char channel;
    wlan_sec_type_t security;
    int16_t rssi;
} ApInfo;

extern IpStatus ip_status;

extern void WifiInit(void);
extern void ApInit(bool use_defaul);
extern void ApConfig(char* name, char* key);
extern void WifiConnect(char* wifi_ssid, char* wifi_key);

#endif
