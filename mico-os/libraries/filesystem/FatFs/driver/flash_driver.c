/**
 ******************************************************************************
 * @file    flash_driver.c
 * @author  You xx
 * @version V1.0.0
 * @date    20-Dec-2016
 * @brief   This file provide flash driver api for fatfs
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

#include "mico.h"
#include "mico_filesystem.h"
#include "mico_filesystem_internal.h"

/******************************************************
 *                      Macros
 ******************************************************/

//#define BLOCK_SIZE                8
#define SECTOR_SIZE               512
#define FLASH_SECTOR              4096

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

void MicoFlashEraseWrite( mico_partition_t partition, volatile uint32_t* off_set, uint8_t* data_addr, uint32_t size );
OSStatus tester_block_device_init( mico_block_device_t* device, mico_block_device_write_mode_t write_mode );
OSStatus tester_block_flush( mico_block_device_t * device );
OSStatus tester_block_status( mico_block_device_t* device, mico_block_device_status_t* status );
OSStatus tester_block_status( mico_block_device_t* device, mico_block_device_status_t* status );
OSStatus tester_block_read( mico_block_device_t* device, uint64_t start_address, uint8_t* buff, uint64_t count );
OSStatus tester_block_write( mico_block_device_t* device, uint64_t start_address, const uint8_t* data, uint64_t size );


/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

const mico_block_device_driver_t tester_block_device_driver =
    {
        .init = tester_block_device_init,
        .deinit = NULL,
        .erase = NULL,
        .write = tester_block_write,
        .flush = tester_block_flush,
        .read = tester_block_read,
        .register_callback = NULL,
        .status = tester_block_status,
    };

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/




/**
 * @brief  Initializes a Drive
 * @param  None
 * @retval DSTATUS: Operation status
 */
OSStatus tester_block_device_init( mico_block_device_t* device, mico_block_device_write_mode_t write_mode )
{
    return kNoErr;
}

OSStatus tester_block_flush( mico_block_device_t * device )
{
    UNUSED_PARAMETER( device );
    return kNoErr;
}

/**
 * @brief  Gets Disk Status
 * @param  None
 * @retval DSTATUS: Operation status
 */
OSStatus tester_block_status( mico_block_device_t* device, mico_block_device_status_t* status )
{
    UNUSED_PARAMETER( device );
    *status = BLOCK_DEVICE_UP_READ_WRITE;
    return kNoErr;
}

/**
 * @brief  Reads Sector(s)
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: Operation result
 */
OSStatus tester_block_read( mico_block_device_t* device, uint64_t start_address, uint8_t* buff, uint64_t count )
{
    OSStatus err = kNoErr;
    uint64_t offset;

    for ( ; count > 0; count-- )
    {
        offset = start_address;
        err = MicoFlashRead( MICO_PARTITION_FILESYS, (uint32_t *) &offset, buff, SECTOR_SIZE );
        offset += SECTOR_SIZE;
        buff += SECTOR_SIZE;
        if ( err != kNoErr )
        {
            return err;
        }
    }
    return err;
}

/**
 * @brief  Writes Sector(s)
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: Operation result
 */
OSStatus tester_block_write( mico_block_device_t* device, uint64_t start_address, const uint8_t* data, uint64_t size )
{
    OSStatus err = kNoErr;
    uint64_t offset;

    for ( ; size > 0; size-- )
    {
        offset = start_address;
        MicoFlashEraseWrite( MICO_PARTITION_FILESYS, (uint32_t *) &offset, (uint8_t *) data, SECTOR_SIZE );
        offset += SECTOR_SIZE;
        data += SECTOR_SIZE;
        if ( err != kNoErr )
        {
            return err;
        }
    }

    return err;
}

void MicoFlashEraseWrite( mico_partition_t partition, volatile uint32_t* off_set, uint8_t* data_addr, uint32_t size )
{
    uint32_t f_sector;
    uint32_t f_addr;
    uint8_t *f_sector_buf = NULL;
    uint32_t pos = 0;

    uint16_t s_sector;

    f_sector = (*off_set) >> 12;
    f_addr = f_sector << 12;

    s_sector = (*off_set) & 0x0F00;

    f_sector_buf = malloc( FLASH_SECTOR );

    MicoFlashRead( partition, &f_addr, f_sector_buf, FLASH_SECTOR );

    for ( pos = 0; pos < size; pos++ )
    {
        if ( f_sector_buf[s_sector + pos] != 0xFF )
            break;
    }

    if ( pos != size )
    {
        f_addr -= FLASH_SECTOR;
        MicoFlashErase( partition, f_addr, size );

        for ( pos = 0; pos < size; pos++ )
        {
            f_sector_buf[s_sector + pos] = data_addr[pos];
        }
        MicoFlashWrite( partition, &f_addr, f_sector_buf, FLASH_SECTOR );
    } else
    {
        MicoFlashWrite( partition, off_set, data_addr, size );
    }

    free( f_sector_buf );
}
