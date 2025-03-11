/**
 ******************************************************************************
 * @file    platform_mcu_peripheral.c
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
#include "mico_platform.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
#include "crt0.h"
#include "mico_rtos.h"
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


/******************************************************
*               Variables Definitions
******************************************************/

#ifndef MICO_DISABLE_MCU_POWERSAVE
static bool          wake_up_interrupt_triggered  = false;
static volatile uint32_t     rtk_clock_needed_counter  = 0;
#endif /* #ifndef MICO_DISABLE_MCU_POWERSAVE */

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus platform_mcu_powersave_init(void)
{
    return kNoErr;
}

OSStatus platform_mcu_powersave_disable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
    /* Atomic operation starts */
    DISABLE_INTERRUPTS;

	if (rtk_clock_needed_counter <= 0) {
		acquire_wakelock(BIT(0));
    	rtk_clock_needed_counter = 0;
	}
	/* Increment counter to indicate CPU clock is needed preventing CPU from entering WAIT mode */
    rtk_clock_needed_counter++;
    /* Atomic operation ends */
    ENABLE_INTERRUPTS;
	
    return kNoErr;	
#else
    return kNoErr;
#endif
    return kNoErr;
}

OSStatus platform_mcu_powersave_enable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
    /* Atomic operation starts */
    DISABLE_INTERRUPTS;

    /* Decrement counter only if it's not 0 */
	rtk_clock_needed_counter--;
    if ( rtk_clock_needed_counter <= 0 )
    {
        release_wakelock(BIT(0));
        rtk_clock_needed_counter = 0;
    }
	
    /* Atomic operation ends */
    ENABLE_INTERRUPTS;
	
    return kNoErr;	
#else
    return kNoErr;
#endif
    return kNoErr;
}

void platform_mcu_powersave_exit_notify( void )
{
#if 0
#ifndef MICO_DISABLE_MCU_POWERSAVE
    wake_up_interrupt_triggered = true;
#endif
#endif
}

/******************************************************
 *               RTOS Powersave Hooks
 ******************************************************/

void platform_idle_hook( void )
{
#if 0
    __asm("wfi");
#endif
}

uint32_t platform_power_down_hook( uint32_t sleep_ms )
{
#if 0
#ifdef MICO_DISABLE_MCU_POWERSAVE
    /* If MCU powersave feature is disabled, enter idle mode when powerdown hook is called by the RTOS */
    return idle_power_down_hook( sleep_ms );

#else
    /* If MCU powersave feature is enabled, enter STOP mode when powerdown hook is called by the RTOS */
    return stop_mode_power_down_hook( sleep_ms );

#endif
#endif
	return 0;
}

#ifdef MICO_DISABLE_MCU_POWERSAVE
/* MCU Powersave is disabled */
static unsigned long idle_power_down_hook( unsigned long sleep_ms  )
{
#if 0
    UNUSED_PARAMETER( sleep_ms );
    ENABLE_INTERRUPTS;
    __asm("wfi");
#endif	
    return 0;
}
#else
static unsigned long stop_mode_power_down_hook( unsigned long sleep_ms )
{
  return 0;
  UNUSED_PARAMETER(sleep_ms);
  
  if (rtk_clock_needed_counter == 0 )
  {

    DISABLE_INTERRUPTS;
    
    SleepCG(SLP_STIMER, sleep_ms);
	
    /* as soon as interrupts are enabled, we will go and execute the interrupt handler */
    /* which triggered a wake up event */
    ENABLE_INTERRUPTS;
    wake_up_interrupt_triggered = false;
    return 0;
  }
  else
  {
    ENABLE_INTERRUPTS;
    __asm("wfi");
    
    /* Note: We return 0 ticks passed because system tick is still going when wfi instruction gets executed */
    return 0;
  }
}
#endif /* MICO_DISABLE_MCU_POWERSAVE */

// NOTICE: The pull condition may differnet on your board
PinName pull_down_list[] = {
    PA_0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7,
    PB_0,             PB_3, PB_4, PB_5, PB_6, PB_7,
    PC_0, PC_1, PC_2, PC_3, PC_4, PC_5, PC_6, PC_7, PC_8, PC_9,
    PD_0, PD_1, PD_2, PD_3, PD_4, PD_5, PD_6, PD_7, PD_8, PD_9,
    PE_0, PE_1, PE_2, PE_3, PE_4, PE_5, PE_6, PE_7, PE_8, PE_9, PE_A,
          PF_1, PF_2, PF_3, PF_4, PF_5
};

// NOTICE: The pull condition may differnet on your board
PinName pull_up_list[] = {
                PB_2,
    PF_0,
    PG_0, PG_1, PG_2, PG_3, PG_4, PG_5, PG_6, PG_7,
    PH_0, PH_1, PH_2, PH_3, PH_4, PH_5, PH_6, PH_7,
    PI_0, PI_1, PI_2, PI_3, PI_4, PI_5, PI_6, PI_7,
    PJ_0, PJ_1, PJ_2, PJ_3, PJ_4, PJ_5, PJ_6,
    PK_0, PK_1, PK_2, PK_3, PK_4, PK_5, PK_6
};

void gpio_pull_control()
{
    int i;
    gpio_t gpio_obj;

    for (i=0; i < sizeof(pull_down_list) / sizeof(pull_down_list[0]); i++) {
        gpio_init(&gpio_obj, pull_down_list[i]);
        gpio_dir(&gpio_obj, PIN_INPUT);
        gpio_mode(&gpio_obj, PullDown);
    }

    for (i=0; i < sizeof(pull_up_list) / sizeof(pull_up_list[0]); i++) {
        gpio_init(&gpio_obj, pull_up_list[i]);
        gpio_dir(&gpio_obj, PIN_INPUT);
        gpio_mode(&gpio_obj, PullUp);
    }
}

void platform_mcu_enter_standby(uint32_t secondsToWakeup)
{ 
  // turn off log uart
  sys_log_uart_off();
  gpio_pull_control();
  deepsleep_ex(BIT1 | BIT0, secondsToWakeup*1000);
}


/******************************************************
 *         IRQ Handlers Definition & Mapping
 ******************************************************/
#if 0
#ifndef WICED_DISABLE_MCU_POWERSAVE
MICO_RTOS_DEFINE_ISR( RTC_WKUP_irq )
{
    EXTI_ClearITPendingBit( RTC_INTERRUPT_EXTI_LINE );
}
#endif
#endif

