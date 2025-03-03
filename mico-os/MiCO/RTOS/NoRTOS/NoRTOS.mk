#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := NoRTOS

GLOBAL_DEFINES += NO_MICO_RTOS

GLOBAL_INCLUDES := .

$(NAME)_SOURCES := rtos.c

Cortex-M3_SOURCES  := 
Cortex-M3_INCLUDES := portable/GCC/ARM_CM

Cortex-M4_SOURCES  := 
Cortex-M4_INCLUDES := portable/GCC/ARM_CM

Cortex-M4F_SOURCES  := 
Cortex-M4F_INCLUDES := portable/GCC/ARM_CM

ARM968E-S_SOURCES	:= portable/GCC/ARM968E_S/port.c 
ARM968E-S_INCLUDES 	:= portable/GCC/ARM968E_S

$(NAME)_SOURCES += $($(HOST_ARCH)_SOURCES)
GLOBAL_INCLUDES += $($(HOST_ARCH)_INCLUDES)
