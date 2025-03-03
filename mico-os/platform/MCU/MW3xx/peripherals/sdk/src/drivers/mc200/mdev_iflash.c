/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * mdev_iflash.c: mdev driver for internal flash
 */
#include <pwrmgr.h>
#include <mdev_iflash.h>
#include <lowlevel_drivers.h>
#include <flash.h>
#include <wmstdio.h>
#include <wm_os.h>

static mdev_t MDEV_iflash;
static const char *MDEV_NAME_iflash = "iflash";
static flash_cfg fl_cfg;
static os_mutex_t iflash_mutex;


void iflash_ps_cb(power_save_event_t event, void *data)
{

	switch (event) {
	case ACTION_ENTER_PM2: {
		/* Deep off Vfl on entry to PM2 */
		PMU_PowerDeepOffVDDIO(PMU_VDDIO_FL);
		/* Disable Serial Flash pins */
		SFLASH_HOLDnConfig(MODE_SHUTDOWN);
		SFLASH_DIOConfig(MODE_SHUTDOWN);
		SFLASH_WPConfig(MODE_SHUTDOWN);
		SFLASH_DOConfig(MODE_SHUTDOWN);
	}
		break;
	case ACTION_EXIT_PM2: {
		/* Turn on  Vfl on exit form  PM2 */
		PMU_PowerOnVDDIO(PMU_VDDIO_FL);
		/* Restore internal flash pin mode to defualt state */
		SFLASH_HOLDnConfig(MODE_DEFAULT);
		SFLASH_DIOConfig(MODE_DEFAULT);
		SFLASH_WPConfig(MODE_DEFAULT);
		SFLASH_DOConfig(MODE_DEFAULT);
	}
		break;
	case ACTION_ENTER_PM3:
	case ACTION_ENTER_PM4: {
		/* Turn off  Vfl on exit form  PM3/4 */
		PMU_PowerOffVDDIO(PMU_VDDIO_FL);
	}
		break;
	case ACTION_EXIT_PM3: {
		/* Turn on  Vfl on exit form  PM3/4 */
		PMU_PowerOnVDDIO(PMU_VDDIO_FL);
	}

	default:
		break;
	}
}

mdev_t *iflash_drv_open(const char *name, uint32_t flags)
{
	mdev_t *mdev_p = mdev_get_handle(name);

	if (mdev_p == NULL) {
		ifl_d("open called without reg dev (%s)", name);
		return NULL;
	}

	return mdev_p;
}


int iflash_drv_close(mdev_t *dev)
{
	return 0;
}

int iflash_drv_get_id(mdev_t *dev, uint64_t *id)
{
	flash_cfg *temp;
	temp = (flash_cfg *) dev->private_data;
	*id = temp->fl_id;
	return WM_SUCCESS;

}
int iflash_drv_erase(mdev_t *dev, unsigned long start, unsigned long size)
{
	int ret = 0;

	if (FLASH_GetInterface() == FLASH_INTERFACE_FLASHC) {
		ifl_d("Cannot erase when Flash Controller is enabled");
		return -WM_FAIL;
	}

	ret = os_mutex_get(&iflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	ret = FLASH_Erase(start, (start + size - 1));
	os_mutex_put(&iflash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int iflash_drv_eraseall(mdev_t *dev)
{
	int ret = 0;

	if (FLASH_GetInterface() == FLASH_INTERFACE_FLASHC) {
		ifl_d("Cannot erase when Flash Controller is enabled");
		return -WM_FAIL;
	}

	ret = os_mutex_get(&iflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	ret = FLASH_EraseAll();
	os_mutex_put(&iflash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int iflash_drv_read(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr)
{
	int ret;

	if (len == 0)
		return 0;
	if (!buf)
		return -WM_FAIL;

	ret = os_mutex_get(&iflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	if (FLASH_GetInterface() == FLASH_INTERFACE_FLASHC) {
		memcpy(buf, (uint8_t *)addr + 0x1f000000, len);
		ret = len;
	} else
		ret = FLASH_Read(FLASH_FAST_READ_QUAD_IO, addr, buf, len);
	os_mutex_put(&iflash_mutex);

	return (ret == len) ? 0 : -WM_FAIL;
}


int iflash_drv_write(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr)
{
	int ret;

	if (len == 0)
		return 0;
	if (!buf)
		return -WM_FAIL;

	if (FLASH_GetInterface() == FLASH_INTERFACE_FLASHC) {
		ifl_d("Cannot write when Flash Controller is enabled");
		return -WM_FAIL;
	}

	ret = os_mutex_get(&iflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	ret = FLASH_Write(FLASH_PROGRAM_QUAD, addr, buf, len);
	os_mutex_put(&iflash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int iflash_drv_init(void)
{
	int ret;

	if (mdev_get_handle(MDEV_NAME_iflash) != NULL)
		return WM_SUCCESS;

	MDEV_iflash.name = MDEV_NAME_iflash;

	ret = os_mutex_create(&iflash_mutex, "iflash", OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	if (FLASH_GetInterface() == FLASH_INTERFACE_QSPI0) {
		/* Max QSPI0 Clock Frequency is 50MHz */
		if (CLK_GetSystemClk() > 50000000)
			CLK_ModuleClkDivider(CLK_QSPI0, 4);
		QSPI0_Init_CLK();
		/* Store the id for internal flash into private data */
		fl_cfg.fl_id = FLASH_GetUniqueID();
	}

	fl_cfg.fl_dev = FL_INT;
	MDEV_iflash.private_data = (uint32_t)&fl_cfg;

	mdev_register(&MDEV_iflash);

	int handle = pm_register_cb(ACTION_ENTER_PM2|ACTION_EXIT_PM2
				    | ACTION_ENTER_PM3|ACTION_EXIT_PM3
				    | ACTION_ENTER_PM4,
				    iflash_ps_cb, NULL);
	if (handle < WM_SUCCESS)
		return -WM_FAIL;
	else
		return WM_SUCCESS;
}
