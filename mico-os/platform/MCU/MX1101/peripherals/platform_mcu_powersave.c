/**
 ******************************************************************************
 * @file    platform_mcu_powersave.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide functions called by MICO to drive stm32f2xx
 *          platform: - e.g. power save, reboot, platform initialize
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */


#include "platform_peripheral.h"
#include "platform.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
#include "crt0.h"
#include "platform_init.h"

/******************************************************
*                      Macros
******************************************************/

/******************************************************
*                    Constants
******************************************************/

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Function Declarations
******************************************************/
#ifndef MICO_DISABLE_MCU_POWERSAVE
static unsigned long  stop_mode_power_down_hook( unsigned long sleep_ms );
#else
static unsigned long  idle_power_down_hook( unsigned long sleep_ms );
#endif

#define ENABLE_INTERRUPTS   __asm("CPSIE i")  /**< Enable interrupts to start task switching in MICO RTOS. */
#define DISABLE_INTERRUPTS  __asm("CPSID i")  /**< Disable interrupts to stop task switching in MICO RTOS. */

/******************************************************
*               Variables Definitions
******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus platform_mcu_powersave_init(void)
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
  #error "Current platform is unsupported"?
#else
  return kNoErr;
#endif
}

OSStatus platform_mcu_powersave_disable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
  #error "Current platform is unsupported"?
#else
  return kNoErr;
#endif
}

OSStatus platform_mcu_powersave_enable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
  #error "Current platform is unsupported"?
#else
  return kNoErr;
#endif
}

void platform_mcu_powersave_exit_notify( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
  #error "Current platform is unsupported"?
#endif
}

/******************************************************
 *               RTOS Powersave Hooks
 ******************************************************/

void platform_idle_hook( void )
{
    __asm("wfi");
}

uint32_t platform_power_down_hook( uint32_t sleep_ms )
{
#ifdef MICO_DISABLE_MCU_POWERSAVE
    /* If MCU powersave feature is disabled, enter idle mode when powerdown hook is called by the RTOS */
    return idle_power_down_hook( sleep_ms );

#else
    #error "Current platform is unsupported"
    /* If MCU powersave feature is enabled, enter STOP mode when powerdown hook is called by the RTOS */
    return stop_mode_power_down_hook( sleep_ms );

#endif
}

#ifdef MICO_DISABLE_MCU_POWERSAVE
/* MCU Powersave is disabled */
static unsigned long idle_power_down_hook( unsigned long sleep_ms  )
{
    UNUSED_PARAMETER( sleep_ms );
    ENABLE_INTERRUPTS;;
    __asm("wfi");
    return 0;
}
#else
static unsigned long stop_mode_power_down_hook( unsigned long sleep_ms )
{
    UNUSED_PARAMETER( sleep_ms );
    ENABLE_INTERRUPTS;;
    __asm("wfi");
    return 0;
}
#endif /* MICO_DISABLE_MCU_POWERSAVE */


#define USE_DEEPSLEEP_MODE

extern uint32_t RtcGetRefCnt(void);
extern void RtcSetAlarmCnt(uint32_t AlarmCnt);

void platform_mcu_enter_standby(uint32_t secondsToWakeup)
{
  SysGetWakeUpFlag();             //get wake up flag, DO NOT remove this!!

  SysClrWakeUpSrc(WAKEUP_SRC_PD_POWERKEY);

  SysClrWakeUpSrc(WAKEUP_SRC_PD_RTC);
  
  SysPowerKeyInit(POWERKEY_MODE_SLIDE_SWITCH, 300); 

  if(secondsToWakeup != MICO_WAIT_FOREVER){
    RtcSetAlarmCnt( RtcGetRefCnt()+secondsToWakeup );
  }

#ifdef USE_DEEPSLEEP_MODE
  SysSetWakeUpSrcInDeepSleep(WAKEUP_SRC_SLEEP_RTC, WAKEUP_POLAR_POWERKEY_LOW, 1);
  SysGotoDeepSleep();
#else
  SysSetWakeUpSrcInPowerDown(WAKEUP_SRC_PD_RTC);
  SysGotoPowerDown();
#endif
  
  

}

/******************************************************
 *         IRQ Handlers Definition & Mapping
 ******************************************************/

