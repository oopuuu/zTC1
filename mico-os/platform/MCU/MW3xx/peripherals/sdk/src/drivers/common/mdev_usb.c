/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * mdev_usb.c: mdev driver for USB
 */
#include <wmstdio.h>
#include <mdev_usb.h>
#include <wm_os.h>
#include <incl/usbsysinit.h>
#include <incl/usb_device_api.h>

static mdev_t MDEV_usb;
static const char *MDEV_NAME_usb = "MDEV_USB";
static os_mutex_t usb_mutex;

int usb_drv_write(mdev_t *dev, uint8_t *buf, uint32_t nbytes,
	uint32_t timeout, uint32_t endpointnum)
{
	int ret, bytes_write;

	ret = os_mutex_get(&usb_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_LOG("failed to get mutex\n\r");
		return ret;
	}
	bytes_write = usb2Write(buf, nbytes, timeout,
			endpointnum);

	os_mutex_put(&usb_mutex);

	return bytes_write;
}

int usb_drv_read(mdev_t *dev, uint8_t *buf, uint32_t nbytes,
	uint32_t timeout, uint32_t endpointnum)
{
	int ret, bytes_read;

	ret = os_mutex_get(&usb_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_LOG("failed to get mutex\n\r");
		return ret;
	}

	bytes_read = usb2Read(buf, nbytes, timeout,
				endpointnum);

	os_mutex_put(&usb_mutex);
	return bytes_read;
}

int usb_drv_close(mdev_t *dev)
{
	return WM_SUCCESS;
}

mdev_t *usb_drv_open(const char *name)
{
	mdev_t *mdev_p = mdev_get_handle(name);

	if (mdev_p == NULL) {
		USB_LOG("driver open called without registering device"
							" (%s)\n\r", name);
		return NULL;
	}
	return mdev_p;
}

int usb_drv_init(int usb_class)
{
	int ret;

	if (mdev_get_handle(MDEV_NAME_usb) != NULL)
		return WM_SUCCESS;

	MDEV_usb.name = MDEV_NAME_usb;

	ret = os_mutex_create(&usb_mutex, "usb", OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	ret = usb_device_system_init(usb_class);
	if (ret == -WM_FAIL) {
		USB_LOG("Failed to initialize usb device");
		return -WM_FAIL;
	}

	mdev_register(&MDEV_usb);

	return WM_SUCCESS;
}
