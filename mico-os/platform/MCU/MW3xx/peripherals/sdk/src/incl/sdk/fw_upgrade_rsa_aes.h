/*! \file fw_upgrade_rsa_aes.h
 * \brief  Firmware Upgrades using RSA and AES
 *
 * The WMSDK Provides an API to enable secure remote firmware upgrades.
 * It uses AES CTR 128 encryption and signing using RSA by taking a SHA512
 * checksum of the firmware image. This has to be enabled using the
 * CONFIG_FWUPG_RSA_AES config option.
 *
 * Please refer \ref secure_upgrade.h for details about the upgrade
 * process and the format of the firmware image.
 *
 * Please also have a look at the README for fw_generator utility in tools/
 *
 * A short summary of steps to follow for using this module are:
 * - Go to the sdk/tools/src/fw_generator
 * - Execute make.
 *   ~# make CRYPTO_OPTION=rsa_aes PRISTINE_CYASSL_PATH=/path/to/cyassl-3.1.0
 *   This will generate a executable named fw_generator
 * - Create a config file fw_generator.config using
 *   \code
 *   ~# ./rsa_aes_fw_generator config > fw_generator.config
 *   \endcode
 * - Create an encrypted binary using
 *   \code
 *   ~# ./rsa_aes_fw_generator <ipfile> <opfile> fw_generator.config
 *   \endcode
 *   The input file should be the .bin firmware image that is normally
 *   used to flash on the board.
 * - Use the fwupg_verification_key and fwupg_encrypt_decrypt_key as the
 *   keys for the functions rsa_aes_client_mode_upgrade_fw() or
 *   rsa_aes_server_mode_upgrade_fw()
 * - The mechanism to use for providing these keys to the functions is
 *   left to the applications. A common method is to store the keys in
 *   a manufacturing psm partition and then use it from there.
 *
 */

/* Copyright (C) 2015, Marvell International Ltd.
 * All Rights Reserved.
 */

/*
 * The WMSDK Provides an API to enable secure remote firmware upgrades.
 * It uses AES CTR 128 encryption and signing using RSA by taking a SHA512
 * checksum of the firmware image. This has to be enabled using the
 * CONFIG_FWUPG_RSA_AES config option.
 *
 *
 */

#include <stdint.h>
#include <partition.h>
#include <secure_upgrade.h>
#ifdef CONFIG_ENABLE_TLS
#include <wm-tls.h>
#else
typedef void tls_client_t;
#endif /* CONFIG_ENABLE_TLS */
/** Length of the decryption key
 */
#define RSA_PUBLIC_KEY_LEN 294
/** Length of the verification key required
 */
#define AES_KEY_LEN 16

/** Upgrade Firmware in client mode (HTTPC)
 *
 * \param[in] flash_comp_type Flash component to be updated. Valid values are
 * \ref FC_COMP_FW, and \ref FC_COMP_WLAN_FW.
 * \param[in] fw_upgrade_url URL from which the firmware image should be
 * fetched.
 * \param[in] verification_pk Pointer to the server's RSA public key which
 * will be used for verification of the image. The key must be of length
 * \ref RSA_PUBLIC_KEY_LEN.
 * \param[in] decrypt_key Pointer to the AES CTR key which will be
 * used for decryption of the firmware image. The key must be of length
 * \ref AES_KEY_LEN.
 * \param[in] cfg Pointer to the \ref tls_client_t structure variable
 * giving TLS certificate details. This should be specified as "NULL" when
 * TLS is not being used.
 *
 * \note \ref tls_client_t parameter can be specified as NULL in case
 * of HTTP. For HTTPS, the HTTPS support needs to be enabled from HTTP Client
 * option in make menuconfig.
 *
 * \return WM_SUCCESS if the upgrade succeeds.
 * \return -WM_FAIL if the upgrade fails.
 */
int rsa_aes_client_mode_upgrade_fw(enum flash_comp flash_comp_type,
					  char *fw_upgrade_url,
					  uint8_t *verification_pk,
					  uint8_t *decrypt_key,
					  const tls_client_t *cfg);

/** Upgrade Firmware in Server mode (HTTPD)
 *
 * \param[in] flash_comp_type Flash component to be updated. Valid values are
 * \ref FC_COMP_FW, and \ref FC_COMP_WLAN_FW.
 * \param[in] verification_pk Pointer to the server's RSA public key which
 * will be used for verification of the image. The key must be of length
 * \ref RSA_PUBLIC_KEY_LEN.
 * \param[in] decrypt_key Pointer to the AES CTR key which will be
 * used for decryption of the firmware image. The key must be of length
 * \ref AES_KEY_LEN.
 * \param[in] fw_data_read_fn Pointer to a callback function of type
 * \ref fw_data_read_fn_t to be used by the secure_upgrade module to read
 * the firmware image data
 * \param[in] priv Private data that will be passed to fw_data_read_fn as
 * an argument. This can be used to maintain some context of the session.
 *
 * \return WM_SUCCESS if the upgrade succeeds.
 * \return -WM_FAIL if the upgrade fails.
 */
int rsa_aes_server_mode_upgrade_fw(enum flash_comp flash_comp_type,
					  uint8_t *verification_pk,
					  uint8_t *decrypt_key,
					  fw_data_read_fn_t fw_data_read_fn,
					  void *priv);
