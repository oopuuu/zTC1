/**
 ******************************************************************************
 * @file    MicoDriverFlash.h
 * @author  William Xu
 * @version V1.0.0
 * @date    16-Sep-2014
 * @brief   This file provides all the headers of Flash operation functions.
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

#ifndef __MICODRIVERFLASH_H__
#define __MICODRIVERFLASH_H__

#pragma once
#include "common.h"
#include "platform.h"

/** @addtogroup MICO_PLATFORM
* @{
*/

/** @defgroup MICO_FLASH MICO Flash Driver
* @brief  Flash operation Functions
* @{
*/
/******************************************************
 *                   Macros
 ******************************************************/  

#define PAR_OPT_READ_POS      ( 0 )
#define PAR_OPT_WRITE_POS     ( 1 )

#define PAR_OPT_READ_MASK  	  ( 0x1u << PAR_OPT_READ_POS )
#define PAR_OPT_WRITE_MASK 	  ( 0x1u << PAR_OPT_WRITE_POS )

#define PAR_OPT_READ_DIS      ( 0x0u << PAR_OPT_READ_POS )
#define PAR_OPT_READ_EN       ( 0x1u << PAR_OPT_READ_POS )
#define PAR_OPT_WRITE_DIS     ( 0x0u << PAR_OPT_WRITE_POS )
#define PAR_OPT_WRITE_EN      ( 0x1u << PAR_OPT_WRITE_POS )

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
     
typedef enum
{
    MICO_PARTITION_ERROR = -1,
    MICO_PARTITION_BOOTLOADER = MICO_PARTITION_USER_MAX,
    MICO_PARTITION_APPLICATION,
    MICO_PARTITION_ATE,
    MICO_PARTITION_OTA_TEMP,
    MICO_PARTITION_RF_FIRMWARE,
    MICO_PARTITION_PARAMETER_1,
    MICO_PARTITION_PARAMETER_2,
#ifdef MICO_USE_BT_PARTITION
    MICO_PARTITION_BT_FIRMWARE,
#endif
    MICO_PARTITION_MAX,
    MICO_PARTITION_NONE,
} mico_partition_t;

typedef platform_logic_partition_t  mico_logic_partition_t;


/******************************************************
 *                 Global Variables
 ******************************************************/

extern const char*  flash_name[];  /**< A name string of a Flash drive */

 /******************************************************
 *                 Function Declarations
 ******************************************************/

/**@brief   Get the infomation of the specified flash area 
 *
 * @param   inPartition:  The target flash logical partition which should be erased
 *
 * @return  mico_logi_partition struct
 */
mico_logic_partition_t* MicoFlashGetInfo( mico_partition_t inPartition );


/**@brief   Erase an area on a Flash logical partition
 *
 * @note    Erase on an address will erase all data on a sector that the 
 *          address is belonged to, this function does not save data that
 *          beyond the address area but in the affected sector, the data
 *          will be lost.
 *
 * @param  inPartition     : The target flash logical partition which should be erased
 * @param  off_set         : Start address of the erased flash area
 * @param  size    	   : Size of the erased flash area
 *
 * @return  kNoErr        : On success.
 * @return  kGeneralErr   : If an error occurred with any step
 */
OSStatus MicoFlashErase(mico_partition_t inPartition, uint32_t off_set, uint32_t size);

/**@brief  Write data to an area on a Flash logical partition
 *
 * @param  inPartition    : The target flash logical partition which should be read which should be written
 * @param  off_set        : Point to the start address that the data is written to, and
 *                          point to the last unwritten address after this function is 
 *                          returned, so you can call this function serval times without
 *                          update this start address.
 * @param  inBuffer       : point to the data buffer that will be written to flash
 * @param  inBufferLength : The length of the buffer
 *
 * @return  kNoErr        : On success.
 * @return  kGeneralErr   : If an error occurred with any step
 */
OSStatus MicoFlashWrite( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* inBuffer ,uint32_t inBufferLength);

/**@brief    Read data from an area on a Flash to data buffer in RAM
 *
 * @param    inPartition    : The target flash logical partition which should be read
 * @param    off_set        : Point to the start address that the data is read, and
 *                          point to the last unread address after this function is 
 *                          returned, so you can call this function serval times without
 *                          update this start address.
 * @param    outBuffer      : Point to the data buffer that stores the data read from flash
 * @param    inBufferLength : The length of the buffer
 *
 * @return    kNoErr        : On success.
 * @return    kGeneralErr   : If an error occurred with any step
 */
OSStatus MicoFlashRead( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* outBuffer, uint32_t inBufferLength);



/**@brief    Set security options on a logical partition
 *
 * @param    partition     : The target flash logical partition
 * @param    offset        : Point to the start address that the data is read, and
 *                          point to the last unread address after this function is 
 *                          returned, so you can call this function serval times without
 *                          update this start address.
 * @param    size          : Size of enabled flash area
 *
 * @return    kNoErr        : On success.
 * @return    kGeneralErr   : If an error occurred with any step
 */
OSStatus MicoFlashEnableSecurity( mico_partition_t partition, uint32_t off_set, uint32_t size );



#ifdef BOOTLOADER
/**@brief    Disable security options on a logical partition
 *
 * @param    partition     : The target flash logical partition
 * @param    offset        :  Point to the start address that the data is read, and
 *                          point to the last unread address after this function is 
 *                          returned, so you can call this function serval times without
 *                          update this start address.
 * @param    size          : Size of disabled flash area
 *
 * @return   kNoErr        : On success.
 * @return   kGeneralErr   : If an error occurred with any step
 */
OSStatus MicoFlashDisableSecurity( mico_partition_t partition, uint32_t off_set, uint32_t size );
#endif


/** @} */
/** @} */

#endif


