#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME = Lib_MiCO_System_QC


$(NAME)_SOURCES := qc_test.c \
                   internal/qc_test_tcpip.c \
                   internal/qc_test_wlan.c \
                   internal/qc_test_ble.c \
                   internal/qc_test_cli.c
                   
GLOBAL_INCLUDES := . 
$(NAME)_INCLUDES := internal

# Enable GPIO test if gpio_pair_$(MODULE).c exist
ifneq ($(wildcard $(CURDIR)internal/gpio_pair/gpio_pair_$(MODULE).c),)
$(NAME)_SOURCES += internal/qc_test_gpio.c \
                   internal/gpio_pair/gpio_pair_$(MODULE).c
                      
GLOBAL_DEFINES += QC_TEST_GPIO_ENABLE
endif



