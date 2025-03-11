/**
 ******************************************************************************
 * @file    mico_rtos.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-Aug-2018
 * @brief   This file provide the MiCO RTOS abstract layer functions.
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

#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "moc_api.h"

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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

extern const mico_api_t *lib_api_p;

static mico_time_t mico_time_offset = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

/* OS Layer*/
OSStatus mico_rtos_create_thread( mico_thread_t* thread, uint8_t priority, const char* name, mico_thread_function_t function, uint32_t stack_size, mico_thread_arg_t arg )
{
    return lib_api_p->mico_rtos_create_thread( thread, priority, name, function, stack_size, (void *)arg );
}

OSStatus mico_rtos_delete_thread( mico_thread_t* thread )
{
    return lib_api_p->mico_rtos_delete_thread( thread );
}

void mico_rtos_suspend_thread(mico_thread_t* thread)
{
    lib_api_p->mico_rtos_suspend_thread(thread);
}

void mico_rtos_suspend_all_thread(void)
{
    lib_api_p->mico_rtos_suspend_all_thread();
}

long mico_rtos_resume_all_thread(void)
{
    return lib_api_p->mico_rtos_resume_all_thread();
}

OSStatus mico_rtos_thread_join( mico_thread_t* thread )
{
    return lib_api_p->mico_rtos_thread_join(thread);
}

OSStatus mico_rtos_thread_force_awake( mico_thread_t* thread )
{
    return lib_api_p->mico_rtos_thread_force_awake(thread);
}

bool mico_rtos_is_current_thread( mico_thread_t* thread )
{
    return lib_api_p->mico_rtos_is_current_thread(thread);
}

OSStatus mico_rtos_init_semaphore( mico_semaphore_t* semaphore, int count )
{
    return lib_api_p->mico_rtos_init_semaphore(semaphore, count);
}
OSStatus mico_rtos_set_semaphore( mico_semaphore_t* semaphore )
{
    return lib_api_p->mico_rtos_set_semaphore(semaphore);
}
OSStatus mico_rtos_get_semaphore( mico_semaphore_t* semaphore, uint32_t timeout_ms )
{
    return lib_api_p->mico_rtos_get_semaphore(semaphore, timeout_ms);
}
OSStatus mico_rtos_deinit_semaphore( mico_semaphore_t* semaphore )
{
    return lib_api_p->mico_rtos_deinit_semaphore(semaphore);
}
OSStatus mico_rtos_init_mutex( mico_mutex_t* mutex )
{
    return lib_api_p->mico_rtos_init_mutex( mutex );
}
OSStatus mico_rtos_lock_mutex( mico_mutex_t* mutex )
{
    return lib_api_p->mico_rtos_lock_mutex( mutex );
}
OSStatus mico_rtos_unlock_mutex( mico_mutex_t* mutex )
{
    return lib_api_p->mico_rtos_unlock_mutex( mutex );
}
OSStatus mico_rtos_deinit_mutex( mico_mutex_t* mutex )
{
    return lib_api_p->mico_rtos_deinit_mutex( mutex );
}
OSStatus mico_rtos_init_queue( mico_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages )
{
    return lib_api_p->mico_rtos_init_queue( queue, name, message_size, number_of_messages );
}
OSStatus mico_rtos_push_to_queue( mico_queue_t* queue, void* message, uint32_t timeout_ms )
{
    return lib_api_p->mico_rtos_push_to_queue( queue, message, timeout_ms );
}
OSStatus mico_rtos_pop_from_queue( mico_queue_t* queue, void* message, uint32_t timeout_ms )
{
    return lib_api_p->mico_rtos_pop_from_queue( queue, message, timeout_ms );
}
OSStatus mico_rtos_deinit_queue( mico_queue_t* queue )
{
    return lib_api_p->mico_rtos_deinit_queue( queue );
}
bool mico_rtos_is_queue_empty( mico_queue_t* queue )
{
    return lib_api_p->mico_rtos_is_queue_empty( queue );
}
bool mico_rtos_is_queue_full( mico_queue_t* queue )
{
    return lib_api_p->mico_rtos_is_queue_full( queue );
}

OSStatus mico_init_timer( mico_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg )
{
    return lib_api_p->mico_init_timer( timer, time_ms, function, arg );
}
OSStatus mico_start_timer( mico_timer_t* timer )
{
    return lib_api_p->mico_start_timer( timer );
}
OSStatus mico_stop_timer( mico_timer_t* timer )
{
    return lib_api_p->mico_stop_timer( timer );
}
OSStatus mico_reload_timer( mico_timer_t* timer )
{
    return lib_api_p->mico_reload_timer( timer );
}
OSStatus mico_deinit_timer( mico_timer_t* timer )
{
    return lib_api_p->mico_deinit_timer( timer );
}
bool mico_is_timer_running( mico_timer_t* timer )
{
    return lib_api_p->mico_is_timer_running( timer );
}
int mico_create_event_fd(mico_event_t handle)
{
    return lib_api_p->mico_create_event_fd(handle);
}
int mico_delete_event_fd(int fd)
{
    return lib_api_p->mico_delete_event_fd(fd);
}

/**
 * Gets time in milliseconds since RTOS start
 *
 * @Note: since this is only 32 bits, it will roll over every 49 days, 17 hours.
 *
 * @returns Time in milliseconds since RTOS started.
 */
mico_time_t mico_rtos_get_time( void )
{
    return lib_api_p->mico_get_time();
}

OSStatus mico_time_get_time( mico_time_t* time_ptr )
{
    *time_ptr = lib_api_p->mico_get_time( ) + mico_time_offset;
    return kNoErr;
}

OSStatus mico_time_set_time( const mico_time_t* time_ptr )
{
    mico_time_offset = *time_ptr - lib_api_p->mico_get_time( );
    return kNoErr;
}


/**
 * Delay for a number of milliseconds
 *
 * Processing of this function depends on the minimum sleep
 * time resolution of the RTOS.
 * The current thread sleeps for the longest period possible which
 * is less than the delay required, then makes up the difference
 * with a tight loop
 *
 * @return OSStatus : kNoErr if delay was successful
 *
 */
OSStatus mico_rtos_delay_milliseconds( uint32_t num_ms )
{
    lib_api_p->mico_thread_msleep(num_ms);
    return kNoErr;
}

void *mico_malloc( size_t xWantedSize )
{
	return lib_api_p->malloc(xWantedSize);
}

void mico_free( void *pv )
{
	lib_api_p->free(pv);
}

void *mico_realloc( void *pv, size_t xWantedSize )
{
	return lib_api_p->realloc(pv, xWantedSize);
}

