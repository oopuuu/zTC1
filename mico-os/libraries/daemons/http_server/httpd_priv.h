/**
 ******************************************************************************
 * @file    httpd_priv.h
 * @author  QQ DING
 * @version V1.0.0
 * @date    1-September-2015
 * @brief   This file is httpd_ssi.c and httpd_sys.c and httpd_wsgi.c header files
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#ifndef __HTTPD_PRIV_H__
#define __HTTPD_PRIV_H__

#include "httpd.h"
#include "mico.h"

//#define CONFIG_HTTPD_DEBUG 

#ifdef CONFIG_HTTPD_DEBUG
#define httpd_d(M, ...)				\
	custom_log("httpd", M, ##__VA_ARGS__)
#else
#define httpd_d(...)
#endif /* ! CONFIG_HTTPD_DEBUG */


int httpd_test_setup(void);
int httpd_wsgi_init(void);

/** Set the httpd-wide error message
 *
 * Often, when something fails, a 500 Internal Server Error will be emitted.
 * This is especially true when the reason is something arbitrary, like the
 * maximum header line that the httpd can handle is exceeded.  When this
 * happens, a function can set the error string to be passed back to the user.
 * This facilitates debugging.  Note that the error message will also be
 * httpd_d'd, so no need to add extra lines of code for that.
 *
 * Note that this function is not re-entrant.
 *
 * Note that at most HTTPD_MAX_ERROR_STRING characters will be stored.
 *
 * Note: no need to have a \r\n on the end of the error message.
 */
#define HTTPD_MAX_ERROR_STRING 256
void httpd_set_error(const char *fmt, ...);

int handle_message(char *msg_in, int msg_in_len, int conn);
int httpd_parse_hdr_main(const char *data_p, httpd_request_t *req_p);
int httpd_handle_message(int conn);

/* Various Defines */
#ifndef NULL
#define NULL 0
#endif

int httpd_wsgi(httpd_request_t *req_p);

httpd_ssifunction httpd_ssi(char *);
int httpd_ssi_init(void);
int htsys_getln_soc(int sd, char *data_p, int buflen);

void httpd_parse_useragent(char *hdrline, httpd_useragent_t *agent);

int httpd_send_last_chunk(int conn);

enum {
	HTTP_404,
	HTTP_500,
	HTTP_505,
};

int httpd_send_error(int conn, int http_error);

bool httpd_is_https_active( void );
#endif				/* __HTTPD_PRIV_H__ */
