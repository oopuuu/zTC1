#include "http_server/web_log.h"

#include "main.h"
#include "user_gpio.h"
#include "user_wifi.h"
#include "mqtt_server/user_mqtt_client.h"

mico_gpio_t relay[Relay_NUM] = {Relay_0, Relay_1, Relay_2, Relay_3, Relay_4, Relay_5};
char socket_status[32] = {0};
char short_click_config[32] = {0};

void UserLedSet(char x) {
    if (x == -1)
        MicoGpioOutputTrigger(Led);
    else if (x)
        MicoGpioOutputHigh(Led);
    else
        MicoGpioOutputLow(Led);
}

bool RelayOut(void) {
    int i;
    for (i = 0; i < SOCKET_NUM; i++) {
        if (user_config->socket_status[i] != 0) {
            return true;
        }
    }
    return false;
}

char *GetSocketStatus() {
    sprintf(socket_status, "%d,%d,%d,%d,%d,%d",
            user_config->socket_status[0],
            user_config->socket_status[1],
            user_config->socket_status[2],
            user_config->socket_status[3],
            user_config->socket_status[4],
            user_config->socket_status[5]);
    return socket_status;
}

char *GetShortClickConfig() {
    sprintf(short_click_config, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
            user_config->user[1],
            user_config->user[2],
            user_config->user[3],
            user_config->user[4],
            user_config->user[5],
            user_config->user[6],
            user_config->user[7],
            user_config->user[8],
            user_config->user[9],
            user_config->user[10]);
    return short_click_config;
}

void SetSocketStatus(char *socket_status) {
    sscanf(socket_status, "%d,%d,%d,%d,%d,%d,",
           (int *) &user_config->socket_status[0],
           (int *) &user_config->socket_status[1],
           (int *) &user_config->socket_status[2],
           (int *) &user_config->socket_status[3],
           (int *) &user_config->socket_status[4],
           (int *) &user_config->socket_status[5]);
    int i = 0;
    for (i = 0; i < SOCKET_NUM; i++) {
        UserRelaySet(i, user_config->socket_status[i]);
        UserMqttSendSocketState(i);
    }
    UserMqttSendTotalSocketState();
    mico_system_context_update(sys_config);
}

/*UserRelaySet
 * 设置继电器开关
 *  i:编号 0-5
 * on:开关 0:关 1:开 -1:切换
 */
void UserRelaySet(unsigned char i, char on) {
    if (i < 0 || i >= SOCKET_NUM) return;

    if (on == Relay_ON) {
        MicoGpioOutputHigh(relay[i]);
    } else if (on == Relay_OFF) {
        MicoGpioOutputLow(relay[i]);
    } else if (on == Relay_TOGGLE) {
        MicoGpioOutputTrigger(relay[i]);
    }

    user_config->socket_status[i] = on >= 0 ? on : (user_config->socket_status[i] == 0 ? 1 : 0);

    if (RelayOut() && user_config->power_led_enabled) {
        UserLedSet(1);
    } else {
        UserLedSet(0);
    }
}

/*
 * 设置所有继电器开关
 * y: 0:全部关 1:全部开
 *
 */
void UserRelaySetAll(char y) {
    int i;
    for (i = 0; i < SOCKET_NUM; i++)
        UserRelaySet(i, y);
}

static void KeyLong5sPress(void) {
    key_log("WARNGIN: wifi ap started!");
    micoWlanSuspendStation();
    ApInit(true);
}

static void KeyLong10sPress(void) {
    key_log("WARNGIN: user params restored!");
    mico_system_context_restore(sys_config);
//    appRestoreDefault_callback(user_config, sizeof(user_config_t));
//    sys_config->micoSystemConfig.ssid[0] = 0;
//    mico_system_context_update(mico_system_context_get());
}

static void KeyShortPress(int clickCnt) {
    key_log("WARNGIN:Power key quick clicked %d time%s",clickCnt,clickCnt>1?"s":"");
    if (clickCnt > 10)
        return;
    switch (user_config->user[clickCnt]) {
        case SWITCH_TOTAL_SOCKET:
            if (RelayOut()) {
                UserRelaySetAll(0);
            } else {
                UserRelaySetAll(1);
            }

            for (int i = 0; i < SOCKET_NUM; i++) {
                UserMqttSendSocketState(i);
            }
            UserMqttSendTotalSocketState();
            break;
        case SWITCH_SOCKET_1:
        case SWITCH_SOCKET_2:
        case SWITCH_SOCKET_3:
        case SWITCH_SOCKET_4:
        case SWITCH_SOCKET_5:
        case SWITCH_SOCKET_6:
            UserRelaySet(user_config->user[clickCnt] - 1, Relay_TOGGLE);
            UserMqttSendSocketState(user_config->user[clickCnt] - 1);
            UserMqttSendTotalSocketState();
            mico_system_context_update(sys_config);
            break;
        case SWITCH_LED_ENABLE:
            MQTT_LED_ENABLED = MQTT_LED_ENABLED == 0 ? 1 : 0;
            if (RelayOut() && MQTT_LED_ENABLED) {
                UserLedSet(1);
            } else {
                UserLedSet(0);
            }
            UserMqttSendLedState();
            mico_system_context_update(sys_config);
            break;
        default:
            break;
    }
}

mico_timer_t user_key_timer;
uint16_t key_time = 0;
#define BUTTON_LONG_PRESS_TIME    10     //100ms*10=1s

static void KeyTimeoutHandler(void *arg) {
    if (childLockEnabled)
        return;

    static char key_trigger, key_continue;
    static uint8_t click_count = 0;
    static bool waiting_click_end = false;
    static uint8_t click_timer = 0;  // 单位：100ms

    // 按键扫描
    char tmp = ~(0xfe | MicoGpioInputGet(Button));
    key_trigger = tmp & (tmp ^ key_continue);
    key_continue = tmp;

    if (key_trigger != 0) key_time = 0;

    if (key_continue != 0) {
        key_time++;
        if (key_time > BUTTON_LONG_PRESS_TIME) {
            if (key_time == 50) {
                KeyLong5sPress();
            } else if (key_time > 50 && key_time < 57) {
                switch (key_time) {
                    case 51:
                        UserLedSet(1);
                        break;
                    case 52:
                        UserLedSet(0);
                        break;
                    case 53:
                        UserLedSet(1);
                        break;
                    case 54:
                        UserLedSet(0);
                        break;
                    case 55:
                        UserLedSet(1);
                        break;
                    case 56:
                        UserLedSet(0);
                        break;
                }
            } else if (key_time == 57) {
                UserLedSet(RelayOut() && user_config->power_led_enabled);
            } else if (key_time == 100) {
                KeyLong10sPress();
            } else if (key_time == 102) {
                UserLedSet(1);
            } else if (key_time == 103) {
                UserLedSet(0);
                key_time = 101;
            }
        }
    } else {
        // 按键释放
        if (key_time < BUTTON_LONG_PRESS_TIME) {
            click_count++;
            waiting_click_end = true;
            click_timer = 0;
        } else if (key_time > 100) {
            MicoSystemReboot();
        }
        key_time = 0;  // 只重置，不要马上 stop timer
    }

// 多击判定处理
    if (waiting_click_end) {
        click_timer++;
        if (click_timer >= 5) {
            KeyShortPress(click_count);
            click_count = 0;
            waiting_click_end = false;
            click_timer = 0;
            // ✅ 此时再 stop timer（可选）
         mico_rtos_stop_timer(&user_key_timer);
        }
    }
}

static void KeyFallingIrqHandler(void *arg) {
    mico_rtos_start_timer(&user_key_timer);
}

void KeyInit(void) {
    MicoGpioInitialize(Button, INPUT_PULL_UP);
    mico_rtos_init_timer(&user_key_timer, 100, KeyTimeoutHandler, NULL);

    MicoGpioEnableIRQ(Button, IRQ_TRIGGER_FALLING_EDGE, KeyFallingIrqHandler, NULL);

}

