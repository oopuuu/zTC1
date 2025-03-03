/**
 ******************************************************************************
 * @file    mico_filesystem.c
 * @author  You xx
 * @version V1.0.0
 * @date    01-Dec-2016
 * @brief   This file provide the function for mico filesystem.
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
 *  Implementation of the MICOFS External-Use file system.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mico_filesystem.h"
#include "mico_filesystem_internal.h"
#include "mico_result.h"
#include "platform_block_device.h"


/******************************************************
 *                      Macros
 ******************************************************/
#define os_filesystem_log(format, ...)  custom_log("filesystem", format, ##__VA_ARGS__)
/******************************************************
 *                    Constants
 ******************************************************/

#define DEFAULT_SECTOR_SIZE  (512)

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
 *                 Static Variables
 ******************************************************/

static mico_bool_t mico_filesystem_inited = MICO_FALSE;

static mico_filesystem_mounted_device_t mounted_table[MICO_FILESYSTEM_MOUNT_DEVICE_NUM_MAX];
static uint32_t total_mounted_index = 0;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static OSStatus mico_filesystem_add_mounted_device ( mico_filesystem_t* fs_handle, const char* mounted_name);
static OSStatus mico_filesystem_del_mounted_device ( mico_filesystem_t* fs_handle );

/******************************************************
 *               Variable Definitions
 ******************************************************/


/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus mico_filesystem_init ( void )
{
    if ( mico_filesystem_inited == true )
    {
        return kNoErr;
    }

    memset( (void *) mounted_table, 0, sizeof(mounted_table) );
    total_mounted_index = 0;

#ifdef USING_FTFS
    mico_filesystem_driver_ftfs.init();
#endif /* USING_micoFS */
#ifdef USING_FATFS
    mico_filesystem_driver_fatfs.init();
#endif /* USING_FATFS */
    mico_filesystem_inited = true;
    return kNoErr;
}

static OSStatus mico_filesystem_add_mounted_device ( mico_filesystem_t* fs_handle, const char* mounted_name)
{
    uint32_t i;

    /* Error checking */
    if ( (fs_handle == NULL) || (mounted_name == NULL) )
    {
        os_filesystem_log( "Null input!" );
        return kUnknownErr;
    }

    if ( total_mounted_index >= MICO_FILESYSTEM_MOUNT_DEVICE_NUM_MAX )
    {
        os_filesystem_log( "Mounted device number exceeds upper limit!" );
        return kUnknownErr;
    }

    if ( strlen(mounted_name) > (MICO_FILESYSTEM_MOUNT_NAME_LENGTH_MAX - 1) )
    {
        os_filesystem_log( "Device name length too long!" );
        return kUnknownErr;
    }

    /* Device duplicated checking */
    for ( i = 0; i < MICO_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        if ( mounted_table[i].fs_handle == fs_handle )
        {
            os_filesystem_log( "Duplicated handle with index %ld", i );
            return kUnknownErr;
        }
        if ( strcmp(mounted_table[i].name, mounted_name) == 0 )
        {
            os_filesystem_log( "Duplicated name with index %ld\n", i );
            return kUnknownErr;
        }
    }

    /* Add into table */
    mounted_table[total_mounted_index].fs_handle = fs_handle;
    memcpy( mounted_table[total_mounted_index].name, mounted_name, MICO_FILESYSTEM_MOUNT_NAME_LENGTH_MAX );
    mounted_table[total_mounted_index].name[strlen(mounted_name)] = '\0';
    os_filesystem_log( "Added %s into mounted table with index %lu", mounted_table[total_mounted_index].name, (unsigned long)total_mounted_index );

    /* Increase index if all right */
    total_mounted_index ++;

    /* Only for debug dump */
    #if 0
    printf( "\n--- Filesystem Mounted Table ---\n");
    for ( i = 0; i < MICO_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        printf( "[%02ld]\t %s\t\t 0x%08lx\n", i, mounted_table[i].name, (uint32_t)mounted_table[i].fs_handle);
    }
    #endif

    return kNoErr;
}

static OSStatus mico_filesystem_del_mounted_device ( mico_filesystem_t* fs_handle )
{
    mico_bool_t is_handle_found = MICO_FALSE;
    uint32_t shift_up_num = 0;
    uint32_t i;

    /* Error checking */
    if ( fs_handle == NULL )
    {
        os_filesystem_log( "Null input!" );
        return kUnknownErr;
    }

    /* Find device in mounted table */
    for ( i = 0; i < MICO_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        if ( mounted_table[i].fs_handle == fs_handle )
        {
            os_filesystem_log( "Found device in mounted table with index %lu", (unsigned long)i );
            is_handle_found = MICO_TRUE;
            break;
        }
    }
    if ( (is_handle_found == MICO_FALSE) || (i > total_mounted_index) )
    {
        os_filesystem_log( "Not existing mounted device!" );
        return kUnknownErr;
    }

    /* Delete in table (shift up in table if deleted not the last one) */
    shift_up_num = (total_mounted_index - i);
    os_filesystem_log( "Deleting %s in mounted table with index %lu. (shift up %lu)", mounted_table[i].name, (unsigned long)i, (unsigned long)shift_up_num );

    if ( ( shift_up_num != 0 ) && ( i < ( MICO_FILESYSTEM_MOUNT_DEVICE_NUM_MAX - 1 ) ) )
    {
        /* If shift up not zero, copy up before clearing the last entry */
        memcpy( (void *) &mounted_table[i], (void *) &mounted_table[i+1], (sizeof(mico_filesystem_mounted_device_t) * shift_up_num) );
    }
    memset( (void *) &mounted_table[total_mounted_index], 0, sizeof(mico_filesystem_mounted_device_t) );

    /* Decrease index if all right */
    total_mounted_index --;

    /* Only for debug dump */
    #if 0
    printf( "\n--- Filesystem Mounted Table ---\n");
    for ( i = 0; i < mico_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        printf( "[%02ld]\t %s\t\t 0x%08lx\n", i, mounted_table[i].name, (uint32_t)mounted_table[i].fs_handle);
    }
    #endif

    return kNoErr;
}

OSStatus mico_filesystem_mount ( mico_block_device_t* device, mico_filesystem_handle_type_t fs_type, mico_filesystem_t* fs_handle_out, const char* mounted_name )
{
    OSStatus result;

    /* These ifdefs ensure that the drivers are only pulled in if they are used */
    switch ( fs_type )
    {
#ifdef USING_FTFS
        case MICO_FILESYSTEM_HANDLE_FTFS:
            fs_handle_out->driver = &mico_filesystem_driver_ftfs;
            break;
#endif /* ifdef USING_micoFS */
#ifdef USING_FATFS
        case MICO_FILESYSTEM_HANDLE_FATFS:
            fs_handle_out->driver = &mico_filesystem_driver_fatfs;
            break;
#endif /* ifdef USING_FATFS */

#ifndef USING_FTFS
        case MICO_FILESYSTEM_HANDLE_FTFS:
#endif /* ifdef USING_micoFS */
#ifndef USING_FATFS
        case MICO_FILESYSTEM_HANDLE_FATFS:
#endif /* ifdef USING_FATFS */
        default:
            return MICO_FILESYSTEM_ERROR;
    }

    fs_handle_out->device = device;

    result = fs_handle_out->driver->mount( device, fs_handle_out );
    if ( result == kNoErr )
    {
       result = mico_filesystem_add_mounted_device ( fs_handle_out, mounted_name );
    }

    return result;
}

OSStatus mico_filesystem_unmount ( mico_filesystem_t* fs_handle )
{
    OSStatus result;

    result = fs_handle->driver->unmount( fs_handle );
    if ( result == kNoErr )
    {
        result = mico_filesystem_del_mounted_device ( fs_handle );
    }

    return result;
}

mico_filesystem_t* mico_filesystem_retrieve_mounted_fs_handle ( const char* mounted_name )
{
    mico_filesystem_t* fs_handle_get = NULL;
    uint32_t i;

    /* Find device in mounted table */
    for ( i = 0; i < MICO_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        if ( strcmp(mounted_table[i].name, mounted_name) == 0 )
        {
            os_filesystem_log( "Found name in mounted table with index %lu", (unsigned long)i );
            fs_handle_get = mounted_table[i].fs_handle;
            break;
        }
    }

    return fs_handle_get;
}

OSStatus mico_filesystem_file_open ( mico_filesystem_t* fs_handle, mico_file_t* file_handle_out, const char* filename, mico_filesystem_open_mode_t mode )
{
    file_handle_out->filesystem = fs_handle;
    file_handle_out->driver     = fs_handle->driver;
    return fs_handle->driver->file_open( fs_handle, file_handle_out, filename, mode );
}

OSStatus mico_filesystem_file_get_details ( mico_filesystem_t* fs_handle, const char* filename, mico_dir_entry_details_t* details_out )
{
    return fs_handle->driver->file_get_details( fs_handle, filename, details_out );
}

OSStatus mico_filesystem_file_close ( mico_file_t* file_handle )
{
    return file_handle->driver->file_close( file_handle );
}

OSStatus mico_filesystem_file_delete ( mico_filesystem_t* fs_handle, const char* filename )
{
    return fs_handle->driver->file_delete( fs_handle, filename );
}

OSStatus mico_filesystem_file_seek ( mico_file_t* file_handle, int64_t offset, mico_filesystem_seek_type_t whence )
{
    return file_handle->driver->file_seek( file_handle, offset, whence );
}

OSStatus mico_filesystem_file_tell ( mico_file_t* file_handle, uint64_t* location )
{
    return file_handle->driver->file_tell( file_handle, location );
}

OSStatus mico_filesystem_file_read ( mico_file_t* file_handle, void* data, uint64_t bytes_to_read, uint64_t* returned_bytes_count )
{
    return file_handle->driver->file_read( file_handle, data, bytes_to_read, returned_bytes_count );
}

OSStatus mico_filesystem_file_write( mico_file_t* file_handle, const void* data, uint64_t bytes_to_write, uint64_t* written_bytes_count )
{
    return file_handle->driver->file_write( file_handle, data, bytes_to_write, written_bytes_count );
}

OSStatus mico_filesystem_file_flush ( mico_file_t* file_handle )
{
    return file_handle->driver->file_flush( file_handle );
}


int mico_filesystem_file_end_reached ( mico_file_t* file_handle )
{
    return file_handle->driver->file_end_reached( file_handle );
}

OSStatus mico_filesystem_dir_open ( mico_filesystem_t* fs_handle, mico_dir_t* dir_handle, const char* dir_name )
{
    dir_handle->filesystem = fs_handle;
    dir_handle->driver     = fs_handle->driver;

    if ( ( dir_name == NULL ) || ( strlen(dir_name) <= 0 ) )
    {
        return MICO_FILESYSTEM_BADARG;
    }

    return fs_handle->driver->dir_open( fs_handle, dir_handle, dir_name );
}

OSStatus mico_filesystem_dir_close ( mico_dir_t* dir_handle )
{
    return dir_handle->driver->dir_close( dir_handle );
}

OSStatus mico_filesystem_dir_read( mico_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length, mico_dir_entry_type_t* type, mico_dir_entry_details_t* details )
{
    return dir_handle->driver->dir_read( dir_handle, name_buffer, name_buffer_length, type, details );
}

int mico_filesystem_dir_end_reached ( mico_dir_t* dir_handle )
{
    return dir_handle->driver->dir_end_reached( dir_handle );
}

OSStatus mico_filesystem_dir_rewind ( mico_dir_t* dir_handle )
{
    return dir_handle->driver->dir_rewind( dir_handle );
}

OSStatus mico_filesystem_dir_create( mico_filesystem_t* fs_handle, const char* directory_name )
{
    return fs_handle->driver->dir_create( fs_handle, directory_name );
}

OSStatus mico_filesystem_format( mico_block_device_t* device, mico_filesystem_handle_type_t fs_type )
{
    mico_filesystem_driver_t* driver;
    /* These ifdefs ensure that the drivers are only pulled in if they are used */
    switch ( fs_type )
    {
#ifdef USING_FTFS
        case MICO_FILESYSTEM_HANDLE_FTFS:
            driver = &mico_filesystem_driver_ftfs;
            break;
#endif /* ifdef USING_micoFS */
#ifdef USING_FATFS
        case MICO_FILESYSTEM_HANDLE_FATFS:
            driver = &mico_filesystem_driver_fatfs;
            break;
#endif /* ifdef USING_FATFS */

#ifndef USING_FTFS
        case MICO_FILESYSTEM_HANDLE_FTFS:
#endif /* ifdef USING_micoFS */
#ifndef USING_FATFS
        case MICO_FILESYSTEM_HANDLE_FATFS:
#endif /* ifdef USING_FATFS */
        default:
            return MICO_FILESYSTEM_ERROR;
    }

    return driver->format( device );
}

OSStatus mico_filesystem_get_info( mico_filesystem_t* fs_handle,mico_filesystem_info* info,char* mounted_name )
{
    return fs_handle->driver->get_info( info,mounted_name );
}

OSStatus mico_filesystem_scan_files( mico_filesystem_t* fs_handle, char* mounted_name, mico_scan_file_handle arg )
{
    return fs_handle->driver->scan_files( mounted_name, arg );
}
