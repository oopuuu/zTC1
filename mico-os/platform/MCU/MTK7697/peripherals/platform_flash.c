/**
 ******************************************************************************
 * @file    platform_flash.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provides flash operation functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "platform_logging.h"
#include "platform_peripheral.h"
#include "platform.h"
#include "platform_config.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLOCK_SIZE_4K	(1024*4)
#define BLOCK_SIZE_32K	(1024*32)
#define BLOCK_SIZE_64K	(1024*64)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/**
 * Init flash driver and hardware interface
 *
 */
OSStatus platform_flash_init( const platform_flash_t *peripheral )
{
	return kNoErr;
}

/**
 * Erase flash
 *
 */
OSStatus platform_flash_erase( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address  )
{
#if 0
	if((end_address - start_address + 1) % BLOCK_SIZE_4K || (start_address != 0 && start_address % BLOCK_SIZE_4K)){
		return kParamErr;
	}
#endif
	if((end_address + 1) % BLOCK_SIZE_4K){
		//Force align to 4K
		end_address = ((end_address + 1) / BLOCK_SIZE_4K + 1) * BLOCK_SIZE_4K - 1;
	}
	while(1){
		if((end_address - start_address + 1) >= BLOCK_SIZE_64K && !(start_address != 0 && start_address % BLOCK_SIZE_64K)){
			hal_flash_erase(start_address, HAL_FLASH_BLOCK_64K);
			start_address += BLOCK_SIZE_64K;
		}else if((end_address - start_address + 1) >= BLOCK_SIZE_32K && !(start_address != 0 && start_address % BLOCK_SIZE_32K)){
			hal_flash_erase(start_address, HAL_FLASH_BLOCK_32K);
			start_address += BLOCK_SIZE_32K;
		}else if((end_address - start_address + 1) >= BLOCK_SIZE_4K && !(start_address != 0 && start_address % BLOCK_SIZE_4K)){
			hal_flash_erase(start_address, HAL_FLASH_BLOCK_4K);
			start_address += BLOCK_SIZE_4K;
		}else{
			break;
		}
	}
	return kNoErr;
}

/**
 * Write flash
 *
 */
OSStatus platform_flash_write( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  )
{
	int32_t ret = hal_flash_write(*start_address, data, length);
	*start_address += length;
	return ret == HAL_FLASH_STATUS_OK ? kNoErr : kWriteErr;
}

/**
 * Read flash
 *
 */
OSStatus platform_flash_read( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  )
{
	hal_flash_read(*start_address, data, length);
	*start_address += length;
	return kNoErr;
}

/**
 * Flash protect operation
 *
 */
OSStatus platform_flash_enable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address ){ return kNoErr; }
OSStatus platform_flash_disable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address ){ return kNoErr; }
