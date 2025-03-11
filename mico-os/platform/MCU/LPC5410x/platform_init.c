/**
 ******************************************************************************
 * @file    stm32f2xx_platform.c
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
#include "platform.h"
#include "platform_config.h"
#include "mico_platform.h"
#include "mico_rtos.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
#include "crt0.h"
#include "platform_init.h"

#ifdef __GNUC__
#include "../../GCC/stdio_newlib.h"
#endif /* ifdef __GNUC__ */


/******************************************************
*                      Macros
******************************************************/

/******************************************************
*                    Constants
******************************************************/

#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE   64
#endif

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Function Declarations
******************************************************/

extern OSStatus host_platform_init( void );

/******************************************************
*               Variables Definitions
******************************************************/
extern platform_uart_t platform_uart_peripherals[];
extern platform_uart_driver_t platform_uart_drivers[];

// rocky: add global definitions of peripheral for watch window
USED LPC_SYSCON_T * g_pSys = LPC_SYSCON;
USED LPC_ASYNC_SYSCON_T *g_pASys = LPC_ASYNC_SYSCON;
USED LPC_IOCON_T *g_pIO = LPC_IOCON;
USED LPC_GPIO_T *g_pGP = LPC_GPIO;
USED LPC_INMUX_T *g_pInMux = LPC_INMUX;
USED LPC_DMA_T *g_pDMA = LPC_DMA;
USED LPC_SPI_T *g_pSPI0 = LPC_SPI0;
USED LPC_SPI_T *g_pSPI1 = LPC_SPI1;
USED LPC_PMU_T *g_pPMU = LPC_PMU;
USED LPC_FIFO_T *g_pFIFO = LPC_FIFO;
USED NVIC_Type *g_pNVIC = NVIC;
USED SCB_Type *g_pSCB = SCB;
USED SysTick_Type *g_pSysTick = SysTick;

/* mico_cpu_clock_hz is used by MICO RTOS */
//volatile uint32_t  mico_cpu_clock_hz;

#ifndef MICO_DISABLE_STDIO
static const mico_uart_config_t stdio_uart_config =
{
  .baud_rate    = STDIO_UART_BAUDRATE,
  .data_width   = DATA_WIDTH_8BIT,
  .parity       = NO_PARITY,
  .stop_bits    = STOP_BITS_1,
  .flow_control = FLOW_CONTROL_DISABLED,
  .flags        = 0,
};

static volatile ring_buffer_t stdio_rx_buffer;
static volatile uint8_t             stdio_rx_data[STDIO_BUFFER_SIZE];
mico_mutex_t        stdio_rx_mutex;
mico_mutex_t        stdio_tx_mutex;
#endif /* #ifndef MICO_DISABLE_STDIO */

/******************************************************
*               Function Definitions
******************************************************/
#if defined ( __ICCARM__ )
static inline void __jump_to( uint32_t addr )
{
  __asm( "MOV R1, #0x00000001" );
  __asm( "ORR R0, R0, R1" );  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  __asm( "BLX R0" );
}


#elif defined ( __GNUC__ )
__attribute__( ( always_inline ) ) static __INLINE void __jump_to( uint32_t addr )
{
  addr |= 0x00000001;  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  __ASM volatile ("BX %0" : : "r" (addr) );
}


#elif defined ( __CC_ARM )
static void __asm __jump_to( uint32_t addr )
{
  MOV R1, #0x00000001
  ORR R0, R0, R1  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  BLX R0
}
#endif

/*Boot to mico application form APPLICATION_START_ADDRESS defined in platform_common_config.h */
void startApplication( uint32_t app_addr )
{
  uint32_t* stack_ptr;
  uint32_t* start_ptr;
  //if (((*(volatile uint32_t*)app_addr) & 0x2FFE0000 ) != 0x20000000)
  //app_addr += 0x200;
  /* Test if user code is programmed starting from address "ApplicationAddress" */
 // if (((*(volatile uint32_t*)app_addr) & 0x2FFE0000 ) == 0x20000000)
  {
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    /* Clear all interrupt enabled by bootloader */
    for (int i = 0; i < 8; i++ )
        NVIC->ICER[i] = 0xFFFFFFFF;

    stack_ptr = (uint32_t*) app_addr;  /* Initial stack pointer is first 4 bytes of vector table */
    start_ptr = ( stack_ptr + 1 );  /* Reset vector is second 4 bytes of vector table */

    #if defined ( __ICCARM__)
    __ASM( "MOV LR,        #0xFFFFFFFF" );
    __ASM( "MOV R1,        #0x01000000" );
    __ASM( "MSR APSR_nzcvq,     R1" );
    __ASM( "MOV R1,        #0x00000000" );
    __ASM( "MSR PRIMASK,   R1" );
    __ASM( "MSR FAULTMASK, R1" );
    __ASM( "MSR BASEPRI,   R1" );
    __ASM( "MSR CONTROL,   R1" );
    #endif

    __set_MSP( *stack_ptr );
    __jump_to( *start_ptr );
  }
}

void platform_mcu_reset( void )
{
    NVIC_SystemReset();
}

#include "power_control.h"


void DMAInit(void)
{
    static uint8_t ls_isDMAInited;
    if (ls_isDMAInited == 0)
    {
        ls_isDMAInited = 1;
        // enable clock for DMA
        g_pSys->AHBCLKCTRLSET[0] = 1UL << 20;
        // make DMA has highest bus matrix priority
        g_pSys->AHBMATPRIO |= 3UL<<8;
        // reset, and clear reset of DMA
        g_pSys->PRESETCTRLSET[0] = 1UL << 20;
        g_pSys->PRESETCTRLCLR[0] = 1UL << 20;

        g_pDMA->CTRL = 1;    // enable DMA controller
        g_pDMA->SRAMBASE = (uint32_t)(Chip_DMA_Table);    // set desc table addr
        NVIC_EnableIRQ(DMA_IRQn);
    }
}

void AsyncClockCfg(void)
{
	// make sure SystemCoreClock is multiples of 12MHz
	if (SystemCoreClock % (48 * 1000 * 1000) == 0
)
	{
		// configure 48MHz
		g_pASys->ASYNCCLKDIV = SystemCoreClock / 48000000;
	}
	else if (SystemCoreClock % (24 * 1000 * 1000) == 0)
	{
		// configure 24MHz
		g_pASys->ASYNCCLKDIV = SystemCoreClock / 24000000;
	}
	else if (SystemCoreClock % (12 * 1000 * 1000) == 0)
	{
		// make ASync clock at 12MHz
		g_pASys->ASYNCCLKDIV = SystemCoreClock / 12000000;

	}
	else
		while(1);
	// make USART clock = AsyncClk * 22 / 256, so it is multiples of 11.0592MHz
	g_pASys->FRGCTRL = 255 | 22<<8;
}
/* MCU common clock initialisation function
* This brings up enough clocks to allow the processor to run quickly while initialising memory.
* Other platform specific clock init can be done in init_platform() or init_architecture()
*/
extern void PwrCtlStateReset(void);
void init_clocks( void )
{
	// >>> rocky: make sure linker does not remove
    g_pSys = g_pSys;
    g_pASys = g_pASys;
    g_pIO = g_pIO;
    g_pGP = g_pGP ;
    g_pInMux = g_pInMux;
    g_pDMA = g_pDMA;
    g_pSPI0 = g_pSPI0;
    g_pSPI1 = g_pSPI1;
  	g_pPMU = g_pPMU;
  	// <<<
  	g_pSys = LPC_SYSCON;

    g_pSys->PDRUNCFGCLR = 0x0FUL<<13;   // Turn on power for all SRAM blocks
    g_pSys->AHBCLKCTRL[0] |= 0x18;  	// Turn on clock for all SRAM blocks

    g_pSys->AHBMATPRIO = 2UL<<0 | 3UL<<2;   // set ICode and DCode bus priority high
//  extern void *__Vectors;
//  SCB->VTOR = (uint32_t) &__Vectors;

  fpuInit();
    PwrCtlStateReset();
//    SystemCoreClockUpdate();

#if (defined FIRMWARE_DOWNLOAD) || (defined BOOTLOADER)
//#ifdef FIRMWARE_DOWNLOAD
//#if 0
	// enable clock to InMux, PinINT, IOCON, GPIO0 & 1
	g_pSys->AHBCLKCTRLSET[0] = 1UL<<11 | 1UL<<18 | 1UL<<13 | 1UL<<14 | 1UL<<15;
	// reset InMux, PinINT, IOCON, GPIO0 & 1
	g_pSys->PRESETCTRLSET[0] = 1UL<<11 | 1UL<<18 | 1UL<<13 | 1UL<<14 | 1UL<<15;
	g_pSys->PRESETCTRLCLR[0] = 1UL<<11 | 1UL<<18 | 1UL<<13 | 1UL<<14 | 1UL<<15;

	/* Turn on the IRC by clearing the power down bit */
	Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_IRC_OSC | SYSCON_PDRUNCFG_PD_IRC);
	/* Set main clock source to the system PLL. This will drive 24MHz
	   for the main clock and 24MHz for the system clock */
	Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_IRC);

	/* Set system clock divider to 1 */
	Chip_Clock_SetSysClockDiv(1);

	Chip_SYSCON_Enable_ASYNC_Syscon(true);

	g_pASys->ASYNCAPBCLKCTRL = 1;	// enable Async APB

	// select main clock (CPU clock) as ASync clock for faster SPI
	g_pASys->ASYNCAPBCLKSELA = 0;
	g_pASys->ASYNCAPBCLKSELB = 0;

	Chip_Clock_SetAsyncSysconClockSource(SYSCON_ASYNC_MAINCLK);

	/* Setup FLASH access to 1 clock */
	Chip_FMC_SetFLASHAccess(FLASHTIM_20MHZ_CPU);

	// set frequency for mico task
	TaskProcNotify(MICO_TASK, 1);

	AsyncClockCfg();

#endif

#ifdef NO_MICO_RTOS
  SysTick_Config( SystemCoreClock / 1000 );
#endif
	//mico_cpu_clock_hz = SystemCoreClock;
  	g_pSys->AHBCLKCTRL[0] |= 0x18;    // Opened SRAM1 and SRAM2

}

WEAK void init_memory( void )
{

}

// RockyS : set up timer interrupt


// extern uint32_t CFG_PRIO_BITS;
void init_architecture( void )
{
    init_clocks();
  	DMAInit();
    platform_init_peripheral_irq_priorities();
    g_pSys->FIFOCTRL =0;

  /* Initialise GPIO IRQ manager */
  platform_gpio_irq_manager_init();

#ifndef MICO_DISABLE_STDIO
#ifndef NO_MICO_RTOS
  mico_rtos_init_mutex( &stdio_tx_mutex );
  mico_rtos_unlock_mutex ( &stdio_tx_mutex );
  mico_rtos_init_mutex( &stdio_rx_mutex );
  mico_rtos_unlock_mutex ( &stdio_rx_mutex );
#endif

  ring_buffer_init  ( (ring_buffer_t*)&stdio_rx_buffer, (uint8_t*)stdio_rx_data, STDIO_BUFFER_SIZE );
  platform_uart_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_uart_config, (ring_buffer_t*)&stdio_rx_buffer );
#endif

  /* Ensure 802.11 device is in reset. */
  host_platform_init( );

  /* Initialise nanosecond clock counter */
  platform_init_nanosecond_clock();

#ifdef BOOTLOADER
  return;
#else

  /* Initialise RTC */
  platform_rtc_init( );

#ifndef MICO_DISABLE_MCU_POWERSAVE
  /* Initialise MCU powersave */
  platform_mcu_powersave_init( );
#endif /* ifndef MICO_DISABLE_MCU_POWERSAVE */

  platform_mcu_powersave_disable( );
#endif
}

OSStatus stdio_hardfault( char* data, uint32_t size )
{
#ifndef MICO_DISABLE_STDIO
  uint32_t idx;
  for(idx = 0; idx < size; idx++){
// Magicoe TODO delete
//    while ( ( platform_uart_peripherals[ STDIO_UART ].port->SR & USART_SR_TXE ) == 0 );
//    platform_uart_peripherals[ STDIO_UART ].port->DR = (data[idx] & (uint16_t)0x01FF);
    while ((Chip_UART_GetStatus(platform_uart_peripherals[STDIO_UART].port) & UART_STAT_TXRDY) == 0);
    Chip_UART_SendByte(platform_uart_peripherals[STDIO_UART].port, (data[idx] & (uint16_t)0x00FF) );
  }
#endif
  return kNoErr;
}

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

// end file --- MG.Niu ---
