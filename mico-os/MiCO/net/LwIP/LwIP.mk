#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := LwIP

ifneq ($(filter $(HOST_MCU_FAMILY),MTK7697),)
VERSION := 1.5.0
else
VERSION := 1.4.0.rc1
endif

VERSION_MAJOR 		= $(word 1, $(subst ., ,$(VERSION)))
VERSION_MINOR 		= $(word 2, $(subst ., ,$(VERSION)))
VERSION_REVISION	= $(word 3, $(subst ., ,$(VERSION)))

$(NAME)_COMPONENTS += MiCO/net/LwIP/mico

VALID_RTOS_LIST:= FreeRTOS

# Define some macros to allow for some network-specific checks
GLOBAL_DEFINES += NETWORK_$(NAME)=1
GLOBAL_DEFINES += MXCHIP
GLOBAL_DEFINES += $(NAME)_VERSION_MAJOR=$(VERSION_MAJOR)
GLOBAL_DEFINES += $(NAME)_VERSION_MINOR=$(VERSION_MINOR)
GLOBAL_DEFINES += $(NAME)_VERSION_REVISION=$(VERSION_REVISION)


$(NAME)_INCLUDES := ver$(VERSION) \
                    ver$(VERSION)/src/include \
                    ver$(VERSION)/src/include/ipv4

$(NAME)_SOURCES :=  mico/mico_socket.c
