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
#include "CheckSumUtils.h"
#include "keypad/gpio_button/button.h"

#ifdef USE_MiCOKit_STMEMS
#include "MiCOKit_STmems/MiCOKit_STmems.h"
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

/* This table maps STM32 pins to GPIO definitions on the schematic*/
const platform_gpio_t platform_gpio_pins[] =
{
  /* Common GPIOs for internal use */  
//  [MFG_SEL]                           = { GPIOB,  4 }, 
  [EasyLink_BUTTON]                   = { GPIOC, 13 }, 
  [MICO_SYS_LED]                      = { GPIOA,  5 }, 

  /* GPIOs for external use */
  [MICO_GPIO_1]                       = { GPIOA,  3 },
  [MICO_GPIO_2]                       = { GPIOA,  2 },
  [MICO_GPIO_3]                       = { GPIOA, 10 },
  [MICO_GPIO_4]                       = { GPIOB,  3 },
  [MICO_GPIO_5]                       = { GPIOB,  5 },
  [MICO_GPIO_6]                       = { GPIOB,  4 },
  [MICO_GPIO_7]                       = { GPIOB, 10 },
  [MICO_GPIO_8]                       = { GPIOA,  8 },
  
  [MICO_GPIO_9]                       = { GPIOA,  9 },
  [MICO_GPIO_10]                      = { GPIOC,  7 },
  [MICO_GPIO_11]                      = { GPIOB,  6 },
  [MICO_GPIO_12]                      = { GPIOA,  7 },
  [MICO_GPIO_13]                      = { GPIOA,  6 },
  [MICO_GPIO_14]                      = { GPIOA,  5 },
  [MICO_GPIO_17]                      = { GPIOB,  9 },
  [MICO_GPIO_18]                      = { GPIOB,  8 },
  
  [MICO_GPIO_19]                      = { GPIOC,  0 },
  [MICO_GPIO_20]                      = { GPIOC,  1 },
  [MICO_GPIO_21]                      = { GPIOB,  0 },
  [MICO_GPIO_22]                      = { GPIOA,  4 }, //A2
  [MICO_GPIO_23]                      = { GPIOA,  1 }, //A1
  [MICO_GPIO_24]                      = { GPIOA,  0 }, //A0

};

const platform_pwm_t *platform_pwm_peripherals = NULL;

const platform_i2c_t platform_i2c_peripherals[] =
{
  [MICO_I2C_1] =
  {
    .port                         = I2C1,
    .pin_scl                      = &platform_gpio_pins[MICO_GPIO_18],
    .pin_sda                      = &platform_gpio_pins[MICO_GPIO_17],
    .peripheral_clock_reg         = RCC_APB1Periph_I2C1,
    .tx_dma                       = DMA1,
    .tx_dma_peripheral_clock      = RCC_AHB1Periph_DMA1,
    .tx_dma_stream                = DMA1_Stream7,
    .rx_dma_stream                = DMA1_Stream5,
    .tx_dma_stream_id             = 7,
    .rx_dma_stream_id             = 5,
    .tx_dma_channel               = DMA_Channel_1,
    .rx_dma_channel               = DMA_Channel_1,
    .gpio_af_scl                  = GPIO_AF_I2C1,
    .gpio_af_sda                  = GPIO_AF_I2C1
  },
};

platform_i2c_driver_t platform_i2c_drivers[MICO_I2C_MAX];

const platform_uart_t platform_uart_peripherals[] =
{
  [MICO_UART_1] =
  {
    .port                         = USART2,
    .pin_tx                       = &platform_gpio_pins[MICO_GPIO_1],
    .pin_rx                       = &platform_gpio_pins[MICO_GPIO_2],
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
//  [MICO_UART_2] =
//  {
//    .port                         = USART1,
//    .pin_tx                       = &platform_gpio_pins[MICO_GPIO_30],
//    .pin_rx                       = &platform_gpio_pins[MICO_GPIO_29],
//    .pin_cts                      = NULL,
//    .pin_rts                      = NULL,
//    .tx_dma_config =
//    {
//      .controller                 = DMA2,
//      .stream                     = DMA2_Stream7,
//      .channel                    = DMA_Channel_4,
//      .irq_vector                 = DMA2_Stream7_IRQn,
//      .complete_flags             = DMA_HISR_TCIF7,
//      .error_flags                = ( DMA_HISR_TEIF7 | DMA_HISR_FEIF7 ),
//    },
//    .rx_dma_config =
//    {
//      .controller                 = DMA2,
//      .stream                     = DMA2_Stream2,
//      .channel                    = DMA_Channel_4,
//      .irq_vector                 = DMA2_Stream2_IRQn,
//      .complete_flags             = DMA_LISR_TCIF2,
//      .error_flags                = ( DMA_LISR_TEIF2 | DMA_LISR_FEIF2 | DMA_LISR_DMEIF2 ),
//    },
//  },
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
    .pin_mosi                     = &platform_gpio_pins[MICO_GPIO_12],
    .pin_miso                     = &platform_gpio_pins[MICO_GPIO_13],
    .pin_clock                    = &platform_gpio_pins[MICO_GPIO_14],
    .tx_dma =
    {
      .controller                 = DMA2,
      .stream                     = DMA2_Stream5,
      .channel                    = DMA_Channel_3,
      .irq_vector                 = DMA2_Stream5_IRQn,
      .complete_flags             = DMA_HISR_TCIF5,
      .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 ),
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
    .flash_length                 = 0x200000,   //2M
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
    .partition_start_addr      = 0x0800C000,
    .partition_length          =    0x74000,   //464k bytes
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
    .partition_length          = 0x3E000,  //248k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_OTA_TEMP] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "OTA Storage",
    .partition_start_addr      = 0x40000,
    .partition_length          = 0x70000, //448k bytes
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
  }
};


#if defined ( USE_MICO_SPI_FLASH )
const mico_spi_device_t mico_spi_flash =
{
  .port        = MICO_SPI_1,
  .chip_select = MICO_GPIO_8,
  .speed       = 40000000,
  .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_USE_DMA | SPI_MSB_FIRST ),
  .bits        = 8
};
#endif

const platform_adc_t platform_adc_peripherals[] =
{
  [MICO_ADC_1] = { ADC1, ADC_Channel_0, RCC_APB2Periph_ADC1, 1, (platform_gpio_t*)&platform_gpio_pins[MICO_GPIO_24] },
  [MICO_ADC_2] = { ADC1, ADC_Channel_1, RCC_APB2Periph_ADC1, 1, (platform_gpio_t*)&platform_gpio_pins[MICO_GPIO_23] },
  [MICO_ADC_3] = { ADC1, ADC_Channel_4, RCC_APB2Periph_ADC1, 1, (platform_gpio_t*)&platform_gpio_pins[MICO_GPIO_22] },
};

/* Wi-Fi control pins. Used by platform/MCU/wlan_platform_common.c
* SDIO: EMW1062_PIN_BOOTSTRAP[1:0] = b'00
* gSPI: EMW1062_PIN_BOOTSTRAP[1:0] = b'01
*/
const platform_gpio_t wifi_control_pins[] =
{
  [WIFI_PIN_RESET   ]        = { GPIOA,  0 },
  [WIFI_PIN_32K_CLK]         = { GPIOC,  1 },
};

/* Wi-Fi gSPI bus pins. Used by platform/MCU/STM32F2xx/EMW1062_driver/wlan_spi.c */
const platform_gpio_t wifi_spi_pins[] =
{
  [WIFI_PIN_SPI_IRQ ] = { GPIOA,  1 },
  [WIFI_PIN_SPI_CS  ] = { GPIOB,  6 },
  [WIFI_PIN_SPI_CLK ] = { GPIOA,  5 },
  [WIFI_PIN_SPI_MOSI] = { GPIOA,  7 },
  [WIFI_PIN_SPI_MISO] = { GPIOA,  6 },
};

const platform_spi_t wifi_spi =
{
  .port                         = SPI1,
  .gpio_af                      = GPIO_AF_SPI1,
  .peripheral_clock_reg         = RCC_APB2Periph_SPI1,
  .peripheral_clock_func        = RCC_APB2PeriphClockCmd,
  .pin_mosi                     = &wifi_spi_pins[WIFI_PIN_SPI_MOSI],
  .pin_miso                     = &wifi_spi_pins[WIFI_PIN_SPI_MISO],
  .pin_clock                    = &wifi_spi_pins[WIFI_PIN_SPI_CLK],
  .tx_dma =
  {
    .controller                 = DMA2,
    .stream                     = DMA2_Stream5,
    .channel                    = DMA_Channel_3,
    .irq_vector                 = DMA2_Stream5_IRQn,
    .complete_flags             = DMA_HISR_TCIF5,
    .error_flags                = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 ),
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
};



/******************************************************
*           Interrupt Handler Definitions
******************************************************/
#if defined (MICO_WIFI_SHARE_SPI_BUS) || !defined (MICO_NO_WIFI)
MICO_RTOS_DEFINE_ISR( DMA2_Stream0_IRQHandler )
{
  platform_wifi_spi_rx_dma_irq( );
}
#endif

MICO_RTOS_DEFINE_ISR( USART1_IRQHandler )
{
  platform_uart_irq( &platform_uart_drivers[MICO_UART_2] );
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
  platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_2] );
}

MICO_RTOS_DEFINE_ISR( DMA1_Stream5_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
}

MICO_RTOS_DEFINE_ISR( DMA2_Stream2_IRQHandler )
{
  platform_uart_rx_dma_irq( &platform_uart_drivers[MICO_UART_2] );
}


/******************************************************
*               Function Definitions
******************************************************/

void platform_init_peripheral_irq_priorities( void )
{
  /* Interrupt priority setup. Called by MiCO/platform/MCU/STM32F2xx/platform_init.c */
  NVIC_SetPriority( RTC_WKUP_IRQn    ,  1 ); /* RTC Wake-up event   */
  NVIC_SetPriority( SDIO_IRQn        ,  2 ); /* WLAN SDIO           */
  NVIC_SetPriority( DMA2_Stream3_IRQn,  3 ); /* WLAN SDIO DMA       */
  NVIC_SetPriority( DMA2_Stream0_IRQn,  3 ); /* WLAN SPI DMA        */
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
  
  //Initialise EasyLink buttons
  init.gpio = EasyLink_BUTTON;
  init.pressed_func = PlatformEasyLinkButtonClickedCallback;
  init.long_pressed_func = PlatformEasyLinkButtonLongPressedCallback;
  init.long_pressed_timeout = 5000;

  button_init( IOBUTTON_EASYLINK, init );

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
}

#endif

void MicoSysLed(bool onoff)
{
  if (onoff) {
    MicoGpioOutputHigh( (mico_gpio_t)MICO_SYS_LED );
  } else {
    MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
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
//  if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==false)
//    return true;
//  else
    return false;
}

bool MicoShouldEnterBootloader(void)
{
//  if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==true)
//    return true;
//  else
    return false;
}

