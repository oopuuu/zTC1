#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME = LPC5410x_Peripheral_Libraries

#$CFLAGS = -Wno-sign-conversion

GLOBAL_INCLUDES :=  . \
                    ../../../$(HOST_ARCH)/CMSIS \
                    
                    

$(NAME)_SOURCES := \
				   adc_5410x.c \
				   chip_5410x.c \
				   clock_5410x.c \
				   crc_5410x.c \
				   dma_5410x.c \
				   fifo_5410x.c \
				   fpu_init.c \
				   gpio_5410x.c \
				   gpiogroup_5410x.c \
				   i2c_common_5410x.c \
				   i2cm_5410x.c \
				   i2cs_5410x.c \
				   iap.c \
				   iocon_5410x.c \
				   pinint_5410x.c \
				   pll_5410x.c \
				   power_control.c \
				   ring_buffer.c \
				   ritimer_5410x.c \
				   rtc_5410x.c \
				   rtc_ut.c \
				   sct_5410x.c \
				   sct_pwm_5410x.c \
				   spi_5410x.c \
				   spi_common_5410x.c \
				   spim_5410x.c \
				   spis_5410x.c \
				   stopwatch_5410x.c \
				   syscon_5410x.c \
				   sysinit_5410x.c \
				   timer_5410x.c \
				   uart_5410x.c \
				   utick_5410x.c \
				   wwdt_5410x.c
				  
				   


