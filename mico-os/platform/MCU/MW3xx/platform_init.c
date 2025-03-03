/**
 ******************************************************************************
 * @file    platform_init.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-Oct-2016
 * @brief   This file provide functions called by MICO to drive mw3xx
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <flash.h>
#include <partition.h>
#include <flash_layout.h>
#include <lowlevel_drivers.h>
#include "platform_peripheral.h"
#include "board.h"
#include "boot2.h"

/******************************************************
*                      Macros
******************************************************/


/******************************************************
*                    Constants
******************************************************/

#ifndef CONFIG_CPU_MW300
#define CONFIG_CPU_MW300
#endif

#define FREQ_32M 32000000
#define REF_CLK CLK_XTAL_REF

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

/******************************************************
*               Variables Definitions
******************************************************/

unsigned long *nvram_addr = (unsigned long *)NVRAM_ADDR;
unsigned long *sb_e = (unsigned long *)SB_ERR_ADDR;

static struct flash_device_config *flash;
CLK_Src_Type sfll_ref_clk;

extern platform_uart_t platform_uart_peripherals[];
extern platform_uart_driver_t platform_uart_drivers[];

static const platform_uart_config_t stdio_uart_config =
{
    .baud_rate = 115200,
    .data_width = DATA_WIDTH_8BIT,
    .parity = NO_PARITY,
    .stop_bits = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
    .flags = 0,
};

static volatile ring_buffer_t stdio_rx_buffer;
static volatile uint8_t stdio_rx_data[STDIO_BUFFER_SIZE];
mico_mutex_t stdio_rx_mutex;
mico_mutex_t stdio_tx_mutex;

/******************************************************
*               Function Definitions
******************************************************/

/*
 * PLL Configuration Routine
 *
 * Fout=Fvco/P=Refclk/M*2*N /P
 * where Fout is the output frequency of CLKOUT, Fvco is the frequency of the
 * VCO, M is reference divider ratio, N is feedback divider ratio, P is post
 * divider ratio.
 * Given the CLKOUT should be programmed to Fout, it should follow these
 * steps in sequence:
 * A) Select proper M to get Refclk/M = 400K (+/-20%)
 * B) Find proper P to make P*Fout in the range of 150MHz ~ 300MHz
 * C) Find out the N by Round(P*Fout/(Refclk/M*2))
 */
static void CLK_Config_Pll( int ref_clk, CLK_Src_Type type )
{
    int i, refDiv;
    int fout = CHIP_SFLL_FREQ();
    CLK_SfllConfig_Type sfllConfigSet;

    while ( CLK_GetClkStatus( CLK_SFLL ) == SET )
        ;
    sfllConfigSet.refClockSrc = type;

    refDiv = (int) (ref_clk / 400000);

    /* Check for (P*fout) within 150MHz to 300MHz range */
    for ( i = 1; i <= 8; i <<= 1 )
        if ( ((fout * i) >= 150000000) &&
             ((fout * i) <= 300000000) )
            break;

    /* Configure the SFLL */
    sfllConfigSet.refDiv = refDiv;
    sfllConfigSet.fbDiv = (int) ((double) (i * fout) /
                                 (((double) ref_clk / (double) refDiv) * 2));

    sfllConfigSet.kvco = 1;

    /* Post divider ratio, 2-bit
     * 2'b00, Fout = Fvco/1
     * 2'b01, Fout = Fvco/2
     * 2'b10, Fout = Fvco/4
     * 2'b11, Fout = Fvco/8
     */
    sfllConfigSet.postDiv = ffs( i ) - 1;
    CLK_SfllEnable( &sfllConfigSet );
    while ( CLK_GetClkStatus( CLK_SFLL ) == RESET )
        ;
}

static void Setup_RC32M( )
{
    PMU_PowerOnWLAN( );
    CLK_RefClkEnable( CLK_XTAL_REF );
    while ( CLK_GetClkStatus( CLK_XTAL_REF ) == RESET )
        ;
    /* Configure the SFLL */
    CLK_SfllConfig_Type sfllCfgSet;

    /* 38.4M Main Crystal -> 192M Fvco */
    sfllCfgSet.refClockSrc = CLK_XTAL_REF;
    sfllCfgSet.refDiv = 0x60;
    sfllCfgSet.fbDiv = 0xF0;
    sfllCfgSet.kvco = 1;
    sfllCfgSet.postDiv = 0;

    CLK_SfllEnable( &sfllCfgSet );

    while ( CLK_GetClkStatus( CLK_SFLL ) == RESET )
        ;

    /* Set clock divider for IPs */
    CLK_ModuleClkDivider( CLK_APB0, 2 );
    CLK_ModuleClkDivider( CLK_APB1, 2 );
    CLK_ModuleClkDivider( CLK_PMU, 4 );

    /* Switch system clock source to SFLL
     * before RC32M calibration */
    CLK_SystemClkSrc( CLK_SFLL );

    /* Enable RC32M_GATE functional clock
     * for calibration use
     */
    PMU->PERI3_CTRL.BF.RC32M_GATE = 0;

    CLK_RC32MCalibration( CLK_AUTO_CAL, 0 );

    while ( (RC32M->STATUS.BF.CAL_DONE == 0) ||
            (RC32M->STATUS.BF.CLK_RDY == 0) )
        ;

    /* Disable RC32M_GATE functional clock
     * on calibrating RC32M
     */
    PMU->PERI3_CTRL.BF.RC32M_GATE = 1;

    /* Reset the PMU clock divider to 1 */
    CLK_ModuleClkDivider( CLK_PMU, 1 );
}

void CLK_RC32M_SfllRefClk( )
{
    int freq = FREQ_32M;
    CLK_Src_Type type = CLK_RC32M;

    sfll_ref_clk = CLK_RC32M;

    /* Set system clock to RC32M as PLL needs to be disabled for
     * reconfiguring it.*/
    CLK_SystemClkSrc( CLK_RC32M );

    /* Disable XTAL reference clock and SFLL before reconfiguring it. */
    CLK_RefClkDisable( CLK_XTAL_REF );
    CLK_SfllDisable( );

    /* Set reference clock to RC32M */
    CLK_RefClkEnable( CLK_RC32M );
    while ( CLK_GetClkStatus( CLK_RC32M ) == RESET )
        ;

    if ( board_cpu_freq( ) > freq )
    {
        CLK_Config_Pll( freq, type );
        freq = board_cpu_freq( );
        type = CLK_SFLL;
    }

    if ( freq > 50000000 )
    {
        /* Max APB0 freq 50MHz */
        CLK_ModuleClkDivider( CLK_APB0, 2 );
        /* Max APB1 freq 50MHz */
        CLK_ModuleClkDivider( CLK_APB1, 2 );
    }

    /* Select clock source */
    CLK_SystemClkSrc( type );

}

void init_clocks( )
{
    int freq;
    CLK_Src_Type type;
    sfll_ref_clk = REF_CLK;

#if defined(CONFIG_CPU_MC200)
    /* Initialize flash power domain */
    PMU_PowerOnVDDIO(PMU_VDDIO_FL);
    /* Initialize secondary flash gpio power domain */
    PMU_PowerOnVDDIO(PMU_VDDIO_D2);
#elif defined(CONFIG_CPU_MW300)
    /* Turn ON different power domains. */
    PMU_PowerOnVDDIO( PMU_VDDIO_AON );
    PMU_PowerOnVDDIO( PMU_VDDIO_0 );
    PMU_PowerOnVDDIO( PMU_VDDIO_1 );
    PMU_PowerOnVDDIO( PMU_VDDIO_2 );
    PMU_PowerOnVDDIO( PMU_VDDIO_3 );
    GPIO_PinMuxFun( GPIO_28, GPIO28_QSPI_SSn );
    GPIO_PinMuxFun( GPIO_29, GPIO29_QSPI_CLK );
    GPIO_PinMuxFun( GPIO_30, GPIO30_QSPI_D0 );
    GPIO_PinMuxFun( GPIO_31, GPIO31_QSPI_D1 );
    GPIO_PinMuxFun( GPIO_32, GPIO32_QSPI_D2 );
    GPIO_PinMuxFun( GPIO_33, GPIO33_QSPI_D3 );
#endif

    if ( board_cpu_freq( ) == FREQ_32M )
    {
        Setup_RC32M( );
        CLK_SystemClkSrc( CLK_RC32M );
        /* SFLL will not be used as system clock
         * when board_cpu_frequency = 32M
         * Hence, disable it
         */
        CLK_SfllDisable( );
        type = CLK_RC32M;
    } else
    {
        if ( sfll_ref_clk == CLK_RC32M )
        {
            /* SFLL(driven by RC32M) will be used as
             * the system clock source */
            CLK_RefClkEnable( CLK_RC32M );
            while ( CLK_GetClkStatus( CLK_RC32M ) == RESET )
                ;
            type = CLK_RC32M;
            freq = FREQ_32M;
        } else
        {
            /* XTAL/SFLL(driven by XTAL) will be used as
             * the system clock source */
            /* 38.4 MHx XTAL is routed through WLAN */
            PMU_PowerOnWLAN( );
            CLK_RefClkEnable( CLK_XTAL_REF );
            while ( CLK_GetClkStatus( CLK_XTAL_REF ) == RESET )
                ;
            type = CLK_XTAL_REF;
            freq = CLK_MAINXTAL_FREQUENCY;
        }
        Setup_RC32M( );

        /* On RC32M setup, SystemClkSrc = SFLL
         * Change the clock to reference clock before configuring PLL */
        CLK_SystemClkSrc( type );

        /* If board_cpu_frequency > board_main_xtal, SFLL would be
         * used as system clock source.
         * SFLL should be disabled otherwise.
         * Also, SFLL should be disabled before reconfiguring
         * SFLL to a new frequency value */
        CLK_SfllDisable( );

        /*
         * Check if expected cpu frequency is greater than the
         * source clock frequency. In that case we need to enable
         * the PLL.
         */

        if ( board_cpu_freq( ) > freq )
        {
            CLK_Config_Pll( freq, type );
            freq = board_cpu_freq( );
            type = CLK_SFLL;
        }

        if ( freq > 50000000 )
        {
            /* Max APB0 freq 50MHz */
            CLK_ModuleClkDivider( CLK_APB0, 2 );
            /* Max APB1 freq 50MHz */
            CLK_ModuleClkDivider( CLK_APB1, 2 );
        }

    }
    /* Select clock source */
    CLK_SystemClkSrc( type );

    /* Power down WLAN in order to save power */
    if ( sfll_ref_clk == CLK_RC32M )
        PMU_PowerDownWLAN( );
}

WEAK void init_memory( void )
{

}

void init_architecture( void )
{
//    u8 select[10];
    int ret;
    uint32_t system_clock = 0;

    /*
     * It is observed that clock remains at 200MHz if application has
     * initialized it even after reset-halt. And hence following clock
     * divider mechanism will be needed, without relying on application.
     * Max QSPI0/1 Clock Frequency is 50MHz.
     *
     * We do this by default.
     */
#if defined(CONFIG_CPU_MW300)
    system_clock = CLK_GetSystemClk( );
    if ( system_clock > 50000000 )
        #endif
        CLK_ModuleClkDivider( CLK_QSPI0, 4 );
#if defined(CONFIG_CPU_MC200)
    QSPI0_Init_CLK();
#elif defined(CONFIG_CPU_MW300)
    QSPI->CONF.BF.CLK_PRESCALE = 0;
    FLASH_ResetFastReadQuad( );
#endif

    /* 4 pre-emption, 4 subpriority bits */
    NVIC_SetPriorityGrouping( 4 );
    SysTick_Config( system_clock / 1000 );

#ifndef MICO_DISABLE_STDIO
#ifndef NO_MICO_RTOS
    mico_rtos_init_mutex( &stdio_tx_mutex );
    mico_rtos_unlock_mutex( &stdio_tx_mutex );
    mico_rtos_init_mutex( &stdio_rx_mutex );
    mico_rtos_unlock_mutex( &stdio_rx_mutex );
#endif

    ring_buffer_init( (ring_buffer_t*) &stdio_rx_buffer, (uint8_t*) stdio_rx_data, STDIO_BUFFER_SIZE );
    platform_uart_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_uart_config,
                        (ring_buffer_t*) &stdio_rx_buffer );
#endif

#ifdef CONFIG_SPI_FLASH_DRIVER
    spi_flash_init();
#endif

#ifdef CONFIG_XFLASH_DRIVER
    CLK_ModuleClkDivider(CLK_QSPI1, 4);
    QSPI1_Init_CLK();

    /* Initialize secondary QSPI gpios */
    GPIO_PinMuxFun(GPIO_72, GPIO72_QSPI1_SSn);
    GPIO_PinMuxFun(GPIO_73, GPIO73_QSPI1_CLK);
    GPIO_PinMuxFun(GPIO_76, GPIO76_QSPI1_D0);
    GPIO_PinMuxFun(GPIO_77, GPIO77_QSPI1_D1);
    GPIO_PinMuxFun(GPIO_78, GPIO78_QSPI1_D2);
    GPIO_PinMuxFun(GPIO_79, GPIO79_QSPI1_D3);

    XFLASH_PowerDown(DISABLE);
#endif

#if defined(CONFIG_CPU_MC200)
    flash = &flash_config;
#elif defined(CONFIG_CPU_MW300)
    /* Read the JEDEC id of the primary flash that is connected */
    uint32_t jedec_id = FLASH_GetJEDECID( );

    /* Set the flash configuration as per the JEDEC id */
    ret = FLASH_SetConfig( jedec_id );
    /* In case of an error, print error message and continue */
    if ( ret != SUCCESS )
        printf( "Flash JEDEC ID %lx not present in supported flash "
                "list, using default config for W25Q32BV",
                jedec_id );
    flash = (struct flash_device_config *)FLASH_GetConfig( );
#endif

    boot2_main( 0 );
    boot_init();
    part_init();

    /* Initialise nanosecond clock counter */
    platform_init_nanosecond_clock();

}

uint32_t stdio_hardfault( char* data, uint32_t size )
{
#ifndef MICO_DISABLE_STDIO
    uint32_t idx, stdio_port;

    stdio_port = platform_uart_peripherals[STDIO_UART].port_id;
    for ( idx = 0; idx < size; idx++ )
    {
        while ( UART_GetLineStatus( stdio_port, UART_LINESTATUS_TDRQ ) != SET );
        UART_SendData( stdio_port, (data[idx] & (uint16_t) 0x01FF) );

    }
#endif
    return 0;
}

OSStatus platform_watchdog_kick( void )
{
#ifndef MICO_DISABLE_WATCHDOG
  IWDG_ReloadCounter();
  return kNoErr;
#else
  return kUnsupportedErr;
#endif
}

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

