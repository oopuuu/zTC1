#include "http_server/web_log.h"

#include "main.h"
#include "user_gpio.h"
#include "user_wifi.h"
#include "mqtt_server/user_mqtt_client.h"

mico_gpio_t relay[Relay_NUM] = {Relay_0, Relay_1, Relay_2, Relay_3, Relay_4, Relay_5};
char socket_status[32] = {0};
char btn_click_config[500] = {0};

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

char* get_func_name(char func_code) {
    static char buffer[32];
    switch (func_code) {
        case SWITCH_ALL_SOCKETS:
            return "Toggle All Sockets";
        case SWITCH_SOCKET_1:
        case SWITCH_SOCKET_2:
        case SWITCH_SOCKET_3:
        case SWITCH_SOCKET_4:
        case SWITCH_SOCKET_5:
        case SWITCH_SOCKET_6:
            sprintf(buffer, "Toggle Socket %d %s", func_code - 1,
                    user_config->socket_names[func_code - 1]);
            return buffer;
        case SWITCH_LED_ENABLE:
            return "Toggle LED";
        case REBOOT_SYSTEM:
            return "Reboot";
        case CONFIG_WIFI:
            return "WiFi Config";
        case RESET_SYSTEM:
            return "Factory Reset";
        case SWITCH_CHILD_LOCK_ENABLE:
            return "Toggle ChildLick";
        case -1:
        case NO_FUNCTION:
            return "Unassigned";
        default:
            return "Unknown";
    }
}

/// 针对电源按钮的点击事件
/// \param index 判断短按（连击）时，代表连击次数，判断长按时代表长按秒数
/// \param short_func 功能码 在user_gpio.h中定义了
/// \param long_func 功能码 在user_gpio.h中定义了
void set_key_map(char user[],int index, char short_func, char long_func) {
    user[index] = ((long_func & 0x0F) << 4) | (short_func & 0x0F);
}

char get_short_func(char val) {
    char func = val & 0x0F;
    return (func == NO_FUNCTION) ? -1 : func;  // -1 表示未配置
}

char get_long_func(char val) {
    char func = (val >> 4) & 0x0F;
    return (func == NO_FUNCTION) ? -1 : func;  // -1 表示未配置
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

char *GetButtonClickConfig() {
    char temp[32];
    int len = 0;
    int max_len =sizeof(btn_click_config);
    len += snprintf(btn_click_config + len, max_len - len, "[");

    for (int i = 1; i <= 30; i++) {
        char short_func = get_short_func(user_config->user[i]);
        char long_func  = get_long_func(user_config->user[i]);
//    key_log("WARNGIN:KEY func %d %d %d", i,short_func,long_func);

        snprintf(temp, sizeof(temp), "{'%d':[%d,%d]}%s", i, short_func, long_func, (i != 30) ? "," : "");
        len += snprintf(btn_click_config + len, max_len - len, "%s", temp);

        if (len >= max_len - 1) break;
    }
    snprintf(btn_click_config + len, max_len - len, "]");

    return btn_click_config;
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
        if (user_config->socket_status[i] == Relay_OFF) {
            MicoGpioOutputHigh(relay[i]);
        } else {
            MicoGpioOutputLow(relay[i]);
        }
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
static void KeyEventHandler(int num, boolean longPress) {
    key_log("WARNGIN:Power key %s %d %s", !longPress ? "quick clicked" : "longPressed", num,
            num > 1 ? (longPress ? "seconds" : "times") : (longPress ? "second" : "time"));
    if (num > 30 || num <= 0)
        return;
    int function = !longPress ? get_short_func(user_config->user[num]) : get_long_func(
            user_config->user[num]);
            boolean showLog= childLockEnabled==0;
    switch (function) {
        case SWITCH_ALL_SOCKETS:
         if (childLockEnabled)
                break;
            if (RelayOut()) {
                UserRelaySetAll(0);
            } else {
                UserRelaySetAll(1);
            }
            mico_system_context_update(sys_config);
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
        if (childLockEnabled)
                        break;
            UserRelaySet(function - 1, Relay_TOGGLE);
            UserMqttSendSocketState(function - 1);
            UserMqttSendTotalSocketState();
            mico_system_context_update(sys_config);
            break;
        case SWITCH_LED_ENABLE:
        if (childLockEnabled)
                        break;
            MQTT_LED_ENABLED = MQTT_LED_ENABLED == 0 ? 1 : 0;
            if (RelayOut() && MQTT_LED_ENABLED) {
                UserLedSet(1);
            } else {
                UserLedSet(0);
            }
            UserMqttSendLedState();
            mico_system_context_update(sys_config);
            break;
        case SWITCH_CHILD_LOCK_ENABLE:
            showLog=true;
            user_config->user[0] = user_config->user[0] == 0 ? 1 : 0;
            childLockEnabled = user_config->user[0];
            mico_system_context_update(sys_config);
            UserMqttSendChildLockState();
            break;
        case REBOOT_SYSTEM:
        if (childLockEnabled)
                        break;
            MicoSystemReboot();
            break;
        case CONFIG_WIFI:
        if (childLockEnabled)
                        break;
            StartLedBlink(3);
            micoWlanSuspendStation();
            ApInit(true);
            break;
        case RESET_SYSTEM:
        if (childLockEnabled)
                        break;
            StartLedBlink(8);
            mico_system_context_restore(sys_config);
            mico_rtos_thread_sleep(1);
            MicoSystemReboot();
            break;
        default:
            break;
    }
    key_log("WARNGIN:%s",showLog? get_func_name(function):"child lock enabled,ignore key event !");
}

mico_timer_t user_key_timer;
// 全局静态变量声明
static uint8_t click_count = 0;
static mico_timer_t click_end_timer;
uint16_t key_time = 0;

static mico_timer_t led_blink_timer;
static bool timer_initialized = false;

static uint8_t total_blinks = 0;
static uint8_t blink_counter = 0;
static bool led_state = false;
#define BUTTON_LONG_PRESS_TIME    10     //100ms*10=1s
// 定时器回调
static void _led_blink_timer_handler(void *arg)
{
    if (blink_counter >= total_blinks) {
        UserLedSet(0);  // 闪烁完成，灭灯
        mico_stop_timer(&led_blink_timer);
        mico_deinit_timer(&led_blink_timer);
        timer_initialized = false;
        return;
    }

    led_state = !led_state;
    UserLedSet(led_state ? 1 : 0);
    blink_counter++;
}

// 安全重入的启动函数
void StartLedBlink(uint8_t times)
{
    if (times == 0) return;

    // 如果之前已启动，先停止并清理
    if (timer_initialized) {
        mico_stop_timer(&led_blink_timer);
        mico_deinit_timer(&led_blink_timer);
        timer_initialized = false;
    }

    total_blinks = times * 2;
    blink_counter = 0;
    led_state = false;

    mico_init_timer(&led_blink_timer, 100, _led_blink_timer_handler, NULL);
    mico_start_timer(&led_blink_timer);
    timer_initialized = true;
}
static void ClickEndTimeoutHandler(void *arg) {
    if (click_count <= 0) {
        click_count = 0;
        return;
    }
    KeyEventHandler(click_count,false);
    click_count = 0;
}

static void KeyTimeoutHandler(void *arg) {
    static char key_trigger, key_continue;
    static uint8_t key_time = 0;

    char tmp = ~(0xfe | MicoGpioInputGet(Button));
    key_trigger = tmp & (tmp ^ key_continue);
    key_continue = tmp;

    if (key_trigger != 0)
        key_time = 0;

    if (key_continue != 0) {
        key_time++;
//        if (key_time > BUTTON_LONG_PRESS_TIME) {
//            if (key_time == 50) {
//                KeyLong5sPress();
//            } else if (key_time > 50 && key_time < 57) {
//                switch (key_time) {
//                    case 51:
//                        UserLedSet(1);
//                        break;
//                    case 52:
//                        UserLedSet(0);
//                        break;
//                    case 53:
//                        UserLedSet(1);
//                        break;
//                    case 54:
//                        UserLedSet(0);
//                        break;
//                    case 55:
//                        UserLedSet(1);
//                        break;
//                    case 56:
//                        UserLedSet(0);
//                        break;
//                }
//            } else if (key_time == 57) {
//                UserLedSet(RelayOut() && user_config->power_led_enabled);
//            } else if (key_time == 100) {
//                KeyLong10sPress();
//            } else if (key_time == 102) {
//                UserLedSet(1);
//            } else if (key_time == 103) {
//                UserLedSet(0);
//                key_time = 101;
//            }
//        }
    } else {
        if (key_time < BUTTON_LONG_PRESS_TIME) {
            click_count++;

            // 重启 click_end_timer（300ms）
            mico_rtos_stop_timer(&click_end_timer);
            mico_rtos_start_timer(&click_end_timer);
        } else {
            KeyEventHandler(key_time/10,true);
        }

        mico_rtos_stop_timer(&user_key_timer);
    }
}

static void KeyFallingIrqHandler(void *arg) {
    mico_rtos_start_timer(&user_key_timer);
}

void KeyInit(void) {
    MicoGpioInitialize(Button, INPUT_PULL_UP);
    mico_rtos_init_timer(&user_key_timer, 100, KeyTimeoutHandler, NULL);
    mico_rtos_init_timer(&click_end_timer, 400, ClickEndTimeoutHandler, NULL);
    MicoGpioEnableIRQ(Button, IRQ_TRIGGER_FALLING_EDGE, KeyFallingIrqHandler, NULL);

}

