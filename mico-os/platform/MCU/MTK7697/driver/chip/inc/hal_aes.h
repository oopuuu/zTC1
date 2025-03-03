/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_AES_H__
#define __HAL_AES_H__
#include "hal_platform.h"

#ifdef HAL_AES_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup AES
 * @{
 * This section introduces the AES driver APIs including terms and acronyms,
 * supported features, software architecture details on how to use this driver, AES function groups, enums, structures and functions.
 *
 * @section HAL_AES_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b AES                        | The Advanced Encryption Standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Encryption_Standard"> introduction to the AES in Wikipedia</a>.|
 * |\b CBC                        | The Cipher Block Chaining(CBC), each block of plain text is XOR'ed with the previous cipher block before being encrypted. This way, each cipher text block depends on all plain text blocks processed up to that point.
 * |\b ECB                        | The simplest of the encryption mode is the Electronic Codebook (ECB) mode. The message is divided into blocks, and each block is encrypted separately.
 *
 * @section HAL_AES_Features_Chapter Supported features
 *
 * - \b Support \b CBC \b and \b ECB \b modes. \n
 *   During the encryption and decryption, the hardware behaves differently according to the mode configuration:
 *  - \b CBC \b mode: In this mode, the hardware uses CBC algorithm for encryption and decryption.
 *  - \b ECB \b mode: In this mode, the hardware uses EBC algorithm for encryption and decryption.
 *
 * @section HAL_AES_Driver_Usage_Chapter How to use this driver
 *
 * - Use the AES in a CBC mode to encrypt and decrypt. \n
 *  - Step1: Call #hal_aes_cbc_encrypt() to encrypt.
 *  - Step2: Call #hal_aes_cbc_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *
 *       uint8_t aes_cbc_iv[HAL_AES_CBC_IV_LENGTH] = {
 *           0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43,
 *           0x77, 0x68, 0x36, 0x33, 0x50, 0x76, 0x33, 0x46
 *       };
 *       uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       hal_aes_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       hal_aes_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       uint8_t encrypted_buffer[32] = {0};
 *       hal_aes_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_aes_cbc_encrypt(&encrypted_text, &plain_text, &key, aes_cbc_iv);
 *
 *       uint8_t decrypted_buffer[32] = {0};
 *       hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_aes_cbc_decrypt(&decrypted_text, &encrypted_text, &key, aes_cbc_iv);
 *
 *    @endcode
 * - Use the AES in a ECB mode to encrypt and decrypt. \n
 *  - Step1: Call #hal_aes_ecb_encrypt() to encrypt.
 *  - Step2: Call #hal_aes_ecb_decrypt() to decrypt.
 *  - sample code:
 *    @code
 *       uint8_t hardware_id[16] = {
 *           0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30,
 *           0x32, 0x30, 0x31, 0x34, 0x30, 0x38, 0x31, 0x35
 *       };
 *       uint8_t plain[] = {
 *           0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
 *           66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
 *       };
 *       hal_aes_buffer_t plain_text = {
 *           .buffer = plain,
 *           .length = sizeof(plain)
 *       };
 *       hal_aes_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *       uint8_t encrypted_buffer[32] = {0};
 *       hal_aes_buffer_t encrypted_text = {
 *           .buffer = encrypted_buffer,
 *           .length = sizeof(encrypted_buffer)
 *       };
 *       hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
 *
 *       uint8_t decrypted_buffer[32] = {0};
 *       hal_aes_buffer_t decrypted_text = {
 *           .buffer = decrypted_buffer,
 *           .length = sizeof(decrypted_buffer)
 *       };
 *       hal_aes_ecb_decrypt(&decrypted_text, &encrypted_text, &key);
 *
 *    @endcode
 *
 *
 *
 */



#include "hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_aes_define Define
  * @{
  */

#define HAL_AES_BLOCK_SIZES       (16)
#define HAL_AES_KEY_LENGTH_128    (16)
#define HAL_AES_KEY_LENGTH_192    (24)
#define HAL_AES_KEY_LENGTH_256    (32)
#define HAL_AES_CBC_IV_LENGTH     (16)

/**
  * @}
  */


/** @defgroup hal_aes_enum Enum
  * @{
  */

typedef enum {
    HAL_AES_STATUS_ERROR = -1,      /**< AES error */
    HAL_AES_STATUS_OK = 0           /**< AES ok */
} hal_aes_status_t;

/**
  * @}
  */

/** @defgroup hal_aes_struct Struct
  * @{
  */

/** @brief aes buffer */
typedef struct {
    uint8_t *buffer;
    uint32_t length;
} hal_aes_buffer_t;

/**
  * @}
  */



/**
 * @brief     This function provides an AES encryption in a CBC mode.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  key is the AES encryption key.
 * @param[in]  init_vector is the AES initialzation vector for encryption.
 * @return     if successful, returns #HAL_AES_STATUS_OK
 * @par    Example
 * Sample code, please refer to @ref Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_cbc_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH]);

/**
 * @brief     This function provides an AES decryption in a CBC mode.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  key is the AES encryption key.
 * @param[in]  init_vector is the AES initialization vector for encryption.
 * @return if successful, returns #HAL_AES_STATUS_OK
 * @par    Example
 * Sample code, please refer to @ref Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_cbc_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH]);


/**
 * @brief     This function provides an AES encryption in a EBC mode.
 * @param[out] encrypted_text is the target encrypted text.
 * @param[in]  plain_text is the source plain text.
 * @param[in]  key is the AES encryption key.
 * @return if successful, returns #HAL_AES_STATUS_OK
 * @par    Example
 * Sample code, please refer to @ref Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_ecb_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key);

/**
 * @brief     This function provides an AES decryption in a EBC mode.
 * @param[out] plain_text is the target plain text.
 * @param[in]  encrypted_text is the source encrypted text.
 * @param[in]  key is the AES encryption key.
 * @return if successful, returns #HAL_AES_STATUS_OK
 * @par    Example
 * Sample code, please refer to @ref Driver_Usage_Chapter
 *
 */
hal_aes_status_t hal_aes_ecb_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key);


#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /* HAL_AES_MODULE_ENABLED */

#endif /* __HAL_AES_H__ */


