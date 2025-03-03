/**
 ******************************************************************************
 * @file    port.c
 * @author  William Xu
 * @version V1.0.0
 * @date    06-Jun-2017
 * @brief   This file provide the NoRTOS portable functions
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2017 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */
#include <stdlib.h>
#include <stdint.h>

/*-----------------------------------------------------------*/

uint32_t platform_is_in_irq_enable( void )
{
    #define ARM968_IF_MASK      0xC0
    #define ARM968_IRQ_ENABLE   0x80

    uint32_t interrupt;

    __asm volatile(
        "MRS %0,CPSR\n"
        "AND %0,%0,#0xC0\n"
        :"=r" (interrupt)
        :
        :"memory"
    );

    return (!(interrupt & ARM968_IRQ_ENABLE));
}

uint32_t platform_is_in_fiq_enable( void )
{
    #define ARM968_IF_MASK      0xC0
    #define ARM968_FIQ_ENABLE   0x40

    uint32_t interrupt;

    __asm volatile(
        "MRS %0,CPSR\n"
        "AND %0,%0,#0xC0\n"
        :"=r" (interrupt)
        :
        :"memory"
    );

    return (!(interrupt & ARM968_FIQ_ENABLE));
}

/*-----------------------------------------------------------*/

uint32_t platform_is_in_irq_context( void )
{
    #define ARM968_IRQ_MODE      0x12

    uint32_t mode;

    __asm volatile(
        "MRS %0,CPSR\n"
        "AND %0,%0,#0x1f\n"
        :"=r" (mode)
        :
        :"memory"
    );

    return (ARM968_IRQ_MODE == mode);
}


uint32_t platform_is_in_fiq_context( void )
{
    #define ARM968_FIQ_MODE      0x11

    uint32_t mode;

    __asm volatile(
        "MRS %0,CPSR\n"
        "AND %0,%0,#0x1f\n"
        :"=r" (mode)
        :
        :"memory"
    );

    return (ARM968_FIQ_MODE == mode);
}

/*-----------------------------------------------------------*/

uint8_t platform_is_in_interrupt_context( void )
{
    return ((platform_is_in_fiq_context()) || (platform_is_in_irq_context()));
}
