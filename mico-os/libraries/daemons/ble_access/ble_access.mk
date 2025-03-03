#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME := Lib_Ble_Access_Core_Framework

GLOBAL_INCLUDES += .

$(NAME)_SOURCES := ble_access_core.c \
                    ble_access_core_i.c

$(NAME)_COMPONENTS := daemons/bt_smart bluetooth/low_energy 