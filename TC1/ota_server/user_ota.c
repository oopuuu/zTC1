#include "http_server/web_log.h"
#define os_log(format, ...) do { custom_log("OTA", format, ##__VA_ARGS__); web_log(format, ##__VA_ARGS__) } while(0)

#include "mico.h"
#include "ota_server/ota_server.h"
#include "main.h"
#include "user_udp.h"
#include "mqtt_server/user_mqtt_client.h"

float ota_progress = 0;

static void OtaServerStatusHandler(OTA_STATE_E state, float progress)
{
    char str[64] = { 0 };
    switch (state)
    {
        case OTA_LOADING:
            ota_progress = progress;
            os_log("ota server is loading, progress %.2f%%", progress);
            if (((int) progress)%10 == 1)
                sprintf(str, "{\"mac\":\"%s\",\"ota_progress\":%d}", str_mac,((int) progress));
            break;
        case OTA_SUCCE:
            ota_progress = 100;
            os_log("ota server daemons success");
            sprintf(str, "{\"mac\":\"%s\",\"ota_progress\":100}", str_mac);
            break;
        case OTA_FAIL:
            os_log("ota server daemons failed");
            sprintf(str, "{\"mac\":\"%s\",\"ota_progress\":-1}", str_mac);
            break;
        default:
            break;
    }
    if (str[0] > 0)
    {
        UserSend(true, str);
    }
}

void UserOtaStart(char *url, char *md5)
{
    ota_progress = 0;
    os_log("ready to ota:%s",url);
    OtaServerStart(url, md5, OtaServerStatusHandler);
}

