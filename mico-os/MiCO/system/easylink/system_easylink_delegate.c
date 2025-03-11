/**
 ******************************************************************************
 * @file    system_easylink_delegate.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide delegate functions from Easylink.
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
#include "platform_config.h"

#include "StringUtils.h"  
#include "system.h"      

#define SYS_LED_TRIGGER_INTERVAL 100 
#define SYS_LED_TRIGGER_INTERVAL_AFTER_EASYLINK 500 

static mico_timer_t _Led_EL_timer;
static bool _Led_EL_timer_initialized = false;

static void _led_EL_Timeout_handler( void* arg )
{
  (void)(arg);
  MicoGpioOutputTrigger((mico_gpio_t)MICO_SYS_LED);
}

WEAK void mico_system_delegate_config_will_start( void )
{
  /*Led trigger*/
  if(_Led_EL_timer_initialized == true)
  {
    mico_stop_timer(&_Led_EL_timer);
    mico_deinit_timer( &_Led_EL_timer );
    _Led_EL_timer_initialized = false;
  }

  mico_init_timer(&_Led_EL_timer, SYS_LED_TRIGGER_INTERVAL, _led_EL_Timeout_handler, NULL);
  mico_start_timer(&_Led_EL_timer);
  _Led_EL_timer_initialized = true;
  return;
}

WEAK void mico_system_delegate_soft_ap_will_start( void )
{
  return;
}

WEAK void mico_system_delegate_config_will_stop( void )
{
  if(_Led_EL_timer_initialized == true)
  {
    mico_stop_timer(&_Led_EL_timer);
    mico_deinit_timer( &_Led_EL_timer );
    _Led_EL_timer_initialized = false;
  }
  MicoSysLed(true);
  return;
}

WEAK void mico_system_delegate_config_recv_ssid ( char *ssid, char *key )
{
  UNUSED_PARAMETER(ssid);
  UNUSED_PARAMETER(key);

  if(_Led_EL_timer_initialized == true)
  {
    mico_stop_timer(&_Led_EL_timer);
    mico_deinit_timer( &_Led_EL_timer );
    _Led_EL_timer_initialized = false;
  }

  mico_init_timer(&_Led_EL_timer, SYS_LED_TRIGGER_INTERVAL_AFTER_EASYLINK, _led_EL_Timeout_handler, NULL);
  mico_start_timer(&_Led_EL_timer);
  _Led_EL_timer_initialized = true;
  return;
}

WEAK void mico_system_delegate_config_success( mico_config_source_t source )
{
  //system_log( "Configed by %d", source );
  UNUSED_PARAMETER(source);
  return;
}


WEAK OSStatus mico_system_delegate_config_recv_auth_data(char * anthData  )
{
  (void)(anthData);
  return kNoErr;
}

WEAK void mico_easylink_monitor_delegate_will_start( void )
{

}

WEAK void mico_easylink_monitor_delegate_stoped( void )
{

}

WEAK void mico_easylink_monitor_delegate_connect_success( mico_config_source_t source )
{
    UNUSED_PARAMETER( source );
}

WEAK void mico_easylink_monitor_delegate_channel_changed( uint8_t currnet_channel )
{
    UNUSED_PARAMETER( currnet_channel );
}

WEAK void mico_easylink_monitor_delegate_package_recved( uint8_t * frame, int len )
{
    UNUSED_PARAMETER( frame );
    UNUSED_PARAMETER( len );
}
