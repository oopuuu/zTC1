#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Lib_libwebsocket

WS_SOURCE_DIR := libwebsocket/src

ifneq ($(wildcard $(CURDIR)Lib_libwebsocket.$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a),)
$(NAME)_PREBUILT_LIBRARY := Lib_libwebsocket.$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a
else



$(NAME)_SOURCES := 	$(WS_SOURCE_DIR)/alloc.c \
                    $(WS_SOURCE_DIR)/sha-1.c \
					$(WS_SOURCE_DIR)/base64-decode.c \
					$(WS_SOURCE_DIR)/client-handshake.c  \
					$(WS_SOURCE_DIR)/handshake.c \
					$(WS_SOURCE_DIR)/libwebsockets.c \
					$(WS_SOURCE_DIR)/parsers.c \
					$(WS_SOURCE_DIR)/ssl-client.c \
					$(WS_SOURCE_DIR)/client-parser.c \
					$(WS_SOURCE_DIR)/header.c \
					$(WS_SOURCE_DIR)/pollfd.c \
					$(WS_SOURCE_DIR)/client.c \
					$(WS_SOURCE_DIR)/output.c \
					$(WS_SOURCE_DIR)/lws-plat-mico.c \
					$(WS_SOURCE_DIR)/context.c \
					$(WS_SOURCE_DIR)/server-handshake.c \
					$(WS_SOURCE_DIR)/daemonize.c \
					$(WS_SOURCE_DIR)/service.c \
					$(WS_SOURCE_DIR)/ssl.c



#$(WS_SOURCE_DIR)/ssl-server.c  \					
# $(WS_SOURCE_DIR)/lejp-conf.c $(WS_SOURCE_DIR)/server.c	
endif

GLOBAL_INCLUDES := include
$(NAME)_INCLUDES := $(WS_SOURCE_DIR)


