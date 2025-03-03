# Copyright (C) 2008-2015, Marvell International Ltd.
# All Rights Reserved.

libs-y += libutil
libutil-objs-y := boot_flags.c flash.c hexbin.c
libutil-objs-y += dumphex.c panic.c crc/soft_crc32.c
libutil-objs-y += crc/soft_crc16.c crc/crc32.c wm_utils.c
libutil-objs-y += wm_wlan.c block_alloc.c critical_error.c
libutil-objs-y += wmtlv.c critical_error.c work-queue.c
libutil-objs-y += system-work-queue.c base64.c arc4.c

libutil-objs-$(CONFIG_CPU_MW300)        += keystore.c
libutil-objs-$(CONFIG_BA_TESTS)         += block_alloc_test.c
libutil-objs-$(CONFIG_WORK_QUEUE_TESTS) += work-queue-tests.c
