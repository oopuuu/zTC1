/**
 ******************************************************************************
 * @file    stm32f2xx_platform.c
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
#include "mico_rtos.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
#include "crt0.h"
#include "platform_init.h"

#include "power_control.h"

/******************************************************
*                      Macros
******************************************************/

#define NUMBER_OF_LSE_TICKS_PER_MILLISECOND(scale_factor) ( 32768 / 1000 / scale_factor )
#define CONVERT_FROM_TICKS_TO_MS( n,s )                   ( n / NUMBER_OF_LSE_TICKS_PER_MILLISECOND(s) )

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
#else
static unsigned long  idle_power_down_hook( unsigned long sleep_ms );
#endif
//void wake_up_interrupt_notify( void );


/******************************************************
*               Variables Definitions
******************************************************/


/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus platform_mcu_powersave_init(void)
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
#if 0 // Magicoe
EXTI_InitTypeDef EXTI_InitStructure;
  RTC_InitTypeDef RTC_InitStruct;

  //RTC_DeInit( );

  RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;

  /* RTC ticks every second */
  RTC_InitStruct.RTC_AsynchPrediv = 0x7F;
  RTC_InitStruct.RTC_SynchPrediv = 0xFF;

  RTC_Init( &RTC_InitStruct );

  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* RTC clock source configuration ------------------------------------------*/
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
  PWR_BackupRegulatorCmd(ENABLE);

  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);


  /* RTC configuration -------------------------------------------------------*/
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  RTC_WakeUpCmd( DISABLE );
  EXTI_ClearITPendingBit( RTC_INTERRUPT_EXTI_LINE );
  PWR_ClearFlag(PWR_FLAG_WU);
  RTC_ClearFlag(RTC_FLAG_WUTF);

  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);

  EXTI_ClearITPendingBit( RTC_INTERRUPT_EXTI_LINE );
  EXTI_InitStructure.EXTI_Line = RTC_INTERRUPT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_EnableIRQ( RTC_WKUP_IRQn );

  RTC_ITConfig(RTC_IT_WUT, DISABLE);

  /* Prepare Stop-Mode but leave disabled */
  //PWR_ClearFlag(PWR_FLAG_WU);
  PWR->CR  |= PWR_CR_LPDS;
  PWR->CR  &= (unsigned long)(~(PWR_CR_PDDS));
  SCB->SCR |= ((unsigned long)SCB_SCR_SLEEPDEEP_Msk);

#ifdef USE_RTC_BKP
  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != USE_RTC_BKP) {
    /* set it to 12:20:30 08/04/2013 monday */
    platform_rtc_set_time(&default_rtc_time);
    RTC_WriteBackupRegister(RTC_BKP_DR0, USE_RTC_BKP);
  }
#else
  //#ifdef RTC_ENABLED
  /* application must have mico_application_default_time structure declared somewhere, otherwise it wont compile */
  /* write default application time inside rtc */
  platform_rtc_set_time( &default_rtc_time );
  //#endif /* RTC_ENABLED */
#endif
#endif

  return kNoErr;
#else
  return kUnsupportedErr;
#endif
}

OSStatus platform_mcu_powersave_disable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
    disable_interrupts();
#if 0 // Magicoe
    if ( stm32f2_clock_needed_counter <= 0 )
    {
        SCB->SCR &= (~((unsigned long)SCB_SCR_SLEEPDEEP_Msk));
        stm32f2_clock_needed_counter = 0;
    }
    stm32f2_clock_needed_counter++;
#endif
    enable_interrupts();
    return kNoErr;
#else
    return kUnsupportedErr;
#endif
}


OSStatus platform_mcu_powersave_enable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
//    disable_interrupts;
//	#if 0 // Magicoe
//		stm32f2_clock_needed_counter--;
//	    if ( stm32f2_clock_needed_counter <= 0 )
//	    {
//	        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
//	        stm32f2_clock_needed_counter = 0;
//	    }
//	#endif
//    enable_interrupts;

    return kNoErr;
#else
    return kUnsupportedErr;
#endif
}

void platform_mcu_powersave_exit_notify( void )
{
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

    UNUSED_PARAMETER( sleep_ms );
    // ENABLE_INTERRUPTS;;
    // __asm("wfi");
    return 0;
}

#ifdef MICO_DISABLE_MCU_POWERSAVE
/* MCU Powersave is disabled */
static unsigned long idle_power_down_hook( unsigned long sleep_ms  )
{
    UNUSED_PARAMETER( sleep_ms );
    MICO_ENABLE_INTERRUPTS( );
    __asm("wfi");
    return 0;
}
#endif /* MICO_DISABLE_MCU_POWERSAVE */


void platform_mcu_enter_standby(uint32_t secondsToWakeup)
{
  platform_log("unimplemented");
  return;
}


/******************************************************
 *         IRQ Handlers Definition & Mapping
 ******************************************************/

#ifndef MICO_DISABLE_MCU_POWERSAVE
#if 0 // Magioce
MICO_RTOS_DEFINE_ISR( RTC_WKUP_irq )
{
    EXTI_ClearITPendingBit( RTC_INTERRUPT_EXTI_LINE );
}
#endif
#endif

