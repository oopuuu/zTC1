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


#include "platform.h"
#include "platform_peripheral.h"
#include "debug.h"

/******************************************************
*                      Macros
******************************************************/

#define MICO_VERIFY_TIME(time, valid) \
if( (time->sec > 60) || ( time->min > 60 ) || (time->hr > 24) || ( time->date > 31 ) || ( time->month > 12 )) \
  { \
    valid= false; \
  } \
else \
  { \
    valid= true; \
  }

#define LEAP_YEAR_OR_NOT(year)( ( year % 4 ) ? ( 0 ) : ( 1 ) )

/******************************************************
*                    Constants
******************************************************/

#define LEAP_YEAR_DAY_COUNT         ( 366 )
#define NOT_LEAP_YEAR_DAY_COUNT     ( 365 )
#define NUM_SECONDS_IN_MINUTE       ( 60 )
#define NUM_SECONDS_IN_HOUR         ( 3600 )
#define NUM_1P25MS_IN_SEC           ( 800 )

/******************************************************
*                   Enumerations
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
	hal_rtc_init();
	return kNoErr;
}


/**
 * Get current real-time clock
 *
 * @param[in] time : variable that will contain the current real-time clock
 *
 * @return @ref OSStatus
 */
OSStatus platform_rtc_get_time( platform_rtc_time_t* time )
{
	hal_rtc_time_t rtc_time;
	hal_rtc_get_time(&rtc_time);
  time->year 		= rtc_time.rtc_year;
  time->month 	= rtc_time.rtc_mon;
  time->date 		= rtc_time.rtc_day;
  time->weekday = rtc_time.rtc_week;
  time->hr 			= rtc_time.rtc_hour;
  time->min 		= rtc_time.rtc_min;
  time->sec 		= rtc_time.rtc_sec;
	return kNoErr;
}


/**
 * Set real-time clock
 *
 * @param[in] time : real-time clock
 *
 * @return @ref OSStatus
 */
OSStatus platform_rtc_set_time( const platform_rtc_time_t* time )
{
	hal_rtc_time_t rtc_time;
  rtc_time.rtc_year = time->year;
  rtc_time.rtc_mon 	= time->month;
  rtc_time.rtc_day 	= time->date;
  rtc_time.rtc_week = time->weekday;
  rtc_time.rtc_hour = time->hr;
  rtc_time.rtc_min 	= time->min;
  rtc_time.rtc_sec 	= time->sec;
  hal_rtc_set_time(&rtc_time);
	return kNoErr;
}



















