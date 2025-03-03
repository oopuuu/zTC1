/**
 ******************************************************************************
 * @file    httpd.c
 * @author  QQ DING
 * @version V1.0.0
 * @date    1-September-2015
 * @brief   The main HTTPD server thread and its initialization.
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

/*! \file httpd.h
 *  \brief HTTP Web Server
 *
 * HTTP Web Server can serve static pages from a file system, dynamic content
 * using a server-side include (SSI) capability, and dynamic content using
 * custom web service gateway interface (WSGI) handlers.  This documentation
 * describes:
 * -# How to initialize, start, stop, and shutdown the server.
 * -# How static files are served.
 * -# How to implement WSGI handlers.
 * -# How the SSI directives "file" and "virtual" are processed.
 * Several limitations that may be unexpected by programmers accustomed to using
 * more sophisticated web servers and frameworks are also discussed.
 *
 * \section httpd_usage Usage
 *
 * httpd must be initialized by calling \ref httpd_init.  After \ref httpd_init
 * returns WM_SUCESS, \ref httpd_start and \ref httpd_stop can be called to
 * start and stop the server respectively.  Finally, to perform a final cleanup,
 * \ref httpd_shutdown should be called.  After a successful call to
 * httpd_start, the server is listening for connections on \ref HTTP_PORT or
 * \ref HTTPS_PORT.
 *
 * The WSGI and SSI handlers can be registered by calling the functions
 * httpd_register_wsgi_handler() and httpd_register_ssi().
 *
 * When a client request is made, httpd first checks if the requested URI
 * matches any registered WSGI handlers (see below). If a matching handler is
 * found it gets executed and the processing is terminated. If a file matching
 * the requested URI is found, it is served.  If that file is a .shtml file, it
 * is processed for directives as described in the file handling section.
 *
 * \section wsgi Web Services Gateway Interface (WSGI)
 *
 * The Web Services Gateway Interface (WSGI) allows developers to take complete
 * control over how HTTP requests are processed. When a request arrives, its
 * request method (or type) and URI are parsed. A table of all registered WSGI
 * handlers is queried for any matches. If an exact match is obtained, the
 * control is transferred to the corresponding WSGI handler. If the
 * WSGI handler returns WM_SUCCESS, no further processing is performed.
 * In case
 * A 500 Internal Server Error is returned to the client:
 * -# When any other value than WM_SUCCESS is returned by the WSGI handler.
 *
 * A 404 Not Found Error is returned to the client:
 * -# If no handler (including the file system) matches the incoming URI
 * -# If incoming request type is not supported by that WSGI
 *
 * A WSGI handler typically is executed in response to a HTTP GET or
 * HTTP POST operation. In case of HTTP GET, the handler fetches some
 * data from the device, formats it as desired and sends it to the
 * requester. In case of HTTP POST, the handler reads the data that is
 * sent along with the POST request and takes some action on the
 * device. The WSGI handlers can make use of some of the common httpd
 * helper functions to perform tasks pertaining to the
 * sending/receiving of the data.
 *
 * A simple WSGI HTTP GET handler is shown below:
 * Example: JSON Time Get
 *
 * \code
 * int sys_get_time(httpd_request_t *req)
 * {
 *	char content[30];
 *	time_t time_val;
 *
 *	some_api_to_fetch_time(&time_val)
 *	snprintf(content, sizeof(content), "{\"epoch\":%d}", time_val);
 *
 *	return httpd_send_response(req, HTTP_RES_200
 *		, content, strlen(content), HTTP_CONTENT_JSON_STR);
 * }
 * \endcode
 *
 * A simple WSGI HTTP POST handler is shown below:
 * Example: JSON Time Set
 *
 * \code
 * int sys_set_time(httpd_request_t *req)
 * {
 *	char sys_time_req[30];
 *	int ret;
 *	time_t time_val;
 *
 *	ret = httpd_get_data_json(req, sys_time_req,
 *		sizeof(sys_time_req), &obj);
 *	if (ret < 0) {
 *		wmprintf("Failed to get post request data");
 *		return ret;
 *	}
 *	json_get_val_int(&obj, "epoch", (int *)&time_val);
 *
 *	ret = Set the time using the value time_val func()
 *	if (ret == WM_SUCCESS)
 *		return httpd_send_response(req, HTTP_RES_200
 *		   , HTTPD_JSON_SUCCESS, strlen(HTTPD_JSON_SUCCESS),
 *		   HTTP_CONTENT_JSON_STR);
 *	else
 *		return httpd_send_response(req, HTTP_RES_200
 *		   , HTTPD_JSON_ERROR, strlen(HTTPD_JSON_ERROR),
 *		   HTTP_CONTENT_JSON_STR);
 * }
 * \endcode
 *
 * The above examples being simplest examples use the most convenient
 * functions, namely httpd_send_response() and httpd_get_data_json()
 * for most of their tasks. An advanced user may prefer to have more
 * flexibility by using functions that provide a greater control. To
 * enable this, the exact set of steps that are performed by both
 * these functions is documented below.
 *
 * - httpd_send_response()
 *   -# Purge HTTP headers (valid only for GET request) : httpd_purge_headers()
 *   -# Send response status (200 OK/404 Not Found etc.): httpd_send()
 *   -# Send the default headers from \ref httpd_hdr_field_sel_t :
 *      httpd_send_default_headers()
 *   -# Send the Content-Type of the response : httpd_send_header()
 *   -# Send the header terminator : httpd_send_crlf()
 *   -# Send the chunked data : httpd_send_chunk()
 *   -# Send the last chunk : httpd_send_chunk() with length = 0
 *   .
 * .
 *
 * - httpd_get_data_json()
 *   -# httpd_get_data()
 *     -# Parse the received headers : httpd_parse_hdr_tags()
 *     -# Receive the POST data : httpd_recv()
 *     .
 *   -# Initialize the JSON object : json_object_init()
 *   .
 * .
 *
 * \section files Static File Handling
 *
 * Static files can be served from a filesystem. A WSGI handler can be
 * registered at '/'. This handler can call httpd_handle_file() for dealing with
 * file serving requests. If the fs contains a file called /404.html, this file
 * will be returned when httpd receives a request for a URL that does not have a
 * registered handler or page.  The following table summarizes the Content-Types
 * with which files of various file extensions are served:
 *
 | Extension | Content-Type |
 | ----:---- | --------- |
 | .html | text/html (with charset=iso8859-1) |
 | .shtml | same as .html but with caching disabled |
 | .css | text/css |
 | .png | image/png |
 | .gif | image/gif |
 | .jpg | image/jpeg |
 | [none] | application/octet-stream |
 | [other] | text/plain |
 *
 * \subsection gzip_handling Compressed Files
 *
 * If the request for a file <em>abc.html</em> is received, the web server first
 * tries to check if a compressed version of this file <em>abc.html.gz</em>
 * exists. If found, this compressed version is served. The
 * <em>Content-Encoding</em> field is set to \e gzip for these requests, so that
 * the HTTP clients can handle it properly.
 *
 * \section ssi Server Side Include (SSI)
 *
 * Server Side Include (SSI) directives are mark-up snippets in .shtml files
 * that are detected by HTTPD and replaced with some other content.  Two SSI
 * directives are supported: file and virtual.
 *
 * When the file directive is encountered, it is replaced by the contents of a
 * file.  For example, suppose you have the following .shtml files in your
 * filesystem:
 *
 * index.shtml:
 * \code
 * Hello World
 * <!--#include file=footer.html-->
 * \endcode
 *
 * footer.html:
 * \code
 * <b>Copyright 2008-2013 your name here<\b>
 * \endcode
 *
 * When index.shtml is requested, the SSI file directive will be replaced by
 * the contents of footer.html, and the following will be served:
 * index.shtml:
 * \code
 * Hello World
 * <b>Copyright 2008-2013 your name here<\b>
 * \endcode
 *
 * Much like a file SSI directive, a virtual SSI directive can also be used in
 * .shtml files.  They look something like this:
 *
 * \code
 * <!--#include virtual=foo 1 2 3-->
 * \endcode
 *
 * When a virtual SSI directive is encountered, control is passed to the SSI
 * handler that is registered under the specified name ("foo" in this example).
 * The foo handler would be passed the string "1 2 3" as its argument, along
 * with the open socket to which the response is being sent, and an \ref
 * httpd_request_t that can be queried for the request type (POST or GET) and
 * any url-encoded form data (POST only).  The handler is expected to send
 * complete http chunks to the socket (see \ref httpd_send_chunk).
 *
 * When using .shtml files and SSI directives, keep the following tips and
 * caveats in mind:
 *
 * - See \ref HTTPD_SSI_CALL, \ref httpd_register_ssi, struct \ref
 *   httpd_ssifunction for additional documentation and pointers.
 *
 * - Placing SSI directives in a file that does not have the .shtml extension
 *   has no effect; the file will be served as-is.
 *
 * - Each SSI directive should be on its own line with no other mark up.  If
 *   there is any other markup, it will be ignored and will not be served to
 *   the client.
 *
 * - The SSI directives must start with either "<!--#include file=" or
 *   "<!--#include virtual=".  They must not contain any extraneous spaces.
 *
 * - File names passed to the "file=" directive must not contain spaces.
 *
 * - If a virtual directive is encountered for which there is no registered
 *   handler, nothing will be output and the page processing will continue.
 *
 * \section other Other Considerations
 *
 * HTTPD operates in a highly memory constrained environment.  Accordingly, it
 * allocates a limited amount of memory for various elements such as the maximum
 * URL length, maximum number of form elements, etc. Developers writing the
 * handlers and web applications must consider these constraints. Details
 * regarding these are discussed in the Defines section below.  If necessary,
 * these values can be changed and HTTPD can be rebuilt.
 *
 * \section httpd_call_graph HTTPD Call Graph
 *
 * The HTTPD library fundamentally uses the functions httpd_send() and
 * httpd_recv() to send and receive data over the HTTP
 * socket. Additional functionality is built on top of these functions
 * in a hierarchical manner for the user's convenience. The following
 * callgraph shows the layering of these calls.
 *
 * @cond uml_diag
 *
 * @startuml{httpd_wsgi.png}
 * [*] --> WSGI_Handler_Function
 * WSGI_Handler_Function --> httpd_send_response : GET/HEAD/POST Handler
 * WSGI_Handler_Function --> httpd_get_data_json : POST Handler
 * httpd_get_data_json --> httpd_get_data : Fetch the POST data
 * httpd_get_data_json --> json_object_init : JSON object init
 * httpd_get_data --> httpd_parse_hdr_tags : Parse the HTTP headers
 * httpd_get_data --> httpd_recv : Receive the data
 * httpd_parse_hdr_tags --> httpd_recv
 * httpd_send_response --> httpd_purge_headers : Purge HTTP headers
 * httpd_purge_headers --> httpd_recv : Receive the headers and purge them
 * httpd_send_response --> httpd_send : Send response status
 * httpd_send_response --> httpd_send_default_headers : Send default headers
 * httpd_send_response --> httpd_send_header : Send the Content-Type header
 * httpd_send_response --> httpd_send_crlf : Send header end indicator
 * httpd_send_response --> httpd_send_chunk : Send chunked data
 * httpd_send_response --> httpd_send_chunk : Send last chunk (len = 0)
 * httpd_send_default_headers --> httpd_send
 * httpd_send_header --> httpd_send
 * httpd_send_crlf --> httpd_send
 * httpd_send_chunk --> httpd_send_chunk_begin
 * httpd_send_chunk --> httpd_send
 * httpd_send_chunk --> httpd_send_crlf
 * httpd_send_chunk_begin --> httpd_send
 *
 * @enduml
 * @endcond
 *
 ******************************************************************************/


/** @defgroup MICO_Middleware_Interface  MiCO Middleware APIs
  * @brief Provide MiCO Middleware Programming APIs.
  * @{
  */

/** @addtogroup MICO_Middleware_Interface
  * @{
  */

/** @defgroup MICO_HTTP_Web_Server  MiCO HTTP Web Server
  * @brief Provide MiCO HTTP Web Server APIs.
  * @{
  */


#ifndef _HTTPD_H_
#define _HTTPD_H_

#include "common.h"
//#include <json.h>

/** Port on which the httpd listens for non secure connections. */
#define HTTP_PORT 80

/** Port on which the httpd listens for secure connections. */
#define HTTPS_PORT 443

/** Do not perform an exact match of the URI, but ensure that only the prefix is
 * correct. This is required when the registered URI is /nodes/ while the
 * handler wishes to match with /nodes/0/, /nodes/1/ etc.
 */
#define APP_HTTP_FLAGS_NO_EXACT_MATCH   1

/** This REQ_TYPE should not be used externally. */
#define HTTPD_REQ_TYPE_UNKNOWN 0

/** This request type indicates that incoming http request is of type POST. */
#define HTTPD_REQ_TYPE_POST 1

/** This request type indicates that incoming http request is of type GET.
 *
 *  \note GET does not support URL-encoded tag/val pairs.
 */
#define HTTPD_REQ_TYPE_GET 2

/** This request type indicates that incoming http request is of type PUT. */
#define HTTPD_REQ_TYPE_PUT 4

/** This request type indicates that incoming http request is of type DELETE. */
#define HTTPD_REQ_TYPE_DELETE 8

/** This request type indicates that incoming http request is of type HEAD. */
#define HTTPD_REQ_TYPE_HEAD 16

/** HTTPD Error Codes */
enum wm_httpd_errno {
	WM_E_HTTPD_ERRNO_BASE = 0,
	/** Error parsing filename in HTTP Header */
	WM_E_HTTPD_HDR_FNAME,
	/** HTTPD doesn't support the http version */
	WM_E_HTTPD_NOTSUPP,
	/** Post data too long */
	WM_E_HTTPD_TOOLONG,
	/** Data read failed */
	WM_E_HTTPD_DATA_RD,
	/** SSI Table full */
	WM_E_HTTPD_SSI_MAX,
	/** Handler generated a HTTP 404 File Not Found error */
	WM_E_HTTPD_HANDLER_404,
	/** Handler generated a HTTP 400 Bad Request error */
	WM_E_HTTPD_HANDLER_400,
	/** Handler generated a HTTP 500 Internal Server error */
	WM_E_HTTPD_HANDLER_500,
	/** No handler was found */
	WM_E_HTTPD_NO_HANDLER,
};

/** This value is returned when a WSGI handler is successful to indicate that
 * all processing has been completed.
 */
#define HTTPD_DONE 1

/** Maximum received message length
 *
 *  This value is essentially the longest single line that the HTTPD can
 *  handle.  This limits programmers and users in some interesting ways.  For
 *  example:
 *  -# This is the maximum length of any url-encoded data sent as the body of a
 *     POST request.
 *     If this limit is exceeded, the server will generate a 500 Internal
 *     Server Error response.
 *     Applications that require a higher capacity for form data must implement
 *     their own custom handler to process form data or tune this value.
 *  -# This is the maximum length of a header line that can be processed by
 *     \ref httpd_parse_hdr_tags.  Header lines that exceed this
 *     length will be truncated before being analyzed.  Applications that
 *     require more capacity must implement their own custom handler to process
 *     HTTP header data.
 *  -# This is the maximum length of a single line in a .shtml script.
 *
 *  Developers must ensure that these limits are addressed to avoid unexpected
 *  errors in the field.
 *
 *  \note This value does not include any NULL termination required internally
 *  by the httpd.
 */
#define HTTPD_MAX_MESSAGE 512

/** Maximum URI length
 *
 * This is the maximum supported URI length.  For example, if a client sends a
 * GET /foobar.html, the length of "/foobar.html" must be less than this value.
 * Programmers are obliged to ensure that none of the URIs or HTTPD file names
 * exceed this limit.  If URIs do end up exceeding this limit, a 500 Internal
 * Server Error response will be generated.
 *
 * \note This value does not include any NULL termination required internally
 * by the HTTPD.  However it does include the leading "/".
 */
#define HTTPD_MAX_URI_LENGTH 64


/** Maximum length of the value portion of a tag/value pair
 *
 *  If this length is exceeded, the value will be truncated to this length and
 *  no error will be generated.  If debugging is enabled, however, a warning
 *  will be written to the console.
 *
 *  \note Truncating the user input passed by client may result in unexpected
 *  behavior depending on how a custom handler uses that data.  Programmers
 *  should take steps to validate and/or limit user input using javascript,
 *  input tag properties, or other client-side means.
 *
 *  \note This value does not include the NULL termination of the value.
 *
 */
#define HTTPD_MAX_VAL_LENGTH 64

/** Maximum length of Content-Type
 *
 * httpd_parse_hdr_tags() will parse the headers and store the value of
 * the Content-Type. The maximum value of this field has been arrived
 * keeping in mind the need to support the following type of Content-Type
 * header for working with multipart/form-data for file uploads:
 * Content-Type: multipart/form-data; boundary=\<delimiter_max_length_70\>
 * The length of this line is 104 bytes
 */
#define HTTPD_MAX_CONTENT_TYPE_LENGTH 128

#define ISO_nl      0x0a
#define ISO_cr      0x0d
#define ISO_tab     0x09
#define ISO_space   0x20
#define ISO_bang    0x21
#define ISO_quot    0x22
#define ISO_percent 0x25
#define ISO_period  0x2e
#define ISO_slash   0x2f
#define ISO_colon   0x3a

#define HTTPD_JSON_ERR_MSG    "{\"error_msg\":\"%s\"}"
#define HTTPD_JSON_RETURN_MSG  "{\"return_msg\":\"%s\"}"
#define HTTPD_JSON_ERROR  "{\"error\": -1}"
#define HTTPD_JSON_SUCCESS "{\"success\": 0}"
#define HTTPD_TEXT_ERROR  "error -1\r\n"
#define HTTPD_TEXT_SUCCESS "success\r\n"

/** Content-Type: application/json */
#define HTTP_CONTENT_JSON_STR "application/json"
/** Content-Type: text/xml */
#define HTTP_CONTENT_XML_STR "text/xml"
/** Content-Type: text/hxml */
#define HTTP_CONTENT_HTML_STR "text/html"
/** Content-Type: text/css */
#define HTTP_CONTENT_CSS_STR "text/css"
/** Content-Type: application/x-javascript */
#define HTTP_CONTENT_JS_STR "application/javascript"
/** Content-Type: image/png */
#define HTTP_CONTENT_PNG_STR "image/png"
/** Content-Type: text/plain */
#define HTTP_CONTENT_PLAIN_TEXT_STR "text/plain"

/** HTTP Response: 200 OK */
#define HTTP_RES_200 "HTTP/1.1 200 OK\r\n"
/** HTTP Response: 301 Moved Permanently */
#define HTTP_RES_301 "HTTP/1.1 301 Moved Permanently\r\n"
/** HTTP Response: 304 Not Modified */
#define HTTP_RES_304 "HTTP/1.1 304 Not Modified\r\n"
/** HTTP Response: 400 Bad Request */
#define HTTP_RES_400 "HTTP/1.1 400 Bad Request\r\n"
/** HTTP Response: 404 Not Found */
#define HTTP_RES_404 "HTTP/1.1 404 Not Found\r\n"
/** HTTP Response: 505 HTTP Version Not Supported */
#define HTTP_RES_505 "HTTP/1.1 505 HTTP Version Not Supported\r\n"

/* Are these \r\n required in here? The send chunk will append its own set of
 * \r\n, thus making it two \r\n.
 */

#define HTTPD_XML_ERROR \
	"<?xml version=\"1.0\" encoding=\"US-ASCII\"?><error>%d</error>\r\n"
#define HTTPD_XML_SUCCESS \
	"<?xml version=\"1.0\" encoding=\"US-ASCII\"?><success>0</success>\r\n"

/** Send carriage return and new line characters on the socket \_sock\_ */
#define httpd_send_crlf(_sock_) httpd_send(_sock_, "\r\n", 2);

/** httpd_useragent_t reports the product and version of the client accessing
 * the httpd.
 */
typedef struct  {
	/** Product of the client */
	char product[HTTPD_MAX_VAL_LENGTH + 1];	/* +1 for null termination */
	/** Version of the client */
	char version[HTTPD_MAX_VAL_LENGTH + 1];	/* +1 for null termination */
} httpd_useragent_t;

struct httpd_wsgi_call;

/** Request structure representing various properties of an HTTP request
 */
typedef struct {
	/** HTTP Request type: GET, HEAD, POST, PUT, DELETE */
	int type;
	/** The incoming URI */
	/* +1 is for null termination */
	char filename[HTTPD_MAX_URI_LENGTH + 1];
	/** The socket of the incoming HTTP Request */
	int sock;
	/** Indicator notifying whether the HTTP headers are parsed or not */
	unsigned char hdr_parsed;
	/** The number of data bytes not yet parsed */
	int remaining_bytes;
	/** The size of data in the incoming HTTP Request */
	int body_nbytes;
	/** Pointer to the corresponding wsgi structure */
	const struct httpd_wsgi_call *wsgi;
	/** Set to 1 if chunked data is used */
	unsigned char chunked;
	/** User-agent: Product and Version */
	httpd_useragent_t agent;
	/** The content type of the incoming HTTP Request */
	char content_type[HTTPD_MAX_CONTENT_TYPE_LENGTH];
	/** True if "If-None-Match" header is present in the incoming
	 * HTTP Request */
	bool if_none_match;
	/** Used for storing the etag of an URI */
	unsigned etag_val;
} httpd_request_t;

/** @brief Initialize the httpd
 *
 *  @note  This function must be called before any of the other API functions.  If any
 *  of the initialization steps fail, the function will fail.
 *
 *  @return WM_SUCCESS if successful
 *  @return -WM_FAIL otherwise
 */
int httpd_init(void);

/** @brief  Shutdown the httpd
 *
 *  @note  This function is the opposite of httpd_init().  It cleans up any
 *  resources used by the httpd.  If the httpd thread is running (i.e.,
 *  httpd_stop has not been called), it will be halted.
 *
 *  @return WM_SUCCESS   :if successful
 *  @return -WM_FAIL     :otherwise
 *
 */
int httpd_shutdown(void);

/** @brief   Start the httpd
 *
 *  @note    Upon WM_SUCCESS, the user can expect that the server is running and
 *  ready to receive connections.  httpd_stop() should be called to stop the
 *  server.  A return value of -WM_FAIL generally indicates that the thread
 *  has already been started.
 *
 *  @return   WM_SUCCESS      :if successful
 *  @return   WM_FAIL         :otherwise
 */
int httpd_start(void);

/** @brief    Stop the httpd
  *
  * @note     This function is the opposite of httpd_start().  It stops the httpd.  If
  *  WM_SUCCESS is returned, the server has been stopped and will no longer
  *  accept connections.  -WM_FAIL is returned if the httpd is uninitialized, or
  *  if an internal error occurs (which is not expected).  Note that this
  *  function forces the httpd thread to stop and closes any open sockets. If
  *  a handler is being executed during this time, it will be terminated.
  *
  * @return   WM_SUCCESS      : if successful
  * @return   WM_FAIL         : otherwise
  */
int httpd_stop(void);

/** @brief    Check if httpd is running
 *
 *  @return   0              : if httpd is not running
 *  @return   non-zero       : if it is running
 */
int httpd_is_running(void);

/** @brief  Helper function to send a buffer over a connection.
 *
 *  @note  WSGI handlers will often want to send data over a connection.  This can be
 *  achieved with the regular send() system call, however that call may not send
 *  all of the data.  This helper function calls send in a loop repeatedly until
 *  all data has been sent or until an error occurs. This function internally
 *  calls tls_send() if TLS is enabled, else it calls send()
 *
 *  @param[in] sock         The socket on which the data is to be sent
 *  @param[in] buf          Pointer to the buffer
 *  @param[in] len          Length of the buffer
 *
 *  @return  WM_SUCCESS     : if successful
 *  @return  -WM_FAIL       : otherwise
 *
 *  \see httpd_send_chunk
 */
int httpd_send(int sock, const char *buf, int len);

/** @brief  Helper function to send an HTTP chunk of data over a connection
 *
 *  @note   This function sends HTTP chunked data. It includes sending the chunked
 *  header (which consists of the chunk size in ascii followed by \\r\\n
 *  characters), then the actual http data and at the end chunk end indicator
 *  (\\r\\n characters)
 *  If the len is passed as 0 to this function, it sends the last chunk i.e.
 *  0\\r\\n
 *
 *  Note that while using chunked data, HTTP Header "Transfer Encoding: Chunked"
 *  should be sent in the response
 *
 *  Send either an HTTP chunk of data or the terminating HTTP chunk.
 *
 *  @param[in]  sock        The socket on which the data is to be sent
 *  @param[in]  buf         Pointer to the buffer or NULL for last chunk
 *  @param[in]  len         Length of the buffer or 0 for last chunk
 *
 *  @return  WM_SUCCESS    : if successful
 *  @return  -WM_FAIL      : otherwise
 *
 */
int httpd_send_chunk(int sock, const char *buf, int len);

/** @brief  Helper function to send a HTTP chunk header
 *
 *  @note  This function is not required when \ref httpd_send_chunk is used.
 *  \ref httpd_send_chunk internally calls this function
 *
 *  This function can be followed by a call to \ref httpd_send to send the
 *  actual content followed by sending the chunk end indicator (\\r\\n)
 *
 *  @param[in] sock           The socket over which chunk header is to be sent
 *  @param[in] size           Length of the chunk to be sent
 *
 *  @return    WM_SUCCESS     :if successful
 *  @return   - WM_FAIL       :otherwise
 *
 */
int httpd_send_chunk_begin(int sock, int size);

/** Structure representing a WSGI handler
 *
 */
struct httpd_wsgi_call {
	/** URI of the WSGI */
	const char *uri;
	/** Indicator for HTTP headers to be sent in the response*/
	int hdr_fields;
	/** Flag indicating if exact match of the URI is required or not */
	int http_flags;
	/** HTTP GET or HEAD Handler */
	int (*get_handler) (httpd_request_t *req);
	/** HTTP POST Handler */
	int (*set_handler) (httpd_request_t *req);
	/** HTTP PUT Handler */
	int (*put_handler) (httpd_request_t *req);
	/** HTTP DELETE Handler */
	int (*delete_handler) (httpd_request_t *req);
};


/** @brief  Register a WSGI handler
 *
 *  @note   WSGI handlers declared must be registered with the
 *  httpd by calling this function.
 *
 *  @param[in]   wsgi_call       pointer to a struct httpd_wsgi_call. The memory
 *  location pointed to by this pointer should be available until
 *  the httpd handler is unregistered.
 *
 *  @return  WM_SUCCESS         :if successful
 *  @return -WM_FAIL            :otherwise
 */
int httpd_register_wsgi_handler(struct httpd_wsgi_call *wsgi_call);

/** @brief Register a list of WSGI handlers
 *
 *  @note  WSGI handler list declared must be registered with the
 *  httpd by calling this function.
 *
 *  @param[in] wsgi_call_list     pointer to a list of struct httpd_wsgi_call. The
 *  memory location pointed to by this pointer should be available until
 *  the httpd handler list is unregistered.
 *  @param[in] handler_cnt number of WSGI handlers in wsgi_call_list
 *
 *  @return WM_SUCCESS         :if successful
 *  @return -WM_FAIL           :otherwise
 */
int httpd_register_wsgi_handlers(struct httpd_wsgi_call *wsgi_call_list,
					int handler_cnt);
/** @brief  Unregister a WSGI handler
 *
 *  @note  This call unregisters a WSGI handler.
 *
 *  @param[in] wsgi_call     pointer to a struct httpd_wsgi_call
 *
 *  @return  WM_SUCCESS     :if successful
 *  @return -WM_FAIL        :otherwise
 */
int httpd_unregister_wsgi_handler(struct httpd_wsgi_call *wsgi_call);

/** @brief  Unregister a list of WSGI handlers
 *
 *  @note  WSGI handler list declared can be unregistered with the
 *  httpd by calling this function.
 *
 *  @param[in] wsgi_call_list      list of struct httpd_wsgi_call
 *  @param[in] handler_cnt         number of WSGI handlers in wsgi_call_list
 *
 *  @return  WM_SUCCESS           :if successful
 *  @return  -WM_FAIL             :otherwise
 */
int httpd_unregister_wsgi_handlers(struct httpd_wsgi_call *wsgi_call_list,
					int handler_cnt);

/** @brief  Maximum length of virtual SSI arguments
 *
 *  @note  The supported format for virtual SSI directives is:
 *
 *  @code
 *  <!--#include virtual="foo arg1 arg2 arg3"-->
 *  @endcode
 *
 *  or
 *
 *  @code
 *  <!--#include virtual=foo arg1 arg2 arg3-->
 *  @endcode
 *
 *  HTTPD_MAX_SSI_ARGS is the maximum length for "arg1 arg2 arg3" (not
 *  including the quotes).  If this length is exceeded, the arguments will be
 *  truncated.
 *
 *  @note This does not include any null-termination required internally by the
 *  httpd or the virtual SSI handlers.
 *
 *  @note whitespace between the function name (e.g., foo) and the first
 *  argument (e.g. arg1) is ignored and not passed to handlers.
 *
 *  @note Double quotes are reserved and should only be used as described in
 *  the above examples.
 */
#define HTTPD_MAX_SSI_ARGS 32

/** @brief  SSI virtual include handler function prototype
 *
 *  @param[in]    req   The incoming HTTP request \ref httpd_request_t.  Minimally,
 *  the type and filename members will be initialized.  Handlers are not
 *  expected to write to this struct.
 *
 *  @param[in]   args   Null-terminated string that is the arguments passed to the
 *  handler in the include directive.
 *
 *  @param[in]   sock   Socket to which the handler output should be sent.
 *
 *  @param[in]  scratch  A scratch buffer that can be used by the handler.  It is
 *  @ref HTTPD_MAX_MESSAGE bytes long.
 *
 *  @note  SSI virtual include handlers should only send complete chunks i.e.
 *  they should send any data using  \ref httpd_send_chunk, not \ref httpd_send
 *  or any lower-level system calls.  These handlers should also not send the
 *  final chunk i.e. they should not invoke httpd_send_chunk with a length of 0.
 *
 */
typedef int (*httpd_ssifunction) (const httpd_request_t *req, const char *args,
				  int sock, char *scratch);


/** @brief  Opaque structure representing a virtual SSI handler
 *
 *  @note  Users should use the macro \ref HTTPD_SSI_CALL to initialize this struct
 *  statically.
 */
struct httpd_ssi_call {
	/** The name of the function, used in the script file */
	const char *name;
	/** The pointer to the function that implements the handler */
	const httpd_ssifunction function;
};

/** @brief  httpd virtual SSI handler declaration
 *
 *  @note  This macro is used for declaring a virtual SSI handler struct \ref
 * httpd_ssi_call. The function is then added to the list of handlers with \ref
 * httpd_register_ssi, and removed with \ref httpd_unregister_ssi function.
 *
 * @param name     Name of the \ref httpd_ssi_call structure object
 *
 * @param str      The string name of the function, used in the script file
 *
 * @param function A pointer to the function that implements the handler
 *
 * @hideinitializer
 */
#define HTTPD_SSI_CALL(name, str, function) \
struct httpd_ssi_call name = {str, function}

/** Maximum number of virtual SSI handlers that are supported.
 */
#define MAX_REGISTERED_SSIS 32

/** @brief Register a virtual SSI handler
 *
 *  @note virtual SSI handlers declared with \ref HTTPD_SSI_CALL must be
 *  registered with the httpd by calling this function.
 *
 *  @param[in] ssi_call      pointer to the httpd_ssi_call to register
 *
 *  @return   WM_SUCCESS           :if successful
 *  @return   -WM_E_HTTPD_SSI_MAX  :if the maximum number of supported handlers has
 *  been reache
 *  @return  -WM_FAIL              :if the handler is already registered
 *
 *  @note All SSI handlers are cleared when httpd_init is called.
 */
int httpd_register_ssi(struct httpd_ssi_call *ssi_call);

/** @brief Unregister a virtual SSI handler
 *
 *  @param[in]  ssi_call pointer to the httpd_ssi_call to unregister
 *
 *  @note  All SSI handlers are cleared when httpd_init is called.
 */
void httpd_unregister_ssi(struct httpd_ssi_call *ssi_call);

/** @brief Register and Enable httpd cli-commands
 *
 *  @return WM_SUCCESS if successful
 *  @return -WM_FAIL otherwise
 *
 *  @note This function can be called by the application
 *  after the httpd is initialized.
 */

/* no usage */
int httpd_cli_init(void);

/** HTTP Content types
 */
enum http_content_type {
	/** Content-Type: text/plain */
	HTTP_CONTENT_PLAIN_TEXT,
	/** Content-Type: application/json */
	HTTP_CONTENT_JSON,
	/** Content-Type: text/xml */
	HTTP_CONTENT_XML,
	/** Content-Type: text/html */
	HTTP_CONTENT_HTML,
	/** Content-Type: image/jpeg */
	HTTP_CONTENT_JPEG,
};

/** HTTP Headers to be sent in the HTTP response
 */
typedef enum {
	/** "Server: Marvell-WM\r\n" */
	HTTPD_HDR_ADD_SERVER                    = 0x0001,
	/** "Connection: keep-alive\r\n" */
	HTTPD_HDR_ADD_CONN_KEEP_ALIVE           = 0x0002,
	/** "Connection: close\r\n" */
	HTTPD_HDR_ADD_CONN_CLOSE                = 0x0004,
	/** "Transfer-Encoding: chunked\r\n" */
	HTTPD_HDR_ADD_TYPE_CHUNKED              = 0x0008,
	/** "Cache-Control: no-store, no-cache, must-revalidate\r\n" */
	HTTPD_HDR_ADD_CACHE_CTRL                = 0x0010,
	/** "Cache-Control: post-check=0, pre-check=0\r\n" */
	HTTPD_HDR_ADD_CACHE_CTRL_NO_CHK         = 0x0020,
	/** "Pragma: no-cache\r\n" */
	HTTPD_HDR_ADD_PRAGMA_NO_CACHE           = 0x0040,
} httpd_hdr_field_sel_t;

/** Default HTTP headers from \ref httpd_hdr_field_sel_t */
#define HTTPD_DEFAULT_HDR_FLAGS (HTTPD_HDR_ADD_SERVER | \
		HTTPD_HDR_ADD_CONN_CLOSE | HTTPD_HDR_ADD_TYPE_CHUNKED)

/** @brief Send HTTP Status Headers
 *
 *  @note  This function sends the corresponding HTTP headers on the socket.
 *
 *  @param[in] sock          the socket to send headers on
 *  @param[in] stat_code     the status code WM_SUCCESS, -WM_E_HTTPD_HANDLER_404 etc.
 *  @param[in] content_type  the content type \ref http_content_type
 *
 *  @return   WM_SUCCESS     :if successful
 *  @return   -WM_FAIL       :otherwise
 */
int httpd_send_hdr_from_code(int sock, int stat_code,
			     enum http_content_type content_type);


/** @brief Validate an incoming request with a registered handler
 *
 *  @note  This function is used to match the incoming request URI against the one that
 *  is registered with HTTPD
 *
 *  @param[in] req_uri        URI of the incoming HTTP request
 *  @param[in] uri            URI of an handler registered with HTTPD
 *  @param[in] flags          Flag indicating if exact match is required or not \ref
 *  APP_HTTP_FLAGS_NO_EXACT_MATCH
 *
 *  @return    WM_SUCCESS    :if successful
 *  @return    -WM_FAIL      :otherwise
 */
int httpd_validate_uri(char *req_uri, const char *uri, int flags);

/** @brief Helper function to receive data over a socket
 *
 *  @note  This function abstracts the TCP/IP recv call. We need the abstraction
 *  because recv could be through TLS connection in which case we do not
 *  call the TCP/IP stack 'recv' function directly.
 *
 *  This API is only for the httpd handlers who have been passed the socket
 *  descriptors by httpd.
 *
 *  @param[in]  sock     The socket on which the data is to be received
 *  @param[out] buf      The buffer in which data will be received
 *  @param[in]  n        Number of bytes to be received
 *  @param[in] flags     Same values as that of flag parameter in man page of recv
 *
 *  @return  WM_SUCCESS   :if successful
 *  @return  -WM_FAIL     :otherwise
 */
int httpd_recv(int sock, void *buf, size_t n, int flags);

/** @brief Send the entire HTTP response
 *
 *  @note  This is a helper function which can be used by the WSGI handlers
 *  to send the entire HTTP response.
 *  This function first purges the incoming headers (this step is valid only for
 *  HTTP GET or HEAD request.) Then it sends the first line of the response i.e.
 *  200 OK, 404 Not Found etc. as specified by the user. Further, it sends the
 *  default HTTP headers as per the value of hdr_fields parameter of that WSGI.
 *  After that, it sends the Content-Type HTTP header using the content_type
 *  sent by the user. If the request type is HTTP HEAD, only the headers are
 *  sent out and this function terminates, else data provided by the user
 *  (chunked/non chunked data) follows.
 *
 *  Note that this API is optional and WSGI handlers may prefer to
 *  send the response as per their wish using other HTTPD APIs.
 * 
 *  @param[in] req          The incoming HTTP request \ref httpd_request_t
 *  @param[in] first_line   First line of the response. for e.g.: To send 200 OK
 *  it will be: "HTTP/1.1 200 OK\r\n"
 *  @param[in] content      The data to be sent
 *  @param[in] length       The length of data to be sent
 *  @param[in] content_type The content type of the response. for e.g.:
 *  To send JSON response it will be: "application/json"
 *
 *  @return WM_SUCCESS     :if successful
 *  @return -WM_FAIL       :otherwise
 */
int httpd_send_response(httpd_request_t *req, const char *first_line,
		char *content, int length, const char *content_type);

/** @brief Send HTTP response 301: Moved Permanently
 *
 *  @note  This is a helper function which can be used by the WSGI handlers to send
 *  HTTP response HTTP 301 Moved Permanently. This function first purges the
 *  incoming headers (this step is valid only for HTTP GET or HEAD request.)
 *  Then it sends the first line of the response i.e. 301 Moved Permanently.
 *  Further, it sends the default HTTP headers as per the value of hdr_fields
 *  parameter of that WSGI. After that, it sends the HTTP header Location which
 *  is the new link for redirection. Then, it sends the Content-Type HTTP header
 *  using the content_type sent by the user. If the request type is HTTP HEAD,
 *  only the headers are sent out and this function terminates, else data
 *  provided by the user (chunked/non chunked data) follows.
 *
 *  Note that this API is optional and WSGI handlers may prefer to
 *  send the response as per their wish using other HTTPD APIs.
 *
 *  @param[in] req          The incoming HTTP request \ref httpd_request_t
 *  @param[in] location     The new link for redirection
 *  @param[in] content_type The content type of the response. for e.g.:
 *  To send JSON response it will be: "application/json"
 *  @param[in] content      The data to be sent (optional)
 *  @param[in] content_len  The length of data to be sent
 *
 *  @return WM_SUCCESS      :if successful
 *  @return -WM_FAIL        :otherwise
 */
int httpd_send_response_301(httpd_request_t *req, char *location, const char
		*content_type, char *content, int content_len);

/** @brief Send an HTTP header
 *
 *  @note  This function can be to send out an HTTP header.
 *
 *  It accepts a name value pair as input and sends it out as: "name: value\r\n"
 *  for e.g.:
 *  To send header "Content-Type: application/json\r\n" call
 *  httpd_send_header(sock, "Content-Type", "application/json");
 *
 *  @param[in] sock the socket to send header on
 *  @param[in] name name of the HTTP header
 *  @param[in] value value of the HTTP header 'name'
 *
 *  @return WM_SUCCESS      :if successful
 *  @return -WM_FAIL        :otherwise
 */
int httpd_send_header(int sock, const char *name, const char *value);

/** @brief Send all http header
 *
 *  @note  This function can be to send out all http header.
 *  Only for array types
 *
 *  @return WM_SUCCESS      :if successful
 *  @return -WM_FAIL        :otherwise
 */
int httpd_send_all_header(httpd_request_t *req, const char *first_line, int body_lenth, const char *content_type);

#define HTTPD_SEND_BODY_DATA_MAX_LEN 1024
/** Send an HTTP body
 *
 *  This function can be to send out an HTTP body.
 *  Only for array types
 *
 *  @return WM_SUCCESS       :if successful
 *  @return -WM_FAIL         :otherwise
 */
int httpd_send_body(int sock, const unsigned char *body_image, uint32_t body_size);

/** @brief Send the default HTTP Headers
 *
 *  @note  This function can be used by the WSGI handlers to send out some or all
 *  headers present in \ref httpd_hdr_field_sel_t. The WSGI handlers when
 *  declared can decide which headers they want to send out in the response and
 *  accordingly populate hdr_fields parameter in the httpd_wsgi_call structure
 *
 *  @param[in] sock       The socket to send headers on
 *  @param[in] hdr_fields The hdr_fields parameter of the matching WSGI for the
 *  HTTP request, selected from \ref httpd_hdr_field_sel_t
 *
 *  @return WM_SUCCESS     :if successful
 *  @return -WM_FAIL       :otherwise
 */
int httpd_send_default_headers(int sock, int hdr_fields);

/** @brief Purge the headers of the incoming HTTP request
 *
 *  @note  This function is used to purge the headers of the incoming HTTPD request
 *
 *  Note that it is mandatory to consume all the HTTP headers for the proper
 *  functioning of web server
 *
 *  @param[in] sock        the socket of the incoming HTTP request
 *
 *  @return WM_SUCCESS    :if successful
 *  @return -WM_FAIL      :otherwise
 */
int httpd_purge_headers(int sock);

/** @brief Parse the http headers into the req httpd_request_t structure.
 *
 *  @note  This function will one by one read lines terminated by CR-LF (\\r\\n) from
 * the socket and parse the headers contained in them. Some of the headers (at
 * this time only the Content-Length) will be then copied to the req
 * httpd_request_t structure.
 *
 * Note that it is mandatory to consume all the HTTP headers
 *
 * After executing this function, the socket will point to the request entity
 * body, if any.
 *
 *  @param[in] req     The incoming HTTP request \ref httpd_request_t
 *  @param[in] sock    The socket of the incoming HTTP request
 *  @param[in] scratch Buffer used to read the HTTP headers
 *  @param[in] len     Length of the scratch buffer
 *
 *  @return WM_SUCCESS     :if successful
 *  @return -WM_FAIL       :otherwise
 */
int httpd_parse_hdr_tags(httpd_request_t *req, int sock,
			 char *scratch, int len);

/** @brief Get the incoming data in case of HTTP POST request
 *
 *  @note  This function first parses the HTTP header tags and then receives the data
 *  in the buffer provided by the user.
 *
 *  It can be called multiple times if the size of user buffer is less than the
 *  actual data that is received. Care has been taken that, in such situations,
 *  the HTTP headers are parsed only once. When it returns zero it indicates
 *  that all the data has been received.
 *
 *  Note that this API is optional and WSGI handlers may prefer to parse the
 *  headers and get the data as per their wish using other HTTPD APIs.
 *
 *  @param[in] req      The incoming HTTP request \ref httpd_request_t
 *  @param[out] content The buffer in which the data is to be received
 *  @param[in] length   The length of the content buffer
 *  @return  The number of bytes still remaining to be read if successful
 *  @return  -WM_FAIL   :otherwise
 */
int httpd_get_data(httpd_request_t *req, char *content, int length);

/** @brief Get the incoming JSON data in case of HTTP POST request
 *
 *  @note  This function is an extension to \ref httpd_get_data. Additionally this
 *  function takes in a pointer to the JSON object and initializes the same. In
 *  this case, content is actually used as a scratch buffer. JSON object is what
 *  you actually need to use after it returns.
 *
 *  Note that this function can be called only once i.e. the size of content
 *  buffer passed to this function should be large enough to store all the
 *  incoming data.
 *
 *  @param[in] req      The incoming HTTP request \ref httpd_request_t
 *  @param[out] content The buffer in which the data is to be received
 *  @param[in] length   The length of the content buffer
 *  @param[out] obj     A pointer to JSON object structure
 *  @return   The number of bytes still remaining to be read (should be 0 if used
 *  appropriately) if successful
 *  @return   -WM_FAIL   : otherwise
 */

typedef struct {
	/* Mandatory. Will be sent to the client */
	const unsigned char *server_cert;
	/* Size of server_cert */
	int server_cert_size;
	/*
	 * Server private key. Mandatory.
	 * For the perusal of the server
	 */
	const unsigned char *server_key;
	/* Size of server_key */
	int server_key_size;
	/*
	 * Needed if the server wants to verify client
	 * certificate. Otherwise set to NULL.
	 */
	const unsigned char *client_cert;
	/* Size of client_cert */
	int client_cert_size;
} httpd_tls_certs_t;


int httpd_auth_init(char *name, char *passwd);

char *get_httpd_auth( void );

/**
  * @}
  */


/**
  * @}
  */


/*
 * Call this function to set httpd tls certificates.
 */
//int httpd_use_tls_certificates(const httpd_tls_certs_t *httpd_tls_certs);
#endif



