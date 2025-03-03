/**
 ******************************************************************************
 * @file    platform_init.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide functions called by MICO to drive stm32f2xx
 *          platform: - e.g. power save, reboot, platform initialize
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
#include "platform_config.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
#include "crt0.h"
#include "mico_rtos.h"
#include "platform_init.h"

#include "portmacro.h"

#ifdef __GNUC__
#include "../../GCC/stdio_newlib.h"
#endif /* ifdef __GNUC__ */


/******************************************************
*                      Macros
******************************************************/

/******************************************************
*                    Constants
******************************************************/

#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE   1024
#endif

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
*               Function Declarations
******************************************************/

extern OSStatus host_platform_init( void );
extern void system_reload(void);
/******************************************************
*               Variables Definitions
******************************************************/
extern platform_uart_t platform_uart_peripherals[];
extern platform_uart_driver_t platform_uart_drivers[];

#ifndef MICO_DISABLE_STDIO
static const platform_uart_config_t stdio_uart_config =
{
  .baud_rate    = STDIO_UART_BAUDRATE,
  .data_width   = DATA_WIDTH_8BIT,
  .parity       = NO_PARITY,
  .stop_bits    = STOP_BITS_1,
  .flow_control = FLOW_CONTROL_DISABLED,
  .flags        = 0,
};

static volatile ring_buffer_t stdio_rx_buffer;
static volatile uint8_t             stdio_rx_data[STDIO_BUFFER_SIZE];
mico_mutex_t        stdio_rx_mutex;
mico_mutex_t        stdio_tx_mutex;
#endif /* #ifndef MICO_DISABLE_STDIO */

/******************************************************
*               Function Definitions
******************************************************/


void __jump_to( uint32_t addr )
{
 // addr |= 0x00000001;  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  __asm volatile ("BX %0" : : "r" (addr) );
}


void startApplication( uint32_t app_addr )
{
    intc_deinit();
    DISABLE_INTERRUPTS();
    __jump_to( app_addr );
}

void platform_mcu_reset( void )
{
    bk_wdg_initialize(1);
}

/* STM32F2 common clock initialisation function
* This brings up enough clocks to allow the processor to run quickly while initialising memory.
* Other platform specific clock init can be done in init_platform() or init_architecture()
*/
void init_clocks( void )
{
#ifdef NO_MICO_RTOS
    fclk_init();
#endif
}

WEAK void init_memory( void )
{
  
}

void init_architecture( void )
{
}

extern void entry_main(void);

/* mico_main is executed after rtos is start up and before real main*/
void mico_main( void )
{
    /* Customized board configuration. */
    init_platform( );

#ifndef MICO_DISABLE_STDIO
    if( stdio_tx_mutex == NULL )
        mico_rtos_init_mutex( &stdio_tx_mutex );

    ring_buffer_init  ( (ring_buffer_t*)&stdio_rx_buffer, (uint8_t*)stdio_rx_data, STDIO_BUFFER_SIZE );
    platform_uart_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_uart_config, (ring_buffer_t*)&stdio_rx_buffer );
#endif
}


void software_init_hook(void)
{
    entry_main();
    main();
}

OSStatus stdio_hardfault( char* data, uint32_t size )
{
  return kNoErr;
}

static char global_cid[25] = { 0 };
const char *mico_generate_cid( uint8_t* length )
{
  return global_cid;
}

bool isWakeUpFlagPowerOn(void){
  return false;
};

/******************************************************
*            NO-OS Functions
******************************************************/

#ifdef NO_MICO_RTOS

uint32_t mico_get_time_no_os(void)
{
    return fclk_get_tick();
}
#endif


