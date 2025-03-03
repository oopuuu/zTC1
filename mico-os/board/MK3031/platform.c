/**
******************************************************************************
* @file    platform.c
* @author  William Xu
* @version V1.0.0
* @date    05-Oct-2016
* @brief   This file provides all MICO Peripherals mapping table and platform
*          specific functions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2016 MXCHIP Inc.
*
*  Permission is hereby gra nted, free of charge, to any person obtaining a copy
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

#include "common.h"
#include "mico_platform.h"

#include "platform.h"
#include "button.h"

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

/******************************************************
*               Variables Definitions
******************************************************/
static int uart2_redirect_pin = 0;

/* UART devices */
const platform_uart_t platform_uart_peripherals[] =
{
    [MICO_UART_1] =
    {
        .port_id = UART1_ID,
    },
    [MICO_UART_2] =
    {
        .port_id = UART2_ID,
    },
    [MICO_UART_3] =
    {
        .port_id = UART0_ID,
    }
};

platform_uart_driver_t platform_uart_drivers[MICO_UART_MAX];


/* Flash memory devices */
const platform_flash_t platform_flash_peripherals[] =
{
    [MICO_FLASH_SPI] =
    {
        .flash_type = FLASH_TYPE_SPI,
        .flash_start_addr = 0x0,
        .flash_length = 0x1FA000,
    },
};

platform_flash_driver_t platform_flash_drivers[MICO_FLASH_MAX];

platform_i2c_driver_t platform_i2c_drivers[MICO_I2C_MAX];

platform_spi_driver_t platform_spi_drivers[MICO_SPI_MAX];

/*
FC_COMP_BOOT2               0x0         0x6000  0 boot2
FC_COMP_FW                  0x6000      0x68000 0 mcufw
FC_COMP_FW                  0x6E000     0x68000 0 mcufw
FC_COMP_PSM                 0xD6000     0x4000  0 psm
FC_COMP_PSM                 0xDA000     0x4000  0 psm
FC_COMP_ATE                 0xDE000     0x4C000 0 ATE
FC_COMP_WLAN_FW             0x12a000    0x49000 0 wififw
FC_COMP_WLAN_FW             0x173000    0x49000 0 wififw
*/
typedef enum
{
    phy_PARTITION_BOOTLOADER,
    phy_PARTITION_APPLICATION1,
    phy_PARTITION_APPLICATION2,
    phy_PARTITION_ATE,
    phy_PARTITION_RF_FIRMWARE1,
    phy_PARTITION_PARAMETER_1,
    phy_PARTITION_PARAMETER_2,
    phy_PARTITION_USER,
    phy_PARTITION_NONE,
} phy_partition_t;

static char part1_desc[16] = "Application", part2_desc[16] = "Application";

const mico_logic_partition_t mico_partitions[] =
{
  [phy_PARTITION_BOOTLOADER] = // bootloader move to application
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "BOOTLOADER",
    .partition_start_addr      = 0x0,
    .partition_length          = 0x6000,   //24k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_APPLICATION1] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = part1_desc,
    .partition_start_addr      = 0x6000,
    .partition_length          = 0xA9000,   //676k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_APPLICATION2] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = part2_desc,
    .partition_start_addr      = 0xAF000,
    .partition_length          = 0xA9000,   //676k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },

  [phy_PARTITION_PARAMETER_1] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER1",
    .partition_start_addr      = 0x158000,
    .partition_length          = 0x4000, // 16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_PARAMETER_2] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "PARAMETER2",
    .partition_start_addr      = 0x15C000,
    .partition_length          = 0x4000, //16k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_ATE] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "ATE",
    .partition_start_addr      = 0x160000,
    .partition_length          = 0x40000, //256k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_RF_FIRMWARE1] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "RF Firmware",
    .partition_start_addr      = 0x1a0000,
    .partition_length          = 0x48000, //288k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [phy_PARTITION_USER] =
  {
    .partition_owner           = MICO_FLASH_SPI,
    .partition_description     = "User",
    .partition_start_addr      = 0x1e0000,
    .partition_length          = 0x18000, //96k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
};

/******************************************************
*               Function Definitions
******************************************************/
void init_platform( void )
{
#if defined (MOC) && (MOC == 1)
    button_init_t init;
    extern int get_last_reset_reason(void);

    if ( get_last_reset_reason() & LAST_RST_CAUSE_WDT )
    {
        platform_log( "WARNING: Watchdog reset occured previously." );
    }

    MicoGpioInitialize( (mico_gpio_t)MICO_SYS_LED, OUTPUT_PUSH_PULL );
    MicoGpioOutputLow( (mico_gpio_t)MICO_SYS_LED );
    MicoGpioInitialize( (mico_gpio_t)MICO_RF_LED, OUTPUT_PUSH_PULL );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );

    //	Initialise EasyLink buttons
    init.gpio = EasyLink_BUTTON;
    init.pressed_func = PlatformEasyLinkButtonClickedCallback;
    init.long_pressed_func = PlatformEasyLinkButtonLongPressedCallback;
    init.long_pressed_timeout = RestoreDefault_TimeOut;

    button_init( IOBUTTON_EASYLINK, init );

#ifdef USE_MiCOKit_EXT
  dc_motor_init( );
  dc_motor_set( 0 );

  rgb_led_init();
  rgb_led_open(0, 0, 0);
#endif

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

void MicoRfLed(bool onoff)
{
  if (onoff) {
    MicoGpioOutputLow( (mico_gpio_t)MICO_RF_LED );
  } else {
    MicoGpioOutputHigh( (mico_gpio_t)MICO_RF_LED );
  }
}

mico_logic_partition_t* paltform_flash_get_info(int inPartition)
{
    mico_logic_partition_t *logic_partition;
    int tmp;

    platform_flash_init( &platform_flash_peripherals[MICO_FLASH_SPI] );

    switch(inPartition)
    {
    case MICO_PARTITION_BOOTLOADER:
        logic_partition =  (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_BOOTLOADER];
        break;
    case MICO_PARTITION_APPLICATION:
        tmp = get_passive_firmware();
        if (tmp == 2)
            logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_APPLICATION1];
        else if (tmp == 1)
            logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_APPLICATION2];
        else
            logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_NONE];
        break;
    case MICO_PARTITION_ATE:
        logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_ATE];
        break;
    case MICO_PARTITION_OTA_TEMP: /* OTA always write the passive firmware */
        tmp = get_passive_firmware();
        if (tmp == 2)
            logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_APPLICATION2];
        else if (tmp == 1)
            logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_APPLICATION1];
        else
            logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_NONE];
        break;
    case MICO_PARTITION_RF_FIRMWARE: /* RF firmware always RW the active firmware*/
        logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_RF_FIRMWARE1];
        break;
    case MICO_PARTITION_PARAMETER_1:
        logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_PARAMETER_1];
        break;
    case MICO_PARTITION_PARAMETER_2:
        logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_PARAMETER_2];
        break;
    case MICO_PARTITION_USER: // user partition use the passive wifi firmware partition
        logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_USER];
        break;
    default:
        logic_partition = (mico_logic_partition_t *)&mico_partitions[phy_PARTITION_NONE];
        break;
    }

    return logic_partition;
}

WEAK int board_cpu_freq()
{
    return MCU_CLOCK_HZ;
}

WEAK void board_uart_pin_config( int id )
{
    switch ( id )
    {
        case UART0_ID:
            GPIO_PinMuxFun( GPIO_2, GPIO2_UART0_TXD );
            GPIO_PinMuxFun( GPIO_3, GPIO3_UART0_RXD );
            break;
        case UART1_ID:
            GPIO_PinMuxFun( GPIO_44, GPIO44_UART1_TXD );
            GPIO_PinMuxFun( GPIO_45, GPIO45_UART1_RXD );
            break;
        case UART2_ID:
            if ( uart2_redirect_pin == 0 )
            {
                GPIO_PinMuxFun( GPIO_48, GPIO48_UART2_TXD );
                GPIO_PinMuxFun( GPIO_49, GPIO49_UART2_RXD );
            } else
            {
                GPIO_PinMuxFun( GPIO_9, GPIO9_UART2_TXD );
                GPIO_PinMuxFun( GPIO_10, GPIO10_UART2_RXD );
            }
            break;
    }
}

void redirect_uart2_pin(void)
{
    uart2_redirect_pin = 1;
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


