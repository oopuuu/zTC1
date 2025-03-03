/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */

/*
 * @file
 * Interface functions for Newlib libC implementation
 */

#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include "stdio_newlib.h"

#include "common.h"

#undef errno
extern int errno;


#ifndef EBADF
#include <errno.h>
#endif

int _close( int file )
{
    (void) file; /* unused parameter */
    return -1;
}

int _open(const char *name, int flags, int mode)
{
    return -1;
}

/* fstat
 * Status of an open file. For consistency with other minimal implementations in these examples,
 * all files are regarded as character special devices.
 * The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */

int _fstat( int file, struct stat *st )
{
    (void) file; /* unused parameter */

    st->st_mode = S_IFCHR;
    return 0;
}




/* isatty
 * Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */

int _isatty( int file )
{
    switch ( file )
    {
        case STDOUT_FILENO:
        case STDERR_FILENO:
        case STDIN_FILENO:
            return 1;
        default:
            /* errno = ENOTTY; */
            errno = EBADF;
            return 0;
    }
}



/* lseek - Set position in a file. Minimal implementation: */

int _lseek( int file, int ptr, int dir )
{
    (void) file; /* unused parameter */
    (void) ptr;  /* unused parameter */
    (void) dir;  /* unused parameter */
    return 0;
}





/* read
 * Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 * Returns -1 on error or blocks until the number of characters have been read.
 */

int _read( int file, char *ptr, int len )
{
    switch ( file )
    {
        case STDIN_FILENO:
            MicoUartRecv(STDIO_UART, ptr, len, 0xFFFFFFFF );
            break;
        default:
            errno = EBADF;
            return -1;
    }
    return len;
}

/* write
 * Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 * Returns -1 on error or number of bytes sent
 */

int _write( int file, char *ptr, int len )
{
	char channel;
    switch ( file )
    {
        case STDOUT_FILENO: /*stdout*/
            channel = 0;
            break;
        case STDERR_FILENO: /* stderr */
            channel = 1;
            break;
        default:
            errno = EBADF;
            return -1;
    }
    UNUSED_PARAMETER( channel );
#ifndef MICO_DISABLE_STDIO

    MicoUartSend( STDIO_UART, (const char*)ptr, len );

#endif
    return len;
}


