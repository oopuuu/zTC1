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


#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <malloc.h>


//#include "platform.h"

extern char read_usart_blocking( void );
extern void write_usart_blocking( char channel, char val );

#undef errno
extern int errno;

struct mxchip_mallinfo {
  int num_of_chunks;  /* number of free chunks */
  int total_memory;  /* maximum total allocated space */
  int allocted_memory; /* total allocated space */
  int free_memory; /* total free space */
};
#if 1
/* sbrk
 * Increase program data space.
 * Malloc and related functions depend on this
 */
extern unsigned char _heap[];
extern unsigned char _eheap[];
static unsigned char *sbrk_heap_top = _heap;
caddr_t _sbrk( int incr )
{
    unsigned char *prev_heap;

    if ( sbrk_heap_top + incr > _eheap )
    {
        /* Out of dynamic memory heap space */
        errno = ENOMEM;
        return (caddr_t) -1;
    }
    prev_heap = sbrk_heap_top;

    sbrk_heap_top += incr;

    return (caddr_t) prev_heap;
}

/* Override the default Newlib assert, since it tries to do printf stuff */

void __assert_func( const char * file, int line, const char * func, const char * failedexpr )
{
    /* Assertion failed!
     *
     * To find out where this assert was triggered, either look up the call stack,
     * or inspect the file, line and function parameters
     */

    /* unused parameters */
    (void)file;
    (void)line;
    (void)func;
    (void)failedexpr;
}

/*
 * These are needed for C++ programs. They shouldn't really be here, so let's just
 * hit a breakpoint when these functions are called.
 */

#if 1

int _kill( int pid, int sig )
{
    return 0;
}

int _getpid( )
{
    return 0;
}

#endif

int __data_GetMemChunk()
{
    return 0;
}

struct mxchip_mallinfo *mxchip_memory_info(void);

#if defined (MOC) && ( MOC == 1 )
#include "moc_api.h"
extern mico_api_t *lib_api_p;

void * __wrap_malloc (size_t size)
{
	return lib_api_p->malloc(size);
}

void * __wrap__malloc_r (void *p, size_t size)
{
	
	return lib_api_p->malloc(size);
}

void __wrap_free (void *pv)
{
	lib_api_p->free(pv);
}


void * __wrap_calloc (size_t a, size_t b)
{
	return lib_api_p->calloc(a, b);
}

void * __wrap_realloc (void* pv, size_t size)
{
	return lib_api_p->realloc(pv, size);
}


void __wrap__free_r (void *p, void *x)
{
  lib_api_p->free(x);
}

void* __wrap__realloc_r (void *p, void* x, size_t sz)
{
  return realloc (x, sz);
}


#else
struct mxchip_mallinfo* mico_memory_info(void)
{
    struct mallinfo mi = mallinfo();
    static struct mxchip_mallinfo mico_memory;
    unsigned int total_mem = _eheap - _heap;
	
    mico_memory.allocted_memory = mi.uordblks;
    mico_memory.free_memory = total_mem - mi.uordblks;
    mico_memory.num_of_chunks = mi.ordblks;
    mico_memory.total_memory = total_mem;
    return &mico_memory;
}
#endif // MOC

#else

caddr_t _sbrk( int incr )
{
    return (caddr_t) -1;
}

void _exit(int __status )
{
}

struct mxchip_mallinfo *mxchip_memory_info(void);

struct mxchip_mallinfo* mico_memory_info(void)
{
  struct mallinfo mi = mallinfo();
  static struct mxchip_mallinfo mico_memory;
 // unsigned int total_mem = _eheap - _heap;

  mico_memory.allocted_memory = mi.uordblks;
  //mico_memory.free_memory = total_mem - mi.uordblks;
  mico_memory.num_of_chunks = mi.ordblks;
  //mico_memory.total_memory = total_mem;
    return &mico_memory;
}
#endif
