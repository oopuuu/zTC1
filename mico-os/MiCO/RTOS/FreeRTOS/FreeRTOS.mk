#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := FreeRTOS


ifneq ($(filter $(HOST_MCU_FAMILY),MTK7697),)
VERSION := 8.2.0
$(NAME)_SOURCES := ver$(VERSION)/Source/portable/MemMang/heap_4.c
else
VERSION := 9.0.0
$(NAME)_SOURCES := ver$(VERSION)/Source/portable/MemMang/heap_3.c
endif

VERSION_MAJOR 	= $(word 1, $(subst ., ,$(VERSION)))
VERSION_MINOR 	= $(word 2, $(subst ., ,$(VERSION)))
VERSION_REVISION= $(word 3, $(subst ., ,$(VERSION)))

$(NAME)_COMPONENTS += MiCO/RTOS/FreeRTOS/mico

# Define some macros to allow for some network-specific checks
GLOBAL_DEFINES += RTOS_$(NAME)=1
GLOBAL_DEFINES += MXCHIP
GLOBAL_DEFINES += configUSE_MUTEXES
GLOBAL_DEFINES += configUSE_RECURSIVE_MUTEXES
GLOBAL_DEFINES += $(NAME)_VERSION=$$(SLASH_QUOTE_START)v$(VERSION)$$(SLASH_QUOTE_END)
GLOBAL_DEFINES += $(NAME)_VERSION_MAJOR=$(VERSION_MAJOR)
GLOBAL_DEFINES += $(NAME)_VERSION_MINOR=$(VERSION_MINOR)
GLOBAL_DEFINES += $(NAME)_VERSION_REVISION=$(VERSION_REVISION)

GLOBAL_INCLUDES := ver$(VERSION)/Source/include \
				   ver$(VERSION) \
                   ..

$(NAME)_SOURCES +=  ver$(VERSION)/Source/croutine.c \
                    ver$(VERSION)/Source/list.c \
                    ver$(VERSION)/Source/queue.c \
                    ver$(VERSION)/Source/tasks.c
                    
ifeq ($(VERSION_MAJOR),7)
$(NAME)_SOURCES += ver$(VERSION)/Source/ostimers.c
else
$(NAME)_SOURCES += ver$(VERSION)/Source/timers.c
$(NAME)_SOURCES += FreeRTOS-openocd.c
GLOBAL_LDFLAGS  += -Wl,--undefined=uxTopUsedPriority
endif

# Win32_x86 specific sources and includes
Win32_x86_SOURCES  := ver$(VERSION)/Source/portable/MSVC-MingW/port.c
Win32_x86_INCLUDES := ver$(VERSION)/Source/portable/MSVC-MingW

# ARM Cortex M3/4 specific sources and includes

Cortex-M3_SOURCES  := ver$(VERSION)/Source/portable/GCC/ARM_CM3/port.c 
Cortex-M3_INCLUDES := ver$(VERSION)/Source/portable/GCC/ARM_CM3

Cortex-M4_SOURCES  := $(Cortex-M3_SOURCES) 
Cortex-M4_INCLUDES := $(Cortex-M3_INCLUDES)

ifeq ($(VERSION_MAJOR),7)
Cortex-M4F_SOURCES  := $(Cortex-M3_SOURCES) 
Cortex-M4F_INCLUDES := $(Cortex-M3_INCLUDES)
else
Cortex-M4F_SOURCES  := ver$(VERSION)/Source/portable/GCC/ARM_CM4F/port.c 
Cortex-M4F_INCLUDES := ver$(VERSION)/Source/portable/GCC/ARM_CM4F
endif

$(NAME)_SOURCES += $($(HOST_ARCH)_SOURCES)
GLOBAL_INCLUDES += $($(HOST_ARCH)_INCLUDES)


