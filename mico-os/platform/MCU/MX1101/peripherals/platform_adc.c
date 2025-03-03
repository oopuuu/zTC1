/**
 ******************************************************************************
 * @file    platform_adc.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide ADC driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
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



OSStatus platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle )
{
  UNUSED_PARAMETER(adc);
  UNUSED_PARAMETER(sample_cycle);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output )
{
  UNUSED_PARAMETER(output);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length )
{
  UNUSED_PARAMETER(adc);
  UNUSED_PARAMETER(buffer);
  UNUSED_PARAMETER(buffer_length);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_adc_deinit( const platform_adc_t* adc )
{
  UNUSED_PARAMETER(adc);
  platform_log("unimplemented");
  return kUnsupportedErr;
}
