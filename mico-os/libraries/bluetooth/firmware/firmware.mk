#
# Copyright 2016, MXCHIP Corporation
# All Rights Reserved.
#

NAME := Lib_MICO_Bluetooth_Firmware_for_$(BT_CHIP)$(BT_CHIP_REVISION)

# Use Firmware images which are already present
ifeq ($(BT_CHIP_XTAL_FREQUENCY),)
$(NAME)_SOURCES := $(BT_CHIP)$(BT_CHIP_REVISION)/bt_firmware_image.c
else
$(NAME)_SOURCES := $(BT_CHIP)$(BT_CHIP_REVISION)/$(BT_CHIP_XTAL_FREQUENCY)/bt_firmware_image.c
endif
