#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

SFLASH_LOG_FILE      ?= $(BUILD_DIR)/sflash_writer.log
SFLASH_REDIRECT	= > $(SFLASH_LOG_FILE)

SFLASH_APP_TARGET := sub_build.spi_flash_write@NoRTOS@$(PLATFORM)
SFLASH_APP_PLATFROM_BUS := $(PLATFORM)
SFLASH_PREBUILD_APP := $(if $(wildcard $(MICO_OS_PATH)/board/$(PLATFORM)),$(MICO_OS_PATH)/board/$(PLATFORM),$(if $(wildcard $(SOURCE_ROOT)/board/$(PLATFORM)),$(SOURCE_ROOT)/board/$(PLATFORM),))/flash_prog.elf

FILE_BIN_SCRIPT:= $(MAKEFILES_PATH)/scripts/flash_pack.py
FILE_BIN_NUM:= 001
FILES_BIN_NAME:= filesystem

# If do not exist resources folder, error
ifeq ($(APP_FULL), bootloader)
APP_FILE_RESOURCE :=
else
ifeq ($(APP_FULL), sub_build/spi_flash_write)
APP_FILE_RESOURCE :=
else
APP_FILE_RESOURCE := $(APP_FULL)/resources
endif
endif
ifeq ($(APP_FILE_RESOURCE),)
else
ifeq ($(APP_FILE_RESOURCE), $(wildcard $(APP_FILE_RESOURCE)))
SFLASH_GEN_FTFS_BIN:= build/$(APP_FULL)@$(PLATFORM)/resources/filesystem.bin
else
SFLASH_GEN_FTFS_BIN:= 
#$(warning Do not exist filesystem folder...)
endif
endif

clean:
	$(QUIET)$(RM) -rf $(SFLASH_PREBUILD_APP)

# If Downloading is required, then the Serial Flash app need to be built
$(SFLASH_PREBUILD_APP):
	$(QUIET)$(ECHO) Building Flash Loader App...
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)mico-os/makefiles/Makefile $(SFLASH_APP_TARGET)  SFLASH= EXTERNAL_MiCO_GLOBAL_DEFINES=$(EXTERNAL_MiCO_GLOBAL_DEFINES) SUB_BUILD=sflash_app $(SFLASH_REDIRECT)
	$(QUIET)$(CP) -f $(BUILD_DIR)/$(SFLASH_APP_TARGET)/binary/$(SFLASH_APP_TARGET).elf   $(SFLASH_PREBUILD_APP)
	$(QUIET)$(ECHO) Finished Building Flash Loader App
	$(QUIET)$(ECHO_BLANK_LINE)

$(SFLASH_GEN_FTFS_BIN):
	$(QUIET)$(ECHO) Generating Filesystem Image...
	$(QUIET)$(shell $(PYTHON) $(FILE_BIN_SCRIPT) $(FILE_BIN_NUM) $(FILES_BIN_NAME).bin $(APP_FILE_RESOURCE))
	$(QUIET)$(MV) $(SOURCE_ROOT)$(FILES_BIN_NAME).bin $(SOURCE_ROOT)build/$(CLEANED_BUILD_STRING)/resources
	$(QUIET)$(ECHO) Finished Generating Filesystem Image
	$(QUIET)$(ECHO_BLANK_LINE)


sflash_write_app: $(SFLASH_PREBUILD_APP)
sflash_gen_filesystem: $(SFLASH_GEN_FTFS_BIN)



