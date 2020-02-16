#ifndef __MAIN_H_
#define __MAIN_H_

#include "mico.h"
#include "micokit_ext.h"

#define VERSION "v1.0.3"

#define TYPE 1
#define TYPE_NAME "zTC1"

#define ZTC1_NAME "zTC1-%s"

#define USER_CONFIG_VERSION 3
#define SETTING_MQTT_STRING_LENGTH_MAX 32 //必须4字节对齐。

#define SOCKET_NAME_LENGTH   32
#define SOCKET_NUM           6  //插座数量

#define Led    MICO_GPIO_5
#define Button MICO_GPIO_23
#define POWER  MICO_GPIO_15

#define Relay_ON  1
#define Relay_OFF 0

#define Relay_0   MICO_GPIO_6
#define Relay_1   MICO_GPIO_8
#define Relay_2   MICO_GPIO_10
#define Relay_3   MICO_GPIO_7
#define Relay_4   MICO_GPIO_9
#define Relay_5   MICO_GPIO_18
#define Relay_NUM SOCKET_NUM

typedef struct
{
    char name[SOCKET_NAME_LENGTH];
    char on;    //记录当前开关
} user_socket_config_t;

//用户保存参数结构体
typedef struct
{
    char version;
    char mqtt_ip[SETTING_MQTT_STRING_LENGTH_MAX];        //mqtt service ip
    int mqtt_port;                                       //mqtt service port
    char mqtt_user[SETTING_MQTT_STRING_LENGTH_MAX];      //mqtt service user
    char mqtt_password[SETTING_MQTT_STRING_LENGTH_MAX];  //mqtt service user
    user_socket_config_t socket_configs[SOCKET_NUM];
    char user[maxNameLen];
    WiFiEvent last_wifi_status;
} user_config_t;

extern char rtc_init;
extern uint32_t total_time;
extern char str_mac[16];
extern uint32_t real_time_power;
extern system_config_t* sys_config;
extern user_config_t* user_config;
extern char socket_status[32];
extern mico_gpio_t Relay[Relay_NUM];

#endif
