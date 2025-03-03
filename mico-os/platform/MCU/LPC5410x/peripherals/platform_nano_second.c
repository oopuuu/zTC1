/**
 ******************************************************************************
 * @file    platform_nano_second.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide time delay function using nano second.
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
#include "chip.h"

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
 *                      Macros
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


void platform_init_nanosecond_clock(void)
{
  Chip_TIMER_Init(LPC_TIMER4);  
  Chip_TIMER_PrescaleSet(LPC_TIMER4,0);  
  Chip_TIMER_Reset(LPC_TIMER4);
  Chip_TIMER_Enable(LPC_TIMER4);
}


void platform_nanosecond_delay( uint64_t delayns )//max 1s
{
  uint32_t currentns = 0;
  uint32_t delay10ns = ((delayns*96)/1000);
  if(delay10ns>100000000)
    delay10ns=100000000;
  
  platform_init_nanosecond_clock();
  
  do
  {
    currentns = Chip_TIMER_ReadCount(LPC_TIMER4);
  }
  while(currentns < delay10ns);
  
  Chip_TIMER_Disable(LPC_TIMER4);
  
}

