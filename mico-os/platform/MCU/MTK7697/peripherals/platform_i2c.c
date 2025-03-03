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
#include "platform_logging.h"

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

/**
 * Initialise I2C interface
 *
 * @param[in] i2c_interface : I2C interface
 * @param[in] config        : I2C configuration
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
	hal_i2c_config_t i2c_init;
	hal_gpio_init(HAL_GPIO_27);
	hal_gpio_init(HAL_GPIO_28);
	hal_pinmux_set_function(HAL_GPIO_27,HAL_GPIO_27_I2C1_CLK);
	hal_pinmux_set_function(HAL_GPIO_28,HAL_GPIO_28_I2C1_DATA);
	i2c_init.frequency = config->speed_mode == I2C_LOW_SPEED_MODE ? HAL_I2C_FREQUENCY_50K
		: config->speed_mode == I2C_STANDARD_SPEED_MODE ? HAL_I2C_FREQUENCY_100K : HAL_I2C_FREQUENCY_400K;
	hal_i2c_master_init(HAL_I2C_MASTER_0,&i2c_init);
	return kNoErr;
}


/**
 * Deinitialise I2C interface
 *
 * @param[in] i2c_interface : I2C interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
	hal_i2c_master_deinit(HAL_I2C_MASTER_0);
	hal_gpio_deinit(HAL_GPIO_27);
	hal_gpio_deinit(HAL_GPIO_28);
	return kNoErr;
}


/**
 * Probe I2C slave device
 *
 * @param[in] i2c_interface : I2C interface
 * @param[in] retries       : number of retries
 *
 * @return @ref OSStatus
 */
bool platform_i2c_probe_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries )
{
	uint8_t dummy_byte = 0xAA;

	while(retries--){
		if(hal_i2c_master_send_polling(HAL_I2C_MASTER_0,config->address,&dummy_byte,1) == HAL_I2C_STATUS_OK){
			return true;
		}
	}
	return false;
}


/**
 * Initialise I2C transmit message
 *
 * @param[in,out] message          : I2C message
 * @param[in]     tx_buffer        : transmit buffer
 * @param[in]     tx_buffer_length : transmit buffer length is bytes
 * @param[in]     retries          : number of transmission retries
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries )
{
  memset(message, 0x00, sizeof(platform_i2c_message_t));
  message->tx_buffer = tx_buffer;
  message->retries = retries;
  message->tx_length = tx_buffer_length;

  return kNoErr;
}


/**
 * Initialise I2C receive message
 *
 * @param[in,out] message          : I2C message
 * @param[in]     rx_buffer        : receive buffer
 * @param[in]     rx_buffer_length : receive buffer length is bytes
 * @param[in]     retries          : number of transmission retries
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries )
{
  memset(message, 0x00, sizeof(platform_i2c_message_t));
  message->rx_buffer = rx_buffer;
  message->retries = retries;
  message->rx_length = rx_buffer_length;

  return kNoErr;
}


/**
 * Initialise I2C combined message
 *
 * @param[in,out] message          : I2C message
 * @param[in]     tx_buffer        : transmit buffer
 * @param[in]     rx_buffer        : receive buffer
 * @param[in]     tx_buffer_length : transmit buffer length is bytes
 * @param[in]     rx_buffer_length : receive buffer length is bytes
 * @param[in]     retries          : number of transmission retries
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_init_combined_message( platform_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries )
{
  memset(message, 0x00, sizeof(platform_i2c_message_t));
  message->rx_buffer = rx_buffer;
  message->tx_buffer = tx_buffer;
  message->retries = retries;
  message->tx_length = tx_buffer_length;
  message->rx_length = rx_buffer_length;

  return kNoErr;
}


/**
 * Transfer data via the I2C interface
 *
 * @param[in] i2c_interface      : I2C interface
 * @param[in] messages           : pointer to an array of messages to transceive
 * @param[in] number_of_messages : number of messages in the array
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages )
{
  for( uint8_t i=0; i<number_of_messages; i++ )
  {
  	platform_i2c_message_t *message = &messages[i];
    if ( message->tx_buffer != NULL )
    {
    	if(hal_i2c_master_send_polling(HAL_I2C_MASTER_0, config->address, messages->tx_buffer, messages->tx_length) != HAL_I2C_STATUS_OK)
    	{
    		return kTimeoutErr;
    	}
    }

    if ( message->rx_buffer != NULL )
    {
    	if(hal_i2c_master_receive_polling(HAL_I2C_MASTER_0, config->address, messages->rx_buffer, messages->rx_length) != HAL_I2C_STATUS_OK)
    	{
    		return kTimeoutErr;
    	}
    }
  }
	return kNoErr;
}




