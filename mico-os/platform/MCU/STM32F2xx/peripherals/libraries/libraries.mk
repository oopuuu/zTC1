#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME = STM32F2xx_Peripheral_Libraries

GLOBAL_INCLUDES :=  . \
                    STM32F2xx_StdPeriph_Driver/inc \
                    ../../../$(HOST_ARCH)/CMSIS

$(NAME)_SOURCES := \
                   system_stm32f2xx.c \
                   STM32F2xx_StdPeriph_Driver/src/misc.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_adc.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_can.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_crc.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_dac.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_dbgmcu.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_dma.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_exti.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_flash.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_gpio.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rng.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_i2c.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_iwdg.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_pwr.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rcc.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rtc.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_sdio.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_spi.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_syscfg.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_tim.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_usart.c \
                   STM32F2xx_StdPeriph_Driver/src/stm32f2xx_wwdg.c

