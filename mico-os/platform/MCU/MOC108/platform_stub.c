/**
 ******************************************************************************
 * @file    platform_init.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide functions called by MICO to drive stm32f2xx
 *          platform: - e.g. power save, reboot, platform initialize
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include "stdio_newlib.h"

#include "common.h"

#undef errno
extern int errno;

#ifndef EBADF
#include <errno.h>
#endif

/************** wrap C library functions **************/
void * __wrap_malloc (size_t size)
{
	return pvPortMalloc(size);
}

void * __wrap__malloc_r (void *p, size_t size)
{
	
	return pvPortMalloc(size);
}

void __wrap_free (void *pv)
{
	vPortFree(pv);
}

void * __wrap_calloc (size_t a, size_t b)
{
	void *pvReturn;

    pvReturn = pvPortMalloc( a*b );
    if (pvReturn)
    {
        memset(pvReturn, 0, a*b);
    }

    return pvReturn;
}

void * __wrap_realloc (void* pv, size_t size)
{
	return pvPortRealloc(pv, size);
}

void __wrap__free_r (void *p, void *x)
{
  __wrap_free(x);
}

void* __wrap__realloc_r (void *p, void* x, size_t sz)
{
  return __wrap_realloc (x, sz);
}

int __wrap_printf (const char* format, ...)
{
  int size;
  va_list ap;
	static char print_buf[1024];

	va_start(ap, format);
	size = vsnprintf(print_buf, sizeof(print_buf) - 1, format, ap);
	va_end(ap);

	MicoUartSend(STDIO_UART, print_buf, size);

	return size;
}

int _gettimeofday( struct timeval * __p, void * __tz )
{
    mico_utc_time_ms_t current_utc_time_ms = 0;
    uint64_t current_time_ns;
    mico_time_get_utc_time_ms( &current_utc_time_ms );

    current_time_ns = mico_nanosecond_clock_value();
    __p->tv_sec = current_utc_time_ms / 1000;
    __p->tv_usec = ( current_utc_time_ms % 1000 ) * 1000 + ( current_time_ns / 1000 ) % 1000;

    return 0;
}

int gettimeofday( struct timeval *__restrict __p, void *__restrict __tz )
{
    return _gettimeofday( __p, __tz );
}


time_t time(time_t *tloc)
{
    mico_utc_time_ms_t current_utc_time_ms = 0;
    unsigned long long t;

    mico_time_get_utc_time_ms( &current_utc_time_ms );

    t = current_utc_time_ms / 1000;

    if (tloc)
        *tloc = t ;

    return t;
}

