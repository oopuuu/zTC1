/**
******************************************************************************
* @file    platform.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provides all MICO Peripherals mapping table and platform
*          specific funcgtions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 

#include "mico_platform.h"
#include "platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_config.h"
#include "platform_logging.h"
#include "wlan_platform_common.h"
#include "CheckSumUtils.h"
#include "keypad/gpio_button/button.h"

#ifdef USE_MiCOKit_EXT
#include "MiCOKit_EXT/micokit_ext.h"
#endif

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

/******************************************************
*               Function Declarations
******************************************************/
extern WEAK void PlatformEasyLinkButtonClickedCallback(void);
extern WEAK void PlatformStandbyButtonClickedCallback(void);
extern WEAK void PlatformEasyLinkButtonLongPressedCallback(void);
extern WEAK void bootloader_start(void);
extern bool mfg_test_for_app (void);
/******************************************************
*               Variables Definitions
******************************************************/

static uint32_t _default_start_time = 0;
static mico_timer_t _button_EL_timer;

const platform_gpio_t platform_gpio_pins[] =
{
	/* Common GPIOs for internal use */
	[MICO_SYS_LED]  = { .pin = PB_2, }, 

	/* GPIOs for external use */
	//[MICO_GPIO_1/3]   = { .pin = PE_4,},SWCLK
	//[MICO_GPIO_2/4]   = { .pin = PE_3,},SWDIO
	//[MICO_GPIO_5/6]   = {NC},
	
	[MICO_GPIO_7]   = { .pin = PA_2,},//USER_UART_RTS
	[MICO_GPIO_8]   = { .pin = PA_1,},//USER_UART_CTS
	[MICO_GPIO_9]   = { .pin = PA_4,},//USER_UART_TX
	[MICO_GPIO_10]  = { .pin = PA_0,},//USER_UART_RX
	//[MICO_GPIO_11]  = {CHIP_EN},
	[MICO_GPIO_12]  = { .pin = PC_0,},
	[MICO_GPIO_13]  = { .pin = PC_3,},
	[MICO_GPIO_14]  = { .pin = PC_2,},
	[MICO_GPIO_15]  = { .pin = PC_1,},	
	//[MICO_GPIO_16]  = {VDD},
	//[MICO_GPIO_17]  = {GND},
	//[MICO_GPIO_18]  = {NC},
	[MICO_GPIO_19]  = { .pin = PC_4,},	//BOOT
	[MICO_GPIO_20]  = { .pin = PC_5,},  //MFG
	//[MICO_GPIO_21/24]  = {DEBUG_TX},
	//[MICO_GPIO_22/25]  = {DEBUG_RX},
	[MICO_GPIO_23]  = { .pin = PB_3,},	//ELINK
};

/*
* Possible compile time inputs:
* - Set which ADC peripheral to use for each ADC. All on one ADC allows sequential conversion on all inputs. All on separate ADCs allows concurrent conversion.
*/
/* TODO : These need fixing */
const platform_adc_t platform_adc_peripherals[] =
{
  [MICO_ADC_1]  = 
  { 
    .pin = AD_1,
  },
  
  [MICO_ADC_2] =   
  { 
    .pin = AD_2,
  },
  
  [MICO_ADC_3] =   
  { 
    .pin = AD_3,
  },
};


/* PWM mappings */
const platform_pwm_t platform_pwm_peripherals[] =
{  
  [MICO_PWM_1]  = 
  { 
    .pin = PC_0,
  },
  [MICO_PWM_2]  = 
  { 
    .pin = PC_1,
  },
  [MICO_PWM_3]  = 
  { 
    .pin = PC_2,
  },
  [MICO_PWM_4]  = 
  { 
    .pin = PC_3,
  },
  /* TODO: fill in the other options here ... */
};

const platform_spi_t platform_spi_peripherals[] =
{
  [MICO_SPI_1]  =
  { 
    .mosi = PC_2,
    .miso = PC_3,
    .sclk = PC_1,
    .ssel = PC_0,
  },

  [MICO_SPI_1]  =
  { 
    .mosi = PB_6,
    .miso = PB_7,
    .sclk = PB_5,
    .ssel = PB_4,
  },

  [MICO_SPI_1]  =
  { 
    .mosi = PA_1,
    .miso = PA_0,
    .sclk = PA_2,
    .ssel = PA_4,
  },	
};

const platform_uart_t platform_uart_peripherals[] =
{  
  [MICO_UART_1] =
  {
    .tx = PC_3,
    .rx = PC_0,
  },
};
platform_uart_driver_t platform_uart_drivers[MICO_UART_MAX];

const platform_i2c_t platform_i2c_peripherals[] =
{
  [MICO_I2C_1] =
  {
    .sda = PB_3,
    .scl = PB_2,
  },
};
platform_i2c_driver_t platform_i2c_drivers[MICO_I2C_MAX];

/* Flash memory devices */
const platform_flash_t platform_flash_peripherals[] =
{
  [MICO_FLASH_SPI] =
  {
    .flash_type                   = FLASH_TYPE_SPI,
    .flash_start_addr             = 0x00000000,
    .flash_length                 = 0x100000,
    .flash_readonly_start         = 0x9000,
    .flash_readonly_len           = 0x2000,
  },
};

platform_flash_driver_t platform_flash_drivers[MICO_FLASH_MAX];

#if 1
/* Logic partition on flash devices */
const mico_logic_partition_t mico_partitions[] =
{
  [MICO_PARTITION_BOOTLOADER] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "Bootloader",
    .partition_start_addr      = 0xB000,
    .partition_length          = 0x8000,    //32k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_APPLICATION] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "Application",
    .partition_start_addr      = 0x13000,
    .partition_length          = 0x59000,   //356k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_RF_FIRMWARE] =
  {
    .partition_owner           = MICO_FLASH_NONE,
  },
  
  [MICO_PARTITION_PARAMETER_1] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER1",
    .partition_start_addr      = 0x0006C000,
    .partition_length          = 0x4000, // 16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_PARAMETER_2] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER2",
    .partition_start_addr      = 0x00070000,
    .partition_length          = 0x4000, //16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_ATE] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "ATE",
    .partition_start_addr      = 0x0074000,
    .partition_length          = 0x34000, //208k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_OTA_TEMP] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "OTA Storage",
    .partition_start_addr      = 0x00A8000,
    .partition_length          = 0x58000, //352k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
};


#endif
/******************************************************
*           Interrupt Handler Definitions
******************************************************/
/******************************************************
*               Function Definitions
******************************************************/
#if 1
static void _button_EL_high_irq_handler( void* arg );

static void _button_EL_low_irq_handler( void* arg )
{
  (void)(arg);

  MicoGpioEnableIRQ( (mico_gpio_t)EasyLink_BUTTON, IRQ_TRIGGER_RISING_EDGE, _button_EL_high_irq_handler, NULL );
  _default_start_time = mico_get_time()+1;
  mico_start_timer(&_button_EL_timer);
}
static void _button_EL_high_irq_handler( void* arg )
{
  (void)(arg);
  int interval = -1;

	MicoGpioEnableIRQ( (mico_gpio_t)EasyLink_BUTTON, IRQ_TRIGGER_FALLING_EDGE, _button_EL_low_irq_handler, NULL );
   
    interval = mico_get_time() + 1 - _default_start_time;
    if ( (_default_start_time != 0) && interval > 50 && interval < RestoreDefault_TimeOut){
      /* EasyLink button clicked once */
      PlatformEasyLinkButtonClickedCallback();
    } 
    mico_stop_timer(&_button_EL_timer);
    _default_start_time = 0;
}

static void _button_EL_Timeout_handler( void* arg )
{
  (void)(arg);
  _default_start_time = 0;
  MicoGpioEnableIRQ( (mico_gpio_t)EasyLink_BUTTON, IRQ_TRIGGER_FALLING_EDGE, _button_EL_low_irq_handler, NULL );
  if( MicoGpioInputGet( (mico_gpio_t)EasyLink_BUTTON ) == 0){
    PlatformEasyLinkButtonLongPressedCallback();
  } 
  mico_stop_timer(&_button_EL_timer);
}

static void _button_STANDBY_irq_handler( void* arg )
{
  (void)(arg);
  PlatformStandbyButtonClickedCallback();
}


#endif
bool watchdog_check_last_reset( void )
{
#if 0
  if ( RCC->CSR & RCC_CSR_WDGRSTF )
  {
    /* Clear the flag and return */
    RCC->CSR |= RCC_CSR_RMVF;
    return true;
  }
#endif  
  return false;
}

void platform_init_peripheral_irq_priorities( void )
{
#if 0
  /* Interrupt priority setup. Called by WICED/platform/MCU/STM32F2xx/platform_init.c */
  NVIC_SetPriority( RTC_WKUP_IRQn    ,  1 ); /* RTC Wake-up event   */
  NVIC_SetPriority( SDIO_IRQn        ,  2 ); /* WLAN SDIO           */
  NVIC_SetPriority( DMA2_Stream3_IRQn,  3 ); /* WLAN SDIO DMA       */
  NVIC_SetPriority( DMA1_Stream3_IRQn,  3 ); /* WLAN SPI DMA        */
  NVIC_SetPriority( USART1_IRQn      ,  6 ); /* MICO_UART_1         */
  NVIC_SetPriority( USART6_IRQn      ,  6 ); /* MICO_UART_2         */
  NVIC_SetPriority( DMA2_Stream7_IRQn,  7 ); /* MICO_UART_1 TX DMA  */
  NVIC_SetPriority( DMA2_Stream2_IRQn,  7 ); /* MICO_UART_1 RX DMA  */
  NVIC_SetPriority( DMA2_Stream6_IRQn,  7 ); /* MICO_UART_2 TX DMA  */
  NVIC_SetPriority( DMA2_Stream1_IRQn,  7 ); /* MICO_UART_2 RX DMA  */
  NVIC_SetPriority( EXTI0_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI1_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI2_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI3_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI4_IRQn       , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI9_5_IRQn     , 14 ); /* GPIO                */
  NVIC_SetPriority( EXTI15_10_IRQn   , 14 ); /* GPIO                */
#endif  
}

void init_platform( void )
{
#if 1 
  MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_SYS_LED );
  MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );
  MicoGpioInitialize(BOOT_SEL, INPUT_PULL_UP);
  MicoGpioInitialize(MFG_SEL, INPUT_PULL_UP);
#endif
  //  Initialise EasyLink buttons
  MicoGpioInitialize( (mico_gpio_t)EasyLink_BUTTON, INPUT_PULL_UP );
  mico_init_timer(&_button_EL_timer, RestoreDefault_TimeOut, _button_EL_Timeout_handler, NULL);
  MicoGpioEnableIRQ( (mico_gpio_t)EasyLink_BUTTON, IRQ_TRIGGER_FALLING_EDGE, _button_EL_low_irq_handler, NULL );

  /* Initialise RTC */
  platform_rtc_init( );
#if 1   
  //  Initialise Standby/wakeup switcher
  //MicoGpioInitialize( Standby_SEL, INPUT_PULL_UP );
 // MicoGpioEnableIRQ( Standby_SEL , IRQ_TRIGGER_FALLING_EDGE, _button_STANDBY_irq_handler, NULL);
#endif
}

#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE   64
#endif

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
#endif /* #ifndef MICO_DISABLE_STDIO */

void init_platform_bootloader( void )
{
  ring_buffer_init  ( (ring_buffer_t*)&stdio_rx_buffer, (uint8_t*)stdio_rx_data, STDIO_BUFFER_SIZE );
  platform_uart_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_uart_config, (ring_buffer_t*)&stdio_rx_buffer );

  MicoGpioInitialize(BOOT_SEL, INPUT_PULL_UP);
  MicoGpioInitialize(MFG_SEL, INPUT_PULL_UP);
  MicoGpioInitialize( (mico_gpio_t)EasyLink_BUTTON, INPUT_PULL_UP );
}



void MicoSysLed(bool onoff)
{
  if (onoff) {
    MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
  } else {
    MicoGpioOutputHigh( (mico_gpio_t)MICO_SYS_LED );
  }
}

void MicoRfLed(bool onoff)
{
  if (onoff) {
    MicoGpioOutputLow( (mico_gpio_t)MICO_RF_LED );
  } else {
    MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );
  }
}


static bool mfg_test_for_app (void)
{
  mico_uart_config_t uart_config;
  ring_buffer_t  rx_buffer;
  uint8_t *      rx_data;
  int ret = false;
  int i,j,waittimes=10;
  uint8_t c;

  rx_data = malloc (1024);
  require (rx_data, exit);

  /* Initialize UART interface */
  uart_config.baud_rate    = 921600;
  uart_config.data_width   = DATA_WIDTH_8BIT;
  uart_config.parity       = NO_PARITY;
  uart_config.stop_bits    = STOP_BITS_1;
  uart_config.flow_control = FLOW_CONTROL_DISABLED;
  uart_config.flags = UART_WAKEUP_DISABLE;

  ring_buffer_init ((ring_buffer_t *)&rx_buffer, (uint8_t *)rx_data, 1024);
  MicoUartInitialize (MFG_TEST, &uart_config, (ring_buffer_t *)&rx_buffer);

  for(i=0, j=0;i<waittimes;i++) {
    if (kNoErr != MicoUartRecv( MFG_TEST, &c, 1, 20)) 
      continue;

    if (c == '#') {
	  waittimes = 30;
      j++;
      if (j > 3) {
        ret = true; 
		break;
      }
    } else {
		ret = false;
		break;
    }
  }
  MicoUartFinalize(MFG_TEST);
  free(rx_data);
exit:
  return ret;
}

bool MicoShouldEnterMFGMode(void)
{
  if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==false)
    return true;
  else {
#ifndef BOOTLOADER 
	if (mfg_test_for_app()) {
		return true;
	}
#endif

    return false;
}
}

bool MicoShouldEnterBootloader(void)
{
  if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==true)
    return true;
  else
    return false;
}

bool MicoShouldEnterATEMode(void)
{
  if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)EasyLink_BUTTON)==false)
    return true;
  else
    return false;
}
static int boot_blink_led = 0;

void ymodem_upload_done(void)
{
	boot_blink_led = 1;
	MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
}

void platform_bootloader_tick(void)
{
	static int led_state = 0;
	
	if (boot_blink_led == 0)
		return;
	
	if (led_state == 1) {
		MicoSysLed(1);
		led_state = 0;
	} else {
		MicoSysLed(0);
		led_state = 1;
	}
}
