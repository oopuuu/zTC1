#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := App_SFlash_write

include $(MAKEFILES_PATH)/mico_partition.mk

$(NAME)_SOURCES  := sflash_write.c

ifeq ($(TOOLCHAIN_NAME),IAR)
NoRTOS_START_STACK := 10000
else
NoRTOS_START_STACK := 5024
endif

# This uses cflags instead of the normal includes to avoid being
# relative to the directory of this module
#$(NAME)_CFLAGS += -I$(SPI_FLASH_IMAGE_DIR)

#$(NAME)_DEFINES += FACTORY_RESET_AFTER_SFLASH

# blocking printf so breakpoint calls still print information
#GLOBAL_DEFINES += PRINTF_BLOCKING

#NoOS_START_STACK := 6000

#GLOBAL_LINK_SCRIPT := mfg_spi_flash_link.ld

NO_WIFI_FIRMWARE := YES
NO_WIFI          := YES

GLOBAL_DEFINES += MICO_NO_WIFI
GLOBAL_DEFINES += NO_WIFI_FIRMWARE
GLOBAL_DEFINES += FIRMWARE_DOWNLOAD

ifeq ($(WIPE),1)
GLOBAL_DEFINES += WIPE_SFLASH
endif

GLOBAL_LDFLAGS   += $$(CLIB_LDFLAGS_NANO)

VALID_OSNS_COMBOS := NoRTOS