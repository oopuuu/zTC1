/**
 ******************************************************************************
 * @file    mico_ssl.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-Aug-2018
 * @brief   This file provide the MiCO Socket abstract layer convert functions.
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "common.h"
#include "moc_api.h"


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

extern const mico_api_t *lib_api_p;

/******************************************************
 *               Function Definitions
 ******************************************************/

/* SSL */
void ssl_set_cert(const char *_cert_pem, const char *private_key_pem)
{
    lib_api_p->ssl_set_cert(_cert_pem, private_key_pem);
}

void* ssl_connect(int fd, int calen, char *ca, int *ssl_errno)
{
    return lib_api_p->ssl_connect(fd, calen, ca, ssl_errno);
}

void* ssl_nonblock_connect(int fd, int calen, char*ca, int *errno, int timeout)
{
	return lib_api_p->ssl_nonblock_connect(fd, calen, ca, errno, timeout);
}

void* ssl_accept(int fd)
{
    return lib_api_p->ssl_accept(fd);
}

int ssl_send(mico_ssl_t ssl, void *data, size_t len)
{
    return lib_api_p->ssl_send(ssl, data, len);
}

int ssl_recv(mico_ssl_t ssl, void *data, size_t len)
{
  return lib_api_p->ssl_recv(ssl, data, len);
}

int ssl_close(mico_ssl_t ssl)
{
  return lib_api_p->ssl_close( ssl);
}

int ssl_socket( mico_ssl_t ssl )
{
  return lib_api_p->ssl_socket( ssl);
}

int ssl_set_loggingcb(ssl_Logging_cb f)
{
  return lib_api_p->ssl_set_loggingcb(f);
}

void ssl_set_client_version( uint8_t version )
{
    lib_api_p->set_ssl_client_version(version);
}

int ssl_pending(void*ssl)
{
	return lib_api_p->ssl_pending(ssl);
}

int ssl_get_error(void* ssl, int ret)
{
	return lib_api_p->ssl_get_error(ssl, ret);
}

void ssl_set_using_nonblock(void* ssl, int nonblock)
{
	 lib_api_p->ssl_set_using_nonblock(ssl, nonblock);
}

void ssl_set_client_cert(const char *cert_pem, const char *private_key_pem)
{
	lib_api_p->ssl_set_client_cert(cert_pem, private_key_pem);
}

void* ssl_connect_sni(int fd, int calen, char*ca, char *sni_servername, int *errno)
{
	return lib_api_p->ssl_connect_sni(fd, calen, ca, sni_servername, errno);
}

