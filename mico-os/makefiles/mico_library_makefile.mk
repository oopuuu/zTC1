#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#
export SOURCE_ROOT ?= ./
export MICO_OS_PATH := $(SOURCE_ROOT)mico-os

LIB_NAME := $(notdir $(LIB_DIR))
LIB_OUT_DIR := $(dir $(LIB_DIR))

ALWAYS_OPTIMISE := 1

BYPASS_LIBRARY_POISON_CHECK=1

include $(LIB_DIR)/$(LIB_NAME)_src.mk

SOURCES := $(addprefix $(LIB_DIR)/,$($(NAME)_SOURCES))
LIBRARY_OUTPUT_DIR := $(LIB_OUT_DIR)

# Standard library defines
CFLAGS += -c -MD -ggdb $(CPU_CFLAGS) $(ENDIAN_CFLAGS_LITTLE) -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11 

CFLAGS += $(addprefix -I,$(GLOBAL_INCLUDES)) $(addprefix -D,$(GLOBAL_DEFINES)) $(addprefix -I$(LIB_DIR)/,$($(NAME)_INCLUDES)) $(addprefix -D,$($(NAME)_DEFINES)) $($(NAME)_CFLAGS)

CFLAGS += -I$(MICO_OS_PATH)/include \
          -I$(MICO_OS_PATH)/mico/security \
          -I$(MICO_OS_PATH)/mico/security/Sodium/inc \
          -I$(MICO_OS_PATH)/mico/security/SRP_6a/inc \
          -I$(MICO_OS_PATH)/mico/system \
          -I$(MICO_OS_PATH)/libraries/drivers \
          -I$(MICO_OS_PATH)/libraries/drivers/MiCOKit_EXT \
          -I$(MICO_OS_PATH)/libraries/utilities \
          -I$(MICO_OS_PATH)/platform \
          -I$(MICO_OS_PATH)/platform/include \
          -I$(MICO_OS_PATH)/platform/$(HOST_ARCH) \
          -I$(MICO_OS_PATH)/template/includes

include $(MICO_OS_PATH)/makefiles/mico_library_build.mk

