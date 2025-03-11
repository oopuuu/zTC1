#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME := Lib_MICO_Bluetooth_Smart_Framework

GLOBAL_INCLUDES += include

$(NAME)_INCLUDES += internal

$(NAME)_SOURCES := mico_bt_management.c \
                   mico_bt_peripheral.c \
                   mico_bt_smartbridge_gatt.c \
                   mico_bt_smartbridge.c \
                   mico_bt_smartbridge_cfg.c \
                   internal/bt_peripheral_stack_interface.c \
                   internal/bt_smart_attribute.c \
                   internal/bt_smartbridge_att_cache_manager.c \
                   internal/bt_smartbridge_socket_manager.c \
                   internal/bt_smartbridge_helper.c \
                   internal/bt_smartbridge_stack_interface.c
                   
                   