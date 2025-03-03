/**
 ******************************************************************************
 * @file    MicoDriverI2s.h
 * @author  William Xu
 * @version V1.0.0
 * @date    16-Sep-2014
 * @brief   This file provides all the headers of IIS operation functions.
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

#ifndef __MICODRIVERI2S_H__
#define __MICODRIVERI2S_H__

#pragma once
#include "common.h"
#include "platform.h"
#include "platform_peripheral.h"

/** @addtogroup MICO_PLATFORM
* @{
*/
/** @defgroup MICO_SPI MICO IIS Driver
* @brief  IIS Functions
* @{
*/
/******************************************************
 *                    Constants
 ******************************************************/


/******************************************************
 *                   Enumerations
 ******************************************************/


/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    mico_iis_t   port;
    uint32_t     bits;
    uint8_t      master_enable;// 1=master mode; 0=slave mode
    uint32_t     clk_freq;
    bool         is_mono_channel;
} mico_iis_device_t;

typedef platform_iis_message_segment_t mico_iis_message_segment_t;

/******************************************************
 *                     Variables
 ******************************************************/

/******************************************************
 *                 Function Declarations
 ******************************************************/

/**@brief Initialises the IIS interface for a given IIS device
 *
 * @note  Prepares a IIS hardware interface for communication as a master
 *
 * @param  IIS : the iis device to be initialised
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if the iis device could not be initialised
 */
OSStatus MicoIISInitialize( const mico_iis_device_t* iis );


/**@brief Transmits and/or receives data from a IIS device
 *
 * @param  iis      : the IIS device to be initialised
 * @param  segments : a pointer to an array of segments
 * @param  number_of_segments : the number of segments to transfer
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred
 */
OSStatus MicoIISTransfer( const mico_iis_device_t* iis, const mico_iis_message_segment_t* segments, uint16_t number_of_segments );


/**@brief De-initialises a IIS interface
 *
 * @note Turns off a IIS hardware interface
 *
 * @param  iis : the IIS device to be de-initialised
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred
 */
OSStatus MicoIISFinalize( const mico_iis_device_t* iis );

/**@brief Write through IIS interface
 *
 * @note Write through IIS hardware interface
 *
 * @param  iis : the IIS device to be de-initialised
 * @param  p_buf : buf to be transfer
 * @param  size: size of buf
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred
 */
OSStatus MicoIISWrite( const mico_iis_device_t* iis, uint8_t *p_buf, uint32_t size );

/**@brief Read through IIS interface
 *
 * @note Read through IIS hardware interface
 *
 * @param  iis : the IIS device to be de-initialised
 * @param  p_buf : buf to be receive
 * @param  size: size of buf
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred
 */
OSStatus MicoIISRead( const mico_iis_device_t* iis, uint8_t *p_buf, uint32_t size );

/** @} */
/** @} */

#endif
