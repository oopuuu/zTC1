#include "http_server/web_log.h"

#include "main.h"
#include "user_gpio.h"
#include "mqtt_server/user_mqtt_client.h"

mico_gpio_t relay[Relay_NUM] = { Relay_0, Relay_1, Relay_2, Relay_3, Relay_4, Relay_5 };
char socket_status[32] = { 0 };

void UserLedSet(char x)
{
    if (x == -1)
        MicoGpioOutputTrigger(Led);
    else if (x)
        MicoGpioOutputHigh(Led);
    else
        MicoGpioOutputLow(Led);
}

bool RelayOut(void)
{
    int i;
    for (i = 0; i < SOCKET_NUM; i++)
    {
        if (user_config->socket_status[i] != 0)
        {
            return true;
        }
    }
    return false;
}

char* GetSocketStatus()
{
    sprintf(socket_status, "%d,%d,%d,%d,%d,%d",
        user_config->socket_status[0],
        user_config->socket_status[1],
        user_config->socket_status[2],
        user_config->socket_status[3],
        user_config->socket_status[4],
        user_config->socket_status[5]);
    return socket_status;
}

void SetSocketStatus(char* socket_status)
{
    sscanf(socket_status, "%d,%d,%d,%d,%d,%d,",
        (int*)&user_config->socket_status[0],
        (int*)&user_config->socket_status[1],
        (int*)&user_config->socket_status[2],
        (int*)&user_config->socket_status[3],
        (int*)&user_config->socket_status[4],
        (int*)&user_config->socket_status[5]);
    int i = 0;
    for (i = 0; i < SOCKET_NUM; i++)
    {
        UserRelaySet(i, user_config->socket_status[i]);
        UserMqttSendSocketState(i);
    }
    UserMqttSendTotalSocketState();
    mico_system_context_update(sys_config);
}

/*UserRelaySet
 * 设置继电器开关
 *  i:编号 0-5
 * on:开关 0:关 1:开
 */
void UserRelaySet(unsigned char i, unsigned char on)
{
    if (i < 0 || i >= SOCKET_NUM) return;

    if (on == Relay_ON)
    {
        MicoGpioOutputHigh(relay[i]);
    }
    else
    {
        MicoGpioOutputLow(relay[i]);
    }

    user_config->socket_status[i] = on;

    if (RelayOut()&&user_config->power_led_enabled)
    {
        UserLedSet(1);
    }
    else
    {
        UserLedSet(0);
    }
}

/*
 * 设置所有继电器开关
 * y: 0:全部关 1:全部开
 *
 */
void UserRelaySetAll(char y)
{
    int i;
    for (i = 0; i < SOCKET_NUM; i++)
        UserRelaySet(i, y);
}

static void KeyLong5sPress(void)
{
    key_log("WARNGIN: wifi ap started!");
    sys_config->micoSystemConfig.ssid[0] = 0;
    mico_system_context_update(mico_system_context_get());
}

static void KeyLong10sPress(void)
{
    key_log("WARNGIN: user params restored!");
    appRestoreDefault_callback1(user_config, sizeof(user_config_t));
    sys_config->micoSystemConfig.ssid[0] = 0;
    mico_system_context_update(mico_system_context_get());
}
static void KeyShortPress(void)
{
    char i;

    if (RelayOut())
    {
        UserRelaySetAll(0);
    }
    else
    {
        UserRelaySetAll(1);
    }

    for (i = 0; i < SOCKET_NUM; i++)
    {
        UserMqttSendSocketState(i);
    }
    UserMqttSendTotalSocketState();
}
mico_timer_t user_key_timer;
uint16_t key_time = 0;
#define BUTTON_LONG_PRESS_TIME    10     //100ms*10=1s

static void KeyTimeoutHandler(void* arg)
{
    static char key_trigger, key_continue;
    //按键扫描程序
    char tmp = ~(0xfe | MicoGpioInputGet(Button));
    key_trigger = tmp & (tmp ^ key_continue);
    key_continue = tmp;
    if (key_trigger != 0) key_time = 0; //新按键按下时,重新开始按键计时
    if (key_continue != 0)
    {
        //any button pressed
        key_time++;
        if (key_time <= BUTTON_LONG_PRESS_TIME)
        {
            key_log("button long pressed:%d",key_time);

            if (key_time == 50)
            {
                KeyLong5sPress();
            }
            else if (key_time == 100)
            {
                KeyLong10sPress();
            }
            else if (key_time == 102)
            {
                UserLedSet(1);
            }
            else if (key_time == 103)
            {
                UserLedSet(0);
                key_time = 101;
            }
        }
    }
    else
    {
        //button released
        if (key_time < BUTTON_LONG_PRESS_TIME)
        {   //100ms*10=1s 大于1s为长按
            key_time = 0;
            key_log("button short pressed:%d",key_time);
            KeyShortPress();
        }
        else if (key_time > 100)
        {
            MicoSystemReboot();
        }
        mico_rtos_stop_timer(&user_key_timer);
    }
}

static void KeyFallingIrqHandler(void* arg)
{
    mico_rtos_start_timer(&user_key_timer);
}

void KeyInit(void)
{
    MicoGpioInitialize(Button, INPUT_PULL_UP);
    mico_rtos_init_timer(&user_key_timer, 100, KeyTimeoutHandler, NULL);

    MicoGpioEnableIRQ(Button, IRQ_TRIGGER_FALLING_EDGE, KeyFallingIrqHandler, NULL);

}

