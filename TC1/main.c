#include "main.h"

#include "user_gpio.h"
#include "user_wifi.h"
#include "time_server/user_rtc.h"
#include "user_udp.h"
#include "user_power.h"
#include "mqtt_server/user_mqtt_client.h"
#include "mqtt_server/user_function.h"
#include "http_server/app_httpd.h"
#include "timed_task/timed_task.h"

#define os_log(format, ...) do { custom_log("TC1", format, ##__VA_ARGS__); web_log(format, ##__VA_ARGS__) } while(0)

char rtc_init = 0; //sntp校时成功标志位
uint32_t total_time = 0;
char strMac[16] = { 0 };
char str_mac[16] = { 0 };
uint32_t power = 0;

system_config_t* sys_config;
user_config_t* user_config;
char socket_status[32] = { 0 };

mico_gpio_t Relay[Relay_NUM] = { Relay_0, Relay_1, Relay_2, Relay_3, Relay_4, Relay_5 };

/* MICO system callback: Restore default configuration provided by application */
void appRestoreDefault_callback(void * const user_config_data, uint32_t size)
{
    UNUSED_PARAMETER(size);

    mico_system_context_get()->micoSystemConfig.name[0] = 1; //在下次重启时使用默认名称
    mico_system_context_get()->micoSystemConfig.name[1] = 0;

    user_config_t* userConfigDefault = user_config_data;
    userConfigDefault->user[0] = 0;
    userConfigDefault->mqtt_ip[0] = 0;
    userConfigDefault->mqtt_port = 0;
    userConfigDefault->mqtt_user[0] = 0;
    userConfigDefault->mqtt_password[0] = 0;
    userConfigDefault->version = USER_CONFIG_VERSION;

    int i, j;
    for (i = 0; i < SOCKET_NUM; i++)
    {
        userConfigDefault->socket[i].on = 1;
        //插座名称 插口1-6
        userConfigDefault->socket[i].name[0] = 0xe6;
        userConfigDefault->socket[i].name[1] = 0x8f;
        userConfigDefault->socket[i].name[2] = 0x92;
        userConfigDefault->socket[i].name[3] = 0xe5;
        userConfigDefault->socket[i].name[4] = 0x8f;
        userConfigDefault->socket[i].name[5] = 0xa3;
        userConfigDefault->socket[i].name[6] = i + '1';
        userConfigDefault->socket[i].name[7] = 0;
        //sprintf(userConfigDefault->socket[i].name, "插座%d", i);//编码异常

        for (j = 0; j < SOCKET_TIME_TASK_NUM; j++)
        {
            userConfigDefault->socket[i].task[j].hour = 0;
            userConfigDefault->socket[i].task[j].minute = 0;
            userConfigDefault->socket[i].task[j].repeat = 0x00;
            userConfigDefault->socket[i].task[j].on = 0;
            userConfigDefault->socket[i].task[j].action = 1;
        }
    }
    //mico_system_context_update(sys_config);
}

int application_start(void)
{
    int i;
    os_log("start version[%s]", VERSION);

    //char main_num=0;
    OSStatus err = kNoErr;

    // Create mico system context and read application's config data from flash
    sys_config = mico_system_context_init(sizeof(user_config_t));
    user_config = ((system_context_t*)sys_config)->user_config_data;
    require_action(user_config, exit, err = kNoMemoryErr);

    err = mico_system_init(sys_config);
    require_noerr(err, exit);

    uint8_t mac[8];
    mico_wlan_get_mac_address(mac);
    sprintf(str_mac, "%02X%02X%02X%02X%02X%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    os_log("str_mac[%s]", str_mac);

    bool open_ap = false;
    MicoGpioInitialize((mico_gpio_t)Button, INPUT_PULL_UP);
    if (!MicoGpioInputGet(Button))
    {   //开机时按钮状态
        os_log("press ap_init");
        ApInit();
        open_ap = true;
    }

    MicoGpioInitialize((mico_gpio_t) Led, OUTPUT_PUSH_PULL);
    for (i = 0; i < Relay_NUM; i++)
    {
        MicoGpioInitialize(Relay[i], OUTPUT_PUSH_PULL);
        UserRelaySet(i, user_config->socket[i].on);
    }
    MicoSysLed(0);

    if (user_config->version != USER_CONFIG_VERSION
        || user_config->socket[0].task[0].hour < 0
        || user_config->socket[0].task[0].hour > 23)
    {
        os_log("WARNGIN: user params restored!");
        err = mico_system_context_restore(sys_config);
        require_noerr(err, exit);
    }

    if (sys_config->micoSystemConfig.name[0] == 1)
    {
        strcpy(strMac, str_mac);
        sprintf(sys_config->micoSystemConfig.name, ZTC1_NAME, str_mac+8);
    }

    os_log("user:%s",user_config->user);
    os_log("device name:%s",sys_config->micoSystemConfig.name);
    os_log("mqtt_ip:%s",user_config->mqtt_ip);
    os_log("mqtt_port:%d",user_config->mqtt_port);
    os_log("mqtt_user:%s",user_config->mqtt_user);
    os_log("mqtt_password:%s",user_config->mqtt_password);
    os_log("version:%d",user_config->version);

    WifiInit();
    if (!open_ap)
    {
        if (sys_config->micoSystemConfig.reserved != NOTIFY_STATION_UP)
        {
            ApInit();
        }
        else
        {
            WifiConnect(sys_config->micoSystemConfig.ssid,
                sys_config->micoSystemConfig.user_key);
        }
    }
    user_udp_init();
    KeyInit();
    err = user_mqtt_init();
    require_noerr(err, exit);
    err = user_rtc_init();
    require_noerr(err, exit);
    PowerInit();

    //uint32_t power_last = 0xffffffff;
    AppHttpdStart(); // start http server thread
    char* power_buf = malloc(128);
    if (!power_buf) goto exit;

    uint32_t last_p_count = p_count;
    while (1)
    {
        //发送功率数据
        uint32_t power2 = 171 * (p_count - last_p_count) / 10;
        last_p_count = p_count;
        //SetPowerRecord(&power_record, power2);
        sprintf(power_buf, "{\"mac\":\"%s\",\"power\":\"%u.%u\",\"total_time\":%u}",
            strMac, (unsigned int)(power2 / 10), (unsigned int)(power2 % 10), (unsigned int)total_time);
        user_send(0, power_buf);
        user_mqtt_hass_power();

        time_t now = time(NULL);
        if (task_top && now >= task_top->prs_time)
        {
            os_log("process task time[%ld] socket_idx[%d] on[%d]",
                task_top->prs_time, task_top->socket_idx, task_top->on);
            UserRelaySet(task_top->socket_idx, task_top->on);
            DelFirstTask();
        }
        else
        {
            //os_log("timed task count[%u]", task_count);
        }
        mico_thread_msleep(1000);
    }

exit:
    os_log("application_start ERROR!");
    if (power_buf) free(power_buf);
    return 0;
}

