/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */

/** @file
 * Defines ATSAMG55 default unhandled ISR and default mappings to unhandled ISR
 */
#include <stdint.h>
#include "platform_assert.h"
#include "platform_cmsis.h"
#include "platform_isr.h"

/******************************************************
 *                      Macros
 ******************************************************/

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
 *               Static Function Declarations
 ******************************************************/

extern void UnhandledInterrupt( void );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

PLATFORM_DEFINE_ISR( UnhandledInterrupt )
{
   uint32_t active_interrupt_vector = (uint32_t) ( SCB->ICSR & 0x3fU );
//
//    /* This variable tells you which interrupt vector is currently active */
    (void)active_interrupt_vector;
//    MICO_TRIGGER_BREAKPOINT( );

    while( 1 )
    {
    }
}

/******************************************************
 *          Default IRQ Handler Declarations
 ******************************************************/

PLATFORM_SET_DEFAULT_ISR( NMI_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( HardFault_Handler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( MemManage_Handler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( BusFault_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UsageFault_Handler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SVC_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( DebugMon_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PendSV_Handler                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SysTick_Handler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SUPC_Handler                  , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RSTC_Handler                  , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RTC_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RTT_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( WDT_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PMC_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( EFC_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( FLEXCOM7_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( FLEXCOM0_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( FLEXCOM1_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIOA_Handler                  , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIOB_Handler                  , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PDMIC0_Handler                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( FLEXCOM2_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( MEM2MEM_Handler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( I2SC0_Handler                 , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( I2SC1_Handler                 , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PDMIC1_Handler                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( FLEXCOM3_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( FLEXCOM4_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( FLEXCOM5_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( FLEXCOM6_Handler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC0_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC1_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC2_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC3_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC4_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC5_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ADC_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ARM_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UHP_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UDP_Handler                   , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( CRCCU_Handler                 , UnhandledInterrupt )
