/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#include "mc200.h"
#include "stdio.h"
#include "mc200_uart.h"
#include "mc200_pmu.h"
#include "mc200_clock.h"
#include "board.h"
#include "core_cm3.h"
#include "board.h"
#include "wmstdio.h"
#include "wmlog.h"
#include "wm_os.h"
#include "boot_flags.h"
#include "critical_error.h"

typedef struct stframe {
	int r0;
	int r1;
	int r2;
	int r3;
	int r12;
	int lr;
	int pc;
	int psr;
} stframe_t;
#ifdef CONFIG_LL_DEBUG
static int stptr;
#endif

/* Default Fault Handlers. */
void Reset_IRQHandler(void) __attribute__ ((naked));
void HardFault_IRQHandler(void) __attribute__ ((naked));

void WEAK NMI_IRQHandler(void);
void WEAK MemManage_IRQHandler(void);
void WEAK BusFault_IRQHandler(void);
void WEAK UsageFault_IRQHandler(void);
void WEAK SVC_IRQHandler(void);
void WEAK DebugMonitor_IRQHandler(void);
void WEAK PendSV_IRQHandler(void);
void WEAK SysTick_IRQHandler(void);
void WEAK ExtPin0_IRQHandler(void);
void WEAK ExtPin1_IRQHandler(void);
void WEAK RTC_IRQHandler(void);
void WEAK CRC_IRQHandler(void);
void WEAK AES_IRQHandler(void);
void WEAK I2C0_IRQHandler(void);
void WEAK I2C1_IRQHandler(void);
void WEAK I2C2_IRQHandler(void);
void WEAK DMA_IRQHandler(void);
void WEAK GPIO_IRQHandler(void);
void WEAK SSP0_IRQHandler(void);
void WEAK SSP1_IRQHandler(void);
void WEAK SSP2_IRQHandler(void);
void WEAK QSPI0_IRQHandler(void);
void WEAK GPT0_IRQHandler(void);
void WEAK GPT1_IRQHandler(void);
void WEAK GPT2_IRQHandler(void);
void WEAK GPT3_IRQHandler(void);
void WEAK UART0_IRQHandler(void);
void WEAK UART1_IRQHandler(void);
void WEAK UART2_IRQHandler(void);
void WEAK UART3_IRQHandler(void);
void WEAK WDT_IRQHandler(void);
void WEAK ADC1_IRQHandler(void);
void WEAK ADC0_IRQHandler(void);
void WEAK DAC_IRQHandler(void);
void WEAK ACOMPWKUP_IRQHandler(void);
void WEAK ACOMP_IRQHandler(void);
void WEAK SDIO_IRQHandler(void);
void WEAK USB_IRQHandler(void);
void WEAK ExtPin2_IRQHandler(void);
void WEAK PLL_IRQHandler(void);
void WEAK QSPI1_IRQHandler(void);
void WEAK RC32M_IRQHandler(void);
void WEAK ExtPin3_IRQHandler(void);
void WEAK ExtPin4_IRQHandler(void);
void WEAK ExtPin5_IRQHandler(void);
void WEAK ExtPin6_IRQHandler(void);
void WEAK ExtPin7_IRQHandler(void);
void WEAK ExtPin8_IRQHandler(void);
void WEAK ExtPin9_IRQHandler(void);
void WEAK ExtPin10_IRQHandler(void);
void WEAK ExtPin11_IRQHandler(void);
void WEAK ExtPin12_IRQHandler(void);
void WEAK ExtPin13_IRQHandler(void);
void WEAK ExtPin14_IRQHandler(void);
void WEAK ExtPin15_IRQHandler(void);
void WEAK ExtPin16_IRQHandler(void);
void WEAK ExtPin17_IRQHandler(void);
void WEAK ExtPin18_IRQHandler(void);
void WEAK ExtPin19_IRQHandler(void);
void WEAK ExtPin20_IRQHandler(void);
void WEAK ExtPin21_IRQHandler(void);
void WEAK ExtPin22_IRQHandler(void);
void WEAK ExtPin23_IRQHandler(void);
void WEAK ExtPin24_IRQHandler(void);
void WEAK ExtPin25_IRQHandler(void);
void WEAK ExtPin26_IRQHandler(void);
void WEAK ExtPin27_IRQHandler(void);
void WEAK ExtPin28_IRQHandler(void);
void WEAK ULPCOMP_IRQHandler(void);
void WEAK BRNDET_IRQHandler(void);

/* System main stack pointer address */
extern unsigned long _estack;

/*
 * The minimal vector table for a Cortex-M3.  Note that the proper constructs
 * must be placed on this to ensure that it ends up at physical address
 * 0x0000.0000.
 */
__attribute__ ((used, section(".isr_vector")))
void (*const __vector_table[]) (void) = {
	/* Cortex-M3 common exception */
	(void (*)(void))(&_estack),
	    Reset_IRQHandler,
	    NMI_IRQHandler,
	    HardFault_IRQHandler,
	    MemManage_IRQHandler,
	    BusFault_IRQHandler,
	    UsageFault_IRQHandler,
	    0,
	    0,
	    0,
	    0,
	    SVC_IRQHandler,
	    DebugMonitor_IRQHandler,
	    0,
	    PendSV_IRQHandler,
	    SysTick_IRQHandler,
	    /* Marvell specific interrupts */
	    ExtPin0_IRQHandler,
	    ExtPin1_IRQHandler,
	    RTC_IRQHandler,
	    CRC_IRQHandler,
	    AES_IRQHandler,
	    I2C0_IRQHandler,
	    I2C1_IRQHandler,
	    I2C2_IRQHandler,
	    DMA_IRQHandler,
	    GPIO_IRQHandler,
	    SSP0_IRQHandler,
	    SSP1_IRQHandler,
	    SSP2_IRQHandler,
	    QSPI0_IRQHandler,
	    GPT0_IRQHandler,
	    GPT1_IRQHandler,
	    GPT2_IRQHandler,
	    GPT3_IRQHandler,
	    UART0_IRQHandler,
	    UART1_IRQHandler,
	    UART2_IRQHandler,
	    UART3_IRQHandler,
	    WDT_IRQHandler,
	    ADC1_IRQHandler,
	    ADC0_IRQHandler,
	    DAC_IRQHandler,
	    ACOMPWKUP_IRQHandler,
	    ACOMP_IRQHandler,
	    SDIO_IRQHandler,
	    USB_IRQHandler,
	    ExtPin2_IRQHandler,
	    PLL_IRQHandler,
	    QSPI1_IRQHandler,
	    RC32M_IRQHandler,
	    ExtPin3_IRQHandler,
	    ExtPin4_IRQHandler,
	    ExtPin5_IRQHandler,
	    ExtPin6_IRQHandler,
	    ExtPin7_IRQHandler,
	    ExtPin8_IRQHandler,
	    ExtPin9_IRQHandler,
	    ExtPin10_IRQHandler,
	    ExtPin11_IRQHandler,
	    ExtPin12_IRQHandler,
	    ExtPin13_IRQHandler,
	    ExtPin14_IRQHandler,
	    ExtPin15_IRQHandler,
	    ExtPin16_IRQHandler,
	    ExtPin17_IRQHandler,
	    ExtPin18_IRQHandler,
	    ExtPin19_IRQHandler,
	    ExtPin20_IRQHandler,
	    ExtPin21_IRQHandler,
	    ExtPin22_IRQHandler,
	    ExtPin23_IRQHandler,
	    ExtPin24_IRQHandler,
	    ExtPin25_IRQHandler,
	    ExtPin26_IRQHandler,
	    ExtPin27_IRQHandler,
	    ExtPin28_IRQHandler,
	    ULPCOMP_IRQHandler,
	    BRNDET_IRQHandler,};

/*
 * The following are constructs created by the linker, indicating where the
 * ".bss", ".iobufs" and ".nvram" segments reside in memory.
 */
extern unsigned long _bss;
extern unsigned long _ebss;
extern unsigned long _iobufs_start;
extern unsigned long _iobufs_end;
extern unsigned long _nvram_begin;
extern unsigned long _nvram_end;

/*
 * This is the code that gets called when the processor first starts execution
 * following a reset event. Only the absolutely necessary set is performed,
 * after which the os_init() routine is called, to start kernel. os_init()
 * after starting scheduler, hands over control to application specific entry
 * point, main() or user_app_init() as defined.
 */
void Reset_IRQHandler(void)
{
	__asm volatile ("mov sp, %0" : : "r" (&_estack));
	__asm volatile ("b Reset_IRQHandler_C");
}

__attribute__((used))
void Reset_IRQHandler_C(void)
{
	/* Zero fill the bss segment. */
	memset(&_bss, 0x00, ((unsigned) &_ebss - (unsigned) &_bss));

	/* Zero fill iobufs segment */
	memset(&_iobufs_start, 0, ((unsigned) &_iobufs_end -
				(unsigned) &_iobufs_start));

	/* Zero fill part of nvram if this is a reboot */
	if (PMU->PWR_MODE.BF.PWR_MODE != PMU_PM4) {
		memset(&_nvram_begin, 0, ((unsigned) &_nvram_end -
				(unsigned) &_nvram_begin));
	}

	/*
	 * Vector table relocation:
	 *
	 * 1. Bit 29 decides whether CODE = 0 or SRAM = 1 area.
	 * 2. Bit 0-6 are reserved, hence address should be aligned to
	 *    32 word boundary -- minimal.
	 *
	 * See programming VTOR in cortex-m3 trm for more details.
	 */
	if ((unsigned) __vector_table >= 0x20000000)
		SCB->VTOR = ((uint32_t) __vector_table) & 0x3FFFFF80;
	else
		SCB->VTOR = ((uint32_t) __vector_table) & 0x1FFFFF80;

	/* 4 pre-emption, 4 subpriority bits */
	NVIC_SetPriorityGrouping(4);

	/* Turn ON different power domains. */
	PMU_PowerOnVDDIO(PMU_VDDIO_AON);
	PMU_PowerOnVDDIO(PMU_VDDIO_FL);
	PMU_PowerOnVDDIO(PMU_VDDIO_SDIO);
	PMU_PowerOnVDDIO(PMU_VDDIO_D0);
	PMU_PowerOnVDDIO(PMU_VDDIO_D1);
	PMU_PowerOnVDDIO(PMU_VDDIO_D2);

	/*Enable Brown-out on VBAT*/
	PMU_VbatBrndetCmd(ENABLE);
	PMU_VbatBrndetRstCmd(ENABLE);

	/* Do board specific Power On GPIO settings */
	board_gpio_power_on();

	/* Set PMU clock to 32Mhz */
	RC32M->CLK.BF.SEL_32M = 1;
	CLK_ModuleClkDivider(CLK_PMU, 1);

	/* Initialize System Clock */
	CLK_Init();

	/* Initialize boot flags as set up by Boot2 */
	boot_init();

	/* Initialize Operating System */
	os_init();
}

#pragma weak NMI_IRQHandler = Default_IRQHandler
#pragma weak MemManage_IRQHandler = Default_IRQHandler
#pragma weak BusFault_IRQHandler = Default_IRQHandler
#pragma weak UsageFault_IRQHandler = Default_IRQHandler
#pragma weak SVC_IRQHandler = Default_IRQHandler
#pragma weak DebugMonitor_IRQHandler = Default_IRQHandler
#pragma weak PendSV_IRQHandler = Default_IRQHandler
#pragma weak SysTick_IRQHandler = Default_IRQHandler
#pragma weak ExtPin0_IRQHandler = Default_IRQHandler
#pragma weak ExtPin1_IRQHandler = Default_IRQHandler
#pragma weak RTC_IRQHandler = Default_IRQHandler
#pragma weak CRC_IRQHandler = Default_IRQHandler
#pragma weak AES_IRQHandler = Default_IRQHandler
#pragma weak I2C0_IRQHandler = Default_IRQHandler
#pragma weak I2C1_IRQHandler = Default_IRQHandler
#pragma weak I2C2_IRQHandler = Default_IRQHandler
#pragma weak DMA_IRQHandler = Default_IRQHandler
#pragma weak GPIO_IRQHandler = Default_IRQHandler
#pragma weak SSP0_IRQHandler = Default_IRQHandler
#pragma weak SSP1_IRQHandler = Default_IRQHandler
#pragma weak SSP2_IRQHandler = Default_IRQHandler
#pragma weak QSPI0_IRQHandler = Default_IRQHandler
#pragma weak GPT0_IRQHandler = Default_IRQHandler
#pragma weak GPT1_IRQHandler = Default_IRQHandler
#pragma weak GPT2_IRQHandler = Default_IRQHandler
#pragma weak GPT3_IRQHandler = Default_IRQHandler
#pragma weak UART0_IRQHandler = Default_IRQHandler
#pragma weak UART1_IRQHandler = Default_IRQHandler
#pragma weak UART2_IRQHandler = Default_IRQHandler
#pragma weak UART3_IRQHandler = Default_IRQHandler
#pragma weak WDT_IRQHandler = Default_IRQHandler
#pragma weak ADC1_IRQHandler = Default_IRQHandler
#pragma weak ADC0_IRQHandler = Default_IRQHandler
#pragma weak DAC_IRQHandler = Default_IRQHandler
#pragma weak ACOMPWKUP_IRQHandler = Default_IRQHandler
#pragma weak ACOMP_IRQHandler = Default_IRQHandler
#pragma weak SDIO_IRQHandler = Default_IRQHandler
#pragma weak USB_IRQHandler = Default_IRQHandler
#pragma weak ExtPin2_IRQHandler = Default_IRQHandler
#pragma weak PLL_IRQHandler = Default_IRQHandler
#pragma weak QSPI1_IRQHandler = Default_IRQHandler
#pragma weak RC32M_IRQHandler = Default_IRQHandler
#pragma weak ExtPin3_IRQHandler = Default_IRQHandler
#pragma weak ExtPin4_IRQHandler = Default_IRQHandler
#pragma weak ExtPin5_IRQHandler = Default_IRQHandler
#pragma weak ExtPin6_IRQHandler = Default_IRQHandler
#pragma weak ExtPin7_IRQHandler = Default_IRQHandler
#pragma weak ExtPin8_IRQHandler = Default_IRQHandler
#pragma weak ExtPin9_IRQHandler = Default_IRQHandler
#pragma weak ExtPin10_IRQHandler = Default_IRQHandler
#pragma weak ExtPin11_IRQHandler = Default_IRQHandler
#pragma weak ExtPin12_IRQHandler = Default_IRQHandler
#pragma weak ExtPin13_IRQHandler = Default_IRQHandler
#pragma weak ExtPin14_IRQHandler = Default_IRQHandler
#pragma weak ExtPin15_IRQHandler = Default_IRQHandler
#pragma weak ExtPin16_IRQHandler = Default_IRQHandler
#pragma weak ExtPin17_IRQHandler = Default_IRQHandler
#pragma weak ExtPin18_IRQHandler = Default_IRQHandler
#pragma weak ExtPin19_IRQHandler = Default_IRQHandler
#pragma weak ExtPin20_IRQHandler = Default_IRQHandler
#pragma weak ExtPin21_IRQHandler = Default_IRQHandler
#pragma weak ExtPin22_IRQHandler = Default_IRQHandler
#pragma weak ExtPin23_IRQHandler = Default_IRQHandler
#pragma weak ExtPin24_IRQHandler = Default_IRQHandler
#pragma weak ExtPin25_IRQHandler = Default_IRQHandler
#pragma weak ExtPin26_IRQHandler = Default_IRQHandler
#pragma weak ExtPin27_IRQHandler = Default_IRQHandler
#pragma weak ExtPin28_IRQHandler = Default_IRQHandler
#pragma weak ULCOMP_IRQHandler = Default_IRQHandler
#pragma weak BRNDET_IRQHandler = Default_IRQHandler

void Default_IRQHandler(void)
{
	/*
	 * Check bit 2 in LR to find which stack pointer we should refer to,
	 * if bit = 0 then main stack pointer (msp) else process stack
	 * pointer (psp).
	 */
	__asm volatile ("tst lr, #4");
	__asm volatile ("ite eq");
	__asm volatile ("mrseq r0, msp");
	__asm volatile ("mrsne r0, psp");
	__asm volatile ("b Default_IRQHandler_C");
}

__attribute__((used))
void Default_IRQHandler_C(unsigned long *addr)
{
#ifdef CONFIG_LL_DEBUG
	stframe_t *stf;
	ll_log("NVIC->ISPR[0] : %x\r\n", NVIC->ISPR[0]);
	ll_log("NVIC->ISPR[1] : %x\r\n", NVIC->ISPR[1]);

	ll_log("NVIC->ISER[0] : %x\r\n", NVIC->ISER[0]);
	ll_log("NVIC->ISER[1] : %x\r\n", NVIC->ISER[1]);

	ll_log("NVIC->ISPR[0] &  NVIC->ISER[0] : %x\r\n",
		  NVIC->ISER[0] & NVIC->ISPR[0]);
	ll_log("NVIC->ISPR[1] &  NVIC->ISER[1] : %x\r\n",
		  NVIC->ISER[1] & NVIC->ISPR[1]);


	stf = (stframe_t *) addr;
	ll_log("Default IRQHandler: using stack pointer @ 0x%08x",
		  (unsigned int)addr);

	ll_log("\r\nr0 = 0x%08x\r\nr1 = 0x%08x\r\nr2 = 0x%08x\r\n"
		  "r3 = 0x%08x\r\nr12 = 0x%08x\r\nlr = 0x%08x\r\npc = 0x%08x"
		  "\r\npsr = 0x%08x\r\n", stf->r0, stf->r1, stf->r2, stf->r3,
		  stf->r12, stf->lr, stf->pc, stf->psr);

	ll_log("Task name %s\r\n", get_current_taskname());

#endif
	while (1)
		;
}

/* Trap MSP or PSP register value and then call real fault handler */
void HardFault_IRQHandler(void)
{
	__asm volatile ("mrs r0, msp");
	__asm volatile ("mrs r1, psp");
	__asm volatile ("b HardFault_IRQHandler_C");
}

__attribute__((used))
WEAK void HardFault_IRQHandler_C(unsigned long *msp, unsigned long *psp)
{
#ifdef CONFIG_LL_DEBUG
	int regval;
	stframe_t *stf;

	stf = (stframe_t *) msp;
	ll_log("HardFault IRQHandler: using msp @ 0x%08x",
		  (unsigned int)msp);

	ll_log("\r\nr0 = 0x%08x\r\nr1 = 0x%08x\r\nr2 = 0x%08x\r\n"
		  "r3 = 0x%08x\r\nr12 = 0x%08x\r\nlr = 0x%08x\r\npc = 0x%08x"
		  "\r\npsr = 0x%08x\r\n", stf->r0, stf->r1, stf->r2, stf->r3,
		  stf->r12, stf->lr, stf->pc, stf->psr);

	stf = (stframe_t *) psp;
	ll_log("HardFault IRQHandler: using psp @ 0x%08x",
		  (unsigned int)psp);

	ll_log("\r\nr0 = 0x%08x\r\nr1 = 0x%08x\r\nr2 = 0x%08x\r\n"
		  "r3 = 0x%08x\r\nr12 = 0x%08x\r\nlr = 0x%08x\r\npc = 0x%08x"
		  "\r\npsr = 0x%08x\r\n", stf->r0, stf->r1, stf->r2, stf->r3,
		  stf->r12, stf->lr, stf->pc, stf->psr);

	ll_log("Task name %s\r\n", get_current_taskname());

	/* read HFSR  Hard Fault Status Register */
	stptr = SCB->HFSR;
	ll_log("HFSR : 0x%08x\r\n", stptr);

	if (stptr & SCB_HFSR_FORCED_Msk) {

		/* read CFSR Configurable Fault Status Register */
		stptr = SCB->CFSR;

		ll_log("Configurable Fault Status Register"
			  "  (CFSR): 0x%08x\r\n", stptr);
		ll_log("  MemManage Fault Status Register"
			  " (MMSR)  : 0x%02x\r\n", (stptr & 0xff));

		/*
		 * If MMARVALID bit is set, read the address
		 * that caused the fault
		 */
		if (stptr & SCB_CFSR_MEMFAULTSR_Msk) {
			/* MemManage Fault Address Reg (MMAR) */
			regval = SCB->MMFAR;

			ll_log("    MMAR : 0x%08x\r\n", regval);
		}

		ll_log("  Bus Fault Status Register"
			  " (BFSR)        : 0x%02x\r\n", ((stptr >> 8) & 0xff));

		/*
		 * If BFARVALID bit is set, read the address
		 * that caused the fault
		 */
		if (stptr & SCB_CFSR_BUSFAULTSR_Msk) {
			/* Bus Fault Address Reg (BFAR) */
			regval = SCB->BFAR;

			ll_log("      BFAR      : 0x%08x\r\n", regval);
		}

		ll_log("  Usage Fault Status Register (UFSR)"
			  "      : 0x%04x\r\n", (stptr >> 16));
	}
#endif
	critical_error(-CRIT_ERR_HARD_FAULT, NULL);

	while (1)
		;
}
