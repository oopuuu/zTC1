# Copyright (C) 2008-2015, Marvell International Ltd.
# All Rights Reserved.

libs-y += libpwrmgr

libpwrmgr-objs-y                        := pwrmgr.c pwrmgr_cli.c
libpwrmgr-objs-$(CONFIG_WAKELOCK_DEBUG) += wakelock_debug.c
