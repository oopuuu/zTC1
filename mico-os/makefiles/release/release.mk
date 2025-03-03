#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

.PHONY: total A B C ABC release

include $(MAKEFILES_PATH)/micoder_host_cmd.mk

ifeq ($(HOST_OS),Win32)
$(error This script can only run on Linux or macOS!)
endif

JOBSNO := -j1

MiCO_SDK_VERSION ?= $(MiCO_SDK_VERSION_MAJOR).$(MiCO_SDK_VERSION_MINOR).$(MiCO_SDK_VERSION_REVISION)

SDK_TYPE_LIST := A B C ABC all
SDK_TYPE := $(filter $(SDK_TYPE_LIST), $(MAKECMDGOALS))

ifneq ($(SDK_TYPE),)
SDK_OUTPUT_FOLDER := MiCO_$(SDK_TYPE)_v$(MiCO_SDK_VERSION)
SDK_OUTPUT_DIR := $(SOURCE_ROOT)$(SDK_OUTPUT_FOLDER)/
SDK_OUTPUT_OSX := $(SOURCE_ROOT)../MiCO_$(SDK_TYPE)_v$(MiCO_SDK_VERSION).macOS.tar.gz
SDK_OUTPUT_Linux64 := $(SOURCE_ROOT)../MiCO_$(SDK_TYPE)_v$(MiCO_SDK_VERSION).Linux64.tar.gz
SDK_OUTPUT_WIN := $(SOURCE_ROOT)../MiCO_$(SDK_TYPE)_v$(MiCO_SDK_VERSION).Win32.zip

ECLIPSE_PROJECT_FILE := $(SDK_OUTPUT_DIR)/.project
comma:= ,


$(info Releasing MiCO SDK v$(MiCO_SDK_VERSION) for $(SDK_TYPE)!)
else
$(warning Release SDK type not defined!)
endif

BASIC_FILES_WITH_MOCODER: BASIC_FILES
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)MiCoder 	$(SDK_OUTPUT_DIR)MiCoder)
	

BASIC_FILES:	
	$(QUIET)$(ECHO) Erasing old files...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_OSX)
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_WIN)
	
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR))
	
	$(QUIET)$(ECHO) Copying source codes...
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)bootloader 	$(SDK_OUTPUT_DIR)bootloader)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)demos 		$(SDK_OUTPUT_DIR)demos)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)document 	$(SDK_OUTPUT_DIR)document)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)include 	$(SDK_OUTPUT_DIR)include)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)libraries 	$(SDK_OUTPUT_DIR)libraries)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)makefiles 	$(SDK_OUTPUT_DIR)makefiles)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)MiCO 		$(SDK_OUTPUT_DIR)MiCO)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)platform 	$(SDK_OUTPUT_DIR)platform)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)resources 	$(SDK_OUTPUT_DIR)resources)
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)Doxygen 		$(SDK_OUTPUT_DIR)
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)make 			$(SDK_OUTPUT_DIR)
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)make.exe 		$(SDK_OUTPUT_DIR)
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)Makefile 		$(SDK_OUTPUT_DIR)
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)README.md 		$(SDK_OUTPUT_DIR)
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)CHANGELOG.txt 	$(SDK_OUTPUT_DIR)
	
	$(QUIET)$(ECHO) Generate eclipse project file...
	$(QUIET)$(call WRITE_FILE_CREATE, $(ECLIPSE_PROJECT_FILE) ,<?xml version="1.0" encoding="UTF-8"?>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,<projectDescription>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<name>MiCO_$(SDK_TYPE)_v$(MiCO_SDK_VERSION)</name>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<comment></comment>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<projects>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	</projects>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<buildSpec>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<buildCommand>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<name>org.eclipse.cdt.managedbuilder.core.genmakebuilder</name>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<triggers>clean$(comma)full$(comma)incremental$(comma)</triggers>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<arguments>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			</arguments>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		</buildCommand>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<buildCommand>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<name>org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder</name>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<triggers>full$(comma)incremental$(comma)</triggers>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<arguments>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			</arguments>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		</buildCommand>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	</buildSpec>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<natures>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<nature>org.eclipse.cdt.core.cnature</nature>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<nature>org.eclipse.cdt.core.ccnature</nature>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<nature>org.eclipse.cdt.managedbuilder.core.ScannerConfigNature</nature>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	</natures>)
	$(QUIET)$(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,</projectDescription>)
	

	$(QUIET)$(ECHO) Remove libraries...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/MiCO/core/*.a
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/MiCO/core/rf_driver/MARVELL_sd8801_P34.bin
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/MiCO/core/rf_driver/MARVELL_sd8801_P70.bin

	$(QUIET)$(ECHO) Remove all MCU libraries...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)platform/MCU
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)platform/MCU)
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)platform/MCU/mico_platform_common.c $(SDK_OUTPUT_DIR)platform/MCU/
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)platform/MCU/wlan_platform_common.c $(SDK_OUTPUT_DIR)platform/MCU/
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)platform/MCU/moc_platform_common.c  $(SDK_OUTPUT_DIR)platform/MCU/
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)platform/MCU/platform_nsclock.c     $(SDK_OUTPUT_DIR)platform/MCU/

	$(QUIET)$(ECHO) Remove BTE source folder...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/BTE/Components
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/BTE/mico
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/BTE/mico_bt_api
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/BTE/Projects
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/BTE/proto_disp
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/dual_mode/dual_mode_src.mk
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/dual_mode/Makefile
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/low_energy/low_energy_src.mk
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/low_energy/Makefile
	
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/libraries/bluetooth/BTE_MTK
	
	$(QUIET)$(ECHO) Remove SSL source folder...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)MiCO/security/TLS/wolfSSL/wolfssl
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)MiCO/security/TLS/wolfSSL/mico

	$(QUIET)$(ECHO) Remove libwebsocket...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)libraries/protocols/libwebsocket/src
	
	$(QUIET)$(ECHO) Remove AT application...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)demos/application/at
	
	$(QUIET)$(ECHO) Remove HomeKit application...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)demos/application/homekit
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)libraries/daemons/homekit_server/src
	
	$(QUIET)$(ECHO) Remove HomeKit security source...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)MiCO/security/Sodium/src
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)MiCO/security/SRP_6a/src
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)MiCO/system/easylink/MFi_WAC/src

	$(QUIET)$(ECHO) Remove MFG application...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/demos/test/bt_mfg_test
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/demos/test/iperf_power
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/demos/test/auto_self_test
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/demos/test/hardware_test_3080

	$(QUIET)$(ECHO) Remove Alink application and libraries...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/demos/application/alink
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)libraries/protocols/alink

	$(QUIET)$(ECHO) Remove unused documents...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)document/EMW3162
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)document/MICO_DEV_1


A_FILES: $(if $(findstring total,$(MAKECMDGOALS)), BASIC_FILES_WITH_MOCODER, BASIC_FILES) 
	$(QUIET)$(ECHO) Copying IAR projects...
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)Projects)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)Projects/STM32F2xx 	$(SDK_OUTPUT_DIR)Projects/STM32F2xx)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)Projects/STM32F4xx 	$(SDK_OUTPUT_DIR)Projects/STM32F4xx)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)Projects/ATSAMG55 	$(SDK_OUTPUT_DIR)Projects/ATSAMG55)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)Projects/LPC5410x 	$(SDK_OUTPUT_DIR)Projects/LPC5410x)
	
	$(QUIET)$(ECHO) Copying MiCO core libraries...
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCO/core/*.1062.*.a $(SDK_OUTPUT_DIR)MiCO/core/
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCO/core/*.3165.*.a $(SDK_OUTPUT_DIR)MiCO/core/
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCO/core/*.3166.*.a $(SDK_OUTPUT_DIR)MiCO/core/
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCO/core/*.3238.*.a $(SDK_OUTPUT_DIR)MiCO/core/
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCO/core/*.3239.*.a $(SDK_OUTPUT_DIR)MiCO/core/
	
	$(QUIET)$(ECHO) Copying board configurations...
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)board)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3162 		$(SDK_OUTPUT_DIR)board/MK3162)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3165 		$(SDK_OUTPUT_DIR)board/MK3165)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3166 		$(SDK_OUTPUT_DIR)board/MK3166)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3238 		$(SDK_OUTPUT_DIR)board/MK3238)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3239 		$(SDK_OUTPUT_DIR)board/MK3239)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MKF205 		$(SDK_OUTPUT_DIR)board/MKF205)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MKG55 		$(SDK_OUTPUT_DIR)board/MKG55)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MKLPC5410x 	$(SDK_OUTPUT_DIR)board/MKLPC5410x)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/NUCLEO_F411RE $(SDK_OUTPUT_DIR)board/NUCLEO_F411RE)
	
	$(QUIET)$(ECHO) Copying MCU libraries...
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)platform/MCU)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)platform/MCU/STM32F2xx 		$(SDK_OUTPUT_DIR)platform/MCU/STM32F2xx)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)platform/MCU/STM32F4xx 		$(SDK_OUTPUT_DIR)platform/MCU/STM32F4xx)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)platform/MCU/LPC5410x 		$(SDK_OUTPUT_DIR)platform/MCU/LPC5410x)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)platform/MCU/ATSAMG55 		$(SDK_OUTPUT_DIR)platform/MCU/ATSAMG55)
	
A: A_FILES
	# Gen OSX SDK
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCoder/eclipse_project/OSX/.cproject $(SDK_OUTPUT_DIR)
	tar -zcvf $(SDK_OUTPUT_OSX) --exclude=MiCoder/compiler/*/Linux* --exclude=MiCoder/compiler/*/Win32 --exclude=*.DS_Store -C $(SDK_OUTPUT_DIR) ./
	
	# Gen Linux64 SDK
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCoder/eclipse_project/Linux64/.cproject $(SDK_OUTPUT_DIR)
	tar -zcvf $(SDK_OUTPUT_Linux64) --exclude=MiCoder/compiler/*/Linux32 --exclude=MiCoder/compiler/*/OSX --exclude=MiCoder/compiler/*/Win32 --exclude=*.DS_Store -C $(SDK_OUTPUT_DIR) ./
	
	# Gen Win32 SDK
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCoder/eclipse_project/Win32/.cproject $(SDK_OUTPUT_DIR)
	zip -r $(SDK_OUTPUT_WIN) $(SDK_OUTPUT_DIR) -x /$(SDK_OUTPUT_FOLDER)/MiCoder/compiler/*/Linux*/* /$(SDK_OUTPUT_FOLDER)/MiCoder/compiler/*/OSX/* *.DS_Store
	
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)
	$(QUIET)$(ECHO) Done
	
B_FILES: $(if $(findstring total,$(MAKECMDGOALS)), BASIC_FILES_WITH_MOCODER, BASIC_FILES)
	$(QUIET)$(ECHO) Copying IAR projects...
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)Projects)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)Projects/MW3xx 	$(SDK_OUTPUT_DIR)Projects/MW3xx)
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)Projects/MW3xx/demo/EWARM/MK3031
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)Projects/MW3xx/demo/EWARM/EMW5031
	
	$(QUIET)$(ECHO) Copying MiCO core libraries...
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCO/core/*.1088.*.a $(SDK_OUTPUT_DIR)MiCO/core/
	
	$(QUIET)$(ECHO) Copying board configurations...
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)board)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3031 		$(SDK_OUTPUT_DIR)board/MK3031)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/EMW5031 		$(SDK_OUTPUT_DIR)board/EMW5031)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3288 		$(SDK_OUTPUT_DIR)board/MK3288)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3080A 		$(SDK_OUTPUT_DIR)board/MK3080A)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3080B 		$(SDK_OUTPUT_DIR)board/MK3080B)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/MK3080C 		$(SDK_OUTPUT_DIR)board/MK3080C)
	
	$(QUIET)$(ECHO) Copying MCU libraries...
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)platform/MCU)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)platform/MCU/MW3xx 		$(SDK_OUTPUT_DIR)platform/MCU/MW3xx)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)platform/MCU/MX1290 	$(SDK_OUTPUT_DIR)platform/MCU/MX1290)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)platform/MCU/STM32F4xx	$(SDK_OUTPUT_DIR)platform/MCU/STM32F4xx)
		
	$(QUIET)$(ECHO) Remove MCU source codes...
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/platform/MCU/MW3xx/peripherals/boot2
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/platform/MCU/MW3xx/peripherals/sdk/src/boards
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/platform/MCU/MW3xx/peripherals/sdk/src/core
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/platform/MCU/MW3xx/peripherals/sdk/src/drivers
	
B: B_FILES
	# Gen OSX SDK
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCoder/eclipse_project/OSX/.cproject $(SDK_OUTPUT_DIR)
	tar -zcvf $(SDK_OUTPUT_OSX) --exclude=MiCoder/compiler/*/Linux* --exclude=MiCoder/compiler/*/Win32 --exclude=*.DS_Store -C $(SDK_OUTPUT_DIR) ./
	
	# Gen Linux64 SDK
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCoder/eclipse_project/Linux64/.cproject $(SDK_OUTPUT_DIR)
	tar -zcvf $(SDK_OUTPUT_Linux64) --exclude=MiCoder/compiler/*/Linux32 --exclude=MiCoder/compiler/*/OSX --exclude=MiCoder/compiler/*/Win32 --exclude=*.DS_Store -C $(SDK_OUTPUT_DIR) ./
	
	# Gen Win32 SDK
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCoder/eclipse_project/Win32/.cproject $(SDK_OUTPUT_DIR)
	zip -r $(SDK_OUTPUT_WIN) $(SDK_OUTPUT_DIR) -x /$(SDK_OUTPUT_FOLDER)/MiCoder/compiler/*/Linux*/* /$(SDK_OUTPUT_FOLDER)/MiCoder/compiler/*/OSX/* *.DS_Store
	
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)
	$(QUIET)$(ECHO) Done
	

C_FILES: BASIC_FILES
	$(QUIET)$(ECHO) Copying IAR projects...
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)Projects)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)Projects/RTL8711 	$(SDK_OUTPUT_DIR)Projects/RTL8711)
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/Projects/RTL8711/demo/EWARM/MiCOKit-3081
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)/Projects/RTL8711/bootloader/EWARM/MiCOKit-3081
	
	$(QUIET)$(ECHO) Copying MiCO core libraries...
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCO/core/*.3081.*.a $(SDK_OUTPUT_DIR)MiCO/core/
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCO/core/*.3081A.*.a $(SDK_OUTPUT_DIR)MiCO/core/
	
	$(QUIET)$(ECHO) Copying board configurations...
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)board)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/EMW3081 		$(SDK_OUTPUT_DIR)board/EMW3081)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/EMW3081A 		$(SDK_OUTPUT_DIR)board/EMW3081A)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)board/EMW3081B 		$(SDK_OUTPUT_DIR)board/EMW3081B)
	
	$(QUIET)$(ECHO) Copying MCU libraries...
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)Projects/component 	$(SDK_OUTPUT_DIR)Projects/component)
	$(QUIET)$(call MKDIR, $(SDK_OUTPUT_DIR)platform/MCU)
	$(QUIET)$(call CPDIR, $(SOURCE_ROOT)platform/MCU/RTL8711 		$(SDK_OUTPUT_DIR)platform/MCU/RTL8711)
	
C: C_FILES
	zip -r $(SDK_OUTPUT_WIN) $(SDK_OUTPUT_DIR) -x *.DS_Store
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)
	
	$(QUIET)$(ECHO) Done
	
ABC: A_FILES B_FILES C_FILES
	# Gen OSX SDK
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCoder/eclipse_project/OSX/.cproject $(SDK_OUTPUT_DIR)
	tar -zcvf $(SDK_OUTPUT_OSX) --exclude=MiCoder/compiler/*/Linux* --exclude=MiCoder/compiler/*/Win32 --exclude=*.DS_Store -C $(SDK_OUTPUT_DIR) ./
	
	# Gen Linux64 SDK
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCoder/eclipse_project/Linux64/.cproject $(SDK_OUTPUT_DIR)
	tar -zcvf $(SDK_OUTPUT_Linux64) --exclude=MiCoder/compiler/*/Linux32 --exclude=MiCoder/compiler/*/OSX --exclude=MiCoder/compiler/*/Win32 --exclude=*.DS_Store -C $(SDK_OUTPUT_DIR) ./
	
	# Gen Win32 SDK
	$(QUIET)$(CP) -f  $(SOURCE_ROOT)MiCoder/eclipse_project/Win32/.cproject $(SDK_OUTPUT_DIR)
	zip -r $(SDK_OUTPUT_WIN) $(SDK_OUTPUT_DIR) -x /$(SDK_OUTPUT_FOLDER)/MiCoder/compiler/*/Linux*/* /$(SDK_OUTPUT_FOLDER)/MiCoder/compiler/*/OSX/* *.DS_Store
	
	$(QUIET)$(RM) -rf $(SDK_OUTPUT_DIR)
	$(QUIET)$(ECHO) Done
	
ifneq (total,$(findstring total,$(MAKECMDGOALS)))
all: 
	$(QUIET)$(ECHO) Generate all SDKs
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile release A
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile release B
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile release C
else
all: 
	$(QUIET)$(ECHO) Generate all SDKs with toolchain
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile release A total
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile release B total
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile release C total
endif



	