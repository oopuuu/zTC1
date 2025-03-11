/**
 ******************************************************************************
 * @file    sntp.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   Provide sntp client header files.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
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

#pragma once

#include "common.h"


/** @addtogroup MICO_Middleware_Interface
  * @{
  */

/** @defgroup MICO_Network_Time MiCO Network Time APIs
  * @brief Provide MiCO APIs to get standard Network time .
  * @{
  */

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

typedef struct
{
    uint32_t seconds;
    uint32_t microseconds;
} ntp_timestamp_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

typedef void (* time_synced_fun)( void );

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus sntp_start_auto_time_sync( uint32_t interval_ms, time_synced_fun call_back );
OSStatus sntp_stop_auto_time_sync( void );
OSStatus sntp_get_time( const struct in_addr *addr, ntp_timestamp_t* timestamp );
OSStatus sntp_set_server_ip_address(uint32_t index, struct in_addr address);
OSStatus sntp_clr_server_ip_address(uint32_t index);


/**
  * @}
  */


/**
  * @}
  */
