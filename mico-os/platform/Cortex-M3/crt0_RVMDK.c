/**
 ******************************************************************************
 * @file    crt0_IAR.h
 * @author  William Xu
 * @version V1.0.0
 * @date    16-Sep-2014
 * @brief   __low_level_init called by IAR before main.
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
#include "crt0.h"

extern void* app_hdr_start_addr_loc;
#define SCB_VTOR_ADDRESS         ( ( volatile unsigned long* ) 0xE000ED08 )
#define APP_HDR_START_ADDR   ((unsigned char*)&app_hdr_start_addr_loc)
  
extern unsigned long Image$$ER_IROM1$$Base;

int __low_level_init( void );

/* This is the code that gets called on processor reset. To initialize the */
/* device. */
int __low_level_init( void )
{
     extern void init_clocks(void);
     extern void init_memory(void);
     /* IAR allows init functions in __low_level_init(), but it is run before global
      * variables have been initialised, so the following init still needs to be done
      * When using GCC, this is done in crt0_GCC.c
      */
     
     /* Setup the interrupt vectors address */
     *SCB_VTOR_ADDRESS = (unsigned long)&Image$$ER_IROM1$$Base;
     init_clocks();
     init_memory();

     return 1; /* return 1 to force memory init */
}
