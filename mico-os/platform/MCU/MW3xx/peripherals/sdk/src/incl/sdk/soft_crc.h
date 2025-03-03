/*
 *  Copyright (C) 2011-2013, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __CRC_H__
#define __CRC_H__

#include <stdint.h>

uint32_t soft_crc32(const void *__data, int data_size, uint32_t crc);
void soft_crc32_init();
uint32_t soft_crc16(const void *__data, uint16_t len, uint32_t crc);
void soft_crc16_init();

#endif
