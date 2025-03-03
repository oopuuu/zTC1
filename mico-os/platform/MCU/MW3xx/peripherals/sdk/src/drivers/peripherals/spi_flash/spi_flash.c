/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include <wmstdio.h>
#include <wm_os.h>
#include <flash.h>
#include <peripherals/spi_flash.h>

static mdev_t MDEV_spi_flash;
static const char *MDEV_NAME_spi_flash = "spi_flash";
static flash_cfg fl_cfg;
static os_mutex_t spi_flash_mutex;

int spi_flash_drv_read_id(mdev_t *mdev, uint8_t *p_id)
{
	int ret = 0;

	ret = os_mutex_get(&spi_flash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		SPI_LOG("Failed to get mutex\n\r");
		return ret;
	}
	ret = spi_flash_read_id(p_id);
	os_mutex_put(&spi_flash_mutex);

	return ret;
}

int spi_flash_drv_eraseall(mdev_t *mdev)
{
	int ret = 0;

	ret = os_mutex_get(&spi_flash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		SPI_LOG("Failed to get mutex\n\r");
		return ret;
	}
	ret = spi_flash_eraseall();
	os_mutex_put(&spi_flash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int spi_flash_drv_erase(mdev_t *mdev, uint32_t start, uint32_t size)
{
	int ret = 0;
	ret = os_mutex_get(&spi_flash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		SPI_LOG("Failed to get mutex\n\r");
		return ret;
	}
	ret = spi_flash_erase(start, (start + size - 1));
	os_mutex_put(&spi_flash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int spi_flash_drv_read(mdev_t *mdev, uint8_t *p_buf, uint32_t size,
							uint32_t address)
{
	int ret = 0;

	ret = os_mutex_get(&spi_flash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		SPI_LOG("Failed to get mutex\n\r");
		return ret;
	}
	ret = spi_flash_read(p_buf, size, address);
	os_mutex_put(&spi_flash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int spi_flash_drv_write(mdev_t *mdev, uint8_t *p_buf, uint32_t size,
							uint32_t address)
{

	int ret = 0;

	ret = os_mutex_get(&spi_flash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		SPI_LOG("Failed to get mutex\n\r");
		return ret;
	}
	ret = spi_flash_write(p_buf, size, address);
	os_mutex_put(&spi_flash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int spi_flash_drv_sleep(mdev_t *mdev)
{
	int ret = 0;

	ret = os_mutex_get(&spi_flash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		SPI_LOG("Failed to get mutex\n\r");
		return ret;
	}
	ret = spi_flash_sleep();
	os_mutex_put(&spi_flash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int spi_flash_drv_wakeup(mdev_t *mdev)
{
	int ret = 0;

	ret = os_mutex_get(&spi_flash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		SPI_LOG("Failed to get mutex\n\r");
		return ret;
	}
	ret = spi_flash_wakeup();
	os_mutex_put(&spi_flash_mutex);

	return ret ? 0 : -WM_FAIL;
}

mdev_t *spi_flash_drv_open(const char *name, uint32_t flags)
{
	mdev_t *mdev_p = mdev_get_handle(name);

	if (mdev_p == NULL) {
		SPI_LOG("driver open called without registering device"
							" (%s)\n\r", name);
		return NULL;
	}

	return mdev_p;
}

int spi_flash_drv_close(mdev_t *dev)
{
	return WM_SUCCESS;
}

int spi_flash_drv_init()
{
	int ret;

	if (mdev_get_handle(MDEV_NAME_spi_flash) != NULL)
		return WM_SUCCESS;

	MDEV_spi_flash.name = MDEV_NAME_spi_flash;

	fl_cfg.fl_dev = FL_SPI_EXT;
	MDEV_spi_flash.private_data = (uint32_t)&fl_cfg;

	ret = os_mutex_create(&spi_flash_mutex, "spi_flash",
						OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	spi_flash_init();

	mdev_register(&MDEV_spi_flash);

	return WM_SUCCESS;
}
