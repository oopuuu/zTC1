#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME = LPC5410x_Peripheral_Drivers

GLOBAL_INCLUDES := .

# Include ATSAMG55 Standard Peripheral Libraries
$(NAME)_COMPONENTS += MCU/LPC5410x/peripherals/libraries

$(NAME)_SOURCES := platform_adc.c \
                   platform_rtc.c \
                   platform_gpio.c \
                   platform_i2c.c \
                   platform_rng.c \
                   platform_nano_second.c \
                   platform_mcu_powersave.c \
                   platform_pwm.c \
                   platform_flash.c \
                   platform_spi.c \
                   platform_uart.c \
                   platform_watchdog.c
