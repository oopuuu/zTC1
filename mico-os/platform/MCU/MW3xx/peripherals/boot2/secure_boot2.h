/* Copyright (C) 2008-2014, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef	_SECURE_BOOT2_H_
#define _SECURE_BOOT2_H_

/**  Prototype for the decrypt initialization function. Please use a
 * wrapper for your function if it does not match this signature.
 *
 * \param[in,out] context A pointer to the decrypt function's context.
 * \param[in] key Pointer to key that will be used for decryption.
 * \param[in] key_len decryption key length.
 * \param[in] iv Pointer to initialization vector.
 *
 * \note Right now only AES-CTR-128 mode is supported.
 */
typedef void (*decrypt_init_func_t) (void *context, void *key,
		uint32_t key_len, void *iv);

/** Prototype for the decrypt function. Please use a wrapper for your
 * function if it does not match this signature.
 *
 * \param[in,out] context A pointer to the decrypt function's context.
 * \param[in] Pointer to the input buffer which has the ciphertext.
 * \param[out] outbuf A pointer to the output buffer which will hold
 * the decrypted data. It can be same as the inbuf if the algorithm
 * uses in-place decryption.
 * \param[in] len Length of the input buffer (number of bytes).
 *
 * \return WM_SUCCESS on success.
 * \return -WM_FAIL on error.
 */
typedef int (*decrypt_func_t) (void *context, void *inbuf,
			void *outbuf, uint32_t len);

/** Prototype for the signature verification function. Please use a wrapper
 * for your function if it does not match this signature.
 *
 * \param[in] msg Pointer to a buffer which holds the message whose
 * signature has to be verified.
 * \param[in] msglen Length of the message
 * \param[in] pk Pointer to the public key
 * \param[in] signature The signature against which the verification is
 * to be done.
 *
 * \return 0 if signature verification successful.
 * \return <any other value> if signature verification failed.
 */
typedef int (*signature_verify_func_t) (void *msg, int msglen,
		void *pk, void *signature);

/** Prototype for the hash update function. Please use a wrapper for your
 * function if it does not match this signature.
 *
 * \param[in,out] context A pointer to the hashing function's context.
 * \param[in] buf Pointer to a buffer whose hash (checksum) is to be
 * calculated.
 * \param[in] buflen Length of the buffer
 */
typedef void (*hash_update_func_t) (void *context, void *buf, int buflen);

/** Prototype for the hash finish function. Please use a wrapper for your
 * function if it does not match this signature.
 *
 * \param[in] context A pointer to the hashing function's context.
 * \param[out] result Pointer to a buffer where the result of the hash
 * (checksum) will be stored.
 */
typedef void (*hash_finish_func_t) (void *context, void *result);

/** Secure boot structure
 *
 * Structure to be populated by the application and passed to
 * secure_boot_firmware().
 */
typedef struct {
	/** Pointer to the decryption context initialization function
	 * of type \ref decrypt_init_func_t
	 */
	decrypt_init_func_t decrypt_init;
	/** Pointer to the decryption function of type
	 * \ref decrypt_func_t
	 */
	decrypt_func_t decrypt;
	/** Pointer to the signature verification function of type
	 * \ref signature_verify_func_t
	 */
	signature_verify_func_t signature_verify;
	/** Pointer to the hash update function of type
	 * \ref hash_update_func_t
	 */
	hash_update_func_t hash_update;
	/** Pointer to a hash finish function of type
	 * \ref hash_finish_func_t
	 */
	hash_finish_func_t hash_finish;
	/** Pointer to the decrypt context initialized by the application
	 * before using secure_boot_firmware().
	 */
	void *decrypt_ctx;
	/** Pointer to the hash context initialized by the application
	 * before using secure_boot_firmware().
	 */
	void *hash_ctx;
	/** Pointer to a buffer which will hold the decryption context
	 * initialization vector. This must point to an allocated buffer of
	 * size equal to the IV required by the decryption algorithm being
	 * used.
	 */
	void *iv;
	/** Length of the IV as per the decryption algorithm being used. This
	 * must be initialized by the application.
	 */
	int iv_len;
	/** Pointer to a buffer which holds the received signature. This
	 * must point to an allocated buffer which holds the actual decrypted
	 * signature.
	 */
	void *signature;
	/** Length of the signature as per the algorithm being used. This
	 * must be initialized by the application.
	 */
	int signature_len;
	/** Pointer to a buffer which holds the hash value. The memory must
	 * be allocated by the application.
	 */
	void *hash;
	/** Length of the hash as per the hashing algorithm being used.
	 */
	int hash_len;
	/** Pointer to the public key for verification.
	 */
	void *pk;
	/** Pointer to the decryption key which will later be passed to the
	 * decrypt_init function.
	 */
	void *decrypt_key;
	/** Length of the decryption key which will later be passed to the
	 * decrypt_init function.
	 */
	int decrypt_key_len;
	/** Number of bytes remaining to read. For internal use only.
	 * Do not modify.
	 */
	int bytes_to_read;
} secure_boot_struct_t;

typedef struct {
	uint8_t sign_algo;
	uint8_t encrypt_algo;
	uint8_t hash_algo;
	struct tlv_entry *public_key;
	struct tlv_entry *digital_sig;
	struct tlv_entry *decrypt_key;
	struct tlv_entry *nonce;
	uint32_t fw_img_len;
} secure_boot_init_t;

typedef enum {
	/* Success */
	SB_ERR_NONE = 0x00000000,
	/* No keystorage found, normal boot */
	SB_ERR_NO_KS = 0x00000001,
	/* Bad keystorage, CRC or magic mismatch */
	SB_ERR_BAD_KS = 0x00000002,
	/* Bad firmware hdr, CRC or magic mismatch */
	SB_ERR_BAD_FW_HDR = 0x00000004,
	/* Un-encrypted image */
	SB_ERR_NO_ENCRYPT = 0x00000008,
	/* Un-signed image */
	SB_ERR_NO_SIGN = 0x00000010,
	/* Firmware len missing */
	SB_ERR_NO_FW_LEN = 0x00000020,
	/* Bad or missing public key */
	SB_ERR_BAD_PUB_KEY = 0x00000040,
	/* Bad or missing decrypt key */
	SB_ERR_BAD_DECRYPT_KEY = 0x00000080,
	/* Bad or missing signature */
	SB_ERR_BAD_SIGN = 0x00000100,
	/* Bad or missing nonce/iv */
	SB_ERR_BAD_NONCE = 0x00000200,
	/* Secure boot init failed */
	SB_ERR_INIT_FAIL = 0x00000400,
	/* Sign match failed */
	SB_ERR_SIGN_MISMATCH = 0x00000800,
	/* Invalid firmware header size */
	SB_ERR_BAD_FW_HDR_LEN = 0x00001000,
	/* Unknon err */
	SB_ERR_UNKNOWN = 0x80000000,
} sb_err_e;

int secure_boot_firmware(secure_boot_init_t *sb, secure_boot_struct_t **priv);
void secure_boot_cleanup(secure_boot_struct_t *priv);

#endif /*_SECUR_BOOT2_H_ */
