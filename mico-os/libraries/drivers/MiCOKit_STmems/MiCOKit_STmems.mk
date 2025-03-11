#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Lib_MiCOKit_STmems

$(NAME)_SOURCES := MiCOKit_STmems.c
               
GLOBAL_INCLUDES := . \
                   ..
                   
$(NAME)_COMPONENTS += drivers/display/VGM128064 \
                      drivers/keypad/gpio_button \
                      drivers/motor/dc_motor \
                      drivers/rgb_led/P9813 \
                      drivers/sensor/HTS221 \
                      drivers/sensor/UVIS25 \
                      drivers/sensor/LSM9DS1 \
                      drivers/sensor/LPS25HB \
                      drivers/sensor/light_adc