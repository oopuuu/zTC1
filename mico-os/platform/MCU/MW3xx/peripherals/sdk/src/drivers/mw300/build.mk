# Copyright (C) 2008-2015, Marvell International Ltd.
# All Rights Reserved.

libs-y += libdrv

libdrv-objs-y := lowlevel/mw300_clock.o lowlevel/mw300_driver.o lowlevel/mw300_pinmux.o
libdrv-objs-y += lowlevel/mw300_gpio.o lowlevel/mw300_pmu.o lowlevel/mw300_sdio.o
libdrv-objs-y += lowlevel/mw300_flashc.c lowlevel/mw300_flash.c lowlevel/mw300_crc.o
libdrv-objs-y += lowlevel/mw300_aes.o lowlevel/mw300_uart.o lowlevel/mw300_sdio.o
libdrv-objs-y += lowlevel/mw300_ssp.o lowlevel/mw300_i2c.o lowlevel/mw300_adc.o
libdrv-objs-y += lowlevel/mw300_crc.o lowlevel/mw300_acomp.o lowlevel/mw300_dac.o
libdrv-objs-y += lowlevel/mw300_dma.o lowlevel/mw300_qspi.o lowlevel/mw300_gpt.o
libdrv-objs-y += lowlevel/mw300_wdt.o lowlevel/mw300_rtc.o lowlevel/mw300_bg.o
libdrv-objs-y += lowlevel/mw300_spi_flash.o
libdrv-objs-y += ../common/cyccnt.o ../common/mdev_wdt.c ../common/mdev_gpio.c
libdrv-objs-y += ../common/mdev_gpt.c ../common/mdev_crc.c ../common/mdev_acomp.c
libdrv-objs-y += ../common/mdev_dac.c ../common/mdev_aes.c mdev_startup.c
libdrv-objs-y += mdev_pinmux.c mdev_rtc.c mdev_pm.c mdev_sdio.c mdev_uart.c
libdrv-objs-y += mdev_ssp.c mdev_iflash.c mdev_i2c.c mdev_adc.c mdev_dma.c

libdrv-objs-$(CONFIG_USB_DRIVER)      += ../common/mdev_usb.c lowlevel/mw300_usb.o
ifdef USB_HOST_PATH
 libdrv-objs-y += ../common/mdev_usb_host.c lowlevel/mw300_usb.o
endif
