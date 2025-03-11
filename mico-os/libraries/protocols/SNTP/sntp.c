/**
 ******************************************************************************
 * @file    sntp.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   Create a NTP client timed enent, and synchronize with NTP server,
 *          generate callback when synced.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include <time.h>

#include "mico.h"
#include "sntp.h"
#include "TimeUtils.h"
#include "SocketUtils.h"

#ifdef DEBUG
#define ntp_log(M, ...) custom_log("SNTP", M, ##__VA_ARGS__)
#define ntp_log_trace() custom_log_trace("SNTP")
#else
#define ntp_log(M, ...)
#define ntp_log_trace()
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define DEFAULT_NTP_Server   "pool.ntp.org"

#define NTP_EPOCH            (86400U * (365U * 70U + 17U))
#define NTP_PORT             123

#ifndef MICO_NTP_REPLY_TIMEOUT
#define MICO_NTP_REPLY_TIMEOUT 300
#endif

#define MAX_NTP_ATTEMPTS     3
#define TIME_BTW_ATTEMPTS    5000
/* RFC4330 recommends min 15s between polls */
#define MIN_POLL_INTERVAL    15 * 1000

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/*
 * Taken from RFC 1305
 * http://www.ietf.org/rfc/rfc1305.txt
 */
typedef struct
{
    unsigned int mode : 3;
    unsigned int vn   : 3;
    unsigned int li   : 2;
    uint8_t      stratum;
    int8_t       poll;
    uint32_t     root_delay;
    uint32_t     root_dispersion;
    uint32_t     reference_identifier;
    uint32_t     reference_timestamp_seconds;
    uint32_t     reference_timestamp_fraction;
    uint32_t     originate_timestamp_seconds;
    uint32_t     originate_timestamp_fraction;
    uint32_t     receive_timestamp_seconds;
    uint32_t     receive_timestamp_fraction;
    uint32_t     transmit_timestamp_seconds;
    uint32_t     transmit_timestamp_fraction;
} ntp_packet_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static OSStatus sync_ntp_time( void* arg );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static time_synced_fun time_synced_call_back = NULL;
static mico_timed_event_t sync_ntp_time_event;
/* Only support primary and secondary servers */
static struct in_addr ntp_server[2];

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus sntp_start_auto_time_sync( uint32_t interval_ms, time_synced_fun call_back )
{
    OSStatus err = kNoErr;
    uint8_t random_initial;

    time_synced_call_back = call_back;
    /* Synchronize time with NTP server and schedule for re-sync every one day */
    MicoRandomNumberRead( &random_initial, 1 );
    /* prevent thundering herd scenarios by randomizing per RFC4330 */
    //mico_rtos_delay_milliseconds(300 * (unsigned int)random_initial);
    mico_rtos_send_asynchronous_event( MICO_NETWORKING_WORKER_THREAD, sync_ntp_time, 0 );
    if ( interval_ms < MIN_POLL_INTERVAL )
        interval_ms = MIN_POLL_INTERVAL;
    mico_rtos_register_timed_event( &sync_ntp_time_event, MICO_NETWORKING_WORKER_THREAD, sync_ntp_time, interval_ms,
                                    0 );
    return err;
}

OSStatus sntp_set_server_ip_address( uint32_t index, struct in_addr address )
{
    if ( (index != 0) && (index != 1) )
        return kParamErr;

    ntp_server[index] = address;
    return kNoErr;
}

OSStatus sntp_clr_server_ip_address( uint32_t index )
{
    if ( index > 1 )
        return kParamErr;

    ntp_server[index].s_addr = 0;
    return kNoErr;
}

OSStatus sntp_stop_auto_time_sync( void )
{
    return mico_rtos_deregister_timed_event( &sync_ntp_time_event );
}

OSStatus sntp_get_time( const struct in_addr *ntp_server_ip, ntp_timestamp_t* timestamp)
{
    int                Ntp_fd = -1;
    OSStatus           err;
    ntp_packet_t       data;
    mico_utc_time_t    utc_time;
    fd_set             readfds;
    struct timeval     t;
    struct sockaddr_in self_addr, remote_addr;
    socklen_t          remote_addr_len;
    uint32_t           client_sent_timestamp;

    Ntp_fd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    require_action( IsValidSocket( Ntp_fd ), exit, err = kNoResourcesErr );

    self_addr.sin_family = AF_INET;
    self_addr.sin_addr.s_addr = INADDR_ANY;
    self_addr.sin_port = htons(NTP_PORT);

    err = bind( Ntp_fd, (struct sockaddr *)&self_addr, sizeof(struct sockaddr_in) );
    require_noerr( err, exit );

    t.tv_sec = 3;
    t.tv_usec = 0;

    /* Fill packet contents */
    mico_time_get_utc_time( &utc_time );
    memset( &data, 0, sizeof(ntp_packet_t) );
    data.li = 3;
    data.vn = 3;
    data.mode = 3;
    data.poll = 17;
    data.transmit_timestamp_seconds = Swap32( utc_time + NTP_EPOCH );
    client_sent_timestamp = data.transmit_timestamp_seconds;

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = ntp_server_ip->s_addr;
    remote_addr.sin_port = htons(NTP_PORT);

    require_action(
        sendto( Ntp_fd, &data, sizeof(ntp_packet_t), 0, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr) ),
        exit, err = kNotWritableErr );

    FD_ZERO( &readfds );
    FD_SET( Ntp_fd, &readfds );

    require_action( select( Ntp_fd + 1, &readfds, NULL, NULL, &t ) >= 0, exit, err = kUnexpectedErr );

    require_action( FD_ISSET( Ntp_fd, &readfds ), exit, err = kTimeoutErr );

    require_action(
        recvfrom( Ntp_fd, &data, sizeof(ntp_packet_t), 0, (struct sockaddr *)&remote_addr, &remote_addr_len ), exit,
        err = kNotReadableErr );

    require_action_string( client_sent_timestamp == data.originate_timestamp_seconds, exit,
                           err = kResponseErr,
                           "Server Returned Bad Originate TimeStamp" );

    require_action_string( data.li <= 2 && data.vn == 3 && data.stratum != 0 && data.stratum <= 15
                           && data.transmit_timestamp_seconds != 0
                           && data.mode == 4,
                           exit, err = kResponseErr, "Invalid Protocol Parameters returned" );

    timestamp->seconds = Swap32( data.transmit_timestamp_seconds ) - NTP_EPOCH;
    timestamp->microseconds = Swap32( data.transmit_timestamp_fraction ) / 4295; /* 4295 = 2^32 / 10^6 */

    ntp_log("Time Synchronized, %s",asctime(localtime((const time_t *)&timestamp->seconds)));

    exit:
    if ( err != kNoErr ) ntp_log("Exit: NTP client exit with err = %d", err);
    SocketClose( &Ntp_fd );
    return err;
}

static OSStatus sync_ntp_time( void* arg )
{
    OSStatus             err = kGeneralErr;
    ntp_timestamp_t      current_time;
    struct hostent *     hostent_content = NULL;
    struct in_addr       ntp_server_ip;
    char **              pptr = NULL;
    uint32_t             i;

    UNUSED_PARAMETER( arg );

    /* Get the time */
    ntp_log( "Getting NTP time... ");

    for ( i = 0; i < MAX_NTP_ATTEMPTS; i++ )
    {
        /* First check if there are local servers to use */
        if ( ntp_server[0].s_addr != 0 )
        {
            ntp_log( "Sending request primary ..." );
            err = sntp_get_time( &ntp_server[0], &current_time );
        }
        if ( err != kNoErr && (ntp_server[1].s_addr != 0) )
        {
            ntp_log( "Sending request secondary ...");
            err = sntp_get_time( &ntp_server[1], &current_time );
        }
        /* only fall back to global servers if we can't get local */
        if ( err != kNoErr )
        {
            ntp_log("Resolving SNTP server address ...");
            hostent_content = gethostbyname( DEFAULT_NTP_Server );
            if( hostent_content == NULL )
            {
                ntp_log("SNTP server address can not be resolved");
                return kNotFoundErr;
            }
            pptr=hostent_content->h_addr_list;
            ntp_server_ip.s_addr = *(uint32_t *)(*pptr);
            ntp_log("SNTP server address: %s, host ip: %s", DEFAULT_NTP_Server, inet_ntoa(ntp_server_ip));
            err = sntp_get_time( &ntp_server_ip, &current_time );
        }

        if ( err == kNoErr )
        {
            ntp_log( "success" );
            break;
        }
        else
        {
            mico_rtos_delay_milliseconds( TIME_BTW_ATTEMPTS );
            ntp_log( "failed, trying again..." );
        }
    }

    if ( i >= MAX_NTP_ATTEMPTS )
    {
        ntp_log( "Give up getting NTP time\n" );
        memset( &current_time, 0, sizeof(current_time) );
        return kTimeoutErr;
    }
    else
    {
        mico_utc_time_ms_t utc_time_ms = (uint64_t) current_time.seconds * (uint64_t) 1000
            + (current_time.microseconds / 1000);

        /* Set & Print the time */
        mico_time_set_utc_time_ms( &utc_time_ms );
    }

    if ( time_synced_call_back ) time_synced_call_back( );
#ifdef DEBUG
    iso8601_time_t iso8601_time;
    mico_time_get_iso8601_time( &iso8601_time );
    ntp_log("Current time is: %.26s\n", (char*)&iso8601_time);
#endif
    return kNoErr;
}
