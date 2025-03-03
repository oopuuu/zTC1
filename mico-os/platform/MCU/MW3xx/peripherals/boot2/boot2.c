/*
 *  Copyright (C) 2011-2014, Marvell International Ltd.
 *  All Rights Reserved.
 */

/**
 *  @brief This is a second stage boot loader for the mc200/mw300
 *  It provides below features-
 *  -- Reading partition table information and execute the
 *     latest programmed firmware
 *  -- Boot from backup firmware
 *  -- Passing boot flags to firmware for appropriate actions
 */

#include <stdint.h>
#include <stdio.h>

#include "boot2.h"

#include <crc32.h>
#include <flash.h>
#include <partition.h>
#include <flash_layout.h>
#include <boot_flags.h>
#include <lowlevel_drivers.h>
#include <firmware_structure.h>
#include <mw300_flash.h>
#include <mw300_flashc.h>

#include <secure_boot.h>
#include <secure_boot2.h>
#ifdef CONFIG_ENABLE_MXCHIP
#include "Common.h"
#include "MicoPlatform.h"
#endif

void encrypt_arc4(uint8_t *in, uint8_t *out, int len);

#define FLASH_OTP_NVRAM_ADDR 0x480C002C
#define FLASH_OTP_OFFSET     18
#define DISABLE_SECURE_BOOT // yhb added

#define DEFAULT_FIRMWARE_OFFSET (0x10000)
#define DEFAULT_FIRMWARE_SIZE (0x10000*6)


#define crc32 _crc32

/* Make the "MRVL" string into an endian invariant 32-bit constant */
#if defined(__ARMEL__)
#define FW_MAGIC_STR		(('M' << 0)|('R' << 8)|('V' << 16)|('L' << 24))
#elif defined(__ARMEB__)
#define FW_MAGIC_STR		(('M' << 24)|('R' << 16)|('V' << 8)|('L' << 0))
#else
#error "unknown endian mode"
#endif

#define FAIL			1	/* non-zero return = failure */

static secure_boot_struct_t *priv;
static secure_boot_init_t sb;
static uint8_t sec_fw_image_hdr[SB_SEC_IMG_HDR_SIZE];
static struct img_hdr ih;
static struct seg_hdr sh[SEG_CNT];
static uint16_t keystore_len;
/* JEDECID of flash */
static uint32_t id;

static FLASH_Interface_Type FLASH_GetInterface(void)
{
    return FLASHC->FCCR.BF.FLASHC_PAD_EN;
}

static int flash_read(void *buf, uint32_t addr, uint32_t size)
{
    int ret;

    if (FLASH_GetInterface() == FLASH_INTERFACE_FLASHC) {
        memcpy(buf, (uint8_t *)addr + 0x1f000000, size);
        ret = size;
    } else {
        ret = FLASH_Read(FLASH_FAST_READ_QUAD_IO, addr,
                (unsigned char *)buf, size);
    }
    return ret;
}

#ifndef DISABLE_SECURE_BOOT
static int secure_boot_feed_data(void *buf, uint32_t size)
{
	if (sb.hash_algo != NO_HASH) {
		if (!priv->hash_update)
			return FAIL;
		priv->hash_update(priv->hash_ctx, buf, size);
		priv->bytes_to_read -= size;
		if (priv->bytes_to_read < 0) {
			dbg("Err...invalid fw image length\r\n");
			return FAIL;
		}
		dbg("hash update, remaining:%d\r\n", priv->bytes_to_read);
		/* If bytes_to_read=0, it means that the entire firmware
		 * image has been read and decrypted. Get the hash result,
		 * take its signature and verify with the received signature.
		 */
		if (priv->bytes_to_read == 0) {
			if (!priv->hash_finish || !priv->signature_verify)
				return FAIL;

			priv->hash_finish(priv->hash_ctx, priv->hash);
			if (priv->signature_verify(priv->hash, priv->hash_len,
					priv->pk, priv->signature) != 0) {
				dbg("Err...fw signature match failed\r\n");
				writel((readel(sb_e) | SB_ERR_SIGN_MISMATCH),
						sb_e);
				return FAIL;
			}
		}
	}

	if (sb.encrypt_algo != NO_ENCRYPT) {
		if (!priv->decrypt)
			return FAIL;
		else
			return priv->decrypt(priv->decrypt_ctx, buf,
							buf, size);
	}

	return 0;
}

#endif

static uint32_t secure_read_flash(void *buf, uint32_t addr, uint32_t size,
               uint32_t crc)
{
    int ret;

    dbg("flash read from @0x%08x, len 0x%08x\r\n", addr, size);
    flash_read(buf, addr, size);
	
#ifndef DISABLE_SECURE_BOOT	
	ret = secure_boot_feed_data(buf, size);
	if (ret)
		return ret;
#endif
    crc = crc32(buf, size, crc);
    if ((uint32_t)buf == 0x20000040) {
        encrypt_arc4(buf, buf, size);
    }

    return crc;
}

/**
 * read_flash : Reads flash memory
 * @buf : pointer to butter in which flash content to be read
 * @addr: Flash read address
 * @size : no of bytes to be read
 * @return value : calculated crc
 */
static uint32_t read_flash(void *buf, uint32_t addr, uint32_t size,
               uint32_t crc)
{
    flash_read(buf, addr, size);
    return crc32(buf, size, crc);
}

static int load_partition_table(struct partition_table *ph,
                  uint32_t addr)
{
    uint32_t crc = read_flash(ph, addr, sizeof(*ph), 0);

    if (crc != 0 || ph->magic != PARTITION_TABLE_MAGIC) {
        if (crc != 0)
            writel((readel(nvram_addr) | BAD_PART_TABLE_CRC),
                                nvram_addr);
        else
            writel((readel(nvram_addr) | BAD_PART_TABLE_MAGIC),
                                nvram_addr);
        /* Invalidate number of partition entries */
        ph->partition_entries_no = 0;
        /* Invalidate generation level */
        ph->gen_level = 0;

        return -1;
    }
    return 0;
}

static int select_active_partition_table(uint32_t *addr)
{
    int ret, status;
    struct partition_table ph[2];

    status = 0;
    ret = load_partition_table(&ph[0], FL_PART1_START);
    if (ret != 0)
        /* Set partition table 1 status to indicate corruption */
        status |= PARTITION_TABLE1_STATE_MASK;

    ret = load_partition_table(&ph[1], FL_PART2_START);
    if (ret != 0)
        /* Set partition table 2 status to indicate corruption */
        status |= PARTITION_TABLE2_STATE_MASK;

    if (ph[1].gen_level > ph[0].gen_level) {
        *addr = FL_PART2_START;
        status |= BOOT_PARTITION_TABLE_MASK;
        return status;
    } else {
        *addr = FL_PART1_START;
        return status;
    }
}

static void load_partition_entry(struct partition_table *pt,
        struct partition_entry *ph,
        uint32_t addr, short *start_index)
{
    int i = 0;
    if (start_index)
        i = *start_index;

    for (; i < pt->partition_entries_no; i++) {
        read_flash(ph, addr, sizeof(*ph), 0);
        addr += sizeof(*ph);
        if (ph->type == FC_COMP_FW)
            break;
    }
    *start_index = i + 1;
    if (ph->type != FC_COMP_FW)
        /* Invalidate partition generation level */
        ph->gen_level = 0;
}

static void read_from_seghdr(struct firmware_segment *segment, void *addr,
								uint32_t size)
{
    while (size) {
        uint32_t length = segment->length;
        if (!length)
            return;
        if (length > size)
            length = size;
        secure_read_flash(addr, segment->offset, length, 0);
        segment->offset += length;
        segment->length -= length;
        addr += length;
        size -= length;
    }

    return;
}

static int load_firmware(uint32_t firm_addr, uint32_t firm_size)
{
    struct firmware_segment segment;
    uint32_t value;
    uint32_t *data_ptr;
    int (*fn_ptr) (uint32_t), ret;

    dbg("%s: firm_addr:0x%x, size: 0x%x\r\n", __func__,
                    firm_addr, firm_size);

    segment.offset = firm_addr;
    segment.length = firm_size;

    /* load and validate image header */
    read_from_seghdr(&segment, &ih, sizeof(ih));
    if (ih.magic_str != FW_MAGIC_STR || ih.magic_sig != FW_MAGIC_SIG) {
        writel((readel(nvram_addr) | BAD_FIRMWARE_SIG), nvram_addr);
        dbg("firmware magic not found\r\n");
        return FAIL;
    }

    if ((ih.entry & 0x1f000000) == 0x1f000000) {
        dbg("XIP image found, enabling flashc\r\n");

        PMU->PERI1_CLK_DIV.BF.FLASH_CLK_DIV = 4;
        /* Sample data on the falling edge.
         * Sampling on rising edge does not
         * meet timing requirements at 50MHz
         */
        FLASHC->FCTR.BF.CLK_CAPT_EDGE = 1;

        /* Set FLASH QUAD mode type (continuous/non-continuous mode)
         * based on the flash */
        ret = FLASH_SetCmdType_QuadModeRead(id);
        if (ret == ERROR) {
            writel((readel(nvram_addr) | BAD_FLASH_JEDECID),
                    nvram_addr);
            dbg("invalid flash identifier read\r\n");
            return FAIL;
        }

        /* Enable cache mode and disable SRAM mode */
        FLASHC->FCCR.BF.CACHE_EN = ENABLE;
        FLASHC->FCCR.BF.SRAM_MODE_EN = DISABLE;

        /* Enable cache hit/miss counters */
        FLASHC->FCACR.BF.HIT_CNT_EN = ENABLE;
        FLASHC->FCACR.BF.MISS_CNT_EN = ENABLE;

        /* Set correct clock prescale value, default is 0x2 */
        FLASHC->FCCR.BF.CLK_PRESCALE = FLASHC_CLK_DIVIDE_1;

        /* Enable Flash Controller */
        FLASHC->FCCR.BF.FLASHC_PAD_EN = ENABLE;
    }

    int i;
    for (i = 0; i < SEG_CNT; i++)
        read_from_seghdr(&segment, &sh[i], sizeof(struct seg_hdr));

    int index = 0;
    while (ih.seg_cnt--) {
        sh[index].offset += firm_addr;

        switch (sh[index].type) {
        case FW_BLK_LOADABLE_SEGMENT:
			
            if ((sh[index].laddr & 0x1f000000) == 0x1f000000) {
#ifndef DISABLE_SECURE_BOOT			
				if (secure_boot_feed_data((uint8_t *)
					sh[index].offset + 0x1f000000,
					sh[index].len))
					return FAIL;
#endif	
                break;
            }
			dbg("loading fw segment\r\n");
            if (secure_read_flash((void *)sh[index].laddr,
                sh[index].offset, sh[index].len,
                sh[index].crc))
                return FAIL;
            break;

        case FW_BLK_POKE_DATA:
            data_ptr = (void *)sh[index].laddr;
            value = sh[index].len;
            *data_ptr = value;
            break;

        case FW_BLK_FN_CALL:
            writel((readel(nvram_addr) | BOOT_SUCCESS),
                            nvram_addr);
            dbg("fw-boot success\r\n");
            fn_ptr = (void *)sh[index].laddr;
            value = sh[index].len;
            if (fn_ptr(value) != 0)
                return FAIL;
            break;

        case FW_BLK_ANCILLARY:
            /* skip over it */
            break;

        default:
            dbg("invalid FW block found\r\n");
            return FAIL;
        }
        index++;
    }

    if ((ih.entry & 0x1f000000) == 0x1f000000) {
        if (firm_addr & 0x3) {
            dbg("Err...offset value should be word aligned\r\n");
            return FAIL;
        }
        /* Set flash controller offset register */
        FLASHC->FAOFFR.BF.OFFSET_VAL = firm_addr;
        FLASHC->FCACR.BF.OFFSET_EN = ENABLE;
    }
#ifndef DISABLE_SECURE_BOOT
	/* Copy keystorage to NVRAM for further use by firmware */
	memcpy((void *)SB_KEYSTORE_ADDR, &_keystore_start, keystore_len);
#endif
    /* Hand over execution */
    writel((readel(nvram_addr) | BOOT_SUCCESS), nvram_addr);
    dbg("fw-boot success\r\n");
	fn_ptr = (void *)ih.entry;
	fn_ptr((uint32_t)NULL);
	return FAIL;
}

int keystore_search(uint8_t *keystore_buf,
            uint8_t type, uint8_t *value, struct tlv_entry **t)
{
    struct tlv_hdr *hdr = (struct tlv_hdr *) keystore_buf;
    struct tlv_entry *tentry =
        (struct tlv_entry *) (keystore_buf + sizeof(struct tlv_hdr));

    while ((uint8_t *)tentry < keystore_buf + hdr->len) {
        if (tentry->type == type) {
            if (value && tentry->len == 1)
                memcpy(value, tentry->value, 1);
            else if (t)
                *t = tentry;
            else
                return FAIL;
            return 0;
        }
        tentry = (struct tlv_entry *) ((uint8_t *)tentry +
                sizeof(tentry->type) + sizeof(tentry->len)
                + tentry->len);
    }

    return FAIL;
}

#ifndef DISABLE_SECURE_BOOT			
static int validate_and_parse_keystore(boot2_ks_hdr *hdr_t)
{
    int ret;
    uint32_t crc;

    crc = crc32((uint8_t *) hdr_t + sizeof(boot2_ks_hdr),
                hdr_t->len - sizeof(boot2_ks_hdr), 0);
    if (crc != hdr_t->crc) {
        dbg("CRC verification failed for keystore\r\n");
        writel((readel(sb_e) | SB_ERR_BAD_KS), sb_e);
        return -1;
    }

    /* Is firmware image signed? */
    ret = keystore_search((uint8_t *) hdr_t,
                KEY_FW_SIGNING_ALGO, &sb.sign_algo, NULL);
    if (!ret && sb.sign_algo != NO_SIGN) {
        /* Firmware image is signed, get public key */
        ret = keystore_search((uint8_t *) hdr_t,
                KEY_FW_PUBLIC_KEY, NULL, &sb.public_key);
        if (ret) {
            writel((readel(sb_e) | SB_ERR_BAD_PUB_KEY), sb_e);
            return ret;
        }
        /* Firmware image is signed, get hash algorithm type */
        ret = keystore_search((uint8_t *) hdr_t,
                KEY_FW_HASH_ALGO, &sb.hash_algo, NULL);
        if (ret) {
            dbg("Err...fw hash algo not found\r\n");
            return ret;
        }
    } else {
        sb.sign_algo = NO_SIGN;
        writel((readel(sb_e) | SB_ERR_NO_SIGN), sb_e);
    }

    /* Is firmware image encrypted? */
    ret = keystore_search((uint8_t *) hdr_t,
            KEY_FW_ENCRYPT_ALGO, &sb.encrypt_algo, NULL);
    if (!ret && sb.encrypt_algo != NO_ENCRYPT) {
        /* Firmware image is encrypted, get decryption key */
        ret = keystore_search((uint8_t *) hdr_t,
                KEY_FW_DECRYPT_KEY, NULL, &sb.decrypt_key);
        if (ret) {
            writel((readel(sb_e) | SB_ERR_BAD_DECRYPT_KEY),
                                sb_e);
            return ret;
        }
    } else {
        sb.encrypt_algo = NO_ENCRYPT;
        writel((readel(sb_e) | SB_ERR_NO_ENCRYPT), sb_e);
    }

    /* Save keystorage length for future use */
    keystore_len = hdr_t->len;

    return 0;
}

static int validate_and_parse_image_hdr(uint32_t firm_addr, uint32_t *offset)
{
	int ret = -1;
	img_sec_hdr hdr;

	read_flash((uint8_t *)&hdr, firm_addr, sizeof(img_sec_hdr), 0);
	if (hdr.magic == SEC_FW_MAGIC_SIG) {
		dbg("found fw image header @%x\r\n", firm_addr);
		if (hdr.len > SB_SEC_IMG_HDR_SIZE) {
			dbg("Err...invalid fw image header size %d\r\n",
								hdr.len);
			writel((readel(sb_e) | SB_ERR_BAD_FW_HDR_LEN), sb_e);
			return -1;
		}
		memcpy(sec_fw_image_hdr, &hdr, sizeof(img_sec_hdr));
		read_flash(sec_fw_image_hdr + sizeof(img_sec_hdr),
				firm_addr + sizeof(img_sec_hdr),
				hdr.len - sizeof(img_sec_hdr), 0);

		uint32_t crc = crc32((uint8_t *) sec_fw_image_hdr
				+ sizeof(img_sec_hdr),
				hdr.len - sizeof(img_sec_hdr), 0);
		if (crc != hdr.crc) {
			dbg("CRC verification failed for image header\r\n");
			writel((readel(sb_e) | SB_ERR_BAD_FW_HDR), sb_e);
			return -1;
		}

		struct tlv_entry *entry;
		ret = keystore_search((uint8_t *) sec_fw_image_hdr,
				KEY_FW_IMAGE_LEN, NULL, &entry);
		if (ret) {
			dbg("Err...firmware image length not found\r\n");
			writel((readel(sb_e) | SB_ERR_NO_FW_LEN), sb_e);
			return ret;
		} else {
			sb.fw_img_len = (entry->value[3] << 24)
					| (entry->value[2] << 16)
					| (entry->value[1] << 8)
					| (entry->value[0]);
		}

		if (sb.encrypt_algo != NO_ENCRYPT) {
			ret = keystore_search((uint8_t *) sec_fw_image_hdr,
					KEY_FW_NONCE, NULL, &sb.nonce);
			if (ret) {
				dbg("Err...firmware nonce not found\r\n");
				writel((readel(sb_e) | SB_ERR_BAD_NONCE),
								sb_e);
				return ret;
			}
		}

		if (sb.sign_algo != NO_SIGN) {
			ret = keystore_search((uint8_t *) sec_fw_image_hdr,
				KEY_FW_SIGNATURE, NULL, &sb.digital_sig);
			if (ret) {
				dbg("Err...firmware signature not found\r\n");
				writel((readel(sb_e) | SB_ERR_BAD_SIGN),
								sb_e);
			}
		}
	} else {
		writel((readel(sb_e) | SB_ERR_BAD_FW_HDR), sb_e);
	}

	if (!ret && offset)
		*offset = hdr.len;

	return ret;
}
#endif

int boot2_main( uint8_t will_load_firmware )
{
    int ret;
    short index;
    bool p_num;
	bool secure_boot = false;
    uint32_t boot_flags;
    struct partition_entry p_entry[2];
    struct partition_table p_table;
    uint32_t part_addr;
	uint32_t img_offset = 0;
	uint32_t otp[5];
	unsigned long *otp_nvram_addr = (unsigned long *)FLASH_OTP_NVRAM_ADDR;

    /* Read flash JEDECID and set appropriate configuration first */
    id = FLASH_GetJEDECID();
    ret = FLASH_SetConfig(id);
    dbg("jedecID %08x\r\n", id);
    if (ret == ERROR) {
        writel((readel(nvram_addr) | CANNOT_CONFIG_JEDECID),
                nvram_addr);
        dbg("Cannot set the flash configuration\r\n");
        for (;;)
            ;
    }
#ifdef CONFIG_ENABLE_MXCHIP
	/* read out flash OTP data, save to nvram */
	FLASH_ENSO();
	ret = FLASH_Read(FLASH_FAST_READ_QUAD_IO, FLASH_OTP_OFFSET, otp, 20);
	FLASH_EXSO();
	otp_nvram_addr[0] = otp[0];
	otp_nvram_addr[1] = otp[1];
	otp_nvram_addr[2] = otp[2];
	otp_nvram_addr[3] = otp[3];
	otp_nvram_addr[4] = otp[4];
	
	if ((MicoGpioInputGet((mico_gpio_t)BOOT_SEL) == false) &&
		  (MicoGpioInputGet((mico_gpio_t)EasyLink_BUTTON) == false) &&
		  (MicoGpioInputGet((mico_gpio_t)MFG_SEL) == true)) {
		dbg("load ATE firmware\r\n");
		load_firmware(0x160000, 0x40000);
	}
#endif
    /* Select active partition table */
    boot_flags = 0;
	ret = select_active_partition_table(&part_addr);
    boot_flags |= ret;

    /* Read out partition table */
	dbg("reading partition table @0x%x\r\n", part_addr);
	load_partition_table(&p_table, part_addr);
    part_addr += sizeof(struct partition_table);

    /* Read out firmware partition entries */
	if (p_table.partition_entries_no > 0) {
        index = 0;
		load_partition_entry(&p_table, &p_entry[0], part_addr, &index);
        part_addr += sizeof(struct partition_entry) * index;
		load_partition_entry(&p_table, &p_entry[1], part_addr, &index);

		dbg("gen levno: %d,%d\r\n",
			   p_entry[0].gen_level, p_entry[1].gen_level);

        /* select partition with highest generation number */
        p_num = (p_entry[1].gen_level > p_entry[0].gen_level);
	} else {
        /* No partition entries found, load default */
        p_num = 0;
        p_entry[0].start = DEFAULT_FIRMWARE_OFFSET;
        p_entry[0].size = DEFAULT_FIRMWARE_SIZE;
        p_entry[0].gen_level = 1;
    }

#ifndef DISABLE_SECURE_BOOT			
	/* See, if keystore exists for secure boot scheme */
	boot2_ks_hdr *hdr_t = (boot2_ks_hdr *)&_keystore_start;
	if (hdr_t->magic == SEC_KEYSTORE_MAGIC_SIG) {
		/* Keystore is available, boot only trusted fw image (?) */
		ret = validate_and_parse_keystore(hdr_t);
		if (ret) {
			dbg("Valid keystore not found %d\r\n", ret);
			/* We're toast... */
			for (;;)
				;
		}
		if (sb.sign_algo != NO_SIGN ||
				sb.encrypt_algo != NO_ENCRYPT) {
			secure_boot = true;
			boot_flags |= BOOT_SECURE_FW_ENABLE;
			if (sb.sign_algo != NO_SIGN)
				boot_flags |= BOOT_SECURE_SIGN_FW;
			if (sb.encrypt_algo != NO_ENCRYPT)
				boot_flags |= BOOT_SECURE_ENCRYPT_FW;
		}
	} else {
		writel((readel(sb_e) | SB_ERR_NO_KS), sb_e);
	}
#endif

	if( will_load_firmware == 0 )
	    return 0;

	/* Try to boot both primary and secondary fw images */
    int i;
	for (i = 0; i < 2; i++) {
        /* store boot flags in memory for firmware consumption */
        *BOOT_FLAGS = boot_flags | p_num;
#ifndef DISABLE_SECURE_BOOT			
        /* Parse secure firmware image header and initialize secure
         * boot context with same */
        if (secure_boot) {
            ret = validate_and_parse_image_hdr
                    (p_entry[p_num].start, &img_offset);
            if (ret) {
                dbg("Invalid secure fw img hdr %d\r\n", ret);
                /* Invalidate generation level */
                p_entry[p_num].gen_level = 0;
            } else {
                ret = secure_boot_firmware(&sb, &priv);
                if (ret) {
                    dbg("Secure boot init failed %d\r\n",
                                    ret);
                    writel((readel(sb_e) |
                        SB_ERR_INIT_FAIL), sb_e);
                    /* We're toast */
                    for (;;)
                        ;
                }
            }
        }
#endif
		if (p_entry[p_num].gen_level >= 1)
			load_firmware(p_entry[p_num].start + img_offset,
					p_entry[p_num].size);

        /* we get here only if selected partition/firmware is bad */
        boot_flags |= BOOT_MAIN_FIRMWARE_BAD_CRC;
        writel((readel(nvram_addr) | BAD_FIRMWARE_IMG), nvram_addr);
#ifndef DISABLE_SECURE_BOOT			
		/* cleanup secure boot context */
		secure_boot_cleanup(priv);
#endif
        /* Backup firmware: partition selection has to be flipped */
        p_num ^= 1;
        dbg("Trying backup firmware image boot\r\n");
    }

	writel((readel(nvram_addr) | BOOT_FAILED), nvram_addr);
	dbg("fw-boot failed boot_flags(0x%08x), error(0x%08x)\r\n",
					boot_flags, readel(nvram_addr));
	if (secure_boot)
		dbg("secure_boot_error(0x%08x)\r\n", readel(sb_e));
#ifdef UART_DEBUG
	uart_cleanup();
#endif
	/* Nothing succeeded. We're toast... */
	for (;;)
		;
}
