#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME := Lib_ota_server

$(NAME)_SOURCES := ota_server.c
				   
GLOBAL_INCLUDES += . 

$(NAME)_COMPONENTS += utilities/url
