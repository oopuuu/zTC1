/**
 ******************************************************************************
 * @file    fatfs_user_api_driver.c
 * @author  You xx
 * @version V1.0.0
 * @date    28-Nov-2016
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

#include "common.h"
#include "mico_result.h"
#include "mico_filesystem_internal.h"
#include "StringUtils.h"

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

/******************************************************
 *               Static Function Declarations
 ******************************************************/
static OSStatus fatfs_init( void );
static OSStatus fatfs_mount( mico_block_device_t* device, mico_filesystem_t* fs_handle_out );
static OSStatus fatfs_unmount( mico_filesystem_t* fs_handle );
static OSStatus fatfs_file_get_details( mico_filesystem_t* fs_handle, const char* filename,
                                        mico_dir_entry_details_t* details_out );
static OSStatus fatfs_file_open( mico_filesystem_t* fs_handle, mico_file_t* file_handle_out, const char* filename,
                                 mico_filesystem_open_mode_t mode );
static OSStatus fatfs_file_seek( mico_file_t* file_handle, int64_t offset, mico_filesystem_seek_type_t whence );
static OSStatus fatfs_file_tell( mico_file_t* file_handle, uint64_t* location );
static OSStatus fatfs_file_read( mico_file_t* file_handle, void* data, uint64_t bytes_to_read,
                                 uint64_t* returned_bytes_count );
static OSStatus fatfs_file_write( mico_file_t* file_handle, const void* data, uint64_t bytes_to_write,
                                  uint64_t* written_bytes_count );
static OSStatus fatfs_file_flush( mico_file_t* file_handle );
static int fatfs_file_end_reached( mico_file_t* file_handle );
static OSStatus fatfs_file_close( mico_file_t* file_handle );
static OSStatus fatfs_file_delete( mico_filesystem_t* fs_handle, const char* filename );
static OSStatus fatfs_dir_open( mico_filesystem_t* fs_handle, mico_dir_t* dir_handle, const char* dir_name );
static OSStatus fatfs_dir_read( mico_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length,
                                mico_dir_entry_type_t* type, mico_dir_entry_details_t* details );
static int fatfs_dir_end_reached( mico_dir_t* dir_handle );
static OSStatus fatfs_dir_rewind( mico_dir_t* dir_handle );
static OSStatus fatfs_dir_close( mico_dir_t* dir_handle );
static OSStatus fatfs_dir_create( mico_filesystem_t* fs_handle, const char* directory_name );
static OSStatus fatfs_format( mico_block_device_t* device );
static OSStatus fatfs_get_info( mico_filesystem_info* info,char* mounted_name );
static OSStatus fatfs_scan_files( char* mounted_name, mico_scan_file_handle arg );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* This is the User API driver structure for FatFS */
mico_filesystem_driver_t mico_filesystem_driver_fatfs =
    {
        .init = fatfs_init,
        .mount = fatfs_mount,
        .unmount = fatfs_unmount,
        .file_get_details = fatfs_file_get_details,
        .file_open = fatfs_file_open,
        .file_seek = fatfs_file_seek,
        .file_tell = fatfs_file_tell,
        .file_read = fatfs_file_read,
        .file_write = fatfs_file_write,
        .file_flush = fatfs_file_flush,
        .file_end_reached = fatfs_file_end_reached,
        .file_close = fatfs_file_close,
        .file_delete = fatfs_file_delete,
        .dir_open = fatfs_dir_open,
        .dir_read = fatfs_dir_read,
        .dir_end_reached = fatfs_dir_end_reached,
        .dir_rewind = fatfs_dir_rewind,
        .dir_close = fatfs_dir_close,
        .dir_create = fatfs_dir_create,
        .format = fatfs_format,
        .get_info = fatfs_get_info,
        .scan_files = fatfs_scan_files,
    };

/******************************************************
 *               Function Definitions
 ******************************************************/

/* Initialises FatFS shim - nothing to be done */
static OSStatus fatfs_init( void )
{
    return kNoErr;
}

/* Internal function for mounting a FatFS filesystem from a block device (with a "mount_now" parameter) */
static OSStatus fatfs_internal_mount( mico_block_device_t* device, mico_filesystem_t* fs_handle_out,
                                      mico_bool_t mount_now )
{
    static uint8_t next_drive_id = 0;
    FRESULT fatfs_result;

    /* Create a logical drive name  "0:", "1:" etc */
    unsigned_to_decimal_string( next_drive_id, (char*) &fs_handle_out->data.fatfs.drive_id, 1, 3 );
    strcat( (char*) &fs_handle_out->data.fatfs.drive_id, ":" );
    next_drive_id++;

    /* Mount the drive */
    fatfs_result = f_mount( &fs_handle_out->data.fatfs.handle, (char*) &fs_handle_out->data.fatfs.drive_id,
                            (mount_now == MICO_TRUE) ? 1 : 0, device );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    return kNoErr;
}

/* Unmounts a FatFS filesystem from a block device */
static OSStatus fatfs_unmount( mico_filesystem_t* fs_handle )
{
    FRESULT fatfs_result;

    /* Unmount the drive */
    fatfs_result = f_mount( NULL, (TCHAR*) &fs_handle->data.fatfs.drive_id, 1, NULL );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }
    return kNoErr;
}

/* Formats a block device with a FatFS filesystem */
static OSStatus fatfs_format( mico_block_device_t* device )
{
    FRESULT fatfs_result;
    OSStatus result;
    mico_filesystem_t fs_handle;

    /* Check that the block sizes are OK */

    /* Init the block device to populate the device sizes */
    result = device->driver->init( device, BLOCK_DEVICE_READ_ONLY );
    if ( result != kNoErr )
    {
        return result;
    }

    if ( (device->erase_block_size != BLOCK_DEVICE_ERASE_NOT_REQUIRED) &&
         (((device->erase_block_size < DEFAULT_SECTOR_SIZE) &&
           (( DEFAULT_SECTOR_SIZE % device->erase_block_size) != 0))
          ||
          ((device->erase_block_size >= DEFAULT_SECTOR_SIZE) &&
           ((device->erase_block_size % DEFAULT_SECTOR_SIZE) != 0))) )
    {
        /* Erase block size is invalid - not a multiple or sub-multiple of DEFAULT_SECTOR_SIZE */
        return MICO_FILESYSTEM_BLOCK_SIZE_BAD;
    }

    if ( (device->write_block_size != BLOCK_DEVICE_WRITE_NOT_ALLOWED) &&
         (((device->write_block_size < DEFAULT_SECTOR_SIZE) &&
           (( DEFAULT_SECTOR_SIZE % device->write_block_size) != 0))
          ||
          ((device->write_block_size >= DEFAULT_SECTOR_SIZE) &&
           ((device->write_block_size % DEFAULT_SECTOR_SIZE) != 0))) )
    {
        /* Write block size is invalid - not a multiple or sub-multiple of DEFAULT_SECTOR_SIZE */
        return MICO_FILESYSTEM_BLOCK_SIZE_BAD;
    }

    if ( ((device->read_block_size < DEFAULT_SECTOR_SIZE) &&
          (( DEFAULT_SECTOR_SIZE % device->read_block_size) != 0))
         ||
         ((device->read_block_size >= DEFAULT_SECTOR_SIZE) &&
          ((device->read_block_size % DEFAULT_SECTOR_SIZE) != 0)) )
    {
        /* Read block size is invalid - not a multiple or sub-multiple of DEFAULT_SECTOR_SIZE */
        return MICO_FILESYSTEM_BLOCK_SIZE_BAD;
    }

    /* Temporarily mount the drive (with  mount-later flag) */
    result = fatfs_internal_mount( device, &fs_handle, MICO_FALSE );
    if ( result != kNoErr )
    {
        return result;
    }

    /* Format device */
    fatfs_result = f_mkfs( (char*) &fs_handle.data.fatfs.drive_id, 1, DEFAULT_SECTOR_SIZE );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    /* Unmount again */
    return fatfs_unmount( &fs_handle );
}

/* Mounts a FatFS filesystem from a block device */
static OSStatus fatfs_mount( mico_block_device_t* device, mico_filesystem_t* fs_handle_out )
{
    OSStatus result;

    /* Format if required */
    if ( device->init_data->volatile_and_requires_format_when_mounting == MICO_TRUE )
    {
        /* Format device */
        result = fatfs_format( device );
        if ( result != kNoErr )
        {
            return result;
        }
    }

    /* Do the mount */
//    return fatfs_internal_mount( device, fs_handle_out, MICO_TRUE );
    return fatfs_internal_mount( device, fs_handle_out, MICO_FALSE );

}

/* Opens a file within a FatFS filesystem */
static OSStatus fatfs_file_open( mico_filesystem_t* fs_handle, mico_file_t* file_handle_out, const char* filename,
                                 mico_filesystem_open_mode_t mode )
{
    FRESULT fatfs_result;
    FATFS_BYTE fatfs_mode;
    FIL* file_handle = &file_handle_out->data.fatfs;

    /* Match the mico mode to a FatFS mode */
    switch ( mode )
    {
        case MICO_FILESYSTEM_OPEN_ZERO_LENGTH:
            fatfs_mode = FA_CREATE_ALWAYS | FA_READ | FA_WRITE;
            break;

        case MICO_FILESYSTEM_OPEN_WRITE_CREATE:
            case MICO_FILESYSTEM_OPEN_APPEND_CREATE:
            fatfs_mode = FA_OPEN_ALWAYS | FA_READ | FA_WRITE;
            break;

        case MICO_FILESYSTEM_OPEN_FOR_READ:
            fatfs_mode = FA_OPEN_EXISTING | FA_READ;
            break;

        case MICO_FILESYSTEM_OPEN_FOR_WRITE:
            case MICO_FILESYSTEM_OPEN_APPEND:
            fatfs_mode = FA_OPEN_EXISTING | FA_READ | FA_WRITE;
            break;

        default:
            /* Unknown mode */
            return kParamErr;
    }

    /* Change default drive to the requested drive */
    fatfs_result = f_chdrive( (char*) &fs_handle->data.fatfs.drive_id );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    /* Open the file */
    fatfs_result = f_open( file_handle, filename, fatfs_mode );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    /* If appending was requested , move to the end of the file */
    if ( (mode == MICO_FILESYSTEM_OPEN_APPEND) ||
         (mode == MICO_FILESYSTEM_OPEN_APPEND_CREATE) )
    {
        /* Seek to end of the file */
        fatfs_result = f_lseek( file_handle, f_size( file_handle ) );
        if ( fatfs_result != FR_OK )
        {
            f_close( file_handle );
            return MICO_FILESYSTEM_ERROR;
        }
    }
    return kNoErr;
}

/* Get details of a file within a FatFS filesystem */
static OSStatus fatfs_file_get_details( mico_filesystem_t* fs_handle, const char* filename,
                                        mico_dir_entry_details_t* details_out )
{
    FILINFO file_stat;
    FRESULT fatfs_result;

    /* Change default drive to the requested drive */
    fatfs_result = f_chdrive( (TCHAR*) &fs_handle->data.fatfs.drive_id );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    fatfs_result = f_stat( filename, &file_stat );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    /* Fill in the details structure */
    details_out->size = file_stat.fsize;
    details_out->attributes_available = file_stat.fattrib;
    details_out->date_time = (file_stat.fdate << 16) + file_stat.ftime; /* TODO: This is wrong - need to do conversion */
    details_out->attributes_available = MICO_TRUE;
    details_out->date_time_available = MICO_TRUE;
    details_out->permissions_available = MICO_FALSE;

    return kNoErr;
}

/* Close a file within a FatFS filesystem */
static OSStatus fatfs_file_close( mico_file_t* file_handle )
{
    FRESULT fatfs_result;

    fatfs_result = f_close( &file_handle->data.fatfs );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    return kNoErr;
}

/* Delete a file within a FatFS filesystem */
static OSStatus fatfs_file_delete( mico_filesystem_t* fs_handle, const char* filename )
{
    FRESULT fatfs_result;

    /* Change default drive to the requested drive */
    fatfs_result = f_chdrive( (TCHAR*) &fs_handle->data.fatfs.drive_id );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    /* Delete the file */
    fatfs_result = f_unlink( filename );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    return kNoErr;
}

/* Seek to a location in an open file within a FatFS filesystem */
static OSStatus fatfs_file_seek( mico_file_t* file_handle, int64_t offset, mico_filesystem_seek_type_t whence )
{
    FRESULT fatfs_result;
    FATFS_DWORD new_location;
    FATFS_DWORD file_size = f_size( &file_handle->data.fatfs );

    /* Translate mico "whence" to an absolute location, since FatFS does not do relative seeks */
    switch ( whence )
    {
        case MICO_FILESYSTEM_SEEK_SET:
            new_location = offset;
            break;

        case MICO_FILESYSTEM_SEEK_CUR:
            new_location = f_tell( &file_handle->data.fatfs ) + offset;
            break;

        case MICO_FILESYSTEM_SEEK_END:
            new_location = file_size + offset;
            break;

        default:
            return kParamErr;
    }

    if ( new_location < 0 )
    {
        return kParamErr;
    }

    /* Perform the seek */
    fatfs_result = f_lseek( &file_handle->data.fatfs, new_location );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    return kNoErr;
}

/* Get the current location in an open file within a FatFS filesystem */
static OSStatus fatfs_file_tell( mico_file_t* file_handle, uint64_t* location )
{
    *location = f_tell( &file_handle->data.fatfs );
    return kNoErr;
}

/* Read data from an open file within a FatFS filesystem */
static OSStatus fatfs_file_read( mico_file_t* file_handle, void* data, uint64_t bytes_to_read,
                                 uint64_t* returned_bytes_count )
{
    FRESULT fatfs_result;
    FATFS_UINT bytes_read;

    fatfs_result = f_read( &file_handle->data.fatfs, data, bytes_to_read, &bytes_read );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    *returned_bytes_count = bytes_read;

    return kNoErr;
}

/* Write data to an open file within a FatFS filesystem */
static OSStatus fatfs_file_write( mico_file_t* file_handle, const void* data, uint64_t bytes_to_write,
                                  uint64_t* written_bytes_count )
{
    FRESULT fatfs_result;
    FATFS_UINT bytes_written;

    fatfs_result = f_write( &file_handle->data.fatfs, data, bytes_to_write, &bytes_written );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    *written_bytes_count = bytes_written;

    return kNoErr;
}

/* Flush unwritten data in an open file within a FatFS filesystem */
static OSStatus fatfs_file_flush( mico_file_t* file_handle )
{
    FRESULT fatfs_result;

    fatfs_result = f_sync( &file_handle->data.fatfs );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    return kNoErr;
}

/* Get end-of-file (EOF) flag for an open file within a FatFS filesystem */
static int fatfs_file_end_reached( mico_file_t* file_handle )
{
    return f_eof( &file_handle->data.fatfs );
}

/* Opens a directory within a FatFS filesystem */
static OSStatus fatfs_dir_open( mico_filesystem_t* fs_handle, mico_dir_t* dir_handle, const char* dir_name )
{
    FRESULT fatfs_result;

    /* Change default drive to the requested drive */
    fatfs_result = f_chdrive( (TCHAR*) &fs_handle->data.fatfs.drive_id );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    fatfs_result = f_opendir( &dir_handle->data.fatfs.handle, (TCHAR*) dir_name );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    dir_handle->data.fatfs.eodir = MICO_FALSE;

    return kNoErr;
}

/* Reads directory entry from an open within a FatFS filesystem */
static OSStatus fatfs_dir_read( mico_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length,
                                mico_dir_entry_type_t* type, mico_dir_entry_details_t* details )
{
    FRESULT fatfs_result;
    FILINFO info;
#if _USE_LFN
    info.lfname = name_buffer;
    info.lfsize = name_buffer_length;
#endif /* if _USE_LFN */
    fatfs_result = f_readdir( &dir_handle->data.fatfs.handle, &info );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    /* Zero length filename indicates end of directory entries */
    if ( info.fname[0] == '\x00' )
    {
        dir_handle->data.fatfs.eodir = MICO_TRUE;
        return MICO_FILESYSTEM_END_OF_RESOURCE;
    }

    /* Copy the directory entry name to the output buffer */
#if _USE_LFN
    if ( name_buffer[0] == '\x00' ) /* Indicates that the filename is only in the short filename part */
    {
        strlcpy( name_buffer, info.fname, MIN( name_buffer_length, 13 ) );
    }
#else
    strlcpy( name_buffer, info.fname, MIN( name_buffer_length, 13 ) );
#endif /* _USE_LFN */

    /* Copy the directory entry details to the detail structure */
    details->size = info.fsize;
    details->attributes = info.fattrib;
    details->date_time = (info.fdate << 16) + info.ftime; /* TODO: This is wrong - need to do conversion */
    details->attributes_available = MICO_TRUE;
    details->date_time_available = MICO_TRUE;
    details->permissions_available = MICO_FALSE;

    *type = ((details->attributes & AM_DIR) != 0) ? MICO_FILESYSTEM_DIR : MICO_FILESYSTEM_FILE;

    return kNoErr;
}

/* Get end-of-directory flag for an open directory within a FatFS filesystem */
static int fatfs_dir_end_reached( mico_dir_t* dir_handle )
{
    return dir_handle->data.fatfs.eodir;
}

/* Moves the current location within a directory back to the first entry within a FatFS filesystem */
static OSStatus fatfs_dir_rewind( mico_dir_t* dir_handle )
{
    FRESULT fatfs_result;

    /* Rewind is achieved by passing NULL to f_readdir */
    fatfs_result = f_readdir( &dir_handle->data.fatfs.handle, NULL );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    dir_handle->data.fatfs.eodir = MICO_FALSE;

    return kNoErr;
}

/* Closes an open directory within a FatFS filesystem */
static OSStatus fatfs_dir_close( mico_dir_t* dir_handle )
{
    FRESULT fatfs_result;

    fatfs_result = f_closedir( &dir_handle->data.fatfs.handle );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    return kNoErr;
}

/* Creates a new directory within a FatFS filesystem */
static OSStatus fatfs_dir_create( mico_filesystem_t* fs_handle, const char* directory_name )
{
    FRESULT fatfs_result;

    /* Change default drive to the requested drive */
    fatfs_result = f_chdrive( (TCHAR*) &fs_handle->data.fatfs.drive_id );
    if ( fatfs_result != FR_OK )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    /* Create the directory */
    fatfs_result = f_mkdir( directory_name );
    if ( (fatfs_result != FR_OK) && (fatfs_result != FR_EXIST) )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    return kNoErr;
}

static OSStatus fatfs_get_info( mico_filesystem_info* info,char* mounted_name )
{
    FATFS *fs;
    uint8_t res;
    unsigned long fre_clust = 0, fre_sect = 0, tot_sect = 0;


    memset( info, 0x0, sizeof(mico_filesystem_info) );

    res = f_getfree( (const TCHAR*) mounted_name, &fre_clust, &fs );
    if ( res == 0 )
    {
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize;
#if _MAX_SS!=512
        tot_sect *= fs->ssize / 512;
        fre_sect *= fs->ssize / 512;
#endif
        if ( tot_sect < 20480 )
        {
            /* Print free space in unit of KB (assuming 512 bytes/sector) */
            info->total_space = tot_sect >> 1;
            info->free_space = fre_sect >> 1;
        } else
        {
            /* Print free space in unit of MB (assuming 512 bytes/sector) */
            info->total_space = tot_sect >> 11;
            info->free_space = fre_sect >> 11;
        }
    }
    return kNoErr;
}

static OSStatus fatfs_scan_files( char* mounted_name, mico_scan_file_handle arg )
{
    FRESULT res;
    FILINFO fno;
    FATFS_DIR dir;

    char *fn;   /* This function is assuming non-Unicode cfg. */

  #if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
  #endif

    res = f_opendir(&dir, mounted_name);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
  #if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
  #else
            fn = fno.fname;
  #endif
            arg( mounted_name,fn );
        }
        f_closedir(&dir);
    }

    return res;
}
