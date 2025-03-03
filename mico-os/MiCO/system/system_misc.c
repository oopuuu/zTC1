/**
 ******************************************************************************
 * @file    system_misc.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide the system mics functions for internal usage
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mico.h"
#include "StringUtils.h"
#include "time.h"

#include "system_internal.h"

extern system_context_t* sys_context;

system_context_t *system_context( void )
{
    return sys_context;
}

static void micoNotify_DHCPCompleteHandler(IPStatusTypedef *pnet, system_context_t * const inContext)
{
  system_log_trace();
  require(inContext, exit);
  mico_rtos_lock_mutex(&inContext->flashContentInRam_mutex);
  strcpy((char *)inContext->micoStatus.localIp, pnet->ip);
  strcpy((char *)inContext->micoStatus.netMask, pnet->mask);
  strcpy((char *)inContext->micoStatus.gateWay, pnet->gate);
  strcpy((char *)inContext->micoStatus.dnsServer, pnet->dns);
  mico_rtos_unlock_mutex(&inContext->flashContentInRam_mutex);
exit:
  return;
}

static void micoNotify_ConnectFailedHandler(OSStatus err, system_context_t * const inContext)
{
  system_log_trace();
  (void)inContext;
  system_log("Wlan Connection Err %d", err);
}

static void micoNotify_WlanFatalErrHandler(system_context_t * const inContext)
{
  system_log_trace();
  (void)inContext;
  system_log("Wlan Fatal Err!");
  MicoSystemReboot();
}

static void micoNotify_StackOverflowErrHandler(char *taskname, system_context_t * const inContext)
{
  system_log_trace();
  (void)inContext;
  system_log("Thread %s overflow, system rebooting", taskname);
  MicoSystemReboot();
}

static void micoNotify_WifiStatusHandler(WiFiEvent event, system_context_t * const inContext)
{
  system_log_trace();
  (void)inContext;
  switch (event) {
  case NOTIFY_STATION_UP:
    system_log("Station up");
    MicoRfLed(true);
    break;
  case NOTIFY_STATION_DOWN:
    system_log("Station down");
    MicoRfLed(false);
    break;
  case NOTIFY_AP_UP:
    system_log("uAP established");
    MicoRfLed(true);
    break;
  case NOTIFY_AP_DOWN:
    system_log("uAP deleted");
    MicoRfLed(false);
    break;
  default:
    break;
  }
  return;
}

static void micoNotify_WiFIParaChangedHandler(apinfo_adv_t *ap_info, char *key, int key_len, system_context_t * const inContext)
{
  system_log_trace();
  bool _needsUpdate = false;
  require(inContext, exit);
  mico_rtos_lock_mutex(&inContext->flashContentInRam_mutex);
  if(strncmp(inContext->flashContentInRam.micoSystemConfig.ssid, ap_info->ssid, maxSsidLen)!=0){
    strncpy(inContext->flashContentInRam.micoSystemConfig.ssid, ap_info->ssid, maxSsidLen);
    _needsUpdate = true;
  }

  if(memcmp(inContext->flashContentInRam.micoSystemConfig.bssid, ap_info->bssid, 6)!=0){
    memcpy(inContext->flashContentInRam.micoSystemConfig.bssid, ap_info->bssid, 6);
    _needsUpdate = true;
  }

  if(inContext->flashContentInRam.micoSystemConfig.channel != ap_info->channel){
    inContext->flashContentInRam.micoSystemConfig.channel = ap_info->channel;
    _needsUpdate = true;
  }
  
  if(inContext->flashContentInRam.micoSystemConfig.security != ap_info->security){
    inContext->flashContentInRam.micoSystemConfig.security = ap_info->security;
    _needsUpdate = true;
  }

  if(memcmp(inContext->flashContentInRam.micoSystemConfig.key, key, maxKeyLen)!=0){
    memcpy(inContext->flashContentInRam.micoSystemConfig.key, key, maxKeyLen);
    _needsUpdate = true;
  }

  if(inContext->flashContentInRam.micoSystemConfig.keyLength != key_len){
    inContext->flashContentInRam.micoSystemConfig.keyLength = key_len;
    _needsUpdate = true;
  }

  if(_needsUpdate== true)  
    mico_system_context_update( &inContext->flashContentInRam );
  mico_rtos_unlock_mutex(&inContext->flashContentInRam_mutex);
  
exit:
  return;
}

OSStatus system_notification_init( system_context_t * const inContext )
{
  OSStatus err = kNoErr;

  err = mico_system_notify_register( mico_notify_WIFI_CONNECT_FAILED, (void *)micoNotify_ConnectFailedHandler, inContext );
  require_noerr( err, exit );

  err = mico_system_notify_register( mico_notify_WIFI_Fatal_ERROR, (void *)micoNotify_WlanFatalErrHandler, inContext );
  require_noerr( err, exit ); 

  err = mico_system_notify_register( mico_notify_Stack_Overflow_ERROR, (void *)micoNotify_StackOverflowErrHandler, inContext );
  require_noerr( err, exit );

  err = mico_system_notify_register( mico_notify_DHCP_COMPLETED, (void *)micoNotify_DHCPCompleteHandler, inContext );
  require_noerr( err, exit ); 

  err = mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED, (void *)micoNotify_WifiStatusHandler, inContext );
  require_noerr( err, exit );

  err = mico_system_notify_register( mico_notify_WiFI_PARA_CHANGED, (void *)micoNotify_WiFIParaChangedHandler, inContext );
  require_noerr( err, exit ); 

exit:
  return err;
}

void system_connect_wifi_normal( system_context_t * const inContext)
{
  system_log_trace();
  network_InitTypeDef_adv_st wNetConfig;
  memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_adv_st));
  
  mico_rtos_lock_mutex(&inContext->flashContentInRam_mutex);
  strncpy((char*)wNetConfig.ap_info.ssid, inContext->flashContentInRam.micoSystemConfig.ssid, maxSsidLen);
  wNetConfig.ap_info.security = SECURITY_TYPE_AUTO;
  memcpy(wNetConfig.key, inContext->flashContentInRam.micoSystemConfig.user_key, maxKeyLen);
  wNetConfig.key_len = inContext->flashContentInRam.micoSystemConfig.user_keyLength;
  wNetConfig.dhcpMode = inContext->flashContentInRam.micoSystemConfig.dhcpEnable;
  strncpy((char*)wNetConfig.local_ip_addr, inContext->flashContentInRam.micoSystemConfig.localIp, maxIpLen);
  strncpy((char*)wNetConfig.net_mask, inContext->flashContentInRam.micoSystemConfig.netMask, maxIpLen);
  strncpy((char*)wNetConfig.gateway_ip_addr, inContext->flashContentInRam.micoSystemConfig.gateWay, maxIpLen);
  strncpy((char*)wNetConfig.dnsServer_ip_addr, inContext->flashContentInRam.micoSystemConfig.dnsServer, maxIpLen);
  wNetConfig.wifi_retry_interval = 100;
  mico_rtos_unlock_mutex(&inContext->flashContentInRam_mutex);

  system_log("connect to %s.....", wNetConfig.ap_info.ssid);
  micoWlanStartAdv(&wNetConfig);
}

void system_connect_wifi_fast( system_context_t * const inContext)
{
  system_log_trace();
  network_InitTypeDef_adv_st wNetConfig;
  memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_adv_st));
  
  mico_rtos_lock_mutex(&inContext->flashContentInRam_mutex);
  strncpy((char*)wNetConfig.ap_info.ssid, inContext->flashContentInRam.micoSystemConfig.ssid, maxSsidLen);
  memcpy(wNetConfig.ap_info.bssid, inContext->flashContentInRam.micoSystemConfig.bssid, 6);
  wNetConfig.ap_info.channel = inContext->flashContentInRam.micoSystemConfig.channel;
  wNetConfig.ap_info.security = inContext->flashContentInRam.micoSystemConfig.security;
  memcpy(wNetConfig.key, inContext->flashContentInRam.micoSystemConfig.key, inContext->flashContentInRam.micoSystemConfig.keyLength);
  wNetConfig.key_len = inContext->flashContentInRam.micoSystemConfig.keyLength;
  if(inContext->flashContentInRam.micoSystemConfig.dhcpEnable == true)
    wNetConfig.dhcpMode = DHCP_Client;
  else
    wNetConfig.dhcpMode = DHCP_Disable;
  strncpy((char*)wNetConfig.local_ip_addr, inContext->flashContentInRam.micoSystemConfig.localIp, maxIpLen);
  strncpy((char*)wNetConfig.net_mask, inContext->flashContentInRam.micoSystemConfig.netMask, maxIpLen);
  strncpy((char*)wNetConfig.gateway_ip_addr, inContext->flashContentInRam.micoSystemConfig.gateWay, maxIpLen);
  strncpy((char*)wNetConfig.dnsServer_ip_addr, inContext->flashContentInRam.micoSystemConfig.dnsServer, maxIpLen);
  mico_rtos_unlock_mutex(&inContext->flashContentInRam_mutex);

  wNetConfig.wifi_retry_interval = 100;
  system_log("Connect to %s.....", wNetConfig.ap_info.ssid);
  micoWlanStartAdv(&wNetConfig);
}

OSStatus system_network_daemen_start( system_context_t * const inContext )
{
  IPStatusTypedef para;
  uint8_t major, minor, revision;

  MicoInit();
  MicoSysLed(true);
  system_log("Free memory %d bytes", MicoGetMemoryInfo()->free_memory); 
  micoWlanGetIPStatus(&para, Station);
  formatMACAddr(inContext->micoStatus.mac, (char *)&para.mac);
  MicoGetRfVer(inContext->micoStatus.rf_version, sizeof(inContext->micoStatus.rf_version));
  inContext->micoStatus.rf_version[49] = 0x0;

  system_log("Kernel version: %s", MicoGetVer());

  mico_sdk_version( &major, &minor, &revision );
  system_log( "MiCO version: %d.%d.%d", major, minor, revision );

  system_log("Wi-Fi driver version %s, mac %s", inContext->micoStatus.rf_version, inContext->micoStatus.mac);

  if(inContext->flashContentInRam.micoSystemConfig.rfPowerSaveEnable == true){
    micoWlanEnablePowerSave();
  }

  if(inContext->flashContentInRam.micoSystemConfig.mcuPowerSaveEnable == true){
    MicoMcuPowerSaveConfig(true);
  }  
  return kNoErr;
}

void mico_sdk_version( uint8_t *major, uint8_t *minor, uint8_t *revision )
{
  *major = MiCO_SDK_VERSION_MAJOR;
  *minor = MiCO_SDK_VERSION_MINOR;
  *revision = MiCO_SDK_VERSION_REVISION;
}

OSStatus mico_system_get_status_wlan( system_status_wlan_t** status )
{
    if( sys_context == NULL )
    {
        *status = NULL;
        return kNotPreparedErr;
    }
    else
    {
        *status = &sys_context->micoStatus;
        return kNotPreparedErr;
    }
}

OSStatus mico_system_wlan_get_status( mico_system_status_wlan_t** status )
{
    return mico_system_get_status_wlan( status );
}

void mico_app_info(char *str, int len)
{
  snprintf( str, len, "%s %s, build at %s %s", APP_INFO, FIRMWARE_REVISION, __TIME__, __DATE__);
}


