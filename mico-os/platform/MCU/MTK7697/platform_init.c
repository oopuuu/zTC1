/**
 ******************************************************************************
 * @file    platform_init.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide functions called by MICO to drive stm32f2xx
 *          platform: - e.g. power save, reboot, platform initialize
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "platform_peripheral.h"
#include "hal_sys.h"
#include "mt7637_cm4_hw_memmap.h"
#include "platform.h"
#include "platform_config.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
#include "crt0.h"
#include "mico_rtos.h"
#include "platform_init.h"
#include "../../GCC/stdio_newlib.h"
#include "MiCODrivers/MiCODriverFlash.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
typedef enum
{
	SF_UNDEF = 0, SPI = 1, SPIQ = 2, QPI = 3
} SF_MODE_Enum;
/******************************************************
 *               Function Declarations
 ******************************************************/
extern void top_xtal_init(void);
extern void flash_switch_mode(unsigned long mode);
extern void cmnCpuClkConfigureTo64M(void);

/******************************************************
 *               Variables Definitions
 ******************************************************/

extern platform_uart_t platform_uart_peripherals[];
extern platform_uart_driver_t platform_uart_drivers[];

#ifndef MICO_DISABLE_STDIO
static const platform_uart_config_t stdio_uart_config =
		{ .baud_rate = STDIO_UART_BAUDRATE, .data_width = DATA_WIDTH_8BIT, .parity =
				NO_PARITY, .stop_bits = STOP_BITS_1, .flow_control =
				FLOW_CONTROL_DISABLED, .flags = 0, };

mico_mutex_t stdio_rx_mutex;
mico_mutex_t stdio_tx_mutex;
#endif /* #ifndef MICO_DISABLE_STDIO */

#ifdef NO_MICO_RTOS
extern uint32_t SystemCoreClock;
#endif

#ifdef BOOTLOADER
extern void ResetISR(void);
extern unsigned long __isr_vector[32];
extern void NVIC_SetupVectorTable(uint32_t addr);
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/

/*
 * Boot to application
 */
void startApplication(uint32_t app_addr)
{
	platform_uart_deinit(&platform_uart_drivers[STDIO_UART]);
	/* Disable system tick timer interrupt */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	/* Clear all external interrupt */
	for (int i = 0; i < 8; i++) {
		/* ICET: Interrupt Clear Enable Register */
		NVIC->ICER[i] = 0xFFFFFFFF;
	}

	uint32_t sp_addr, pc_addr;

	if(app_addr == MicoFlashGetInfo(MICO_PARTITION_ATE)->partition_start_addr){
		sp_addr = *(uint32_t*)app_addr;
		pc_addr = *(uint32_t*)(app_addr + 4) | 0x1; /* switch to ARM state */
	}else{
		sp_addr = CM4_SYSRAM_END;
		pc_addr = (app_addr + 0x10000000) | 0x1; /* switch to ARM state */
	}
	__asm(
			"    isb                        \n\t"
			"    dsb                        \n\t"
			"    mov   sp, %0               \n\t"
			"    mov   r0, %1               \n\t"
			"    bx    r0                   \n\t"
			:: "r" (sp_addr), "r" (pc_addr) :
	);
}

void platform_mcu_reset(void)
{
	hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
}

void init_clocks(void)
{
	top_xtal_init();
	cmnCpuClkConfigureTo64M();
#ifdef NO_MICO_RTOS
	SysTick_Config( SystemCoreClock / 1000 );
#endif
}

void init_memory(void)
{
	flash_switch_mode(SPIQ);
}

void init_architecture(void)
{
#ifdef BOOTLOADER
	__isr_vector[1] = (unsigned long)ResetISR; /* Set reset handler */
	NVIC_SetupVectorTable((unsigned long)__isr_vector); /* Relocate vector table to RAM */
#endif

	/* Initialise the interrupt priorities to a priority lower than 0
	 * so that the BASEPRI register can mask them */
	for (uint8_t i = 0; i < 81; i++) {
		/* IP: Interrupt Priority */
		NVIC->IP[i] = 0xff;
	}
	/* Enable interrupt(Clear PRIMASK to 0) */
	/* If PRIMASK = 1, the mask all interrupts except NMI and hard fault */
	__enable_irq();

	/* Initialize STDIO UART */
#ifndef MICO_DISABLE_STDIO
#ifndef NO_MICO_RTOS
	mico_rtos_init_mutex(&stdio_tx_mutex);
	mico_rtos_unlock_mutex(&stdio_tx_mutex);
	mico_rtos_init_mutex(&stdio_rx_mutex);
	mico_rtos_unlock_mutex(&stdio_rx_mutex);
#endif
	platform_uart_init(&platform_uart_drivers[STDIO_UART],
			&platform_uart_peripherals[STDIO_UART], &stdio_uart_config, NULL);
#endif
}

static char global_cid[25] = { 0 };
const char *mico_generate_cid(uint8_t* length)
{
	uint32_t temp0, temp1, temp2;
	uint8_t temp[12];
	temp0 = *(volatile uint32_t*) (0x1FFF7A10);
	temp1 = *(volatile uint32_t*) (0x1FFF7A14);
	temp2 = *(volatile uint32_t*) (0x1FFF7A18);

	temp[0] = (uint8_t) (temp0 & 0x000000FF);
	temp[1] = (uint8_t) ((temp0 & 0x0000FF00) >> 8);
	temp[2] = (uint8_t) ((temp0 & 0x00FF0000) >> 16);
	temp[3] = (uint8_t) ((temp0 & 0xFF000000) >> 24);
	temp[4] = (uint8_t) (temp1 & 0x000000FF);
	temp[5] = (uint8_t) ((temp1 & 0x0000FF00) >> 8);
	temp[6] = (uint8_t) ((temp1 & 0x00FF0000) >> 16);
	temp[7] = (uint8_t) ((temp1 & 0xFF000000) >> 24);
	temp[8] = (uint8_t) (temp2 & 0x000000FF);
	temp[9] = (uint8_t) ((temp2 & 0x0000FF00) >> 8);
	temp[10] = (uint8_t) ((temp2 & 0x00FF0000) >> 16);
	temp[11] = (uint8_t) ((temp2 & 0xFF000000) >> 24);
	sprintf(global_cid, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
			temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7],
			temp[8], temp[9], temp[10], temp[11]);
	*length = 12;
	return global_cid;
}

void _exit( int status )
{
    while (1)
    {
        /* do nothing */
    }
}

#ifndef NO_MICO_RTOS
#include "FreeRTOS.h"
/************** wrap C library functions **************/
void * __wrap_malloc (size_t size)
{
	return pvPortMalloc(size);
}

void * __wrap__malloc_r (void *p, size_t size)
{
	
	return pvPortMalloc(size);
}

void __wrap_free (void *pv)
{
	vPortFree(pv);
}

void * __wrap_calloc (size_t a, size_t b)
{
	return pvPortCalloc(a, b);
}

void * __wrap_realloc (void* pv, size_t size)
{
	return pvPortRealloc(pv, size);
}
/************** end **************/
#endif

/******************************************************
 *            NO-OS Functions
 ******************************************************/

#ifdef NO_MICO_RTOS
static volatile uint32_t no_os_tick = 0;

void SysTick_Handler(void)
{
	no_os_tick ++;
	platform_watchdog_kick( );
}

uint32_t mico_get_time_no_os(void)
{
	return no_os_tick;
}
#endif
