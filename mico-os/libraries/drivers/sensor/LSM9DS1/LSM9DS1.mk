#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME := Sensor_LSM9DS1_$(PLATFORM)

$(NAME)_SOURCES := lsm9ds1_acc_gyr.c \
				   lsm9ds1_mag.c

GLOBAL_INCLUDES := .
