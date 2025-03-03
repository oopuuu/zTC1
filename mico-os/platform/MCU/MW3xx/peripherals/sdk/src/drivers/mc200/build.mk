# Copyright (C) 2008-2015, Marvell International Ltd.
# All Rights Reserved.

libs-y += libdrv

libdrv-objs-y := mdev_i2c.c mdev_pinmux.c mdev_iflash.c
libdrv-objs-y += mdev_uart.c mdev_rtc.c mdev_startup.c
libdrv-objs-y += mdev_adc.c mdev_ssp.c mdev_sdio.c
libdrv-objs-y += ../common/mdev_aes.c ../common/mdev_dac.c ../common/mdev_acomp.c
libdrv-objs-y += ../common/mdev_wdt.c ../common/mdev_crc.c ../common/mdev_gpio.c
libdrv-objs-y += ../common/mdev_gpt.c lowlevel/core_cm3.o lowlevel/mc200_aes.o
libdrv-objs-y += lowlevel/mc200_clock.o lowlevel/mc200_dma.o lowlevel/mc200_driver.o
libdrv-objs-y += lowlevel/mc200_flash.o lowlevel/mc200_gpio.o lowlevel/mc200_i2c.o
libdrv-objs-y += lowlevel/mc200_pinmux.o lowlevel/mc200_pmu.o lowlevel/mc200_qspi0.o
libdrv-objs-y += lowlevel/mc200_qspi1.o lowlevel/mc200_sdio.o lowlevel/mc200_uart.o
libdrv-objs-y += lowlevel/mc200_wdt.o lowlevel/mc200_xflash.o lowlevel/mc200_gpt.o
libdrv-objs-y += lowlevel/mc200_crc.o lowlevel/mc200_rtc.o lowlevel/mc200_acomp.o
libdrv-objs-y += lowlevel/mc200_adc.o lowlevel/mc200_ssp.o lowlevel/mc200_spi_flash.o
libdrv-objs-y += lowlevel/mc200_dac.o mdev_pm.o ../common/cyccnt.o

libdrv-objs-$(CONFIG_USB_DRIVER)      += ../common/mdev_usb.c lowlevel/mc200_usb.o
ifdef USB_HOST_PATH
 libdrv-objs-y += ../common/mdev_usb_host.c lowlevel/mc200_usb.o
endif
