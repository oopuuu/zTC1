#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

.PHONY: total download kill_openocd

EXTRA_POST_BUILD_TARGETS += copy_output_for_eclipse

include $(MICO_OS_PATH)/platform/MCU/$(HOST_MCU_FAMILY)/merge/gen_moc_images.mk

ifeq (download,$(findstring download,$(MAKECMDGOALS)))
EXTRA_POST_BUILD_TARGETS  += sflash_write_app
OPENOCD_LOG_FILE ?= $(SOURCE_ROOT)build/openocd_log.txt
DOWNLOAD_LOG := >> $(OPENOCD_LOG_FILE)
endif


ifeq (,$(and $(OPENOCD_PATH),$(OPENOCD_FULL_NAME)))
	$(error Path to OpenOCD has not been set using OPENOCD_PATH and OPENOCD_FULL_NAME)
endif


ifneq (total,$(findstring total,$(MAKECMDGOALS)))
download_app: gen_moc_images sflash_write_app kill_openocd
	$(eval MOC_APP_OUTPUT_FILE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(MOC_APP_BIN_OUTPUT_FILE)))
	$(QUIET)$(ECHO) Downloading MOC APP to partition: $(APPLICATION_FIRMWARE_PARTITION_TCL) offset: $(MOC_APP_OFFSET) size: $(MOC_APP_OUTPUT_FILE_SIZE) bytes...
	$(call CONV_SLASHES, $(OPENOCD_FULL_NAME)) -s $(SOURCE_ROOT) -f $(OPENOCD_CFG_PATH)interface/$(JTAG).cfg -f $(OPENOCD_CFG_PATH)$(HOST_OPENOCD)/$(HOST_OPENOCD).cfg -f $(MICO_OS_PATH)/sub_build/spi_flash_write/sflash_write.tcl -c "sflash_write_file $(MOC_APP_BIN_OUTPUT_FILE) $(APPLICATION_FIRMWARE_PARTITION_TCL) $(MOC_APP_OFFSET) $(SFLASH_APP_PLATFROM_BUS) 0" -c shutdown $(DOWNLOAD_LOG) 2>&1 && $(ECHO) Download complete && $(ECHO_BLANK_LINE) || $(ECHO) Download failed
else
download_app: gen_moc_images sflash_write_app kill_openocd
	$(eval MOC_BOOT_OUTPUT_FILE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(MOC_BOOT_BIN_FILE)))
	$(QUIET)$(ECHO) Downloading MOC BOOT to partition: $(BOOTLOADER_FIRMWARE_PARTITION_TCL) size: $(MOC_BOOT_OUTPUT_FILE_SIZE) bytes... 
	$(call CONV_SLASHES, $(OPENOCD_FULL_NAME)) -s $(SOURCE_ROOT) -f $(OPENOCD_CFG_PATH)interface/$(JTAG).cfg -f $(OPENOCD_CFG_PATH)$(HOST_OPENOCD)/$(HOST_OPENOCD).cfg -f $(MICO_OS_PATH)/sub_build/spi_flash_write/sflash_write.tcl -c "sflash_write_file $(MOC_BOOT_BIN_FILE) $(BOOTLOADER_FIRMWARE_PARTITION_TCL) 0x0 $(SFLASH_APP_PLATFROM_BUS) 0" -c shutdown $(DOWNLOAD_LOG) 2>&1 && $(ECHO) Download complete && $(ECHO_BLANK_LINE) || $(ECHO) Download failed
	
	$(eval MOC_KERNEL_APP_OUTPUT_FILE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(MOC_KERNEL_APP_BIN_OUTPUT_FILE)))
	$(QUIET)$(ECHO) Downloading MOC kernel and app to partition: $(APPLICATION_FIRMWARE_PARTITION_TCL) size: $(MOC_KERNEL_APP_OUTPUT_FILE_SIZE) bytes... 
	$(call CONV_SLASHES, $(OPENOCD_FULL_NAME)) -s $(SOURCE_ROOT) -f $(OPENOCD_CFG_PATH)interface/$(JTAG).cfg -f $(OPENOCD_CFG_PATH)$(HOST_OPENOCD)/$(HOST_OPENOCD).cfg -f $(MICO_OS_PATH)/sub_build/spi_flash_write/sflash_write.tcl -c "sflash_write_file $(MOC_KERNEL_APP_BIN_OUTPUT_FILE) $(APPLICATION_FIRMWARE_PARTITION_TCL) 0x0 $(SFLASH_APP_PLATFROM_BUS) 0" -c shutdown $(DOWNLOAD_LOG) 2>&1 && $(ECHO) Download complete && $(ECHO_BLANK_LINE) || $(ECHO) Download failed

	$(eval MOC_ATE_OUTPUT_FILE_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $(MOC_ATE_BIN_OUTPUT_FILE)))
	$(QUIET)$(ECHO) Downloading MOC ATE to partition: $(ATE_FIRMWARE_PARTITION_TCL) size: $(MOC_ATE_OUTPUT_FILE_SIZE) bytes... 
	$(call CONV_SLASHES, $(OPENOCD_FULL_NAME)) -s $(SOURCE_ROOT) -f $(OPENOCD_CFG_PATH)interface/$(JTAG).cfg -f $(OPENOCD_CFG_PATH)$(HOST_OPENOCD)/$(HOST_OPENOCD).cfg -f $(MICO_OS_PATH)/sub_build/spi_flash_write/sflash_write.tcl -c "sflash_write_file $(MOC_ATE_BIN_OUTPUT_FILE) $(ATE_FIRMWARE_PARTITION_TCL) 0x0 $(SFLASH_APP_PLATFROM_BUS) 0" -c shutdown $(DOWNLOAD_LOG) 2>&1 && $(ECHO) Download complete && $(ECHO_BLANK_LINE) || $(ECHO) Download failed
endif

copy_output_for_eclipse: build_done
	$(QUIET)$(call MKDIR, $(BUILD_DIR)/eclipse_debug/)
	$(QUIET)$(CP) $(LINK_OUTPUT_FILE) $(BUILD_DIR)/eclipse_debug/last_built.elf


ifeq (download,$(filter download,$(MAKECMDGOALS)))
EXT_IMAGES_DOWNLOAD_DEP := download_app
endif

download: download_app $(if $(findstring total,$(MAKECMDGOALS)), EXT_IMAGE_DOWNLOAD,)

kill_openocd:
	$(KILL_OPENOCD)
	
$(if $(RTOS),,$(error No RTOS specified. Options are: $(notdir $(wildcard MiCO/RTOS/*))))

run: $(SHOULD_I_WAIT_FOR_DOWNLOAD)
	$(QUIET)$(ECHO) Resetting target
	$(QUIET)$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -c "log_output $(OPENOCD_LOG_FILE)" -s $(SOURCE_ROOT) -f $(OPENOCD_CFG_PATH)interface/$(JTAG).cfg -f $(OPENOCD_CFG_PATH)$(HOST_OPENOCD)/$(HOST_OPENOCD).cfg -c init -c "reset run" -c shutdown $(DOWNLOAD_LOG) 2>&1 && $(ECHO) Target running


debug: $(BUILD_STRING) $(SHOULD_I_WAIT_FOR_DOWNLOAD)
	$(QUIET)$(GDB_COMMAND) $(LINK_OUTPUT_FILE) -x .gdbinit_attach



