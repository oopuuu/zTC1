/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__


#include "hal_define.h"
#include "hal_feature_config.h"
#include "mt7687.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
* module sub features define
*****************************************************************************/
#ifdef HAL_ADC_MODULE_ENABLED
#define HAL_ADC_FEATURE_NO_NEED_PINMUX             /*please not to modify*/
#endif

#ifdef HAL_PWM_MODULE_ENABLED
#define HAL_PWM_FEATURE_SINGLE_SOURCE_CLOCK        /*please not to modify*/
#endif


#ifdef HAL_RTC_MODULE_ENABLED
#define HAL_RTC_FEATURE_SLEEP        /*please not to modify*/
#endif

#ifdef HAL_UART_MODULE_ENABLED
#define HAL_UART_FEATURE_VFIFO_DMA_TIMEOUT        /*please not to modify*/
#endif

#ifdef HAL_I2S_MODULE_ENABLED
#define HAL_I2S_EXTENDED                        /*please not to modify*/
#define HAL_I2S_SUPPORT_VFIFO                   /*please not to modify*/
#endif

#ifdef HAL_GDMA_MODULE_ENABLED
#define HAL_GDMA_WRAP_FLASH_ADDRESS_TO_VIRTUAL_ADDRESS       /*please not to modify*/
#endif


#ifdef HAL_UART_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup UART
 * @{
 * @addtogroup hal_uart_enum Enum
 * @{
 */
/*****************************************************************************
* uart
*****************************************************************************/
/** @brief UART port index
 * We have total 2 UART ports. And UART0 and UART1 support hardware flow control.
 * | UART port | hardware flow control |
 * |-----------|-----------------------|
 * |  UART0    |           V           |
 * |  UART1    |           V           |
 */
typedef enum {
    HAL_UART_0 = 0,                            /**< uart port0 */
    HAL_UART_1 = 1,                            /**< uart port1 */
    HAL_UART_MAX
} hal_uart_port_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_I2C_MASTER_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup I2C_MASTER
 * @{
 * @defgroup hal_i2c_master_define Define
 * @{
  */

/** @brief  The maximum polling mode transaction size.
  */
#define HAL_I2C_MAXIMUM_POLLING_TRANSACTION_SIZE  8

/** @brief  The maximum DMA mode transaction size.
  */
#define HAL_I2C_MAXIMUM_DMA_TRANSACTION_SIZE   65535


/**
  * @}
  */

/** @addtogroup hal_i2c_master_enum Enum
  * @{
  */

/*****************************************************************************
* i2c master
*****************************************************************************/
/** @brief This enum define the i2c port.
 *  We have total 2 I2C masters. Both of them support polling mode and DMA mode.
 *
 *
*/
typedef enum {
    HAL_I2C_MASTER_0 = 0,           /**< i2c master 0. */
    HAL_I2C_MASTER_1 = 1,           /**< i2c master 1. */
    HAL_I2C_MASTER_MAX              /**< max i2c master number, <invalid> */
} hal_i2c_port_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_GPIO_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup GPIO
* @{
*
* @addtogroup hal_gpio_enum Enum
* @{
*/

/*****************************************************************************
* gpio
*****************************************************************************/
/** @brief This enum define the gpio port.
 *
*/

typedef enum {
    HAL_GPIO_0 = 0,                       /**< GPIO pin0 */
    HAL_GPIO_1 = 1,                       /**< GPIO pin1 */
    HAL_GPIO_2 = 2,                       /**< GPIO pin2 */
    HAL_GPIO_3 = 3,                       /**< GPIO pin3 */
    HAL_GPIO_4 = 4,                       /**< GPIO pin4 */
    HAL_GPIO_5 = 5,                       /**< GPIO pin5 */
    HAL_GPIO_6 = 6,                       /**< GPIO pin6 */
    HAL_GPIO_7 = 7,                       /**< GPIO pin7 */

    HAL_GPIO_24 = 24,                     /**< GPIO pin24 */
    HAL_GPIO_25 = 25,                     /**< GPIO pin25 */
    HAL_GPIO_26 = 26,                     /**< GPIO pin26 */
    HAL_GPIO_27 = 27,                     /**< GPIO pin27 */
    HAL_GPIO_28 = 28,                     /**< GPIO pin28 */
    HAL_GPIO_29 = 29,                     /**< GPIO pin29 */
    HAL_GPIO_30 = 30,                     /**< GPIO pin30 */
    HAL_GPIO_31 = 31,                     /**< GPIO pin31 */
    HAL_GPIO_32 = 32,                     /**< GPIO pin32 */
    HAL_GPIO_33 = 33,                     /**< GPIO pin33 */
    HAL_GPIO_34 = 34,                     /**< GPIO pin34 */
    HAL_GPIO_35 = 35,                     /**< GPIO pin35 */
    HAL_GPIO_36 = 36,                     /**< GPIO pin36 */
    HAL_GPIO_37 = 37,                     /**< GPIO pin37 */
    HAL_GPIO_38 = 38,                     /**< GPIO pin38 */
    HAL_GPIO_39 = 39,                     /**< GPIO pin39 */

    HAL_GPIO_57 = 57,                     /**< GPIO pin57 */
    HAL_GPIO_58 = 58,                     /**< GPIO pin58 */
    HAL_GPIO_59 = 59,                     /**< GPIO pin59 */
    HAL_GPIO_60 = 60,                     /**< GPIO pin60 */
    HAL_GPIO_MAX                          /**< GPIO pin max number(invalid) */
} hal_gpio_pin_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_ADC_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup ADC
* @{
*
* @addtogroup hal_adc_enum Enum
* @{
*/

/*****************************************************************************
* adc
*****************************************************************************/
/** @brief adc channel */
typedef enum {
    HAL_ADC_CHANNEL_0 = 0,                   /**< ADC channel 0 */
    HAL_ADC_CHANNEL_1 = 1,                   /**< ADC channel 1 */
    HAL_ADC_CHANNEL_2 = 2,                   /**< ADC channel 2 */
    HAL_ADC_CHANNEL_3 = 3,                   /**< ADC channel 3 */
    HAL_ADC_CHANNEL_MAX                      /**< ADC max channel(invalid) */
} hal_adc_channel_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif



#ifdef HAL_I2S_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup I2S
* @{
*
* @addtogroup hal_i2s_enum Enum
* @{
*/

/*****************************************************************************
* i2s
*****************************************************************************/
/** @brief This enum defines initial type of I2S.
 */

typedef enum {
    HAL_I2S_TYPE_EXTERNAL_MODE          = 0,        /**< i2s external mode */
    HAL_I2S_TYPE_EXTERNAL_TDM_MODE      = 1,        /**< i2s external tdm mode(invalid) */
    HAL_I2S_TYPE_INTERNAL_MODE          = 2,	    /**< i2s internal mode(invalid) */
    HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE = 3         /**< i2s internal loopback mode */
} hal_i2s_initial_type_t;

/** @brief I2S event */
typedef enum {
    HAL_I2S_EVENT_DATA_REQUEST        =  0, /**<  This value means request user to fill data. */
    HAL_I2S_EVENT_DATA_NOTIFICATION   =  1  /**<  This value means notify user the RX data is ready. */
} hal_i2s_event_t;

/** @brief I2S sample rate definition */
typedef enum {
    HAL_I2S_SAMPLE_RATE_8K        = 0,  /**<  8000Hz  */
    HAL_I2S_SAMPLE_RATE_12K       = 1,  /**<  12000Hz */
    HAL_I2S_SAMPLE_RATE_16K       = 2,  /**<  16000Hz */
    HAL_I2S_SAMPLE_RATE_24K       = 3,  /**<  24000Hz */
    HAL_I2S_SAMPLE_RATE_32K       = 4,  /**<  32000Hz */
    HAL_I2S_SAMPLE_RATE_48K       = 5,  /**<  48000Hz */
} hal_i2s_sample_rate_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif


#ifdef HAL_SPI_MASTER_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_MASTER
 * @{
 * @defgroup hal_spi_master_define Define
 * @{
 */

/** @brief  The maximum polling mode transaction size.
 */
#define HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE  16

/**
 * @}
 */

/**
 * @addtogroup hal_spi_master_enum Enum
 * @{
 */

/*****************************************************************************
* spi master
*****************************************************************************/
/** @brief This enum defines the SPI master port.
 *  This chip total has 1 SPI master port
 */
typedef enum {
    HAL_SPI_MASTER_0 = 0,                             /**< spi master port 0 */
    HAL_SPI_MASTER_MAX                                /**< spi master max port number(invalid) */
} hal_spi_master_port_t;


/** @brief selection of spi slave device connected to which cs pin of spi master */
typedef enum {
    HAL_SPI_MASTER_SLAVE_0 = 0,                       /**< spi slave device connect to spi master cs0 pin */
    HAL_SPI_MASTER_SLAVE_1 = 1,                       /**< spi slave device connect to spi master cs1 pin */
    HAL_SPI_MASTER_SLAVE_MAX                          /**< spi master max cs pin number(invalid) */
} hal_spi_master_slave_port_t;

/**
 * @}
 */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_SLAVE
 * @{
 * @addtogroup hal_spi_slave_enum Enum
 * @{
 */

/*****************************************************************************
* spi slave
*****************************************************************************/
/** @brief This enum defines the SPI slave port. This chip just support one
 *  SPI slave port.
 */
typedef enum {
    HAL_SPI_SLAVE_0 = 0,                             /**< spi slave port 0 */
    HAL_SPI_SLAVE_MAX                                /**< spi slave max port number(invalid) */
} hal_spi_slave_port_t;

/**
 * @}
 */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_RTC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup RTC
 * @{
 */

/*****************************************************************************
* rtc
*****************************************************************************/
/* NULL */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_EINT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 * @addtogroup hal_eint_enum Enum
 * @{
 */

/*****************************************************************************
* eint
*****************************************************************************/
/** @brief eint pin number */
typedef enum {
    HAL_EINT_NUMBER_0 = 0,
    HAL_EINT_NUMBER_1 = 1,
    HAL_EINT_NUMBER_2 = 2,
    HAL_EINT_NUMBER_3 = 3,
    HAL_EINT_NUMBER_4 = 4,
    HAL_EINT_NUMBER_5 = 5,
    HAL_EINT_NUMBER_6 = 6,
    HAL_EINT_NUMBER_19 = 19,
    HAL_EINT_NUMBER_20 = 20,
    HAL_EINT_NUMBER_21 = 21,
    HAL_EINT_NUMBER_22 = 22,
    HAL_EINT_NUMBER_MAX
} hal_eint_number_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif

#ifdef HAL_GPT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * @addtogroup hal_gpt_enum Enum
 * @{
 */

/*****************************************************************************
* gpt
*****************************************************************************/
/** @brief GPT port */
typedef enum {
    HAL_GPT_0 = 0,                         /**< GPT port0 */
    HAL_GPT_1 = 1,                         /**< GPT port1 */
    HAL_GPT_2 = 2,                         /**< GPT port2, free run counter */
    HAL_GPT_MAX                            /**< GPT max port <invalid> */
} hal_gpt_port_t;

/** @brief GPT clock source */
typedef enum {
    HAL_GPT_CLOCK_SOURCE_32K = 0           /**< Set the GPT clock source to 32Khz*/
} hal_gpt_clock_source_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif


#ifdef HAL_GDMA_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GDMA
 * @{
 * @addtogroup hal_gdma_enum Enum
 * @{
 */
/*****************************************************************************
* gdma
*****************************************************************************/
/** @brief gdma channel */
typedef enum {
    HAL_GDMA_CHANNEL_0 = 0,                        /**< gdma channel 0 */
    HAL_GDMA_CHANNEL_1 = 1,                        /**< gdma channel 1 */
    HAL_GDMA_CHANNEL_MAX                           /**< gdma max channel (invalid) */
} hal_gdma_channel_t;

/**
  * @}
  */

#ifdef  HAL_GDMA_WRAP_FLASH_ADDRESS_TO_VIRTUAL_ADDRESS
/** @brief  the gdma access flash address should be 0x3000000 instead of 0x1000000.
 */
#define HAL_GDMA_WRAP_FLASH_ADDRESS_OFFSET          (28)

#define HAL_GDMA_WRAP_FLASH_ADDRESS_HIGH_BYTE_MASK  (0xfUL<<HAL_GDMA_WRAP_FLASH_ADDRESS_OFFSET)

#define HAL_GDMA_WRAP_FLASH_ADDRESS_MASK            (0x1UL<<HAL_GDMA_WRAP_FLASH_ADDRESS_OFFSET)

#define HAL_GDMA_WRAP_FLASH_VIRTUAL_ADDRESS_MASK    (0x3UL<<HAL_GDMA_WRAP_FLASH_ADDRESS_OFFSET)

/**
 * @}
 */
#endif

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_PWM_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup PWM
 * @{
 * @addtogroup hal_pwm_enum Enum
 * @{
 */
/*****************************************************************************
* pwm
*****************************************************************************/
/** @brief pwm channel */
typedef enum {
    HAL_PWM_0  = 0,                        /**< pwm channe0 */
    HAL_PWM_1  = 1,                        /**< pwm channe1 */
    HAL_PWM_2  = 2,                        /**< pwm channe2 */
    HAL_PWM_3  = 3,                        /**< pwm channe3 */
    HAL_PWM_4  = 4,                        /**< pwm channe4 */
    HAL_PWM_5  = 5,                        /**< pwm channe5 */
    HAL_PWM_18 = 18,                       /**< pwm channe18 */
    HAL_PWM_19 = 19,                       /**< pwm channe19 */
    HAL_PWM_20 = 20,                       /**< pwm channe20 */
    HAL_PWM_21 = 21,                       /**< pwm channe21 */
    HAL_PWM_22 = 22,                       /**< pwm channe22 */
    HAL_PWM_23 = 23,                       /**< pwm channe23 */
    HAL_PWM_24 = 24,                       /**< pwm channe24 */
    HAL_PWM_25 = 25,                       /**< pwm channe25 */
    HAL_PWM_26 = 26,                       /**< pwm channe26 */
    HAL_PWM_27 = 27,                       /**< pwm channe27 */
    HAL_PWM_28 = 28,                       /**< pwm channe28 */
    HAL_PWM_29 = 29,                       /**< pwm channe29 */
    HAL_PWM_30 = 30,                       /**< pwm channe30 */
    HAL_PWM_31 = 31,                       /**< pwm channe31 */
    HAL_PWM_32 = 32,                       /**< pwm channe32 */
    HAL_PWM_33 = 33,                       /**< pwm channe33 */
    HAL_PWM_34 = 34,                       /**< pwm channe34 */
    HAL_PWM_35 = 35,                       /**< pwm channe35 */
    HAL_PWM_36 = 36,                       /**< pwm channe36 */
    HAL_PWM_37 = 37,                       /**< pwm channe37 */
    HAL_PWM_38 = 38,                       /**< pwm channe38 */
    HAL_PWM_39 = 39,                       /**< pwm channe39 */
    HAL_PWM_MAX                            /**< pwm max channel <invalid>*/
} hal_pwm_channel_t;

/** @brief pwm clock source seletion */
typedef enum {
    HAL_PWM_CLOCK_32KHZ  = 0,               /**< pwm clock source: Embedded 32KHz clock */
    HAL_PWM_CLOCK_2MHZ   = 1,               /**< pwm clock srouce: Embedded 2MHz  clock */
    HAL_PWM_CLOCK_20MHZ  = 2,               /**< pwm clock srouce: External 20MHz clock */
    HAL_PWM_CLOCK_26MHZ  = 3,               /**< pwm clock srouce: External 26MHz clock */
    HAL_PWM_CLOCK_40MHZ  = 4,               /**< pwm clock srouce: External 40MHz clock */
    HAL_PWM_CLOCK_52MHZ  = 5,               /**< pwm clock srouce: External 52MHz clock */
    HAL_PWM_CLOCK_MAX                       /**< pwm max clock <invalid>*/
} hal_pwm_source_clock_t ;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif



#ifdef HAL_GPC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPC
 * @{
 * @addtogroup hal_gpc_enum Enum
 * @{
 */
/** @brief gpc port */
typedef enum {
    HAL_GPC_0 = 0,                          /**< gpc port0 */
    HAL_GPC_MAX_PORT                        /**< gpc max port */
} hal_gpc_port_t;


/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif



#ifdef HAL_SLEEP_MANAGER_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SLEEP_MANAGER
 * @{
 * @addtogroup hal_sleep_manager_enum Enum
 * @{
 */
/*****************************************************************************
 * Enum
 *****************************************************************************/
/** @brief Sleep modes */
typedef enum {
    HAL_SLEEP_MODE_NONE = 0,        /**< no sleep */
    HAL_SLEEP_MODE_IDLE,            /**< idle state */
    HAL_SLEEP_MODE_SLEEP,           /**< sleep state */
    HAL_SLEEP_MODE_NUMBER           /**< for support range detection */
} hal_sleep_mode_t;

/** @brief sleep_manager wake up source */
typedef enum {
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_GPT = 0,                     /**< General purpose timer*/
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_EINT,                        /**< External interrupt*/
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_RESERVED,                    /**< Reserved*/
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_KP,                          /**< Keypad*/
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MSDC1,                       /**< SD/eMMC'IP */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MDIF,                        /**< Modem interface*/
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_RESERVED_2,                  /**< Reserved*/
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MSDC2,                       /**< SD/eMMC's second IP */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_SPI_SLAVE,                   /**< SPI protocol slave*/
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_RGU,                         /**< Reset Generation Unit*/
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MM_DBI,                      /**< Multimedia display bus interface*/
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MM_DSI,                      /**< Multimedia display serial interface*/
} hal_sleep_manager_wakeup_source_t;
/**
 * @}
 * @}
 * @}
 */
#endif


#ifdef __cplusplus
}
#endif

#endif /* __HAL_PLATFORM_H__ */

