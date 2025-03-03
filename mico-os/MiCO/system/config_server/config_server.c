/**
 ******************************************************************************
 * @file    config_server.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   Local TCP server for mico device configuration
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mico.h"
#include "system_internal.h"
#include "platform.h"
#include "easylink_internal.h"

#include "SocketUtils.h"
#include "HTTPUtils.h"
#include "StringUtils.h"
#include "CheckSumUtils.h"

#define kCONFIGURLRead          "/config-read"
#define kCONFIGURLWrite         "/config-write"
#define kCONFIGURLWriteByUAP    "/config-write-uap"  /* Don't reboot but connect to AP immediately */
#define kCONFIGURLOTA           "/OTA"

#define kMIMEType_MXCHIP_OTA    "application/ota-stream"

typedef struct _configContext_t{
  uint32_t offset;
  bool     isFlashLocked;
  CRC16_Context crc16_contex;
} configContext_t;

extern OSStatus     ConfigIncommingJsonMessage( int fd, const char *input, bool *need_reboot, mico_Context_t * const inContext );
extern json_object* ConfigCreateReportJsonMessage( mico_Context_t * const inContext );

static void localConfiglistener_thread(uint32_t inContext);
static void localConfig_thread(uint32_t inFd);
static OSStatus _LocalConfigRespondInComingMessage(int fd, HTTPHeader_t* inHeader, system_context_t * const inContext);
static OSStatus onReceivedData(struct _HTTPHeader_t * httpHeader, uint32_t pos, uint8_t * data, size_t len, void * userContext );
static void onClearHTTPHeader(struct _HTTPHeader_t * httpHeader, void * userContext );

bool is_config_server_established = false;

/* Defined in uAP config mode */
extern OSStatus ConfigIncommingJsonMessageUAP( int fd, const uint8_t *input, size_t size, system_context_t * const inContext );
extern system_context_t* sys_context;

static mico_semaphore_t close_listener_sem = NULL, close_client_sem[ MAX_TCP_CLIENT_PER_SERVER ] = { NULL };

WEAK void config_server_delegate_report( json_object *app_menu, mico_Context_t *in_context )
{
  UNUSED_PARAMETER(app_menu);
  UNUSED_PARAMETER(in_context);
  return;
}

WEAK void config_server_delegate_recv( const char *key, json_object *value, bool *need_reboot, mico_Context_t *in_context )
{
  UNUSED_PARAMETER(key);
  UNUSED_PARAMETER(value);
  UNUSED_PARAMETER(need_reboot);
  UNUSED_PARAMETER(in_context);
  return;  
}

OSStatus config_server_start ( void )
{
  int i = 0;
  OSStatus err = kNoErr;
  
  require( sys_context, exit );
  
  if( is_config_server_established )
    return kNoErr;

  is_config_server_established = true;

  close_listener_sem = NULL;
  for (; i < MAX_TCP_CLIENT_PER_SERVER; i++)
    close_client_sem[ i ] = NULL;
  err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "Config Server", localConfiglistener_thread, STACK_SIZE_LOCAL_CONFIG_SERVER_THREAD, 0 );
  require_noerr(err, exit);
  
  mico_thread_msleep(200);

exit:
  return err;
}

OSStatus config_server_stop( void )
{
  int i = 0;
  OSStatus err = kNoErr;

  if( !is_config_server_established )
    return kNoErr;

  for (; i < MAX_TCP_CLIENT_PER_SERVER; i++){
    if( close_client_sem[ i ] != NULL )
      mico_rtos_set_semaphore( &close_client_sem[ i ] );
  }
  mico_thread_msleep(50);

  if( close_listener_sem != NULL )
    mico_rtos_set_semaphore( &close_listener_sem );

  mico_thread_msleep(500);
  is_config_server_established = false;
  
  return err;
}

void localConfiglistener_thread( mico_thread_arg_t arg)
{
  OSStatus err = kUnknownErr;
  int j;
  struct sockaddr_in addr;
  int sockaddr_t_size;
  fd_set readfds;
  char ip_address[16];
  
  int localConfiglistener_fd = -1;
  int close_listener_fd = -1;

  mico_rtos_init_semaphore( &close_listener_sem, 1);
  close_listener_fd = mico_create_event_fd( close_listener_sem );

  /*Establish a TCP server fd that accept the tcp clients connections*/ 
  localConfiglistener_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
  require_action(IsValidSocket( localConfiglistener_fd ), exit, err = kNoResourcesErr );
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr =INADDR_ANY;
  addr.sin_port = htons(MICO_CONFIG_SERVER_PORT);
  err = bind(localConfiglistener_fd, (struct sockaddr *)&addr, sizeof(addr));
  require_noerr( err, exit );

  err = listen(localConfiglistener_fd, 1);
  require_noerr( err, exit );

  system_log("Config Server established at port: %d, fd: %d", MICO_CONFIG_SERVER_PORT, localConfiglistener_fd);
  
  while(1){
    FD_ZERO(&readfds);
    FD_SET(localConfiglistener_fd, &readfds);
    FD_SET(close_listener_fd, &readfds);
    select( Max(localConfiglistener_fd, close_listener_fd) + 1, &readfds, NULL, NULL, NULL);

    /* Check close requests */
    if(FD_ISSET(close_listener_fd, &readfds)){
      mico_rtos_get_semaphore( &close_listener_sem, 0 );
      goto exit;
    }

    /* Check tcp connection requests */
    if(FD_ISSET(localConfiglistener_fd, &readfds)){
      sockaddr_t_size = sizeof(struct sockaddr_in);
      j = accept(localConfiglistener_fd, (struct sockaddr *)&addr, (socklen_t *)&sockaddr_t_size);
      if ( IsValidSocket( j ) ) {
        strcpy(ip_address,inet_ntoa( addr.sin_addr ));
        system_log("Config Client %s:%d connected, fd: %d", ip_address, addr.sin_port, j);
        if(kNoErr !=  mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "Config Clients", localConfig_thread, STACK_SIZE_LOCAL_CONFIG_CLIENT_THREAD, (mico_thread_arg_t)j) )
          SocketClose(&j);
      }
    }
  }

exit:
    if( close_listener_sem != NULL ){
      mico_delete_event_fd( close_listener_fd );
      mico_rtos_deinit_semaphore( &close_listener_sem );
      close_listener_sem = NULL;
    };
    system_log("Exit: Config listener exit with err = %d", err);
    SocketClose( &localConfiglistener_fd );
    is_config_server_established = false;
    mico_rtos_delete_thread(NULL);
    return;
}

void localConfig_thread(uint32_t inFd)
{
  OSStatus err = kNoErr;
  int clientFd = (int)inFd;
  int clientFdIsSet;
  int close_sem_index;
  fd_set readfds;
  struct timeval t;
  HTTPHeader_t *httpHeader = NULL;
  int close_client_fd = -1;
  configContext_t httpContext = {0, false};

  for( close_sem_index = 0; close_sem_index < MAX_TCP_CLIENT_PER_SERVER; close_sem_index++ ){
    if( close_client_sem[close_sem_index] == NULL )
      break;
  }

  if( close_sem_index == MAX_TCP_CLIENT_PER_SERVER){
    mico_rtos_delete_thread(NULL);
    return;
  }

  mico_rtos_init_semaphore( &close_client_sem[close_sem_index], 1);
  close_client_fd = mico_create_event_fd( close_client_sem[close_sem_index] );

  httpHeader = HTTPHeaderCreateWithCallback( 512, onReceivedData, onClearHTTPHeader, &httpContext );
  require_action( httpHeader, exit, err = kNoMemoryErr );
  HTTPHeaderClear( httpHeader );

  t.tv_sec = 60;
  t.tv_usec = 0;
  system_log("Free memory %d bytes", MicoGetMemoryInfo()->free_memory) ; 

  while(1){
    FD_ZERO(&readfds);
    FD_SET(clientFd, &readfds);
    FD_SET(close_client_fd, &readfds);
    clientFdIsSet = 0;

    if(httpHeader->len == 0){
      require(select( Max(clientFd, close_client_fd) + 1 , &readfds, NULL, NULL, &t) >= 0, exit);
      clientFdIsSet = FD_ISSET(clientFd, &readfds);
    }

    /* Check close requests */
    if(FD_ISSET(close_client_fd, &readfds)){
      mico_rtos_get_semaphore( &close_client_sem[close_sem_index], 0 );
      err = kConnectionErr;
      goto exit;
    }    
  
    if(clientFdIsSet||httpHeader->len){
      err = SocketReadHTTPHeader( clientFd, httpHeader );

      switch ( err )
      {
        case kNoErr:
          // Read the rest of the HTTP body if necessary
          //do{
          err = SocketReadHTTPBody( clientFd, httpHeader );
          
          if(httpHeader->dataEndedbyClose == true){
            err = _LocalConfigRespondInComingMessage( clientFd, httpHeader, sys_context );
            require_noerr(err, exit);
            err = kConnectionErr;
            goto exit;
          }else{
            require_noerr(err, exit);
            err = _LocalConfigRespondInComingMessage( clientFd, httpHeader, sys_context );
            require_noerr(err, exit);
          }

          HTTPHeaderClear( httpHeader );
        break;

        case EWOULDBLOCK:
            // NO-OP, keep reading
        break;

        case kNoSpaceErr:
          system_log("ERROR: Cannot fit HTTPHeader.");
          goto exit;
        
        case kConnectionErr:
          // NOTE: kConnectionErr from SocketReadHTTPHeader means it's closed
          system_log("ERROR: Connection closed.");
          goto exit;

        default:
          system_log("ERROR: HTTP Header parse internal error: %d", err);
          goto exit;
      }
    }
  }

exit:
  system_log("Exit: Client exit with err = %d", err);
  SocketClose(&clientFd);

  if( close_client_sem[close_sem_index] != NULL )
  {
    mico_delete_event_fd( close_client_fd );
    mico_rtos_deinit_semaphore( &close_client_sem[close_sem_index] );
    close_client_sem[close_sem_index] = NULL;
  };

  HTTPHeaderDestory( &httpHeader );
  mico_rtos_delete_thread(NULL);
  return;
}

static OSStatus onReceivedData(struct _HTTPHeader_t * inHeader, uint32_t inPos, uint8_t * inData, size_t inLen, void * inUserContext )
{
  OSStatus err = kUnknownErr;
  const char *    value;
  size_t          valueSize;
  configContext_t *context = (configContext_t *)inUserContext;
  mico_logic_partition_t* ota_partition = MicoFlashGetInfo( MICO_PARTITION_OTA_TEMP );

  err = HTTPGetHeaderField( inHeader->buf, inHeader->len, "Content-Type", NULL, NULL, &value, &valueSize, NULL );
  if(err == kNoErr && strnicmpx( value, valueSize, kMIMEType_MXCHIP_OTA ) == 0){

    if( ota_partition->partition_owner == MICO_FLASH_NONE ){
      system_log("OTA storage is not exist");
      return kUnsupportedErr;
    }

     if(inPos == 0){
       context->offset = 0x0;
       CRC16_Init( &context->crc16_contex );
       mico_rtos_lock_mutex(&sys_context->flashContentInRam_mutex); //We are write the Flash content, no other write is possible
       context->isFlashLocked = true;
       err = MicoFlashErase( MICO_PARTITION_OTA_TEMP, 0x0, ota_partition->partition_length);
       require_noerr(err, flashErrExit);
       err = MicoFlashWrite( MICO_PARTITION_OTA_TEMP, &context->offset, (uint8_t *)inData, inLen);
       require_noerr(err, flashErrExit);
       CRC16_Update( &context->crc16_contex, inData, inLen);
     }else{
       err = MicoFlashWrite( MICO_PARTITION_OTA_TEMP, &context->offset, (uint8_t *)inData, inLen);
       require_noerr(err, flashErrExit);
       CRC16_Update( &context->crc16_contex, inData, inLen);
     }
  }
  else{
    return kUnsupportedErr;
  }

  if(err!=kNoErr)  system_log("onReceivedData");
  return err;

flashErrExit:
  mico_rtos_unlock_mutex(&sys_context->flashContentInRam_mutex);
  return err;
}

static void onClearHTTPHeader(struct _HTTPHeader_t * inHeader, void * inUserContext )
{
  UNUSED_PARAMETER(inHeader);
  configContext_t *context = (configContext_t *)inUserContext;

  if(context->isFlashLocked == true){
    mico_rtos_unlock_mutex(&sys_context->flashContentInRam_mutex);
    context->isFlashLocked = false;
  }
 }

OSStatus _LocalConfigRespondInComingMessage(int fd, HTTPHeader_t* inHeader, system_context_t * const inContext)
{
  OSStatus err = kUnknownErr;
  const char *  json_str;
  uint8_t *httpResponse = NULL;
  size_t httpResponseLen = 0;
  json_object* report = NULL, *config = NULL;
  bool need_reboot = false;
  uint16_t crc;
  configContext_t *http_context = (configContext_t *)inHeader->userContext;
  mico_logic_partition_t* ota_partition = MicoFlashGetInfo( MICO_PARTITION_OTA_TEMP );
  char name[50];

  json_object *sectors, *sector = NULL;

  if(HTTPHeaderMatchURL( inHeader, kCONFIGURLRead ) == kNoErr){    
    //report = ConfigCreateReportJsonMessage( inContext );

    mico_rtos_lock_mutex(&inContext->flashContentInRam_mutex);
    snprintf(name, 50, "%s(%c%c%c%c%c%c)",MODEL, 
                                          inContext->micoStatus.mac[9],  inContext->micoStatus.mac[10], 
                                          inContext->micoStatus.mac[12], inContext->micoStatus.mac[13],
                                          inContext->micoStatus.mac[15], inContext->micoStatus.mac[16]);
    report = json_object_new_object();
    require_action(report, exit, err = kNoMemoryErr);

    sectors = json_object_new_array();
    require( sectors, exit );

    json_object_object_add(report, "T", json_object_new_string("Current Configuration"));
    json_object_object_add(report, "N", json_object_new_string(name));
    json_object_object_add(report, "C", sectors);

    json_object_object_add(report, "PO", json_object_new_string(PROTOCOL));
    json_object_object_add(report, "HD", json_object_new_string(HARDWARE_REVISION));
    json_object_object_add(report, "FW", json_object_new_string(FIRMWARE_REVISION));
    json_object_object_add(report, "RF", json_object_new_string(inContext->micoStatus.rf_version));

#ifdef MICO_CONFIG_SERVER_REPORT_SYSTEM_DATA
    /*Sector 1*/
    sector = json_object_new_array();
    require( sector, exit );
    err = config_server_create_sector(sectors, "MICO SYSTEM",    sector);
    require_noerr(err, exit);

      /*name cell*/
      err = config_server_create_string_cell(sector, "Device Name",    inContext->flashContentInRam.micoSystemConfig.name,               "RW", NULL);
      require_noerr(err, exit);

      //RF power save switcher cell
      err = config_server_create_bool_cell(sector, "RF power save",  inContext->flashContentInRam.micoSystemConfig.rfPowerSaveEnable,  "RW");
      require_noerr(err, exit);

      //MCU power save switcher cell
      err = config_server_create_bool_cell(sector, "MCU power save", inContext->flashContentInRam.micoSystemConfig.mcuPowerSaveEnable, "RW");
      require_noerr(err, exit);

      /*SSID cell*/
      err = config_server_create_string_cell(sector, "Wi-Fi",        inContext->flashContentInRam.micoSystemConfig.ssid,     "RW", NULL);
      require_noerr(err, exit);
      /*PASSWORD cell*/
      err = config_server_create_string_cell(sector, "Password",     inContext->flashContentInRam.micoSystemConfig.user_key, "RW", NULL);
      require_noerr(err, exit);
      /*DHCP cell*/
      err = config_server_create_bool_cell(sector, "DHCP",        inContext->flashContentInRam.micoSystemConfig.dhcpEnable,   "RW");
      require_noerr(err, exit);
      /*Local cell*/
      err = config_server_create_string_cell(sector, "IP address",  inContext->micoStatus.localIp,   "RW", NULL);
      require_noerr(err, exit);
      /*Netmask cell*/
      err = config_server_create_string_cell(sector, "Net Mask",    inContext->micoStatus.netMask,   "RW", NULL);
      require_noerr(err, exit);
      /*Gateway cell*/
      err = config_server_create_string_cell(sector, "Gateway",     inContext->micoStatus.gateWay,   "RW", NULL);
      require_noerr(err, exit);
      /*DNS server cell*/
      err = config_server_create_string_cell(sector, "DNS Server",  inContext->micoStatus.dnsServer, "RW", NULL);
      require_noerr(err, exit);
#endif
      
    /*Sector 2*/
    sector = json_object_new_array();
    require( sector, exit );
    err = config_server_create_sector(sectors, "APPLICATION",    sector);
    require_noerr(err, exit);

    config_server_delegate_report( sector, &inContext->flashContentInRam );

    mico_rtos_unlock_mutex(&inContext->flashContentInRam_mutex);

    json_str = json_object_to_json_string(report);
    require_action( json_str, exit, err = kNoMemoryErr );
    system_log("Send config object=%s", json_str);
    err =  CreateSimpleHTTPMessageNoCopy( kMIMEType_JSON, strlen(json_str), &httpResponse, &httpResponseLen );
    require_noerr( err, exit );
    require( httpResponse, exit );
    err = SocketSend( fd, httpResponse, httpResponseLen );
    require_noerr( err, exit );
    err = SocketSend( fd, (uint8_t *)json_str, strlen(json_str) );
    require_noerr( err, exit );
    system_log("Current configuration sent");
    goto exit;
  }
  else if(HTTPHeaderMatchURL( inHeader, kCONFIGURLWrite ) == kNoErr){
    if(inHeader->contentLength > 0){
      system_log("Recv new configuration, apply");

      err =  CreateSimpleHTTPOKMessage( &httpResponse, &httpResponseLen );
      require_noerr( err, exit );
      require( httpResponse, exit );
      err = SocketSend( fd, httpResponse, httpResponseLen );
      require_noerr( err, exit );

      config = json_tokener_parse(inHeader->extraDataPtr);
      require_action(config, exit, err = kUnknownErr);
      system_log("Recv config object=%s", json_object_to_json_string(config));
      mico_rtos_lock_mutex(&inContext->flashContentInRam_mutex);
      json_object_object_foreach(config, key, val) {
        if(!strcmp(key, "Device Name")){
          strncpy(inContext->flashContentInRam.micoSystemConfig.name, json_object_get_string(val), maxNameLen);
          need_reboot = true;
        }else if(!strcmp(key, "RF power save")){
          inContext->flashContentInRam.micoSystemConfig.rfPowerSaveEnable = json_object_get_boolean(val);
          need_reboot = true;
        }else if(!strcmp(key, "MCU power save")){
          inContext->flashContentInRam.micoSystemConfig.mcuPowerSaveEnable = json_object_get_boolean(val);
          need_reboot = true;
        }else if(!strcmp(key, "Wi-Fi")){
          strncpy(inContext->flashContentInRam.micoSystemConfig.ssid, json_object_get_string(val), maxSsidLen);
          inContext->flashContentInRam.micoSystemConfig.channel = 0;
          memset(inContext->flashContentInRam.micoSystemConfig.bssid, 0x0, 6);
          inContext->flashContentInRam.micoSystemConfig.security = SECURITY_TYPE_AUTO;
          memcpy(inContext->flashContentInRam.micoSystemConfig.key, inContext->flashContentInRam.micoSystemConfig.user_key, maxKeyLen);
          inContext->flashContentInRam.micoSystemConfig.keyLength = inContext->flashContentInRam.micoSystemConfig.user_keyLength;
          need_reboot = true;
        }else if(!strcmp(key, "Password")){
          inContext->flashContentInRam.micoSystemConfig.security = SECURITY_TYPE_AUTO;
          strncpy(inContext->flashContentInRam.micoSystemConfig.key, json_object_get_string(val), maxKeyLen);
          strncpy(inContext->flashContentInRam.micoSystemConfig.user_key, json_object_get_string(val), maxKeyLen);
          inContext->flashContentInRam.micoSystemConfig.keyLength = strlen(inContext->flashContentInRam.micoSystemConfig.key);
          inContext->flashContentInRam.micoSystemConfig.user_keyLength = strlen(inContext->flashContentInRam.micoSystemConfig.key);
          need_reboot = true;
        }else if(!strcmp(key, "DHCP")){
          inContext->flashContentInRam.micoSystemConfig.dhcpEnable   = json_object_get_boolean(val);
          need_reboot = true;
        }else if(!strcmp(key, "IP address")){
          strncpy(inContext->flashContentInRam.micoSystemConfig.localIp, json_object_get_string(val), maxIpLen);
          need_reboot = true;
        }else if(!strcmp(key, "Net Mask")){
          strncpy(inContext->flashContentInRam.micoSystemConfig.netMask, json_object_get_string(val), maxIpLen);
          need_reboot = true;
        }else if(!strcmp(key, "Gateway")){
          strncpy(inContext->flashContentInRam.micoSystemConfig.gateWay, json_object_get_string(val), maxIpLen);
          need_reboot = true;
        }else if(!strcmp(key, "DNS Server")){
          strncpy(inContext->flashContentInRam.micoSystemConfig.dnsServer, json_object_get_string(val), maxIpLen);
          need_reboot = true;
        }else{
          config_server_delegate_recv( key, val, &need_reboot, &inContext->flashContentInRam );
        }
      }
      mico_rtos_unlock_mutex(&inContext->flashContentInRam_mutex);

      json_object_put(config);

      inContext->flashContentInRam.micoSystemConfig.configured = allConfigured;
      mico_system_context_update( &inContext->flashContentInRam );

      if( need_reboot == true ){
        mico_system_power_perform( &inContext->flashContentInRam, eState_Software_Reset );
      }
    }
    goto exit;
  }
  else if(HTTPHeaderMatchURL( inHeader, kCONFIGURLWriteByUAP ) == kNoErr){
    if(inHeader->contentLength > 0){
      system_log( "Recv new configuration from uAP, apply and connect to AP" );
      err = ConfigIncommingJsonMessageUAP( fd, (uint8_t *)inHeader->extraDataPtr, inHeader->extraDataLen, inContext );
      require_noerr( err, exit );
    }
    goto exit;
  }
  else if(HTTPHeaderMatchURL( inHeader, kCONFIGURLOTA ) == kNoErr && ota_partition->partition_owner != MICO_FLASH_NONE){
    if(inHeader->contentLength > 0){
      system_log("Receive OTA data!");
      CRC16_Final( &http_context->crc16_contex, &crc);
      memset(&inContext->flashContentInRam.bootTable, 0, sizeof(boot_table_t));
      inContext->flashContentInRam.bootTable.length = inHeader->contentLength;
      inContext->flashContentInRam.bootTable.start_address = ota_partition->partition_start_addr;
      inContext->flashContentInRam.bootTable.type = 'A';
      inContext->flashContentInRam.bootTable.upgrade_type = 'U';
      inContext->flashContentInRam.bootTable.crc = crc;
      mico_system_context_update( &inContext->flashContentInRam );
      SocketClose( &fd );
      mico_system_power_perform( &inContext->flashContentInRam, eState_Software_Reset );
      mico_thread_sleep( MICO_WAIT_FOREVER );
    }
    goto exit;
  }
  else{
    return kNotFoundErr;
  };

 exit:
  if(inHeader->persistent == false)  //Return an err to close socket and exit the current thread
    err = kConnectionErr;
  if(httpResponse)  free(httpResponse);
  if(report)        json_object_put(report);
  if(config)        json_object_put(config);

  return err;

}

