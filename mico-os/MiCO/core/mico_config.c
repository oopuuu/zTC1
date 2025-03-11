/**
 ******************************************************************************
 * @file    mico_config.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide the MICO running constants.
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "common.h"
#include "mico_config.h"
#include "platform_config.h"

#ifdef MICO_DEFAULT_APPLICATION_STACK_SIZE
uint32_t  app_stack_size = MICO_DEFAULT_APPLICATION_STACK_SIZE; 
#else
uint32_t  app_stack_size = 1500;
#endif


#ifdef  MICO_DEFAULT_TICK_RATE_HZ
const uint32_t  mico_tick_rate_hz = MICO_DEFAULT_TICK_RATE_HZ;
#else 
const uint32_t  mico_tick_rate_hz = 1000; // Default OS tick is 1000Hz
#endif

#ifndef MCU_CLOCK_HZ
	#error "MCU_CLOCK_HZ not defined in device header!"
#else
	const uint32_t  mico_cpu_clock_hz = MCU_CLOCK_HZ;
#endif

#ifndef CORTEX_NVIC_PRIO_BITS
	#define CORTEX_NVIC_PRIO_BITS          4
	#warning "CORTEX_NVIC_PRIO_BITS not defined in device header file; using default!"
#endif

const int CFG_PRIO_BITS = CORTEX_NVIC_PRIO_BITS;

const uint32_t  mico_timer_queue_len = 5;

const uint32_t mico_nmode_enable = true;

#ifdef DEBUG
int mico_debug_enabled = 1;
#else
int mico_debug_enabled = 0;
#endif

#ifdef SDIO_1_BIT
int sdio_1_bit_mode = 1;
#else
int sdio_1_bit_mode = 0;
#endif

