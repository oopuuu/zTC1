/*
 * Copyright (C) 2008-2013 Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef _BOOT2_H_
#define _BOOT2_H_

#include <stdint.h>

/* Nvram address word to store debugging information */
#define NVRAM_ADDR 0x480C0020
#define SB_ERR_ADDR 0x480C0028

/* Boot2 progress status */
#define SYS_INIT (1<<0)
#define FLASH_INIT (1<<1)
#define PRIMARY_FIRMWARE (1<<2)

/* Boot2 error status */
#define BAD_PART_TABLE_CRC (1<<16)
#define BAD_PART_ENTRY_CRC (1<<17)
#define BAD_FIRMWARE_CRC (1<<18)
#define BAD_PART_TABLE_MAGIC (1<<19)
#define BAD_FIRMWARE_SIG (1<<20)
#define BAD_FIRMWARE_IMG (1<<21)
#define BAD_FLASH_JEDECID (1<<22)
#define CANNOT_CONFIG_JEDECID (1<<23)

/* Boot2 success or failure status */
#define BOOT_SUCCESS (0x80000000)
#define BOOT_FAILED (0x00800000)

extern unsigned long _bss;
extern unsigned long _ebss;
extern unsigned long _estack;

#ifdef UART_DEBUG
#define dbg(_fmt_, ...) \
	uart_dbg("[boot2] "_fmt_, ## __VA_ARGS__)
void uart_dbg(const char *format, ...);
void uart_cleanup(void);
#elif SEMIHOST_DEBUG
#define dbg	printf
#else
#define	dbg(...)
#endif

extern unsigned long *nvram_addr;
extern unsigned long *sb_e;

static inline void writel(uint32_t data, void *addr)
{
	*((volatile uint32_t *)addr) = data;
}

static inline uint32_t readel(void *addr)
{
	return *((volatile uint32_t *)addr);
}

int boot2_main( uint8_t will_load_firmware );
extern unsigned long _heap_end, _heap_start, _keystore_start;
void *ROM_pvPortMalloc(size_t xWantedSize);
void ROM_vPortFree(void *pv);
void *ROM_pvPortReAlloc(void *pv, size_t xWantedSize);
void ROM_prvHeapInit(unsigned char *startAddress, unsigned char *endAddress);

#endif
