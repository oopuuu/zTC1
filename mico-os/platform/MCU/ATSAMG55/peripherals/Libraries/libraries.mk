#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME = ATSAMG55_Peripheral_Libraries

#$CFLAGS = -Wno-sign-conversion

GLOBAL_INCLUDES :=  . \
                    ../../../$(HOST_ARCH)/CMSIS \
                    asf-3.22.0/sam/utils/cmsis/samg/samg55/include \
                    asf-3.22.0/sam/services/flash_efc \
                    asf-3.22.0/sam/drivers/pio \
                    asf-3.22.0/sam/drivers/usart \
                    asf-3.22.0/sam/drivers/rtt \
                    asf-3.22.0/sam/drivers/supc \
                    asf-3.22.0/sam/drivers/matrix \
                    asf-3.22.0/sam/drivers/spi \
                    asf-3.22.0/sam/drivers/efc \
                    asf-3.22.0/sam/drivers/pmc \
                    asf-3.22.0/sam/drivers/pdc \
                    asf-3.22.0/sam/drivers/wdt \
                    asf-3.22.0/sam/drivers/flexcom \
                    asf-3.22.0/common/services/ioport \
                    asf-3.22.0/common/services/clock \
                    asf-3.22.0/common/utils \
                    asf-3.22.0/sam/utils \
                    asf-3.22.0/sam/utils/cmsis/samg/samg55/include \
                    asf-3.22.0/sam/utils/cmsis/samg/samg55/include/instance \
                    asf-3.22.0/sam/utils/preprocessor \
                    asf-3.22.0/sam/utils/header_files \
                    asf-3.22.0/sam/utils/cmsis/samg/samg55/include \
                    asf-3.22.0/common/services/sleepmgr \
                    asf-3.22.0/common/boards \
                    asf-3.22.0/sam/drivers/twi \
                    asf-3.22.0/sam/drivers/adc
                    
                    

$(NAME)_SOURCES := \
                    asf-3.22.0/sam/services/flash_efc/flash_efc.c \
                    asf-3.22.0/sam/drivers/pio/pio.c \
                    asf-3.22.0/sam/drivers/pio/pio_handler.c \
                    asf-3.22.0/sam/drivers/usart/usart.c \
                    asf-3.22.0/sam/drivers/rtt/rtt.c \
                    asf-3.22.0/sam/drivers/supc/supc.c \
                    asf-3.22.0/sam/drivers/matrix/matrix.c \
                    asf-3.22.0/sam/drivers/spi/spi.c \
                    asf-3.22.0/sam/drivers/efc/efc.c \
                    asf-3.22.0/sam/drivers/pmc/pmc.c \
                    asf-3.22.0/sam/drivers/pmc/sleep.c \
                    asf-3.22.0/sam/drivers/pdc/pdc.c \
                    asf-3.22.0/sam/drivers/wdt/wdt.c \
                    asf-3.22.0/sam/drivers/flexcom/flexcom.c \
                    asf-3.22.0/sam/drivers/twi/twi.c \
                    asf-3.22.0/sam/drivers/adc/adc2.c \
                    asf-3.22.0/common/services/clock/samg/sysclk.c \
                    asf-3.22.0/sam/utils/cmsis/samg/samg55/source/templates/system_samg55.c \
                    asf-3.22.0/common/utils/interrupt/interrupt_sam_nvic.c \
                    asf-3.22.0/common/services/sleepmgr/sam/sleepmgr.c
#                    asf-3.22.0/sam/utils/cmsis/samg/samg55/source/templates/gcc/startup_samg55.c

