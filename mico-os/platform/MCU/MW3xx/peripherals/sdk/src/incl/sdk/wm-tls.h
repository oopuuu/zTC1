/*! \file wm-tls.h
 *  \brief TLS API's
 *
 *
 * The TLS module is a software component that provides an interface to a
 * TLS library. Currently this module interfaces with the CyaSSL
 * library. This provides simplified APIs to work with secure TLS
 * connections for both client and server applications.
 *
 * Following is the process used when working with CyaSSL for TLS based
 * server or TLS based client:
 * (1) Create server/client context (API in current header file)
 *   - This step creates a context structure inside CyaSSL containing
 *     certificate information passed by you. A handle is returned.
 *     This handle can be used multiple times for any number of socket
 *     connections to the peer corresponding to the passed X509 certs.
 * (2) Create SSL object using CyaSSL APIs directly (not in this file)
 *   - The context created in step 1 above is taken as input parameter by
 *     this API .
 * (3) Communicate with the connected peer over TLS using CyaSSL read/write
 *     API's directly.
 * (4) When done destroy the SSL object created in step 2 (using CyaSSL
 *     API's directly)
 * (5) Goto Step 2 if new socket communication is to be established.
 * (6) Server/Client context can now be destroyed using the purge API's
 *     (API's from current header file)
 *
 */
/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __WM_TLS_H__
#define __WM_TLS_H__

#include <stdint.h>
#include <openssl/ssl.h>

/**
 * All the below certificates and keys have to be encoded in the PEM
 * format.
 */
typedef struct {
	/**
	 * Needed if the client wishes to verify server certificate,
	 * Otherwise set to NULL. Chained buffers are automatically
	 * handled.
	 */
	const unsigned char *ca_cert;
	/** Size of CA_cert */
	int ca_cert_size;
	/**
	 * Needed if the server (the other end) mandates verification of
	 * client certificate. i.e. the server wants to verify if we are
	 * authentic. Set to NULL if not required. Typically, to connect to
	 * any public server (over SSL) like Wikipedia, Google this does
	 * not need to be set. This is for those special cases where the
	 * server wishes to ensure that it is talking with the correct
	 * client (us).
	 */
	const unsigned char *client_cert;
	/** Set this to true if client certificate is chained */
	bool client_cert_is_chained;
	/** Size of client_cert */
	int client_cert_size;
	/** Client private key */
	const unsigned char *client_key;
	/** Size of client key */
	int client_key_size;
} tls_client_t;

/**
 * The tls_init_config_t parameter in the tls_session_init() takes a pointer
 * to the certificate buffer. The certificate buffer can either have a
 * single (or a chain of) X.509 PEM format certificate(s). If the server
 * uses a self-signed certificate, the same certificate needs to be present
 * in this buffer. If the server uses a CA signed certificate, you need to
 * have either of the following:
 * - the root CA certificate only
 * - the chain of certificates (entire or partial starting with the Root CA
 * certificate)
 * Note: The system time needs to be set correctly for successful
 * certificate validation.
 */

typedef int tls_handle_t;

/** TLS session flags. Set single or combination of these in
    \ref tls_init_config_t flags member before passing it to
    tls_session_init() */
typedef enum {
	/* TLS server mode */
	/* If this flag bit is zero client mode is assumed.*/
	TLS_SERVER_MODE = 0x01,
	TLS_CHECK_CLIENT_CERT = 0x02,

	/* TLS Client mode */
	TLS_CHECK_SERVER_CERT = 0x04,
	/* This will be needed if server mandates client certificate. If
	   this flag is enabled then client_cert and client_key from the
	   client structure in the union tls (from tls_init_config_t)
	   needs to be passed to tls_session_init() */
	TLS_USE_CLIENT_CERT = 0x08,
#ifdef CONFIG_WPA2_ENTP
	TLS_WPA2_ENTP = 0x10,
#endif
	/* Set this bit if given client_cert (client mode) or server_cert
	   (server mode) is a chained buffer */
	TLS_CERT_BUFFER_CHAINED = 0x20,
} tls_flags_t;

/**
 * @note: When a certificate is passed to tls function its size parameter
 * should be 1 less that sizeof(array) in which the certificate is present.
 *
 * @note: The user buffers containing certificate and private key need not
 * be available after return from tls_session_init(). CyaSSL maintains own
 * copy.
 */
typedef struct {
	/** OR of flags defined in \ref tls_flags_t */
	int flags;
	/** Either a client or a server can be configured at a time through
	   tls_session_init(). Fill up appropriate structure from the
	   below union depending on your requirement. */
	union {
		/** Structure for client TLS configuration */
		struct {
			/**
			 * Needed if the RADIUS server mandates verification of
			 * CA certificate. Otherwise set to NULL.*/
			const unsigned char *ca_cert;
			/** Size of CA_cert */
			int ca_cert_size;
			/**
			 * Needed if the server mandates verification of
			 * client certificate. Otherwise set to NULL. In
			 * the former case please OR the flag
			 * TLS_USE_CLIENT_CERT to flags variable in
			 * tls_init_config_t passed to tls_session_init()
			 */
			const unsigned char *client_cert;
			/** Size of client_cert */
			int client_cert_size;
			/** Client private key */
			const unsigned char *client_key;
			/** Size of client key */
			int client_key_size;
		} client;
		/** Structure for server TLS configuration */
		struct {
			/** Mandatory. Will be sent to the client */
			const unsigned char *server_cert;
			/** Size of server_cert */
			int server_cert_size;
			/**
			 * Server private key. Mandatory.
			 * For the perusal of the server 
			 */
			const unsigned char *server_key;
			/** Size of server_key */
			int server_key_size;
			/**
			 * Needed if the server wants to verify client
			 * certificate. Otherwise set to NULL.
			 */
			const unsigned char *client_cert;
			/** Size of client_cert */
			int client_cert_size;
		}server;
	} tls;
} tls_init_config_t;

/**
 * Initialize the CTAO Crypto library
 *
 * \return -WM_FAIL if initialization failed.
 * \return WM_SUCCESS if operation successful.
 */
int ctaocrypt_lib_init(void);

/**
 * Initialize the TLS library
 *
 * @return Standard WMSDK return codes.
 */
int tls_lib_init(void);

/**
 * Start an TLS session on the given socket.
 *
 * @param[out] h Pointer to the handle,
 * @param[in] sockfd A socket descriptor on which 'connect' is called
 * before. This parameter is not to be given for TLS server init. The
 * client socket is to be 
 * @param[in] cfg TLS configuration request structure. To be allocated and
 * populated by the caller.
 *
 * @return Standard WMSDK return codes.
 */
int tls_session_init(tls_handle_t *h, int sockfd,
		     const tls_init_config_t *cfg);


int tls_server_set_clientfd(SSL *ssl, int clientfd);

/**
 * Create server context.
 *
 * This function creates a TLS context for a server implementation. This
 * context can then be reused for multiple client connections. Note that
 * after the context is created the user buffers containing the
 * certificates and keys can be freed. CyaSSL maintains its own copies.
 *
 * @post When this context is not required anymore please call
 * tls_purge_server_context() to free the internal buffers allocated by
 * tls_create_server_context().
 *
 * @param[in] cfg Certificate details.
 *
 * @return Allocated SSL_CTX structure.
 * @return NULL if structure could not be allocated.
 */
SSL_CTX *tls_create_server_context(const tls_init_config_t *cfg);

/**
 * Purge the context created by tls_create_server_context()
 *
 * This function is generally called during HTTP server shutdown.
 *
 * @param[in] ctx Context structure of type SSL_CTX created by
 * tls_create_server_context()
 */
void tls_purge_server_context(SSL_CTX *ctx);

/**
 * Create client context.
 *
 * This function creates a TLS context for a client implementation. This
 * context can then be reused for multiple connections with a server. Note
 * that, after the context is created the user buffers containing the
 * certificates and keys can be freed. CyaSSL maintains its own copies.
 *
 * @post When this context is not required anymore please call
 * tls_purge_client_context() to free the internal buffers allocated by
 * tls_create_client_context().
 *
 * @param[in] cfg Certificate details.
 *
 * @return Allocated SSL_CTX structure.
 * @return NULL if structure could not be allocated.
 */
SSL_CTX *tls_create_client_context(const tls_client_t *cfg);

/**
 * Purge the context created by tls_create_client_context()
 *
 * @param[in] ctx Context structure of type SSL_CTX created by
 * tls_create_client_context()
 */
void tls_purge_client_context(SSL_CTX *ctx);

/**
 * Send data over an existing TLS connection.
 *
 * @param[in] h Handle returned from a previous call to TLS_session_init
 * @param[in] buf Buffer to send.
 * @param[in] len Length to write
 *
 * @return Amount data written to the network
 */
int tls_send(tls_handle_t h, const void *buf, int len);

/**
 * Receive data from an existing TLS connection.
 *
 * @param[in] h Handle returned from a previous call to TLS_session_init
 * @param[in] buf Buffer to receive data.
 * @param[in] max_len Max length of the buffer.
 *
 * @return Amount data read from the network
 */
int tls_recv(tls_handle_t h, void *buf, int max_len);

/**
 * Close the tls connection.
 *
 * This function will not close the socket. It will only terminate TLS
 * connection over it.
 *
 * @param[in,out] h Handle returned from a previous call to
 *                                  TLS_session_init. Will set it to NULL
 *                                  before returning.
 */
void tls_close(tls_handle_t *h);

/**
 * @internal
 */
int tls_session_key(tls_handle_t h, uint8_t *key, int len);

/**
 * @internal
 */
int get_session_key(uint8_t *key, int len);

/**
 * Get pending length
 *
 * @param[in] h Handle returned from a previous call to TLS_session_init
 *
 * @return pending length
 */
int tls_pending(tls_handle_t h);

/**
 * Turn error to SSL type
 *
 * @param[in] h Handle returned from a previous call to TLS_session_init
 * @param[in] rc error code.
 *
 * @return SSL error code
 */
int tls_get_error(tls_handle_t h, int rc);



/**
 * @internal
 */
void load_certs(const unsigned char *ca_cert,
		const unsigned char *client_cert,
		const unsigned char *client_key);

/**
 * @internal
 *
 * Helper function for TCP connect. No SSL related activity is done in this
 * function.
 */
int tcp_connect_helper(int *sockfd, const char *ip, uint16_t port,
		       int timeout_ms);

#endif  /* __WM_TLS_H__ */
