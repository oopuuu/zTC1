/**
 ******************************************************************************
 * @file    Retarget.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 ******************************************************************************
 */

#include <stdlib.h>
#include <yfuns.h>
#include "platform.h"
#include "platform_config.h"
#include "mico_platform.h"

#if defined (MOC) && ( MOC == 1 )
#include "moc_api.h"
extern mico_api_t *lib_api_p;
/* memory management*/
void* _malloc(size_t size)
{
    return lib_api_p->malloc(size);
} // malloc
void* _realloc(void* pv, size_t size)
{
    return lib_api_p->realloc(pv, size);
} // realloc
void _free(void* pv)
{
    lib_api_p->free(pv);
}     //free
void* _calloc(size_t a, size_t b)
{
    return lib_api_p->calloc(a, b);
}     // calloc
#endif


#ifdef BOOTLOADER
int putchar(int ch)
{
  MicoUartSend( STDIO_UART, &ch, 1 );
  return ch;
}
#else
#if (!defined CONFIG_PLATFORM_8195A) || (defined MOC100) 
size_t __write( int handle, const unsigned char * buffer, size_t size )
{
  UNUSED_PARAMETER(handle);
  
  if ( buffer == 0 )
  {
    return 0;
  }

#ifndef MICO_DISABLE_STDIO
  MicoUartSend( STDIO_UART, (const char*)buffer, size );
#endif
  
  return size;
}
#endif
#endif
