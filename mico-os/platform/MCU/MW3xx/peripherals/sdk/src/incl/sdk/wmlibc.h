/*
 *  Copyright (C) 2008-2016, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * Some functions not present in standard libc
 */

#ifndef __WMLIBC_H__
#define __WMLIBC_H__

#include <stdint.h>

uintmax_t strntoumax(const char *, char **, int, size_t);
void memswap(void *, void *, size_t);
void *memmem(const void *, size_t, const void *, size_t);

#endif /* __WMLIBC_H__ */
