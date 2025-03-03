/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __MC200_SPI_FLASH_H__
#define __MC200_SPI_FLASH_H__

#include <stdint.h>
#include <stdbool.h>
#include <mc200_ssp.h>

#define AT25_SECTOR_SIZE          0x1000   /*!< 4KB */
#define AT25_32K_BLOCK_SIZE       0x8000   /*!< 32KB */
#define AT25_64K_BLOCK_SIZE       0x10000  /*!< 64KB */

#define DATAFLASH_TOTAL_SIZE            0x100000
#define DATAFLASH_PAGE_SIZE             0x100
/** Read array command code. */
#define AT25_READ_ARRAY             0x0B
/** Read array (low frequency) command code. */
#define AT25_READ_ARRAY_LF          0x03
/** Block erase command code (4K block). */
#define AT25_BLOCK_ERASE_4K         0x20
/** Block erase command code (32K block). */
#define AT25_BLOCK_ERASE_32K        0x52
/** Block erase command code (64K block). */
#define AT25_BLOCK_ERASE_64K        0xD8
/** Chip erase command code 1. */
#define AT25_CHIP_ERASE_1           0x60
/** Chip erase command code 2. */
#define AT25_CHIP_ERASE_2           0xC7
/** Byte/page program command code. */
#define AT25_BYTE_PAGE_PROGRAM      0x02
/** Sequential program mode command code 1. */
#define AT25_SEQUENTIAL_PROGRAM_1   0xAD
/** Sequential program mode command code 2. */
#define AT25_SEQUENTIAL_PROGRAM_2   0xAF
/** Write enable command code. */
#define AT25_WRITE_ENABLE           0x06
/** Write disable command code. */
#define AT25_WRITE_DISABLE          0x04
/** Protect sector command code. */
#define AT25_PROTECT_SECTOR         0x36
/** Unprotect sector command code. */
#define AT25_UNPROTECT_SECTOR       0x39
/** Read sector protection registers command code. */
#define AT25_READ_SECTOR_PROT       0x3C
/** Read status register command code. */
#define AT25_READ_STATUS            0x05
/** Write status register command code. */
#define AT25_WRITE_STATUS           0x01
/** Read manufacturer and device ID command code. */
#define AT25_READ_JEDEC_ID          0x9F
/** Deep power-down command code. */
#define AT25_DEEP_PDOWN             0xB9
/** Resume from deep power-down command code. */
#define AT25_RES_DEEP_PDOWN         0xAB

/** Device ready/busy status bit. */
#define AT25_STATUS_RDYBSY          (1 << 0)
/** Erase/program error bit. */
#define AT25_STATUS_EPE             (1 << 5)

typedef struct spi_flash {
	SSP_ID_Type port_id;
	int cs;
	bool level;
} spi_flash_t;

typedef struct spi_command {
	uint8_t *data;
	uint8_t *cmd;
	uint16_t data_size;
	uint8_t cmd_size:7, cmd_rx:1;
	uint8_t dummy_size;
} spi_cmd_t;

/* AT25 command struct */
typedef struct at25_command {
	uint32_t op_code:8,	/* Opcode */
	address_h:8,	/* Address high byte */
	address_m:8,	/* Address medium byte */
	address_l:8;	/* Address low byte */
} at25_cmd_t;

int spi_flash_init();
int spi_flash_read(uint8_t *p_buf, uint32_t size, uint32_t address);
int spi_flash_write(uint8_t *p_buf, uint32_t size, uint32_t address);
int spi_flash_erase(uint32_t address, uint32_t end_address);
int spi_flash_erase_block_4K(uint32_t address);
int spi_flash_erase_block_32K(uint32_t address);
int spi_flash_erase_block_64K(uint32_t address);
int spi_flash_eraseall();
int spi_flash_read_id(uint8_t *p_id);
int spi_flash_wakeup();
int spi_flash_sleep();

#endif /* __MC200_SPI_FLASH_H__ */
