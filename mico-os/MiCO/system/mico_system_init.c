/**
 ******************************************************************************
 * @file    mico_system_init.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide the mico system initialize function.
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

#include <time.h>

#include "mico.h"

#include "system_internal.h"

extern system_context_t* sys_context;
#ifndef  EasyLink_Needs_Reboot
static mico_worker_thread_t wlan_autoconf_worker_thread;
#endif


/******************************************************
 *               Variables Definitions
 ******************************************************/

static OSStatus system_config_mode_worker( void *arg )
{
    OSStatus err = kNoErr;
    mico_Context_t* in_context = mico_system_context_get();
    require( in_context, exit );

    micoWlanPowerOn();
#if (MICO_WLAN_CONFIG_MODE == CONFIG_MODE_EASYLINK)
    err = mico_easylink( in_context, MICO_TRUE, MICO_FALSE );
#elif (MICO_WLAN_CONFIG_MODE == CONFIG_MODE_EASYLINK_WITH_SOFTAP)
    err = mico_easylink( in_context, MICO_TRUE, MICO_TRUE );
#elif ( MICO_WLAN_CONFIG_MODE == CONFIG_MODE_SOFTAP)
    err = mico_easylink_softap( in_context, MICO_TRUE );
#elif ( MICO_WLAN_CONFIG_MODE == CONFIG_MODE_MONITOR)
    err = mico_easylink_monitor( in_context, MICO_TRUE );
#elif ( MICO_WLAN_CONFIG_MODE == CONFIG_MODE_MONITOR_EASYLINK)
    err = mico_easylink_monitor_with_easylink( in_context, MICO_TRUE );
#elif ( MICO_WLAN_CONFIG_MODE == CONFIG_MODE_USER)
    err = mico_easylink_usr( in_context, MICO_TRUE );
#elif ( MICO_WLAN_CONFIG_MODE == CONFIG_MODE_WAC)
    err = mico_easylink_wac( in_context, MICO_TRUE );
#elif ( MICO_WLAN_CONFIG_MODE == CONFIG_MODE_NONE)
#else
    #error "Wi-Fi configuration mode is not defined"
#endif
    require_noerr( err, exit );
exit:
    return err;
}

OSStatus mico_system_wlan_start_autoconf( void )
{
  /* Enter auto-conf mode only once in reboot mode, use MICO_NETWORKING_WORKER_THREAD to save ram */
#ifdef  EasyLink_Needs_Reboot
    return mico_rtos_send_asynchronous_event( MICO_NETWORKING_WORKER_THREAD, system_config_mode_worker, NULL );
#else
    return mico_rtos_send_asynchronous_event( &wlan_autoconf_worker_thread, system_config_mode_worker, NULL );
#endif
}


OSStatus mico_system_init( mico_Context_t* in_context )
{
  OSStatus err = kNoErr;

  require_action( in_context, exit, err = kNotPreparedErr );

  /* Initialize mico notify system */
  err = system_notification_init( sys_context );
  require_noerr( err, exit ); 

#ifdef MICO_SYSTEM_MONITOR_ENABLE
  /* MiCO system monitor */
  err = mico_system_monitor_daemen_start( );
  require_noerr( err, exit ); 
#endif

#ifdef MICO_CLI_ENABLE
  /* MiCO command line interface */
  cli_init();
#endif

  /* Network PHY driver and tcp/ip stack init */
  err = system_network_daemen_start( sys_context );
  require_noerr( err, exit ); 

#ifdef MICO_WLAN_CONNECTION_ENABLE
#ifndef  EasyLink_Needs_Reboot
  /* Create a worker thread for user handling wlan auto-conf event, this worker thread only has
     one event on queue, avoid some unwanted operation */
  err = mico_rtos_create_worker_thread( &wlan_autoconf_worker_thread, MICO_APPLICATION_PRIORITY, 0x500, 1 );
  require_noerr_string( err, exit, "ERROR: Unable to start the autoconf worker thread." );
#endif

  if( sys_context->flashContentInRam.micoSystemConfig.configured == unConfigured){
    system_log("Empty configuration. Starting configuration mode...");
    err = mico_system_wlan_start_autoconf( );
    require_noerr( err, exit );
  }
#ifdef EasyLink_Needs_Reboot
  else if( sys_context->flashContentInRam.micoSystemConfig.configured == wLanUnConfigured ){
      system_log("Re-config wlan configuration. Starting configuration mode...");
      err = mico_system_wlan_start_autoconf( );
      require_noerr( err, exit );
  }
#endif

#ifdef MFG_MODE_AUTO
  else if( sys_context->flashContentInRam.micoSystemConfig.configured == mfgConfigured ){
    system_log( "Enter MFG mode automatically" );
    mico_mfg_test( in_context );
    mico_thread_sleep( MICO_NEVER_TIMEOUT );
  }
#endif
  else{
    system_log("Available configuration. Starting Wi-Fi connection...");
    system_connect_wifi_fast( sys_context );
  }
#endif
  
  /* System discovery */
#ifdef MICO_SYSTEM_DISCOVERY_ENABLE
  system_discovery_init( sys_context );
#endif

  /*Local configuration server*/
#ifdef MICO_CONFIG_SERVER_ENABLE
  config_server_start( );
#endif
  
#ifdef AIRKISS_DISCOVERY_ENABLE
  err = airkiss_discovery_start( AIRKISS_APP_ID, AIRKISS_DEVICE_ID );
  require_noerr( err, exit );
#endif

exit:
  return err;
}


