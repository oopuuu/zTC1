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
#include "platform.h"
#include "platform_config.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
#include "crt0.h"
#include "mico_platform.h"
#include "platform_init.h"


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
extern platform_uart_t          platform_uart_peripherals[];
extern platform_uart_driver_t   platform_uart_drivers[];
extern const platform_flash_t   platform_flash_peripherals[];
extern platform_flash_driver_t  platform_flash_drivers[];
extern const mico_logic_partition_t     mico_partitions[];


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

void startApplication( uint32_t app_addr )
{
  uint32_t* stack_ptr;
  uint32_t* start_ptr;
  
  if (((*(volatile uint32_t*)app_addr) & 0x2FFE0000 ) != 0x20000000)
  app_addr += 0x200;
  /* Test if user code is programmed starting from address "ApplicationAddress" */
  if (((*(volatile uint32_t*)app_addr) & 0x2FFE0000 ) == 0x20000000)
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

/* STM32F2 common clock initialisation function
* This brings up enough clocks to allow the processor to run quickly while initialising memory.
* Other platform specific clock init can be done in init_platform() or init_architecture()
*/
void init_clocks( void )
{  
//  /* Configure Clocks */
	ClkModuleDis(ALL_MODULE_CLK_SWITCH &(~(FSHC_CLK_EN)));	
  ClkModuleEn( FSHC_CLK_EN );
	//ClkModuleEn( FSHC_CLK_EN | FUART_CLK_EN | SD_CLK_EN | BUART_CLK_EN );	//enable Fuart clock for debugging
	ClkModuleGateEn( ALL_MODULE_CLK_GATE_SWITCH );        //open all clk gating  
  
  ClkPorRcToDpll(0);              //clock src is 32768hz OSC
	ClkDpllClkGatingEn(1);
  
  CacheInit();

  //Disable Watchdog
  WdgDis();
#ifdef NO_MICO_RTOS  
  SysTick_Config( MCU_CLOCK_HZ / 1000 );
#endif
}

WEAK void init_memory( void )
{
  
}

void init_architecture( void )
{ 
  NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS + 1);
  
  /* Initialise the interrupt priorities to a priority lower than 0 so that the BASEPRI register can mask them */
  NVIC_SetPriority(MMFLT_IRQn,  MMFLT_IRQn_PRIO);
  NVIC_SetPriority(BUSFLT_IRQn, BUSFLT_IRQn_PRIO);
  NVIC_SetPriority(USGFLT_IRQn, USGFLT_IRQn_PRIO);
  NVIC_SetPriority(SVCALL_IRQn, SVCALL_IRQn_PRIO);
  NVIC_SetPriority(DBGMON_IRQn, DBGMON_IRQn_PRIO);
  NVIC_SetPriority(PENDSV_IRQn, PENDSV_IRQn_PRIO);
  NVIC_SetPriority(SysTick_IRQn,SYSTICK_IRQn_PRIO);
  
  /*
   * enable 3 exception interrupt
   */
  SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
  SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

  /*
   * SOC interrupt(External Interrupt)
   */
  NVIC_SetPriority(GPIO_IRQn,   GPIO_IRQn_PRIO);
  NVIC_SetPriority(RTC_IRQn,    RTC_IRQn_PRIO);
  NVIC_SetPriority(IR_IRQn,     IR_IRQn_PRIO);
  NVIC_SetPriority(FUART_IRQn,  FUART_IRQn_PRIO);
  NVIC_SetPriority(BUART_IRQn,  BUART_IRQn_PRIO);
  NVIC_SetPriority(PWC_IRQn,    PWC_IRQn_PRIO);
  NVIC_SetPriority(TMR0_IRQn,   TMR0_IRQn_PRIO);
  NVIC_SetPriority(USB_IRQn,    USB_IRQn_PRIO);
  NVIC_SetPriority(DMACH0_IRQn, DMACH0_IRQn_PRIO);
  NVIC_SetPriority(DMACH1_IRQn, DMACH1_IRQn_PRIO);
  NVIC_SetPriority(DECODER_IRQn,DECODER_IRQn_PRIO);
  NVIC_SetPriority(SPIS_IRQn,   SPIS_IRQn_PRIO);
  NVIC_SetPriority(SD_IRQn,     SD_IRQn_PRIO);
  NVIC_SetPriority(SPIM_IRQn,   SPIM_IRQn_PRIO);
  NVIC_SetPriority(TMR1_IRQn,   TMR1_IRQn_PRIO);
  NVIC_SetPriority(WDG_IRQn,    WDG_IRQn_PRIO);
  
  GpioClrRegBits(GPIO_A_IE, 0xFFFFFFFF);
  GpioClrRegBits(GPIO_B_IE, 0xFFFFFFFF);
  GpioClrRegBits(GPIO_C_IE, 0x7FFF);
    
  GpioClrRegBits(GPIO_A_OE, 0xFFFFFFFF);
  GpioClrRegBits(GPIO_B_OE, 0xFFFFFFFF);
  GpioClrRegBits(GPIO_C_OE, 0x7FFF);
    											
  GpioSetRegBits(GPIO_A_PU, 0xFFFFFFFF);
  GpioClrRegBits(GPIO_A_PD, 0x0);
    
  GpioSetRegBits(GPIO_B_PU, 0xFFFFFFFF);
  GpioClrRegBits(GPIO_B_PD, 0x0);
    
  GpioSetRegBits(GPIO_C_PU, 0x7FFF);
  GpioClrRegBits(GPIO_B_PD, 0x0);
  
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
#endif
  
  /* Initialise RTC */
  platform_rtc_init( );
  
#ifndef MICO_DISABLE_MCU_POWERSAVE
  /* Initialise MCU powersave */
  platform_mcu_powersave_init( );
#endif /* ifndef MICO_DISABLE_MCU_POWERSAVE */

  platform_mcu_powersave_disable( );
}

OSStatus stdio_hardfault( char* data, uint32_t size )
{
#ifndef MICO_DISABLE_STDIO
  BuartIOctl(UART_IOCTL_TXINT_SET, 1);

  if(platform_uart_peripherals[STDIO_UART].uart == FUART){
    FuartSend( (uint8_t *)data, size);
    return kNoErr;
  }else if(platform_uart_peripherals[STDIO_UART].uart == BUART){
    BuartSend( (uint8_t *)data, size);
    return kNoErr;
  }else
    return kUnsupportedErr;
#endif
}

static char global_cid[25] = { 0 };
const char *mico_generate_cid(void)
{
  uint8_t *chipid = (uint8_t*)(0x7C0000 + 0x7FDA);
  
  sprintf(global_cid, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
          0, 0, 0, 0, 0, 0, 
          chipid[0], chipid[1], chipid[2], chipid[3], chipid[4], chipid[5]);
  return global_cid;
}

/******************************************************
*            NO-OS Functions
******************************************************/


#ifdef NO_MICO_RTOS
static volatile uint32_t no_os_tick = 0;
extern volatile uint32_t gSysTick;

void SysTick_Handler(void)
{
  no_os_tick ++;
  gSysTick ++;
  WdgFeed();
}

uint32_t mico_get_time_no_os(void)
{
  return no_os_tick;
}
#else
extern volatile uint32_t gSysTick;
void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
  gSysTick ++;
  xPortSysTickHandler();
}

#endif
