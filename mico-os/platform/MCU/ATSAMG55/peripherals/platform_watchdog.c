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
#include "platform_logging.h"
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
  UNUSED_PARAMETER( timeout_ms );
  return kUnsupportedErr;
}

OSStatus MicoWdgFinalize( void )
{
    // PLATFORM_TO_DO
    return kNoErr;
}

OSStatus platform_watchdog_kick( void )
{
  return kUnsupportedErr;
}

bool platform_watchdog_check_last_reset( void )
{
  return false;
}
