#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := GCC

$(NAME)_SOURCES = mem_newlib.c math_newlib.c stdio_newlib.c time_newlib.c eabi.c

$(NAME)_LINK_FILES := mem_newlib.o math_newlib.o stdio_newlib.o time_newlib.o eabi.o

ifneq ($(filter $(subst ., ,$(COMPONENTS)),mocOS),)
GLOBAL_LDFLAGS += -Wl,-wrap,_malloc_r -Wl,-wrap,free -Wl,-wrap,realloc -Wl,-wrap,malloc -Wl,-wrap,calloc -Wl,-wrap,_free_r -Wl,-wrap,_realloc_r #-Wl,-wrap,printf -Wl,-wrap,sprintf
endif
