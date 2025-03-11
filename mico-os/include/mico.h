/**
 ******************************************************************************
 * @file    MICO.h
 * @author  William Xu
 * @version V1.0.0
 * @date    16-Sep-2014
 * @brief   This file provides other MICO API header file and some basic APIs.
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


/** @mainpage MICO 

    This documentation describes the MICO APIs.
    It consists of:
     - MICO Core APIs   
     - MICO Hardware Abstract Layer APIs    
     - MICO Algorithm APIs        
     - MICO System APIs        
     - MICO Middleware APIs
     - MICO Drivers interface
 */

#ifndef __MICO_H_
#define __MICO_H_

/* MiCO SDK APIs */
#include "debug.h"
#include "common.h"
#include "mico_rtos.h"
#include "mico_wlan.h"
#include "mico_socket.h"
#include "mico_security.h"
#include "mico_platform.h"
#include "mico_system.h"


#define MicoGetRfVer                wlan_driver_version
#define MicoGetVer                  system_lib_version
#define MicoInit                    mxchipInit

/** @defgroup MICO_Core_APIs MICO Core APIs
  * @brief MiCO Initialization, RTOS, TCP/IP stack, and Network Management
  */

/** @addtogroup MICO_Core_APIs
  * @{
  */

/** \defgroup MICO_Init_Info Initialization and Tools
  * @brief Get MiCO version or RF version, flash usage information or init MiCO TCPIP stack
  * @{
 */

 /******************************************************
 *                    Structures
 ******************************************************/



/******************************************************
 *              Function Declarations
 ******************************************************/

/**
  * @brief  Get RF driver's version.
  *
  * @note   Create a memery buffer to store the version characters.
  *         THe input buffer length should be 40 bytes at least.
  * @note   This must be executed after micoInit().
  * @param  inVersion: Buffer address to store the RF driver. 
  * @param  inLength: Buffer size. 
  *
  * @return int
  */
int MicoGetRfVer( char* outVersion, uint8_t inLength );

/**
  * @brief  Get MICO's version.
  *
  * @param  None 
  *
  * @return Point to the MICO's version string.
  */
char* MicoGetVer( void );

/**
  * @brief  Initialize the TCPIP stack thread, RF driver thread, and other
            supporting threads needed for wlan connection. Do some necessary
            initialization
  *
  * @param  None
  *
  * @return kNoErr: success, kGeneralErr: fail
  */
OSStatus MicoInit( void );


/**
  * @brief  Get an identifier id from device, every id is unique and will not change in life-time
  *
  * @param  identifier length 
  *
  * @return Point to the identifier 
  */
const uint8_t* mico_generate_cid( uint8_t *length );

#endif /* __MICO_H_ */

/**
  * @}
  */

/**
  * @}
  */

