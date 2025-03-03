/**
 ******************************************************************************
 * @file    rtos.c
 * @author  William Xu
 * @version V1.0.0
 * @date    25-Aug-2016
 * @brief   Definitions of the MiCO RTOS abstraction layer for the special case
 *          of having no RTOS
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 ******************************************************************************
 */

#include "common.h"
#include "platform_peripheral.h"

#include "portmacro.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define SEMAPHORE_POOL_NUM      8
#define MUTEX_POOL_NUM          8

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef volatile struct _noos_semaphore_t
{
    uint8_t used;
    uint8_t count;
} noos_semaphore_t;

typedef volatile struct _noos_mutex_t
{
    uint8_t used;
    uint8_t reversed;
} noos_mutex_t;

/******************************************************
 *                    Structures
 ******************************************************/


/******************************************************
 *               Static Function Declarations
 ******************************************************/


/******************************************************
 *               Variable Definitions
 ******************************************************/

#ifdef  MICO_DEFAULT_TICK_RATE_HZ
uint32_t  ms_to_tick_ratio = (uint32_t)( 1000 / MICO_DEFAULT_TICK_RATE_HZ );
#else
uint32_t  ms_to_tick_ratio = 1; // Default OS tick is 1000Hz
#endif


uint8_t semaphore_pool_init = 0;
noos_semaphore_t semaphore_pool[SEMAPHORE_POOL_NUM];

uint8_t mutex_pool_init = 0;
noos_mutex_t mutex_pool[MUTEX_POOL_NUM];

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus semaphore_pool_alloc( noos_semaphore_t **semaphore )
{
	if(semaphore_pool_init == 0)
	{
		for(uint8_t i=0; i<SEMAPHORE_POOL_NUM; i++)
		{
			semaphore_pool[i].used = 0;
		}
		semaphore_pool_init = 1;
	}

	for(uint8_t i=0; i<SEMAPHORE_POOL_NUM; i++)
	{
		if(semaphore_pool[i].used == 0)
		{
			semaphore_pool[i].used = 1;
			*semaphore = &semaphore_pool[i];
			return kNoErr;
		}
	}
	return kGeneralErr;
}

OSStatus semaphore_pool_free( noos_semaphore_t **semaphore )
{
	if(semaphore != NULL)
	{
		if((*semaphore)->used == 1)
		{
			(*semaphore)->used = 0;
			return kNoErr;
		}
	}
	return kGeneralErr;
}

OSStatus mutex_pool_alloc( noos_mutex_t **mutex )
{
	if(mutex_pool_init == 0)
	{
		for(uint8_t i=0; i<MUTEX_POOL_NUM; i++)
		{
			mutex_pool[i].used = 0;
		}
		mutex_pool_init = 1;
	}

	for(uint8_t i=0; i<MUTEX_POOL_NUM; i++)
	{
		if(mutex_pool[i].used == 0)
		{
			mutex_pool[i].used = 1;
			*mutex = &mutex_pool[i];
			return kNoErr;
		}
	}
	return kGeneralErr;
}

OSStatus mutex_pool_free( noos_mutex_t **mutex )
{
	if(mutex != NULL)
	{
		if((*mutex)->used == 1)
		{
			(*mutex)->used = 0;
			return kNoErr;
		}
	}
	return kGeneralErr;
}

OSStatus mico_rtos_init_semaphore( mico_semaphore_t* semaphore, int count )
{
    noos_semaphore_t *noos_semaphore;
    UNUSED_PARAMETER( count );
    semaphore_pool_alloc(&noos_semaphore);
    noos_semaphore->count = 0;
    *semaphore = (void *)noos_semaphore;
    return kNoErr;
}

OSStatus mico_rtos_get_semaphore( mico_semaphore_t* semaphore, uint32_t timeout_ms )
{
    noos_semaphore_t *noos_semaphore = (noos_semaphore_t *)*semaphore;
    int delay_start;

    if( noos_semaphore == NULL)
        return kNotInitializedErr;

    delay_start = mico_rtos_get_time();
    while( noos_semaphore->count == 0){
      if(mico_rtos_get_time() >= delay_start + timeout_ms && timeout_ms != MICO_NEVER_TIMEOUT){
        return kTimeoutErr;
      }
    }

    DISABLE_INTERRUPTS();
    noos_semaphore->count--;
    ENABLE_INTERRUPTS();

    return kNoErr;
}

OSStatus mico_rtos_set_semaphore( mico_semaphore_t* semaphore )
{
    noos_semaphore_t *noos_semaphore = (noos_semaphore_t *)*semaphore;

    if( noos_semaphore == NULL)
        return kNotInitializedErr;

    DISABLE_INTERRUPTS();
    noos_semaphore->count++;
    ENABLE_INTERRUPTS();

    return kNoErr;
}

OSStatus mico_rtos_deinit_semaphore( mico_semaphore_t* semaphore )
{
    noos_semaphore_t *noos_semaphore = (noos_semaphore_t *)*semaphore;

    if( noos_semaphore == NULL)
        return kNotInitializedErr;

    semaphore_pool_free(&noos_semaphore);
    *semaphore = NULL;

    return kNoErr;
}


OSStatus mico_rtos_init_mutex( mico_mutex_t* mutex )
{
    noos_mutex_t *noos_mutex;
    mutex_pool_alloc(&noos_mutex);
    noos_mutex->reversed = 0;
    *mutex = (void *)noos_mutex;
    return kNoErr;
}


OSStatus mico_rtos_lock_mutex( mico_mutex_t* mutex )
{
    UNUSED_PARAMETER(mutex);
    return kNoErr;
}

OSStatus mico_rtos_unlock_mutex( mico_mutex_t* mutex )
{
    UNUSED_PARAMETER(mutex);
    return kNoErr;
}

OSStatus mico_rtos_deinit_mutex( mico_mutex_t* mutex )
{
    noos_mutex_t *noos_mutex = (noos_mutex_t *)*mutex;

    if( noos_mutex == NULL)
        return kNotInitializedErr;

    mutex_pool_free(&noos_mutex);
    *mutex = NULL;

    return kNoErr;    
}

extern uint32_t mico_get_time_no_os(void);

mico_time_t mico_rtos_get_time(void)
{
    uint32_t tick = mico_get_time_no_os( );
    return ms_to_tick_ratio * tick;
}

/**
 * Delay for a number of milliseconds
 *
 * Simply implemented with a tight loop
 *
 * @return OSStatus : kNoErr if delay was successful
 *
 */
OSStatus mico_rtos_delay_milliseconds( uint32_t num_ms )
{
    mico_time_t start = mico_rtos_get_time( );

    while ( ( mico_rtos_get_time( ) - start ) < num_ms )
    {
        /* do nothing */
    }

    return kNoErr;
}

OSStatus mico_rtos_init_timer( mico_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg )
{
    UNUSED_PARAMETER( timer );
    UNUSED_PARAMETER( time_ms );
    UNUSED_PARAMETER( function );
    UNUSED_PARAMETER( arg );
    return kUnsupportedErr;
}


OSStatus mico_rtos_start_timer( mico_timer_t* timer )
{
    UNUSED_PARAMETER( timer );
    return kUnsupportedErr;
}

OSStatus mico_rtos_stop_timer( mico_timer_t* timer )
{
    UNUSED_PARAMETER( timer );
    return kUnsupportedErr;
}

void mico_rtos_enter_critical( void )
{
}

void mico_rtos_exit_critical( void )
{
}

