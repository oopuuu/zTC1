/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include <crc32.h>
#include <wmstdio.h>
#include <wmerrno.h>
#include <wmlog.h>
#include <wm_utils.h>

#define crc_e(...)				\
	wmlog_e("crc", ##__VA_ARGS__)
#define crc_w(...)				\
	wmlog_w("crc", ##__VA_ARGS__)

#ifdef CONFIG_CRC_DEBUG
#define crc_d(...)				\
	wmlog("crc", ##__VA_ARGS__)
#else
#define crc_d(...)
#endif /* ! CONFIG_CRC_DEBUG */

#ifndef CONFIG_SOFTWARE_CRC32
#include <mdev_crc.h>
#endif

uint32_t crc32(const void *__data, int data_size, uint32_t prev_crc)
{
#ifdef CONFIG_SOFTWARE_CRC32
	return soft_crc32(__data, data_size, prev_crc);
#else
	uint32_t crc;
	if (prev_crc) {
		crc_e("Hardware crc engine do not support"
			       "checksum accumulation\r\n");
		return -WM_FAIL;
	}
	mdev_t *crc_drv = crc_drv_open(MDEV_CRC_ENGINE_0, CRC32_IEEE);
	if (crc_drv == NULL) {
		crc_e("Unable to open driver");
		return -WM_FAIL;
	}

	crc_drv_block_calc(crc_drv, (uint8_t *)__data, data_size, &crc);
	crc_drv_close(crc_drv);
	return crc;
#endif
}

void crc32_init()
{
#ifndef CONFIG_SOFTWARE_CRC32
	crc_drv_init();
#endif
}
