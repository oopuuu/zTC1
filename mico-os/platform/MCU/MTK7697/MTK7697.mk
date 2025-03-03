#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME = MTK7697

# Host architecture is ARM Cortex M4
HOST_ARCH := Cortex-M4F

# Host MCU alias for OpenOCD
HOST_OPENOCD := mtk7697

GLOBAL_INCLUDES := 	./peripherals			\
					../..					\
					../../include			\
					../../Cortex-M4			\
					../../$(TOOLCHAIN_NAME) \
					../../Cortex-M4/CMSIS
					
# Global flags
GLOBAL_CFLAGS   += $$(CPU_CFLAGS)    $$(ENDIAN_CFLAGS_LITTLE)
GLOBAL_CXXFLAGS += $$(CPU_CXXFLAGS)  $$(ENDIAN_CXXFLAGS_LITTLE)
GLOBAL_ASMFLAGS += $$(CPU_ASMFLAGS)  $$(ENDIAN_ASMFLAGS_LITTLE)
GLOBAL_LDFLAGS  += $$(CPU_LDFLAGS)   $$(ENDIAN_LDFLAGS_LITTLE)

GLOBAL_LDFLAGS  += -nostartfiles
GLOBAL_LDFLAGS  += -L .

# Components
$(NAME)_COMPONENTS += GCC
$(NAME)_COMPONENTS += MCU/MTK7697/peripherals
$(NAME)_COMPONENTS += utilities

ifeq ($(filter $(APP),bootloader spi_flash_write),)
GLOBAL_LDFLAGS   += $$(CLIB_LDFLAGS_NANO)
endif

ifeq ($(APP),bootloader)
PRE_APP_BUILDS				+=
STARTUP_FILE 				:= 	./startup_bootloader.c
GLOBAL_INCLUDES 			+=	./apps/bootloader/inc
$(NAME)_PREBUILT_LIBRARY 	:= 	./apps/bootloader/GCC/Build/libhal.a
DEFAULT_LINK_SCRIPT 		+= 	bootloader.ld
GLOBAL_DEFINES      		+=
else
ifeq ($(APP),spi_flash_write)
PRE_APP_BUILDS      		+= 	bootloader
STARTUP_FILE 				:= 	./startup_app_ram.c
GLOBAL_INCLUDES 			+=	./apps/bootloader/inc
$(NAME)_PREBUILT_LIBRARY 	:= 	./apps/bootloader/GCC/Build/libhal.a
DEFAULT_LINK_SCRIPT 		:= 	app_ram.ld
GLOBAL_DEFINES      		+= 	__JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=16384
else
PRE_APP_BUILDS      		+= 	bootloader
STARTUP_FILE 				:= 	./startup_application.s
GLOBAL_INCLUDES 			+=	./apps/mxchipWnet/inc	\
								./kernel/service/inc
$(NAME)_PREBUILT_LIBRARY 	:= 	./apps/mxchipWnet/GCC/Build/libhal.a
DEFAULT_LINK_SCRIPT 		:= 	application.ld
GLOBAL_LDFLAGS  			+= -Wl,-wrap,_malloc_r -Wl,-wrap,free -Wl,-wrap,realloc -Wl,-wrap,malloc -Wl,-wrap,calloc
endif
endif

# Source files
$(NAME)_SOURCES := 	tbd.c						\
					$(STARTUP_FILE) 			\
					system_mt7687.c				\
					platform_init.c 			\
					../mico_platform_common.c	\
					../../Cortex-M4/platform_core.c \

#Incldue Path
GLOBAL_INCLUDES += 	./driver/CMSIS/Device/MTK/mt7687/Include	\
					./driver/chip/mt7687/inc					\
					./driver/chip/inc