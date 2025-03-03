/**
 ******************************************************************************
 * @file    fatfs_to_block_device_driver.c
 * @author  You xx
 * @version V1.0.0
 * @date    20-Dec-2016
 * @brief   This file provide User API driver for FatFs
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
/** @file
 *  Block device driver for FatFs
 *  Provides the functions which are called by the bottom
 *  of FatFS
 */

/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"        /* FatFs lower layer API */
#include "platform_block_device.h"
#include "mico_filesystem_internal.h"
#include "mico.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define FATFS_WRITE_STRATEGY  BLOCK_DEVICE_WRITE_IMMEDIATELY

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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
                     void* user_data /* Physical drive nmuber to identify the drive */
                     )
{
    OSStatus result;
    mico_block_device_status_t status;
    mico_block_device_t* device = (mico_block_device_t*) user_data;

    /* Get the block device status */
    result = device->driver->status( device, &status );
    if ( result != kNoErr )
    {
        return STA_NOINIT;
    }

    /* Translate the status into a FatFS status */
    if ( status == BLOCK_DEVICE_UNINITIALIZED )
    {
        return STA_NOINIT;
    }

    if ( status == BLOCK_DEVICE_DOWN )
    {
        return STA_NODISK;
    }

    if ( status == BLOCK_DEVICE_UP_READ_ONLY )
    {
        return STA_PROTECT;
    }

    return 0;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
                         void* user_data /* Physical drive nmuber to identify the drive */
                         )
{
    mico_block_device_t* device = (mico_block_device_t*) user_data;
    mico_result_t result;

    /* Init the block device */
    result = device->driver->init( device, FATFS_WRITE_STRATEGY );
    if ( result != kNoErr )
    {
        return STA_NOINIT;
    }

    return disk_status( user_data );
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
                   void* user_data, /* Physical drive nmuber to identify the drive */
                   FATFS_BYTE *buff, /* Data buffer to store read data */
                   FATFS_DWORD sector, /* Sector address in LBA */
                   FATFS_UINT count /* Number of sectors to read */
                   )
{
    /* Read from the block device */
    mico_result_t result;
    mico_block_device_t* device = (mico_block_device_t*) user_data;

    result = device->driver->read( device, sector * DEFAULT_SECTOR_SIZE, buff, count );

    if ( result != kNoErr )
    {
        return RES_ERROR;
    }

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _FS_READONLY == 0
DRESULT disk_write(
                    void* user_data, /* Physical drive nmuber to identify the drive */
                    const FATFS_BYTE *buff, /* Data to be written */
                    FATFS_DWORD sector, /* Sector address in LBA */
                    FATFS_UINT count /* Number of sectors to write */
                    )
{
    /* Write to the block device */
    mico_result_t result;
    mico_block_device_t* device = (mico_block_device_t*) user_data;

    result = device->driver->write( device, sector * DEFAULT_SECTOR_SIZE, buff, count );

    if ( result != kNoErr )
    {
        return RES_ERROR;
    }

    return RES_OK;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if !_FS_READONLY
DRESULT disk_ioctl(
                    void* user_data,
                    FATFS_BYTE cmd, /* Control code */
                    void *buff /* Buffer to send/receive control data */
                    )
{
    /* Handle the various IOCTL's */
    mico_block_device_t* device = (mico_block_device_t*) user_data;
    mico_logic_partition_t *fatfs_partition;
    fatfs_partition = MicoFlashGetInfo( MICO_PARTITION_FILESYS );

    switch ( cmd )
    {
        case CTRL_SYNC: /* Complete pending write process (needed at _FS_READONLY == 0) */
        {
            if ( device->driver->flush( device ) != kNoErr )
            {
                return RES_ERROR;
            }
        }
            break;
        case GET_SECTOR_COUNT: /* Get media size (needed at _USE_MKFS == 1) */
        {
            FATFS_DWORD* number_of_sectors = (FATFS_DWORD*) buff;

            if((device->device_size)>(fatfs_partition->partition_length))
            {
                *number_of_sectors = fatfs_partition->partition_length / DEFAULT_SECTOR_SIZE;
            }
            else
            {
                *number_of_sectors = device->device_size / DEFAULT_SECTOR_SIZE;
            }
        }
            break;
        case GET_SECTOR_SIZE: /* Get sector size (needed at _MAX_SS != _MIN_SS) */
//            return RES_ERROR; /* TODO: Not implemented yet */
            *(FATFS_DWORD*)buff = DEFAULT_SECTOR_SIZE;
            break;
        case GET_BLOCK_SIZE: /* Get erase block size (needed at _USE_MKFS == 1) */
        {
            FATFS_DWORD* block_size = (FATFS_DWORD*) buff;
            *block_size = (device->erase_block_size == BLOCK_DEVICE_ERASE_NOT_REQUIRED) ? 1 : device->erase_block_size;
        }
            break;
        case CTRL_TRIM: /* Inform device that the data on the block of sectors is no longer used (needed at _USE_TRIM == 1) */
        {
            FATFS_DWORD* start_end_sector = (FATFS_DWORD*) buff;
            uint64_t start = start_end_sector[0] * start_end_sector[2];
            uint64_t size = (start_end_sector[1] - start_end_sector[0]) * start_end_sector[2];
            if ( (device->driver->erase != NULL) &&
                 (device->driver->erase( device, start, size ) != kNoErr) )
            {
                return RES_ERROR;
            }
        }
            break;

    }

    return RES_OK;
}
#endif

#if !_FS_READONLY && !_FS_NORTC
FATFS_DWORD get_fattime( void )
{
    mico_time_t time = 0;
    mico_time_get_time( &time );
    return time;
}
#endif /* if !_FS_READONLY && !_FS_NORTC */
