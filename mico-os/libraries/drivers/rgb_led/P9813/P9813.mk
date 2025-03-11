#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Lib_RGB_P9813_$(PLATFORM)

$(NAME)_SOURCES := hsb2rgb_led.c \
                   rgb_led.c

GLOBAL_INCLUDES := .
