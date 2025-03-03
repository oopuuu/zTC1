/**
 ******************************************************************************
 * @file    ftfs_drivers.c
 * @author  You xx
 * @version V1.0.0
 * @date    28-Nov-2016
 * @brief   This file provide User API driver for FTFS
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
 *  User API driver for FTFS
 *  Adapts the top level FTFS API to match the MICO API
 */

#include "mico_result.h"
#include "ftfs_driver.h"
#include "mico_platform.h"
#include "mico_filesystem.h"
#include "mico_filesystem_internal.h"

#define ftfs_driver_log(format,...) custom_log("ftfs_driver", format, ##__VA_ARGS__)

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
static OSStatus ftfs_system_init( void );
static struct fs * ftfs_file_init( struct ftfs_super *sb, mico_partition_t partition );
static OSStatus ftfs_mount( mico_block_device_t* device, mico_filesystem_t* fs_handle_out );
static OSStatus ftfs_unmount( mico_filesystem_t* fs_handle );
static OSStatus ftfs_file_get_details( mico_filesystem_t* fs_handle, const char* filename,
                                       mico_dir_entry_details_t* details_out );
static OSStatus ftfs_file_open( mico_filesystem_t* fs_handle, mico_file_t* file_handle_out, const char* filename,
                                mico_filesystem_open_mode_t mode );
static OSStatus ftfs_file_seek( mico_file_t* file_handle, int64_t offset, mico_filesystem_seek_type_t whence );
static OSStatus ftfs_file_tell( mico_file_t* file_handle, uint64_t* location );
static OSStatus ftfs_file_read( mico_file_t* file_handle, void* data, uint64_t bytes_to_read,
                                uint64_t* returned_bytes_count );
static OSStatus ftfs_file_write( mico_file_t* file_handle, const void* data, uint64_t bytes_to_write,
                                 uint64_t* written_bytes_count );
static OSStatus ftfs_file_flush( mico_file_t* file_handle );
static int ftfs_file_end_reached( mico_file_t* file_handle );
static OSStatus ftfs_file_close( mico_file_t* file_handle );
static OSStatus ftfs_file_delete( mico_filesystem_t* fs_handle, const char* filename );
static OSStatus ftfs_dir_open( mico_filesystem_t* fs_handle, mico_dir_t* dir_handle, const char* dir_name );
static OSStatus ftfs_dir_read( mico_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length,
                               mico_dir_entry_type_t* type, mico_dir_entry_details_t* details );
static int ftfs_dir_end_reached( mico_dir_t* dir_handle );
static OSStatus ftfs_dir_rewind( mico_dir_t* dir_handle );
static OSStatus ftfs_dir_close( mico_dir_t* dir_handle );
static OSStatus ftfs_dir_create( mico_filesystem_t* fs_handle, const char* directory_name );
static OSStatus ftfs_format( mico_block_device_t* device );
static OSStatus ftfs_scan_files( char* mounted_name, mico_scan_file_handle arg );
static OSStatus ftfs_get_info( mico_filesystem_info* info,char* mounted_name );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* This is the User API driver structure for FTFS */
mico_filesystem_driver_t mico_filesystem_driver_ftfs =
    {
        .init = ftfs_system_init,
        .mount = ftfs_mount,
        .unmount = ftfs_unmount,
        .file_get_details = ftfs_file_get_details,
        .file_open = ftfs_file_open,
        .file_seek = ftfs_file_seek,
        .file_tell = ftfs_file_tell,
        .file_read = ftfs_file_read,
        .file_write = ftfs_file_write,
        .file_flush = ftfs_file_flush,
        .file_end_reached = ftfs_file_end_reached,
        .file_close = ftfs_file_close,
        .file_delete = ftfs_file_delete,
        .dir_open = ftfs_dir_open,
        .dir_read = ftfs_dir_read,
        .dir_end_reached = ftfs_dir_end_reached,
        .dir_rewind = ftfs_dir_rewind,
        .dir_close = ftfs_dir_close,
        .dir_create = ftfs_dir_create,
        .format = ftfs_format,
        .get_info = ftfs_get_info,
        .scan_files = ftfs_scan_files,
    };

/******************************************************
 *               Function Definitions
 ******************************************************/

static OSStatus ftfs_system_init( void )
{
    return kNoErr;
}

/* Initialises FTFS */
static struct fs * ftfs_file_init( struct ftfs_super *sb, mico_partition_t partition )
{
    FT_HEADER sec;
    uint32_t start_addr = 0;
    mico_logic_partition_t *ftfs_partition;

    ftfs_partition = MicoFlashGetInfo( partition );

    start_addr = ftfs_partition->partition_start_addr;

    if ( ft_read_header( &sec, start_addr ) != kNoErr )
        return NULL;

    if ( !ft_is_valid_magic( sec.magic ) )
    {
        ftfs_driver_log( "Invalid magic number!" );
        return NULL;
    }

    memset( sb, 0, sizeof(*sb) );
    sb->fs.fopen = ft_fopen;
    sb->fs.fclose = ft_fclose;
    sb->fs.fread = ft_fread;
    sb->fs.fwrite = ft_fwrite;
    sb->fs.ftell = ft_ftell;
    sb->fs.fseek = ft_fseek;

    memset( sb->fds, 0, sizeof(sb->fds) );
    sb->fds_mask = 0;

    sb->active_addr = start_addr;

    if ( sb->active_addr < 0 )
        return 0;

    /* Check CRC on each init */
    sb->fs_crc32 = sec.crc;
    return (struct fs *) sb;
}

/* Mounts a FTFS filesystem from a block device */
static OSStatus ftfs_mount( mico_block_device_t* device, mico_filesystem_t* fs_handle_out )
{
    UNUSED_PARAMETER( device );
    UNUSED_PARAMETER( fs_handle_out );
    return MICO_FILESYSTEM_ERROR;
}

/* Unmounts a FTFS filesystem from a block device */
static OSStatus ftfs_unmount( mico_filesystem_t* fs_handle )
{
    UNUSED_PARAMETER( fs_handle );
    ftfs_driver_log( "FTFS not Support!" );
    return MICO_FILESYSTEM_ERROR;
}

/* Opens a file within a FTFS filesystem */
static OSStatus ftfs_file_open( mico_filesystem_t* fs_handle, mico_file_t* file_handle_out, const char* filename,
                                mico_filesystem_open_mode_t mode )
{

    if ( mode != MICO_FILESYSTEM_OPEN_FOR_READ )
    {
        return MICO_FILESYSTEM_WRITE_PROTECTED;
    }
    fs_handle->data.fs = ftfs_file_init( &(fs_handle->data.sb), MICO_PARTITION_FILESYS );
    fs_handle->data.f = ft_fopen( fs_handle->data.fs, filename, NULL );
    file_handle_out->data.f = fs_handle->data.f;
    if ( fs_handle->data.f == NULL )
    {
        return MICO_FILESYSTEM_ERROR;
    }
    return kNoErr;
}

/* Get details of a file within a FTFS filesystem */
static OSStatus ftfs_file_get_details( mico_filesystem_t* fs_handle, const char* filename,
                                       mico_dir_entry_details_t* details_out )
{
    int file_size;
    file_size = ((FT_FILE *) (fs_handle->data.f))->length;
    if ( file_size < 0 )
    {
        return MICO_FILESYSTEM_ERROR;
    }
    details_out->size = (uint64_t) file_size;
    details_out->attributes_available = MICO_FALSE;
    details_out->date_time_available = MICO_FALSE;
    details_out->permissions_available = MICO_FALSE;

    return kNoErr;
}

/* Close a file within a FTFS filesystem */
static OSStatus ftfs_file_close( mico_file_t* file_handle )
{
    int result;
    result = ft_fclose( file_handle->data.f );
    if ( result != 0 )
    {
        return MICO_FILESYSTEM_ERROR;
    }

    return kNoErr;
}

/* Seek to a location in an open file within a FTFS filesystem */
static OSStatus ftfs_file_seek( mico_file_t* file_handle, int64_t offset, mico_filesystem_seek_type_t whence )
{
    if ( ft_fseek( file_handle->data.f, offset, whence ) != 0 )
    {
        return MICO_FILESYSTEM_ERROR;
    }
    return kNoErr;
}

/* Get the current location in an open file within a FTFS filesystem */
static OSStatus ftfs_file_tell( mico_file_t* file_handle, uint64_t* location )
{
    *location = (uint64_t) ft_ftell( file_handle->data.f );

    return kNoErr;
}

/* Read data from an open file within a FTFS filesystem */
static OSStatus ftfs_file_read( mico_file_t* file_handle, void* data, uint64_t bytes_to_read,
                                uint64_t* returned_bytes_count )
{
    *returned_bytes_count = ft_fread( data, bytes_to_read, 1, file_handle->data.f );
    return kNoErr;
}

/******************************************************
 * Unimplemented Functions - Due to being Read-Only
 ******************************************************/

/* Get end-of-file (EOF) flag for an open file within a FTFS filesystem */
static int ftfs_file_end_reached( mico_file_t* file_handle )
{
    UNUSED_PARAMETER( file_handle );
    ftfs_driver_log( "FTFS not Support!" );
    return MICO_FILESYSTEM_ERROR;
}

/* Opens a directory within a FTFS filesystem */
static OSStatus ftfs_dir_open( mico_filesystem_t* fs_handle, mico_dir_t* dir_handle, const char* dir_name )
{
    UNUSED_PARAMETER( fs_handle );
    UNUSED_PARAMETER( dir_handle );
    UNUSED_PARAMETER( dir_name );
    ftfs_driver_log( "FTFS not Support!" );
    return MICO_FILESYSTEM_ERROR;
}

/* Reads directory entry from an open within a FTFS filesystem */
static OSStatus ftfs_dir_read( mico_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length,
                               mico_dir_entry_type_t* type, mico_dir_entry_details_t* details_out )
{
    UNUSED_PARAMETER( dir_handle );
    UNUSED_PARAMETER( name_buffer );
    UNUSED_PARAMETER( name_buffer_length );
    UNUSED_PARAMETER( type );
    UNUSED_PARAMETER( details_out );
    ftfs_driver_log( "FTFS not Support!" );
    return MICO_FILESYSTEM_ERROR;
}

/* Get end-of-directory flag for an open directory within a FTFS filesystem */
static int ftfs_dir_end_reached( mico_dir_t* dir_handle )
{
    UNUSED_PARAMETER( dir_handle );
    ftfs_driver_log( "FTFS not Support!" );
    return MICO_FILESYSTEM_ERROR;
}

/* Moves the current location within a directory back to the first entry within a FTFS filesystem */
static OSStatus ftfs_dir_rewind( mico_dir_t* dir_handle )
{
    UNUSED_PARAMETER( dir_handle );
    ftfs_driver_log( "FTFS not Support!" );
    return MICO_FILESYSTEM_ERROR;
}

/* Closes an open directory within a FTFS filesystem */
static OSStatus ftfs_dir_close( mico_dir_t* dir_handle )
{
    UNUSED_PARAMETER( dir_handle );
    ftfs_driver_log( "FTFS not Support!" );
    return MICO_FILESYSTEM_ERROR;
}
static OSStatus ftfs_file_delete( mico_filesystem_t* fs_handle, const char* filename )
{
    UNUSED_PARAMETER( fs_handle );
    UNUSED_PARAMETER( filename );
    ftfs_driver_log( "FTFS is Read-Only!" );
    return (OSStatus) MICO_FILESYSTEM_ATTRIBUTE_READ_ONLY;
}

static OSStatus ftfs_file_write( mico_file_t* file_handle, const void* data, uint64_t bytes_to_write,
                                 uint64_t* written_bytes_count )
{
    UNUSED_PARAMETER( file_handle );
    UNUSED_PARAMETER( data );
    UNUSED_PARAMETER( bytes_to_write );
    UNUSED_PARAMETER( written_bytes_count );
    ftfs_driver_log( "FTFS is Read-Only!" );
    return MICO_FILESYSTEM_WRITE_PROTECTED;
}

static OSStatus ftfs_file_flush( mico_file_t* file_handle )
{
    UNUSED_PARAMETER( file_handle );
    ftfs_driver_log( "FTFS is Read-Only!" );
    return MICO_FILESYSTEM_WRITE_PROTECTED;
}

static OSStatus ftfs_dir_create( mico_filesystem_t* fs_handle, const char* directory_name )
{
    UNUSED_PARAMETER( fs_handle );
    UNUSED_PARAMETER( directory_name );
    ftfs_driver_log( "FTFS is Read-Only!" );
    return MICO_FILESYSTEM_WRITE_PROTECTED;
}

static OSStatus ftfs_format( mico_block_device_t* device )
{
    UNUSED_PARAMETER( device );
    ftfs_driver_log( "FTFS is Read-Only!" );
    return MICO_FILESYSTEM_WRITE_PROTECTED;
}

static OSStatus ftfs_scan_files( char* mounted_name, mico_scan_file_handle arg )
{
    UNUSED_PARAMETER( mounted_name );
    char path[]="/";
    uint32_t addr = sizeof(FT_HEADER);

    struct ft_entry entry;
    while ( entry.name[0] != '\0' )
    {
        MicoFlashRead( MICO_PARTITION_FILESYS, &addr, (uint8_t *) &entry, sizeof(entry) );
        if(entry.name[0] == '\0')
        break;
        arg( path,(char *)entry.name);
    }

    return kNoErr;
}

static OSStatus ftfs_get_info( mico_filesystem_info* info,char* mounted_name )
{
    UNUSED_PARAMETER( info );
    UNUSED_PARAMETER( mounted_name );
    ftfs_driver_log( "FTFS not Support!" );
    return MICO_FILESYSTEM_ERROR;
}
