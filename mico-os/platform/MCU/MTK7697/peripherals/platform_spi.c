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
#include "platform_config.h"
#include "platform_peripheral.h"
#include "debug.h"
#include "hal_spi_master.h"

/******************************************************
*                    Constants
******************************************************/
#define MAX_NUM_SPI_PRESCALERS     (8)

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
*               Function Definitions
******************************************************/

OSStatus platform_spi_init( platform_spi_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_config_t* config )
{
	driver->peripheral = (platform_spi_t *)peripheral;

  hal_gpio_init(HAL_GPIO_29);
  hal_gpio_init(HAL_GPIO_30);
  hal_gpio_init(HAL_GPIO_31);
  hal_gpio_init(HAL_GPIO_32);

  /* Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool hasn't been used to configure the related pinmux */
  hal_pinmux_set_function(HAL_GPIO_29, HAL_GPIO_29_SPI_MOSI_M_CM4);
  hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_SPI_MISO_M_CM4);
  hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_SPI_SCK_M_CM4);
  hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_GPIO32);

  /* Init SPI master */
  hal_spi_master_config_t spi_config;
  if(config->mode & SPI_MSB_FIRST){
  	spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
  }else{
  	spi_config.bit_order = HAL_SPI_MASTER_LSB_FIRST;
  }
  spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
  spi_config.clock_frequency = config->speed;

  if(config->mode & SPI_CLOCK_RISING_EDGE){
  	if(config->mode & SPI_CLOCK_IDLE_HIGH){
  		spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE1;
  		spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY1;
  	}else{
  		spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
  		spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
  	}
  }else{
  	if(config->mode & SPI_CLOCK_IDLE_HIGH){
  		spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
  		spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY1;
  	}else{
  		spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE1;
  		spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
  	}
  }

  hal_spi_master_init(HAL_SPI_MASTER_0, &spi_config);

  return kNoErr;
}

OSStatus platform_spi_deinit( platform_spi_driver_t* driver )
{
  /* De-init spi master */
//	hal_spi_master_deinit(HAL_SPI_MASTER_0);
//  hal_gpio_deinit(HAL_GPIO_29);
//  hal_gpio_deinit(HAL_GPIO_30);
//  hal_gpio_deinit(HAL_GPIO_31);
//  hal_gpio_deinit(HAL_GPIO_32);
  return kNoErr;
}


OSStatus platform_spi_transfer( platform_spi_driver_t* driver, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
	if(segments->tx_buffer != NULL && segments->rx_buffer != NULL)
	{
		return kUnsupportedErr;
	}

	if(segments->tx_buffer != NULL)
	{
		for(uint8_t i=0; i<number_of_segments; i++)
		{
			hal_spi_master_send_polling(HAL_SPI_MASTER_0, (uint8_t *)segments[i].tx_buffer, segments[i].length);
		}
	}
	else
	{
		uint8_t dummy_send_data = 0xAA;
		hal_spi_master_send_and_receive_config_t spi_config;
		spi_config.send_data = &dummy_send_data;
		spi_config.send_length = 0;
		for(uint8_t i=0; i<number_of_segments; i++)
		{
			spi_config.receive_buffer = segments[i].rx_buffer;
			spi_config.receive_length = segments[i].length;
			hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_0, &spi_config);
		}
	}

	return kNoErr;
}









