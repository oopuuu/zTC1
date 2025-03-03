/**
 ******************************************************************************
 * @file    platform_spi_slave.c
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

OSStatus platform_spi_slave_init( platform_spi_slave_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_slave_config_t* config )
{
  UNUSED_PARAMETER(driver);
  UNUSED_PARAMETER(peripheral);
  UNUSED_PARAMETER(config);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_spi_slave_deinit( platform_spi_slave_driver_t* driver )
{
  UNUSED_PARAMETER( driver );
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_spi_slave_receive_command( platform_spi_slave_driver_t* driver, platform_spi_slave_command_t* command, uint32_t timeout_ms )
{
  UNUSED_PARAMETER( driver );
  UNUSED_PARAMETER( command );
  UNUSED_PARAMETER( timeout_ms );
  platform_log("unimplemented");
  return kUnsupportedErr;
}


OSStatus platform_spi_slave_transfer_data( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_direction_t direction, platform_spi_slave_data_buffer_t* buffer, uint32_t timeout_ms )
{
  UNUSED_PARAMETER(driver);
  UNUSED_PARAMETER(direction);
  UNUSED_PARAMETER(buffer);
  UNUSED_PARAMETER(timeout_ms);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_spi_slave_send_error_status( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_status_t error_status )
{
  UNUSED_PARAMETER(driver);
  UNUSED_PARAMETER(error_status);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_spi_slave_generate_interrupt( platform_spi_slave_driver_t* driver, uint32_t pulse_duration_ms )
{
  UNUSED_PARAMETER(driver);
  UNUSED_PARAMETER(pulse_duration_ms);
  platform_log("unimplemented");
  return kUnsupportedErr;
}


