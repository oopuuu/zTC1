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
 * Defines STM32F4xx default unhandled ISR and default mappings to unhandled ISR
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


PLATFORM_SET_DEFAULT_ISR( ExtPin0_IRQHandler            , UnhandledInterrupt )

PLATFORM_SET_DEFAULT_ISR( ExtPin1_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RTC_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( CRC_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( AES_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( I2C0_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( I2C1_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( DMA_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( GPIO_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SSP0_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SSP1_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SSP2_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( QSPI_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( GPT0_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( GPT1_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( GPT2_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( GPT3_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UART0_IRQHandler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UART1_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UART2_IRQHandler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( WDT_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ADC0_IRQHandler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( DAC_IRQHandler                 , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ACOMPWKUP_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ACOMP_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SDIO_IRQHandler              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( USB_IRQHandler                 , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PLL_IRQHandler                 , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RC32M_IRQHandler               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin3_IRQHandler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin4_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin5_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin6_IRQHandler             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin7_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin8_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin9_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin10_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin11_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin12_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin13_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin14_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin15_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin16_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin17_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin18_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin19_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin20_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin21_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin22_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin23_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin24_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin25_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin26_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ExtPin27_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ULPCOMP_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( BRNDET_IRQHandler            , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( WIFIWKUP_IRQHandler            , UnhandledInterrupt )



