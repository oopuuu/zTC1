/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander/Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#ifndef __MQTT_CLIENT_H_
#define __MQTT_CLIENT_H_

#include "MQTTPacket.h"
#include "MQTTMiCO.h" //Platform specific implementation header file(e.g: MICO platform)

// lib version 
#define MQTT_MAIN_VERSION       (0x00)
#define MQTT_SUB_VERSION        (0x01)
#define MQTT_REV_VERSION        (0x08)
#define MQTT_LIB_VERSION        ((MQTT_MAIN_VERSION << 16) | (MQTT_SUB_VERSION << 8 ) | (MQTT_REV_VERSION))

#define MAX_PACKET_ID   (65535)
#define MAX_MESSAGE_HANDLERS    (5)
#define DEFAULT_READBUF_SIZE  (512)
#define DEFAULT_SENDBUF_SIZE  (512)
#define MAX_SIZE_CLIENT_ID  (23+1)
#define MAX_SIZE_USERNAME  (12+1)
#define MAX_SIZE_PASSWORD  (12+1)

// all failure return codes must be negative
enum returnCode { MQTT_SOCKET_ERR = -3, MQTT_BUFFER_OVERFLOW = -2, MQTT_FAILURE = -1, MQTT_SUCCESS = 0 };
enum QoS { QOS0, QOS1, QOS2 };

struct MQTTMessage
{
    int qos;
    char retained;
    char dup;
    unsigned short id;
    void *payload;
    size_t payloadlen;
};
typedef struct MQTTMessage MQTTMessage;

struct MessageData
{
    MQTTMessage* message;
    MQTTString* topicName;
};
typedef struct MessageData MessageData;

typedef void (*messageHandler)(MessageData*);

// mqtt client object
struct Client {
    unsigned int next_packetid;
    unsigned int command_timeout_ms;
    size_t buf_size, readbuf_size;
    unsigned char *buf;  
    unsigned char *readbuf; 
    unsigned int keepAliveInterval;
    char ping_outstanding;
    int isconnected;
    int heartbeat_retry_max;  // heartbeat max retry cnt
    int heartbeat_retry_cnt;  // heartbeat retry cnt

    struct MessageHandlers
    {
        const char* topicFilter;
        void (*fp) (MessageData*);
    } messageHandlers[MAX_MESSAGE_HANDLERS];      // Message handlers are indexed by subscription topic
    
    void (*defaultMessageHandler) (MessageData*);
    
    Network* ipstack;
    Timer ping_timer;
};
typedef struct Client Client;
#define DefaultClient {0, 0, 0, 0, NULL, NULL, 0, 0, 0}

#define MQTTMessage_publishData_initializer {QOS0, 0, 0, 1, "default_payload", strlen("default_payload") }

//-------------------------------- USER API ------------------------------------
int MQTTClientInit(Client*, Network*, unsigned int);
int MQTTClientDeinit(Client*);

int MQTTConnect (Client*, MQTTPacket_connectData*);
int MQTTPublish (Client*, const char*, MQTTMessage*);
int MQTTSubscribe (Client*, const char*, enum QoS, messageHandler);  // NOTE: subtopic memory must not be freed
int MQTTUnsubscribe (Client*, const char*);
int MQTTDisconnect (Client*);
int MQTTYield (Client*, int);
int keepalive(Client* c);

uint32_t MQTTClientLibVersion(void);
//------------------------------------------------------------------------------


#endif
