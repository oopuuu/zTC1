/*! \file chacha20.h
 * \brief ChaCha20 wrapper functions
 *
 * The functions in this file are wrappers over the chacha20 code,
 * obtained from here:
 * http://cr.yp.to/chacha.html
 *
 * It has code for chacha8 which needs slight modification for usage as
 * chacha20
 */
/*
 *  Copyright (C) 2014, Marvell International Ltd.
 *  All Rights Reserved.
 */
#ifndef _CHACHA_20_H_
#define _CHACHA_20_H_
#include <stdint.h>
#include "ecrypt-sync.h"

/** The chacha20 context which is required by all the chacha20 APIs*/
typedef ECRYPT_ctx chacha20_ctx_t;

/** Initialize chacha20 context
 *
 * This API must be called once for every unique combination of key and nonce,
 * before using the other APIs. It initializes the chacha20 context with the
 * key and nonce provided.
 *
 * \param[out] ctx Pointer to a chacha20 context of type \ref chacha20_ctx_t
 * that will be initialized by this API.
 * \param[in] key Pointer to a buffer holding the chacha20 key to be used for
 * initialization.
 * \param[in] key_len Length of the above key. It should typically be 32 bytes.
 * \param[in] iv Pointer to the initialization vector (Nonce) to be used to
 * initialize the chacha20 context.
 * \param[in] iv_len Length of the IV. It should typically be 8 bytes
 */
void chacha20_init(chacha20_ctx_t *ctx, const uint8_t *key, uint32_t key_len,
	const uint8_t *iv, uint32_t iv_len);

/** Encrypt using chacha20
 *
 * This API encrypts the data provided. It can be called repeatedly if the data
 * size is large.
 *
 * \param[in] ctx The chacha20 context \ref chacha20_ctx_t which has previously
 * been initialized with chacha20_init()
 * \param[in] inputmsg Pointer to a buffer holding the input message.
 * \param[out] outputcipher Pointer to a buffer where the encrypted data will
 * be stored. The size of this buffer must be at least as large as the input
 * message size. The same buffer can be used for input and output as chacha20
 * can do in-place encryption.
 * \param[in] bytes The number of bytes in input message.
 */
void chacha20_encrypt(chacha20_ctx_t *ctx, const uint8_t *inputmsg,
	uint8_t *outputcipher, uint32_t bytes);

/** Decrypt using chacha20
 *
 * This API decrypts the encrypted data provided. It can be called repeatedly
 * if the data size is large.
 *
 * \param[in] ctx The chacha20 context \ref chacha20_ctx_t which has previously
 * been initialized with chacha20_init(). Please note that the context used
 * for encryption should not be used directly for decryption. It needs to be
 * reinitialized using chacha20_init().
 * \param[in] inputcipher Pointer to a buffer holding the input encrypted
 * message.
 * \param[out] outputmsg Pointer to a buffer where the decrypted data will
 * be stored. The size of this buffer must be at least as large as the input
 * message size. The same buffer can be used for input and output as chacha20
 * can do in-place decryption.
 * \param[in] bytes The number of bytes in input message.
 */
void chacha20_decrypt(chacha20_ctx_t *ctx, const uint8_t *inputcipher,
	uint8_t *outputmsg, uint32_t bytes);

/** Increment chacha20 nonce
 *
 * This API increments the chacha20 iv/nonce by one. This is useful for AEAD
 * algorithm like chacha20-poly1305
 *
 * \param[in/out] ctx The chacha20 context \ref chacha20_ctx_t being used for
 * encryption/decryption.
 */
void chacha20_increment_nonce(chacha20_ctx_t *ctx);
#endif /* _CHACHA_20_H_ */
