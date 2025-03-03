#
#  UNPUBLISHED PROPRIETARY wolfssl CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

$(NAME)_SOURCES :=	wolfSSL/wolfcrypt/src/aes.c \
					wolfSSL/wolfcrypt/src/arc4.c \
					wolfSSL/wolfcrypt/src/asm.c \
					wolfSSL/wolfcrypt/src/asn.c \
					wolfSSL/wolfcrypt/src/blake2b.c \
					wolfSSL/wolfcrypt/src/camellia.c \
					wolfSSL/wolfcrypt/src/chacha.c \
					wolfSSL/wolfcrypt/src/chacha20_poly1305.c \
					wolfSSL/wolfcrypt/src/coding.c \
					wolfSSL/wolfcrypt/src/compress.c \
					wolfSSL/wolfcrypt/src/curve25519.c \
					wolfSSL/wolfcrypt/src/des3.c \
					wolfSSL/wolfcrypt/src/dh.c \
					wolfSSL/wolfcrypt/src/dsa.c \
					wolfSSL/wolfcrypt/src/ecc.c \
					wolfSSL/wolfcrypt/src/ecc_fp.c \
					wolfSSL/wolfcrypt/src/ed25519.c \
					wolfSSL/wolfcrypt/src/error.c \
					wolfSSL/wolfcrypt/src/fe_low_mem.c \
					wolfSSL/wolfcrypt/src/fe_operations.c \
					wolfSSL/wolfcrypt/src/ge_low_mem.c \
					wolfSSL/wolfcrypt/src/ge_operations.c \
					wolfSSL/wolfcrypt/src/hash.c \
					wolfSSL/wolfcrypt/src/hc128.c \
					wolfSSL/wolfcrypt/src/hmac.c \
					wolfSSL/wolfcrypt/src/integer.c \
					wolfSSL/wolfcrypt/src/logging.c \
					wolfSSL/wolfcrypt/src/md2.c \
					wolfSSL/wolfcrypt/src/md4.c \
					wolfSSL/wolfcrypt/src/md5.c \
					wolfSSL/wolfcrypt/src/memory.c \
					wolfSSL/wolfcrypt/src/pkcs7.c \
					wolfSSL/wolfcrypt/src/poly1305.c \
					wolfSSL/wolfcrypt/src/pwdbased.c \
					wolfSSL/wolfcrypt/src/rabbit.c \
					wolfSSL/wolfcrypt/src/random.c \
					wolfSSL/wolfcrypt/src/ripemd.c \
					wolfSSL/wolfcrypt/src/rsa.c \
					wolfSSL/wolfcrypt/src/sha.c \
					wolfSSL/wolfcrypt/src/sha256.c \
					wolfSSL/wolfcrypt/src/sha512.c \
					wolfSSL/wolfcrypt/src/srp.c \
					wolfSSL/wolfcrypt/src/tfm.c \
					wolfSSL/wolfcrypt/src/wc_encrypt.c \
					wolfSSL/wolfcrypt/src/wc_port.c \
					wolfSSL/src/crl.c \
					wolfSSL/src/internal.c \
					wolfSSL/src/io.c \
					wolfSSL/src/keys.c \
					wolfSSL/src/ocsp.c \
					wolfSSL/src/sniffer.c \
					wolfSSL/src/ssl.c \
					wolfSSL/src/tls.c
					
$(NAME)_SOURCES +=	wolfSSL/mico/crypto_wrap.c \
                    wolfSSL/mico/ssl_wrap.c
					
$(NAME)_INCLUDES := wolfSSL \
					wolfSSL/mico
					
$(NAME)_DEFINES :=	WOLFSSL_USER_SETTINGS

