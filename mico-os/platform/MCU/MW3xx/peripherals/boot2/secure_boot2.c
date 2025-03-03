/*
 * Copyright (C) 2008-2014, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <secure_boot2.h>
#include <secure_boot.h>
#include <cyassl/ctaocrypt/rsa.h>
#include <cyassl/ctaocrypt/aes.h>
#include <cyassl/ctaocrypt/sha256.h>
#include <boot2.h>
#include <aes.h>
#include <secure_boot.h>

#define RSA_SIGN_LEN 256

static Sha256 sha;
static RsaKey key;
static aes_t dec;
static uint8_t sig[RSA_SIGN_LEN];
static uint8_t hash[SHA256_DIGEST_SIZE];
static uint8_t nonce[AES_BLOCK_SIZE];

static secure_boot_struct_t priv_data;

/* Initialize the AES CTR context */
static void aes_ctr_init(void *context, void *key, uint32_t key_len,
							void *iv)
{
	aes_drv_setkey(NULL, context, key, key_len, iv, AES_DECRYPTION,
							HW_AES_MODE_CTR);
}

/* This function needs to be defined to match the signature of decrypt_func_t
 * Use appropriate decryption function in it.
 */
static int aes_ctr_decrypt(void *context, void *inbuf,
			void *outbuf, uint32_t len)
{
	return aes_drv_decrypt(NULL, context, inbuf, outbuf, len);
}

/* RSA verification callback  */
static int rsassl_verify(void *message, int len, void *pk, void *signature)
{
	int ret;
	uint8_t hashDecrypt[SHA256_DIGEST_SIZE];

	ret = RsaSSL_Verify(signature, RSA_SIGN_LEN, hashDecrypt,
			    sizeof(hashDecrypt), pk);
	if (ret < 0) {
		dbg("Err...RSA verification failure: %d\n", ret);
		return -1;
	}

	/* Compare if the hash of the decrypted firmware image matches
	 * with hash derived from Signature */
	if (memcmp(message, hashDecrypt, sizeof(hashDecrypt)))
		return -1;

	return 0;
}

/* Sha256 hash update callback */
static void sha256_update(void *context, void *buf, int len)
{
	Sha256Update(context, buf, len);
}

/* Sha256 has finish callback */
static void sha256_finish(void *context, void *result)
{
	Sha256Final(context, result);
}

extern unsigned long _rom_data_start, _rom_data_size;
static void secure_boot_init()
{
	memset(&_rom_data_start, 0, (unsigned) &_rom_data_size);
	ROM_prvHeapInit((unsigned char *)&_heap_start,
				(unsigned char *)&_heap_end);
	CyaSSL_SetAllocators((CyaSSL_Malloc_cb)
				((uint32_t) ROM_pvPortMalloc | 0x1),
				(CyaSSL_Free_cb)
				((uint32_t) ROM_vPortFree | 0x1),
				(CyaSSL_Realloc_cb)
				((uint32_t) ROM_pvPortReAlloc | 0x1));
	aes_drv_init();
}

/* This function will initialize the encryption and hashing
 * context appropriately by fetching the required data.
 */
int secure_boot_firmware(secure_boot_init_t *sb, secure_boot_struct_t **priv)
{
	int ret = 0;
	word32 idx = 0;

	/* Initialize ROM libs and AES hardware engine */
	secure_boot_init();

	if (sb->sign_algo == RSA_SIGN) {
		priv_data.signature_verify = rsassl_verify;
		priv_data.signature = sig;
		priv_data.signature_len = sb->digital_sig->len;
		priv_data.pk = &key;
		InitRsaKey(&key, 0);
		ret = RsaPublicKeyDecode(sb->public_key->value, &idx, &key,
					 sb->public_key->len);
		if (ret != 0) {
			dbg("RSA Public key decode error:%d", ret);
			FreeRsaKey(&key);
			return -1;
		}
		memcpy(priv_data.signature, sb->digital_sig->value,
				priv_data.signature_len > RSA_SIGN_LEN ?
				RSA_SIGN_LEN : priv_data.signature_len);
		if (sb->hash_algo == SHA256_HASH) {
			priv_data.hash_update = sha256_update;
			priv_data.hash_finish = sha256_finish;
			priv_data.hash_ctx = &sha;
			priv_data.hash = hash;
			priv_data.hash_len = SHA256_DIGEST_SIZE;
			InitSha256(&sha);
		}
		priv_data.bytes_to_read = sb->fw_img_len;
	}

	/* AES-CTR algorithm init */
	if (sb->encrypt_algo == AES_CTR_128_ENCRYPT) {
		priv_data.decrypt_init = aes_ctr_init;
		priv_data.decrypt = aes_ctr_decrypt;
		priv_data.decrypt_ctx = &dec;
		priv_data.iv = nonce;
		priv_data.iv_len = sb->nonce->len;
		priv_data.decrypt_key_len = sb->decrypt_key->len;
		priv_data.decrypt_key = sb->decrypt_key->value;
		memcpy(priv_data.iv, sb->nonce->value,
				priv_data.iv_len > AES_BLOCK_SIZE ?
				AES_BLOCK_SIZE : priv_data.iv_len);

		/* Initialize decryption hardware engine */
		priv_data.decrypt_init(priv_data.decrypt_ctx,
					priv_data.decrypt_key,
					priv_data.decrypt_key_len,
					priv_data.iv);
	}

	if (priv)
		*priv = &priv_data;

	return ret;
}

void secure_boot_cleanup(secure_boot_struct_t *priv)
{
	if (priv && priv->pk)
		FreeRsaKey(priv->pk);
}
