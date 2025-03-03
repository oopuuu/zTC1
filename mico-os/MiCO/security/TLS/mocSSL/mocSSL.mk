#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := mocSSL

VERSION := 1.0.0

$(NAME)_SOURCES := mico/mico_ssl.c

VALID_OSNS_COMBOS += mocOS-mocIP

# Define some macros to allow for some specific checks
GLOBAL_DEFINES += TLS_SSL_$(NAME)=1
GLOBAL_DEFINES += $(NAME)_VERSION=$$(SLASH_QUOTE_START)v$(VERSION)$$(SLASH_QUOTE_END)
