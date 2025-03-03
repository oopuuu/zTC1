/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */

#include "sys/time.h"
#include "common.h"
#include "mico_system.h"

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
