/*! \file secure_upgrade.h
 *
 * \brief Secure firmware upgrades
 *
 * This module is used in conjunction with rfget and httpc to provide a secure
 * firmware upgrade mechanism. It is recommended to use the secure upgrade APIs
 * provided in \ref fw_upgrade_ed_chacha.h and \ref fw_upgrade_rsa_aes.h rather
 * than the lower level APIs provided by this module. The APIs given here should
 * be used only if the application wants to use own crypto algorithms and
 * not the ones (ED25519 + Chacha20 and RSA + AES-CTR) provided by the SDK.
 *
 * Most important aspect of this module is populating the \ref upgrade_struct_t
 * structure with appropriate functions and data structures. Then, depending on
 * the desired mode, the client_mode_secure_upgrade() or
 * server_mode_secure_upgrade() API can be used.
 *
 * \note The usage of these APIs assumes that all accessories are programmed
 * with the following information at the time of manufacturing:
 * 1. Firmware Encryption Key unique for each device
 * 2. Firmware Verification Key (Upgrade Server's Public Key)
 *
 * The current secure firmware upgrade mechanism specification version is 0002,
 * the details of which can be found in the following sections.
 *
 * \section image_format Secure Firmware upgrade image format
 *
 * The firmware image to be used for secure upgrades should be formatted as
 * follows:
 * 1. The checksum (using SHA, MD5, or any other suitable hashing algorithm as
 *  per the use case) of the firmware image, abc.bin, is calculated.
 * 2. A signature of this checksum is created (with length signature_len)
 *  as the firmware signature.
 *  The private key that is used for generating the signature should correspond
 *  to the Firmware Verification Key programmed in all the accessories.
 *  This ensures that the accessory upgrades to firmware images that are
 *  delivered from a valid source.
 * 3. The length of the firmware, fw_len is calculated.
 * 4. The [signature+firmware image] is then encrypted with a suitable
 *  encryption algorithm using the  Firmware Encryption Key. This ensures that
 *  only accessories that have the correct Firmware Encryption Key can access
 *  the firmware images.
 * 5. A header is created by appending the 4 byte fw_upgrade_spec_version, 4
 *  byte header_len, 4 byte fw_len and the Initialization Vector (IV) of
 *  length iv_len used for encryption. header_len is the number of bytes
 *  starting from fw_len till the end of the header. The purpose of this
 *  is to allow adding more elements in the header, without affecting
 *  older versions. Older versions will just skip the extra fields in the
 *  header.
 * 6. The data encrypted above is then appended to this header and this
 *  entire image is used as the upgrade image.
 *  A different IV should be used during each communication so that the
 *  encryption will be better secured.
 *
 * \section upgrade_algo Firmware Upgrade Algorithm
 * The firmware upgrade algorithm will be as follows:
 * 1. Fetch 4 bytes of fw_upgrade_spec_version and check if it is greater
 *  or equal to the upgrade specification version supported by the device.
 * If not, abort the upgrade and return an error.
 * 2. Fetch the 4 byte header_len and 4 byte fw_len.
 * 3. Fetch iv_len bytes from the server and initialize the decryption context.
 * 4. Read any more bytes left to be read from the header. There may be some
 * additional fields added by newer versions.
 * 5. Fetch signature_len bytes from the server, decrypt them and store as the
 * signature for later use.
 * 6. Fetch one block of the firmware image. If this is the first block,
 * ensure that it contains the right headers.
 * 7. Decrypt and write to the passive partition.
 * 8. Maintain a running checksum of each decrypted block.
 * 9. Repeat steps 4 to 6 until end of file is reached.
 * 10. When end of file is reached, get the final value of the checksum and
 *  use the Firmware Verification Key to verify that the signature of the
 *  firmware that was calculated in step 1 is valid.
 * 11. If the checksum matches, set the passive partition to be active
 *  partition, and perform a soft reset of the accessory.
 *
 */

/* Copyright (C) 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef	_SECURE_UPGRADE_H_
#define _SECURE_UPGRADE_H_
#include <httpc.h>
#include <partition.h>
#ifdef CONFIG_ENABLE_TLS
#include <wm-tls.h>
#else
typedef void tls_client_t;
#endif /* CONFIG_ENABLE_TLS */
/**  Prototype for the decrypt initialization function. Please use a
 * wrapper for your function if it does not match this signature.
 *
 * \param[in,out] context A pointer to the decrypt function's context.
 * \param[in] Key to be used for Decryption.
 * \param[in] iv The initialization vector received from the server.
 */
typedef void (*decrypt_init_func_t) (void *context, void *key, void *iv);

/** Prototype for the decrypt function. Please use a wrapper for your
 * function if it does not match this signature.
 *
 * \param[in,out] context A pointer to the decrypt function's context.
 * \param[in] Pointer to the input buffer which has the ciphertext.
 * \param[in] inlen Length of the input buffer (number of bytes).
 * \param[out] outbuf A pointer to the output buffer which will hold
 * the decrypted data. It can be same as the inbuf if the algorithm
 * uses in-place decryption.
 * \param[in] outlen Length of the output buffer.
 *
 * \return len Length of the decrypted message.
 * \return -WM_FAIL on error.
 */
typedef int (*decrypt_func_t) (void *context, void *inbuf, int inlen,
		void *outbuf, int outlen);

/** Prototype for the signature verification function. Please use a wrapper
 * for your function if it does not match this signature.
 *
 * \param[in] msg Pointer to a buffer which holds the message whose
 * signature has to be verified.
 * \param[in] msglen Length of the message
 * \param[in] pk Pointer to the public key of the server.
 * \param[in] signature The signature against which the verification is
 * to be done.
 *
 * \return 0 if signature verification successful.
 * \return any other value if signature verification failed.
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

/** Prototype for the firmware data read function. Please use a wrapper for
 * your function if it does not match this signature.
 *
 * \param[in] Private data associated with the data read session. This is
 * defined by the session_priv_data element in \ref upgrade_struct_t
 * \param[out] buf Pointer to a buffer to be used to read the data.
 * \param[in] max_len The length of the buffer buf
 *
 * \return Number of bytes read
 * \return 0 if end of data is reached
 * \return -WM_FAIL on error
 */
typedef int (*fw_data_read_fn_t) (void *priv, void *buf, uint32_t max_len);

/** Upgrade structure
 *
 * Structure to be populated by the application and passed to
 * client_mode_secure_upgrade() or server_mode_secure_upgrade()
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
	/** Pointer to a data read function of type \ref fw_data_read_fn_t
	 * that will be used to fetch the firmware data.
	 */
	fw_data_read_fn_t fw_data_read_fn;
	/** Pointer to the decrypt context initialized by the application
	 * before using the secure_upgrade APIs
	 */
	void *decrypt_ctx;
	/** Pointer to the hash context initialized by the application
	 * before using the secure_upgrade APIs
	 */
	void *hash_ctx;
	/** Pointer to a buffer which will hold the decryption context
	 * initialization vector received from the server. This must
	 * point to an allocated buffer of size equal to the IV required
	 * by the decryption algorithm being used.
	 */
	void *iv;
	/** Length of the IV as per the decryption algorithm being used. This
	 * must be initialized by the application.
	 */
	int iv_len;
	/** Pointer to a buffer which holds the received signature. This
	 * must point to an allocated buffer which holds the actual decrypted
	 * signature received from the server.
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
	/** Pointer to the server's public key for verification.
	 */
	void *pk;
	/** Pointer to the decryption key which will later be passed to the
	 * decrypt_init function.
	 */
	void *decrypt_key;
	/** Private data associated with a firmware upgrade session. This will
	 * be passed as an argument to \ref fw_data_read_fn_t.
	 */
	void *session_priv_data;
	/** Number of bytes remaining to read. For internal use only.
	 * Do not modify.
	 */
	int bytes_to_read;
} upgrade_struct_t;

/** Securely upgrade the firmware in client mode
 *
 * This function can be used to securely upgrade the MCU Firmware
 * or Wi-Fi firmware in HTTP client mode. This function will establish
 * a session with the remote HTTP server, securely fetch the upgrade
 * image and flash it.
 * If there is no special handling required, the fw_data_read_fn field of
 * \ref upgrade_struct_t can be left NULL, so that the default data
 * fetch function will be used.
 *
 * \param flash_comp_type Flash component to be updated. Valid values are
 * \ref FC_COMP_FW, and \ref FC_COMP_WLAN_FW.
 * \param[in] fw_url Pointer to a string which holds the url from where
 * the firmware image should be fetched.
 * \param[in] priv Pointer to a \ref upgrade_struct_t structure to be
 * used by the secure upgrade function. The location pointed by this
 * must be available throughout the lifetime of the upgrade process.
 * \param [in] cfg Pass certificate data if TLS connection is used. Pass
 * NULL if TLS connection is not required. Pass NULL if TLS is required but
 * certificate verification is not required.
 *
 * \return WM_SUCCESS if the secure upgrade succeeds.
 * \return -WM_FAIL if the secure upgrade fails.
 */
int client_mode_secure_upgrade(enum flash_comp flash_comp_type, char *fw_url,
		upgrade_struct_t *priv, const tls_client_t *cfg);

/** Securely upgrade the firmware in server mode
 *
 * This function can be used to securely upgrade the MCU Firmware
 * or Wi-Fi firmware in HTTP server mode. However, the data fetch function
 * has to be provided externally by setting the fw_data_read_fn field
 * of \ref upgrade_struct_t to some valid value. session_priv_data can
 * be used to maintain a context of the http session.
 *
 * \param flash_comp_type Flash component to be updated. Valid values are
 * \ref FC_COMP_FW, and \ref FC_COMP_WLAN_FW.
 * \param[in] priv Pointer to a \ref upgrade_struct_t structure to be
 * used by the secure upgrade function. The location pointed by this
 * must be available throughout the lifetime of the upgrade process.
 *
 * \return WM_SUCCESS if the secure upgrade succeeds.
 * \return -WM_FAIL if the secure upgrade fails.
 */
int server_mode_secure_upgrade(enum flash_comp flash_comp_type,
		upgrade_struct_t *priv);

#endif /*_SECURE_UPGRADE_H_ */

