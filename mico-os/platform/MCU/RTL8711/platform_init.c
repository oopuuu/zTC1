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
#include "mico_platform.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
#include "crt0.h"
#include "mico_rtos.h"
#include "platform_init.h"

#ifdef __GNUC__
#include "../../GCC/stdio_newlib.h"
#endif /* ifdef __GNUC__ */

#include "rtl8195a.h"
#include "build_info.h"
#include "PinNames.h"
#include "serial_api.h"

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

extern u8* __image4_entry_func__;
extern u8* __image4_validate_code__;

void deinit_platform_bootloader( void )
{
  MicoUartFinalize(STDIO_UART);
  
  MicoGpioFinalize(BOOT_SEL);
  MicoGpioFinalize(MFG_SEL);
  
  platform_gpio_ip_deinit();
}

void startApplication( uint32_t app_addr )
{
#if defined ( __ICCARM__ )   
  iar_data_init_fw_loader();
#endif	
  u32 Image2Len, Image2Addr, ImageIndex, SpicBitMode, SpicImageIndex;
  u32 Image2LoadAddr = app_addr;
  
  SPI_FLASH_PIN_FCTRL(ON);
  //3 1) Spi flash calibration
#ifdef CONFIG_SPIC_MODULE  
  // Config spic dual mode
#ifdef CONFIG_MP
  SpicBitMode = SpicOneBitMode;
#else
  SpicBitMode = SpicDualBitMode;
#endif  //CONFIG_MP
  SpicFlashInit(SpicBitMode);
#endif  //CONFIG_SPIC_MODULE
  
  PRAM_START_FUNCTION Image4EntryFun=(PRAM_START_FUNCTION)__image4_entry_func__;
  
  Image2Len = HAL_READ32(SPI_FLASH_BASE, Image2LoadAddr);
  Image2Addr = HAL_READ32(SPI_FLASH_BASE, (Image2LoadAddr+0x4));
  
  DBG_8195A("Flash FW Image2Len 0x%x 0x%x\n", Image2Len, Image2Addr);
  
  
  DBG_8195A("Flash FW Loader:Addr 0x%x, Len %d, Load to SRAM 0x%x\n", Image2LoadAddr, Image2Len, Image2Addr);
  
  SpicImageIndex = 0;
  for (ImageIndex = 0x10 + Image2LoadAddr; ImageIndex < (Image2Len + Image2LoadAddr + 0x10); ImageIndex = ImageIndex + 4) {
    HAL_WRITE32(Image2Addr, SpicImageIndex,
                HAL_READ32(SPI_FLASH_BASE, ImageIndex));
    
    SpicImageIndex += 4;
  }	
  
#ifdef CONFIG_SDR_EN
  u32 Image3LoadAddr;
  u32 Image3Len;
  u32 Image3Addr;
  
  Image3LoadAddr = Image2LoadAddr + Image2Len+0x10;
  Image3Len = HAL_READ32(SPI_FLASH_BASE, Image3LoadAddr);
  Image3Addr = HAL_READ32(SPI_FLASH_BASE, Image3LoadAddr + 0x4);
  
  if( (Image3Len==0xFFFFFFFF) || (Image3Len==0) || (Image3Addr!=0x30000000)){
    DBG_8195A("No Image3\n\r");
  }else{
    DBG_8195A("Image3 length: 0x%x, Image3 Addr: 0x%x\n",Image3Len, Image3Addr);
    SpicImageIndex = 0;
    
    for (ImageIndex = 0x10 + Image3LoadAddr; 
         ImageIndex < (Image3Len + Image3LoadAddr + 0x10);
         ImageIndex = ImageIndex + 4) {
           HAL_WRITE32(Image3Addr, SpicImageIndex,
                       HAL_READ32(SPI_FLASH_BASE, ImageIndex));
           
           SpicImageIndex += 4;
         }
  }
#endif	
  //3 	3) Jump to image 4
  DBG_8195A("InfraStart: %p, Img2 Sign %s \n", __image4_entry_func__, (char*)__image4_validate_code__);
  if (_strcmp((char *)__image4_validate_code__, "RTKWin") == 0) {
    deinit_platform_bootloader();
    
    Image4EntryFun->RamStartFun();	
  }
}

/**
 *  @brief   system software reset
 *
 *  @return  None
 *
 */
static void _sys_reset(void)
{
  // Set processor clock to default before system reset
  HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
  HalDelayUs(100*1000);
  
  HAL_WRITE32(PERI_ON_BASE, REG_SOC_FUNC_EN, HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN) & ~(1 << 27));
  
  // Cortex-M3 SCB->AIRCR
  HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
                (1 << 2));                                  // SYSRESETREQ
}

void platform_mcu_reset( void )
{
  _sys_reset();
}

/* STM32F2 common clock initialisation function
* This brings up enough clocks to allow the processor to run quickly while initialising memory.
* Other platform specific clock init can be done in init_platform() or init_architecture()
*/
void init_clocks( void )
{
#if 0
  //RCC_DeInit( ); /* if not commented then the LSE PA8 output will be disabled and never comes up again */
  
  /* Configure Clocks */
  RCC_HSEConfig( HSE_SOURCE );
  RCC_WaitForHSEStartUp( );
  
  RCC_HCLKConfig( AHB_CLOCK_DIVIDER );
  RCC_PCLK2Config( APB2_CLOCK_DIVIDER );
  RCC_PCLK1Config( APB1_CLOCK_DIVIDER );
  
  /* Enable the PLL */
  FLASH_SetLatency( INT_FLASH_WAIT_STATE );
  FLASH_PrefetchBufferCmd( ENABLE );
  
  /* Use the clock configuration utility from ST to calculate these values
  * http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/utility/stsw-stm32090.zip
  */
  RCC_PLLConfig( PLL_SOURCE, PLL_M_CONSTANT, PLL_N_CONSTANT, PLL_P_CONSTANT, PPL_Q_CONSTANT ); /* NOTE: The CPU Clock Frequency is independently defined in <WICED-SDK>/Wiced/Platform/<platform>/<platform>.mk */
  RCC_PLLCmd( ENABLE );
  
  while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET )
  {
  }
  RCC_SYSCLKConfig( SYSTEM_CLOCK_SOURCE );
  
  while ( RCC_GetSYSCLKSource( ) != 0x08 )
  {
  }
  
  /* Configure HCLK clock as SysTick clock source. */
  SysTick_CLKSourceConfig( SYSTICK_CLOCK_SOURCE );
  SysTick_Config( SystemCoreClock / 1000 );
  
#endif
  
  /* Workaround for the GPIOA_7 didn't pull high: it may cause the 
  SDIO Device hardware be enabled automatically at power on and then 
  GPIOA[7:0] will be used for SDIO device */
#ifndef CONFIG_SDIO_DEVICE_EN
  /* Disable Clock for SDIO function */
  ACTCK_SDIOD_CCTRL(OFF);
  
  /* SDIO Function Disable */
  SDIOD_ON_FCTRL(OFF);
  SDIOD_OFF_FCTRL(OFF);
  
  // SDIO Pin Mux off
  SDIOD_PIN_FCTRL(OFF);
#endif
}

WEAK void init_memory( void )
{
  
}

void init_architecture( void )
{
#ifndef MICO_DISABLE_STDIO
#ifndef NO_MICO_RTOS
  mico_rtos_init_mutex( &stdio_tx_mutex );
  mico_rtos_unlock_mutex ( &stdio_tx_mutex );
  mico_rtos_init_mutex( &stdio_rx_mutex );
  mico_rtos_unlock_mutex ( &stdio_rx_mutex );
#endif
#if ( defined MOC100 )  
  ring_buffer_init  ( (ring_buffer_t*)&stdio_rx_buffer, (uint8_t*)stdio_rx_data, STDIO_BUFFER_SIZE );
  platform_uart_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_uart_config, (ring_buffer_t*)&stdio_rx_buffer );
#endif
#endif
  
#ifdef BOOTLOADER
  return;
#endif
  
#ifndef MICO_DISABLE_MCU_POWERSAVE
  /* Initialise MCU powersave */
  platform_mcu_powersave_init( );
#endif /* ifndef MICO_DISABLE_MCU_POWERSAVE */
  
  platform_mcu_powersave_disable( );
}

/******************************************************
*            NO-OS Functions
******************************************************/


#ifdef NO_MICO_RTOS
static volatile uint32_t no_os_tick = 0;

void SysTick_Handler(void)
{
  //no_os_tick ++;
  //platform_watchdog_kick( );
}

uint32_t mico_get_time_no_os(void)
{
  int current_tick;
  current_tick = HalTimerOp.HalTimerReadCount(1);
  no_os_tick = (0xFFFFFFFF - current_tick)*TIMER_TICK_US/1000;
  return no_os_tick;
}

#endif


