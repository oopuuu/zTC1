/**
 ******************************************************************************
 * @file    mqtt_client.c
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    16-Nov-2015
 * @brief   MiCO application demonstrate a MQTT client.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
 ******************************************************************************
 */
#include "http_server/web_log.h"

#include "main.h"
#include "mico.h"
#include "MQTTClient.h"
#include "user_gpio.h"
#include "user_power.h"
#include "user_mqtt_client.h"

typedef struct {
    char topic[MAX_MQTT_TOPIC_SIZE];
    char qos;
    char retained;

    char data[MAX_MQTT_DATA_SIZE];
    uint32_t datalen;
} mqtt_recv_msg_t, *p_mqtt_recv_msg_t, mqtt_send_msg_t, *p_mqtt_send_msg_t;

static void MqttClientThread(mico_thread_arg_t arg);

static void MessageArrived(MessageData *md);

static OSStatus
MqttMsgPublish(Client *c, const char *topic, char qos, char retained, const unsigned char *msg,
               uint32_t msg_len);

OSStatus UserRecvHandler(void *arg);

void ProcessHaCmd(char *cmd);

bool isconnect = false;
mico_queue_t mqtt_msg_send_queue = NULL;

Client c;  // mqtt client object
Network n;  // socket network for mqtt client

static mico_worker_thread_t mqtt_client_worker_thread; /* Worker thread to manage send/recv events */
//static mico_timed_event_t mqtt_client_send_event;

char topic_state[MAX_MQTT_TOPIC_SIZE];
char topic_set[MAX_MQTT_TOPIC_SIZE];

mico_timer_t timer_handle;
static char timer_status = 0;

void UserMqttTimerFunc(void *arg) {
    LinkStatusTypeDef LinkStatus;
    micoWlanGetLinkStatus(&LinkStatus);
    if (LinkStatus.is_connected != 1) {
        mico_stop_timer(&timer_handle);
        return;
    }
    if (mico_rtos_is_queue_empty(&mqtt_msg_send_queue)) {

        switch (timer_status) {
            case 0:
                UserMqttHassAutoLed();
                UserMqttHassAutoTotalSocket();
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                UserMqttHassAuto(timer_status);
                break;
            case 7:
                UserMqttHassAutoPower();
                break;
            default:
                mico_stop_timer(&timer_handle);
                break;
        }
        timer_status++;
    }
}

/* Application entrance */
OSStatus UserMqttInit(void) {
    OSStatus err = kNoErr;

    sprintf(topic_set, MQTT_CLIENT_SUB_TOPIC1);
    sprintf(topic_state, MQTT_CLIENT_PUB_TOPIC, str_mac);
    //TODO size:0x800
    int mqtt_thread_stack_size = 0x2000;
    uint32_t mqtt_lib_version = MQTTClientLibVersion();mqtt_log(
            "MQTT client version: [%ld.%ld.%ld]",
            0xFF & (mqtt_lib_version >> 16), 0xFF & (mqtt_lib_version >> 8),
            0xFF & mqtt_lib_version);

    /* create mqtt msg send queue */
    err = mico_rtos_init_queue(&mqtt_msg_send_queue, "mqtt_msg_send_queue",
                               sizeof(p_mqtt_send_msg_t),
                               MAX_MQTT_SEND_QUEUE_SIZE);
    require_noerr_action(err, exit, mqtt_log("ERROR: create mqtt msg send queue err=%d.", err));

    /* start mqtt client */
    err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "mqtt_client",
                                  (mico_thread_function_t) MqttClientThread,
                                  mqtt_thread_stack_size, 0);
    require_noerr_string(err, exit, "ERROR: Unable to start the mqtt client thread.");

    /* Create a worker thread for user handling MQTT data event  */
    err = mico_rtos_create_worker_thread(&mqtt_client_worker_thread, MICO_APPLICATION_PRIORITY,
                                         0x800, 5);
    require_noerr_string(err, exit, "ERROR: Unable to start the mqtt client worker thread.");

    exit:
    if (kNoErr != err)mqtt_log("ERROR2, app thread exit err: %d kNoErr[%d]", err, kNoErr);
    return err;
}

static OSStatus UserMqttClientRelease(Client *c, Network *n) {
    OSStatus err = kNoErr;

    if (c->isconnected) MQTTDisconnect(c);

    n->disconnect(n);  // close connection

    if (MQTT_SUCCESS != MQTTClientDeinit(c)) { mqtt_log("MQTTClientDeinit failed!");
        err = kDeletedErr;
    }
    return err;
}

// publish msg to mqtt server
static OSStatus MqttMsgPublish(Client *c, const char *topic, char qos, char retained,
                               const unsigned char *msg,
                               uint32_t msg_len) {
    OSStatus err = kUnknownErr;
    int ret = 0;
    MQTTMessage publishData = MQTTMessage_publishData_initializer;

    require(topic && msg_len && msg, exit);

    // upload data qos0
    publishData.qos = (enum QoS) qos;
    publishData.retained = retained;
    publishData.payload = (void *) msg;
    publishData.payloadlen = msg_len;

    ret = MQTTPublish(c, topic, &publishData);

    if (MQTT_SUCCESS == ret) {
        err = kNoErr;
    } else if (MQTT_SOCKET_ERR == ret) {
        err = kConnectionErr;
    } else {
        err = kUnknownErr;
    }

    exit:
    return err;
}

void registerMqttEvents(void) {
    mico_start_timer(&timer_handle);
}

void MqttClientThread(mico_thread_arg_t arg) {
    OSStatus err = kUnknownErr;

    int rc = -1;
    fd_set readfds;
    struct timeval t = {0, MQTT_YIELD_TMIE * 1000};

    ssl_opts ssl_settings;
    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;

    p_mqtt_send_msg_t p_send_msg = NULL;
    int msg_send_event_fd = -1;
    bool no_mqtt_msg_exchange = true;

    mqtt_log("MQTT client thread started...");

    memset(&c, 0, sizeof(c));
    memset(&n, 0, sizeof(n));

    /* create msg send queue event fd */
    msg_send_event_fd = mico_create_event_fd(mqtt_msg_send_queue);
    require_action(msg_send_event_fd >= 0, exit,
                   mqtt_log("ERROR: create msg send queue event fd failed!!!"));

    MQTT_start:

    isconnect = false;
    /* 1. create network connection */
    ssl_settings.ssl_enable = false;
    LinkStatusTypeDef LinkStatus;
    while (1) {
        isconnect = false;
        mico_rtos_thread_sleep(3);
        if (MQTT_SERVER[0] < 0x20 || MQTT_SERVER[0] > 0x7f || MQTT_SERVER_PORT < 1)
            continue;  //鏈厤缃甿qtt鏈嶅姟鍣ㄦ椂涓嶈繛鎺�

        micoWlanGetLinkStatus(&LinkStatus);
        if (LinkStatus.is_connected != 1) { mqtt_log(
                    "ERROR:WIFI not connect, waiting 3s for connecting and then connecting MQTT ");
            mico_rtos_thread_sleep(3);
            continue;
        }

        rc = NewNetwork(&n, MQTT_SERVER, MQTT_SERVER_PORT, ssl_settings);
        if (rc == MQTT_SUCCESS) break;

        //mqtt_log("ERROR: MQTT network connect err=%d, reconnect after 3s...", rc);
    }mqtt_log("MQTT network connect success!");

    /* 2. init mqtt client */
    //c.heartbeat_retry_max = 2;
    rc = MQTTClientInit(&c, &n, MQTT_CMD_TIMEOUT);
    require_noerr_string(rc, MQTT_reconnect, "ERROR: MQTT client init err.");

    mqtt_log("MQTT client init success!");

    /* 3. create mqtt client connection */
    connectData.willFlag = 0;
    connectData.MQTTVersion = 4;  // 3: 3.1, 4: v3.1.1
    connectData.clientID.cstring = str_mac;
    connectData.username.cstring = user_config->mqtt_user;
    connectData.password.cstring = user_config->mqtt_password;
    connectData.keepAliveInterval = MQTT_CLIENT_KEEPALIVE;
    connectData.cleansession = 1;

    rc = MQTTConnect(&c, &connectData);
    require_noerr_string(rc, MQTT_reconnect, "ERROR: MQTT client connect err.");

    mqtt_log("MQTT client connect success!");

    UserLedSet(RelayOut() && user_config->power_led_enabled);

    /* 4. mqtt client subscribe */
    rc = MQTTSubscribe(&c, topic_set, QOS0, MessageArrived);
    require_noerr_string(rc, MQTT_reconnect, "ERROR: MQTT client subscribe err.");mqtt_log(
            "MQTT client subscribe success! recv_topic=[%s].", topic_set);
    /*4.1 杩炴帴鎴愬姛鍚庡厛鏇存柊鍙戦�佷竴娆℃暟鎹�*/
    isconnect = true;

    int i = 0;
    for (; i < SOCKET_NUM; i++) {
        UserMqttSendSocketState(i);
    }

    UserMqttSendLedState();

    mico_init_timer(&timer_handle, 150, UserMqttTimerFunc, &arg);
    registerMqttEvents();
    /* 5. client loop for recv msg && keepalive */
    while (1) {
        isconnect = true;
        no_mqtt_msg_exchange = true;
        FD_ZERO(&readfds);
        FD_SET(c.ipstack->my_socket, &readfds);
        FD_SET(msg_send_event_fd, &readfds);
        select(msg_send_event_fd + 1, &readfds, NULL, NULL, &t);

        /* recv msg from server */
        if (FD_ISSET(c.ipstack->my_socket, &readfds)) {
            rc = MQTTYield(&c, (int) MQTT_YIELD_TMIE);
            require_noerr(rc, MQTT_reconnect);
            no_mqtt_msg_exchange = false;
        }

        /* recv msg from user worker thread to be sent to server */
        if (FD_ISSET(msg_send_event_fd, &readfds)) {
            while (mico_rtos_is_queue_empty(&mqtt_msg_send_queue) == false) {
                // get msg from send queue
                mico_rtos_pop_from_queue(&mqtt_msg_send_queue, &p_send_msg, 0);
                require_string(p_send_msg, exit, "Wrong data point");

                // send message to server
                err = MqttMsgPublish(&c, p_send_msg->topic, p_send_msg->qos, p_send_msg->retained,
                                     (const unsigned char *) p_send_msg->data,
                                     p_send_msg->datalen);

                require_noerr_string(err, MQTT_reconnect, "ERROR: MQTT publish data err");

                //mqtt_log("MQTT publish data success! send_topic=[%s], msg=[%ld].", p_send_msg->topic, p_send_msg->datalen);
                no_mqtt_msg_exchange = false;
                free(p_send_msg);
                p_send_msg = NULL;
            }
        }

        /* if no msg exchange, we need to check ping msg to keep alive. */
        if (no_mqtt_msg_exchange) {
            rc = keepalive(&c);
            require_noerr_string(rc, MQTT_reconnect, "ERROR: keepalive err");
        }
    }

    MQTT_reconnect:

mqtt_log("Disconnect MQTT client, and reconnect after 5s, reason: mqtt_rc = %d, err = %d", rc, err);

    timer_status = 100;

    UserMqttClientRelease(&c, &n);
    isconnect = false;
    UserLedSet(-1);
    mico_rtos_thread_msleep(100);
    UserLedSet(-1);
    mico_rtos_thread_sleep(5);
    goto MQTT_start;

    exit:
    isconnect = false;mqtt_log("EXIT: MQTT client exit with err = %d.", err);
    UserMqttClientRelease(&c, &n);
    mico_rtos_delete_thread(NULL);
}

// callback, msg received from mqtt server
static void MessageArrived(MessageData *md) {
    OSStatus err = kUnknownErr;
    p_mqtt_recv_msg_t p_recv_msg = NULL;
    MQTTMessage *message = md->message;

    p_recv_msg = (p_mqtt_recv_msg_t) calloc(1, sizeof(mqtt_recv_msg_t));
    require_action(p_recv_msg, exit, err = kNoMemoryErr);

    p_recv_msg->datalen = message->payloadlen;
    p_recv_msg->qos = (char) (message->qos);
    p_recv_msg->retained = message->retained;
    strncpy(p_recv_msg->topic, md->topicName->lenstring.data, md->topicName->lenstring.len);
    memcpy(p_recv_msg->data, message->payload, message->payloadlen);

    mqtt_log("MessageArrived topic[%s] data[%s]", p_recv_msg->topic, p_recv_msg->data);
    err = mico_rtos_send_asynchronous_event(&mqtt_client_worker_thread, UserRecvHandler,
                                            p_recv_msg);
    require_noerr(err, exit);

    exit:
    if (err != kNoErr) { mqtt_log("ERROR: Recv data err = %d", err);
        if (p_recv_msg) free(p_recv_msg);
    }
    return;
}

/* Application process MQTT received data */
OSStatus UserRecvHandler(void *arg) {
    OSStatus err = kUnknownErr;
    p_mqtt_recv_msg_t p_recv_msg = arg;
    require(p_recv_msg, exit);

    mqtt_log("user get data success! from_topic=[%s], msg=[%ld].", p_recv_msg->topic,
             p_recv_msg->datalen);
    //UserFunctionCmdReceived(0, p_recv_msg->data);

    ProcessHaCmd(p_recv_msg->data);

    free(p_recv_msg);

    exit:
    return err;
}

void ProcessHaCmd(char *cmd) {
    mqtt_log("ProcessHaCmd[%s]", cmd);
    char mac[20] = {0};

    if (strcmp(cmd, "set socket") == ' ') {
        int i, on;
        sscanf(cmd, "set socket %s %d %d", mac, &i, &on);
        if (strcmp(mac, str_mac)) return;mqtt_log("set socket[%d] on[%d]", i, on);
        UserRelaySet(i, on);
        UserMqttSendSocketState(i);
        UserMqttSendTotalSocketState();
        mico_system_context_update(sys_config);
    } else if (strcmp(cmd, "set led") == ' ') {
        int on;
        sscanf(cmd, "set led %s %d", mac, &on);
        if (strcmp(mac, str_mac)) return;mqtt_log("set led on[%d]", on);
        user_config->power_led_enabled = on;
        if (RelayOut() && user_config->power_led_enabled) {
            UserLedSet(1);
        } else {
            UserLedSet(0);
        }
        UserMqttSendLedState();
        mico_system_context_update(sys_config);
    } else if (strcmp(cmd, "set total_socket") == ' ') {
        int on;
        sscanf(cmd, "set total_socket %s %d", mac, &on);
        if (strcmp(mac, str_mac)) return;mqtt_log("set total_socket on[%d]", on);
        UserRelaySetAll(on);
        int i = 0;
        for (i = 0; i < SOCKET_NUM; i++) {
            UserRelaySet(i, user_config->socket_status[i]);
            UserMqttSendSocketState(i);
        }
        UserMqttSendTotalSocketState();
    }
}

OSStatus UserMqttSendTopic(char *topic, char *arg, char retained) {
    OSStatus err = kUnknownErr;
    p_mqtt_send_msg_t p_send_msg = NULL;

//  mqtt_log("======App prepare to send ![%d]======", MicoGetMemoryInfo()->free_memory);

    /* Send queue is full, pop the oldest */
    if (mico_rtos_is_queue_full(&mqtt_msg_send_queue) == true) {
        mico_rtos_pop_from_queue(&mqtt_msg_send_queue, &p_send_msg, 0);
        free(p_send_msg);
        p_send_msg = NULL;
    }

    /* Push the latest data into send queue*/
    p_send_msg = (p_mqtt_send_msg_t) calloc(1, sizeof(mqtt_send_msg_t));
    require_action(p_send_msg, exit, err = kNoMemoryErr);

    p_send_msg->qos = 0;
    p_send_msg->retained = retained;
    p_send_msg->datalen = strlen(arg);
    memcpy(p_send_msg->data, arg, p_send_msg->datalen);
    strncpy(p_send_msg->topic, topic, MAX_MQTT_TOPIC_SIZE);

    err = mico_rtos_push_to_queue(&mqtt_msg_send_queue, &p_send_msg, 0);
    require_noerr(err, exit);

    //mqtt_log("Push user msg into send queue success!");

    exit:
    if (err != kNoErr && p_send_msg) free(p_send_msg);
    return err;
}

/* Application collect data and seng them to MQTT send queue */
OSStatus UserMqttSend(char *arg) {
    return UserMqttSendTopic(topic_state, arg, 0);
}

//鏇存柊ha寮�鍏崇姸鎬�
OSStatus UserMqttSendSocketState(char socket_id) {
    char *send_buf = malloc(64);
    char *topic_buf = malloc(64);
    OSStatus oss_status = kUnknownErr;
    if (send_buf != NULL && topic_buf != NULL) {
        sprintf(topic_buf, "homeassistant/switch/%s/socket_%d/state", str_mac, (int) socket_id);
        sprintf(send_buf, "set socket %s %d %d", str_mac, socket_id,
                (int) user_config->socket_status[(int) socket_id]);
        oss_status = UserMqttSendTopic(topic_buf, send_buf, 1);
    }
    if (send_buf) free(send_buf);
    if (topic_buf) free(topic_buf);

    return oss_status;
}

OSStatus UserMqttSendTotalSocketState(void) {
    char *send_buf = malloc(64);
    char *topic_buf = malloc(64);
    OSStatus oss_status = kUnknownErr;
    if (send_buf != NULL && topic_buf != NULL) {
        sprintf(topic_buf, "homeassistant/switch/%s/total_socket/state", str_mac);
        sprintf(send_buf, "set total_socket %s %d", str_mac, RelayOut() ? 1 : 0);
        oss_status = UserMqttSendTopic(topic_buf, send_buf, 1);
    }
    if (send_buf) free(send_buf);
    if (topic_buf) free(topic_buf);

    return oss_status;
}

OSStatus UserMqttSendLedState(void) {
    char *send_buf = malloc(64);
    char *topic_buf = malloc(64);
    OSStatus oss_status = kUnknownErr;
    if (send_buf != NULL && topic_buf != NULL) {
        sprintf(topic_buf, "homeassistant/switch/%s/led/state", str_mac);
        sprintf(send_buf, "set led %s %d", str_mac, (int) user_config->power_led_enabled);
        oss_status = UserMqttSendTopic(topic_buf, send_buf, 1);
    }
    if (send_buf) free(send_buf);
    if (topic_buf) free(topic_buf);

    return oss_status;
}

//hass mqtt鑷姩鍙戠幇鏁版嵁寮�鍏冲彂閫�
void UserMqttHassAuto(char socket_id) {
    socket_id--;
    char *send_buf = NULL;
    char *topic_buf = NULL;
    send_buf = (char *) malloc(600);
    topic_buf = (char *) malloc(64);
    if (send_buf != NULL && topic_buf != NULL) {
        sprintf(topic_buf, "homeassistant/switch/%s/socket_%d/config", str_mac, socket_id);
        sprintf(send_buf,
                "{\"name\":\"%s\","
                "\"uniq_id\":\"tc1_%s_s%d\","
                "\"object_id\":\"tc1_%s_s%d\","
                "\"stat_t\":\"homeassistant/switch/%s/socket_%d/state\","
                "\"cmd_t\":\"device/ztc1/set\","
                "\"pl_on\":\"set socket %s %d 1\","
                "\"pl_off\":\"set socket %s %d 0\","
                "\"device\":{"
                "\"identifiers\":[\"tc1_%s\"],"
                "\"name\":\"TC1\","
                "\"model\":\"TC1\","
                "\"manufacturer\":\"PHICOMM\"}}",
                user_config->socket_names[(int)socket_id], str_mac, socket_id,str_mac, socket_id, str_mac, socket_id,
                str_mac,
                socket_id, str_mac, socket_id, str_mac);
        UserMqttSendTopic(topic_buf, send_buf, 1);
    }
    if (send_buf)
        free(send_buf);
    if (topic_buf)
        free(topic_buf);
}

void UserMqttHassAutoLed(void) {
    char *send_buf = NULL;
    char *topic_buf = NULL;
    send_buf = (char *) malloc(600);
    topic_buf = (char *) malloc(64);
    if (send_buf != NULL && topic_buf != NULL) {
        sprintf(topic_buf, "homeassistant/switch/%s/led/config", str_mac);
        sprintf(send_buf,
                "{\"name\":\"LED指示灯\","
                "\"uniq_id\":\"tc1_%s_led\","
                "\"object_id\":\"tc1_%s_led\","
                "\"stat_t\":\"homeassistant/switch/%s/led/state\","
                "\"cmd_t\":\"device/ztc1/set\","
                "\"pl_on\":\"set led %s 1\","
                "\"pl_off\":\"set led %s 0\","
                "\"device\":{"
                "\"identifiers\":[\"tc1_%s\"],"
                "\"name\":\"TC1\","
                "\"model\":\"TC1\","
                "\"manufacturer\":\"PHICOMM\"}}",
                str_mac,str_mac,str_mac, str_mac, str_mac, str_mac);
        UserMqttSendTopic(topic_buf, send_buf, 1);
    }
    if (send_buf)
        free(send_buf);
    if (topic_buf)
        free(topic_buf);
}

void UserMqttHassAutoTotalSocket(void) {
    char *send_buf = NULL;
    char *topic_buf = NULL;
    send_buf = (char *) malloc(600);
    topic_buf = (char *) malloc(64);
    if (send_buf != NULL && topic_buf != NULL) {
        sprintf(topic_buf, "homeassistant/switch/%s/total_socket/config", str_mac);
        sprintf(send_buf,
                "{\"name\":\"总开关\","
                "\"uniq_id\":\"tc1_%s_total_socket\","
                "\"object_id\":\"tc1_%s_total_socket\","
                "\"stat_t\":\"homeassistant/switch/%s/total_socket/state\","
                "\"cmd_t\":\"device/ztc1/set\","
                "\"pl_on\":\"set total_socket %s 1\","
                "\"pl_off\":\"set total_socket %s 0\","
                "\"device\":{"
                "\"identifiers\":[\"tc1_%s\"],"
                "\"name\":\"TC1\","
                "\"model\":\"TC1\","
                "\"manufacturer\":\"PHICOMM\"}}",
                str_mac, str_mac, str_mac, str_mac, str_mac, str_mac);
        UserMqttSendTopic(topic_buf, send_buf, 1);
    }
    if (send_buf)
        free(send_buf);
    if (topic_buf)
        free(topic_buf);
}

//hass mqtt鑷姩鍙戠幇鏁版嵁鍔熺巼鍙戦��
void UserMqttHassAutoPower(void) {
    char *send_buf = NULL;
    char *topic_buf = NULL;
    send_buf = malloc(600);
    topic_buf = malloc(128);
    if (send_buf != NULL && topic_buf != NULL) {
        sprintf(topic_buf, "homeassistant/sensor/%s/power/config", str_mac);
        sprintf(send_buf,
                "{\"name\":\"功率\","
                "\"uniq_id\":\"tc1_%s_p\","
                "\"object_id\":\"tc1_%s_p\","
                "\"state_topic\":\"homeassistant/sensor/%s/power/state\","
                "\"unit_of_measurement\":\"W\","
                "\"icon\":\"mdi:gauge\","
                "\"value_template\":\"{{ value_json.power }}\",""\"device\":{"
                "\"identifiers\":[\"tc1_%s\"],"
                "\"name\":\"TC1\","
                "\"model\":\"TC1\","
                "\"manufacturer\":\"PHICOMM\"}}",
                str_mac,str_mac, str_mac, str_mac);
        UserMqttSendTopic(topic_buf, send_buf, 1);
        sprintf(topic_buf, "homeassistant/sensor/%s/powerConsumption/config", str_mac);
        sprintf(send_buf,
                "{\"name\":\"总耗电量\","
                "\"uniq_id\":\"tc1_%s_pc\","
                "\"object_id\":\"tc1_%s_pc\","
                "\"state_topic\":\"homeassistant/sensor/%s/powerConsumption/state\","
                "\"unit_of_measurement\":\"kWh\","
                "\"icon\":\"mdi:fence-electric\","
                "\"value_template\":\"{{ value_json.powerConsumption }}\",""\"device\":{"
                "\"identifiers\":[\"tc1_%s\"],"
                "\"name\":\"TC1\","
                "\"model\":\"TC1\","
                "\"manufacturer\":\"PHICOMM\"}}",
                str_mac, str_mac, str_mac, str_mac);
        UserMqttSendTopic(topic_buf, send_buf, 1);

        sprintf(topic_buf, "homeassistant/sensor/%s/powerConsumptionToday/config", str_mac);
        sprintf(send_buf,
                "{\"name\":\"今日耗电量\","
                "\"uniq_id\":\"tc1_%s_pc_today\","
                "\"object_id\":\"tc1_%s_pc_today\","
                "\"state_topic\":\"homeassistant/sensor/%s/powerConsumptionToday/state\","
                "\"unit_of_measurement\":\"kWh\","
                "\"icon\":\"mdi:fence-electric\","
                "\"value_template\":\"{{ value_json.powerConsumptionToday }}\",""\"device\":{"
                "\"identifiers\":[\"tc1_%s\"],"
                "\"name\":\"TC1\","
                "\"model\":\"TC1\","
                "\"manufacturer\":\"PHICOMM\"}}",
                str_mac,str_mac, str_mac, str_mac);
        UserMqttSendTopic(topic_buf, send_buf, 1);

        sprintf(topic_buf, "homeassistant/sensor/%s/powerConsumptionYesterday/config", str_mac);
        sprintf(send_buf,
                "{\"name\":\"昨日耗电量\","
                "\"uniq_id\":\"tc1_%s_pc_yesterday\","
                "\"object_id\":\"tc1_%s_pc_yesterday\","
                "\"state_topic\":\"homeassistant/sensor/%s/powerConsumptionYesterday/state\","
                "\"unit_of_measurement\":\"kWh\","
                "\"icon\":\"mdi:fence-electric\","
                "\"value_template\":\"{{ value_json.powerConsumptionYesterday }}\",""\"device\":{"
                "\"identifiers\":[\"tc1_%s\"],"
                "\"name\":\"TC1\","
                "\"model\":\"TC1\","
                "\"manufacturer\":\"PHICOMM\"}}",
                str_mac,str_mac, str_mac, str_mac);
        UserMqttSendTopic(topic_buf, send_buf, 1);
    }
    if (send_buf) free(send_buf);
    if (topic_buf) free(topic_buf);
}

char topic_buf[128] = {0};
char send_buf[128] = {0};

void UserMqttHassPower(void) {
    sprintf(topic_buf, "homeassistant/sensor/%s/power/state", str_mac);
    sprintf(send_buf, "{\"power\":\"%.3f\"}", real_time_power / 10);
    UserMqttSendTopic(topic_buf, send_buf, 0);

    sprintf(topic_buf, "homeassistant/sensor/%s/powerConsumption/state", str_mac);
    sprintf(send_buf, "{\"powerConsumption\":\"%.3f\"}", (17.1 * p_count) / 1000 / 36000);
    UserMqttSendTopic(topic_buf, send_buf, 0);

//    tc1_log("p_count %ld, p_count_1_day_ago %ld ,p_count_2_days_ago %ld, result %ld",p_count,user_config->p_count_1_day_ago,user_config->p_count_2_days_ago,((p_count-user_config->p_count_1_day_ago)<0?0:(p_count-user_config->p_count_1_day_ago)));
    sprintf(topic_buf, "homeassistant/sensor/%s/powerConsumptionToday/state", str_mac);
    sprintf(send_buf, "{\"powerConsumptionToday\":\"%.3f\"}", (17.1 * ((p_count -
                                                                        user_config->p_count_1_day_ago) <
                                                                       0 ? 0 : (p_count -
                                                                                user_config->p_count_1_day_ago))) /
                                                              1000 / 36000);
    UserMqttSendTopic(topic_buf, send_buf, 0);

    sprintf(topic_buf, "homeassistant/sensor/%s/powerConsumptionYesterday/state", str_mac);
    sprintf(send_buf, "{\"powerConsumptionYesterday\":\"%.3f\"}", (17.1 *
                                                                   ((user_config->p_count_1_day_ago -
                                                                     user_config->p_count_2_days_ago) <
                                                                    0 ? 0 : (
                                                                            user_config->p_count_1_day_ago -
                                                                            user_config->p_count_2_days_ago))) /
                                                                  1000 / 36000);
    UserMqttSendTopic(topic_buf, send_buf, 0);
}

bool UserMqttIsConnect() {
    return isconnect;
}
