############################################################################### 
#
#  The MIT License
#  Copyright (c) 2016 MXCHIP Inc.
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy 
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights 
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is furnished
#  to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
#  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
############################################################################### 

NAME := Board_MiCOKit_3238

WLAN_CHIP            	:= 43438
WLAN_CHIP_REVISION   	:= A0
WLAN_CHIP_FAMILY     	:= 43438
WLAN_CHIP_FIRMWARE_VER  := 7.10.323.47.cn2

BT_CHIP              	:= 43438
BT_CHIP_REVISION     	:= A0


MODULE               := 3238
HOST_MCU_FAMILY      := STM32F4xx
HOST_MCU_VARIANT     := STM32F411
HOST_MCU_PART_NUMBER := STM32F411CEY6


BUS := SDIO
VALID_BUSES := SDIO


# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/mico_standard_targets.mk

# Global includes
GLOBAL_INCLUDES  := .

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += HSE_VALUE=26000000
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_LDFLAGS += -L $(MICO_OS_PATH)/board/MK$(MODULE)

# Components
$(NAME)_COMPONENTS += drivers/spi_flash
$(NAME)_COMPONENTS += drivers/keypad/gpio_button
$(NAME)_COMPONENTS += drivers/MiCOKit_EXT

# Source files
$(NAME)_SOURCES := platform.c \
                   wifi_nvram.c
                   
$(NAME)_LINK_FILES := wifi_nvram.o


ifndef NO_WIFI_FIRMWARE
WIFI_FIRMWARE           := $(MICO_OS_PATH)/resources/wifi_firmware/$(WLAN_CHIP)/$(WLAN_CHIP)$(WLAN_CHIP_REVISION)$(WLAN_CHIP_BIN_TYPE)-$(WLAN_CHIP_FIRMWARE_VER).bin
endif

ifndef NO_BT_PATCH_IMAGE
BT_PATCH_FIRMWARE       := $(MICO_OS_PATH)/resources/bt_patch/$(BT_CHIP)/$(BT_CHIP)$(BT_CHIP_REVISION).bin
endif


