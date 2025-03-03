# Copyright (C) 2008-2015, Marvell International Ltd.
# All Rights Reserved.

libs-y += libwmtime
libwmtime-objs-y := wmtime.c wmtime_cli.c

ifeq (y,$(CONFIG_HW_RTC))
  libwmtime-objs-y += hwrtc.c
else
  libwmtime-objs-y += swrtc.c
endif
