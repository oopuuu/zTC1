/**
 ******************************************************************************
 * @file    platform_rng.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide RNG driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "platform_peripheral.h"
#include "platform.h"

/******************************************************
 *                   Macros
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
 *                     Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus platform_random_number_read( void *buf, int num )
{
  hal_trng_init();
  uint16_t word_bytes = num / 4;
  uint8_t remain_bytes = num % 4;
  uint32_t random_num;
  uint32_t *word_ptr = (uint32_t*)buf;
  while(word_bytes--){
  	hal_trng_get_generated_random_number(&random_num);
  	*word_ptr++ = random_num;
  }
  hal_trng_get_generated_random_number(&random_num);
  memcpy(word_ptr, &random_num, remain_bytes);
  hal_trng_deinit();
	return kNoErr;
}
