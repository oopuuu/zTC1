/**
******************************************************************************
* @file    MQTTMiCO.c
* @author  Eshen Wang
* @version V1.0.0
* @date    31-May-2016
* @brief   mqtt client demo based on MiCO.
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

#include "MQTTMiCO.h"
#include "mico.h"


#define mqtt_mico_log(M, ...) //custom_log("MQTT", M, ##__VA_ARGS__)
#define mqtt_mico_log_trace() //custom_log_trace("MQTT")

/*
enum {
	SSL_V3_MODE = 1,
	TLS_V1_0_MODE = 2,
	TLS_V1_1_MODE = 3,
	TLS_V1_2_MODE = 4,
};
Ĭ�ϵ�SSL��ʹ��SSLv3.0
*/
extern void set_ssl_client_version(int version);

// ssl debug functions
//int CyaSSL_Debugging_ON(void);
//typedef void (*CyaSSL_Logging_cb)(const int logLevel, const char *const logMessage);
//int CyaSSL_SetLoggingCb(CyaSSL_Logging_cb f);
//void print_ssl_msg(const int logLevel, const char *const logMessage)
//{
//  mqtt_mico_log("%s\n	", logMessage);
//}

char expired(Timer* timer)
{
  //long left = timer->end_time - mico_get_time();

  long left = 0;
  if (timer->over_flow) {
    left = 0xFFFFFFFF - mico_rtos_get_time() + timer->end_time;
  }
  else {
    left = timer->end_time - mico_rtos_get_time();
  }

  return (left < 0);
}


void countdown_ms(Timer* timer, unsigned int timeout)
{
  uint32_t current_time = mico_rtos_get_time();
  timer->end_time = current_time + timeout;
  if(timer->end_time < current_time) {
    timer->over_flow = true;
  }
}


void countdown(Timer* timer, unsigned int timeout)
{
  uint32_t current_time = mico_rtos_get_time();
  timer->end_time = current_time + (timeout * 1000);
  if(timer->end_time < current_time) {
    timer->over_flow = true;
  }
}


int left_ms(Timer* timer)
{
  //long left = timer->end_time - mico_get_time();

  long left = 0;
  if (timer->over_flow) {
    left = 0xFFFFFFFF - mico_rtos_get_time() + timer->end_time;
  }
  else {
    left = timer->end_time - mico_rtos_get_time();
  }

  return (left < 0) ? 0 : left;
}


void InitTimer(Timer* timer)
{
  timer->end_time = 0;
  timer->systick_period = 0;
  timer->over_flow = false;
}


int MICO_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  int rc = 0;
  int recvLen = 0;
  Timer readLenTimer;
  int socket_errno = 0;
  socklen_t socket_errno_len = 4;
  fd_set fdset;
//  struct timeval_t timeVal;

  //read left timer
  InitTimer(&readLenTimer);
  countdown_ms(&readLenTimer, timeout_ms);

  FD_ZERO(&fdset);
  FD_SET(n->my_socket, &fdset);

//  timeVal.tv_sec = 0;
//  timeVal.tv_usec = timeout_ms * 1000;

  //if(1 == select(n->my_socket + 1, &fdset, NULL, NULL, &timeVal)){
    // by wes, 20141021, must be non blocking read.
    do {
      mqtt_mico_log("recv len=%d.", len - recvLen);
#ifdef MICO_MQTT_CLIENT_SUPPORT_SSL
      if(n->ssl_flag & MICO_MQTT_CLIENT_SSL_ENABLE){
        mqtt_mico_log("ssl_recv len=%d.", len - recvLen);
        rc = ssl_recv(n->ssl, (char*)(buffer + recvLen), len - recvLen);
        mqtt_mico_log("ssl_recv got=%d.", rc);
      }
      else
#endif
      {
        rc = recv(n->my_socket, buffer + recvLen, len - recvLen, 0);
        mqtt_mico_log("recv got=%d.", rc);
      }
      if(rc <= 0){  // no data got, check error from socket_errno
        rc = getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &socket_errno, &socket_errno_len);
        if ((rc < 0) || ( 0 != socket_errno)){
          mqtt_mico_log("ssl_recv/recv errno=%d.", socket_errno);
          return -1;   //socket error
        }
      }
      else{
        recvLen += rc;
      }
    } while((recvLen < len) && (!expired(&readLenTimer))); // by wes, 20141021, must be non blocking read.
  //}

  return recvLen;
}


int MICO_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  struct timeval timeVal;
  fd_set fdset;
  int rc = 0;
  int readySock;

  int socket_errno = 0;
  socklen_t socket_errno_len = 4;

  //add timeout by wes 20141106
  Timer writeTimer;

  InitTimer(&writeTimer);
  countdown_ms(&writeTimer, timeout_ms);

  FD_ZERO(&fdset);
  FD_SET(n->my_socket, &fdset);

  timeVal.tv_sec = 0;
  timeVal.tv_usec = timeout_ms * 1000;

  do {
    readySock = select(n->my_socket + 1, NULL, &fdset, NULL, &timeVal);
    mqtt_mico_log("readySock=%d", readySock);
  } while((readySock != 1) && (!expired(&writeTimer)));  //add timeout by wes 20141106

#ifdef MICO_MQTT_CLIENT_SUPPORT_SSL
  if(n->ssl_flag & MICO_MQTT_CLIENT_SSL_ENABLE){
    mqtt_mico_log("ssl_send: [%d][%s]", len, buffer);
    rc = ssl_send(n->ssl, (char*)buffer, len);
  }
  else
#endif
  {
    rc = send(n->my_socket, buffer, len, 0);
  }

  mqtt_mico_log("send rc=%d.", rc);
  if(rc < 0){
    rc = getsockopt(n->my_socket, SOL_SOCKET, SO_ERROR, &socket_errno, &socket_errno_len);
    if ((rc < 0) || ( 0 != socket_errno)){
      mqtt_mico_log("ERROR: getsockopt rc=%d, socket_errno=%d", rc, socket_errno);
      rc = -1;
    }
    else{
      mqtt_mico_log("ERROR: just send error!");
    }
    rc = -2;  // return an error for current writing process
  }

  return rc;
}


void MICO_disconnect(Network* n)
{
#ifdef MICO_MQTT_CLIENT_SUPPORT_SSL
  if(n->ssl_flag & MICO_MQTT_CLIENT_SSL_ENABLE){
    if(NULL !=  n->ssl){
      ssl_close(n->ssl);
      n->ssl = NULL;
      mqtt_mico_log("ssl session closed.");
    }
  }
#endif
  if(n->my_socket >= 0){
    close(n->my_socket);
    n->my_socket = -1;
    mqtt_mico_log("mqtt socket closed!!!");
  }
}

static OSStatus usergethostbyname( const char * domain, uint8_t * addr, uint8_t addrLen )
{
    struct hostent* host = NULL;
    struct in_addr in_addr;
    char **pptr = NULL;
    char *ip_addr = NULL;

    if(addr == NULL || addrLen < 16)
    {
        return kGeneralErr;
    }

    host = gethostbyname( domain );
    if((host == NULL) || (host->h_addr_list) == NULL)
    {
        return kGeneralErr;
    }

    pptr = host->h_addr_list;
//    for (; *pptr != NULL; pptr++ )
    {
        in_addr.s_addr = *(uint32_t *) (*pptr);
        ip_addr = inet_ntoa(in_addr);
        memset(addr, 0, addrLen);
        memcpy(addr, ip_addr, strlen(ip_addr));
    }

    return kNoErr;
}


int SSL_ConnectNetwork(Network* n, char* addr, int port, int ca_str_len, char* ca_str)
{
  struct sockaddr_in sAddr;
  int addrSize;
  unsigned long ipAddress;
  char mqtt_server_ipstr[16];
  int retVal = -1;
  OSStatus err = kUnknownErr;
  int nNetTimeout_ms = MQTT_CLIENT_SOCKET_TIMEOUT;  // socket send && recv timeout = 5s
  int opt = 0;
#ifdef MICO_MQTT_CLIENT_SUPPORT_SSL
  int ssl_errno = 0;
#endif

  mqtt_mico_log("connect to server: %s:%d", addr, port);
  memset(mqtt_server_ipstr, 0, sizeof(mqtt_server_ipstr));
  err = usergethostbyname(addr, (uint8_t *)mqtt_server_ipstr, 16);
  if(kNoErr != err){
    mqtt_mico_log("gethostbyname failed, err=%d.", err);
    return -1;
  }
  mqtt_mico_log("gethostbyname success: [%s ==> %s]", addr, mqtt_server_ipstr);
  ipAddress = inet_addr(mqtt_server_ipstr);
  sAddr.sin_port = htons(port);
  sAddr.sin_addr.s_addr = ipAddress;
  sAddr.sin_family = AF_INET;
  addrSize = sizeof(sAddr);

  n->my_socket = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
  if( n->my_socket < 0 ) {
    mqtt_mico_log("socket error!");
    return -1;
  }
  mqtt_mico_log("create socket ok.");

  retVal = setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout_ms,sizeof(int));
  if( retVal < 0 ) {
    mqtt_mico_log("setsockopt SO_SNDTIMEO error=%d.", retVal);
    close(n->my_socket);
    return retVal;
  }
  mqtt_mico_log("setsockopt SO_SNDTIMEO=%dms ok.", nNetTimeout_ms);

  retVal = setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout_ms,sizeof(int));
  if( retVal < 0 ) {
    mqtt_mico_log("setsockopt SO_RCVTIMEO error=%d.", retVal);
    close(n->my_socket);
    return retVal;
  }
  mqtt_mico_log("setsockopt SO_RCVTIMEO=%dms ok.", nNetTimeout_ms);

  // set keepalive
  opt = 1;
  setsockopt(n->my_socket, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(opt)); // ����socket��Keepalive����
  opt = MQTT_CLIENT_SOCKET_TCP_KEEPIDLE;
  setsockopt(n->my_socket, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&opt, sizeof(opt)); // TCP IDLE 10���Ժ�ʼ���͵�һ��Keepalive��
  opt = MQTT_CLIENT_SOCKET_TCP_KEEPINTVL;
  setsockopt(n->my_socket, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&opt, sizeof(opt)); // TCP�����Keepalive�ļ��ʱ����10�롣
  opt = MQTT_CLIENT_SOCKET_TCP_KEEPCNT;
  setsockopt(n->my_socket, IPPROTO_TCP, TCP_KEEPCNT, (void *)&opt, sizeof(opt)); // Keepalive ����Ϊ3��
  mqtt_mico_log("set tcp keepalive: idle=%d, interval=%d, cnt=%d.", 10, 10, 3);

  retVal = connect(n->my_socket, (struct sockaddr *)&sAddr, addrSize);
  if( retVal < 0 ) {
    mqtt_mico_log("connect error=%d.", retVal);
    close(n->my_socket);
    return retVal;
  }
  mqtt_mico_log("socket connect ok.");

#ifdef MICO_MQTT_CLIENT_SUPPORT_SSL
  // ssl connect
  if(n->ssl_flag & MICO_MQTT_CLIENT_SSL_DEBUG_ENABLE){
    //CyaSSL_Debugging_ON();
    //CyaSSL_SetLoggingCb(print_ssl_msg);
  }

  mqtt_mico_log("Memory remains before ssl connect %d", MicoGetMemoryInfo()->free_memory);
  //ca_str_len = 0;
  if((ca_str_len > 0) && (NULL != ca_str)){
    mqtt_mico_log("SSL connect with ca:[%d][%s]", ca_str_len, ca_str);
    n->ssl = (void*)ssl_connect(n->my_socket, ca_str_len, ca_str, &ssl_errno);
  }
  else{
    mqtt_mico_log("SSL connect without ca.");
    n->ssl = (void*)ssl_connect(n->my_socket, 0, NULL, &ssl_errno);
  }
  mqtt_mico_log("Memory remains after  ssl connect %d", MicoGetMemoryInfo()->free_memory);

  if (NULL == n->ssl){
    mqtt_mico_log("ssl_connect failed, err=%d.\r\n", ssl_errno);
    close(n->my_socket);
    n->my_socket = -1;
    retVal = -1;
  }
  else{
    retVal = 0;
    mqtt_mico_log("ssl connect ok.");
  }
#endif

  return retVal;
}

int ConnectNetwork(Network* n, char* addr, int port)
{
  struct sockaddr_in sAddr;
  int addrSize;
  unsigned long ipAddress;
  char mqtt_server_ipstr[16];
  int retVal = -1;
  OSStatus err = kUnknownErr;
  int nNetTimeout_ms = MQTT_CLIENT_SOCKET_TIMEOUT;  // socket send && recv timeout = 5s
  int opt = 0;

  mqtt_mico_log("connect to server: %s:%d", addr, port);

  memset(mqtt_server_ipstr, 0, sizeof(mqtt_server_ipstr));
  err = usergethostbyname(addr, (uint8_t *)mqtt_server_ipstr, 16);
  if(kNoErr != err){
    mqtt_mico_log("gethostbyname failed, err=%d.", err);
    return -1;
  }
  mqtt_mico_log("gethostbyname success: [%s ==> %s]", addr, mqtt_server_ipstr);

  ipAddress = inet_addr(mqtt_server_ipstr);
  sAddr.sin_family = AF_INET;
  sAddr.sin_addr.s_addr = ipAddress;
  sAddr.sin_port = htons(port);
  addrSize = sizeof(sAddr);


  n->my_socket = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
  if( n->my_socket < 0 ) {
    mqtt_mico_log("socket error!");
    return -1;
  }
  mqtt_mico_log("create socket ok.");

  retVal = setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout_ms,sizeof(int));
  if( retVal < 0 ) {
    mqtt_mico_log("setsockopt SO_SNDTIMEO error=%d.", retVal);
    close(n->my_socket);
    return retVal;
  }
  mqtt_mico_log("setsockopt SO_SNDTIMEO=%dms ok.", nNetTimeout_ms);

  retVal = setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout_ms,sizeof(int));
  if( retVal < 0 ) {
    mqtt_mico_log("setsockopt SO_RCVTIMEO error=%d.", retVal);
    close(n->my_socket);
    return retVal;
  }
  mqtt_mico_log("setsockopt SO_RCVTIMEO=%dms ok.", nNetTimeout_ms);

  // set keepalive
  opt = 1;
  setsockopt(n->my_socket, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(opt)); // ����socket��Keepalive����
  opt = MQTT_CLIENT_SOCKET_TCP_KEEPIDLE;
  setsockopt(n->my_socket, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&opt, sizeof(opt)); // TCP IDLE 10���Ժ�ʼ���͵�һ��Keepalive��
  opt = MQTT_CLIENT_SOCKET_TCP_KEEPINTVL;
  setsockopt(n->my_socket, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&opt, sizeof(opt)); // TCP�����Keepalive�ļ��ʱ����10�롣
  opt = MQTT_CLIENT_SOCKET_TCP_KEEPCNT;
  setsockopt(n->my_socket, IPPROTO_TCP, TCP_KEEPCNT, (void *)&opt, sizeof(opt)); // Keepalive ����Ϊ3��
  mqtt_mico_log("set tcp keepalive: idle=%d, interval=%d, cnt=%d.", 10, 10, 3);

  retVal = connect(n->my_socket, (struct sockaddr *)&sAddr, addrSize);
  if( retVal < 0 ) {
    mqtt_mico_log("connect error:[%d]", retVal);
    close(n->my_socket);
    return retVal;
  }
  mqtt_mico_log("socket connect ok.");

  return retVal;
}

int NewNetwork(Network* n, char* addr, int port, ssl_opts ssl_settings)
{
  int rc = -1;

  mqtt_mico_log("create network...");

  /* 1. init params */
  n->my_socket = -1;
  n->ssl = NULL;
  n->ssl_flag = 0x0;
  n->mqttread = MICO_read;
  n->mqttwrite = MICO_write;
  n->disconnect = MICO_disconnect;

#ifdef MICO_MQTT_CLIENT_SUPPORT_SSL
  if(ssl_settings.ssl_enable){
    // ssl enable
    n->ssl_flag |= MICO_MQTT_CLIENT_SSL_ENABLE;
    mqtt_mico_log("SSL : enabled.");
    // ssl debug log
    if(ssl_settings.ssl_debug_enable){
      n->ssl_flag |= MICO_MQTT_CLIENT_SSL_DEBUG_ENABLE;
      mqtt_mico_log("SSL debug: enabled.");
    }
    // ssl version
    if( ssl_settings.ssl_version > TLS_V1_2_MODE ){
        mqtt_mico_log("ERROR: unsupported SSL version: %d.", ssl_settings.ssl_version);
        //break;
        return -2;
    }

    ssl_set_client_version(ssl_settings.ssl_version);
    n->ssl_flag |= ((ssl_settings.ssl_version) & 0xFFFF) << 2;
    mqtt_mico_log("SSL version: %d.", ssl_settings.ssl_version);
  }
#endif
  /* 2. create connection */
#ifdef MICO_MQTT_CLIENT_SUPPORT_SSL
  if(n->ssl_flag & MICO_MQTT_CLIENT_SSL_ENABLE){
    rc = SSL_ConnectNetwork(n, addr, port, ssl_settings.ca_str_len, ssl_settings.ca_str);
  }
  else
#endif
  {
    rc = ConnectNetwork(n, addr, port);
  }

  if(rc >= 0){
    mqtt_mico_log("create network OK!");
  }
  else{
    mqtt_mico_log("ERROR: create network failed, err=%d", rc);
  }

  return rc;
}
