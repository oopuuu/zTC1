#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME = ATSAMG55

# Host architecture is ARM Cortex M4
HOST_ARCH := Cortex-M4

# Host MCU alias for OpenOCD
HOST_OPENOCD := at91samg5x

GLOBAL_INCLUDES := . \
                   .. \
                   ../.. \
                   ../../include \
                   ../../$(TOOLCHAIN_NAME) \
                   ../../$(HOST_ARCH) \
                   ../../$(HOST_ARCH)/CMSIS \
                   peripherals

# Global defines
GLOBAL_DEFINES  := USE_STDPERIPH_DRIVER \
                   _STM3x_ \
                   _STM32x_
                   
GLOBAL_DEFINES += __SAMG55J19__
GLOBAL_DEFINES += BOARD=USER_BOARD

# Global flags
GLOBAL_CFLAGS   += $$(CPU_CFLAGS)    $$(ENDIAN_CFLAGS_LITTLE)
GLOBAL_CXXFLAGS += $$(CPU_CXXFLAGS)  $$(ENDIAN_CXXFLAGS_LITTLE)
GLOBAL_ASMFLAGS += $$(CPU_ASMFLAGS)  $$(ENDIAN_ASMFLAGS_LITTLE)
GLOBAL_LDFLAGS  += $$(CPU_LDFLAGS)   $$(ENDIAN_LDFLAGS_LITTLE)


GLOBAL_LDFLAGS  += -nostartfiles
GLOBAL_LDFLAGS  += -Wl,--defsym,__STACKSIZE__=$$($(RTOS)_START_STACK)
GLOBAL_LDFLAGS  += -L ./platform/MCU/$(NAME)/$(TOOLCHAIN_NAME)

# Components
$(NAME)_COMPONENTS += $(TOOLCHAIN_NAME)
$(NAME)_COMPONENTS += MCU/ATSAMG55/peripherals
$(NAME)_COMPONENTS += utilities

# Source files
$(NAME)_SOURCES := ../../$(HOST_ARCH)/crt0_$(TOOLCHAIN_NAME).c \
                   ../../$(HOST_ARCH)/platform_core.c \
                   ../../$(HOST_ARCH)/hardfault_handler.c \
                   ../mico_platform_common.c \
                   ../wlan_platform_common.c \
                   platform_init.c \
                   GCC/platform_unhandled_isr.c \
                   platform_vector_table.c \


ifndef NO_WIFI
$(NAME)_SOURCES += wlan_bus_driver/wlan_platform.c \
                   wlan_bus_driver/wlan_bus_$(BUS).c
else
#ifdef SHARED_WIFI_SPI_BUS
#$(NAME)_SOURCES += wlan_bus_driver/wlan_bus_SPI.c
#endif #SHARED_WIFI_SPI_BUS
endif #NO_WIFI


# These need to be forced into the final ELF since they are not referenced otherwise
$(NAME)_LINK_FILES := ../../$(HOST_ARCH)/crt0_$(TOOLCHAIN_NAME).o \
                      ../../$(HOST_ARCH)/hardfault_handler.o \
                      platform_vector_table.o \

# $(NAME)_CFLAGS = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)


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

