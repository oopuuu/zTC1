#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Sensor_BMG160_$(PLATFORM)

$(NAME)_SOURCES := bmg160_user.c \
                   bmg160.c

GLOBAL_INCLUDES := .
