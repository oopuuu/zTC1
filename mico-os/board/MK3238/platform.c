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

#include "mico_platform.h"
#include "platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_config.h"
#include "platform_logging.h"
#include "spi_flash_platform_interface.h"
#include "wlan_platform_common.h"
#include "platform_bluetooth.h"
#include "Keypad/gpio_button/button.h"


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
extern WEAK void PlatformEasyLinkButtonLongPressedCallback(void);
extern WEAK void bootloader_start(void);

/******************************************************
*               Variables Definitions
******************************************************/

const platform_gpio_t platform_gpio_pins[] =
{
  /* Common GPIOs for internal use */
  [BOOT_SEL]                          = { GPIOB,  1 },
  [MFG_SEL]                           = { GPIOB,  0 },
  [EasyLink_BUTTON]                   = { GPIOA,  1 },

  [FLASH_PIN_SPI_CS  ]                = { GPIOA, 15 },
  [FLASH_PIN_SPI_CLK ]                = { GPIOA,  5 },
  [FLASH_PIN_SPI_MOSI]                = { GPIOA,  7 },
  [FLASH_PIN_SPI_MISO]                = { GPIOB,  4 },

  /* GPIOs for external use */
  [MICO_GPIO_2]                       = { GPIOB,  2 },
  [MICO_GPIO_8]                       = { GPIOA , 2 },     //TXD
  [MICO_GPIO_9]                       = { GPIOA,  1 },
  [MICO_GPIO_12]                      = { GPIOA,  3 },     //RXD
  [MICO_GPIO_14]                      = { GPIOA,  0 },
  //[MICO_GPIO_16]                      = { GPIOC, 13 },   ////?????
  [MICO_GPIO_17]                      = { GPIOB, 10 },    //SCL
  [MICO_GPIO_18]                      = { GPIOB,  9 },    //SDA
  [MICO_GPIO_19]                      = { GPIOB, 12 },
  [MICO_GPIO_31]                      = { GPIOB,  8 },
  //[MICO_GPIO_33]                      = { GPIOB, 13 },
//  [MICO_GPIO_34]                      = { GPIOA,  5 },
//  [MICO_GPIO_35]                      = { GPIOA, 10 },
  [MICO_GPIO_36]                      = { GPIOB,  1 },
  [MICO_GPIO_37]                      = { GPIOB,  0 },
  //[MICO_GPIO_38]                      = { GPIOA,  4 },
};

const platform_adc_t *platform_adc_peripherals = NULL;
//const platform_adc_t platform_adc_peripherals[] =
//{
//  [MICO_ADC_1] = { ADC1, ADC_Channel_4, RCC_APB2Periph_ADC1, 1, (platform_gpio_t*)&platform_gpio_pins[MICO_GPIO_38] },
//  [MICO_ADC_2] = { ADC1, ADC_Channel_5, RCC_APB2Periph_ADC1, 1, (platform_gpio_t*)&platform_gpio_pins[MICO_GPIO_34] },
//};

const platform_i2c_t platform_i2c_peripherals[] =
{
  [MICO_I2C_1] =
  {
    .port                         = I2C2,
    .pin_scl                      = &platform_gpio_pins[MICO_GPIO_17],
    .pin_sda                      = &platform_gpio_pins[MICO_GPIO_18],
    .peripheral_clock_reg         = RCC_APB1Periph_I2C2,
    .tx_dma                       = DMA1,
    .tx_dma_peripheral_clock      = RCC_AHB1Periph_DMA1,
    .tx_dma_stream                = DMA1_Stream7,
    .rx_dma_stream                = DMA1_Stream5,
    .tx_dma_stream_id             = 7,
    .rx_dma_stream_id             = 5,
    .tx_dma_channel               = DMA_Channel_1,
    .rx_dma_channel               = DMA_Channel_1,
    .gpio_af_scl                  = GPIO_AF_I2C2,
    .gpio_af_sda                  = GPIO_AF9_I2C2
  },
};

platform_i2c_driver_t platform_i2c_drivers[MICO_I2C_MAX];

const platform_uart_t platform_uart_peripherals[] =
{
  [MICO_UART_1] =
  {
    .port                         = USART2,
    .pin_tx                       = &platform_gpio_pins[MICO_GPIO_8],
    .pin_rx                       = &platform_gpio_pins[MICO_GPIO_12],
    .pin_cts                      = NULL,
    .pin_rts                      = NULL,
    .tx_dma_config =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream6,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA1_Stream6_IRQn,
      .complete_flags             = DMA_HISR_TCIF6,
      .error_flags                = ( DMA_HISR_TEIF6 | DMA_HISR_FEIF6 ),
    },
    .rx_dma_config =
    {
      .controller                 = DMA1,
      .stream                     = DMA1_Stream5,
      .channel                    = DMA_Channel_4,
      .irq_vector                 = DMA1_Stream5_IRQn,
      .complete_flags             = DMA_HISR_TCIF5,
      .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
    },
  },
};
platform_uart_driver_t platform_uart_drivers[MICO_UART_MAX];

const platform_spi_t platform_spi_peripherals[] =
{
  [MICO_SPI_1]  =
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
      .controller                 = DMA2,
      .stream                     = DMA2_Stream5,
      .channel                    = DMA_Channel_3,
      .irq_vector                 = DMA2_Stream5_IRQn,
      .complete_flags             = DMA_HISR_TCIF5,
      .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5),
    },
    .rx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream0,
      .channel                    = DMA_Channel_3,
      .irq_vector                 = DMA2_Stream0_IRQn,
      .complete_flags             = DMA_LISR_TCIF0,
      .error_flags                = ( DMA_LISR_TEIF0 | DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 ),
    },
  }
};

platform_spi_driver_t platform_spi_drivers[MICO_SPI_MAX];

/* Flash memory devices */
const platform_flash_t platform_flash_peripherals[] =
{
  [MICO_FLASH_EMBEDDED] =
  {
    .flash_type                   = FLASH_TYPE_EMBEDDED,
    .flash_start_addr             = 0x08000000,
    .flash_length                 = 0x80000,
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
    .partition_length          =     0x8000,    //32k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_APPLICATION] =
  {
    .partition_owner           = MICO_FLASH_EMBEDDED,
    .partition_description     = "Application",
    .partition_start_addr      = 0x08008000,
    .partition_length          =    0x78000,   //480k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_ATE] =
  {
    .partition_owner           = MICO_FLASH_NONE,
  },
  [MICO_PARTITION_RF_FIRMWARE] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "RF Firmware",
    .partition_start_addr      = 0x2000,
    .partition_length          = 0x6E000,  //440k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_OTA_TEMP] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "OTA Storage",
    .partition_start_addr      = 0x70000,
    .partition_length          = 0x90000, //576k bytes
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
    .partition_description     = "PARAMETER2",
    .partition_start_addr      = 0x1000,
    .partition_length          = 0x1000, //4k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [MICO_PARTITION_BT_FIRMWARE] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "BT Firmware",
    .partition_start_addr      = 0x100000,
    .partition_length          = 0x10000,  //64k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  }
};

#if defined ( USE_MICO_SPI_FLASH )
const mico_spi_device_t mico_spi_flash =
{
  .port        = MICO_SPI_1,
  .chip_select = FLASH_PIN_SPI_CS,
  .speed       = 25000000,
  .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_NO_DMA | SPI_MSB_FIRST ),
  .bits        = 8
};
#endif

/* Wi-Fi control pins. Used by platform/MCU/wlan_platform_common.c
* SDIO: EMW1062_PIN_BOOTSTRAP[1:0] = b'00
* gSPI: EMW1062_PIN_BOOTSTRAP[1:0] = b'01
*/
const platform_gpio_t wifi_control_pins[] =
{
    [WIFI_PIN_POWER      ] = { GPIOB, 14 },
};

/* Wi-Fi SDIO bus pins. Used by platform/MCU/STM32F2xx/EMW1062_driver/wlan_SDIO.c */
const platform_gpio_t wifi_sdio_pins[] =
{
  [WIFI_PIN_SDIO_OOB_IRQ] = { GPIOA,  0 },
  [WIFI_PIN_SDIO_CLK    ] = { GPIOB, 15 },
  [WIFI_PIN_SDIO_CMD    ] = { GPIOA,  6 },
  [WIFI_PIN_SDIO_D0     ] = { GPIOB,  7 },
  [WIFI_PIN_SDIO_D1     ] = { GPIOA,  8 },
  [WIFI_PIN_SDIO_D2     ] = { GPIOA,  9 },
  [WIFI_PIN_SDIO_D3     ] = { GPIOB,  5 },
};

/* Bluetooth control pins.*/
static const platform_gpio_t internal_bt_control_pins[] =
{
    /* Reset pin unavailable */
    [MICO_BT_PIN_POWER      ] = { GPIOB, 13 },
    [MICO_BT_PIN_HOST_WAKE  ] = { GPIOA,  0 },
    [MICO_BT_PIN_DEVICE_WAKE] = { GPIOC, 13 }
};

const platform_gpio_t* mico_bt_control_pins[] =
{
    /* Reset pin unavailable */
    [MICO_BT_PIN_POWER      ] = &internal_bt_control_pins[MICO_BT_PIN_POWER      ],
    [MICO_BT_PIN_HOST_WAKE  ] = &internal_bt_control_pins[MICO_BT_PIN_HOST_WAKE  ],
    [MICO_BT_PIN_DEVICE_WAKE] = &internal_bt_control_pins[MICO_BT_PIN_DEVICE_WAKE],
    [MICO_BT_PIN_RESET      ] = NULL,
};

/* Bluetooth UART pins.*/
static const platform_gpio_t internal_bt_uart_pins[] =
{
    [MICO_BT_PIN_UART_TX ] = { GPIOB,  6 },
    [MICO_BT_PIN_UART_RX ] = { GPIOA, 10 },
    [MICO_BT_PIN_UART_CTS] = { GPIOA, 11 },
    [MICO_BT_PIN_UART_RTS] = { GPIOA, 12 },
};

const platform_gpio_t* mico_bt_uart_pins[] =
{
    [MICO_BT_PIN_UART_TX ] = &internal_bt_uart_pins[MICO_BT_PIN_UART_TX ],
    [MICO_BT_PIN_UART_RX ] = &internal_bt_uart_pins[MICO_BT_PIN_UART_RX ],
    [MICO_BT_PIN_UART_CTS] = &internal_bt_uart_pins[MICO_BT_PIN_UART_CTS],
    [MICO_BT_PIN_UART_RTS] = &internal_bt_uart_pins[MICO_BT_PIN_UART_RTS],
};

static const platform_uart_t internal_bt_uart_peripheral =
{
  .port                         = USART1,
  .pin_tx                       = &internal_bt_uart_pins[MICO_BT_PIN_UART_TX ],
  .pin_rx                       = &internal_bt_uart_pins[MICO_BT_PIN_UART_RX ],
  .pin_cts                      = &internal_bt_uart_pins[MICO_BT_PIN_UART_CTS ],
  .pin_rts                      = &internal_bt_uart_pins[MICO_BT_PIN_UART_RTS ],
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
};

static platform_uart_driver_t internal_bt_uart_driver;
const platform_uart_t*        mico_bt_uart_peripheral = &internal_bt_uart_peripheral;
platform_uart_driver_t*       mico_bt_uart_driver     = &internal_bt_uart_driver;


/* Bluetooth UART configuration. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_uart_config_t mico_bt_uart_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
};

/*BT chip specific configuration information*/
const platform_bluetooth_config_t mico_bt_config =
{
    .patchram_download_mode      = PATCHRAM_DOWNLOAD_MODE_MINIDRV_CMD,
    .patchram_download_baud_rate = 3000000,
    .featured_baud_rate          = 3000000
};

/******************************************************
*           Interrupt Handler Definitions
******************************************************/

MICO_RTOS_DEFINE_ISR( USART1_IRQHandler )
{
  platform_uart_irq( mico_bt_uart_driver );
}

MICO_RTOS_DEFINE_ISR( USART2_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA1_Stream6_IRQHandler )
{
  platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream7_IRQHandler )
{
  platform_uart_tx_dma_irq( mico_bt_uart_driver );
}

MICO_RTOS_DEFINE_ISR( DMA1_Stream5_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream2_IRQHandler )
{
  platform_uart_rx_dma_irq( mico_bt_uart_driver );
}


/******************************************************
*               Function Definitions
******************************************************/

void platform_init_peripheral_irq_priorities( void )
{
  /* Interrupt priority setup. Called by MiCO/platform/MCU/STM32F4xx/platform_init.c */
  NVIC_SetPriority( RTC_WKUP_IRQn    ,  1 ); /* RTC Wake-up event   */
  NVIC_SetPriority( SDIO_IRQn        ,  2 ); /* WLAN SDIO           */
  NVIC_SetPriority( DMA2_Stream3_IRQn,  3 ); /* WLAN SDIO DMA       */
  NVIC_SetPriority( DMA1_Stream3_IRQn,  3 ); /* WLAN SPI DMA        */
  NVIC_SetPriority( USART1_IRQn      ,  6 ); /* MICO_UART_1         */
  NVIC_SetPriority( USART2_IRQn      ,  6 ); /* MICO_UART_2         */
  NVIC_SetPriority( DMA1_Stream6_IRQn,  7 ); /* MICO_UART_1 TX DMA  */
  NVIC_SetPriority( DMA1_Stream5_IRQn,  7 ); /* MICO_UART_1 RX DMA  */
  NVIC_SetPriority( DMA2_Stream7_IRQn,  7 ); /* MICO_UART_2 TX DMA  */
  NVIC_SetPriority( DMA2_Stream2_IRQn,  7 ); /* MICO_UART_2 RX DMA  */
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
  button_init_t init;

  MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
  MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
  MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );

  MicoGpioInitialize((mico_gpio_t)BOOT_SEL, INPUT_PULL_UP);
  MicoGpioInitialize((mico_gpio_t)MFG_SEL, INPUT_PULL_UP);

  //  Initialise EasyLink buttons
  init.gpio = EasyLink_BUTTON;
  init.pressed_func = PlatformEasyLinkButtonClickedCallback;
  init.long_pressed_func = PlatformEasyLinkButtonLongPressedCallback;
  init.long_pressed_timeout = 5000;

  button_init( IOBUTTON_EASYLINK, init );

#ifndef NO_MICO_RTOS
  set_wifi_chip_id(2);
#endif
  //TODO: what for?
  MicoGpioInitialize( (mico_gpio_t)MICO_GPIO_31, OUTPUT_PUSH_PULL );
  
#ifdef USE_MiCOKit_EXT
  dc_motor_init( );
  dc_motor_set( 0 );
  
  rgb_led_init();
  rgb_led_open(0, 0, 0);
#endif
}


#ifdef BOOTLOADER
void init_platform_bootloader( void )
{
  MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
  MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
  MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );

  MicoGpioInitialize((mico_gpio_t)BOOT_SEL, INPUT_PULL_UP);
  MicoGpioInitialize((mico_gpio_t)MFG_SEL, INPUT_PULL_UP);

#ifdef USE_MiCOKit_EXT
  dc_motor_init( );
  dc_motor_set( 0 );
  
  rgb_led_init();
  rgb_led_open(0, 0, 0);
#endif
}

#endif

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


