/**
 ******************************************************************************
 * @file    platform_spi.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide SPI driver functions.
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

OSStatus platform_spi_init( platform_spi_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_config_t* config )
{
  UNUSED_PARAMETER(driver);
  UNUSED_PARAMETER(peripheral);
  UNUSED_PARAMETER(config);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_spi_deinit( platform_spi_driver_t* driver )
{
  UNUSED_PARAMETER(driver);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_spi_transfer( platform_spi_driver_t* driver, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
  UNUSED_PARAMETER(driver);
  UNUSED_PARAMETER(config);
  UNUSED_PARAMETER(segments);
  UNUSED_PARAMETER(number_of_segments);
  platform_log("unimplemented");
  return kUnsupportedErr;
}



