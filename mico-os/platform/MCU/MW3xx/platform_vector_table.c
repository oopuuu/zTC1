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

extern void NMI_Handler         ( void );  // Non Maskable Interrupt
extern void HardFault_Handler   ( void );  // Hard Fault interrupt
extern void MemManage_Handler   ( void );  // Memory Management Fault interrupt
extern void BusFault_Handler    ( void );  // Bus Fault interrupt
extern void UsageFault_Handler  ( void );  // Usage Fault interrupt
extern void DebugMon_Handler    ( void );  // Debug Monitor interrupt

extern void ExtPin0_IRQHandler  ( void );
extern void ExtPin1_IRQHandler  ( void );
extern void RTC_IRQHandler      ( void );
extern void CRC_IRQHandler      ( void );
extern void AES_IRQHandler      ( void );
extern void I2C0_IRQHandler     ( void );
extern void I2C1_IRQHandler     ( void );
extern void DMA_IRQHandler      ( void );
extern void GPIO_IRQHandler     ( void );
extern void SSP0_IRQHandler     ( void );
extern void SSP1_IRQHandler     ( void );
extern void SSP2_IRQHandler     ( void );
extern void QSPI_IRQHandler     ( void );
extern void GPT0_IRQHandler     ( void );
extern void GPT1_IRQHandler     ( void );
extern void GPT2_IRQHandler     ( void );
extern void GPT3_IRQHandler     ( void );
extern void UART0_IRQHandler    ( void );
extern void UART1_IRQHandler    ( void );
extern void UART2_IRQHandler    ( void );
extern void WDT_IRQHandler      ( void );
extern void ADC0_IRQHandler     ( void );
extern void DAC_IRQHandler      ( void );
extern void ACOMPWKUP_IRQHandler( void );
extern void ACOMP_IRQHandler    ( void );
extern void SDIO_IRQHandler     ( void );
extern void USB_IRQHandler      ( void );
extern void PLL_IRQHandler      ( void );
extern void RC32M_IRQHandler    ( void );
extern void ExtPin3_IRQHandler  ( void );
extern void ExtPin4_IRQHandler  ( void );
extern void ExtPin5_IRQHandler  ( void );
extern void ExtPin6_IRQHandler  ( void );
extern void ExtPin7_IRQHandler  ( void );
extern void ExtPin8_IRQHandler  ( void );
extern void ExtPin9_IRQHandler  ( void );
extern void ExtPin10_IRQHandler ( void );
extern void ExtPin11_IRQHandler ( void );
extern void ExtPin12_IRQHandler ( void );
extern void ExtPin13_IRQHandler ( void );
extern void ExtPin14_IRQHandler ( void );
extern void ExtPin15_IRQHandler ( void );
extern void ExtPin16_IRQHandler ( void );
extern void ExtPin17_IRQHandler ( void );
extern void ExtPin18_IRQHandler ( void );
extern void ExtPin19_IRQHandler ( void );
extern void ExtPin20_IRQHandler ( void );
extern void ExtPin21_IRQHandler ( void );
extern void ExtPin22_IRQHandler ( void );
extern void ExtPin23_IRQHandler ( void );
extern void ExtPin24_IRQHandler ( void );
extern void ExtPin25_IRQHandler ( void );
extern void ExtPin26_IRQHandler ( void );
extern void ExtPin27_IRQHandler ( void );
extern void ULPCOMP_IRQHandler  ( void );
extern void BRNDET_IRQHandler   ( void );
extern void WIFIWKUP_IRQHandler ( void );

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
    /* Marvell specific interrupts */
    (uint32_t)ExtPin0_IRQHandler           ,
    (uint32_t)ExtPin1_IRQHandler           ,
    (uint32_t)RTC_IRQHandler               ,
    (uint32_t)CRC_IRQHandler               ,
    (uint32_t)AES_IRQHandler               ,
    (uint32_t)I2C0_IRQHandler              ,
    (uint32_t)I2C1_IRQHandler               ,
    (uint32_t)0                             ,
    (uint32_t)DMA_IRQHandler                ,
    (uint32_t)GPIO_IRQHandler               ,
    (uint32_t)SSP0_IRQHandler               ,
    (uint32_t)SSP1_IRQHandler               ,
    (uint32_t)SSP2_IRQHandler               ,
    (uint32_t)QSPI_IRQHandler               ,
    (uint32_t)GPT0_IRQHandler               ,
    (uint32_t)GPT1_IRQHandler               ,
    (uint32_t)GPT2_IRQHandler               ,
    (uint32_t)GPT3_IRQHandler               ,
    (uint32_t)UART0_IRQHandler              ,
    (uint32_t)UART1_IRQHandler              ,
    (uint32_t)UART2_IRQHandler              ,
    (uint32_t)0                             ,
    (uint32_t)WDT_IRQHandler                ,
    (uint32_t)0                             ,
    (uint32_t)ADC0_IRQHandler               ,
    (uint32_t)DAC_IRQHandler                ,
    (uint32_t)ACOMPWKUP_IRQHandler          ,
    (uint32_t)ACOMP_IRQHandler              ,
    (uint32_t)SDIO_IRQHandler               ,
    (uint32_t)USB_IRQHandler                ,
    (uint32_t)0                             ,
    (uint32_t)PLL_IRQHandler                ,
    (uint32_t)0                             ,
    (uint32_t)RC32M_IRQHandler              ,
    (uint32_t)ExtPin3_IRQHandler            ,
    (uint32_t)ExtPin4_IRQHandler            ,
    (uint32_t)ExtPin5_IRQHandler            ,
    (uint32_t)ExtPin6_IRQHandler            ,
    (uint32_t)ExtPin7_IRQHandler            ,
    (uint32_t)ExtPin8_IRQHandler            ,
    (uint32_t)ExtPin9_IRQHandler            ,
    (uint32_t)ExtPin10_IRQHandler           ,
    (uint32_t)ExtPin11_IRQHandler           ,
    (uint32_t)ExtPin12_IRQHandler           ,
    (uint32_t)ExtPin13_IRQHandler           ,
    (uint32_t)ExtPin14_IRQHandler           ,
    (uint32_t)ExtPin15_IRQHandler           ,
    (uint32_t)ExtPin16_IRQHandler           ,
    (uint32_t)ExtPin17_IRQHandler           ,
    (uint32_t)ExtPin18_IRQHandler           ,
    (uint32_t)ExtPin19_IRQHandler           ,
    (uint32_t)ExtPin20_IRQHandler           ,
    (uint32_t)ExtPin21_IRQHandler           ,
    (uint32_t)ExtPin22_IRQHandler           ,
    (uint32_t)ExtPin23_IRQHandler           ,
    (uint32_t)ExtPin24_IRQHandler           ,
    (uint32_t)ExtPin25_IRQHandler           ,
    (uint32_t)ExtPin26_IRQHandler           ,
    (uint32_t)ExtPin27_IRQHandler           ,
    (uint32_t)0                             ,
    (uint32_t)ULPCOMP_IRQHandler            ,
    (uint32_t)BRNDET_IRQHandler             ,
    (uint32_t)WIFIWKUP_IRQHandler           ,
};

/******************************************************
 *               Function Definitions
 ******************************************************/
