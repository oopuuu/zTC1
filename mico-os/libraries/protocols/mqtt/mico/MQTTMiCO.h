/**
******************************************************************************
* @file    MQTTMiCO.h
* @author  Eshen Wang
* @version V1.0.0
* @date    09-Oct-2015
* @brief   Network connection for mqtt client based on MiCO.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a 
copy 
*  of this software and associated documentation files (the "Software"), to 
deal
*  in the Software without restriction, including without limitation the 
rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is 
furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
******************************************************************************
*/

#ifndef __MQTT_MICO_H_
#define __MQTT_MICO_H_

#include "mico.h"

#define MQTTCLIENT_INFO "MICO_MQTT_CLIENT"
#define MICO_MQTT_CLIENT_SUPPORT_SSL

#ifdef MICO_MQTT_CLIENT_SUPPORT_SSL
#define MICO_MQTT_CLIENT_SSL_ENABLE        (0x0001)
#define MICO_MQTT_CLIENT_SSL_DEBUG_ENABLE  (0x0002)
#endif

// socket opts
#define MQTT_CLIENT_SOCKET_TIMEOUT              (5000)  // 5s
#define MQTT_CLIENT_SOCKET_TCP_KEEPIDLE         (10)  // tcp keepavlie idle time 10s
#define MQTT_CLIENT_SOCKET_TCP_KEEPINTVL        (10)  // tcp keepavlie interval time 10s
#define MQTT_CLIENT_SOCKET_TCP_KEEPCNT          (5)  // max retry

typedef struct Timer Timer;

struct Timer {
  unsigned long systick_period;
  unsigned long end_time;
  bool over_flow;
};

typedef struct Network Network;

struct Network
{
  int my_socket;
  int (*mqttread) (Network*, unsigned char*, int, int);
  int (*mqttwrite) (Network*, unsigned char*, int, int);
  void (*disconnect) (Network*);
  void *ssl;
  uint16_t ssl_flag;  // bit0: ssl_enable, bit1: ssl_debug_enable, bit2~4: ssl_version
};

typedef struct _ssl_opts_t {
  bool ssl_enable;
  bool ssl_debug_enable;
  int ssl_version;  // SSL_V3_MODE=1, TLS_V1_0_MODE=2, TLS_V1_1_MODE=3, TLS_V1_2_MODE=4
  int ca_str_len;  // CA string len
  char* ca_str;  // CA string
} ssl_opts;

char expired(Timer*);
void countdown_ms(Timer*, unsigned int);
void countdown(Timer*, unsigned int);
int left_ms(Timer*);
void InitTimer(Timer*);

int MICO_read(Network*, unsigned char*, int, int);
int MICO_write(Network*, unsigned char*, int, int);
void MICO_disconnect(Network*);

//-------------------------------- USER API ------------------------------------
int NewNetwork(Network* n, char* addr, int port, ssl_opts ssl_settings);

//------------------------------------------------------------------------------


#endif  // __MQTT_MICO_H_
