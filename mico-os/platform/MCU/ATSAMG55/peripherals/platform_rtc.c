/**
 ******************************************************************************
 * @file    platform_rtc.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide RTC driver functions.
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

#include "platform.h"
#include "platform_logging.h"
#include "platform_peripheral.h"

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
 *               Static Function Declarations
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

OSStatus platform_rtc_init(void)
{
  platform_log("unimplemented");
  return kUnsupportedErr;
}


/**
* This function will return the value of time read from the on board CPU real time clock. Time value must be given in the format of
* the structure mico_rtc_time_t
*
* @return    kNoErr : on success.
*/
OSStatus platform_rtc_get_time( platform_rtc_time_t* time)
{
  UNUSED_PARAMETER(time);
  return kUnsupportedErr;
}

/**
* This function will set MCU RTC time to a new value. Time value must be given in the format of
* the structure mico_rtc_time_t
*
* @return    kNoErr : on success.
*/
OSStatus platform_rtc_set_time( const mico_rtc_time_t* time )
{
  UNUSED_PARAMETER(time);
  return kUnsupportedErr;
}
















