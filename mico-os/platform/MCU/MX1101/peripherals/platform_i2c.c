/**
 ******************************************************************************
 * @file    platform_i2c.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide I2C driver functions.
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

OSStatus platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
  UNUSED_PARAMETER(i2c);
  UNUSED_PARAMETER(config);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

bool platform_i2c_probe_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries )
{
  UNUSED_PARAMETER(i2c);
  UNUSED_PARAMETER(config);
  UNUSED_PARAMETER(retries);
  platform_log("unimplemented");
  return false;
}

OSStatus platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries )
{
  UNUSED_PARAMETER(message);
  UNUSED_PARAMETER(tx_buffer);
  UNUSED_PARAMETER(tx_buffer_length);
  UNUSED_PARAMETER(retries);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries )
{
  UNUSED_PARAMETER(message);
  UNUSED_PARAMETER(rx_buffer);
  UNUSED_PARAMETER(rx_buffer_length);
  UNUSED_PARAMETER(retries);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_i2c_init_combined_message( platform_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries )
{
  UNUSED_PARAMETER(message);
  UNUSED_PARAMETER(tx_buffer);
  UNUSED_PARAMETER(tx_buffer_length);
  UNUSED_PARAMETER(rx_buffer);
  UNUSED_PARAMETER(rx_buffer_length);
  UNUSED_PARAMETER(retries);
  platform_log("unimplemented");
  return kUnsupportedErr;
}


OSStatus platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages )
{
  UNUSED_PARAMETER(i2c);
  UNUSED_PARAMETER(config);
  UNUSED_PARAMETER(messages);
  UNUSED_PARAMETER(number_of_messages);
  platform_log("unimplemented");
  return kUnsupportedErr;
}


OSStatus platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
  UNUSED_PARAMETER(i2c);
  UNUSED_PARAMETER(config);
  platform_log("unimplemented");
  return kUnsupportedErr;
}


