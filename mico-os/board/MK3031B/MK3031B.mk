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

NAME := Board_MK3031B

WLAN_CHIP            	:= mw30x
WLAN_CHIP_REVISION   	:= _uapsta
WLAN_CHIP_FAMILY     	:= MW3xx
WLAN_CHIP_FIRMWARE_VER  := 14.76.36.p103

MODULE              	:= 3031B
HOST_MCU_FAMILY      	:= MW3xx
HOST_MCU_VARIANT     	:= MW310
HOST_MCU_PART_NUMBER 	:= 

BUS := MW310

# Extra build target in mico_moc_targets.mk, create MOC images and download
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/mico_moc_targets.mk

# Global includes
GLOBAL_INCLUDES  := .

# Global defines
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_LDFLAGS += -L $(MICO_OS_PATH)/board/MK$(MODULE)

# Components
$(NAME)_COMPONENTS += drivers/MiCOKit_EXT

# Source files
$(NAME)_SOURCES := platform.c

# MOC configuration
MOC_APP_OFFSET      := 0x64000

ifndef NO_WIFI_FIRMWARE
WIFI_FIRMWARE := $(MICO_OS_PATH)/resources/wifi_firmware/$(WLAN_CHIP)/$(WLAN_CHIP)$(WLAN_CHIP_REVISION)$(WLAN_CHIP_BIN_TYPE)-$(WLAN_CHIP_FIRMWARE_VER).bin
endif

VALID_OSNS_COMBOS  := mocOS@mocIP NoRTOS@LwIP
VALID_TLS          := mocSSL wolfSSL

