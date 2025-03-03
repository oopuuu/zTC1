/*
 * Copyright (C) 2013, Marvell International Ltd.
 * All Rights Reserved.
 */

#include "crc.h"

#define TABLE_SIZE 256

static uint32_t crc_table[TABLE_SIZE];
static const int rcrc32 = 0xEDB88320;

void soft_crc32_init(void)
{
	unsigned int crc = 0;
	unsigned char i;
	unsigned int j;

	for (j = 0; j < TABLE_SIZE; j++) {
		crc = 0;
		for (i = 0x01; i != 0x00; i <<= 1) {
			if ((crc & 0x00000001) != 0) {
				crc >>= 1;
				crc ^= rcrc32;
			} else {
				crc >>= 1;
			}
			if ((j & i) != 0)
				crc ^= rcrc32;
		}
		crc_table[j] = crc;
	}
}

uint32_t soft_crc32(const void *__data, int data_size, uint32_t crc)
{
	const uint8_t *data = __data;
	unsigned int result = crc;
	unsigned char crc_H8;

	while (data_size--) {
		crc_H8 = (unsigned char)(result & 0x000000FF);
		result >>= 8;
		result ^= crc_table[crc_H8 ^ (*data)];
		data++;
	}

	return result;
}
