/**
 ******************************************************************************
 * @file    portmacro.c
 * @author  William Xu
 * @version V1.0.0
 * @date    06-Jun-2017
 * @brief   This file provide the NoRTOS portable MACROS
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
#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#define DISABLE_INTERRUPTS() do { __asm("CPSID i"); } while (0)
#define ENABLE_INTERRUPTS() do { __asm("CPSIE i"); } while (0)

#ifdef __cplusplus
}
#endif

#endif









