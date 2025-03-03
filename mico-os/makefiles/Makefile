#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

default: Help

export MiCO_SDK_VERSION_MAJOR    :=  3
export MiCO_SDK_VERSION_MINOR    :=  5
export MiCO_SDK_VERSION_REVISION :=  4

export SOURCE_ROOT ?= ./

export MICO_OS_PATH := $(SOURCE_ROOT)mico-os
export MAKEFILES_PATH := $(SOURCE_ROOT)mico-os/makefiles
export SCRIPTS_PATH := $(SOURCE_ROOT)mico-os/makefiles/scripts
MAKEFILE_TARGETS := clean release  # targets used by makefile 

#define BUILD_STRING, MiCO toolchain commands on different hosts
include $(MAKEFILES_PATH)/micoder_host_cmd.mk

define USAGE_TEXT
Aborting due to invalid targets

Usage: make <target> [download] [run | debug] [JTAG=xxx] [total] [VERBOSE=1]
       make run

  <target>
    One each of the following mandatory [and optional] components separated by '@'
      * [Application] (Skip if app is under source code's root directory, or referenced by subdir.appname if apps in sub-directories)
      * Board ($(filter-out common  include README.txt,$(notdir $(wildcard $(MICO_OS_PATH)/board/*))))
      * [RTOS] ($(filter-out mico_rtos_common.c mico_rtos_common.h,$(notdir $(wildcard $(MICO_OS_PATH)/MiCO/RTOS/*))))
      * [Network Stack] ($(notdir $(wildcard $(MICO_OS_PATH)/MiCO/net/*)))
      * [TLS] ($(notdir $(wildcard $(MICO_OS_PATH)/MiCO/security/TLS/*)))
      * [debug | release_log |release] Building for debug or release configurations, or a release build with log
      
  [download]
    Download firmware image to target platform

  [run]
    Reset and run an application on the target hardware

  [debug]
    Connect to the target platform and run the debugger
    
  [total]
    Build all targets related to this application and board

  [JTAG=xxx]
    JTAG interface configuration file from the tools/OpenOCD dirctory
    Default option is jlink_swd

  [VERBOSE=1]
    Shows the commands as they are being executed

  Notes
    * Component names are case sensitive
    * 'MiCO', 'FreeRTOS', 'Lwip', 'wolfSSL'are reserved component names
    * Use 'moc' add 'mocOS', 'mocIP' and "mocTLS" to target
    * Component names MUST NOT include space or '@' characters
    * Building for '@release_log' is assumed unless '@release' or '@debug'is appended to the target

  Example Usage
    Build for Debug
      $> make application.wifi_uart@MK3165
      $> make bootloader@MK3165@NoRTOS

    Build, Download and Run using the default USB-JTAG programming interface
      $> make helloworld@MK3165 download run

    Build for Release
      $> make helloworld@MK3165@release

    Build, Download and Debug using command line GDB
      $> make helloworld@MK3165 download debug

    Reset and run an application on the target hardware
      $> make run

    Clean output directory
      $> make clean
endef

############################
# Extra options:
#                CHECK_HEADERS=1 : builds header files to test for their completeness
############################

OPENOCD_LOG_FILE ?= $(SOURCE_ROOT)build/openocd_log.txt
DOWNLOAD_LOG := >> $(OPENOCD_LOG_FILE)

BOOTLOADER_LOG_FILE ?= $(SOURCE_ROOT)build/bootloader.log
export HOST_OS
export VERBOSE
export SUB_BUILD
export OPENOCD_LOG_FILE

.PHONY: $(BUILD_STRING) main_app bootloader clean Help download total release download_dct copy_elf_for_eclipse run debug download_bootloader sflash_image .gdbinit factory_reset_dct sflash

Help: 
	$(TOOLCHAIN_HOOK_TARGETS)
	$(error $(USAGE_TEXT))

clean:
	$(QUIET)$(ECHO) Cleaning...
	$(QUIET)$(CLEAN_COMMAND)
	$(QUIET)$(RM) -rf .gdbinit
	$(QUIET)$(ECHO) Done

ifneq (,$(filter release,$(MAKECMDGOALS)))
BUILD_STRING :=
release:
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/release/release.mk $(filter-out release,$(MAKECMDGOALS))
endif

ifneq ($(BUILD_STRING),)
-include $(SOURCE_ROOT)build/$(CLEANED_BUILD_STRING)/config.mk
# Now we know the target architecture - include all toolchain makefiles and check one of them can handle the architecture
 include $(MAKEFILES_PATH)/micoder_toolchain_GCC.mk

$(SOURCE_ROOT)build/$(CLEANED_BUILD_STRING)/config.mk: $(MAKEFILES_PATH)/mico_target_config.mk $(MAKEFILES_PATH)/micoder_host_cmd.mk $(MAKEFILES_PATH)/micoder_toolchain_GCC.mk $(MiCO_SDK_MAKEFILES)
	$(QUIET)$(ECHO) $(if $(MiCO_SDK_MAKEFILES),Applying changes made to: $?,Making config file for first time)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/mico_target_config.mk $(CLEANED_BUILD_STRING)
endif


JOBS ?=4
ifeq (,$(SUB_BUILD))
JOBSNO := -j$(JOBS)
endif


PASSDOWN_TARGETS := $(strip $(filter-out $(MAKEFILE_TARGETS) $(BUILD_STRING),$(MAKECMDGOALS))) #download total
$(PASSDOWN_TARGETS):
	@:

$(BUILD_STRING): main_app $(if $(SFLASH),sflash_image) copy_elf_for_eclipse  $(if $(SUB_BUILD),,.gdbinit .openocd_cfg)

main_app: $(SOURCE_ROOT)build/$(CLEANED_BUILD_STRING)/config.mk $(MiCO_SDK_PRE_APP_BUILDS) $(MAKEFILES_PATH)/mico_target_build.mk
	$(QUIET)$(COMMON_TOOLS_PATH)mkdir -p $(OUTPUT_DIR)/binary $(OUTPUT_DIR)/modules $(OUTPUT_DIR)/libraries $(OUTPUT_DIR)/resources
	$(QUIET)$(MAKE) -r $(JOBSNO) $(SILENT) -f $(MAKEFILES_PATH)/mico_target_build.mk $(CLEANED_BUILD_STRING) $(PASSDOWN_TARGETS)
	$(QUIET)$(ECHO) Build complete

ifeq ($(SUB_BUILD),)
.gdbinit: $(SOURCE_ROOT)build/$(CLEANED_BUILD_STRING)/config.mk $(MAKEFILES_PATH)/micoder_host_cmd.mk main_app
	$(QUIET)$(ECHO) Making $@
	$(QUIET)$(ECHO) set remotetimeout 20 > $@
	$(QUIET)$(ECHO) $(GDBINIT_STRING) >> $@
	
.openocd_cfg: .gdbinit
	$(QUIET)$(ECHO) Making $@
	$(QUIET)$(ECHO) source [find $(OPENOCD_PATH)$(JTAG).cfg] > $@
	$(QUIET)$(ECHO) source [find $(OPENOCD_PATH)$(HOST_OPENOCD).cfg] >> $@
	$(QUIET)$(ECHO) source [find $(OPENOCD_PATH)$(HOST_OPENOCD)_gdb_jtag.cfg] >> $@
	
endif

ifneq ($(SFLASH),)
sflash_image: main_app
	$(QUIET)$(ECHO) Building Serial Flash Image
	$(QUIET)$(MAKE) $(SILENT) -f $(MAKEFILES_PATH)/mfg_image.mk $(SFLASH) FRAPP=$(CLEANED_BUILD_STRING) SFLASH=
endif


sflash: main_app
	$(QUIET)$(ECHO) Building Serial Flash Image $@
	$(QUIET)$(MAKE) $(SILENT) -f $(MAKEFILES_PATH)/mico_sflash.mk FRAPP=$(CLEANED_BUILD_STRING) $@

sflash_download: main_app sflash
	$(QUIET)$(ECHO) Downloading Serial Flash Image $@
	$(QUIET)$(MAKE) $(SILENT) -f $(MAKEFILES_PATH)/mico_sflash.mk FRAPP=$(CLEANED_BUILD_STRING) $@

