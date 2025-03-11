#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Sensor_BMA2x2_$(PLATFORM)

$(NAME)_SOURCES := bma2x2_user.c \
                   bma2x2.c

GLOBAL_INCLUDES := .
