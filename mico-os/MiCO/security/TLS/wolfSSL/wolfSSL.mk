#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Lib_wolfSSL


GLOBAL_INCLUDES += .

ifneq ($(wildcard $(CURDIR)Lib_wolfSSL.$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a),)
ifeq ($(HIGH_SECURITY),1)
$(NAME)_PREBUILT_LIBRARY := High_Security/Lib_wolfSSL.$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a
else
$(NAME)_PREBUILT_LIBRARY := Lib_wolfSSL.$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a
endif
else
# Build from source
include $(CURDIR)wolfSSL_src.mk
endif



