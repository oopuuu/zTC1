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


/******************************************************
*                      Macros
******************************************************/

/******************************************************
*                    Constants
******************************************************/

#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE   64
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
#if defined ( __ICCARM__ )
static inline void __jump_to( uint32_t addr )
{
  __asm( "MOV R1, #0x00000001" );
  __asm( "ORR R0, R0, R1" );  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  __asm( "BLX R0" );
}


#elif defined ( __GNUC__ )
__attribute__( ( always_inline ) ) static __INLINE void __jump_to( uint32_t addr )
{
  addr |= 0x00000001;  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  __ASM volatile ("BX %0" : : "r" (addr) );
}


#elif defined ( __CC_ARM )
static void __asm __jump_to( uint32_t addr )
{
  MOV R1, #0x00000001
  ORR R0, R0, R1  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  BLX R0
}
#endif

void startApplication( uint32_t app_addr )
{
  uint32_t* stack_ptr;
  uint32_t* start_ptr;
  
  if (((*(volatile uint32_t*)app_addr) & 0x2FFE0000 ) != 0x20000000)
  app_addr += 0x200;
  /* Test if user code is programmed starting from address "ApplicationAddress" */
  if (((*(volatile uint32_t*)app_addr) & 0x2FFE0000 ) == 0x20000000)
  { 
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    /* Clear all interrupt enabled by bootloader */
    for (int i = 0; i < 8; i++ )
        NVIC->ICER[i] = 0xFFFFFFFF;
    
    stack_ptr = (uint32_t*) app_addr;  /* Initial stack pointer is first 4 bytes of vector table */
    start_ptr = ( stack_ptr + 1 );  /* Reset vector is second 4 bytes of vector table */

    #if defined ( __ICCARM__)
    __ASM( "MOV LR,        #0xFFFFFFFF" );
    __ASM( "MOV R1,        #0x01000000" );
    __ASM( "MSR APSR_nzcvq,     R1" );
    __ASM( "MOV R1,        #0x00000000" );
    __ASM( "MSR PRIMASK,   R1" );
    __ASM( "MSR FAULTMASK, R1" );
    __ASM( "MSR BASEPRI,   R1" );
    __ASM( "MSR CONTROL,   R1" );
    #endif
    
    __set_MSP( *stack_ptr );
    __jump_to( *start_ptr );
  }  
}

void platform_mcu_reset( void )
{
    NVIC_SystemReset();
}

/* STM32F2 common clock initialisation function
* This brings up enough clocks to allow the processor to run quickly while initialising memory.
* Other platform specific clock init can be done in init_platform() or init_architecture()
*/
void init_clocks( void )
{
  sysclk_init();
  
  /* Switch Slow Clock source to external 32K crystal */
  pmc_switch_sclk_to_32kxtal( 0 );
  while( pmc_osc_is_ready_32kxtal( ) == 0 )
  {
  }

  pmc_disable_udpck( );
  pmc_disable_all_periph_clk( );

#ifdef NO_MICO_RTOS  
  SysTick_Config( SystemCoreClock / 1000 );
#endif

}

WEAK void init_memory( void )
{
  
}

void init_architecture( void )
{
  uint8_t i;
  
  /* Initialise the interrupt priorities to a priority lower than 0 so that the BASEPRI register can mask them */
  for ( i = 0; i < 81; i++ )
  {
    NVIC ->IP[i] = 0xff;
  }
  
  NVIC_SetPriorityGrouping( 7 - __NVIC_PRIO_BITS );

  /* Initialise the interrupt priorities to a priority lower than 0 so that the BASEPRI register can mask them */
  NVIC_SetPriority(SVCall_IRQn,                 0);
  NVIC_SetPriority(DebugMonitor_IRQn,           0);
  NVIC_SetPriority(PendSV_IRQn,                 ((1 << __NVIC_PRIO_BITS) - 1));
  NVIC_SetPriority(SysTick_IRQn,                ((1 << __NVIC_PRIO_BITS) - 1));
  
  /*
   * enable 3 exception interrupt
   */
  SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
  SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;  
  
  platform_init_peripheral_irq_priorities();
  
  ioport_init();
  wdt_disable( WDT );

  /* Initialise GPIO IRQ manager */
  platform_gpio_irq_manager_init();
  
#ifndef MICO_DISABLE_MCU_POWERSAVE
  /* Initialise MCU powersave */
  platform_mcu_powersave_init( );
#endif /* ifndef MICO_DISABLE_MCU_POWERSAVE */
  
#ifndef MICO_DISABLE_STDIO
#ifndef NO_MICO_RTOS
  mico_rtos_init_mutex( &stdio_tx_mutex );
  mico_rtos_unlock_mutex ( &stdio_tx_mutex );
  mico_rtos_init_mutex( &stdio_rx_mutex );
  mico_rtos_unlock_mutex ( &stdio_rx_mutex );
#endif

  ring_buffer_init  ( (ring_buffer_t*)&stdio_rx_buffer, (uint8_t*)stdio_rx_data, STDIO_BUFFER_SIZE );
  platform_uart_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_uart_config, (ring_buffer_t*)&stdio_rx_buffer );
#endif

  /* Ensure 802.11 device is in reset. */
  host_platform_init( );

  /* Initialise nanosecond clock counter */
  platform_init_nanosecond_clock();

#ifdef BOOTLOADER
  return;
#endif
  
  platform_mcu_powersave_disable( );

}

OSStatus stdio_hardfault( char* data, uint32_t size )
{
#ifndef MICO_DISABLE_STDIO
    uint32_t idx;
    Usart *p_usart = platform_uart_peripherals[STDIO_UART].port;

    for ( idx = 0; idx < size; idx++ )
    {
        while ( usart_is_tx_ready( p_usart ) == 0 );
        usart_write( p_usart, (data[idx] & (uint16_t)0x01FF) );
    }
#endif
    return kNoErr;
}

/******************************************************
*            NO-OS Functions
******************************************************/


#ifdef NO_MICO_RTOS
static volatile uint32_t no_os_tick = 0;

void SysTick_Handler(void)
{
  no_os_tick ++;
  platform_watchdog_kick( );
}

uint32_t mico_get_time_no_os(void)
{
  return no_os_tick;
}
#endif


