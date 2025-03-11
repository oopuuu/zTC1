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

#include "MQTTClient.h"

#define mqtt_client_log(M, ...) //custom_log("MQTT", M, ##__VA_ARGS__)
#define mqtt_client_log_trace() custom_log_trace("MQTT")

void NewMessageData(MessageData* md, MQTTString* aTopicName, MQTTMessage* aMessgage) {
    md->topicName = aTopicName;
    md->message = aMessgage;
}


int getNextPacketId(Client *c) {
    return c->next_packetid = (c->next_packetid == MAX_PACKET_ID) ? 1 : c->next_packetid + 1;
}


// return MQTT_SUCCESS/MQTT_FAILURE
int sendPacket(Client* c, int length, Timer* timer)
{
    int rc = MQTT_FAILURE,
        sent = 0;

    while (sent < length && !expired(timer))
    {
        rc = c->ipstack->mqttwrite(c->ipstack, &c->buf[sent], length, left_ms(timer));
        if (rc < 0)  // there was an error writing the data
            break;
        sent += rc;
    }
    if (sent == length)
    {
        countdown(&c->ping_timer, c->keepAliveInterval/c->heartbeat_retry_max); // record the fact that we have successfully sent the packet
        rc = MQTT_SUCCESS;
    }
    else
        rc = MQTT_FAILURE;
    return rc;
}


//void MQTTClient(Client* c, Network* network, unsigned int command_timeout_ms, unsigned char* buf, size_t buf_size, unsigned char* readbuf, size_t readbuf_size)
int MQTTClientInit(Client* c, Network* network, unsigned int command_timeout_ms)  // modified by wes20151010
{
    int i = 0;
    int rc = MQTT_FAILURE;

    if((NULL == c) || (NULL == network)){
      mqtt_client_log("MQTTClient params error: client or network null!");
      return rc;
    }

    c->ipstack = network;
    c->command_timeout_ms = command_timeout_ms;

    // create default send buf, will be realloced when get larger msg && recover after being processed
    c->buf = (unsigned char*)malloc(DEFAULT_SENDBUF_SIZE);
    if(NULL == c->buf){
      mqtt_client_log("create mqtt send buffer failed!");
      rc = MQTT_BUFFER_OVERFLOW;
      goto exit;
    }
    c->buf_size = DEFAULT_SENDBUF_SIZE;
    // create default recv buf, will be realloced when send larger msg && recover after being processed
    c->readbuf = (unsigned char*)malloc(DEFAULT_READBUF_SIZE);
    if(NULL == c->readbuf){
      mqtt_client_log("create mqtt recv buffer failed!");
      rc = MQTT_BUFFER_OVERFLOW;
      goto exit;
    }
    c->readbuf_size = DEFAULT_READBUF_SIZE;

    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
        c->messageHandlers[i].topicFilter = 0;
    c->isconnected = 0;
    c->ping_outstanding = 0;
    c->heartbeat_retry_cnt = 0;
    if(0 >= c->heartbeat_retry_max){  // if not set retry, default to 1
      c->heartbeat_retry_max = 1;
    }
    c->defaultMessageHandler = NULL;
    InitTimer(&c->ping_timer);

    return MQTT_SUCCESS;

exit:
  if(MQTT_SUCCESS !=  rc){
    if(NULL != c->buf){
      free(c->buf);
      c->buf = NULL;
      c->buf_size = 0;
    }
    if(NULL != c->readbuf){
      free(c->readbuf);
      c->readbuf = NULL;
      c->readbuf_size = 0;
    }
  }

  return rc;
}

int MQTTClientDeinit(Client* c)
{
  int rc = MQTT_FAILURE;

  if(NULL != c){
    if(NULL != c->buf){
      free(c->buf);
      c->buf = NULL;
      c->buf_size = 0;
    }
    if(NULL != c->readbuf){
      free(c->readbuf);
      c->readbuf = NULL;
      c->readbuf_size = 0;
    }
    memset((void*)c, 0, sizeof(Client));
    rc = MQTT_SUCCESS;
  }
  else{
    mqtt_client_log("ERROR: MQTTClientDeinit param is null.");
  }

  return rc;
}

// return rem_len bytes(1~4) if success, else errcode: < 0
// *value is real remain len data len
int decodePacket(Client* c, int* value, int timeout)
{
    unsigned char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    *value = 0;
    do
    {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
        {
            rc = MQTTPACKET_READ_ERROR; /* bad data */
            len = rc;  // return err
            goto exit;
        }
        rc = c->ipstack->mqttread(c->ipstack, &i, 1, timeout);
        if (rc != 1){
            len = MQTTPACKET_READ_ERROR;  // return err
            goto exit;
        }
        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);
exit:
    return len;
}

// return packet type if got data or 0 if no data, else return MQTT_FAILURE
int readPacket(Client* c, Timer* timer)
{
    int rc = MQTT_FAILURE;
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;

    /* 1. read the header byte.  This has the packet type in it */
    len = c->ipstack->mqttread(c->ipstack, c->readbuf, 1, left_ms(timer));
    if(0 == len){
      return 0;  // no data
    }
    else if(1 != len){
      rc = MQTT_SOCKET_ERR;
      goto exit;
    }
    else{}

    len = 1;

    /* 2. read the remaining length.  This is variable in itself */
    if(decodePacket(c, &rem_len, left_ms(timer)) <= 0){
      rc = MQTT_SOCKET_ERR;
      goto exit;
    }
    len += MQTTPacket_encode(c->readbuf + 1, rem_len); /* put the original remaining length back into the buffer */

    // realloc readbuf to get large msg
    if((len + rem_len) > c->readbuf_size){
      c->readbuf = (unsigned char*)realloc((void*)c->readbuf, len + rem_len);
      if(NULL != c->readbuf){
        c->readbuf_size = len + rem_len;
      }
      else{
        mqtt_client_log("no enough memory to recv data!");
        rc = MQTT_BUFFER_OVERFLOW;
        goto exit;
      }
    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if (rem_len > 0 && (c->ipstack->mqttread(c->ipstack, c->readbuf + len, rem_len, left_ms(timer)) != rem_len))
        goto exit;

    header.byte = c->readbuf[0];
    rc = header.bits.type;
exit:
    return rc;
}


// assume topic filter and name is in correct format
// # can only be at end
// + and # can only be next to separator
#define TOPIC_NOT_MATCH       0
#define TOPIC_WHOLE_MATCH     1
#define TOPIC_WILDCART_MATCH  2
char isTopicMatched(char* topicFilter, MQTTString* topicName)
{
    char* curf = topicFilter;
    char* curn = topicName->lenstring.data;
    char* curn_end = curn + topicName->lenstring.len;
    char rc = 0;

    while (*curf && curn < curn_end)
    {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+')
        {   // skip until we meet the next separator, or end of string
            char* nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1;    // skip until end of string
        curf++;
        curn++;
    };
    // fix by wes 20141222, return matched for the last level wildcard "/#"
    /*
    return (curn == curn_end) && ((*curf == '\0') ||   // whole match
                                  (('#' == *curf) && ('\0' == *(curf+1))) ||  // like "id/in/" match "id/in/#"
                                  (('/' == *curf) && ('#' == *(curf+1)) && ('\0' == *(curf+2))));  // like "id/in" match "id/in/#"
    */
    // fix return value: 1 whole match, > 1 wildcard match
    if(curn == curn_end){
      if((*curf == '\0')){  // whole match
        rc = TOPIC_WHOLE_MATCH;
      }
      else if(('#' == *curf) && ('\0' == *(curf+1))){ // like "id/in/" match "id/in/#"
        rc = TOPIC_WILDCART_MATCH;
      }
      else if(('/' == *curf) && ('#' == *(curf+1)) && ('\0' == *(curf+2))){  // like "id/in" match "id/in/#"
        rc = TOPIC_WILDCART_MATCH;
      }
      else{
        rc = TOPIC_NOT_MATCH;
      }
    }
    else{
      rc = TOPIC_NOT_MATCH;
    }
    return rc;
}


int deliverMessage(Client* c, MQTTString* topicName, MQTTMessage* message)
{
    int i;
    int rc = MQTT_FAILURE;

    // we have to find the right message handler - indexed by topic
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter != 0 && (MQTTPacket_equals(topicName, (char*)c->messageHandlers[i].topicFilter) ||
                isTopicMatched((char*)c->messageHandlers[i].topicFilter, topicName)))
        {
            if (c->messageHandlers[i].fp != NULL)
            {
                MessageData md;
                NewMessageData(&md, topicName, message);
                c->messageHandlers[i].fp(&md);
                rc = MQTT_SUCCESS;
            }
        }
    }

    if (rc == MQTT_FAILURE && c->defaultMessageHandler != NULL)
    {
        MessageData md;
        NewMessageData(&md, topicName, message);
        c->defaultMessageHandler(&md);
        rc = MQTT_SUCCESS;
    }

    return rc;
}


int keepalive(Client* c)
{
    int rc = MQTT_FAILURE;

    if (c->keepAliveInterval == 0)
    {
        rc = MQTT_SUCCESS;
        goto exit;
    }

    if (expired(&c->ping_timer))
    {
      //****************************** add wes
      // ping response check,
      // ping request has been sent, but no ping response recived in keepAliveInterval,
      // retry until reach max cnt, which means disconnected. add by WES 20151203
      if (c->ping_outstanding)
      {
        c->heartbeat_retry_cnt++;
        mqtt_client_log("WARNING: no MQTT client pong %d times.", c->heartbeat_retry_cnt);
        if(c->heartbeat_retry_max <= c->heartbeat_retry_cnt){
          c->heartbeat_retry_cnt = 0;
          c->isconnected = 0;  // disconnect flag
          return MQTT_FAILURE;  // need to reconnect
        }
        else{
          rc = MQTT_SUCCESS;  // heartbeat retry
        }
      }
      //***************************** add wes

      // send ping every keepalive interval(retry interval)
      Timer timer;
      InitTimer(&timer);
      countdown_ms(&timer, 1000);
      int len = MQTTSerialize_pingreq(c->buf, c->buf_size);
      if (len > 0 && (rc = sendPacket(c, len, &timer)) == MQTT_SUCCESS){ // send the ping packet
        c->ping_outstanding = 1;
        mqtt_client_log("MQTT client ping req.");
      }
    }
    else {
      rc = MQTT_SUCCESS;
    }

exit:
    return rc;
}

// return packet type if success, else errcode < 0
int cycle(Client* c, Timer* timer)
{
  int len = 0;
  int rc = MQTT_FAILURE;
  int packet_type = 0;

  // read the socket, see what work is due
  memset((void*)(c->readbuf), '\0', c->readbuf_size);
  packet_type = readPacket(c, timer);  // return 0 means no data, else packet_type > =1, error < 0

  switch (packet_type)
  {
  case CONNACK:
  case PUBACK:
  case SUBACK:
  case UNSUBACK:
    break;
  case PUBLISH:
    {
      MQTTString topicName;
      MQTTMessage msg;
      if (MQTTDeserialize_publish((unsigned char*)&msg.dup, (int*)&msg.qos, (unsigned char*)&msg.retained, (unsigned short*)&msg.id, &topicName,
                                  (unsigned char**)&msg.payload, (int*)&msg.payloadlen, c->readbuf, c->readbuf_size) != 1){
                                    //goto exit;  //ignored, continue
                                  }

      deliverMessage(c, &topicName, &msg);
      if (msg.qos != QOS0)
      {
        if (msg.qos == QOS1)
          len = MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
        else if (msg.qos == QOS2)
          len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREC, 0, msg.id);
        if (len <= 0)
          rc = MQTT_FAILURE;
        else
          rc = sendPacket(c, len, timer);
        // response failed
        if (MQTT_FAILURE == rc)
            goto exit; // there was a problem
      }
      break;
    }
  case PUBREC:
    {
      unsigned short mypacketid;
      unsigned char dup, type;
      if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
        rc = MQTT_FAILURE;
      else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREL, 0, mypacketid)) <= 0)
        rc = MQTT_FAILURE;
      else if ((rc = sendPacket(c, len, timer)) != MQTT_SUCCESS) // send the PUBREL packet
        rc = MQTT_FAILURE; // there was a problem
      // response failed
      if (MQTT_FAILURE == rc)
          goto exit; // there was a problem
      break;
    }
  case PUBREL:
    {
      unsigned short mypacketid;
      unsigned char dup, type;
      if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
        rc = MQTT_FAILURE;
      else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBCOMP, 0, mypacketid)) <= 0)
        rc = MQTT_FAILURE;
      else if ((rc = sendPacket(c, len, timer)) != MQTT_SUCCESS) // send the PUBCOMP packet
        rc = MQTT_FAILURE; // there was a problem
      // response failed
      if (MQTT_FAILURE == rc)
        goto exit; // there was a problem
      break;
    }
  case PUBCOMP:
    break;
  case PINGRESP:
    c->ping_outstanding = 0;
    c->isconnected = 1;  // recived response, means still connected. add by WES 20141021
    c->heartbeat_retry_cnt = 0;
    mqtt_client_log("MQTT client ping ack.");
    break;
  case DISCONNECT:  //// disconnect recived ,will try reconnect. add by WES 20141022
    c->ping_outstanding = 0;
    c->isconnected = 0;
    c->heartbeat_retry_cnt = 0;
    rc = MQTT_FAILURE;
    goto exit;
    break;
  case MQTT_SOCKET_ERR:
    mqtt_client_log("ERROR: cycle: socket error!");
    rc = MQTT_FAILURE;
    goto exit;
  case 0:  // return 0 means no data recv, need to do ping check
    mqtt_client_log("INFO: cycle: return 0, no data got.");
    rc = MQTT_SUCCESS;
    break;
  default:
    mqtt_client_log("ERROR: cycle:packet_type error, type=%d.", packet_type);
    rc = MQTT_FAILURE;
    goto exit;
    break;
  }

  // heartbeat check if no data got
  if(0 == rc){
    rc = keepalive(c);
    if(MQTT_SUCCESS != rc){
      mqtt_client_log("ERROR: cycle: keepalive error=%d.", rc);
    }
  }

  if((packet_type >= CONNECT) && (packet_type <= DISCONNECT)){
    mqtt_client_log("cycle: got packet ok, type=%d.", packet_type);
    rc = MQTT_SUCCESS;
  }

exit:
  // return msg type if no error
  if (MQTT_SUCCESS == rc){
    if (packet_type > 0){
      rc = packet_type;
    }
  }
  // release unused msg buffer
  if(c->readbuf_size > DEFAULT_READBUF_SIZE){
    c->readbuf = (unsigned char*)realloc((void*)c->readbuf, DEFAULT_READBUF_SIZE);
    if(NULL != c->readbuf){
      c->readbuf_size = DEFAULT_READBUF_SIZE;
      memset(c->readbuf, 0, DEFAULT_READBUF_SIZE);
    }
    else{
      mqtt_client_log("realloc(release) readbuf failed!");
    }
  }
  return rc;
}

/*
int MQTTYield(Client* c, int timeout_ms)
{
    int rc = MQTT_SUCCESS;
    Timer timer;

    InitTimer(&timer);
    countdown_ms(&timer, timeout_ms);
    while (!expired(&timer))
    {
        if (cycle(c, &timer) == MQTT_FAILURE)
        {
            rc = MQTT_FAILURE;
            break;
        }
    }

    return rc;
}
*/
int MQTTYield(Client* c, int timeout_ms)
{
  int rc = MQTT_SUCCESS;
  Timer timer;

  InitTimer(&timer);
  countdown_ms(&timer, timeout_ms);
  if (cycle(c, &timer) == MQTT_FAILURE)
  {
    rc = MQTT_FAILURE;
  }

  return rc;
}

// only used in single-threaded mode where one command at a time is in process
int waitfor(Client* c, int packet_type, Timer* timer)
{
    int rc = MQTT_FAILURE;

    do
    {
        if (expired(timer))
            break; // we timed out

        rc = cycle(c, timer);
    }
    while ((rc != packet_type) && (MQTT_FAILURE != rc));

    return rc;
}


int MQTTConnect(Client* c, MQTTPacket_connectData* options)
{
    Timer connect_timer;
    int rc = MQTT_FAILURE;
    MQTTPacket_connectData default_options = MQTTPacket_connectData_initializer;
    int len = 0;

    if(NULL == c){
      mqtt_client_log("ERROR: MQTTClient params error: client is null!");
      return rc;
    }

    InitTimer(&connect_timer);
    countdown_ms(&connect_timer, c->command_timeout_ms);

    if (c->isconnected){ // don't send connect packet again if we are already connected
      rc = MQTT_SUCCESS;
      goto exit;
    }

    if (options == 0)
        options = &default_options; // set default options if none were supplied

    c->keepAliveInterval = options->keepAliveInterval;
    countdown(&c->ping_timer, c->keepAliveInterval/c->heartbeat_retry_max);
    if ((len = MQTTSerialize_connect(c->buf, c->buf_size, options)) <= 0)
      goto exit;
    if ((rc = sendPacket(c, len, &connect_timer)) != MQTT_SUCCESS)  // send the connect packet
      goto exit; // there was a problem

    // this will be a blocking call, wait for the connack
    if (waitfor(c, CONNACK, &connect_timer) == CONNACK)
    {
        unsigned char connack_rc = 255;
        char sessionPresent = 0;
        if (MQTTDeserialize_connack((unsigned char*)&sessionPresent, &connack_rc, c->readbuf, c->readbuf_size) == 1)
          rc = connack_rc;
        else{
          mqtt_client_log("ERROR: MQTTConnect:MQTTDeserialize_connack error=%d.", rc);
          rc = MQTT_FAILURE;
        }
    }
    else{
      mqtt_client_log("ERROR: MQTTConnect: no response!");
      rc = MQTT_FAILURE;
    }

exit:
    if (rc == MQTT_SUCCESS)
        c->isconnected = 1;
    return rc;
}


int MQTTSubscribe(Client* c, const char* topicFilter, enum QoS qos, messageHandler messageHandler)
{
    int rc = MQTT_FAILURE;
    Timer timer;
    int len = 0;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicFilter;
    topic.lenstring.data = (char*)topicFilter;
    topic.lenstring.len = strlen(topicFilter);

    InitTimer(&timer);
    countdown_ms(&timer, c->command_timeout_ms);

    if (!c->isconnected)
        goto exit;

    ///// add by wes20151009: realloc send buffer
    len = MQTTPacket_len(MQTTSerialize_subscribeLength(1, &topic));
    if (len > c->buf_size)
    {
      c->buf = (unsigned char*)realloc((void*)c->buf, len);
      if(NULL != c->buf){
        c->buf_size = len;
      }
      else{
        mqtt_client_log("no enough memory to serialize subscribe data!");
        rc = MQTT_BUFFER_OVERFLOW;
        goto exit;
      }
    }
    /////

    len = MQTTSerialize_subscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic, (int*)&qos);
    if (len <= 0)
        goto exit;
    if ((rc = sendPacket(c, len, &timer)) != MQTT_SUCCESS) // send the subscribe packet
        goto exit;             // there was a problem

    if (waitfor(c, SUBACK, &timer) == SUBACK)      // wait for suback
    {
        int count = 0, grantedQoS = -1;
        unsigned short mypacketid;
        if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, c->readbuf, c->readbuf_size) == 1)
            rc = grantedQoS; // 0, 1, 2 or 0x80
        if (rc != 0x80)
        {
            int i;
            for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
            {
                ///// add by wes20151009: already subscribed
                if(c->messageHandlers[i].topicFilter != 0){
                  if(TOPIC_WHOLE_MATCH == isTopicMatched((char*)c->messageHandlers[i].topicFilter, &topic)){
                    mqtt_client_log("message handler has already been added into topic filters(topic=%s).", topic.cstring);
                    return MQTT_SUCCESS;
                  }
                }
                else
                /////
                //if (c->messageHandlers[i].topicFilter == 0)
                {
                    c->messageHandlers[i].topicFilter = topicFilter;
                    c->messageHandlers[i].fp = messageHandler;
                    rc = 0;
                    break;
                }
            }
            ///// add by wes20151009
            if(i > MAX_MESSAGE_HANDLERS){  // exceed max number of msg hander
              mqtt_client_log("ERROR: overflow: max message hander num=%d", MAX_MESSAGE_HANDLERS);
              rc = MQTT_FAILURE;
            }
            /////
        }
    }
    else
        rc = MQTT_FAILURE;

exit:
    ///// add by wes20151009: release send buffer
    if(c->buf_size > DEFAULT_SENDBUF_SIZE){
      c->buf = (unsigned char*)realloc((void*)c->buf, DEFAULT_SENDBUF_SIZE);
      if(NULL != c->buf){
        c->buf_size = DEFAULT_SENDBUF_SIZE;
        memset(c->buf, 0, DEFAULT_SENDBUF_SIZE);
      }
      else{
        mqtt_client_log("realloc(release) sendbuf failed!");
      }
    }
    /////
    return rc;
}


int MQTTUnsubscribe(Client* c, const char* topicFilter)
{
    int rc = MQTT_FAILURE;
    Timer timer;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicFilter;
    topic.lenstring.data = (char*)topicFilter;
    topic.lenstring.len = strlen(topicFilter);
    int len = 0;

    InitTimer(&timer);
    countdown_ms(&timer, c->command_timeout_ms);

    if (!c->isconnected)
        goto exit;

    ///// add by wes20151009: realloc send buffer
    len = MQTTPacket_len(MQTTSerialize_unsubscribeLength(1, &topic));
    if (len > c->buf_size)
    {
      c->buf = (unsigned char*)realloc((void*)c->buf, len);
      if(NULL != c->buf){
        c->buf_size = len;
      }
      else{
        mqtt_client_log("no enough memory to serialize subscribe data!");
        rc = MQTT_BUFFER_OVERFLOW;
        goto exit;
      }
    }
    /////

    if ((len = MQTTSerialize_unsubscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic)) <= 0)
        goto exit;
    if ((rc = sendPacket(c, len, &timer)) != MQTT_SUCCESS) // send the subscribe packet
        goto exit; // there was a problem

    if (waitfor(c, UNSUBACK, &timer) == UNSUBACK)
    {
        unsigned short mypacketid;  // should be the same as the packetid above
        if (MQTTDeserialize_unsuback(&mypacketid, c->readbuf, c->readbuf_size) == 1)
            rc = 0;
        ///// add by wes20151010
        if(0 == rc){
            // remove subscribed topic msg hander
            for (int i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
            {
                if(c->messageHandlers[i].topicFilter != 0){
                  if(TOPIC_WHOLE_MATCH == isTopicMatched((char*)c->messageHandlers[i].topicFilter, &topic)){
                    c->messageHandlers[i].topicFilter = NULL;
                    c->messageHandlers[i].fp = NULL;
                    mqtt_client_log("message handler removed from topic filters(topic=%s).", topic.cstring);
                  }
                }
            }
        }
        else{
          mqtt_client_log("ERROR: MQTTUnsubscribe:MQTTDeserialize_unsuback error!");
        }
        /////
    }
    else{
        mqtt_client_log("ERROR: MQTTUnsubscribe no response!");
        rc = MQTT_FAILURE;
    }

exit:
    ///// add by wes20151009: release send buffer
    if(c->buf_size > DEFAULT_SENDBUF_SIZE){
      c->buf = (unsigned char*)realloc((void*)c->buf, DEFAULT_SENDBUF_SIZE);
      if(NULL != c->buf){
        c->buf_size = DEFAULT_SENDBUF_SIZE;
        memset(c->buf, 0, DEFAULT_SENDBUF_SIZE);
      }
      else{
        mqtt_client_log("realloc(release) sendbuf failed!");
      }
    }
    /////
    return rc;
}


int MQTTPublish(Client* c, const char* topicName, MQTTMessage* message)
{
    int rc = MQTT_FAILURE;
    Timer timer;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicName;
    int len = 0;

    InitTimer(&timer);
    countdown_ms(&timer, c->command_timeout_ms);

    if (!c->isconnected){
         rc = MQTT_SOCKET_ERR;   // MQTT connect error
        goto exit;
    }

    if (message->qos == QOS1 || message->qos == QOS2)
        message->id = getNextPacketId(c);

    ///// add by wes20151009: realloc send buffer
    len = MQTTPacket_len(MQTTSerialize_publishLength(message->qos, topic, message->payloadlen));
    if (len > c->buf_size)
    {
      c->buf = (unsigned char*)realloc((void*)c->buf, len);
      if(NULL != c->buf){
        c->buf_size = len;
      }
      else{
        mqtt_client_log("no enough memory to serialize send data!");
        rc = MQTT_BUFFER_OVERFLOW;
        goto exit;
      }
    }
    /////

    len = MQTTSerialize_publish(c->buf, c->buf_size, 0, message->qos, message->retained, message->id,
              topic, (unsigned char*)message->payload, message->payloadlen);
    if (len <= 0)
        goto exit;
    if ((rc = sendPacket(c, len, &timer)) != MQTT_SUCCESS){ // send the subscribe packet
        rc = MQTT_SOCKET_ERR;   // MQTT connect error
        goto exit; // there was a problem
    }

    if (message->qos == QOS1)
    {
        if (waitfor(c, PUBACK, &timer) == PUBACK)
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
                rc = MQTT_FAILURE;
        }
        else{
            rc = MQTT_SOCKET_ERR;  // MQTT connect error
        }
    }
    else if (message->qos == QOS2)
    {
        if (waitfor(c, PUBCOMP, &timer) == PUBCOMP)
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
                rc = MQTT_FAILURE;
        }
        else{
            rc = MQTT_SOCKET_ERR;  // MQTT connect error
        }
    }

exit:
    ///// add by wes20151009: release send buffer
    if(c->buf_size > DEFAULT_SENDBUF_SIZE){
      c->buf = (unsigned char*)realloc((void*)c->buf, DEFAULT_SENDBUF_SIZE);
      if(NULL != c->buf){
        c->buf_size = DEFAULT_SENDBUF_SIZE;
        memset(c->buf, 0, DEFAULT_SENDBUF_SIZE);
      }
      else{
        mqtt_client_log("realloc(release) sendbuf failed!");
      }
    }
    /////
    return rc;
}


int MQTTDisconnect(Client* c)
{
    int rc = MQTT_FAILURE;
    Timer timer;     // we might wait for incomplete incoming publishes to complete
    int len = MQTTSerialize_disconnect(c->buf, c->buf_size);

    InitTimer(&timer);
    countdown_ms(&timer, c->command_timeout_ms);

    if (len > 0)
        rc = sendPacket(c, len, &timer);            // send the disconnect packet

    c->isconnected = 0;
    return rc;
}


uint32_t MQTTClientLibVersion(void)
{
  uint32_t version = (uint32_t)MQTT_LIB_VERSION;
  return version;
}
