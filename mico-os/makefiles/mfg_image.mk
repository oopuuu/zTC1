#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

include $(MAKEFILES_PATH)/micoder_host_cmd.mk

include $(BUILD_DIR)/$(FRAPP)/config.mk

COMPONENTS := $(subst -, ,$(MAKECMDGOALS))
USE_APP       := $(if $(findstring app,$(COMPONENTS)),1)
USE_OTA       := $(if $(findstring ota,$(COMPONENTS)),1)
USE_DCT       := $(if $(findstring dct,$(COMPONENTS)),1)
DO_DOWNLOAD   := $(if $(findstring download,$(COMPONENTS)),1)

DCT ?=$(BUILD_DIR)/$(FRAPP)/DCT.stripped.elf

ifeq ($(wildcard $(DCT)),)
$(error Could not find specified DCT file $(DCT))
endif

ifeq ($(strip $(USE_APP)$(USE_OTA)$(USE_DCT)),)
$(error Specify one or more of: app (Factory Reset App image), dct (Device Config Table), ota (Over The Air Upgrade App Image))
endif

ifneq ($(strip $(USE_OTA)),)
ifeq ($(strip $(OTA)),)
$(error Specify app-rtos-networkstack target for OTA app with OTA=   e.g. OTA=ota_upgrade-ThreadX-NetX_Duo)
endif
endif

APP_COMPONENTS := $(subst -, ,$(FRAPP))
APP_BUS        := $(if $(findstring SDIO,$(APP_COMPONENTS)),SDIO,$(findstring SPI,$(APP_COMPONENTS)))
APP_PLATFORM   := $(notdir $(strip $(foreach comp,$(APP_COMPONENTS),$(wildcard $(SOURCE_ROOT)MiCO/platform/$(comp)))))

OTA_TGT:=$(OTA)-$(PLATFORM)-$(BUS)
SFLASHWRITER_TGT:=waf.sflash_write-NoOS-$(PLATFORM)-$(BUS)

OTA_TGT_DIR:=$(subst .,_,$(OTA_TGT))
SFLASHWRITER_TGT_DIR:=$(subst .,_,$(SFLASHWRITER_TGT))

ifneq ($(DO_DOWNLOAD),)
-include $(SOURCE_ROOT)build/$(SFLASHWRITER_TGT_DIR)/config.mk
endif

OUTPUT_DIR :=$(SOURCE_ROOT)build/$(FRAPP)/
OUTPUT_DIR_CONVD := $(call CONV_SLASHES,$(OUTPUT_DIR))


.PHONY: ota_upgrade_app concated_sflash_image

$(MAKECMDGOALS): concated_sflash_image  $(if $(DO_DOWNLOAD),download_sflash)

ota_upgrade_app:
	$(QUIET)$(ECHO) Building the OTA-Upgrade App $(OTA_TGT)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(MAKE) $(SILENT) -f $(SOURCE_ROOT)mico-os/makefiles/Makefile -s $(OTA_TGT) NO_BUILD_BOOTLOADER=1
	$(QUIET)$(ECHO_BLANK_LINE)


concated_sflash_image: $(if $(USE_OTA),ota_upgrade_app) pad_dct
	$(QUIET)$(ECHO) Concatenating the binaries into an image for the serial-flash chip
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(BUILD_DIR)/$(FRAPP)/binary/$(FRAPP).stripped.elf) > $(OUTPUT_DIR_CONVD)sflash.bin
ifneq ($(strip $(USE_DCT)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(DCT)) >> $(OUTPUT_DIR_CONVD)sflash.bin
else
	$(QUIET)$(PERL) -e "$(PERL_ESC_DOLLAR)x=-s'$(DCT)'; print \"\xff\" x $(PERL_ESC_DOLLAR)x;" > $(call CONV_SLASHES,$(SOURCE_ROOT)build/$(FRAPP)/blankDCT.bin)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(SOURCE_ROOT)build/$(FRAPP)/blankDCT.bin) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif
ifneq ($(strip $(USE_OTA)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(SOURCE_ROOT)build/$(OTA_TGT_DIR)/binary/$(OTA_TGT_DIR).stripped.elf) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif
	$(QUIET)$(ECHO_BLANK_LINE)


sflash_writer_app:
	$(QUIET)$(ECHO) Building the Serial Flash Writer App
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(MAKE) $(SILENT) -f $(SOURCE_ROOT)mico-os/makefiles/Makefile -s $(SFLASHWRITER_TGT) NO_BUILD_BOOTLOADER=1
	$(QUIET)$(ECHO) Done
	$(QUIET)$(ECHO_BLANK_LINE)

$(SOURCE_ROOT)build/$(SFLASHWRITER_TGT_DIR)/config.mk: sflash_writer_app

download_sflash: sflash_writer_app
	$(QUIET)$(ECHO) Downloading Serial Flash image
	$(QUIET)$(ECHO_BLANK_LINE)
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -s $(SOURCE_ROOT) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(OUTPUT_DIR)sflash.bin 0x0 $(PLATFORM)-$(BUS) 1 0" -c shutdown
	$(QUIET)$(ECHO_BLANK_LINE)

pad_dct:
	$(QUIET)$(PERL) $(TOOLS_ROOT)/create_dct/pad_dct.pl $(DCT)
