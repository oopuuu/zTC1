/**
 ******************************************************************************
 * @file    mico_filesystem_internal.h
 * @author  You xx
 * @version V1.0.0
 * @date    01-Dec-2016
 * @brief   This file provide the struct used in mico filesystem
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


#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mico_filesystem.h"


#ifdef __cplusplus
extern "C" {
#endif
/******************************************************
 *                      Macros
 ******************************************************/

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

struct mico_filesystem_driver_struct
{
    OSStatus (*init)             ( void );
    OSStatus (*mount)            ( mico_block_device_t* device, mico_filesystem_t* fs_handle_out );
    OSStatus (*unmount)          ( mico_filesystem_t* fs_handle );
    OSStatus (*file_get_details) ( mico_filesystem_t* fs_handle, const char* filename, mico_dir_entry_details_t* details_out );
    OSStatus (*file_open)        ( mico_filesystem_t* fs_handle, mico_file_t* file_handle_out, const char* filename, mico_filesystem_open_mode_t mode );
    OSStatus (*file_seek)        ( mico_file_t* file_handle, int64_t offset, mico_filesystem_seek_type_t whence );
    OSStatus (*file_tell)        ( mico_file_t* file_handle, uint64_t* location );
    OSStatus (*file_read)        ( mico_file_t* file_handle, void* data, uint64_t bytes_to_read, uint64_t* returned_bytes_count );
    OSStatus (*file_write)       ( mico_file_t* file_handle, const void* data, uint64_t bytes_to_write, uint64_t* written_bytes_count );
    OSStatus (*file_flush)       ( mico_file_t* file_handle );
    int      (*file_end_reached) ( mico_file_t* file_handle );
    OSStatus (*file_close)       ( mico_file_t* file_handle );
    OSStatus (*file_delete)      ( mico_filesystem_t* fs_handle, const char* filename );
    OSStatus (*dir_open)         ( mico_filesystem_t* fs_handle, mico_dir_t* dir_handle, const char* dir_name );
    OSStatus (*dir_read)         ( mico_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length, mico_dir_entry_type_t* type, mico_dir_entry_details_t* details );
    int      (*dir_end_reached)  ( mico_dir_t* dir_handle );
    OSStatus (*dir_rewind)       ( mico_dir_t* dir_handle );
    OSStatus (*dir_close)        ( mico_dir_t* dir_handle );
    OSStatus (*dir_create)       ( mico_filesystem_t* fs_handle, const char* directory_name );
    OSStatus (*format)           ( mico_block_device_t* device );
    OSStatus (*get_info)         ( mico_filesystem_info* info,char* mounted_name );
    OSStatus (*scan_files)       ( char* mounted_name, mico_scan_file_handle arg );
};



/******************************************************
 *                 Static Variables
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/
extern mico_filesystem_driver_t mico_filesystem_driver_ftfs;
extern mico_filesystem_driver_t mico_filesystem_driver_fatfs;

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif
