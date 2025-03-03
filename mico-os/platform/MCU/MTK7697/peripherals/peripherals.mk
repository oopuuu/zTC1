#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME = MTK7697_Peripheral_Drivers

GLOBAL_INCLUDES := .

$(NAME)_SOURCES := platform_uart.c 			\
				   platform_gpio.c 			\
				   platform_flash.c 		\
				   platform_wdt.c			\
				   platform_adc.c			\
				   platform_spi.c			\
				   platform_pwm.c			\
				   platform_rtc.c			\
				   platform_rng.c			\
				   platform_i2c.c			\
				   platform_peripherials.c

$(NAME)_COMPONENTS += MCU/MTK7697/peripherals/libraries