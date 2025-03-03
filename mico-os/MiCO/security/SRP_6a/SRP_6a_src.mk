#
# Copyright 2016, MXCHIP Corporation
# All Rights Reserved.
#


SRP_6a_DIR := Lib_SRP6a

$(NAME)_SOURCES := 	\
					$(SRP_6a_DIR)/bn_add.c \
                    $(SRP_6a_DIR)/bn_asm.c \
                    $(SRP_6a_DIR)/bn_ctx.c \
                    $(SRP_6a_DIR)/bn_div.c \
                    $(SRP_6a_DIR)/bn_exp.c \
                    $(SRP_6a_DIR)/bn_lib.c \
                    $(SRP_6a_DIR)/bn_mul.c \
                    $(SRP_6a_DIR)/bn_shift.c \
                    $(SRP_6a_DIR)/bn_sqr.c \
                    $(SRP_6a_DIR)/srp.c
				
$(NAME)_INCLUDES := inc

