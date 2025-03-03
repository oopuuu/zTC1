/**
 ******************************************************************************
 * @file    MicoDriverWdg.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide WDG driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */


#include "mico_platform.h"
#include "mico_rtos.h"
#include "common.h"
#include "debug.h"

#include "platform.h"
#include "platform_peripheral.h"

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
 *               Variables Definitions
 ******************************************************/
/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
/* Wecan Chen Modify 2015.06.16 */
OSStatus platform_watchdog_init( uint32_t timeout_ms )
{
// PLATFORM_TO_DO
#ifndef MICO_DISABLE_WATCHDOG
  OSStatus err = kNoErr;
  uint32_t reloadTick;	// Magicoe change to Uint32_t
  uint32_t wdtFreq;

  /* Enable the power to the WDT Oscillator */
  Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_WDT_OSC);

  /* The WDT divides the input frequency into it by 4 */
  wdtFreq = Chip_Clock_GetWDTOSCRate() / 4;

  reloadTick = wdtFreq*timeout_ms/1000;

  /* Initialize WWDT (also enables WWDT clock) */
	Chip_WWDT_Init(LPC_WWDT);

  /* Set counter reload value to obtain 250ms watchdog TimeOut. */
	Chip_WWDT_SetTimeOut(LPC_WWDT, reloadTick);

  /* Configure WWDT to reset on timeout */
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);

  /* Clear watchdog warning and timeout interrupts */
	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF|WWDT_WDMOD_WDINT);

  /* Start watchdog */
	Chip_WWDT_Start(LPC_WWDT);

exit:
  return err;
#else
  UNUSED_PARAMETER( timeout_ms );
  return kUnsupportedErr;
#endif
}

OSStatus MicoWdgFinalize( void )
{
    // PLATFORM_TO_DO
    Chip_WWDT_DeInit(LPC_WWDT);
    return kNoErr;
}

OSStatus platform_watchdog_kick( void )
{
#ifndef MICO_DISABLE_WATCHDOG
  Chip_WWDT_Feed(LPC_WWDT); // Magicoe  IWDG_ReloadCounter();
  return kNoErr;
#else
  return kUnsupportedErr;
#endif
}

bool platform_watchdog_check_last_reset( void )
{
#ifndef MICO_DISABLE_WATCHDOG
  if (Chip_WWDT_GetStatus(LPC_WWDT) & WWDT_WDMOD_WDTOF) {
    /* Clear the flag and return */
    Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);
    return true;
}
#endif

    return false;
}
