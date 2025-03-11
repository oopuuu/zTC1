#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME := Lib_http_server_Framework


$(NAME)_SOURCES := http_parse.c \
				   httpd_handle.c \
				   httpd_ssi.c \
				   httpd_sys.c \
				   httpd_wsgi.c \
				   httpd.c \
				   http-strings.c
				   
				   
GLOBAL_INCLUDES += . 

$(NAME)_COMPONENTS += utilities/base64