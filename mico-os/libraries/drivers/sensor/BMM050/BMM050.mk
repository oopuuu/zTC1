#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Sensor_BMM050_$(PLATFORM)

$(NAME)_SOURCES := bmm050_user.c \
                   bmm050.c

GLOBAL_INCLUDES := .
