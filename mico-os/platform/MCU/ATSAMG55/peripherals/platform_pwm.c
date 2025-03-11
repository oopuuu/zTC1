/**
 ******************************************************************************
 * @file    platform_pwm.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide PWM driver functions.
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

OSStatus platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle )
{
  UNUSED_PARAMETER(pwm);
  UNUSED_PARAMETER(frequency);
  UNUSED_PARAMETER(duty_cycle);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_pwm_start( const platform_pwm_t* pwm )
{
  UNUSED_PARAMETER(pwm);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_pwm_stop( const platform_pwm_t* pwm )
{
  UNUSED_PARAMETER(pwm);
  platform_log("unimplemented");
  return kUnsupportedErr;
}




