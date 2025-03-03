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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
OSStatus platform_watchdog_init(uint32_t timeout_ms)
{
  hal_wdt_config_t wdt_init;
  wdt_init.mode = HAL_WDT_MODE_RESET;
  // range: 1 ~ 30s
  wdt_init.seconds = timeout_ms < 1000 ? 1 : timeout_ms > 30000 ? 30 : timeout_ms / 1000;
  hal_wdt_init(&wdt_init);
  hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
	return kNoErr;
}

/**
 * Refresh the watchdog
 *
 * @return @ref OSStatus
 */
OSStatus platform_watchdog_kick(void)
{
	hal_wdt_feed(HAL_WDT_FEED_MAGIC);
	return kNoErr;
}

/**
 * Check if last reset occurred due to watchdog reset
 *
 * @return @ref OSStatus
 */
bool platform_watchdog_check_last_reset(void)
{
	return kNoErr;
}
