#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME = Lib_MiCO_Security

# Curve25519
$(NAME)_SOURCES  += Curve25519/curve25519-donna.c 
GLOBAL_INCLUDES += Curve25519

# SHA
$(NAME)_SOURCES  += SHAUtils/hkdf.c \
                    SHAUtils/hmac.c \
                    SHAUtils/sha1.c \
                    SHAUtils/sha224-256.c \
                    SHAUtils/sha384-512.c \
                    SHAUtils/usha.c
                   
GLOBAL_INCLUDES += SHAUtils

#SRP-6a
$(NAME)_COMPONENTS += MiCO/security/SRP_6a

#Sodium
$(NAME)_COMPONENTS += MiCO/security/Sodium




