#ifndef __USER_MQTT_CLIENT_H_
#define __USER_MQTT_CLIENT_H_


#include "mico.h"

#define MQTT_CLIENT_KEEPALIVE   30
#define MQTT_CLIENT_SUB_TOPIC1  "device/ztc1/set"
#define MQTT_CLIENT_PUB_TOPIC   "device/ztc1/%s/state"
#define MQTT_CMD_TIMEOUT        5000  // 5s
#define MQTT_YIELD_TMIE         5000  // 5s

#define MAX_MQTT_TOPIC_SIZE         (256)
#define MAX_MQTT_DATA_SIZE          (1024)
#define MAX_MQTT_SEND_QUEUE_SIZE    (10)

#define MQTT_SERVER      user_config->mqtt_ip
#define MQTT_SERVER_PORT user_config->mqtt_port
#define MQTT_SERVER_USR  user_config->mqtt_user
#define MQTT_SERVER_PWD  user_config->mqtt_password
#define MQTT_REPORT_FREQ  user_config->mqtt_report_freq
#define MQTT_LED_ENABLED  user_config->power_led_enabled

extern OSStatus UserMqttInit(void);

extern OSStatus UserMqttSend(char *arg);

extern bool UserMqttIsConnect(void);

extern OSStatus UserMqttSendSocketState(char socket_id);

extern OSStatus UserMqttSendLedState(void);

extern void UserMqttHassAuto(char socket_id);

extern void UserMqttHassPower(void);

extern void UserMqttHassAutoPower(void);

extern void UserMqttHassAutoLed(void);

#endif
