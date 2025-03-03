/* 
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __CRC32_H
#define __CRC32_H

#include <stdint.h>

uint32_t crc32(const void *__data, int data_size, uint32_t crc);
void crc32_init(void);

#endif
