/****************************************************************************//**
 * @file     mc200_pinmux.h
 * @brief    PINMUX driver module header file.
 * @version  V1.2.0
 * @date     29-May-2013
 * @author   CE Application Team
 *
 * @note
 * Copyright (C) 2012 Marvell Technology Group Ltd. All rights reserved.
 *
 * @par
 * Marvell is supplying this software which provides customers with programming
 * information regarding the products. Marvell has no responsibility or 
 * liability for the use of the software. Marvell not guarantee the correctness 
 * of this software. Marvell reserves the right to make changes in the software 
 * without notification. 
 * 
 *******************************************************************************/

#ifndef __MC200_PINMUX_H__
#define __MC200_PINMUX_H__

#include "mc200.h"
#include "mc200_driver.h"
#include "mc200_gpio.h"

/** @addtogroup MC200_Periph_Driver
 *  @{
 */

/** @addtogroup PINMUX 
 *  @{
 */
  
/** @defgroup PINMUX_Public_Types PINMUX_Public_Types 
 *  @{
 */

/**  
 *  @brief GPIO Pinmux function type definition 
 */
typedef enum
{
  PINMUX_FUNCTION_0 = 0,                   /*!< GPIO pin mux function 0 define */
  PINMUX_FUNCTION_1,                       /*!< GPIO pin mux function 1 define */
  PINMUX_FUNCTION_2,                       /*!< GPIO pin mux function 2 define */
  PINMUX_FUNCTION_3,                       /*!< GPIO pin mux function 3 define */
  PINMUX_FUNCTION_4,                       /*!< GPIO pin mux function 4 define */
  PINMUX_FUNCTION_5,                       /*!< GPIO pin mux function 5 define */
  PINMUX_FUNCTION_6,                       /*!< GPIO pin mux function 6 define */
  PINMUX_FUNCTION_7,                       /*!< GPIO pin mux function 7 define */
}GPIO_PinMuxFunc_Type;

/**  
 *  @brief GPIO pin mode type definition 
 */
typedef enum
{
  PINMODE_DEFAULT = 0,                      /*!< GPIO pin mode default define */
  PINMODE_PULLUP,                          /*!< GPIO pin mode pullup define */
  PINMODE_PULLDOWN,                        /*!< GPIO pin mode pulldown define */
  PINMODE_NOPULL,                          /*!< GPIO pin mode nopull define */
  PINMODE_TRISTATE,                        /*!< GPIO pin mode tristate define */
}GPIO_PINMODE_Type;

/**
 *  @brief Seiral FLASH mode type definition
 */
typedef enum
{
  MODE_SHUTDOWN = 0,                    /*!< SFLASH mode shutdown define */
  MODE_DEFAULT,                         /*!< SFLASH mode default define */
}SFLASH_MODE_Type;

/*@} end of group PINMUX_Public_Types */

/** @defgroup PINMUX_Public_Constants
 *  @{
 */ 

/** @defgroup GPIO_Pinmux        
 *  @{
 */
/* GPIO0 pinmux function define */
#define GPIO0_GPIO0                            PINMUX_FUNCTION_0
#define GPIO0_GPT0_CH0                         PINMUX_FUNCTION_2
#define GPIO0_ADC0_IN7                         PINMUX_FUNCTION_6
#define GPIO0_ACOMP0_IN7                       PINMUX_FUNCTION_6
#define GPIO0_ACOMP1_IN7                       PINMUX_FUNCTION_6

/* GPIO1 pinmux function define */
#define GPIO1_GPIO1                            PINMUX_FUNCTION_0
#define GPIO1_GPT0_CH1                         PINMUX_FUNCTION_2
#define GPIO1_ADC0_IN6                         PINMUX_FUNCTION_6
#define GPIO1_ACOMP0_IN6                       PINMUX_FUNCTION_6
#define GPIO1_ACOMP1_IN6                       PINMUX_FUNCTION_6

/* GPIO2 pinmux function define */
#define GPIO2_GPIO2                            PINMUX_FUNCTION_0
#define GPIO2_GPT0_CH2                         PINMUX_FUNCTION_2
#define GPIO2_ADC0_IN5                         PINMUX_FUNCTION_6
#define GPIO2_ACOMP0_IN5                       PINMUX_FUNCTION_6
#define GPIO2_ACOMP1_IN5                       PINMUX_FUNCTION_6

/* GPIO3 pinmux function define */
#define GPIO3_GPIO3                            PINMUX_FUNCTION_0
#define GPIO3_GPT0_CH3                         PINMUX_FUNCTION_2
#define GPIO3_ADC0_IN4                         PINMUX_FUNCTION_6
#define GPIO3_ACOMP0_IN4                       PINMUX_FUNCTION_6
#define GPIO3_ACOMP1_IN4                       PINMUX_FUNCTION_6

/* GPIO4 pinmux function define */
#define GPIO4_GPIO4                            PINMUX_FUNCTION_0
#define GPIO4_GPT0_CH4                         PINMUX_FUNCTION_2
#define GPIO4_I2C1_SDA                         PINMUX_FUNCTION_3
#define GPIO4_GPT1_CLKIN                       PINMUX_FUNCTION_4
#define GPIO4_ADC0_IN3                         PINMUX_FUNCTION_6
#define GPIO4_ACOMP0_IN3                       PINMUX_FUNCTION_6
#define GPIO4_ACOMP1_IN3                       PINMUX_FUNCTION_6
#define GPIO4_DACA                             PINMUX_FUNCTION_6
#define GPIO4_DBG_P                            PINMUX_FUNCTION_6
#define GPIO4_ADC0_REF                         PINMUX_FUNCTION_6

/* GPIO5 pinmux function define */
#define GPIO5_GPIO5                            PINMUX_FUNCTION_0
#define GPIO5_GPT0_CH5                         PINMUX_FUNCTION_2
#define GPIO5_I2C1_SCL                         PINMUX_FUNCTION_3
#define GPIO5_GPT3_CLKIN                       PINMUX_FUNCTION_4
#define GPIO5_ADC0_IN2                         PINMUX_FUNCTION_6
#define GPIO5_ACOMP0_IN2                       PINMUX_FUNCTION_6
#define GPIO5_ACOMP1_IN2                       PINMUX_FUNCTION_6

/* GPIO6 pinmux function define */
#define GPIO6_GPIO6                            PINMUX_FUNCTION_0
#define GPIO6_GPT1_CH0                         PINMUX_FUNCTION_2
#define GPIO6_GPT0_CLKIN                       PINMUX_FUNCTION_3
#define GPIO6_GPT3_CH0                         PINMUX_FUNCTION_4
#define GPIO6_ADC0_IN1                         PINMUX_FUNCTION_6
#define GPIO6_ACOMP0_IN1                       PINMUX_FUNCTION_6
#define GPIO6_ACOMP1_IN1                       PINMUX_FUNCTION_6
#define GPIO6_TEMP0                            PINMUX_FUNCTION_6

/* GPIO7 pinmux function define */
#define GPIO7_GPIO7                            PINMUX_FUNCTION_0
#define GPIO7_GPT1_CH1                         PINMUX_FUNCTION_2
#define GPIO7_GPT2_CLKIN                       PINMUX_FUNCTION_3
#define GPIO7_GPT3_CH1                         PINMUX_FUNCTION_4
#define GPIO7_ADC0_IN0                         PINMUX_FUNCTION_6
#define GPIO7_ACOMP0_IN0                       PINMUX_FUNCTION_6
#define GPIO7_ACOMP1_IN0                       PINMUX_FUNCTION_6
#define GPIO7_TEMP0                            PINMUX_FUNCTION_6

/* GPIO8 pinmux function define */ 
#define GPIO8_GPIO8                            PINMUX_FUNCTION_0
#define GPIO8_GPT1_CH2                         PINMUX_FUNCTION_2
#define GPIO8_I2C1_SDA                         PINMUX_FUNCTION_3
#define GPIO8_GPT3_CH2                         PINMUX_FUNCTION_4
#define GPIO8_ADC1_IN0                         PINMUX_FUNCTION_6
#define GPIO8_TEMP1                            PINMUX_FUNCTION_6

/* GPIO9 pinmux function define */ 
#define GPIO9_GPIO9                            PINMUX_FUNCTION_0
#define GPIO9_GPT1_CH3                         PINMUX_FUNCTION_2
#define GPIO9_I2C1_SCL                         PINMUX_FUNCTION_3
#define GPIO9_GPT3_CH3                         PINMUX_FUNCTION_4
#define GPIO9_ADC1_IN1                         PINMUX_FUNCTION_6
#define GPIO9_TEMP1                            PINMUX_FUNCTION_6

/* GPIO10 pinmux function define */ 
#define GPIO10_GPIO10                          PINMUX_FUNCTION_0
#define GPIO10_GPT1_CH4                        PINMUX_FUNCTION_2
#define GPIO10_I2C2_SDA                        PINMUX_FUNCTION_3
#define GPIO10_GPT3_CH4                        PINMUX_FUNCTION_4
#define GPIO10_ADC1_IN2                        PINMUX_FUNCTION_6
#define GPIO10_DAC_REF                         PINMUX_FUNCTION_6

/* GPIO11 pinmux function define */ 
#define GPIO11_GPIO11                          PINMUX_FUNCTION_0
#define GPIO11_GPT1_CH5                        PINMUX_FUNCTION_2
#define GPIO11_I2C2_SCL                        PINMUX_FUNCTION_3
#define GPIO11_GPT3_CH5                        PINMUX_FUNCTION_4
#define GPIO11_ADC1_IN3                        PINMUX_FUNCTION_6
#define GPIO11_DACB                            PINMUX_FUNCTION_6
#define GPIO11_DBG_N                           PINMUX_FUNCTION_6
#define GPIO10_ADC1_REF                        PINMUX_FUNCTION_6

/* GPIO16 pinmux function define */ 
#define GPIO16_GPIO16                          PINMUX_FUNCTION_0
#define GPIO16_GPT2_CH4                        PINMUX_FUNCTION_2
#define GPIO16_GPT3_CH0                        PINMUX_FUNCTION_3
#define GPIO16_GPT0_CH4                        PINMUX_FUNCTION_4

/* GPIO17 pinmux function define */ 
#define GPIO17_GPIO17                          PINMUX_FUNCTION_0
#define GPIO17_GPT1_CH0                        PINMUX_FUNCTION_1
#define GPIO17_GPT2_CH5                        PINMUX_FUNCTION_2
#define GPIO17_GPT3_CH1                        PINMUX_FUNCTION_3
#define GPIO17_GPT0_CH5                        PINMUX_FUNCTION_4

/* GPIO18 pinmux function define */ 
#define GPIO18_GPIO18                          PINMUX_FUNCTION_0
#define GPIO18_GPT3_CH0                        PINMUX_FUNCTION_2
#define GPIO18_I2C0_SDA                        PINMUX_FUNCTION_3
#define GPIO18_UART1_SIR_OUT                   PINMUX_FUNCTION_4
#define GPIO18_OSC32K_IN                       PINMUX_FUNCTION_6

/* GPIO19 pinmux function define */ 
#define GPIO19_GPIO19                          PINMUX_FUNCTION_0
#define GPIO19_GPT3_CH1                        PINMUX_FUNCTION_2
#define GPIO19_I2C0_SCL                        PINMUX_FUNCTION_3
#define GPIO19_UART1_SIR_IN                    PINMUX_FUNCTION_4
#define GPIO19_OSC32K_OUT                      PINMUX_FUNCTION_6

/* GPIO20 pinmux function define */ 
#define GPIO20_TDO                             PINMUX_FUNCTION_0
#define GPIO20_GPIO20                          PINMUX_FUNCTION_1

/* GPIO21 pinmux function define */ 
#define GPIO21_TCK                             PINMUX_FUNCTION_0
#define GPIO21_GPIO21                          PINMUX_FUNCTION_1

/* GPIO22 pinmux function define */ 
#define GPIO22_TMS                             PINMUX_FUNCTION_0
#define GPIO22_GPIO22                          PINMUX_FUNCTION_1

/* GPIO23 pinmux function define */ 
#define GPIO23_TDI                             PINMUX_FUNCTION_0
#define GPIO23_GPIO23                          PINMUX_FUNCTION_1

/* GPIO24 pinmux function define */ 
#define GPIO24_TRST_N                          PINMUX_FUNCTION_0
#define GPIO24_GPIO24                          PINMUX_FUNCTION_1

/* GPIO25 pinmux function define */ 
#define GPIO25_WAKE_UP0                        PINMUX_FUNCTION_0
#define GPIO25_GPIO25                          PINMUX_FUNCTION_1
#define GPIO25_ACOMP0_GPIO_OUT                 PINMUX_FUNCTION_2
#define GPIO25_ACOMP1_GPIO_OUT                 PINMUX_FUNCTION_3
#define GPIO25_UART0_SIR_IN                    PINMUX_FUNCTION_4

/* GPIO26 pinmux function define */ 
#define GPIO26_WAKE_UP1                        PINMUX_FUNCTION_0
#define GPIO26_GPIO26                          PINMUX_FUNCTION_1
#define GPIO26_ACOMP0_EDGE_PULSE               PINMUX_FUNCTION_2
#define GPIO26_ACOMP1_EDGE_PULSE               PINMUX_FUNCTION_3
#define GPIO26_UART0_SIR_OUT                   PINMUX_FUNCTION_4
#define GPIO26_COMP_IN_N                       PINMUX_FUNCTION_6

/* GPIO27 pinmux function define */ 
#define GPIO27_GPIO27                          PINMUX_FUNCTION_0
#define GPIO27_ACOMP0_GPIO_OUT                 PINMUX_FUNCTION_1
#define GPIO27_GPT3_CH2                        PINMUX_FUNCTION_2
#define GPIO27_UART0_DSRn                      PINMUX_FUNCTION_4
#define GPIO27_BOOT                            PINMUX_FUNCTION_5
#define GPIO27_COMP_IN_P                       PINMUX_FUNCTION_6

/* GPIO28 pinmux function define */ 
#define GPIO28_GPIO28                          PINMUX_FUNCTION_0
#define GPIO28_ACOMP0_EDGE_PULSE               PINMUX_FUNCTION_1
#define GPIO28_GPT3_CH3                        PINMUX_FUNCTION_2
#define GPIO28_UART0_DCDn                      PINMUX_FUNCTION_4
#define GPIO28_SDIO_LED                        PINMUX_FUNCTION_5

/* GPIO29 pinmux function define */  
#define GPIO29_GPIO29                          PINMUX_FUNCTION_0
#define GPIO29_ACOMP1_GPIO_OUT                 PINMUX_FUNCTION_1
#define GPIO29_GPT3_CH4                        PINMUX_FUNCTION_2
#define GPIO29_ACOMP0_GPIO_OUT                 PINMUX_FUNCTION_3
#define GPIO29_UART0_Rin                       PINMUX_FUNCTION_4
#define GPIO29_SDIO_CDn                        PINMUX_FUNCTION_5

/* GPIO30 pinmux function define */ 
#define GPIO30_GPIO30                          PINMUX_FUNCTION_0
#define GPIO30_ACOMP1_EDGE_PULSE               PINMUX_FUNCTION_1
#define GPIO30_GPT3_CH5                        PINMUX_FUNCTION_2
#define GPIO30_ACOMP0_EDGE_PULSE               PINMUX_FUNCTION_3
#define GPIO30_UART0_DTRn                      PINMUX_FUNCTION_4
#define GPIO30_SDIO_WP                         PINMUX_FUNCTION_5

/* GPIO32 pinmux function define */ 
#define GPIO32_GPIO32                          PINMUX_FUNCTION_0
#define GPIO32_SSP0_CLK                        PINMUX_FUNCTION_1
#define GPIO32_UART2_CTSn                      PINMUX_FUNCTION_2
#define GPIO32_GPT2_CH0                        PINMUX_FUNCTION_3
#define GPIO32_GPT0_CH0                        PINMUX_FUNCTION_4

/* GPIO33 pinmux function define */ 
#define GPIO33_GPIO33                          PINMUX_FUNCTION_0
#define GPIO33_SSP0_FRM                        PINMUX_FUNCTION_1
#define GPIO33_UART2_RTSn                      PINMUX_FUNCTION_2
#define GPIO33_GPT2_CH1                        PINMUX_FUNCTION_3
#define GPIO33_GPT0_CH1                        PINMUX_FUNCTION_4

/* GPIO34 pinmux function define */ 
#define GPIO34_GPIO34                          PINMUX_FUNCTION_0
#define GPIO34_SSP0_RXD                        PINMUX_FUNCTION_1
#define GPIO34_UART2_TXD                       PINMUX_FUNCTION_2
#define GPIO34_GPT2_CH2                        PINMUX_FUNCTION_3
#define GPIO34_GPT0_CH2                        PINMUX_FUNCTION_4

/* GPIO35 pinmux function define */ 
#define GPIO35_GPIO35                          PINMUX_FUNCTION_0
#define GPIO35_SSP0_TXD                        PINMUX_FUNCTION_1
#define GPIO35_UART2_RXD                       PINMUX_FUNCTION_2
#define GPIO35_GPT2_CH3                        PINMUX_FUNCTION_3
#define GPIO35_GPT0_CH3                        PINMUX_FUNCTION_4

/* GPIO40 pinmux function define */ 
#define GPIO40_GPIO40                          PINMUX_FUNCTION_0
#define GPIO40_UART3_CTSn                      PINMUX_FUNCTION_1
#define GPIO40_SSP2_CLK                        PINMUX_FUNCTION_2
#define GPIO40_GPT1_CH2                        PINMUX_FUNCTION_3

/* GPIO41 pinmux function define */ 
#define GPIO41_GPIO41                          PINMUX_FUNCTION_0
#define GPIO41_UART3_RTSn                      PINMUX_FUNCTION_1
#define GPIO41_SSP2_FRM                        PINMUX_FUNCTION_2
#define GPIO41_GPT1_CH3                        PINMUX_FUNCTION_3

/* GPIO42 pinmux function define */ 
#define GPIO42_GPIO42                          PINMUX_FUNCTION_0
#define GPIO42_UART3_TXD                       PINMUX_FUNCTION_1
#define GPIO42_SSP2_RXD                        PINMUX_FUNCTION_2
#define GPIO42_GPT1_CH4                        PINMUX_FUNCTION_3

/* GPIO43 pinmux function define */ 
#define GPIO43_GPIO43                          PINMUX_FUNCTION_0
#define GPIO43_UART3_RXD                       PINMUX_FUNCTION_1
#define GPIO43_SSP2_TXD                        PINMUX_FUNCTION_2
#define GPIO43_GPT1_CH5                        PINMUX_FUNCTION_3

/* GPIO44 pinmux function define */ 
#define GPIO44_GPIO44                          PINMUX_FUNCTION_0
#define GPIO44_I2C0_SDA                        PINMUX_FUNCTION_1
#define GPIO44_GPT0_CLKIN                      PINMUX_FUNCTION_2
#define GPIO44_GPT3_CH0                        PINMUX_FUNCTION_3
#define GPIO44_ADC_TRIGGER                     PINMUX_FUNCTION_4
#define GPIO44_DAC_TRIGGER                     PINMUX_FUNCTION_4
#define GPIO44_SDIO_CDn                        PINMUX_FUNCTION_5

/* GPIO45 pinmux function define */ 
#define GPIO45_GPIO45                          PINMUX_FUNCTION_0
#define GPIO45_I2C0_SCL                        PINMUX_FUNCTION_1
#define GPIO45_USB2_DRVVBUS                    PINMUX_FUNCTION_2
#define GPIO45_GPT3_CH1                        PINMUX_FUNCTION_3
#define GPIO45_ADC_TRIGGER                     PINMUX_FUNCTION_4
#define GPIO45_DAC_TRIGGER                     PINMUX_FUNCTION_4
#define GPIO45_SDIO_WP                         PINMUX_FUNCTION_5

/* GPIO50 pinmux function define */ 
#define GPIO50_GPIO50                          PINMUX_FUNCTION_0
#define GPIO50_GPT1_CH5                        PINMUX_FUNCTION_1
#define GPIO50_SDIO_LED                        PINMUX_FUNCTION_2

/* GPIO51 pinmux function define */ 
#define GPIO51_GPIO51                          PINMUX_FUNCTION_0
#define GPIO51_SDIO_CLK                        PINMUX_FUNCTION_1
#define GPIO51_SSP2_CLK                        PINMUX_FUNCTION_2
#define GPIO51_GPT0_CH0                        PINMUX_FUNCTION_3
#define GPIO51_UART2_DSRn                      PINMUX_FUNCTION_4

/* GPIO52 pinmux function define */ 
#define GPIO52_GPIO52                          PINMUX_FUNCTION_0
#define GPIO52_SDIO_3                          PINMUX_FUNCTION_1
#define GPIO52_SSP2_FRM                        PINMUX_FUNCTION_2
#define GPIO52_GPT0_CH1                        PINMUX_FUNCTION_3
#define GPIO52_UART2_DCDn                      PINMUX_FUNCTION_4

/* GPIO53 pinmux function define */ 
#define GPIO53_GPIO53                          PINMUX_FUNCTION_0
#define GPIO53_SDIO_2                          PINMUX_FUNCTION_1
#define GPIO53_SSP2_RXD                        PINMUX_FUNCTION_2
#define GPIO53_GPT0_CH2                        PINMUX_FUNCTION_3
#define GPIO53_UART2_Rin                       PINMUX_FUNCTION_4

/* GPIO54 pinmux function define */ 
#define GPIO54_GPIO54                          PINMUX_FUNCTION_0
#define GPIO54_SDIO_1                          PINMUX_FUNCTION_1
#define GPIO54_SSP2_TXD                        PINMUX_FUNCTION_2
#define GPIO54_GPT0_CH3                        PINMUX_FUNCTION_3
#define GPIO54_UART2_DTRn                      PINMUX_FUNCTION_4

/* GPIO55 pinmux function define */ 
#define GPIO55_GPIO55                          PINMUX_FUNCTION_0
#define GPIO55_SDIO_0                          PINMUX_FUNCTION_1
#define GPIO55_GPT2_CLKIN                      PINMUX_FUNCTION_2
#define GPIO55_GPT0_CH4                        PINMUX_FUNCTION_3
#define GPIO55_UART2_SIR_OUT                   PINMUX_FUNCTION_4

/* GPIO56 pinmux function define */ 
#define GPIO56_GPIO56                          PINMUX_FUNCTION_0
#define GPIO56_SDIO_CMD                        PINMUX_FUNCTION_1
#define GPIO56_GPT3_CLKIN                      PINMUX_FUNCTION_2
#define GPIO56_GPT0_CH5                        PINMUX_FUNCTION_3
#define GPIO56_UART2_SIR_IN                    PINMUX_FUNCTION_4

/* GPIO57 pinmux function define */ 
#define GPIO57_USB_DP                          PINMUX_FUNCTION_0
#define GPIO57_GPIO57                          PINMUX_FUNCTION_1
#define GPIO57_GPT0_CLKIN                      PINMUX_FUNCTION_2
#define GPIO57_UART3_SIR_OUT                   PINMUX_FUNCTION_4

/* GPIO58 pinmux function define */ 
#define GPIO58_USB_DM                          PINMUX_FUNCTION_0
#define GPIO58_GPIO58                          PINMUX_FUNCTION_1
#define GPIO58_GPT1_CLKIN                      PINMUX_FUNCTION_2
#define GPIO58_UART3_SIR_IN                    PINMUX_FUNCTION_4

/* GPIO59 pinmux function define */ 
#define GPIO59_GPIO59                          PINMUX_FUNCTION_0
#define GPIO59_UART1_CTSn                      PINMUX_FUNCTION_1
#define GPIO59_GPT3_CH2                        PINMUX_FUNCTION_3
#define GPIO59_UART3_DSRn                      PINMUX_FUNCTION_4

/* GPIO60 pinmux function define */ 
#define GPIO60_GPIO60                          PINMUX_FUNCTION_0
#define GPIO60_UART1_RTSn                      PINMUX_FUNCTION_1
#define GPIO60_GPT3_CH3                        PINMUX_FUNCTION_3
#define GPIO60_UART3_DCDn                      PINMUX_FUNCTION_4

/* GPIO61 pinmux function define */ 
#define GPIO61_GPIO61                          PINMUX_FUNCTION_0
#define GPIO61_UART1_TXD                       PINMUX_FUNCTION_1
#define GPIO61_GPT3_CH4                        PINMUX_FUNCTION_3
#define GPIO61_UART3_Rin                       PINMUX_FUNCTION_4

/* GPIO62 pinmux function define */ 
#define GPIO62_GPIO62                          PINMUX_FUNCTION_0
#define GPIO62_UART1_RXD                       PINMUX_FUNCTION_1
#define GPIO62_GPT3_CH5                        PINMUX_FUNCTION_3
#define GPIO62_UART3_DTRn                      PINMUX_FUNCTION_4

/* GPIO63 pinmux function define */ 
#define GPIO63_GPIO63                          PINMUX_FUNCTION_0
#define GPIO63_UART1_CTSn                      PINMUX_FUNCTION_1
#define GPIO63_SSP1_CLK                        PINMUX_FUNCTION_2
#define GPIO63_GPT3_CH2                        PINMUX_FUNCTION_3
#define GPIO63_UART1_DSRn                      PINMUX_FUNCTION_4

/* GPIO64 pinmux function define */ 
#define GPIO64_GPIO64                          PINMUX_FUNCTION_0
#define GPIO64_UART1_RTSn                      PINMUX_FUNCTION_1
#define GPIO64_SSP1_FRM                        PINMUX_FUNCTION_2
#define GPIO64_GPT3_CH3                        PINMUX_FUNCTION_3
#define GPIO64_UART1_DCDn                      PINMUX_FUNCTION_4

/* GPIO65 pinmux function define */ 
#define GPIO65_GPIO65                          PINMUX_FUNCTION_0
#define GPIO65_UART1_TXD                       PINMUX_FUNCTION_1
#define GPIO65_SSP1_RXD                        PINMUX_FUNCTION_2
#define GPIO65_GPT3_CH4                        PINMUX_FUNCTION_3
#define GPIO65_UART1_Rin                       PINMUX_FUNCTION_4

/* GPIO66 pinmux function define */ 
#define GPIO66_GPIO66                          PINMUX_FUNCTION_0
#define GPIO66_UART1_RXD                       PINMUX_FUNCTION_1
#define GPIO66_SSP1_TXD                        PINMUX_FUNCTION_2
#define GPIO66_GPT3_CH5                        PINMUX_FUNCTION_3
#define GPIO66_UART1_DTRn                      PINMUX_FUNCTION_4

/* GPIO68 pinmux function define */ 
#define GPIO68_GPIO68                          PINMUX_FUNCTION_0
#define GPIO68_GPT2_CH2                        PINMUX_FUNCTION_1
#define GPIO68_GPT1_CLKIN                      PINMUX_FUNCTION_2

/* GPIO72 pinmux function define */ 
#define GPIO72_GPIO72                          PINMUX_FUNCTION_0
#define GPIO72_UART0_CTSn                      PINMUX_FUNCTION_1
#define GPIO72_GPT2_CLKIN                      PINMUX_FUNCTION_2
#define GPIO72_GPT1_CH2                        PINMUX_FUNCTION_3
#define GPIO72_QSPI1_SSn                       PINMUX_FUNCTION_4

/* GPIO73 pinmux function define */ 
#define GPIO73_GPIO73                          PINMUX_FUNCTION_0
#define GPIO73_UART0_RTSn                      PINMUX_FUNCTION_1
#define GPIO73_GPT3_CLKIN                      PINMUX_FUNCTION_2
#define GPIO73_GPT1_CH3                        PINMUX_FUNCTION_3
#define GPIO73_QSPI1_CLK                       PINMUX_FUNCTION_4

/* GPIO74 pinmux function define */ 
#define GPIO74_GPIO74                          PINMUX_FUNCTION_0
#define GPIO74_UART0_TXD                       PINMUX_FUNCTION_1
#define GPIO74_RC32M_CLKOUT                    PINMUX_FUNCTION_2
#define GPIO74_GPT1_CH4                        PINMUX_FUNCTION_3

/* GPIO75 pinmux function define */ 
#define GPIO75_GPIO75                          PINMUX_FUNCTION_0
#define GPIO75_UART0_RXD                       PINMUX_FUNCTION_1
#define GPIO75_GPT1_CH5                        PINMUX_FUNCTION_3

/* GPIO76 pinmux function define */ 
#define GPIO76_GPIO76                          PINMUX_FUNCTION_0
#define GPIO76_UART2_CTSn                      PINMUX_FUNCTION_1
#define GPIO76_SSP0_CLK                        PINMUX_FUNCTION_2
#define GPIO76_I2C0_SDA                        PINMUX_FUNCTION_3
#define GPIO76_QSPI1_D0                        PINMUX_FUNCTION_4

/* GPIO77 pinmux function define */ 
#define GPIO77_GPIO77                          PINMUX_FUNCTION_0
#define GPIO77_UART2_RTSn                      PINMUX_FUNCTION_1
#define GPIO77_SSP0_FRM                        PINMUX_FUNCTION_2
#define GPIO77_I2C0_SCL                        PINMUX_FUNCTION_3
#define GPIO77_QSPI1_D1                        PINMUX_FUNCTION_4

/* GPIO78 pinmux function define */ 
#define GPIO78_GPIO78                          PINMUX_FUNCTION_0
#define GPIO78_UART2_TXD                       PINMUX_FUNCTION_1
#define GPIO78_SSP0_RXD                        PINMUX_FUNCTION_2
#define GPIO78_GPT1_CH0                        PINMUX_FUNCTION_3
#define GPIO78_QSPI1_D2                        PINMUX_FUNCTION_4

/* GPIO79 pinmux function define */ 
#define GPIO79_GPIO79                          PINMUX_FUNCTION_0
#define GPIO79_UART2_RXD                       PINMUX_FUNCTION_1
#define GPIO79_SSP0_TXD                        PINMUX_FUNCTION_2
#define GPIO79_GPT1_CH1                        PINMUX_FUNCTION_3
#define GPIO79_QSPI1_D3                        PINMUX_FUNCTION_4

/*@} end of group GPIO_Pinmux */

/** @defgroup GPIO_PINMUX     
 *  @{
 */
#define IS_GPIO_PINMUXFUN(PINMUXFUN)           ((PINMUXFUN) <= PINMUX_FUNCTION_7)

/*@} end of group GPIO_PINMUX */

/** @defgroup GPIO_PIN_MODE     
 *  @{
 */
#define IS_GPIO_PINMODE(PINMODE)               (((PINMODE) == PINMODE_DEFAULT) || \
                                                ((PINMODE) == PINMODE_PULLUP) || \
                                                ((PINMODE) == PINMODE_PULLDOWN) || \
                                                ((PINMODE) == PINMODE_NOPULL) || \
                                                ((PINMODE) == PINMODE_TRISTATE))

/*@} end of group GPIO_PIN_MODE */

/** @defgroup SFLASH_MODE
 *  @{
 */
#define IS_SFLASH_MODE(MODE)                   (((MODE) == MODE_SHUTDOWN) || \
                                                ((MODE) == MODE_DEFAULT))

/*@} end of group SFLASH_MODE */

/*@} end of group PINMUX_Public_Constants */

/** @defgroup PINMUX_Public_Macro
 *  @{
 */
#define PACKAGE_88_PIN

/*@} end of group PINMUX_Public_Macro */

/** @defgroup PINMUX_Public_FunctionDeclaration
 *  @brief GPIO pinmux functions declaration
 *  @{
 */
void GPIO_PinMuxFun(GPIO_NO_Type gpioNo, GPIO_PinMuxFunc_Type pinMuxFun);
void GPIO_PinModeConfig(GPIO_NO_Type gpioNo, GPIO_PINMODE_Type gpioPinMode);
void SFLASH_HOLDnConfig(SFLASH_MODE_Type sflashMode);
void SFLASH_DIOConfig(SFLASH_MODE_Type sflashMode);
void SFLASH_WPConfig(SFLASH_MODE_Type sflashMode);
void SFLASH_DOConfig(SFLASH_MODE_Type sflashMode);

/*@} end of group PINMUX_Public_FunctionDeclaration */

/*@} end of group PINMUX */

/*@} end of group MC200_Periph_Driver */ 

#endif /* __MC200_PINMUX_H__ */
