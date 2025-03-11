#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Lib_JSON_C

GLOBAL_INCLUDES := .

$(NAME)_SOURCES := arraylist.c \
                   json_debug.c \
                   json_object.c \
                   json_tokener.c \
                   json_util.c \
                   linkhash.c \
                   printbuf.c
                   
$(NAME)_CFLAGS   += -Wno-unused-value

