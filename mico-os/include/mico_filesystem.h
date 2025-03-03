/**
 ******************************************************************************
 * @file    mico_filesystem.h
 * @author  You xx
 * @version V1.0.0
 * @date    01-Dec-2016
 * @brief   This file provide the function prototypes for mico filesystem.
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
 *  Public API of filesystem functions for MICO
 */

#pragma once

#include <stdio.h>
#include "mico_result.h"
#include "mico.h"
#include "platform_block_device.h"

#ifdef USING_FTFS
#include "ftfs_driver.h"
#endif /* USING_WICEDFS */
#ifdef USING_FATFS
#include "ff.h"
#endif /* USING_FATFS */


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                     Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define MICO_FILESYSTEM_DIRECTORY_SEPARATOR    '/'
#define MICO_FILESYSTEM_MOUNT_NAME_LENGTH_MAX  32
#define MICO_FILESYSTEM_MOUNT_DEVICE_NUM_MAX   8

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    MICO_FILESYSTEM_OPEN_FOR_READ,      /** Specifies read access to the object. Data can be read from the file - equivalent to "r" or "rb" */
    MICO_FILESYSTEM_OPEN_FOR_WRITE,     /** Specifies read/write access to the object. Data can be written to the file - equivalent to "r+" or "rb+" or "r+b" */
    MICO_FILESYSTEM_OPEN_WRITE_CREATE,  /** Opens for read/write access, creates it if it doesn't exist */
    MICO_FILESYSTEM_OPEN_ZERO_LENGTH,   /** Opens for read/write access, Truncates file to zero length if it exists, or creates it if it doesn't - equivalent to "w+", "wb+" or "w+b" */
    MICO_FILESYSTEM_OPEN_APPEND,        /** Opens for read/write access, places the current location at the end of the file ready for appending - equivalent to "a", "ab" */
    MICO_FILESYSTEM_OPEN_APPEND_CREATE, /** Opens for read/write access, creates it if it doesn't exist, and places the current location at the end of the file ready for appending  - equivalent to "a+", "ab+" or "a+b" */
} mico_filesystem_open_mode_t;

typedef enum
{
    MICO_FILESYSTEM_HANDLE_FTFS,
    MICO_FILESYSTEM_HANDLE_FATFS,
} mico_filesystem_handle_type_t;

typedef enum
{
    MICO_FILESYSTEM_MEDIA_USB_MSD,
    FATFS_HANDLE,
    FILEX_HANDLE,
} mico_filesystem_physical_media_driver_t;

typedef enum
{
    MICO_FILESYSTEM_SEEK_SET = SEEK_SET,      /* Offset from start of file */
    MICO_FILESYSTEM_SEEK_CUR = SEEK_CUR,      /* Offset from current position in file */
    MICO_FILESYSTEM_SEEK_END = SEEK_END,      /* Offset from end of file */
} mico_filesystem_seek_type_t;


typedef enum
{
    MICO_FILESYSTEM_ATTRIBUTE_READ_ONLY  = 0x01,
    MICO_FILESYSTEM_ATTRIBUTE_HIDDEN     = 0x02,
    MICO_FILESYSTEM_ATTRIBUTE_SYSTEM     = 0x04,
    MICO_FILESYSTEM_ATTRIBUTE_VOLUME     = 0x08,
    MICO_FILESYSTEM_ATTRIBUTE_DIRECTORY  = 0x10,
    MICO_FILESYSTEM_ATTRIBUTE_ARCHIVE    = 0x20,
}mico_filesystem_attribute_type_t;


typedef enum
{
    MICO_FILESYSTEM_FILE,
    MICO_FILESYSTEM_DIR,
    MICO_FILESYSTEM_LINK,
} mico_dir_entry_type_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct mico_filesystem_driver_struct mico_filesystem_driver_t;

/**
 * File-system Handle Structure
 */
typedef struct mico_filesystem_struct mico_filesystem_t;



/**
 * File Handle Structure
 *
 * Equivalent of ISO-C type FILE
 */
typedef struct mico_file_struct mico_file_t;



/**
 * Directory handle structure
 *
 * Equivalent of ISO-C type DIR
 */

typedef struct mico_dir_struct mico_dir_t;


typedef void ( *mico_scan_file_handle )( char* path,char* fn );

/******************************************************
 *                    Structures
 ******************************************************/


/**
 * File Information Structure
 *
 * Equivalent of ISO-C struct dirent
 */
typedef struct
{
    uint64_t                           size;

    bool                               attributes_available;
    bool                               date_time_available;
    bool                               permissions_available;
    mico_filesystem_attribute_type_t  attributes;    /* Attribute */
    mico_utc_time_t                   date_time;     /* Last modified date & time */
//    uint32_t                           permissions;  /* Not supported yet */
} mico_dir_entry_details_t;


/**
 * A list element for user interactive selection of filesystem devices
 */
typedef struct
{
     mico_block_device_t*          device;
     mico_filesystem_handle_type_t type;
     char*                          name;
} filesystem_list_t;

/**
 * A mounted filesystem handle entry
 */
typedef struct mico_filesystem_mounted_device_struct
{
      mico_filesystem_t*            fs_handle;
      char                           name[MICO_FILESYSTEM_MOUNT_NAME_LENGTH_MAX];
} mico_filesystem_mounted_device_t;


/**
 * Info of filesystem
 */
typedef struct mico_filesystem_info_t {
  int total_space;
  int free_space;
} mico_filesystem_info;

/******************************************************
 *                 Global Variables
 ******************************************************/

/**
 * A List of all filesystem devices available on the platform - for interactive user selection (e.g. console app)
 * Terminated by an element where the device pointer is NULL
 */
extern const filesystem_list_t   all_filesystem_devices[];

extern const mico_block_device_driver_t tester_block_device_driver;

/******************************************************
 *               Function Declarations
 ******************************************************/
/**
 * NOTE: The idea of a present/current working directory (pwd/cwd) has been intentionally omitted.
 *       These inherently require per-thread storage which unnecessarily complicates things
 */

/**
 * Initialise the filesystem module
 *
 * Initialises the filesystem module before mounting a physical device.
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_init ( void );


/**
 * Mount the physical device
 *
 * This assumes that the device is ready to read/write immediately.
 *
 * @param[in]  device        - physical media to init
 * @param[out] fs_handle_out - Receives the filesystem handle.
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_mount ( mico_block_device_t* device, mico_filesystem_handle_type_t fs_type, mico_filesystem_t* fs_handle_out, const char* mounted_name);


/**
 * Unmount the filesystem
 *
 * @param[in]  fs_handle   - the filesystem to unmount
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_unmount ( mico_filesystem_t* fs_handle );


/**
 * Get the filesystem handle by name
 *
 * @param[in]  mounted_name   - the mounted name for search corresponding fs_handle
 *
 * @return fs_handle on success, NULL on failure
 */
mico_filesystem_t* mico_filesystem_retrieve_mounted_fs_handle ( const char* mounted_name );


/**
 * Gets the size/timestamp/attribute details of a file
 *
 * @param[in]  fs_handle      - The filesystem handle to use - obtained from wiced_filesystem_mount
 * @param[in]  filename       - The filename of the file to examine
 * @param[out] details_out    - Receives the details of the file
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_file_get_details ( mico_filesystem_t* fs_handle, const char* filename, mico_dir_entry_details_t* details_out );

/**
 * Open a file for reading or writing
 *
 * @param[in]  fs_handle       - The filesystem handle to use - obtained from wiced_filesystem_mount
 * @param[out] file_handle_out - a pointer to a wiced_file_t structure which will receive the
 *                               file handle after it is opened
 * @param[in]  filename        - The filename of the file to open
 * @param[in]  mode            - Specifies read or write access
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_file_open ( mico_filesystem_t* fs_handle, mico_file_t* file_handle_out, const char* filename, mico_filesystem_open_mode_t mode );

/**
 * Seek to a location within a file
 *
 * This is similar to the fseek() in ISO C.
 *
 * @param[in] file_handle - The file handle on which to perform the seek.
 *                          Must have been previously opened with wiced_filesystem_fopen.
 * @param[in] offset      - The offset in bytes
 * @param[in] whence      - MICO_FILESYSTEM_SEEK_SET = Offset from start of file
 *                          MICO_FILESYSTEM_SEEK_CUR = Offset from current position in file
 *                          MICO_FILESYSTEM_SEEK_END = Offset from end of file
 *
 * @return kNoErr  on success
 */
OSStatus mico_filesystem_file_seek ( mico_file_t* file_handle, int64_t offset, mico_filesystem_seek_type_t whence );


/**
 * Returns the current location within a file
 *
 * This is similar to the ftell() in ISO C.
 *
 * @param[in]  file_handle - The file handle to be examined
 * @param[out] location    - Receives the current location within the file
 *
 * @return kNoErr  on success
 */
OSStatus mico_filesystem_file_tell ( mico_file_t* file_handle, uint64_t* location );


/**
 * Reads data from a file into a memory buffer
 *
 * @param[in] file_handle          - the file handle to read from
 * @param[out] data                - A pointer to the memory buffer that will
 *                                   receive the data that is read
 * @param[in] bytes_to_read        - the number of bytes to read
 * @param[out] returned_item_count - the number of items successfully read.
 *
 * @return kNoErr  on success
 */
OSStatus mico_filesystem_file_read ( mico_file_t* file_handle, void* data, uint64_t bytes_to_read, uint64_t* returned_bytes_count );


/**
 * Writes data to a file from a memory buffer
 *
 * @param[in] file_handle          - the file handle to write to
 * @param[in] data                 - A pointer to the memory buffer that contains
 *                                    the data that is to be written
 * @param[in] bytes_to_write       - the number of bytes to write
 * @param[out] written_bytes_count - receives the number of items successfully written.
 *
 * @return kNoErr  on success
 */
OSStatus mico_filesystem_file_write( mico_file_t* file_handle, const void* data, uint64_t bytes_to_write, uint64_t* written_bytes_count );


/**
 * Flush write data to media
 *
 * This is similar to the fflush() in ISO C.
 *
 * @param[in] file_handle - the file handle to flush
 *
 * @return kNoErr  on success
 */
OSStatus mico_filesystem_file_flush ( mico_file_t* file_handle );


/**
 * Check the end-of-file flag for a file
 *
 * This is similar to the feof() in ISO C.
 *
 * @param[in] file_handle - the file handle to check for EOF
 *
 * @return 1 = EOF or invalid file handle
 */
int mico_filesystem_file_end_reached ( mico_file_t* file_handle );


/**
 * Close a file
 *
 * This is similar to the fclose() in ISO C.
 *
 * @param[in] file_handle - the file handle to close
 *
 * @return kNoErr = success
 */
OSStatus mico_filesystem_file_close ( mico_file_t* file_handle );


/**
 * Delete a file
 *
 * This is similar to the remove() in ISO C.
 *
 * @param[in]  fs_handle      - The filesystem handle to use - obtained from wiced_filesystem_mount
 * @param[in]  filename       - the filename of the file to delete
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_file_delete ( mico_filesystem_t* fs_handle, const char* filename );


/**
 * Opens a directory
 *
 * This is similar to the opendir() in ISO C.
 *
 * @param[in]  fsp      - The filesystem handle to use - obtained from wiced_filesystem_mount
 * @param[out] dirp     - a pointer to a directory structure which
 *                        will be filled with the opened handle
 * @param[in]  dir_name - the path of the directory to open
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_dir_open ( mico_filesystem_t* fs_handle, mico_dir_t* dir_handle, const char* dir_name );


/**
 * Reads a directory entry
 *
 * This is similar to the readdir() in ISO C.
 *
 * @param[in]  dir_handle         - the directory handle to read from
 * @param[out] name_buffer        - pointer to a buffer that will receive the filename
 * @param[in]  name_buffer_length - the maximum number of bytes that can be put in the buffer
 * @param[out] type               - pointer to variable that will receive entry type (file or dir)
 * @param[out] details            - pointer to variable that will receive entry information (attribute, size, modified date/time)
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_dir_read( mico_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length, mico_dir_entry_type_t* type, mico_dir_entry_details_t* details );


/**
 * Check the end-of-directory flag for a directory
 *
 * Checks whether the selected directory handle is
 * at the end of the available directory entries.
 *
 * @param[in] dir_handle - the directory handle to check
 *
 * @return 1 = End-of-Directory
 */
int mico_filesystem_dir_end_reached ( mico_dir_t* dir_handle );


/**
 * Returns a directory handle to the first entry
 *
 * This is similar to the rewinddir() in ISO C.
 *
 * @param[in] dir_handle - the directory handle to rewind
 *
 * @return kNoErr = Success
 */
OSStatus mico_filesystem_dir_rewind ( mico_dir_t* dir_handle );


/**
 * Closes a directory handle
 *
 * @param[in] dir_handle - the directory handle to close
 *
 * @return kNoErr = Success
 */
OSStatus mico_filesystem_dir_close ( mico_dir_t* dir_handle );


/**
 * Create a directory
 *
 * @param[in]  fs_handle       - The filesystem handle to use
 * @param[in]  directory_name  - the path of the directory to create
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_dir_create( mico_filesystem_t* fs_handle, const char* directory_name );


/**
 * Formats the media
 *
 * Creates a new, blank filesystem
 *
 * @param[in]  device   - The block device to format
 * @param[in]  fs_type  - Which type of filesystem to create
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_format( mico_block_device_t* device, mico_filesystem_handle_type_t fs_type );


/**
 * Get info of a mounted device
 *
 * @param[in]  fs_handle      - The filesystem handle to use
 * @param[in]  info           - info of filesystem
 * @param[in]  mounted_name   - name of the mounted device
 *
 * @return kNoErr on success
 */
OSStatus mico_filesystem_get_info( mico_filesystem_t* fs_handle, mico_filesystem_info* info, char* mounted_name );


/**
 * scan files of a mounted deivce
 * @param[in]  fs_handle      - The filesystem handle to use
 * @param[in]  arg           - scan file callback
 * @param[in]  mounted_name   - name of the mounted device
 */
OSStatus mico_filesystem_scan_files( mico_filesystem_t* fs_handle, char* mounted_name, mico_scan_file_handle arg );

/******************************************************
 *  Opaque types - do not use directly
 ******************************************************/

struct mico_dir_struct
{
    mico_filesystem_driver_t* driver;
    mico_filesystem_t*        filesystem;

    union
    {
#ifdef USING_FTFS
        file *f;
#endif /* USING_WICEDFS */
#ifdef USING_FATFS
        struct
        {
            FATFS_DIR    handle;
            mico_bool_t eodir;
        } fatfs;
#endif /* USING_FATFS */
    } data;
};

struct mico_file_struct
{
    mico_filesystem_driver_t* driver;
    mico_filesystem_t*        filesystem;
    union
    {
#ifdef USING_FTFS
        file *f;
#endif /* USING_WICEDFS */
#ifdef USING_FATFS
        FIL fatfs;
#endif /* USING_FATFS */
    } data;
};

struct mico_filesystem_struct
{
    mico_filesystem_driver_t* driver;
    mico_block_device_t*      device;
    union
    {
#ifdef USING_FTFS
        struct ftfs_super sb;
        struct fs * fs;
        file * f;
#endif /* USING_WICEDFS */
#ifdef USING_FATFS
        struct
        {
            FATFS       handle;
            const TCHAR drive_id[5];
        } fatfs;
#endif /* USING_FATFS */
    } data;
};



#ifdef __cplusplus
} /*extern "C" */
#endif
