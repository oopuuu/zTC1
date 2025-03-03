/*
 *  Copyright 2014, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * mdev_wdt.c: mdev driver for watchdog
 */
#include <wmtypes.h>
#include <wmerrno.h>
#include <wmstdio.h>
#include <mdev_wdt.h>

#define MAX_TIMEOUT_VALUE 15

static mdev_t MDEV_wdt;
static const char *MDEV_NAME_wdt = "MDEV_WDT";

int wdt_drv_set_timeout(mdev_t *dev, unsigned char index)
{
	WDT_Config_Type wdt_cfg;

	if (index > MAX_TIMEOUT_VALUE) {
		WDT_LOG("Incorrect value.\r\n");
		WDT_LOG("Please enter a value in 0 - 15 range.\r\n");
		return -WM_E_INVAL;
	}

	/* Configure timeout value, 4 bit value */
	wdt_cfg.timeoutVal = index;
	/* Watchdog response mode, reset on timeout */
	wdt_cfg.mode = WDT_MODE_RESET;
	/* Keep default pulse length */
	wdt_cfg.resetPulseLen = WDT_RESET_PULSE_LEN_2;
	WDT_Init(&wdt_cfg);
	return WM_SUCCESS;
}

void wdt_drv_strobe(mdev_t *dev)
{
	WDT_RestartCounter();
}

void wdt_drv_start(mdev_t *dev)
{
	CLK_ModuleClkEnable(CLK_WDT);
#if defined(CONFIG_CPU_MW300)
	/* For 88MW300, APB1 bus runs at 50MHz whereas for 88MC200 it runs at
	 * 25MHz, hence following clk divider is added to keep timeout same.
	 */
	CLK_ModuleClkDivider(CLK_WDT, 1);
#endif
	WDT_Enable();
}

void wdt_drv_stop(mdev_t *dev)
{
	/* Watchdog timer can be disabled, but lets not expose it */
}

int wdt_drv_close(mdev_t *dev)
{
	return 0;
}

mdev_t *wdt_drv_open(const char *name)
{
	mdev_t *mdev_p = mdev_get_handle(name);

	if (mdev_p == NULL) {
		WDT_LOG("driver open called without registering device"
							" (%s)\n\r", name);
		return NULL;
	}

	return mdev_p;
}

int wdt_drv_init(void)
{
	if (mdev_get_handle(MDEV_NAME_wdt) != NULL)
		return WM_SUCCESS;

	MDEV_wdt.name = MDEV_NAME_wdt;
	mdev_register(&MDEV_wdt);

	return WM_SUCCESS;
}
