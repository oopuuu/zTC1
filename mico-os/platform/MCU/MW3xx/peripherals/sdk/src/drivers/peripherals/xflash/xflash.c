/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include "mdev.h"
#include "peripherals/xflash.h"
#include "mc200_xflash.h"
#include "mc200_clock.h"
#include "mc200_gpio.h"
#include "mc200_pinmux.h"
#include <flash.h>
#include <wmstdio.h>
#include <wm_os.h>

static mdev_t MDEV_xflash;
static const char *MDEV_NAME_xflash = "xflash";
static flash_cfg fl_cfg;
static os_mutex_t xflash_mutex;

mdev_t *xflash_drv_open(const char *name, uint32_t flags)
{
	mdev_t *mdev_p = mdev_get_handle(name);

	if (mdev_p == NULL) {
		xfl_d("driver open called without reg dev (%s)\n\r", name);
		return NULL;
	}

	return mdev_p;
}


int xflash_drv_close(mdev_t *dev)
{
	return 0;
}

int xflash_drv_erase(mdev_t *dev, unsigned long start, unsigned long size)
{
	int ret = 0;

	ret = os_mutex_get(&xflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	ret = XFLASH_Erase(start, (start + size - 1));
	os_mutex_put(&xflash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int xflash_drv_eraseall(mdev_t *dev)
{
	int ret = 0;

	ret = os_mutex_get(&xflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	ret = XFLASH_EraseAll();
	os_mutex_put(&xflash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int xflash_drv_read(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr)
{
	int ret;

	if (len == 0)
		return 0;
	if (!buf)
		return -WM_FAIL;

	ret = os_mutex_get(&xflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	ret = XFLASH_Read(XFLASH_FAST_READ_QUAD_IO, addr, buf, len);
	os_mutex_put(&xflash_mutex);

	return (ret == len) ? 0 : -WM_FAIL;
}


int xflash_drv_write(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr)
{
	int ret;

	if (len == 0)
		return 0;
	if (!buf)
		return -WM_FAIL;

	ret = os_mutex_get(&xflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	ret = XFLASH_Write(XFLASH_PROGRAM_QUAD, addr, buf, len);
	os_mutex_put(&xflash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int xflash_drv_init(void)
{
	int ret;

	if (mdev_get_handle(MDEV_NAME_xflash) != NULL)
		return WM_SUCCESS;
	MDEV_xflash.name = MDEV_NAME_xflash;
	fl_cfg.fl_dev = FL_EXT;
	MDEV_xflash.private_data = (uint32_t)&fl_cfg;

	ret = os_mutex_create(&xflash_mutex, "xflash", OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Max QSPI1 Clock Frequency is 50MHz */
	if (CLK_GetSystemClk() > 50000000)
		CLK_ModuleClkDivider(CLK_QSPI1, 4);
	QSPI1_Init_CLK();

	GPIO_PinMuxFun(GPIO_72, GPIO72_QSPI1_SSn);
	GPIO_PinMuxFun(GPIO_73, GPIO73_QSPI1_CLK);
	GPIO_PinMuxFun(GPIO_76, GPIO76_QSPI1_D0);
	GPIO_PinMuxFun(GPIO_77, GPIO77_QSPI1_D1);
	GPIO_PinMuxFun(GPIO_78, GPIO78_QSPI1_D2);
	GPIO_PinMuxFun(GPIO_79, GPIO79_QSPI1_D3);

	XFLASH_PowerDown(DISABLE);
	mdev_register(&MDEV_xflash);

	return WM_SUCCESS;
}
