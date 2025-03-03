/*! \file mdev_wdt.h
 *  \brief WatchDog Timer (WDT) driver
 *
 * A watchdog timer is a piece of hardware that can be used to automatically
 * detect software anomalies and reset the processor if any occur.
 * The watchdog timer regains control in case of system failure (due to a
 * software error) to increase application reliability. The WDT can generate a
 * reset or an interrupt when the counter reaches a given timeout value.
 * WDT driver provides interface to the watchdog hardware.
 *
 * \section wdt_usage Usage
 *
 * A typical WDT device usage scenario is as follows:
 *
 * -# Initialize the watchdog timer driver using wdt_drv_init().
 * -# Open the WDT device handle for use using wdt_drv_open() call.
 * -# Call wdt_drv_set_timeout() in order to set timeout index value. It should
 *    be a 4 bit value between 0 - 15.
 * -# Call wdt_drv_start() to start the WatchDog timer.
 * -# To strobe/restart WDT device call wdt_drv_strobe().
 *
 *  Code snippet:\n
 *  Following code demonstrates how to configure WDT for timeout of 90 seconds.
 *  \code
 *  {
 *	int ret;
 *	mdev_t *wdt_dev;
 *	ret = wdt_drv_init();
 *	if (ret != WM_SUCCESS)
 *		return;
 *	wdt_dev = wdt_drv_open("MDEV_WDT");
 *	if (wdt_dev == NULL)
 *		return;
 *	ret = wdt_drv_set_timeout(wdt_dev,0xF);
 *	if (ret != WM_SUCCESS)
 *		return;
 *	wdt_drv_start(wdt_dev);
 *  }
 * \endcode
 */

/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef _MDEV_WDT_H_
#define _MDEV_WDT_H_

#include <mdev.h>
#include <lowlevel_drivers.h>
#include <wmlog.h>

#define WDT_LOG(...)  wmlog("wdt", ##__VA_ARGS__)

/** Initialize WDT driver
 *
 * This function initializes WDT driver. And registers the device with mdev
 * interface.
 *
 * \return WM_SUCCESS on success.
 */
int wdt_drv_init(void);

/** Open handle to WDT device
 *
 * This function opens the handle to WDT device and enables application to use
 * the device. This handle should be used for other calls related to WDT device.
 *
 * \param[in] name Name of the driver to be opened. Name should be "MDEV_WDT"
 * string.
 * \return NULL if error.
 * \return mdev_t handle otherwise.
 */
extern mdev_t *wdt_drv_open(const char *name);

/** Set WDT Timeout
 *
 * This function sets timeout value for watchdog timer.
 *
 * \param[in] dev Handle to the WDT device returned by wdt_drv_open().
 * \param[in] index for timeout value in between 0 - 15.
 * If index is entered as 15 (0xF), the actual timeout is around 90 seconds.
 * It reduces to half, after decrementing the index by 1 everytime.
 * \return WM_SUCCESS on success.
 * \return -WM_E_INVAL on error.
 */
extern int wdt_drv_set_timeout(mdev_t *dev, unsigned char index);

/** Start watchdog timer
 *
 * This function starts WDT operation.
 *
 * \param[in] dev Handle to the WDT device returned by wdt_drv_open().
 */
extern void wdt_drv_start(mdev_t *dev);

/** Strobe watchdog timer
 *
 * This function strobes/restarts the watchdog timer.
 *
 * \param[in] dev Handle to the WDT device returned by wdt_drv_open().
 */
extern void wdt_drv_strobe(mdev_t *dev);

/* Close WDT Device
 *
 * This function closes the device after use and frees up resources.
 *
 * \param[in] dev Handle to the WDT device returned by wdt_drv_open().
 * \return WM_SUCCESS on success.
 * \return -WM_FAIL on error.
 */
extern int wdt_drv_close(mdev_t *dev);

/* Stop watchdog timer
 *
 * This function stops watchdog timer.
 *
 * \param[in] dev Handle to the WDT device returned by wdt_drv_open().
 */
extern void wdt_drv_stop(mdev_t *dev);

#endif /* _MDEV_WDT_H_ */

