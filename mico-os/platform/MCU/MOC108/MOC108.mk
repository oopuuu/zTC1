#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME = MOC108

# Host architecture is ARM968E-S
HOST_ARCH := ARM968E-S

# Host MCU alias for OpenOCD
HOST_OPENOCD := MOC108

GLOBAL_INCLUDES := . \
                   .. \
                   ../.. \
                   ../../include \
                   ../../$(TOOLCHAIN_NAME) \
                   ../../$(HOST_ARCH) \
                   ../../$(HOST_ARCH)/CMSIS \
                   peripherals

# Global flags
GLOBAL_CFLAGS   += $$(CPU_CFLAGS) -fno-builtin-printf -Wno-implicit-function-declaration -Wno-int-conversion -Wno-unused-variable -Wno-unused-function
GLOBAL_CXXFLAGS += $$(CPU_CXXFLAGS)
GLOBAL_ASMFLAGS += $$(CPU_ASMFLAGS)
GLOBAL_LDFLAGS  += $$(CPU_LDFLAGS)

GLOBAL_DEFINES += CONFIG_MX108
GLOBAL_DEFINES += configUSE_WATCHDOG_TICK=32000
GLOBAL_DEFINES += configTOTAL_HEAP_SIZE=102400
GLOBAL_DEFINES += configTIMER_TASK_STACK_DEPTH=750
GLOBAL_DEFINES += configUSE_TICKLESS_IDLE=0
GLOBAL_DEFINES += CONFIG_USE_LINKER_HEAP=1

ifeq ($(filter $(RTOS),NoRTOS),)
GLOBAL_LDFLAGS += -Wl,-wrap,_malloc_r -Wl,-wrap,free -Wl,-wrap,realloc -Wl,-wrap,malloc -Wl,-wrap,calloc -Wl,-wrap,_free_r -Wl,-wrap,_realloc_r #-Wl,-wrap,printf -Wl,-wrap,sprintf
endif

GLOBAL_LDFLAGS += -Wl,-wrap,printf

GLOBAL_LDFLAGS  += -L ./platform/MCU/$(NAME)/$(TOOLCHAIN_NAME)
GLOBAL_LDFLAGS  += --specs=nosys.specs

# Components
ifdef TEMP_COMMENT
$(NAME)_COMPONENTS += $(TOOLCHAIN_NAME)
endif


ifdef TOOLCHAIN_NAME
ifneq ($(wildcard $(CURDIR)peripherals.$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a),)
$(NAME)_PREBUILT_LIBRARY := peripherals.$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a
else
# Build from source
$(NAME)_COMPONENTS += MCU/MOC108/peripherals
endif

ifeq ($(CONFIG_IPV6),1)
IPV6 := -IPV6
else
IPV6 :=
endif
ifeq ($(CONFIG_SOFTAP),1)
SOFTAP := -SOFTAP
else
SOFTAP :=
endif
ifneq ($(wildcard $(CURDIR)MX108$(IPV6)$(SOFTAP).$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a),)
$(NAME)_PREBUILT_LIBRARY += MX108$(IPV6)$(SOFTAP).$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a
else
# Build from source
$(NAME)_COMPONENTS += MCU/MOC108/MX108
endif
endif

# Source files
$(NAME)_SOURCES := platform_stub.c \
				   ../../$(HOST_ARCH)/platform_core.c \
                   ../mico_platform_common.c \
                   platform_init.c
                   

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  ./mico-os/platform/MCU/MOC108/moc108_standard_targets.mk
EXTRA_TARGET_MAKEFILES +=  ./mico-os/platform/MCU/MOC108/gen_crc_bin.mk

ifeq ($(APP),bootloader)
####################################################################################
# Building bootloader
####################################################################################

DEFAULT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_INCLUDES     += 

else
ifneq ($(filter spi_flash_write, $(APP)),)
####################################################################################
# Building spi_flash_write
####################################################################################

PRE_APP_BUILDS      += bootloader
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_ram$(LINK_SCRIPT_SUFFIX)
GLOBAL_DEFINES      += __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=16384
GLOBAL_INCLUDES     += 

else
ifeq ($(USES_BOOTLOADER),1)
####################################################################################
# Building standard application to run with bootloader
####################################################################################

PRE_APP_BUILDS      += bootloader
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_with_bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_INCLUDES     += 

else
####################################################################################
# Building a standalone application (standalone app without bootloader)
####################################################################################

DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_no_bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_INCLUDES     += 

endif # USES_BOOTLOADER = 1
endif # APP=spi_flash_write
endif # APP=bootloader

