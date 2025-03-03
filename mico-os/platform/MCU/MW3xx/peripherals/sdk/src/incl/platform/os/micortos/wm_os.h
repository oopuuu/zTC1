/*! \file wm_os.h
 *  \brief WMSDK OS Abstraction Layer
 *
 * The WMSDK OS abstraction layer provides wrapper APIs over some of the
 * commonly used OS primitives. Since the behaviour and semantics of the various
 * OSes differs widely, some abstraction APIs require a specific handling as
 * listed below.
 *
 * \section wm_os_usage Usage
 *
 * The WMSDK OS abstraction layer provides the following types of primitives:
 *
 * - Thread: Create or delete a thread using os_thread_create() or
 *    os_thread_delete(). Block a thread using os_thread_sleep(). Complete a
 *    thread's execution using os_thread_self_complete().
 * - Message Queue: Create or delete a message queue using os_queue_create() or
 *    os_queue_delete(). Send a message using os_queue_send() and received a
 *    message using os_queue_recv().
 * - Mutex: Create or delete a mutex using os_mutex_create() or
 *    os_mutex_delete(). Acquire a mutex using os_mutex_get() and release it
 *    using os_mutex_put().
 * - Semaphores: Create or delete a semaphore using os_semaphore_create() /
 *    os_semaphore_create_counting() or os_semaphore_delete. Acquire a semaphore
 *    using os_semaphore_get() and release it using os_semaphore_put().
 * - Timers: Create or delete a timer using os_timer_create() or
 *    os_timer_delete(). Change the timer using os_timer_change(). Activate or
 *    de-activate the timer using os_timer_activate() or
 *    os_timer_deactivate(). Reset a timer using os_timer_reset().
 * - Dynamic Memory Allocation: Dynamically allocate memory using
 *    os_mem_alloc(), os_mem_calloc() or os_mem_realloc() and free it using
 *    os_mem_free().
 *
 */
/* Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef _WM_OS_H_
#define _WM_OS_H_

#include <stdio.h>
#include <string.h>

#include "mico_rtos.h"

#include <wmerrno.h>
#include <wm_utils.h>
#include <wmstdio.h>
#include <wmlog.h>
//#include <semdbg.h>
#include <board.h>

#define OS_WAIT_FOREVER    MICO_WAIT_FOREVER


/*** Mutex ***/
typedef mico_mutex_t os_mutex_t;

/** Priority Inheritance Enabled */
#define OS_MUTEX_INHERIT     1
/** Priority Inheritance Disabled */
#define OS_MUTEX_NO_INHERIT  0


static inline int os_mutex_create(os_mutex_t *mhandle,
                  const char *name, int flags) WARN_UNUSED_RET;
/** Create mutex
 *
 * This function creates a mutex.
 *
 * @param [out] mhandle Pointer to a mutex handle
 * @param [in] name Name of the mutex
 * @param [in] flags Priority inheritance selection. Valid options are \ref
 * OS_MUTEX_INHERIT or \ref OS_MUTEX_NO_INHERIT.
 *
 * @note Currently non-inheritance in mutex is not supported.
 *
 * @return WM_SUCCESS on success
 * @return -WM_FAIL on error
 */
static inline int os_mutex_create(os_mutex_t *mhandle, const char *name, int flags)
{
    return  mico_rtos_init_mutex( mhandle );
}

/** Acquire mutex
 *
 * This function acquires a mutex. Only one thread can acquire a mutex at any
 * given time. If already acquired the callers will be blocked for the specified
 * time duration.
 *
 * @param[in] mhandle Pointer to mutex handle
 * @param[in] wait The maximum amount of time, in OS ticks, the task should
 * block waiting for the mutex to be acquired. The function os_msec_to_ticks()
 * can be used to convert from real-time to OS ticks. The special values \ref
 * OS_WAIT_FOREVER and \ref OS_NO_WAIT are provided to respectively wait
 * infinitely or return immediately.
 *
 * @return WM_SUCCESS when mutex is acquired
 * @return -WM_E_INVAL if invalid parameters are passed
 * @return -WM_FAIL on failure
 */
static inline int os_mutex_get(os_mutex_t *mhandle, unsigned long wait)
{
    return mico_rtos_lock_mutex( mhandle );
}

/** Release mutex
 *
 * This function releases a mutex previously acquired using os_mutex_get().
 *
 * @note The mutex should be released from the same thread context from which it
 * was acquired. If you wish to acquire and release in different contexts,
 * please use os_semaphore_get() and os_semaphore_put() variants.
 *
 * @param[in] mhandle Pointer to the mutex handle
 *
 * @return WM_SUCCESS when mutex is released
 * @return -WM_E_INVAL if invalid parameters are passed
 * @return -WM_FAIL on failure
 */
static inline int os_mutex_put(os_mutex_t *mhandle)
{
    return mico_rtos_unlock_mutex( mhandle );
}

/** Delete mutex
 *
 * This function deletes a mutex.
 *
 * @param[in] mhandle Pointer to the mutex handle
 *
 * @note A mutex should not be deleted if other tasks are blocked on it.
 *
 * @return WM_SUCCESS on success
 */
static inline int os_mutex_delete(os_mutex_t *mhandle)
{
    return mico_rtos_deinit_mutex( mhandle );
}

/* Critical Sections */
static inline unsigned long os_enter_critical_section()
{
    mico_rtos_enter_critical();
    return WM_SUCCESS;
}

static inline void os_exit_critical_section(unsigned long state)
{
    mico_rtos_exit_critical();
}


#endif /* ! _WM_OS_H_ */
