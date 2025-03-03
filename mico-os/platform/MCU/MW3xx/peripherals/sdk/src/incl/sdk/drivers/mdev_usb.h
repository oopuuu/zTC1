/*  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */
/*! \file mdev_usb.h
 *  \brief Universal Serial Bus (USB) Driver
 *
 * The USB Driver (for USB device only, no support for USB Host) is meant to
 * use usb hardware to communicate with external devices connected on the USB
 * bus.
 *
 * @section mdev_usb_usage Usage
 *
 * A typical USB device usage scenario is as follows:
 *
 * -# Initialize the USB driver using usb_drv_init(usb_class).
 * -# Open the USB device handle using usb_drv_open() call.
 * -# Use usb_drv_read() or usb_drv_write() calls to read USB data or
 *  to write data to USB respectively.
 * -# Close the USB device using usb_drv_close() after its use.
 *
 * Code snippet:\n
 * Following code demonstrates how to use USB driver APIs for read/write
 * operation, it reads 4 bytes and echoes them back.
 *
 * @code
 * {
 *  int ret, timeout;
 *  mdev_t *usb_dev;
 *  char buf[4];
 *
 *  ret = usb_drv_init(USB_CDC_CLASS);
 *  if (ret != WM_SUCCESS)
 *  	return;
 *  usb_dev = usb_drv_open("MDEV_USB");
 *  if (usb_dev == NULL)
 *  	return;
 *
 *  timeout = 60;
 *  ret = usb_drv_read(usb_dev, buf, sizeof(buf), timeout, 2);
 *  if (ret != 0 && ret != WM_E_USB_TIMEOUT_ERROR &&
 *         ret != WM_E_USB_CONNECTION_ERROR)
 *	usb_drv_write(usb_dev, buf, sizeof(buf), timeout, 2);
 *
 *  usb_drv_close(usb_dev);
 * }
 * @endcode
 */

#ifndef _MDEV_USB_H_
#define _MDEV_USB_H_

#include <mdev.h>
#include <wmerrno.h>

#define USB_LOG(...)  wmprintf("[usb] " __VA_ARGS__)

/** enum indicates usb device class */
typedef enum {
	/** USB Communication Device Class */
	USB_CDC_CLASS = 1,
	/** USB Human Interface Device Class */
	USB_HID_CLASS = 2,
	/** USB Mass Storage Class */
	USB_MSC_CLASS = 4,
} usb_class;

/*
 * WM_E_USB_TIMEOUT_ERROR and WM_E_USB_CONNECTION_ERROR are generated in
 * the USB stack and correspond to CONIO_TIMEOUT and CONIO_CONERROR
 * respectively.
 */
/** USB timeout error */
#define WM_E_USB_TIMEOUT_ERROR		(-1)
/** USB connection error */
#define WM_E_USB_CONNECTION_ERROR	(-2)

/** Initialize the USB Driver
 *
 * This function registers the USB driver with the mdev interface and
 * initializes the USB device with the provided operational class.
 *
 * @param [in] usb_class for which device should be used. e.g
 * \ref USB_CDC_CLASS, \ref USB_HID_CLASS or \ref USB_MSC_CLASS
 * @return WM_Success on Success
 * @return -WM_FAIL on failure
 */
int usb_drv_init(int usb_class);

/** Open USB driver
 *
 * This returns a handle that should be used for other USB driver calls.
 *
 * @param name Name of mdev usb driver.
 * 		It should be "MDEV_USB" string.
 * @return handle to driver on Success
 * @return NULL otherwise
 */
mdev_t *usb_drv_open(const char *name);

/** Close USB Device
 *
 * This function closes the device after use.
 *
 * @param [in] dev Handle to the USB driver returned by usb_drv_open()
 * @return WM_Success on Success
 */
int usb_drv_close(mdev_t *dev);

/** Read from USB
 *
 * This function reads the specified number of bytes into the provided buffer
 * from the USB device. The timeout parameter makes this call NON-BLOCKING.
 *
 * @param [in] dev mdev_t handle to the driver
 * @param [out] buf Buffer for storing the values read
 * @param [in] nbytes Number of bytes to be read
 * @param [in] timeout for read operation in OS ticks
 * @param [in] endpointnum number from which read needs to be performed
 * @return number of bytes read on Success
 * @return \ref WM_E_USB_TIMEOUT_ERROR or \ref WM_E_USB_CONNECTION_ERROR
 * otherwise
 */
int usb_drv_read(mdev_t *dev, uint8_t *buf, uint32_t nbytes,
	uint32_t timeout, uint32_t endpointnum);

/** Write to USB
 *
 * This function writes the specified number of bytes to the USB device.
 * The timeout parameter makes this call NON-BLOCKING, write operation gets
 * aborted on time out.
 *
 * @param [in] dev mdev_t handle to the driver
 * @param [in] buf Buffer containing data to be written
 * @param [in] nbytes Number of bytes to be written
 * @param [in] timeout for write operation in OS ticks
 * @param [in] endpointnum number on which write needs to be performed
 * @return number of bytes written on Success
 * @return \ref WM_E_USB_CONNECTION_ERROR otherwise
 */
int usb_drv_write(mdev_t *dev, uint8_t *buf, uint32_t nbytes,
	uint32_t timeout, uint32_t endpointnum);

#endif
