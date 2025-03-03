#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Lib_Fatfs

FATFS_VERSION := ver0_11

GLOBAL_INCLUDES := $(FATFS_VERSION)/src \
                   $(FATFS_VERSION)/src/option

$(NAME)_SOURCES := fatfs_to_block_device_driver.c \
				   fatfs_user_api_driver.c \
				   $(FATFS_VERSION)/src/ff.c \
				   driver/flash_driver.c

				  
GLOBAL_DEFINES := USING_FATFS

$(NAME)_COMPONENTS := filesystem.FatFs.$(FATFS_VERSION).src.option



