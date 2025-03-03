#include "http_server/web_log.h"

#include "mico.h"
#include "ota_server/ota_server.h"
#include "main.h"
#include "mqtt_server/user_mqtt_client.h"

float ota_progress = -2;

static void OtaServerStatusHandler(OTA_STATE_E state, float progress)
{
    char str[64] = { 0 };
    switch (state)
    {
        case OTA_LOADING:
            ota_progress = progress;
            ota_log("ota server is loading, progress %.2f%%", progress);
            if (((int) progress)%10 == 1)
                sprintf(str, "{\"mac\":\"%s\",\"ota_progress\":%d}", str_mac,((int) progress));
            break;
        case OTA_SUCCE:
            ota_progress = 100;
            ota_log("ota server daemons success");
            sprintf(str, "{\"mac\":\"%s\",\"ota_progress\":100}", str_mac);
            break;
        case OTA_FAIL:
            ota_log("ota server daemons failed");
            sprintf(str, "{\"mac\":\"%s\",\"ota_progress\":-1}", str_mac);
            break;
        default:
            break;
    }
}

void UserOtaStart(char *url, char *md5)
{
    ota_progress = 0;
    ota_log("ready to ota:%s",url);
    OtaServerStart(url, md5, OtaServerStatusHandler);
}

