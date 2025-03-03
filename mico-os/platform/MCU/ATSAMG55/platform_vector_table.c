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

extern void NMI_Handler             ( void );
extern void HardFault_Handler       ( void );
extern void MemManage_Handler       ( void );
extern void BusFault_Handler        ( void );
extern void UsageFault_Handler      ( void );
extern void SVC_Handler             ( void );
extern void DebugMon_Handler        ( void );
extern void PendSV_Handler          ( void );
extern void SysTick_Handler         ( void );
extern void SUPC_Handler            ( void );
extern void RSTC_Handler            ( void );
extern void RTC_Handler             ( void );
extern void RTT_Handler             ( void );
extern void WDT_Handler             ( void );
extern void PMC_Handler             ( void );
extern void EFC_Handler             ( void );
extern void FLEXCOM7_Handler        ( void );
extern void FLEXCOM0_Handler        ( void );
extern void FLEXCOM1_Handler        ( void );
extern void PIOA_Handler            ( void );
extern void PIOB_Handler            ( void );
extern void PDMIC0_Handler          ( void );
extern void MEM2MEM_Handler         ( void );
extern void FLEXCOM2_Handler        ( void );
extern void I2SC0_Handler           ( void );
extern void I2SC1_Handler           ( void );
extern void PDMIC1_Handler          ( void );
extern void FLEXCOM3_Handler        ( void );
extern void FLEXCOM4_Handler        ( void );
extern void FLEXCOM5_Handler        ( void );
extern void FLEXCOM6_Handler        ( void );
extern void TC0_Handler             ( void );
extern void TC1_Handler             ( void );
extern void TC2_Handler             ( void );
extern void TC3_Handler             ( void );
extern void TC4_Handler             ( void );
extern void TC5_Handler             ( void );
extern void ADC_Handler             ( void );
extern void ARM_Handler             ( void );
extern void UHP_Handler             ( void );
extern void UDP_Handler             ( void );
extern void CRCCU_Handler           ( void );


/******************************************************
 *               Variable Definitions
 ******************************************************/

/* Pointer to stack location */
extern void* link_stack_end;

uint32_t interrupt_vector_table_copy_ramcode[] =
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
    (uint32_t)DebugMon_Handler             // Debug Monitor interrupt
};

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
    (uint32_t)SUPC_Handler                 , // Supply Controller
    (uint32_t)RSTC_Handler                 , // 1  Reset Controller
    (uint32_t)RTC_Handler                  , // 2  Real Time Clock
    (uint32_t)RTT_Handler                  , // 3  Real Time Timer
    (uint32_t)WDT_Handler                  , // 4  Watchdog Timer
    (uint32_t)PMC_Handler                  , // 5  Power Management
    (uint32_t)EFC_Handler                  , // 6  Enhanced Flash Controller
    (uint32_t)FLEXCOM7_Handler             , // 7  FLEXCOM 7
    (uint32_t)FLEXCOM0_Handler             , // 8  FLEXCOM 0
    (uint32_t)FLEXCOM1_Handler             , // 9  FLEXCOM 1
    (uint32_t)0                            , // Reserved
    (uint32_t)PIOA_Handler                 , // 11 Parallel I/O Controller A
    (uint32_t)PIOB_Handler                 , // 12 Parallel I/O Controller B
    (uint32_t)PDMIC0_Handler               , // 13 PDM 0
    (uint32_t)FLEXCOM2_Handler             , // 14 FLEXCOM2
    (uint32_t)MEM2MEM_Handler              , // 15 MEM2MEM
    (uint32_t)I2SC0_Handler                , // 16 I2SC0
    (uint32_t)I2SC1_Handler                , // 17 I2SC1
    (uint32_t)PDMIC1_Handler               , // 18 PDM 1
    (uint32_t)FLEXCOM3_Handler             , // 19 FLEXCOM3
    (uint32_t)FLEXCOM4_Handler             , // 20 FLEXCOM4
    (uint32_t)FLEXCOM5_Handler             , // 21 FLEXCOM5
    (uint32_t)FLEXCOM6_Handler             , // 22 FLEXCOM6
    (uint32_t)TC0_Handler                  , // 23 Timer/Counter 0
    (uint32_t)TC1_Handler                  , // 24 Timer/Counter 1
    (uint32_t)TC2_Handler                  , // 25 Timer/Counter 2
    (uint32_t)TC3_Handler                  , // 26 Timer/Counter 3
    (uint32_t)TC4_Handler                  , // 27 Timer/Counter 4
    (uint32_t)TC5_Handler                  , // 28 Timer/Counter 5
    (uint32_t)ADC_Handler                  , // 29 Analog To Digital Converter
    (uint32_t)ARM_Handler                  , // 30 FPU
    (uint32_t)0                            , // 31 Reserved
    (uint32_t)0                            , // 32 Reserved
    (uint32_t)0                            , // 33 Reserved
    (uint32_t)0                            , // 34 Reserved
    (uint32_t)0                            , // 35 Reserved
    (uint32_t)0                            , // 36 Reserved
    (uint32_t)0                            , // 37 Reserved
    (uint32_t)0                            , // 38 Reserved
    (uint32_t)0                            , // 39 Reserved
    (uint32_t)0                            , // 40 Reserved
    (uint32_t)0                            , // 41 Reserved
    (uint32_t)0                            , // 42 Reserved
    (uint32_t)0                            , // 43 Reserved
    (uint32_t)0                            , // 44 Reserved
    (uint32_t)0                            , // 45 Reserved
    (uint32_t)0                            , // 46 Reserved
    (uint32_t)UHP_Handler                  , // 47 USB OHCI
    (uint32_t)UDP_Handler                  , // 48 USB Device FS
    (uint32_t)CRCCU_Handler                , // 49 CRCCU
};

/******************************************************
 *               Function Definitions
 ******************************************************/
