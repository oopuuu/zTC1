#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Lib_MiCO_Security_Sodium

GLOBAL_INCLUDES := inc

#SRP-6a
ifneq ($(wildcard $(CURDIR)Lib_Sodium.$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a),)
$(NAME)_PREBUILT_LIBRARY := Lib_Sodium.$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a
else
# Build from source
include $(CURDIR)Sodium_src.mk
endif


