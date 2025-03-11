/**
 ******************************************************************************
 * @file    rtos.h
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


#pragma once


/******************************************************
 *                      Macros
 ******************************************************/

//#define MICO_PRIORITY_TO_NATIVE_PRIORITY(priority) (uint8_t)(RTOS_HIGHEST_PRIORITY - priority)
//
//#define MICO_GET_THREAD_HANDLE( thread )           (thread)
//#define MICOD_GET_QUEUE_HANDLE( queue )             (queue)
//
//#define RTOS_HIGHER_PRIORTIY_THAN(x)     (x < RTOS_HIGHEST_PRIORITY ? x+1 : RTOS_HIGHEST_PRIORITY)
//#define RTOS_LOWER_PRIORTIY_THAN(x)      (x > RTOS_LOWEST_PRIORITY ? x-1 : RTOS_LOWEST_PRIORITY)
//#define RTOS_LOWEST_PRIORITY             (0)
//#define RTOS_HIGHEST_PRIORITY            (configMAX_PRIORITIES-1)
//#define RTOS_DEFAULT_THREAD_PRIORITY     (1)

/******************************************************
 *                    Constants
 ******************************************************/

/* Configuration of Built-in Worker Threads
 *
 * 1. mico_hardware_io_worker_thread is designed to handle deferred execution of quick, non-blocking hardware I/O operations.
 *    - priority         : higher than that of mico_networking_worker_thread
 *    - stack size       : small. Consequently, no printf is allowed here.
 *    - event queue size : the events are quick; therefore, large queue isn't required.
 *
 * 2. mico_networking_worker_thread is designed to handle deferred execution of networking operations
 *    - priority         : lower to allow mico_hardware_io_worker_thread to preempt and run
 *    - stack size       : considerably larger than that of mico_hardware_io_worker_thread because of the networking functions.
 *    - event queue size : larger than that of mico_hardware_io_worker_thread because networking operation may block
 */
#define HARDWARE_IO_WORKER_THREAD_STACK_SIZE                                   (512)
#define HARDWARE_IO_WORKER_THREAD_QUEUE_SIZE                                    (10)

#define NETWORKING_WORKER_THREAD_STACK_SIZE                               (2 * 1024)
#define NETWORKING_WORKER_THREAD_QUEUE_SIZE                                     (15)

#define RTOS_NAME                     "mocOS"
#define RTOS_VERSION                  mocOS_VERSION

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus init_timer_thread(void);
