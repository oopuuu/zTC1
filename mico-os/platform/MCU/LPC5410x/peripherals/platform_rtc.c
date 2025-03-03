/**
 ******************************************************************************
 * @file    MicoDriverRtc.c
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

#include "mico_platform.h"
#include "mico_rtos.h"

#include "platform.h"
#include "platform_peripheral.h"
#include "chip.h"   // rocky
#include "debug.h"
#include "StringUtils.h"


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
#define BASE_YEAR                   ( 2000 )

/******************************************************
*                   Enumerations
******************************************************/

typedef enum
{
    CLOCKING_EVERY_SEC,
    CLOCKING_EVERY_1p25MSEC
}rtc_clock_state_t;

extern const platform_gpio_t       platform_gpio_pins[];
/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/
// static OSStatus stm32f2_rtc_change_clock                   ( rtc_clock_state_t* current, rtc_clock_state_t target );
//static uint32_t convert_rtc_calendar_values_to_units_passed( const platform_rtc_time_t* rtc_read_date_time );
//static void convert_units_passed_to_rtc_calendar_values( const uint32_t seconds , platform_rtc_time_t *rtc_get_date_time );
//static uint8_t CaculateWeekDay( const platform_rtc_time_t* rtc_calendar );

/******************************************************
*               Variables Definitions
******************************************************/
mico_rtc_time_t default_rtc_time =
{
  /* set it to 12:20:30 08/04/2013 monday */
  .sec   = 00,
  .min   = 03,
  .hr    = 19,
  .weekday  = 1,
  .date  = 12,
  .month = 6,
  .year  = 15
};
//
//static const char not_leap_days[] =
//{
//    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
//};
//
//static const char leap_days[] =
//{
//    0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
//};

//static const platform_rtc_time_t* saved_rtc_time;
//static rtc_clock_state_t   current_clock_state  = CLOCKING_EVERY_SEC;

/******************************************************
*               Function Declarations
******************************************************/
#define rtc_log(M, ...) custom_log("RTC", M, ##__VA_ARGS__)
#define rtc_log_trace() custom_log_trace("RTC")

/******************************************************
*               Function Definitions
******************************************************/
/* Wecan Modify platform_rtc_init 2015.06.10 */
OSStatus platform_rtc_init(void)
{
#ifdef MICO_ENABLE_MCU_RTC
        /* CLKOUT = 32K_Osc */
	Chip_IOCON_PinMuxSet(LPC_IOCON, platform_gpio_pins[CLKOUT].port , platform_gpio_pins[CLKOUT].pin_number, (IOCON_MODE_PULLUP | IOCON_FUNC1 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF));
	Chip_Clock_SetCLKOUTSource(SYSCON_CLKOUTSRC_RTC, 1);

	/* Turn on the RTC 32K Oscillator */
	Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_32K_OSC);

	/* Enable the RTC oscillator, oscillator rate can be determined by calling Chip_Clock_GetRTCOscRate()	*/
	Chip_Clock_EnableRTCOsc();

	/* Initialize RTC driver (enables RTC clocking) */
	Chip_RTC_Init(LPC_RTC);

	/* Enable RTC as a peripheral wakeup event */
	//Chip_SYSCON_EnsableWakeup(SYSCON_STARTER_RTC);

	/* RTC reset */
	Chip_RTC_Reset(LPC_RTC);

	/* Start RTC at a count of 0 when RTC is disabled. If the RTC is enabled, you need to disable it before setting the initial RTC count. */
	Chip_RTC_Disable(LPC_RTC);
        platform_rtc_set_time(&default_rtc_time);

	//Chip_RTC_SetCount(LPC_RTC, 0);

	/* Set a long alarm time so the interrupt won't trigger */
	//Chip_RTC_SetAlarm(LPC_RTC, (Chip_RTC_GetCount(LPC_RTC) + 5));

	/* Enable RTC and high resolution timer - this can be done in a single call with Chip_RTC_EnableOptions(LPC_RTC, (RTC_CTRL_RTC1KHZ_EN | RTC_CTRL_RTC_EN)); */
	//Chip_RTC_Enable1KHZ(LPC_RTC);
	Chip_RTC_Enable(LPC_RTC);

	/* Clear latched RTC interrupt statuses */
	Chip_RTC_ClearStatus(LPC_RTC, (RTC_CTRL_OFD | RTC_CTRL_ALARM1HZ | RTC_CTRL_WAKE1KHZ));


  return kNoErr;
#else
  return kUnsupportedErr;
#endif
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* This function will return the value of time read from the on board CPU real time clock. Time value must be given in the format of
* the structure mico_rtc_time_t
*
* @return    kNoErr : on success.
*/
OSStatus platform_rtc_get_time( platform_rtc_time_t* time)
{
#ifdef MICO_ENABLE_MCU_RTC
  bool    valid = false;

  if( time == 0 )
  {
    return kParamErr;
  }

  /* get current rtc time */
  convert_units_passed_to_rtc_calendar_values( Chip_RTC_GetCount(LPC_RTC), time );

  MICO_VERIFY_TIME(time, valid);
  if( valid == false )
  {
    return kParamErr;
  }

#if 0
  rtc_log(" return convert_units_passed_to_rtc_calendar_values address = 0x%x ", temp_time);
  rtc_log(" sec     = %d ", temp_time->sec);
  rtc_log(" min     = %d ", temp_time->min);
  rtc_log(" hr      = %d ", temp_time->hr);
  rtc_log(" weekday = %d ", temp_time->weekday);
  rtc_log(" date    = %d ", temp_time->date);
  rtc_log(" month   = %d ", temp_time->month);
  rtc_log(" year    = %d ", temp_time->year);
#endif

  return kNoErr;
#else /* #ifdef MICO_ENABLE_MCU_RTC */
  UNUSED_PARAMETER(time);
  return kUnsupportedErr;
#endif /* #ifdef MICO_ENABLE_MCU_RTC */
}

/**
* This function will set MCU RTC time to a new value. Time value must be given in the format of
* the structure mico_rtc_time_t
*
* @return    kNoErr : on success.
*/
OSStatus platform_rtc_set_time( const mico_rtc_time_t* time )
{
#ifdef MICO_ENABLE_MCU_RTC
  bool    valid = false;

  MICO_VERIFY_TIME(time, valid);
  if( valid == false )
  {
    return kParamErr;
  }

  Chip_RTC_SetCount(LPC_RTC, convert_rtc_calendar_values_to_units_passed( time ));

	return kNoErr;
#else /* #ifdef MICO_ENABLE_MCU_RTC */

	UNUSED_PARAMETER(time);
	return kUnsupportedErr;
#endif /* #ifdef MICO_ENABLE_MCU_RTC */
}

//
//static void convert_units_passed_to_rtc_calendar_values( const uint32_t seconds , platform_rtc_time_t *rtc_get_date_time )
//{
//    long int        temp1=0;
//    int             temp=0;
//    long int        temp_days=0;
//    bool MonthFound = false;
//    bool YearFound = false;
//
//    rtc_log_trace();
//
//    /* Convert seconds passed to hours, seconds and minutes */
//    rtc_get_date_time->sec = seconds % NUM_SECONDS_IN_MINUTE;
//    rtc_get_date_time->min = ( seconds / NUM_SECONDS_IN_MINUTE )% NUM_SECONDS_IN_MINUTE;
//    rtc_get_date_time->hr  = ( seconds / NUM_SECONDS_IN_HOUR ) % 24;
//
//    /* years */
//    temp_days = ( seconds / NUM_SECONDS_IN_HOUR ) / 24;
//    rtc_get_date_time->year = 0;
//    while ( YearFound == false ) {
//      temp1 = (LEAP_YEAR_OR_NOT(temp)) ? (LEAP_YEAR_DAY_COUNT): (NOT_LEAP_YEAR_DAY_COUNT);
//      if ( temp_days >= temp1 ) {
//        temp_days -= temp1;
//        temp++;
//      }
//      else {
//        YearFound = true;
//      }
//    }
//    rtc_get_date_time->year = temp;
//
//    /* Month */
//    temp = 1;
//    rtc_get_date_time->month = 1;
//    /* Determine offset of months from days offset */
//    while ( MonthFound == false ) {
//      temp1 = LEAP_YEAR_OR_NOT(rtc_get_date_time->year )?(leap_days[temp]):(not_leap_days[temp]);
//      if(( temp_days + 1 ) > temp1) {
//        temp_days -= temp1;
//	temp++;
//      }
//      else {
//        MonthFound = true;
//      }
//    }
//    rtc_get_date_time->month = temp;
//
//    /* date */
//    rtc_get_date_time->date = temp_days + 1;
//
//    /* WeekDay */
//    rtc_get_date_time->weekday = CaculateWeekDay( rtc_get_date_time ) + 1;
//
//#if 0
//    rtc_log(" sec       = %d ", rtc_get_date_time->sec);
//    rtc_log(" min       = %d ", rtc_get_date_time->min);
//    rtc_log(" hr        = %d ", rtc_get_date_time->hr);
//    rtc_log(" weekday   = %d ", rtc_get_date_time->weekday);
//    rtc_log(" date      = %d ", rtc_get_date_time->date);
//    rtc_log(" month     = %d ", rtc_get_date_time->month);
//    rtc_log(" year      = %d ", rtc_get_date_time->year);
//    rtc_log(" rtc_get_date_time   = 0x%x ", rtc_get_date_time);
//#endif
//}
//
//static uint8_t CaculateWeekDay( const platform_rtc_time_t* rtc_calendar )
//{
//  uint16_t year;
//  uint8_t  month, date, weekday;
//
//  year  = rtc_calendar->year + BASE_YEAR;
//  month = rtc_calendar->month;
//  date  = rtc_calendar->date;
//  if ( month == 1 || month == 2 )
//     {
//         month+=12;
//         year--;
//     }
//     weekday = ( date + 2 * month + 3 * ( month + 1 ) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
//     return weekday;
//}



















