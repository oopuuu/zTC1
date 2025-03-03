#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Lib_MiCOKit_EXT_$(PLATFORM)

$(NAME)_SOURCES := micokit_ext_mfg.c \
                   micokit_ext.c \
                   motion_sensor.c \
                   temp_hum_sensor.c
               


GLOBAL_INCLUDES := . \
                   ..
                   
$(NAME)_COMPONENTS += drivers/display/VGM128064 \
                      drivers/keypad/gpio_button \
                      drivers/motor/dc_motor \
                      drivers/rgb_led/P9813 \
                      drivers/sensor/BME280 \
                      drivers/sensor/DHT11 \
                      drivers/sensor/APDS9930 \
                      drivers/sensor/light_adc \
                      drivers/sensor/infrared_adc
                      

#(info $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS))
#$(NAME)_CFLAGS  = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)