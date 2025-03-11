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
#include "platform_config.h"
#include "platform_logging.h"
#include "spi_flash_platform_interface.h"
#include "wlan_platform_common.h"

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

/******************************************************
*               Variables Definitions
******************************************************/

static uint32_t _default_start_time = 0;
static mico_timer_t _button_EL_timer;
const uint32_t ExtClockIn = 0;
const platform_gpio_t platform_gpio_pins[] =
{
    /* GPIOs for mico use */
    [NXP_GPIO_0_27]  = { 0, 27 },
    [NXP_GPIO_0_28]  = { 0, 28 },
    [NXP_GPIO_0_29]  = { 0, 29 },
    [NXP_GPIO_0_30]  = { 0, 30 },
    [NXP_GPIO_0_20]  = { 0, 20 },

    [NXP_GPIO_1_12]  = { 1, 12 },
    [NXP_GPIO_1_13]  = { 1, 13 },
    [NXP_GPIO_0_7 ]  = { 0, 7  },
    [NXP_GPIO_1_15]  = { 1, 15 },
    [NXP_GPIO_1_14]  = { 1, 14 },
    [NXP_GPIO_0_10]  = { 0, 10 },
    [NXP_GPIO_0_5 ]  = { 0, 5  },
    [NXP_GPIO_0_6 ]  = { 0, 6  },

    [NXP_GPIO_0_19]  = { 0, 19 },
    [NXP_GPIO_0_18]  = { 0, 18 },
    [NXP_GPIO_0_15]  = { 0, 15 },
    [NXP_GPIO_0_12]  = { 0, 12 },
    [NXP_GPIO_0_13]  = { 0, 13 },
    [NXP_GPIO_0_11]  = { 0, 11 },
    [NXP_GPIO_0_24]  = { 0, 24 },
    [NXP_GPIO_0_23]  = { 0, 23 },

    [NXP_GPIO_1_0 ]  = { 1, 0 },
    [NXP_GPIO_1_1 ]  = { 1, 1 },
    [NXP_GPIO_1_2 ]  = { 1, 2 },
    [NXP_GPIO_1_3 ]  = { 1, 3 },
    [NXP_GPIO_1_4 ]  = { 1, 4 },
    [NXP_GPIO_1_5 ]  = { 1, 5 },
    [FLASH_PIN_SPI_CS]  = { 0, 14 },
};

const platform_gpio_t sys_gpio_pins[] =
{
    /* GPIOs for sys use */
    [CLKOUT             ]  = { 0, 21 },
    [CLKIN              ]  = { 0, 22 },

    [STDIO_UART_RX      ]  = { 0, 0  },
    [STDIO_UART_TX      ]  = { 0, 1  },

    [SPI0_PIN_CLK       ]  = { 0, 11 },
    [SPI0_PIN_MOSI      ]  = { 0, 12 },
    [SPI0_PIN_MISO      ]  = { 0, 13 },
};

const platform_pwm_t platform_pwm_peripherals[] =
{
	[MICO_PWM_0]  =
	{
		.port               = LPC_SCT,
		.pwmNdx = PWMNDX_SCTL_BASE + 0,
		.portNdx = 0,
		.pinNdx = 18,
		.pinMux = 2,
	},

	[MICO_PWM_1]  =
	{
		.pTMR		= LPC_TIMER0,
		.pwmNdx = 1,
		.portNdx = 0,
		.pinNdx = 19,
		.pinMux = 3,
	},

	[MICO_PWM_2]  =
	{
		.pTMR	= LPC_TIMER3,
		.pwmNdx = 0,
		.portNdx = 0,
		.pinNdx = 10,
		.pinMux = 3,
	},

	[MICO_PWM_3]  =
	{
		.port				= LPC_SCT,
		.pwmNdx = PWMNDX_SCTL_BASE + 7,
		.portNdx = 1,
		.pinNdx = 14,
		.pinMux = 2,
	},

	// >>> RockyS: debug, use on-board LED
	[MICO_PWM_4]  =
	{
		.port				= LPC_SCT,
		.pwmNdx = PWMNDX_SCTL_BASE + 2,
		.portNdx = 0,
		.pinNdx = 29,
		.pinMux = 2,
	},

	[MICO_PWM_5]  =
	{
		.pTMR				= LPC_TIMER0,
		.pwmNdx = 2,
		.portNdx = 0,
		.pinNdx = 30,
		.pinMux = 3,
	},
	// <<<

};

//const platform_i2c_t platform_i2c_peripherals[]
extern platform_i2c_cb_t s_i2cCtxs[MICO_I2C_MAX];

platform_i2c_t platform_i2c_peripherals[] =
{
  [MICO_I2C_0] =
  {
    .port = LPC_I2C0,
    .hwNdx = 0,
    .irqNdx = I2C0_IRQn,
    .pCtx = s_i2cCtxs + 0,
  },
//  [MICO_I2C_1] =
//  {
//    .port = LPC_I2C1,
//    .hwNdx = 1,
//    .irqNdx = I2C1_IRQn,
//    .pCtx = s_i2cCtxs + 1,
//  },
//
//  [MICO_I2C_2] =
//  {
//    .port = LPC_I2C2,
//    .hwNdx = 2,
//    .irqNdx = I2C2_IRQn,
//    .pCtx = s_i2cCtxs + 2,
//  },

};

platform_i2c_driver_t platform_i2c_drivers[MICO_I2C_MAX];


const platform_uart_t platform_uart_peripherals[] =
{
  [MICO_UART_0] =
  {
    .port                         = LPC_USART0,
    .pin_tx                       = &sys_gpio_pins[STDIO_UART_TX],
    .pin_rx                       = &sys_gpio_pins[STDIO_UART_RX],
    .pin_cts                      = NULL,
    .pin_rts                      = NULL,
    .hwNdx = 0,
    .irqNdx = UART0_IRQn,
#ifndef BOOTLOADER
    .isRxFifoEn = 0,
#else
    .isRxFifoEn = 1,
#endif
  },

  [MICO_UART_1] =
  {
    .port                         = LPC_USART3,
    .pin_tx                       = &platform_gpio_pins[NXP_GPIO_1_13],
    .pin_rx                       = &platform_gpio_pins[NXP_GPIO_1_12],
    .pin_cts                      = NULL,
    .pin_rts                      = NULL,
    .hwNdx = 3,
    .irqNdx = UART3_IRQn,
#ifndef BOOTLOADER
    .isRxFifoEn = 1,
#else
    .isRxFifoEn = 0,
#endif

  },
};

platform_uart_driver_t platform_uart_drivers[MICO_UART_MAX];

const platform_spi_t platform_spi_peripherals[] =
{
  [MICO_SPI_0]  =
   {
    .port                         = LPC_SPI0,
    .pin_mosi                     = &sys_gpio_pins[SPI0_PIN_MOSI],
    .pin_miso                     = &sys_gpio_pins[SPI0_PIN_MISO],
    .pin_clock                    = &sys_gpio_pins[SPI0_PIN_CLK],
    .dmaRxChnNdx                  = DMAREQ_SPI0_RX,
    .dmaTxChnNdx		  = DMAREQ_SPI0_TX,
   },
};

platform_spi_driver_t platform_spi_drivers[MICO_SPI_MAX] =
{
	[MICO_SPI_0] =
	{
		.peripheral = platform_spi_peripherals + 0,
	},
};

/* Flash memory devices */
const platform_flash_t platform_flash_peripherals[] =
{
  [MICO_FLASH_EMBEDDED] =
  {
    .flash_type                   = FLASH_TYPE_EMBEDDED,
    .flash_start_addr             = 0x00000000,
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


#if defined ( USE_MICO_SPI_FLASH )
const mico_spi_device_t mico_spi_flash =
{
    .port        = MICO_SPI_0,
    .chip_select = FLASH_PIN_SPI_CS,
    .speed       = 40000000,
    .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH |/* SPI_USE_DMA | */SPI_MSB_FIRST),
    .bits        = 8
};
#endif

#define ADC_SAMPLE_RATE 10000000 // 10MHz
const platform_adc_t platform_adc_peripherals[] =
{
  [MICO_ADC_0] = { LPC_ADC, 0, ADC_SAMPLE_RATE, ADC_SEQA_IDX, (platform_gpio_t*)&platform_gpio_pins[NXP_GPIO_1_2] },
  [MICO_ADC_1] = { LPC_ADC, 1, ADC_SAMPLE_RATE, ADC_SEQA_IDX, (platform_gpio_t*)&platform_gpio_pins[NXP_GPIO_1_3] },
};

/* Wi-Fi control pins. Used by platform/MCU/wlan_platform_common.c
*/
const platform_gpio_t wifi_control_pins[] =
{
  [WIFI_PIN_RESET      ] = { 0, 3 },
#if defined ( MICO_USE_WIFI_32K_CLOCK_MCO )
  [WIFI_PIN_32K_CLK    ] = { 0, 21 },
#else
  [WIFI_PIN_32K_CLK    ] = { 0, 21 },
#endif
};

/* Wi-Fi gSPI bus pins. Used by platform/MCU/STM32F2xx/EMW1062_driver/wlan_spi.c */
const platform_gpio_t wifi_spi_pins[] =
{
  [WIFI_PIN_SPI_IRQ ] = { 1, 10 },
  [WIFI_PIN_SPI_CS  ] = { 1, 9 },
  [WIFI_PIN_SPI_CLK ] = { 1, 6 },
  [WIFI_PIN_SPI_MOSI] = { 1, 7 },
  [WIFI_PIN_SPI_MISO] = { 1, 8 },
};


const platform_spi_t wifi_spi =
{
  .port                         = LPC_SPI1,
  .pin_mosi                     = &wifi_spi_pins[WIFI_PIN_SPI_MOSI],
  .pin_miso                     = &wifi_spi_pins[WIFI_PIN_SPI_MISO],
  .pin_clock                    = &wifi_spi_pins[WIFI_PIN_SPI_CLK],
};

/* Logic partition on flash devices */
const mico_logic_partition_t mico_partitions[] =
{
  [MICO_PARTITION_BOOTLOADER] =
  {
    .partition_owner           = MICO_FLASH_EMBEDDED,
    .partition_description     = "Bootloader",
    .partition_start_addr      = 0x00000000,
    .partition_length          =     0x8000,    //32k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_APPLICATION] =
  {
    .partition_owner           = MICO_FLASH_EMBEDDED,
    .partition_description     = "Application",
    .partition_start_addr      = 0x00008000,
    .partition_length          =    0x74000,   //480k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_RF_FIRMWARE] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "RF Firmware",
    .partition_start_addr      = 0x2000,
    .partition_length          = 0x4E000,  //312k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [MICO_PARTITION_OTA_TEMP] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "OTA Storage",
    .partition_start_addr      = 0x50000,
    .partition_length          = 0x74000, //768k bytes
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
  }
};


void DmaAbort(DMA_CHID_T dmaCh)
{
	Chip_DMA_DisableChannel(LPC_DMA, dmaCh);
	while ((Chip_DMA_GetBusyChannels(LPC_DMA) & (1 << dmaCh)) != 0) {}
	Chip_DMA_AbortChannel(LPC_DMA, dmaCh);
	Chip_DMA_EnableChannel(LPC_DMA, dmaCh);
}
/******************************************************
*           Interrupt Handler Definitions
******************************************************/


#ifndef BOOTLOADER
extern volatile uint8_t bDMASPITXDoneFlag;
extern volatile uint8_t bDMASPIRXDoneFlag;
extern volatile uint8_t g_isWlanRx;
extern void wlan_spi_xfer_done(uint32_t isRx);
#endif
extern void OnSPIDmaXferDone(uint32_t spiNdx, uint32_t isRx, uint32_t isErr);
MICO_RTOS_DEFINE_ISR( DMA_IRQHandler )
{
  uint32_t err = g_pDMA->DMACOMMON[0].ERRINT;
  uint32_t isErr;
  /* Clear DMA interrupt for the channel */
  if ( (Chip_DMA_GetIntStatus(LPC_DMA) & DMA_INTSTAT_ACTIVEINT) != 0 ) {
#ifndef BOOTLOADER
    // SPI1 : WLAN
#ifndef MICO_NO_WIFI
    if ( Chip_DMA_GetActiveIntAChannels(LPC_DMA) & (1 << DMAREQ_SPI1_TX) ) {
            Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMAREQ_SPI1_TX);
            // since SPI is fast enough, we assume RX complete will happen wthin this block
            bDMASPITXDoneFlag = true;
            bDMASPIRXDoneFlag = true;
            wlan_spi_xfer_done(0);
    }
    if ( Chip_DMA_GetActiveIntAChannels(LPC_DMA) & (1 << DMAREQ_SPI1_RX) ) {
            Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMAREQ_SPI1_RX);
            //bDMASPIRXDoneFlag = true;
            //wlan_spi_xfer_done(1);
    }
#endif
#endif
    // SPI0 : General SPI
    if ( Chip_DMA_GetActiveIntAChannels(LPC_DMA) & (1 << DMAREQ_SPI0_TX) ) {
            Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMAREQ_SPI0_TX);
            isErr = (err & (1UL<<DMAREQ_SPI0_TX)) ? 1 : 0;
            OnSPIDmaXferDone(0, 0, isErr);
    }
    if ( Chip_DMA_GetActiveIntAChannels(LPC_DMA) & (1 << DMAREQ_SPI0_RX) ) {
            Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMAREQ_SPI0_RX);
            isErr = (err & (1UL<<DMAREQ_SPI0_RX)) ? 1 : 0;
            OnSPIDmaXferDone(0, 1, isErr);
    }

    // USART0 : Console
    if ( Chip_DMA_GetActiveIntAChannels(LPC_DMA) & (1 << DMAREQ_UART0_TX) ) {
            Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMAREQ_UART0_TX);
            platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_0] );
    }

    // USART1 : App
    if ( Chip_DMA_GetActiveIntAChannels(LPC_DMA) & (1 << DMAREQ_UART1_TX) ) {
            Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMAREQ_UART1_TX);
            platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
    }
    if ( Chip_DMA_GetActiveIntAChannels(LPC_DMA) & (1 << DMAREQ_UART2_TX) ) {
            Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMAREQ_UART2_TX);
            platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
    }
    if ( Chip_DMA_GetActiveIntAChannels(LPC_DMA) & (1 << DMAREQ_UART3_TX) ) {
            Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMAREQ_UART3_TX);
            platform_uart_tx_dma_irq( &platform_uart_drivers[MICO_UART_1] );
    }

  }
}

extern void OnUartIRQ(LPC_USART_T *p);

MICO_RTOS_DEFINE_ISR( UART0_IRQHandler )
{
    OnUartIRQ(LPC_USART0);
}

MICO_RTOS_DEFINE_ISR( UART1_IRQHandler )
{
    OnUartIRQ(LPC_USART1);
}
MICO_RTOS_DEFINE_ISR( UART2_IRQHandler )
{
    OnUartIRQ(LPC_USART2);
}
MICO_RTOS_DEFINE_ISR( UART3_IRQHandler )
{
    OnUartIRQ(LPC_USART3);
}


extern void I2CMIrqHandler(platform_i2c_t *pDev);

MICO_RTOS_DEFINE_ISR(I2C0_IRQHandler)
{
	I2CMIrqHandler(platform_i2c_peripherals + 0);
}

//MICO_RTOS_DEFINE_ISR(I2C1_IRQHandler)
//{
//	I2CMIrqHandler(platform_i2c_peripherals + 1);
//}
//
//MICO_RTOS_DEFINE_ISR(I2C2_IRQHandler)
//{
//	I2CMIrqHandler(platform_i2c_peripherals + 2);
//}

void platform_init_peripheral_irq_priorities( void )
{
  uint32_t i;
  uint32_t minPri = (1<<__NVIC_PRIO_BITS) - 1 - 1;
  NVIC_SetPriorityGrouping(0x00000000);

  for (i=0; i<48; i++)
  {
      // NVIC->IP[i] = 0xC0;
      NVIC_SetPriority((IRQn_Type)i, minPri);
  }
  NVIC_SetPriority(SysTick_IRQn, minPri);
  /* Interrupt priority setup. Called by MiCO/platform/MCU/STM32F2xx/platform_init.c */
  NVIC_SetPriority( RTC_IRQn         ,  minPri ); /* RTC Wake-up event   */
  NVIC_SetPriority( SPI0_IRQn        ,  minPri ); /* SPI0          */
  NVIC_SetPriority( SPI1_IRQn        ,  3 ); /* WLAN SDIO           */
  NVIC_SetPriority( DMA_IRQn         ,  3 ); /* WLAN SPI DMA        */
  NVIC_SetPriority( UART0_IRQn       ,  minPri - 1); /* MICO_UART_0         */
  NVIC_SetPriority( UART1_IRQn       ,  minPri - 1); /* MICO_UART_1         */
  NVIC_SetPriority( UART2_IRQn       ,  minPri - 1); /* MICO_UART_2         */
  NVIC_SetPriority( UART3_IRQn       ,  minPri - 1); /* MICO_UART_3         */
  NVIC_SetPriority( I2C0_IRQn        ,  minPri ); /* MICO_I2C_0         */
  NVIC_SetPriority( I2C1_IRQn        ,  minPri ); /* MICO_I2C_1         */
  NVIC_SetPriority( I2C2_IRQn        ,  minPri ); /* MICO_I2C_2         */

  NVIC_SetPriority( PIN_INT0_IRQn    , minPri ); /* GPIO                */
  NVIC_SetPriority( PIN_INT1_IRQn    , minPri ); /* GPIO                */
  NVIC_SetPriority( PIN_INT2_IRQn    , minPri ); /* GPIO                */
  NVIC_SetPriority( PIN_INT3_IRQn    , minPri ); /* GPIO                */
  NVIC_SetPriority( PIN_INT4_IRQn    , minPri ); /* GPIO                */
  NVIC_SetPriority( PIN_INT5_IRQn    , minPri ); /* GPIO                */
  NVIC_SetPriority( PIN_INT6_IRQn    , minPri ); /* GPIO                */
  NVIC_SetPriority( PIN_INT7_IRQn    , minPri ); /* GPIO                */
}

/******************************************************
*               Function Definitions
******************************************************/

static void _button_EL_irq_handler( void* arg )
{
  (void)(arg);
  int interval = -1;

  if ( MicoGpioInputGet( (mico_gpio_t)EasyLink_BUTTON ) == 1 ) {
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

static void _button_EL_Timeout_handler( void* arg )
{
  (void)(arg);
  _default_start_time = 0;
  PlatformEasyLinkButtonLongPressedCallback();
}

void init_platform( void )
{
   MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
   MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
   MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL );
   MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );

   //  Initialise EasyLink buttons
   MicoGpioInitialize( (mico_gpio_t)EasyLink_BUTTON, INPUT_HIGH_IMPEDANCE );
   mico_init_timer(&_button_EL_timer, RestoreDefault_TimeOut, _button_EL_Timeout_handler, NULL);
   MicoGpioEnableIRQ( (mico_gpio_t)EasyLink_BUTTON, IRQ_TRIGGER_BOTH_EDGES, _button_EL_irq_handler, NULL );

#ifdef USE_MiCOKit_EXT
  dc_motor_init( );
  dc_motor_set( 0 );
#endif
}

void init_platform_bootloader( void )
{
  MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
  MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
  MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_OPEN_DRAIN_NO_PULL );
  MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );

  MicoGpioInitialize((mico_gpio_t)BOOT_SEL, INPUT_PULL_UP);
  MicoGpioInitialize((mico_gpio_t)MFG_SEL, INPUT_HIGH_IMPEDANCE);

#ifdef USE_MiCOKit_EXT
  dc_motor_init( );
  dc_motor_set( 0 );

  rgb_led_init();
  rgb_led_open(0, 0, 0);
#endif
}

void MicoSysLed(bool onoff)
{
    if (onoff) {
        MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
    } else {
        MicoGpioOutputHigh( (mico_gpio_t)MICO_SYS_LED );
    }
}

// Only one led on base board, so use system led as RF led.
void MicoRfLed(bool onoff)
{
    if (onoff) {
        MicoSysLed(true);
    } else {
        MicoSysLed(false);
    }
}

#ifdef USE_MiCOKit_EXT
// add test mode for MiCOKit-EXT board,check Arduino_D5 pin when system startup
bool MicoExtShouldEnterTestMode(void)
{
  if( MicoGpioInputGet((mico_gpio_t)Arduino_D5)==false ){
    return true;
  }
  else{
    return false;
  }
}
#endif

// add long press key2 on ext-board when restart to enter MFG MODE
bool MicoShouldEnterMFGMode(void)
{
  if( MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==false )
  {
    return true;
  }
  else{
    return false;
  }
}

// bootloader mode: SW1=ON, SW2=OFF
bool MicoShouldEnterBootloader(void)
{
  if(MicoGpioInputGet((mico_gpio_t)BOOT_SEL)==false && MicoGpioInputGet((mico_gpio_t)MFG_SEL)==true)
    return true;
  else
    return false;
}
