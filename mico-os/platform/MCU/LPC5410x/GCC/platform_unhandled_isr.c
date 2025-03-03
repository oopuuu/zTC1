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
PLATFORM_SET_DEFAULT_ISR( WDT_IRQHandler                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( BOD_IRQHandler                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( Reserved_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( DMA_IRQHandler                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( GINT0_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIN_INT0_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIN_INT1_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIN_INT2_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIN_INT3_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UTICK_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( MRT_IRQHandler                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( CT32B0_IRQHandler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( CT32B1_IRQHandler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( CT32B2_IRQHandler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( CT32B3_IRQHandler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( CT32B4_IRQHandler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SCT0_IRQHandler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UART0_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UART1_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UART2_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UART3_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( I2C0_IRQHandler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( I2C1_IRQHandler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( I2C2_IRQHandler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SPI0_IRQHandler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SPI1_IRQHandler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ADC_SEQA_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ADC_SEQB_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ADC_THCMP_IRQHandler          , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RTC_IRQHandler                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( MAILBOX_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( GINT1_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIN_INT4_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIN_INT5_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIN_INT6_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIN_INT7_IRQHandler           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RIT_IRQHandler                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( Reserved41_IRQHandler         , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( Reserved42_IRQHandler         , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( Reserved43_IRQHandler         , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( Reserved44_IRQHandler         , UnhandledInterrupt )
