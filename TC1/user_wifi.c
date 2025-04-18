#include "user_wifi.h"

#include "main.h"
#include "mico_socket.h"
#include "user_gpio.h"
#include "http_server/web_log.h"
#include "mqtt_server/user_mqtt_client.h"

char wifi_status = WIFI_STATE_NOCONNECT;

mico_timer_t wifi_led_timer;
IpStatus ip_status = { 0, ZZ_AP_LOCAL_IP, ZZ_AP_LOCAL_IP, ZZ_AP_NET_MASK };

//wifi已连接获取到IP地址回调
static void WifiGetIpCallback(IPStatusTypedef *pnet, void * arg)
{
    strcpy(ip_status.ip, pnet->ip);
    strcpy(ip_status.gateway, pnet->gate);
    strcpy(ip_status.mask, pnet->mask);

    wifi_log("got IP:%s", pnet->ip);
    wifi_status = WIFI_STATE_CONNECTED;
    //UserFunctionCmdReceived(1,"{\"cmd\":\"device report\"}");
}

//wifi连接状态改变回调
static void WifiStatusCallback(WiFiEvent status, void* arg)
{
    if (status == NOTIFY_STATION_UP) //wifi连接成功
    {
        //user_config->last_wifi_status = status;
        sys_config->micoSystemConfig.reserved = status;
        mico_system_context_update(sys_config);

        OSStatus status = micoWlanSuspendSoftAP(); //关闭AP
        if (status != kNoErr)
        {
            wifi_log("close ap error[%d]", status);
        }

        ip_status.mode = 1;
        //wifi_status = WIFI_STATE_CONNECTED;
    }
    else if (status == NOTIFY_STATION_DOWN) //wifi断开
    {
        //user_config->last_wifi_status = status;
        sys_config->micoSystemConfig.reserved = status;
        mico_system_context_update(sys_config);

        ApInit(false); //打开AP

        wifi_status = WIFI_STATE_NOCONNECT;
        if (!mico_rtos_is_timer_running(&wifi_led_timer))
        {
            mico_rtos_start_timer(&wifi_led_timer);
        }
    }
    else if (status == NOTIFY_AP_UP)
    {
        ip_status.mode = 0;
    }
}

bool scaned = false;
char* wifi_ret = NULL;
//wifi扫描结果回调
void WifiScanCallback(ScanResult_adv* scan_ret, void* arg)
{
    int count = (int)scan_ret->ApNum;
    wifi_log("wifi_scan_callback ApNum[%d] ApList[0](%s)", count, scan_ret->ApList[0].ssid);

    int i = 0;
    wifi_ret = malloc(sizeof(char)*count * (32 + 2) + 50);
    char* ssids = malloc(sizeof(char)*count * 32);
    char* secs = malloc(sizeof(char)*count * 2 + 1);
    char* tmp1 = ssids;
    char* tmp2 = secs;
    for (; i < count; i++)
    {
        /*
        ApInfo* ap = (ApInfo*)&scan_ret->ApList[i];
        uint8_t* mac = (uint8_t*)ap->bssid;
        wifi_log("wifi_scan_callback ssid[%16s] bssid[%02X-%02X-%02X-%02X-%02X-%02X] security[%d]",
            ap->ssid, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ap->security);
        */
        char* ssid = scan_ret->ApList[i].ssid;
        //排除隐藏的wifi和SSID带'或"的我wifi
        if (ssid[0] == 0 || strstr(ssid, "'") || strstr(ssid, "\"")) continue;
        sprintf(tmp1, "'%s',", ssid);
        tmp1 += (strlen(ssid) + 3);
        sprintf(tmp2, "%d,", scan_ret->ApList[i].security%10);
        tmp2 += 2;
    }
    *(--tmp1) = 0;
    *(--tmp2) = 0;

    sprintf(wifi_ret, WIFI_SCAN_RESULT_JSON, 1, ssids, secs);

    scaned = true;
    free(ssids);
    free(secs);
}


//100ms定时器回调
static void WifiLedTimerCallback(void* arg)
{
    static unsigned int num = 0;
    num++;

    switch (wifi_status)
    {
        case WIFI_STATE_FAIL:
            wifi_log("wifi connect fail");
            UserLedSet(0);
            mico_rtos_stop_timer(&wifi_led_timer);
            break;
        case WIFI_STATE_NOCONNECT:
            //wifi_connect_sys_config();
            break;
        case WIFI_STATE_CONNECTING:
            num = 0;
            UserLedSet(-1);
            break;
        case WIFI_STATE_CONNECTED:
            if (!(MQTT_SERVER[0] < 0x20 || MQTT_SERVER[0] > 0x7f || MQTT_SERVER_PORT < 1)){
                UserMqttInit();
            }
            UserLedSet(0);
            mico_rtos_stop_timer(&wifi_led_timer);
            if (RelayOut()&&user_config->power_led_enabled)
                UserLedSet(1);
            else
                UserLedSet(0);
            break;
    }
}

void WifiConnect(char* wifi_ssid, char* wifi_key)
{
    wifi_log("WifiConnect wifi_ssid[%s] wifi_key[******]", wifi_ssid);
    //wifi配置初始化
    network_InitTypeDef_st wNetConfig;

    memset(&wNetConfig, 0, sizeof(network_InitTypeDef_st));
    wNetConfig.wifi_mode = Station;
    snprintf(wNetConfig.wifi_ssid, 32, wifi_ssid);
    strcpy((char*)wNetConfig.wifi_key, wifi_key);
    wNetConfig.dhcpMode = DHCP_Client;
    wNetConfig.wifi_retry_interval = 6000;
    micoWlanStart(&wNetConfig);

    //保存wifi及密码到Flash
    strcpy(sys_config->micoSystemConfig.ssid, wifi_ssid);
    strcpy(sys_config->micoSystemConfig.user_key, wifi_key);
    sys_config->micoSystemConfig.user_keyLength = strlen(wifi_key);
    mico_system_context_update(sys_config);
    wifi_status = WIFI_STATE_NOCONNECT;
}

void WifiInit(void)
{
    //wifi状态下led闪烁定时器初始化
    mico_rtos_init_timer(&wifi_led_timer, 100, (void*)WifiLedTimerCallback, NULL);
    //wifi已连接获取到IP地址 回调
    mico_system_notify_register(mico_notify_DHCP_COMPLETED, (void*)WifiGetIpCallback, NULL);
    //wifi连接状态改变回调
    mico_system_notify_register(mico_notify_WIFI_STATUS_CHANGED, (void*)WifiStatusCallback, NULL);
    //wifi扫描结果回调
    mico_system_notify_register(mico_notify_WIFI_SCAN_ADV_COMPLETED, (void*)WifiScanCallback, NULL);

    //sntp_init();
    //启动定时器开始进行wifi连接
    if (!mico_rtos_is_timer_running(&wifi_led_timer)) mico_rtos_start_timer(&wifi_led_timer);
}

void ApConfig(char* name, char* key)
{
    strncpy(user_config->ap_name, name, 32);
    strncpy(user_config->ap_key, key, 32);
    wifi_log("ApConfig ap_name[%s] ap_key[******]", user_config->ap_name);
    micoWlanSuspendStation();
    ApInit(false);
    mico_system_context_update(sys_config);
}

void ApInit(bool use_defaul)
{
    if (use_defaul)
    {
        sprintf(user_config->ap_name, ZZ_AP_NAME, str_mac + 6);
        sprintf(user_config->ap_key, "%s", ZZ_AP_KEY);
        wifi_log("ApInit use_defaul[true] key[]");
    }

    network_InitTypeDef_st wNetConfig;
    memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));
    strcpy((char *)wNetConfig.wifi_ssid, user_config->ap_name);
    strcpy((char *)wNetConfig.wifi_key, user_config->ap_key);
    wNetConfig.wifi_mode = Soft_AP;
    wNetConfig.dhcpMode = DHCP_Server;
    wNetConfig.wifi_retry_interval = 100;
    strcpy((char *)wNetConfig.local_ip_addr, ZZ_AP_LOCAL_IP);
    strcpy((char *)wNetConfig.net_mask, ZZ_AP_NET_MASK);
    strcpy((char *)wNetConfig.dnsServer_ip_addr, ZZ_AP_DNS_SERVER);
    micoWlanStart(&wNetConfig);

    wifi_log("ApInit ssid[%s] key[******]", wNetConfig.wifi_ssid);
}

