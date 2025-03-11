/**
 ******************************************************************************
 * @file    platform.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provides all MICO Peripherals mapping table and platform
 *          specific functions.
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

#include "stdio.h"
#include "string.h"

#include "platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_logging.h"
#include "mico_platform.h"
#include "wlan_platform_common.h"
#include "spi_flash_platform_interface.h"

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

/******************************************************
*               Variables Definitions
******************************************************/

static uint32_t _default_start_time = 0;
static mico_timer_t _button_EL_timer;

const platform_gpio_t platform_gpio_pins[] =
{
  /* Common GPIOs for internal use */
  [MICO_SYS_LED]                      = { GPIOB,  7 }, 
  [MICO_RF_LED]                       = { GPIOB,  6 },
  [BOOT_SEL]                          = { GPIOB,  1 }, 
  [MFG_SEL]                           = { GPIOB,  9 }, 
  [Standby_SEL]                       = { GPIOA,  0 }, 
  [EasyLink_BUTTON]                   = { GPIOB,  8 }, 
  [STDIO_UART_RX]                     = { GPIOA,  9 },
  [STDIO_UART_TX]                     = { GPIOA, 10 },
  [STDIO_UART_CTS]                    = { GPIOA, 12 },  
  [STDIO_UART_RTS]                    = { GPIOA, 11 },
  [FLASH_PIN_SPI_CS  ]                = { GPIOA,  4 },
  [FLASH_PIN_SPI_CLK ]                = { GPIOA,  5 },
  [FLASH_PIN_SPI_MOSI]                = { GPIOA,  7 },
  [FLASH_PIN_SPI_MISO]                = { GPIOA,  6 },
  
  /* GPIOs for external use */
  [MICO_GPIO_0]                       = { GPIOC,  6 },
  [MICO_GPIO_1]                       = { GPIOC,  7 },
  [MICO_GPIO_2]                       = { GPIOC,  8 },
  [MICO_GPIO_3]                       = { GPIOC,  9 },
  [MICO_GPIO_4]                       = { GPIOC, 12 },
  [MICO_GPIO_5]                       = { GPIOC, 13 },
  [MICO_GPIO_6]                       = { GPIOC, 10 },
  [MICO_GPIO_7]                       = { GPIOC, 11 },
  [MICO_GPIO_8]                       = { GPIOD,  2 },
  //  [MICO_GPIO_9]
  [MICO_GPIO_10]                      = { GPIOA,  4 },
  [MICO_GPIO_11]                      = { GPIOA,  7 },
  [MICO_GPIO_12]                      = { GPIOA,  6 },
  [MICO_GPIO_13]                      = { GPIOA,  5 },
  //  [MICO_GPIO_14]
  [MICO_GPIO_15]                      = { GPIOB,  2 },
  [MICO_GPIO_16]                      = { GPIOB, 10 },
  [MICO_GPIO_17]                      = { GPIOB, 11 },
  [MICO_GPIO_18]                      = { GPIOC,  5 },
  [MICO_GPIO_19]                      = { GPIOC,  4 },
  [MICO_GPIO_20]                      = { GPIOC,  3 },
  [MICO_GPIO_21]                      = { GPIOC,  2 },
  [MICO_GPIO_22]                      = { GPIOC,  1 },
  [MICO_GPIO_23]                      = { GPIOC,  0 },
};

/*
* Possible compile time inputs:
* - Set which ADC peripheral to use for each ADC. All on one ADC allows sequential conversion on all inputs. All on separate ADCs allows concurrent conversion.
*/
/* TODO : These need fixing */
const platform_adc_t platform_adc_peripherals[] =
{
  [MICO_ADC_1] = {ADC1, ADC_Channel_10, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[MICO_GPIO_23]},
  [MICO_ADC_2] = {ADC1, ADC_Channel_11, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[MICO_GPIO_22]},
  [MICO_ADC_3] = {ADC1, ADC_Channel_12, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[MICO_GPIO_21]},
  [MICO_ADC_4] = {ADC1, ADC_Channel_13, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[MICO_GPIO_20]},
  [MICO_ADC_5] = {ADC1, ADC_Channel_14, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[MICO_GPIO_19]},
  [MICO_ADC_6] = {ADC1, ADC_Channel_15, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[MICO_GPIO_18]},
};


/* PWM mappings */
const platform_pwm_t platform_pwm_peripherals[] =
{
  [MICO_PWM_1]  = {TIM3, 4, RCC_APB1Periph_TIM3, GPIO_AF_TIM3, &platform_gpio_pins[MICO_GPIO_3]},    
  /* TODO: fill in the other options here ... */
};

const platform_spi_t platform_spi_peripherals[] =
{
  [MICO_SPI_1]  =
  {
    .port                  = SPI1,
    .gpio_af               = GPIO_AF_SPI1,
    .peripheral_clock_reg  = RCC_APB2Periph_SPI1,
    .peripheral_clock_func = RCC_APB2PeriphClockCmd,
    .pin_mosi              = &platform_gpio_pins[MICO_GPIO_8],
    .pin_miso              = &platform_gpio_pins[MICO_GPIO_7],
    .pin_clock             = &platform_gpio_pins[MICO_GPIO_6],
    .tx_dma =
    {
      .controller        = DMA2,
      .stream            = DMA2_Stream5,
      .channel           = DMA_Channel_3,
      .irq_vector        = DMA2_Stream5_IRQn,
      .complete_flags    = DMA_HISR_TCIF5,
      .error_flags       = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
    },
    .rx_dma =
    {
      .controller        = DMA2,
      .stream            = DMA2_Stream0,
      .channel           = DMA_Channel_3,
      .irq_vector        = DMA2_Stream0_IRQn,
      .complete_flags    = DMA_LISR_TCIF0,
      .error_flags       = ( DMA_LISR_TEIF0 | DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 ),
    },
  },
  [MICO_SPI_2]  =
  {
    .port                         = SPI1,
    .gpio_af                      = GPIO_AF_SPI1,
    .peripheral_clock_reg         = RCC_APB2Periph_SPI1,
    .peripheral_clock_func        = RCC_APB2PeriphClockCmd,
    .pin_mosi                     = &platform_gpio_pins[FLASH_PIN_SPI_MOSI],
    .pin_miso                     = &platform_gpio_pins[FLASH_PIN_SPI_MISO],
    .pin_clock                    = &platform_gpio_pins[FLASH_PIN_SPI_CLK],
    .tx_dma =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream4,
      .channel                    = DMA_Channel_0,
      .irq_vector                 = DMA1_Stream4_IRQn,
      .complete_flags             = DMA_HISR_TCIF4,
      .error_flags                = ( DMA_HISR_TEIF4 | DMA_HISR_FEIF4 ),
    },
    .rx_dma =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream3,
      .channel                    = DMA_Channel_0,
      .irq_vector                 = DMA1_Stream3_IRQn,
      .complete_flags             = DMA_LISR_TCIF3,
      .error_flags                = ( DMA_LISR_TEIF3 | DMA_LISR_FEIF3 | DMA_LISR_DMEIF3 ),
    },
  }
};

const platform_uart_t platform_uart_peripherals[] =
{
  [MICO_UART_1] =
  {
    .port                         = USART1,
    .pin_tx                       = &platform_gpio_pins[STDIO_UART_TX],
    .pin_rx                       = &platform_gpio_pins[STDIO_UART_RX],
    .pin_cts                      = &platform_gpio_pins[STDIO_UART_CTS],
    .pin_rts                      = &platform_gpio_pins[STDIO_UART_RTS],
    .tx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream7,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA2_Stream7_IRQn,
      .complete_flags             = DMA_HISR_TCIF7,
      .error_flags                = ( DMA_HISR_TEIF7 | DMA_HISR_FEIF7 ),
    },
    .rx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream2,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA2_Stream2_IRQn,
      .complete_flags             = DMA_LISR_TCIF2,
      .error_flags                = ( DMA_LISR_TEIF2 | DMA_LISR_FEIF2 | DMA_LISR_DMEIF2 ),
    },
  },
  [MICO_UART_2] =
  {
    .port                         = USART6,
    .pin_tx                       = &platform_gpio_pins[MICO_GPIO_0],
    .pin_rx                       = &platform_gpio_pins[MICO_GPIO_1],
    .pin_cts                      = NULL,
    .pin_rts                      = NULL,
    .tx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream6,
      .channel                    = DMA_Channel_5,
      .irq_vector                 = DMA2_Stream6_IRQn,
      .complete_flags             = DMA_HISR_TCIF6,
      .error_flags                = ( DMA_HISR_TEIF6 | DMA_HISR_FEIF6 ),
    },
    .rx_dma_config =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream1,
      .channel                    = DMA_Channel_5,
      .irq_vector                 = DMA2_Stream1_IRQn,
      .complete_flags             = DMA_LISR_TCIF1,
      .error_flags                = ( DMA_LISR_TEIF1 | DMA_LISR_FEIF1 | DMA_LISR_DMEIF1 ),
    },
  },
};
platform_uart_driver_t platform_uart_drivers[MICO_UART_MAX];


const platform_i2c_t platform_i2c_peripherals[] =
{
  [MICO_I2C_1] =
  {
    .port                    = I2C2,
    .pin_scl                 = &platform_gpio_pins[MICO_GPIO_16],
    .pin_sda                 = &platform_gpio_pins[MICO_GPIO_17],
    .peripheral_clock_reg    = RCC_APB1Periph_I2C2,
    .tx_dma                  = DMA1,
    .tx_dma_peripheral_clock = RCC_AHB1Periph_DMA1,
    .tx_dma_stream           = DMA1_Stream7,
    .rx_dma_stream           = DMA1_Stream5,
    .tx_dma_stream_id        = 7,
    .rx_dma_stream_id        = 5,
    .tx_dma_channel          = DMA_Channel_1,
    .rx_dma_channel          = DMA_Channel_1,
    .gpio_af                 = GPIO_AF_I2C2
  },
};
platform_i2c_driver_t platform_i2c_drivers[MICO_I2C_MAX];

platform_spi_driver_t platform_spi_drivers[MICO_SPI_MAX];


const platform_flash_t platform_flash_peripherals[] =
{
  [MICO_FLASH_EMBEDDED] =
  {
    .flash_type                   = FLASH_TYPE_EMBEDDED,
    .flash_start_addr             = 0x08000000,
    .flash_length                 = 0x100000,
  },
  [MICO_FLASH_SPI] =
  {
    .flash_type                   = FLASH_TYPE_SPI,
    .flash_start_addr             = 0x000000,
    .flash_length                 = 0x200000,
  },
};

platform_flash_driver_t platform_flash_drivers[MICO_FLASH_MAX];

/* Logic partition on flash devices */
const mico_logic_partition_t mico_partitions[] =
{
  [MICO_PARTITION_BOOTLOADER] =
  {
    .partition_owner           = MICO_FLASH_EMBEDDED,
    .partition_description     = "Bootloader",
    .partition_start_addr      = 0x08000000,
    .partition_length          =     0x8000,    //16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_APPLICATION] =
  {
    .partition_owner           = MICO_FLASH_EMBEDDED,
    .partition_description     = "Application",
    .partition_start_addr      = 0x08008000,
    .partition_length          =    0xF8000,   //992k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_RF_FIRMWARE] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "RF Firmware",
    .partition_start_addr      = 0x2000,
    .partition_length          = 0x4D000,  //308k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_OTA_TEMP] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "OTA Storage",
    .partition_start_addr      = 0x00050000,
    .partition_length          = 0x100000, //1Mbytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_PARAMETER_1] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER1",
    .partition_start_addr      = 0x0,
    .partition_length          = 0x1000, // 4k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_PARAMETER_2] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER1",
    .partition_start_addr      = 0x1000,
    .partition_length          = 0x1000, //4k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_ATE] =
  {
    .partition_owner           = MICO_FLASH_NONE,
  }
};





#if defined ( USE_MICO_SPI_FLASH )
const mico_spi_device_t mico_spi_flash =
{
  .port        = MICO_SPI_2,
  .chip_select = FLASH_PIN_SPI_CS,
  .speed       = 40000000,
  .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_NO_DMA | SPI_MSB_FIRST),
  .bits        = 8
};
#endif


/* Wi-Fi control pins. Used by platform/MCU/wlan_platform_common.c
* SDIO: EMW1062_PIN_BOOTSTRAP[1:0] = b'00
* gSPI: EMW1062_PIN_BOOTSTRAP[1:0] = b'01
*/
const platform_gpio_t wifi_control_pins[] =
{
  [WIFI_PIN_RESET      ] = { GPIOA,  3 },
#if defined ( MICO_USE_WIFI_32K_CLOCK_MCO )
  [WIFI_PIN_32K_CLK    ] = { GPIOA,  8 },
#else
  [WIFI_PIN_32K_CLK    ] = { GPIOA,  8 },
#endif
  [WIFI_PIN_BOOTSTRAP_0] = { GPIOB,  5 },
  [WIFI_PIN_BOOTSTRAP_1] = { GPIOA,  2 },
};

/* Wi-Fi gSPI bus pins. Used by platform/MCU/STM32F2xx/EMW1062_driver/wlan_spi.c */
const platform_gpio_t wifi_spi_pins[] =
{
  [WIFI_PIN_SPI_IRQ ] = { GPIOA,  1 },
  [WIFI_PIN_SPI_CS  ] = { GPIOB, 12 },
  [WIFI_PIN_SPI_CLK ] = { GPIOB, 13 },
  [WIFI_PIN_SPI_MOSI] = { GPIOB, 15 },
  [WIFI_PIN_SPI_MISO] = { GPIOB, 14 },
};

const platform_spi_t wifi_spi =
{
  .port                         = SPI2,
  .gpio_af                      = GPIO_AF_SPI2,
  .peripheral_clock_reg         = RCC_APB1Periph_SPI2,
  .peripheral_clock_func        = RCC_APB1PeriphClockCmd,
  .pin_mosi                     = &wifi_spi_pins[WIFI_PIN_SPI_MOSI],
  .pin_miso                     = &wifi_spi_pins[WIFI_PIN_SPI_MISO],
  .pin_clock                    = &wifi_spi_pins[WIFI_PIN_SPI_CLK],
  .tx_dma =
  {
    .controller                 = DMA1,
    .stream                     = DMA1_Stream4,
    .channel                    = DMA_Channel_0,
    .irq_vector                 = DMA1_Stream4_IRQn,
    .complete_flags             = DMA_HISR_TCIF4,
    .error_flags                = ( DMA_HISR_TEIF4 | DMA_HISR_FEIF4 ),
  },
  .rx_dma =
  {
    .controller                 = DMA1,
    .stream                     = DMA1_Stream3,
    .channel                    = DMA_Channel_0,
    .irq_vector                 = DMA1_Stream3_IRQn,
    .complete_flags             = DMA_LISR_TCIF3,
    .error_flags                = ( DMA_LISR_TEIF3 | DMA_LISR_FEIF3 | DMA_LISR_DMEIF3 ),
  },
};



/******************************************************
*           Interrupt Handler Definitions
******************************************************/
#ifndef MICO_NO_WIFI
MICO_RTOS_DEFINE_ISR( DMA1_Stream3_IRQHandler )
{
  platform_wifi_spi_rx_dma_irq( );
}
#endif

MICO_RTOS_DEFINE_ISR( USART1_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( USART6_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[MICO_UART_2] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream7_IRQHandler )
{
  platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream6_IRQHandler )
{
  platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_2] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream2_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream1_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[MICO_UART_2] );
}


/******************************************************
*               Function Definitions
******************************************************/

static void _button_EL_irq_handler( void* arg )
{
  (void)(arg);
  int interval = -1;
  
  if ( MicoGpioInputGet( (mico_gpio_t)EasyLink_BUTTON ) == 0 ) {
    _default_start_time = mico_rtos_get_time()+1;
    mico_start_timer(&_button_EL_timer);
  } else {
    interval = mico_rtos_get_time() + 1 - _default_start_time;
    if ( (_default_start_time != 0) && interval > 50 && interval < RestoreDefault_TimeOut){
      /* EasyLink button clicked once */
      PlatformEasyLinkButtonClickedCallback();
    }
    mico_stop_timer(&_button_EL_timer);
    _default_start_time = 0;
  }
}

static void _button_STANDBY_irq_handler( void* arg )
{
  (void)(arg);
  PlatformStandbyButtonClickedCallback();
}

static void _button_EL_Timeout_handler( void* arg )
{
  (void)(arg);
  _default_start_time = 0;
  PlatformEasyLinkButtonLongPressedCallback();
}

void platform_init_peripheral_irq_priorities( void )
{
  /* Interrupt priority setup. Called by MiCO/platform/MCU/STM32F2xx/platform_init.c */
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
}

void init_platform( void )
{
  MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_SYS_LED );
  MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );
  
  //  Initialise EasyLink buttons
  MicoGpioInitialize( (mico_gpio_t)EasyLink_BUTTON, INPUT_PULL_UP );
  mico_init_timer(&_button_EL_timer, RestoreDefault_TimeOut, _button_EL_Timeout_handler, NULL);
  MicoGpioEnableIRQ( (mico_gpio_t)EasyLink_BUTTON, IRQ_TRIGGER_BOTH_EDGES, _button_EL_irq_handler, NULL );
  
  //  Initialise Standby/wakeup switcher
  MicoGpioInitialize( (mico_gpio_t)Standby_SEL, INPUT_PULL_UP );
  MicoGpioEnableIRQ( (mico_gpio_t)Standby_SEL , IRQ_TRIGGER_FALLING_EDGE, _button_STANDBY_irq_handler, NULL);
}

void init_platform_bootloader( void )
{
  MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_SYS_LED );
  MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );
  
  MicoGpioInitialize(BOOT_SEL, INPUT_PULL_UP);
  MicoGpioInitialize(MFG_SEL, INPUT_PULL_UP);
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

bool MicoShouldEnterMFGMode(void)
{
   if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==false)
     return true;
   else
    return false;
}

bool MicoShouldEnterBootloader(void)
{
  if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==true)
    return true;
  else
    return false;
}


