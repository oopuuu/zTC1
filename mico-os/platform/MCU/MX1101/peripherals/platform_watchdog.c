/**
 ******************************************************************************
 * @file    platform_watchdog.c
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

#include "platform.h"
#include "platform_config.h"
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

OSStatus platform_watchdog_init( uint32_t timeout_ms )
{
// PLATFORM_TO_DO
#ifdef MICO_DISABLE_WATCHDOG
  return kUnsupportedErr;
#else
  OSStatus err = kNoErr;
  require_action( timeout_ms < 4000 && timeout_ms > 0, exit, err = kParamErr );

  if ( timeout_ms > 3000 )
  	WdgEn(WDG_STEP_4S);
  else if ( timeout_ms > 1000 )
  	WdgEn(WDG_STEP_3S);
  else
  	WdgEn(WDG_STEP_1S);

exit:
	return err;
#endif
}

OSStatus platform_watchdog_deinit( void )
{
    // PLATFORM_TO_DO
    WdgDis();
    return kNoErr;
}

bool platform_watchdog_check_last_reset( void )
{
    return false;
}

OSStatus platform_watchdog_kick( void )
{
#ifndef MICO_DISABLE_WATCHDOG
	WdgFeed();
	return kNoErr;
#else
	return kNoErr;
#endif
}


