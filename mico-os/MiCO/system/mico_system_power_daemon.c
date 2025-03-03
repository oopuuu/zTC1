/**
 ******************************************************************************
 * @file    mico_system_power_daemon.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide the power management daemon and provide a safety
 *          power operation to developer.
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
#include "system_internal.h"

static bool                     needs_update          = false;
static mico_timed_event_t _timed_sys_power_state_change;

extern system_context_t* sys_context;

extern void sendNotifySYSWillPowerOff(void);

USED void PlatformEasyLinkButtonClickedCallback(void)
{
  system_log_trace();
  
  require_quiet( sys_context, exit );
  
#ifdef EasyLink_Needs_Reboot
  if(sys_context->flashContentInRam.micoSystemConfig.easyLinkByPass != EASYLINK_BYPASS_NO){
      sys_context->flashContentInRam.micoSystemConfig.easyLinkByPass = EASYLINK_BYPASS_NO;
    needs_update = true;
  }
  
  /* Enter easylink mode temporary in configed mode */
  if(sys_context->flashContentInRam.micoSystemConfig.configured == allConfigured){
      sys_context->flashContentInRam.micoSystemConfig.configured = wLanUnConfigured;
    needs_update = true;
  }

  mico_system_power_perform( &sys_context->flashContentInRam, eState_Software_Reset );
#else
  mico_system_wlan_start_autoconf( );
#endif

exit: 
  return;
}

USED void PlatformEasyLinkButtonLongPressedCallback(void)
{
  system_log_trace();
  mico_Context_t* context = NULL;
  mico_logic_partition_t *partition = NULL;
  
  context = mico_system_context_get( );
  require( context, exit );

  partition = MicoFlashGetInfo( MICO_PARTITION_PARAMETER_1 );

  MicoFlashErase( MICO_PARTITION_PARAMETER_1 ,0x0, partition->partition_length );
  
  partition = MicoFlashGetInfo( MICO_PARTITION_PARAMETER_2 );

  MicoFlashErase( MICO_PARTITION_PARAMETER_2 ,0x0, partition->partition_length );

  mico_system_power_perform( context, eState_Software_Reset );

exit:
  return;
}

USED void PlatformStandbyButtonClickedCallback(void)
{
  system_log_trace();
  mico_Context_t* context = NULL;
  
  context = mico_system_context_get( );
  require( context, exit );
  
  mico_system_power_perform( context, eState_Standby );

exit: 
  return;
}

static OSStatus _sys_power_state_change_handler(void *arg)
{  
    
  switch( sys_context->micoStatus.current_sys_state )
  {
    case eState_Normal:
      break;
    case eState_Software_Reset:
      MicoSystemReboot( );
      break;
    case eState_Wlan_Powerdown:
      micoWlanPowerOff( );
      break;
    case eState_Standby:
      micoWlanPowerOff( );
      MicoSystemStandBy( MICO_WAIT_FOREVER );
      break;
    default:
      break;
  }
  return kNoErr;
}

static OSStatus _sys_will_power_off_handler(void *arg)
{
  OSStatus err = kNoErr;
  
  require_action( sys_context, exit, err = kNotPreparedErr );

  if(needs_update == true)
  {
    mico_system_context_update( &sys_context->flashContentInRam );
    needs_update = false;
  }

  mico_rtos_register_timed_event( &_timed_sys_power_state_change, MICO_NETWORKING_WORKER_THREAD, _sys_power_state_change_handler, 500, 0 );
  sendNotifySYSWillPowerOff();

exit:
  return err;
}


OSStatus mico_system_power_perform( mico_Context_t* const in_context, mico_system_state_t new_state )
{
  OSStatus err = kNoErr;

  require_action( sys_context, exit, err = kNotPreparedErr );

  sys_context->micoStatus.current_sys_state = new_state;

  /* Send an envent to do some operation before power off, and wait some time to perform power change */
  mico_rtos_send_asynchronous_event( MICO_NETWORKING_WORKER_THREAD, _sys_will_power_off_handler, NULL );

exit:
  return err; 
}







