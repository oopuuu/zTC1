/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#include <mdev.h>
#include <lowlevel_drivers.h>

#define SPI_LOG(...)  wmlog("spi_flash", ##__VA_ARGS__)

int spi_flash_drv_read_id(mdev_t *mdev, uint8_t *p_id);
int spi_flash_drv_eraseall(mdev_t *mdev);
int spi_flash_drv_erase(mdev_t *mdev, uint32_t start, uint32_t size);
int spi_flash_drv_read(mdev_t *mdev, uint8_t *p_buf, uint32_t size,
			uint32_t address);
int spi_flash_drv_write(mdev_t *mdev, uint8_t *p_buf, uint32_t size,
			uint32_t address);
int spi_flash_drv_sleep(mdev_t *mdev);
int spi_flash_drv_wakeup(mdev_t *mdev);
mdev_t *spi_flash_drv_open(const char *name, uint32_t flags);
int spi_flash_drv_close(mdev_t *dev);
int spi_flash_drv_init();

#endif /* __SPI_FLASH_H__ */
