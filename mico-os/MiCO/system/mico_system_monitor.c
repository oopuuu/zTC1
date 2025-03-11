/**
 ******************************************************************************
 * @file    mico_system_monitor.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   System monitor functions, create a system monitor thread, and developer
 *          can add own monitor items.
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

#define DEFAULT_SYSTEM_MONITOR_PERIOD   (2000)

#ifndef MAXIMUM_NUMBER_OF_SYSTEM_MONITORS
#define MAXIMUM_NUMBER_OF_SYSTEM_MONITORS    (5)
#endif

#define APPLICATION_WATCHDOG_TIMEOUT_SECONDS  5 /**< Monitor point defined by mico system
                                                     5 seconds to reload. */

static mico_system_monitor_t* system_monitors[MAXIMUM_NUMBER_OF_SYSTEM_MONITORS];
void mico_system_monitor_thread_main( uint32_t arg );

OSStatus MICOStartSystemMonitor ( void )
{
  OSStatus err = kNoErr;
  require_noerr(MicoWdgInitialize( DEFAULT_SYSTEM_MONITOR_PERIOD + 1000 ), exit);
  memset(system_monitors, 0, sizeof(system_monitors));

  err = mico_rtos_create_thread(NULL, 0, "SYS MONITOR", mico_system_monitor_thread_main, STACK_SIZE_mico_system_MONITOR_THREAD, 0 );
  require_noerr(err, exit);
exit:
  return err;
}

void mico_system_monitor_thread_main( uint32_t arg )
{
  (void)arg;
  
  while (1)
  {
    int a;
    uint32_t current_time = mico_rtos_get_time();
    
    for (a = 0; a < MAXIMUM_NUMBER_OF_SYSTEM_MONITORS; ++a)
    {
      if (system_monitors[a] != NULL)
      {
        if ((current_time - system_monitors[a]->last_update) > system_monitors[a]->longest_permitted_delay)
        {
          /* A system monitor update period has been missed */
          while(1);
        }
      }
    }
    
    MicoWdgReload();
    mico_thread_msleep(DEFAULT_SYSTEM_MONITOR_PERIOD);
  }
}

OSStatus mico_system_monitor_register(mico_system_monitor_t* system_monitor, uint32_t initial_permitted_delay)
{
  int a;
  
  /* Find spare entry and add the new system monitor */
  for ( a = 0; a < MAXIMUM_NUMBER_OF_SYSTEM_MONITORS; ++a )
  {
    if (system_monitors[a] == NULL)
    {
      system_monitor->last_update = mico_rtos_get_time();
      system_monitor->longest_permitted_delay = initial_permitted_delay;
      system_monitors[a] = system_monitor;
      return kNoErr;
    }
  }
  
  return kUnknownErr;
}

OSStatus mico_system_monitor_update(mico_system_monitor_t* system_monitor, uint32_t permitted_delay)
{
  uint32_t current_time = mico_rtos_get_time();
  /* Update the system monitor if it hasn't already passed it's permitted delay */
  if ((current_time - system_monitor->last_update) <= system_monitor->longest_permitted_delay)
  {
    system_monitor->last_update             = current_time;
    system_monitor->longest_permitted_delay = permitted_delay;
  }
  
  return kNoErr;
}

static mico_timer_t _watchdog_reload_timer;

static mico_system_monitor_t mico_monitor;

static void _watchdog_reload_timer_handler( void* arg )
{
  (void)(arg);
  mico_system_monitor_update(&mico_monitor, APPLICATION_WATCHDOG_TIMEOUT_SECONDS*1000);
}


OSStatus mico_system_monitor_daemen_start( void )
{
  OSStatus err = kNoErr;
  /*Start system monotor thread*/
  err = MICOStartSystemMonitor( );
  require_noerr_string( err, exit, "ERROR: Unable to start the system monitor." );

  /* Register first monitor */
  err = mico_system_monitor_register(&mico_monitor, APPLICATION_WATCHDOG_TIMEOUT_SECONDS*1000);
  require_noerr( err, exit );
  mico_init_timer(&_watchdog_reload_timer,APPLICATION_WATCHDOG_TIMEOUT_SECONDS*1000/2, _watchdog_reload_timer_handler, NULL);
  mico_start_timer(&_watchdog_reload_timer);  
exit:
  return err;
}








