/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */

#ifndef INCLUDED_WWD_TOOLCHAIN_H
#define INCLUDED_WWD_TOOLCHAIN_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/
#ifndef WEAK
#define WEAK __weak
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE
#endif

#ifndef USED
#define USED __root
#endif
  
#ifndef MAY_BE_UNUSED
#define MAY_BE_UNUSED
#endif

#ifndef NORETURN
#define NORETURN
#endif

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
//void *memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen );
void *memrchr( const void *s, int c, size_t n );
void iar_set_msp(void*);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* #ifndef INCLUDED_WWD_TOOLCHAIN_H */
