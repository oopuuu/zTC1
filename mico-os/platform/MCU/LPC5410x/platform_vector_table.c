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
 * STM32F4xx vector table
 */
#include <stdint.h>
#include "platform_assert.h"
//#include "platform_constants.h"
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
extern void Reset_Handler     ( void );

extern void vPortSVCHandler     ( void );
extern void xPortPendSVHandler     ( void );
extern void xPortSysTickHandler     ( void );

#ifdef NO_MICO_RTOS
extern void SVC_Handler             ( void );
extern void PendSV_Handler          ( void );
extern void SysTick_Handler         ( void );
#else
#define SVC_Handler     vPortSVCHandler
#define PendSV_Handler  xPortPendSVHandler
#define SysTick_Handler xPortSysTickHandler
#endif

extern void Reset_Handler           ( void );
extern void NMI_Handler             ( void );
extern void HardFault_Handler       ( void );
extern void MemManage_Handler       ( void );
extern void BusFault_Handler        ( void );
extern void UsageFault_Handler      ( void );
extern void SVC_Handler             ( void );
extern void DebugMon_Handler        ( void );
extern void PendSV_Handler          ( void );
extern void SysTick_Handler         ( void );
extern void WDT_IRQHandler          ( void );
extern void BOD_IRQHandler          ( void );
extern void Reserved_IRQHandler     ( void );
extern void DMA_IRQHandler          ( void );
extern void GINT0_IRQHandler        ( void );
extern void PIN_INT0_IRQHandler     ( void );
extern void PIN_INT1_IRQHandler     ( void );
extern void PIN_INT2_IRQHandler     ( void );
extern void PIN_INT3_IRQHandler     ( void );
extern void UTICK_IRQHandler        ( void );
extern void MRT_IRQHandler          ( void );
extern void CT32B0_IRQHandler       ( void );
extern void CT32B1_IRQHandler       ( void );
extern void CT32B2_IRQHandler       ( void );
extern void CT32B3_IRQHandler       ( void );
extern void CT32B4_IRQHandler       ( void );
extern void SCT0_IRQHandler         ( void );
extern void UART0_IRQHandler        ( void );
extern void UART1_IRQHandler        ( void );
extern void UART2_IRQHandler        ( void );
extern void UART3_IRQHandler        ( void );
extern void I2C0_IRQHandler         ( void );
extern void I2C1_IRQHandler         ( void );
extern void I2C2_IRQHandler         ( void );
extern void SPI0_IRQHandler         ( void );
extern void SPI1_IRQHandler         ( void );
extern void ADC_SEQA_IRQHandler     ( void );
extern void ADC_SEQB_IRQHandler     ( void );
extern void ADC_THCMP_IRQHandler    ( void );
extern void RTC_IRQHandler          ( void );
extern void MAILBOX_IRQHandler      ( void );
extern void GINT1_IRQHandler        ( void );
extern void PIN_INT4_IRQHandler     ( void );
extern void PIN_INT5_IRQHandler     ( void );
extern void PIN_INT6_IRQHandler     ( void );
extern void PIN_INT7_IRQHandler     ( void );
extern void RIT_IRQHandler          ( void );
extern void Reserved41_IRQHandler   ( void );
extern void Reserved42_IRQHandler   ( void );
extern void Reserved43_IRQHandler   ( void );
extern void Reserved44_IRQHandler   ( void );


/******************************************************
 *               Variable Definitions
 ******************************************************/

/* Pointer to stack location */
extern void* link_stack_end;

uint32_t interrupt_vector_table[] =
{
    (uint32_t)&link_stack_end              , // Initial stack location
    (uint32_t)Reset_Handler                , // Reset vector
    (uint32_t)NMI_Handler                  , // Non Maskable Interrupt
    (uint32_t)HardFault_Handler            , // Hard Fault interrupt
    (uint32_t)MemManage_Handler            , // Memory Management Fault interrupt
    (uint32_t)BusFault_Handler             , // Bus Fault interrupt
    (uint32_t)UsageFault_Handler           , // Usage Fault interrupt
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)SVC_Handler                  , // SVC interrupt
    (uint32_t)DebugMon_Handler             , // Debug Monitor interrupt
    (uint32_t)0                            , // Reserved
    (uint32_t)PendSV_Handler               , // PendSV interrupt
    (uint32_t)SysTick_Handler              , // Sys Tick Interrupt
    (uint32_t)WDT_IRQHandler               , // Watchdog
    (uint32_t)BOD_IRQHandler               , // Brown Out Detect
    (uint32_t)Reserved_IRQHandler          , // Reserved
    (uint32_t)DMA_IRQHandler               , // DMA Controller
    (uint32_t)GINT0_IRQHandler             , // GPIO Group0 Interrupt
    (uint32_t)PIN_INT0_IRQHandler          , // PIO INT0
    (uint32_t)PIN_INT1_IRQHandler          , // PIO INT1
    (uint32_t)PIN_INT2_IRQHandler          , // PIO INT2
    (uint32_t)PIN_INT3_IRQHandler          , // PIO INT3
    (uint32_t)UTICK_IRQHandler             , // UTICK timer
    (uint32_t)MRT_IRQHandler               , // Multi-Rate Timer
    (uint32_t)CT32B0_IRQHandler            , // CT32B0
    (uint32_t)CT32B1_IRQHandler            , // CT32B1
    (uint32_t)CT32B2_IRQHandler            , // CT32B2
    (uint32_t)CT32B3_IRQHandler            , // CT32B3
    (uint32_t)CT32B4_IRQHandler            , // CT32B4
    (uint32_t)SCT0_IRQHandler              , // Smart Counter Timer
    (uint32_t)UART0_IRQHandler             , // UART0
    (uint32_t)UART1_IRQHandler             , // UART1
    (uint32_t)UART2_IRQHandler             , // UART2
    (uint32_t)UART3_IRQHandler             , // UART3
    (uint32_t)I2C0_IRQHandler              , // I2C0 controller
    (uint32_t)I2C1_IRQHandler              , // I2C1 controller
    (uint32_t)I2C2_IRQHandler              , // I2C2 controller
    (uint32_t)SPI0_IRQHandler              , // SPI0 controller
    (uint32_t)SPI1_IRQHandler              , // SPI1 controller
    (uint32_t)ADC_SEQA_IRQHandler          , // ADC0 A sequence (A/D Converter) interrupt
    (uint32_t)ADC_SEQB_IRQHandler          , // ADC0 B sequence (A/D Converter) interrupt
    (uint32_t)ADC_THCMP_IRQHandler         , // ADC THCMP and OVERRUN ORed
    (uint32_t)RTC_IRQHandler               , // RTC Timer
    (uint32_t)Reserved_IRQHandler          , // Reserved
    (uint32_t)MAILBOX_IRQHandler           , // Mailbox
    (uint32_t)GINT1_IRQHandler             , // GPIO Group1 Interrupt
    (uint32_t)PIN_INT4_IRQHandler          , // PIO INT4
    (uint32_t)PIN_INT5_IRQHandler          , // PIO INT5
    (uint32_t)PIN_INT6_IRQHandler          , // PIO INT6
    (uint32_t)PIN_INT7_IRQHandler          , // PIO INT7
    (uint32_t)Reserved_IRQHandler          , // Reserved
    (uint32_t)Reserved_IRQHandler          , // Reserved
    (uint32_t)Reserved_IRQHandler          , // Reserved
    (uint32_t)RIT_IRQHandler               , // RITimer
    (uint32_t)Reserved41_IRQHandler        , // Reserved
    (uint32_t)Reserved42_IRQHandler        , // Reserved
    (uint32_t)Reserved43_IRQHandler        , // Reserved
    (uint32_t)Reserved44_IRQHandler        , // Reserved
};

/******************************************************
 *               Function Definitions
 ******************************************************/
