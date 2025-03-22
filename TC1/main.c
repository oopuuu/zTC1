#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "unistd.h"
#include "TimeUtils.h"
#include "mico_system.h"

#include "user_gpio.h"
#include "user_wifi.h"
#include "time_server/user_rtc.h"
#include "user_power.h"
#include "mqtt_server/user_mqtt_client.h"
#include "http_server/app_httpd.h"
#include "timed_task/timed_task.h"

char rtc_init = 0; //sntp校时成功标志位
uint32_t total_time = 0;
char str_mac[16] = {0};
int last_check_day = 0;
int childLockEnabled = 0;

system_config_t *sys_config;
user_config_t *user_config;

mico_gpio_t Relay[Relay_NUM] = {Relay_0, Relay_1, Relay_2, Relay_3, Relay_4, Relay_5};

/* MICO system callback: Restore default configuration provided by application */
void appRestoreDefault_callback(void *const user_config_data, uint32_t size) {
    UNUSED_PARAMETER(size);

    mico_system_context_get()->micoSystemConfig.name[0] = 1; //在下次重启时使用默认名称
    mico_system_context_get()->micoSystemConfig.name[1] = 0;

    user_config_t *userConfigDefault = user_config_data;
    userConfigDefault->user[0] = 0;
    userConfigDefault->mqtt_ip[0] = 0;
    userConfigDefault->mqtt_port = 0;
    userConfigDefault->mqtt_user[0] = 0;
    userConfigDefault->mqtt_password[0] = 0;
    userConfigDefault->task_top = NULL;
    userConfigDefault->task_count = 0;
    userConfigDefault->mqtt_report_freq = 2;
    userConfigDefault->p_count_2_days_ago = 0;
    userConfigDefault->p_count_1_day_ago = 0;
    userConfigDefault->power_led_enabled = 1;
    userConfigDefault->version = USER_CONFIG_VERSION;
    set_key_map(userConfigDefault->user,1, SWITCH_ALL_SOCKETS, NO_FUNCTION);
    for (int i = 2; i < 32; i++) {
        int longFunc = NO_FUNCTION;
        //出厂设置，长按5秒开启配网模式，长按10秒恢复出厂设置
        if (i == 5) {
            longFunc = CONFIG_WIFI;
        } else if (i == 10) {
            longFunc = RESET_SYSTEM;
        }
        set_key_map(userConfigDefault->user,i, NO_FUNCTION, longFunc);
    }

    for (int i = 0; i < SOCKET_NUM; i++) {
        userConfigDefault->socket_status[i] = 1;
        snprintf(userConfigDefault->socket_names[i], SOCKET_NAME_LENGTH, "插座-%d", i + 1);
    }
    for (int i = 0; i < MAX_TASK_NUM; i++) {
        userConfigDefault->timed_tasks[i].on_use = false;
    }
    mico_system_context_update(sys_config);
}

void recordDailyPCount() {
    // 获取当前时间
    mico_utc_time_t utc_time;
    mico_time_get_utc_time(&utc_time);
    utc_time += 28800;
    struct tm *current_time = localtime((const time_t *) &utc_time);
    // 判断上次检查的时间与当前时间的日期是否不同
    if (last_check_day != 0) {
        // 如果日期发生变化（即跨天了），则进行记录
        if (current_time->tm_mday != last_check_day) { tc1_log(
                    "WARNGIN: pcount day changed! now day %d hour %d min %d ,lastCheck day %d",
                    current_time->tm_mday, current_time->tm_hour, current_time->tm_min,
                    last_check_day);

//            tc1_log("WARNGIN: pcount day changed! ");
            // 记录数据
            if (user_config->p_count_1_day_ago != 0) {
                user_config->p_count_2_days_ago = user_config->p_count_1_day_ago;
            }
            user_config->p_count_1_day_ago = p_count;

            // 更新系统配置
            mico_system_context_update(sys_config);

            tc1_log("WARNGIN: p_count record! p_count_1_day_ago:%d p_count_2_days_ago:%d",
                    user_config->p_count_1_day_ago, user_config->p_count_2_days_ago);
        } else {
//        	tc1_log("WARNGIN: pcount day not changed , waiting for next run! ");
        }
    } else { tc1_log("WARNGIN: now day %d hour %d min %d ,lastCheck day %d", current_time->tm_mday,
                     current_time->tm_hour, current_time->tm_min, last_check_day);
    }
    // 更新上次检查时间
    last_check_day = current_time->tm_mday;
}

void schedule_p_count_task(mico_thread_arg_t arg) {
    mico_thread_sleep(20);tc1_log("WARNGIN: p_count timer thread created!");
    while (1) {
        recordDailyPCount();
        mico_thread_sleep(60);
    }
}

void reportMqttPowerInfoThread() {
    while (1) {
        UserMqttHassPower();
        int freq = user_config->mqtt_report_freq;

        if (freq == 0) {
            freq = 2;
        }

        mico_thread_msleep(1000 * freq);
    }
}

int application_start(void) {
    int i;tc1_log("start version[%s]", VERSION);

    //char main_num=0;
    OSStatus err = kNoErr;

    // Create mico system context and read application's config data from flash
    sys_config = mico_system_context_init(sizeof(user_config_t));
    user_config = ((system_context_t *) sys_config)->user_config_data;
    require_action(user_config, exit, err = kNoMemoryErr);

    err = mico_system_init(sys_config);
    require_noerr(err, exit);

    uint8_t mac[8];
    mico_wlan_get_mac_address(mac);
    sprintf(str_mac, "%02X%02X%02X%02X%02X%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);tc1_log("str_mac[%s]", str_mac);

    bool open_ap = false;
    MicoGpioInitialize((mico_gpio_t) Button, INPUT_PULL_UP);
    if (!MicoGpioInputGet(Button)) {   //开机时按钮状态
        tc1_log("press ap_init");
        ApInit(true);
        open_ap = true;
    }

    MicoGpioInitialize((mico_gpio_t) Led, OUTPUT_PUSH_PULL);
    for (i = 0; i < Relay_NUM; i++) {
        MicoGpioInitialize(Relay[i], OUTPUT_PUSH_PULL);
        UserRelaySet(i, user_config->socket_status[i]);
    }
    MicoSysLed(0);

    childLockEnabled = (int) user_config->user[0];
    if (user_config->version != USER_CONFIG_VERSION) { tc1_log("WARNGIN: user params restored!");
        err = mico_system_context_restore(sys_config);
        require_noerr(err, exit);
    }

    if (sys_config->micoSystemConfig.name[0] == 1) {
        sprintf(sys_config->micoSystemConfig.name, ZTC1_NAME, str_mac + 8);
    }

    tc1_log("device name:%s",
            sys_config->micoSystemConfig.name);tc1_log(
            "mqtt_ip:%s", user_config->mqtt_ip);tc1_log("mqtt_port:%d",
                                                        user_config->mqtt_port);tc1_log(
            "mqtt_user:%s", user_config->mqtt_user);
    //tc1_log("mqtt_password:%s",user_config->mqtt_password);
    tc1_log("version:%d", user_config->version);

    WifiInit();
    if (!open_ap) {
        if (sys_config->micoSystemConfig.reserved != NOTIFY_STATION_UP) {
            ApInit(true);
        } else {
            WifiConnect(sys_config->micoSystemConfig.ssid,
                        sys_config->micoSystemConfig.user_key);
        }
    }
    KeyInit();
    err = UserMqttInit();
    require_noerr(err, exit);
    err = UserRtcInit();
    require_noerr(err, exit);
    PowerInit();
    AppHttpdStart(); // start http server thread

    UserLedSet(user_config->power_led_enabled);

    err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "p_count",
                                  (mico_thread_function_t) schedule_p_count_task,
                                  0x2000, 0);
    require_noerr_string(err, exit, "ERROR: Unable to start the p_count thread.");

    err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "mqtt_power_report",
                                  (mico_thread_function_t) reportMqttPowerInfoThread,
                                  0x2000, 0);
    require_noerr_string(err, exit, "ERROR: Unable to start the mqtt_power_report thread.");


    while (1) {
        time_t now = time(NULL);
        if (user_config->task_top && now >= user_config->task_top->prs_time) {
            ProcessTask();
        }

        mico_thread_msleep(1000);
    }

    exit:tc1_log("application_start ERROR!");
    return 0;
}

