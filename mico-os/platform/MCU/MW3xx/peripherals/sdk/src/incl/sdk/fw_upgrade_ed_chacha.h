/*! \file fw_upgrade_ed_chacha.h
 * \brief  Firmware Upgrades using ED25519 and Chacha20
 *
 * The WMSDK provides convenience APIs to enable secure firmware upgrades
 * using Chacha20 encryption and signing using ED25519 by taking a SHA512
 * checksum of the firmware image. This has to be enabled using the
 * CONFIG_FWUPG_ED_CHACHA config option.
 *
 * Please refer \ref secure_upgrade.h for details about the upgrade
 * process and the format of the firmware image.
 *
 * Please also have a look at the README for fw_generator utility in tools/
 *
 * A short summary of steps to follow for using this module are:
 * - Go to the sdk/tools/src/fw_generator
 * - Execute make. This will generate an executable named
 *   ed_chacha_fw_generator
 * - Create a config file fw_generator.config using
 *   \code
 *   ~# ./ed_chacha_fw_generator config > fw_generator.config
 *   \endcode
 * - Create an encrypted binary using
 *   \code
 *   ~# ./ed_chacha_fw_generator <ipfile> <opfile> fw_generator.config
 *   \endcode
 *   The input file should be the .bin firmware image that is normally
 *   used to flash on the board.
 * - Use the fwupg_verification_key and fwupg_encrypt_decrypt_key as the
 *   keys for the functions ed_chacha_client_mode_upgrade_fw() or
 *   ed_chacha_server_mode_upgrade_fw()
 * - The mechanism to use for providing these keys to the functions is
 *   left to the applications. A common method is to store the keys in
 *   a manufacturing psm partition and then use it from there.
 *
 */

/* Copyright (C) 2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <partition.h>
#include <secure_upgrade.h>

/** Length of the decryption key required by the
 * ed_chacha_client_mode_upgrade_fw() function
 */
#define ED_CHACHA_DECRYPT_KEY_LEN		32
/** Length of the verification key required by the
 * ed_chacha_client_mode_upgrade_fw() function
 */
#define ED_CHACHA_VERIFICATION_KEY_LEN		32

/** Upgrade Firmware in client mode (HTTPC)
 *
 * \param[in] flash_comp_type Flash component to be updated. Valid values are
 * \ref FC_COMP_FW, and \ref FC_COMP_WLAN_FW.
 * \param[in] fw_upgrade_url URL from which the firmware image should be
 * fetched.
 * \param[in] verification_pk Pointer to the server's ED25519 public key which
 * will be used for verification of the image. The key must be of length
 * \ref ED_CHACHA_VERIFICATION_KEY_LEN.
 * \param[in] decrypt_key Pointer to the CHACHA20 key which will be
 * used for decryption of the firmware image. The key must be of length
 * \ref ED_CHACHA_DECRYPT_KEY_LEN.
 * \param [in] cfg Pass certificate data if TLS connection is used. Pass
 * NULL if TLS connection is not required. Pass NULL if TLS is required but
 * certificate verification is not required.
 *
 * \return WM_SUCCESS if the upgrade succeeds.
 * \return -WM_FAIL if the upgrade fails.
 */
int ed_chacha_client_mode_upgrade_fw(enum flash_comp flash_comp_type,
		char *fw_upgrade_url, uint8_t *verification_pk,
		uint8_t *decrypt_key, tls_client_t *cfg);

/** Upgrade Firmware in Server mode (HTTPD)
 *
 * \param[in] flash_comp_type Flash component to be updated. Valid values are
 * \ref FC_COMP_FW, and \ref FC_COMP_WLAN_FW.
 * \param[in] verification_pk Pointer to the server's ED25519 public key which
 * will be used for verification of the image. The key must be of length
 * \ref ED_CHACHA_VERIFICATION_KEY_LEN.
 * \param[in] decrypt_key Pointer to the CHACHA20 key which will be
 * used for decryption of the firmware image. The key must be of length
 * \ref ED_CHACHA_DECRYPT_KEY_LEN.
 * \param[in] fw_data_read_fn Pointer to a callback function of type
 * \ref fw_data_read_fn_t to be used by the secure_upgrade module to read
 * the firmware image data
 * \param[in] priv Private data that will be passed to fw_data_read_fn as
 * an argument. This can be used to maintain some context of the session.
 *
 * \return WM_SUCCESS if the upgrade succeeds.
 * \return -WM_FAIL if the upgrade fails.
 */
int ed_chacha_server_mode_upgrade_fw(enum flash_comp flash_comp_type,
		uint8_t *verification_pk, uint8_t *decrypt_key,
		fw_data_read_fn_t fw_data_read_fn, void *priv);
