/*
 * Copyright 2008-2014, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <wm_utils.h>
#include <wmassert.h>

#define CRC_16_CCITT 0x8408	/*!< CRC mode: CRC-16-CCITT */
#define TABLE_SIZE 16

static uint32_t crc_table[TABLE_SIZE];
static bool init_done;

void soft_crc16_init(void)
{
	uint16_t crc = 0;
	uint8_t i;
	uint8_t j;

	if (init_done)
		return;

	for (j = 0; j < TABLE_SIZE; j++) {
		crc = 0;
		for (i = 0x01; i != 0x10; i <<= 1) {
			if ((crc & 0x0001) != 0) {
				crc >>= 1;
				crc ^= CRC_16_CCITT;
			} else {
				crc >>= 1;
			}
			if ((j & i) != 0) {
				crc ^= CRC_16_CCITT;
			}
		}
		crc_table[j] = crc;
	}
	init_done = true;
}

uint32_t soft_crc16(const void *__buf, uint16_t len, uint32_t crc_init)
{
	const uint8_t *buf = __buf;
	uint8_t crc_H8;
	uint16_t crc = (uint16_t) crc_init;

	ASSERT(init_done == 1);

	while (len--) {
		crc_H8 = (uint8_t)(crc & 0x000F);
		crc >>= 4;
		crc ^= crc_table[crc_H8 ^ (*buf & 0x0F)];
		crc_H8 = (uint8_t)(crc & 0x000F);
		crc >>= 4;
		crc ^= crc_table[crc_H8 ^ (*buf >> 4)];
		buf++;
	}

	return (uint32_t)crc;
}

