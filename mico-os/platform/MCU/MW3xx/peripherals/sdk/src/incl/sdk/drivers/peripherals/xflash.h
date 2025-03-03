/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef _XFLASH_H_
#define _XFLASH_H_

#include <mdev.h>
#include <mc200_xflash.h>
#include <mc200_qspi1.h>
#include <wmlog.h>

int xflash_drv_init(void);
mdev_t *xflash_drv_open(const char *name, uint32_t flags);
int xflash_drv_close(mdev_t *dev);
int xflash_drv_eraseall(mdev_t *dev);
int xflash_drv_erase(mdev_t *dev, unsigned long start,
		     unsigned long size);
int xflash_drv_read(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr);
int xflash_drv_write(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr);
#endif /* _XFLASH_H_ */
