/**
 ******************************************************************************
 * @file    FreeRTOSConfig.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-Aug-2018
 * @brief   This file provide the FreeRTOS system configurations.
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

/*
 FreeRTOS V6.1.0 - Copyright (C) 2010 Real Time Engineers Ltd.

 ***************************************************************************
 *                                                                         *
 * If you are:                                                             *
 *                                                                         *
 *    + New to FreeRTOS,                                                   *
 *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
 *    + Looking for basic training,                                        *
 *    + Wanting to improve your FreeRTOS skills and productivity           *
 *                                                                         *
 * then take a look at the FreeRTOS books - available as PDF or paperback  *
 *                                                                         *
 *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
 *                  http://www.FreeRTOS.org/Documentation                  *
 *                                                                         *
 * A pdf reference manual is also available.  Both are usually delivered   *
 * to your inbox within 20 minutes to two hours when purchased between 8am *
 * and 8pm GMT (although please allow up to 24 hours in case of            *
 * exceptional circumstances).  Thank you for your support!                *
 *                                                                         *
 ***************************************************************************

 This file is part of the FreeRTOS distribution.

 FreeRTOS is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License (version 2) as published by the
 Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
 ***NOTE*** The exception to the GPL is included to allow you to distribute
 a combined work that includes FreeRTOS without being obliged to provide the
 source code for proprietary components outside of the FreeRTOS kernel.
 FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 more details. You should have received a copy of the GNU General Public
 License and the FreeRTOS license exception along with FreeRTOS; if not it
 can be viewed here: http://www.freertos.org/a00114.html and also obtained
 by writing to Richard Barry, contact details for whom are available on the
 FreeRTOS WEB site.

 1 tab == 4 spaces!

 http://www.FreeRTOS.org - Documentation, latest information, license and
 contact details.

 http://www.SafeRTOS.com - A version that is certified for use in safety
 critical systems.

 http://www.OpenRTOS.com - Commercial support, development, porting,
 licensing and training services.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "mico_FreeRTOS_systick.h"
#include "platform_config.h"

#if defined ( __IAR_SYSTEMS_ICC__ )
/* This file is included from the IAR portasm.s, so must avoid C 
declarations in that case */
//#include "platform_sleep.h"
#endif /* if defined ( __IAR_SYSTEMS_ICC__ ) */

//#define MICO_NEW_OS_PRIO_DEF

#ifdef MXCHIP
extern const int CFG_PRIO_BITS;
#else
#define CFG_PRIO_BITS 4
#endif
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_TIMERS                            ( 1 )
#define configTIMER_TASK_PRIORITY                   ( 2 )
#define configTIMER_QUEUE_LENGTH                    ( 5 )
#define configTIMER_TASK_STACK_DEPTH                ( ( unsigned short ) (1024 / sizeof( portSTACK_TYPE )) )
#define configUSE_PREEMPTION                        ( 1 )
#define configUSE_IDLE_HOOK                         ( 0 )
#define configUSE_TICK_HOOK                         ( 0 )
//#define configCPU_CLOCK_HZ                          ( ( unsigned long ) CPU_CLOCK_HZ )
//#define configTICK_RATE_HZ                          ( ( portTickType ) SYSTICK_FREQUENCY )
#ifdef MICO_NEW_OS_PRIO_DEF
#define configMAX_PRIORITIES                        ( ( unsigned portBASE_TYPE ) 16 )
#else
#define configMAX_PRIORITIES                        ( ( unsigned portBASE_TYPE ) 10 )
#endif
#define configMINIMAL_STACK_SIZE                    ( ( unsigned short ) (500 / sizeof( portSTACK_TYPE )) ) /* size of idle thread stack */
#define configMAX_TASK_NAME_LEN                     ( 16 )

#define configUSE_16_BIT_TICKS                      ( 0 )
#define configIDLE_SHOULD_YIELD                     ( 1 )
#ifndef configUSE_MUTEXES
#define configUSE_MUTEXES                           ( 1 )
#endif /* ifndef configUSE_MUTEXES */
#define configUSE_COUNTING_SEMAPHORES               ( 1 )

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                       ( 0 )
#define configMAX_CO_ROUTINE_PRIORITIES             ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
 to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                    ( 1 )
#define INCLUDE_uxTaskPriorityGet                   ( 1 )
#define INCLUDE_vTaskDelete                         ( 1 )
#define INCLUDE_vTaskCleanUpResources               ( 0 )
#define INCLUDE_vTaskSuspend                        ( 1 )
#define INCLUDE_vTaskDelayUntil                     ( 1 )
#define INCLUDE_vTaskDelay                          ( 1 )
#define INCLUDE_vTaskForceAwake                     ( 1 )

/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
 (lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY             ( 255 )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY        (1 << (8 - CFG_PRIO_BITS))


/* This is the value being used as per the ST library which permits 16
 priority values, 0 to 15.  This must correspond to the
 configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
 NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY     ((1 << CFG_PRIO_BITS) - 1)

/* Check for stack overflows - requires defining vApplicationStackOverflowHook */
#define configCHECK_FOR_STACK_OVERFLOW              ( 2 )

/* Run a handler if a malloc fails - vApplicationMallocFailedHook */
#define configUSE_MALLOC_FAILED_HOOK                ( 1 )

#ifdef MXCHIP
#define configUSE_TRACE_FACILITY                    ( 1 )
#endif
#ifndef configUSE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY                    ( 0 )
#endif /* configUSE_TRACE_FACILITY */

#ifdef MICO_DISABLE_MCU_POWERSAVE

#define configUSE_IDLE_SLEEP_HOOK ( 1 )

#else /* ifdef MICO_DISABLE_MCU_POWERSAVE */

#define configUSE_IDLE_NO_TICK_SLEEP_HOOK ( 1 )

#endif /* ifdef MICO_DISABLE_MCU_POWERSAVE */



#endif /* FREERTOS_CONFIG_H */

