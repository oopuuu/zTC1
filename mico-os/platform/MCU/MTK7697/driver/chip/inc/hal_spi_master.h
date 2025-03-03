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

#ifndef __HAL_SPI_MASTER_H__
#define __HAL_SPI_MASTER_H__
#include "hal_platform.h"

#ifdef HAL_SPI_MASTER_MODULE_ENABLED


/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_MASTER
 * @{
 * This section introduces the SPI_Master APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, enums, structures and functions.
 *
 * @section HAL_SPI_MASTER_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SPI                        | Serial Peripheral Interface. The Serial Peripheral Interface bus is a synchronous serial communication interface specification used for short distance communication. For more information, please check <a href="https://en.wikipedia.org/wiki/Serial_peripheral_interface"> Serial Peripheral Interface Bus in Wikipedia</a>.|
 * |\b NVIC                       | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M series processors. For more details, please refer to <a href="http://infocenter.arm.com/help/topic/com.arm.doc.100166_0001_00_en/arm_cortexm4_processor_trm_100166_0001_00_en.pdf"> ARM Cortex-M4 technical reference manual</a>.|
 * |\b FIFO                       | First In, First Out. FIFO is a method for organizing and manipulating a data buffer, where the first entry, or 'head' of the queue, is processed first.|
 * |\b DMA                        | Direct Memory Access. DMA is a feature of computer systems that allows certain hardware subsystems to access main system memory independent from the central processing unit (CPU).|
 * |\b GPIO                       | General Purpose Inputs-Outputs. For more details, please refer to @ref GPIO.|
 *
 * @section HAL_SPI_MASTER_Features_Chapter Supported features
 *
 * This controller supports a wide range of SPI interface devices, such as full-duplex transaction ability to
 * communicate with both half-duplex and full-duplex devices. For half-duplex devices, the data flow direction is not relevant for the software.
 * Hardware provides access for various timing adjustments.
 *
 * @}
 * @}
 */
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* - \b Support \b transaction \b format. \n
*  - Half-duplex transaction:
*    There is only one valid transaction at a time on a single direction; either send or receive. The functions using half-duplex transaction are:
*   - #hal_spi_master_send_polling().
*   - #hal_spi_master_send_dma().
*
*  - Full-duplex transaction:
*    There are two valid mutually inclusive transactions; send and receive. Functions using full-duplex transaction are:
*   - #hal_spi_master_send_and_receive_polling().
*   - #hal_spi_master_send_and_receive_dma().
* @}
* @}
*/
#else
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* - \b Support \b transaction \b format. \n
*  - Half-duplex transaction:
*    There is only one valid transaction at a time on a single direction; either send or receive. The function using half-duplex transaction is:
*   - #hal_spi_master_send_polling().
*
*  - Full-duplex transaction:
*    There are two valid mutually inclusive transactions; send and receive. The function using full-duplex transaction is:
*   - #hal_spi_master_send_and_receive_polling().
*
* @}
* @}
*/
#endif
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* - \b Support \b transaction \b mode. \n
*  - Polling mode transaction:
*    In polling mode, #hal_spi_master_send_polling() and #hal_spi_master_send_and_receive_polling() return a value once the transaction is complete.
*    Then the user checks the output of the return value for the transaction result and error handling if any.
*
*    See @ref HAL_SPI_MASTER_Architecture_Chapter for the software architecture of polling mode.
*
* @}
* @}
*/
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
*  - DMA mode transaction:
*    In DMA mode, #hal_spi_master_send_dma() and #hal_spi_master_send_and_receive_dma() return a value once the SPI hardware register is assigned and the DMA configured.
*    During this process the transaction is usually incomplete, once it completes, an interrupt is triggered and a related user callback
*    is called in the SPI interrupt service routine.
*
*    See @ref HAL_SPI_MASTER_Architecture_Chapter for the software architecture of DMA mode.
* @}
* @}
*/
#endif
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* @section HAL_SPI_MASTER_Architecture_Chapter Software architecture of the SPI
* - \b Polling \b mode \b architecture. \n
*   Polling mode architecture is similar to the polling mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for polling mode architecture.
* @}
* @}
*/
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* - \b DMA \b mode \b architecture. \n
*   DMA mode architecture is similar to the interrupt mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for interrupt mode architecture.
* @}
* @}
*/
#endif
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* @section HAL_SPI_MASTER_Driver_Usage_Chapter How to use this driver
*
* - \b Using \b SPI \b master \b polling \b mode. \n
*  Call #hal_pinmux_set_function() to pinmux GPIO pins to four SPI pins (CS_N, SCK, MOSI, and MISO) based on the user's hardware platform design.
*  Once a transaction is complete, call #hal_spi_master_deinit() function to release the SPI master resource to make it
*  available for other users. The steps are shown below:
*  - Step1: Call #hal_pinmux_set_function() to set GPIO pinmux, if EPT tool hasn't been used to configure the related pinmux.
*           For more details about #hal_pinmux_set_function(), please refer to @ref GPIO.
*  - Step2: Call #hal_spi_master_init() to initialize one SPI master. If the SPI master is already initialized by another user, user will get #HAL_SPI_MASTER_STATUS_ERROR_BUSY.
*  - Step3: Call #hal_spi_master_set_macro_selection() to configure SPI pad macro. If the GPIO selected in Step 1
*           doesn't belong to SPI pad macro group A.
*  - Step4: Call #hal_spi_master_set_chip_select_timing() to configure chip select signal timing if the default SPI master
*           chip select signal timing doesn't match SPI device's requirement.
*  - Step5: Call #hal_spi_master_set_deassert() if the SPI device similar to <a href="http://www.thaieasyelec.com/downloads/EFDV423/PAH8001EI-2G.pdf">
*           PAH8001EI-2G </a>, requires switching the chip select signal from invalid to valid after each byte transfer.
*  - Step6: Call #hal_spi_master_set_advanced_config() to provide custom settings for the SPI device operation.
*           for more information about the parameters, please refer to #hal_spi_master_advanced_config_t.
*  - Step7: Call #hal_spi_master_send_polling() to send data in the polling mode.
*  - Step8: Call #hal_spi_master_send_and_receive_polling() to send and receive data in the polling mode.
*  - Step9: Call #hal_spi_master_deinit() to deinitialize the SPI master, if it's no longer in use.
*  - sample code:
*    @code
*       hal_pinmux_set_function(gpio_pin, function_index); //need not to configure pinmux if EPT tool is used.
*       ret = hal_spi_master_init(master_port,&spi_config); //init SPI master.
*       if (HAL_SPI_MASTER_STATUS_OK == ret) {
*           //hal_spi_master_set_macro_selection(master_port,macro_select); //configure SPI pad macro if needed
*           //hal_spi_master_set_chip_select_timing(master_port,chip_select_timing); //configure chip select signal timing if needed
*           //hal_spi_master_set_deassert(master_port,deassert); //configure deassert feature if SPI device needed
*           //hal_spi_master_set_advanced_config(master_port,&advanced_config); //do some advanced configuration if needed
*           ret = hal_spi_master_send_polling(master_port,data,size); //send data
*           if (HAL_SPI_MASTER_STATUS_OK != ret) {
*               //error handle;
*           }
*           ret = hal_spi_master_send_and_receive_polling(master_port,&spi_send_and_receive_config); //send and receive data at the same time
*           if (HAL_SPI_MASTER_STATUS_OK != ret) {
*               //error handle;
*           }
*           hal_spi_master_deinit(master_port); //Must call deinit, if the SPI master is no longer in use.
*       } else {
*           //error handle
*       }
*    @endcode
*
* - \b Using \b SPI \b master \b DMA \b mode. \n
*  Call #hal_pinmux_set_function() to pinmux GPIO pins to four SPI pins (CS_N, SCK, MOSI, and MISO) based on the user's hardware platform design.
*  Then call #hal_spi_master_register_callback() to register a callback function. Once a transaction is complete, call
*  #hal_spi_master_deinit() function \b in \b your \b callback \b function to release the SPI master resource to make it available for other users.
*  The steps are shown below:
*  - Step1: Call #hal_pinmux_set_function() to set GPIO pinmux, if EPT tool hasn't been used to configure the related pinmux.
*           For more details about #hal_pinmux_set_function(), please refer to @ref GPIO.
*  - Step2: Call #hal_spi_master_init() to init one SPI master. If the SPI master is already initialized by another user, user will get #HAL_SPI_MASTER_STATUS_ERROR_BUSY.
*  - Step3: Call #hal_spi_master_register_callback() to register a user callback.
*  - Step4: Call #hal_spi_master_set_macro_selection() to configure SPI pad macro. If the GPIO selected in Step 1
*           doesn't belong to SPI pad macro group A.
*  - Step5: Call #hal_spi_master_set_chip_select_timing() to configure chip select signal timing if the default SPI master
*           chip select signal timing doesn't match SPI device's requirement.
*  - Step6: Call #hal_spi_master_set_deassert() if the SPI device similar to <a href="http://www.thaieasyelec.com/downloads/EFDV423/PAH8001EI-2G.pdf">
*           PAH8001EI-2G </a>, requires switching the chip select signal from invalid to valid after each byte transfer.
*  - Step7: Call #hal_spi_master_set_advanced_config() to provide custom settings for the SPI device operation.
*           for more information about the parameters, please refer to #hal_spi_master_advanced_config_t.
*  - Step8: Call #hal_spi_master_send_dma() to send data in the DMA mode.
*  - Step9: Call #hal_spi_master_send_and_receive_dma() to send and receive data in the DMA mode.
*  - Step10: Call #hal_spi_master_deinit() to deinit the SPI master, if it's no longer in use.
*  - sample code:
*    @code
*       hal_pinmux_set_function(gpio_pin, function_index); //need not to configure pinmux if EPT tool is used.
*       ret = hal_spi_master_init(master_port,&spi_config); // init SPI master.
*       if (HAL_SPI_MASTER_STATUS_OK == ret) {
*           hal_spi_master_register_callback(master_port,user_spi_callback,NULL)// register a user callback.
*           //hal_spi_master_set_macro_selection(master_port,macro_select); //configure SPI pad macro if needed
*           //hal_spi_master_set_chip_select_timing(master_port,chip_select_timing); //configure chip select signal timing if needed
*           //hal_spi_master_set_deassert(master_port,deassert); //configure deassert feature if SPI device needed
*           //hal_spi_master_set_advanced_config(master_port,&advanced_config); //do some advanced configuration items if needed
*           ret = hal_spi_master_send_dma(master_port,data,size); //send data
*           if (HAL_SPI_MASTER_STATUS_OK != ret) {
*               //error handle;
*           }
*           ret = hal_spi_master_send_and_receive_dma(master_port,&spi_send_and_receive_config); //send and receive data at the same time
*           if (HAL_SPI_MASTER_STATUS_OK != ret) {
*               //error handle;
*           }
*       } else {
*           //error handle
*       }
*    @endcode
*    @code
*       // Callback function sample code. This function should be passed to driver while call hal_spi_master_register_callback.
*       void user_spi_callback (hal_spi_master_callback_event_t event,void *user_data)
*       {
*           if (HAL_SPI_MASTER_EVENT_SEND_FINISHED == event) {
*               // send finish event handle;
*               hal_spi_master_deinit(master_port); // Must call deinit when you don't use it.
*           } else if (HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED == event) {
*               // receive finish event handle;
*               hal_spi_master_deinit(master_port); // Must call deinit when you don't use it.
*           }
*       }
*    @endcode
* @}
* @}
*/
#else
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* @section HAL_SPI_MASTER_Driver_Usage_Chapter How to use this driver
*
* - \b Using \b SPI \b master \b polling \b mode. \n
*  Call #hal_pinmux_set_function() to pinmux GPIO pins to four SPI pins (CS_N, SCK, MOSI, and MISO) based on the user's hardware platform design.
*  Once a transaction is complete, call #hal_spi_master_deinit() function to release the SPI master resource to make it
*  available for other users. The steps are shown below:
*  - Step1: Call #hal_pinmux_set_function() to set GPIO pinmux, if EPT tool hasn't been used to configure the related pinmux.
*           For more details about #hal_pinmux_set_function(), please refer to @ref GPIO. Note, user
*           should configure the chip select pin as GPIO mode, this driver will set the chip select signal as valid before starting the transaction,
*           then set the chip select signal as invalid after finishing the transaction.
*  - Step2: Call #hal_spi_master_init() to initialize one SPI master.
*  - Step3: Call #hal_spi_master_send_polling() to send data in the polling mode.
*  - Step4: Call #hal_spi_master_send_and_receive_polling() to send and receive data in the polling mode.
*  - Step5: Call #hal_spi_master_deinit() to deinitialize the SPI master, if it's no longer in use.
*  - sample code:
*    @code
*       hal_pinmux_set_function(gpio_pin, function_index);//chip select pin should be configured as GPIO mode. Need not to configure pinmux if EPT tool is used.
*       ret = hal_spi_master_init(master_port,&spi_config); // init SPI master.
*       if (HAL_SPI_MASTER_STATUS_OK == ret) {
*           ret = hal_spi_master_send_polling(master_port,data,size); //send data
*           if (HAL_SPI_MASTER_STATUS_OK != ret) {
*               //error handle;
*           }
*           ret = hal_spi_master_send_and_receive_polling(master_port,&spi_send_and_receive_config); //send and receive data at the same time
*           if (HAL_SPI_MASTER_STATUS_OK != ret) {
*               //error handle;
*           }
*           hal_spi_master_deinit(master_port); //Must call deinit when don't use SPI master
*       } else {
*           //error handle
*       }
*    @endcode
* @}
* @}
*/
#endif

/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
*/

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_spi_master_enum Enum
  * @{
  */

/** @brief SPI master clock polarity definition */
typedef enum {
    HAL_SPI_MASTER_CLOCK_POLARITY0 = 0,                     /**< Clock polarity is 0 */
    HAL_SPI_MASTER_CLOCK_POLARITY1 = 1                      /**< Clock polarity is 1 */
} hal_spi_master_clock_polarity_t;


/** @brief SPI master clock format definition */
typedef enum {
    HAL_SPI_MASTER_CLOCK_PHASE0 = 0,                         /**< Clock format is 0 */
    HAL_SPI_MASTER_CLOCK_PHASE1 = 1                          /**< Clock format is 1 */
} hal_spi_master_clock_phase_t;


/** @brief SPI master transaction bit order definition */
typedef enum {
    HAL_SPI_MASTER_LSB_FIRST = 0,                       /**< Both send and receive data transfer LSB first */
    HAL_SPI_MASTER_MSB_FIRST = 1                        /**< Both send and receive data transfer MSB first */
} hal_spi_master_bit_order_t;


#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
/** @brief SPI master data transfer byte order definition.
 *         Before calling #hal_spi_master_send_polling() or #hal_spi_master_send_and_receive_polling(), user should make sure the \b byte_order
 *         parameter that defined in #hal_spi_master_advanced_config_t structure and configured through #hal_spi_master_set_advanced_config()
 *         must be #HAL_SPI_MASTER_LITTLE_ENDIAN, because setting as #HAL_SPI_MASTER_BIG_ENDIAN has no effect in send direction, but has effect in receive direction.
 */
typedef enum {
    HAL_SPI_MASTER_LITTLE_ENDIAN = 0,                       /**< Both send and receive data use little endian format */
    HAL_SPI_MASTER_BIG_ENDIAN    = 1                        /**< Both send and receive data use big endian format */
} hal_spi_master_byte_order_t;


/** @brief SPI master chip select polarity definition */
typedef enum {
    HAL_SPI_MASTER_CHIP_SELECT_LOW  = 0,                /**< Chip select polarity active low */
    HAL_SPI_MASTER_CHIP_SELECT_HIGH = 1                 /**< Chip select polarity active high */
} hal_spi_master_chip_select_polarity_t;


/** @brief SPI master tolerance get_tick timing(based on SPI system clock) setting */
typedef enum {
    HAL_SPI_MASTER_NO_GET_TICK_MODE = 0,              /**< Don't use timing tolerance ability. */
    HAL_SPI_MASTER_GET_TICK_DELAY1  = 1,              /**< Delay 1 SPI system clock cycle for get_tick. */
    HAL_SPI_MASTER_GET_TICK_DELAY2  = 2,              /**< Delay 2 SPI system clock cycles for get_tick. */
    HAL_SPI_MASTER_GET_TICK_DELAY3  = 3,              /**< Delay 3 SPI system clock cycles for get_tick. */
    HAL_SPI_MASTER_GET_TICK_DELAY4  = 4,              /**< Delay 4 SPI system clock cycles for get_tick. */
    HAL_SPI_MASTER_GET_TICK_DELAY5  = 5,              /**< Delay 5 SPI system clock cycles for get_tick. */
    HAL_SPI_MASTER_GET_TICK_DELAY6  = 6,              /**< Delay 6 SPI system clock cycles for get_tick. */
    HAL_SPI_MASTER_GET_TICK_DELAY7  = 7               /**< Delay 7 SPI system clock cycles for get_tick. */
} hal_spi_master_get_tick_mode_t;


/** @brief SPI master sample edge of MISO definition */
typedef enum {
    HAL_SPI_MASTER_SAMPLE_POSITIVE = 0,               /**< Sample edge of MISO is positive edge */
    HAL_SPI_MASTER_SAMPLE_NEGATIVE = 1                /**< Sample edge of MISO is negative edge */
} hal_spi_master_sample_select_t;
#endif


#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
/** @brief SPI master chip select deasert definition */
typedef enum {
    HAL_SPI_MASTER_DEASSERT_DISABLE = 0,              /**< Chip select deassert is disabled */
    HAL_SPI_MASTER_DEASSERT_ENABLE  = 1               /**< Chip select deassert is enabled */
} hal_spi_master_deassert_t;
#endif


#ifdef HAL_SPI_MASTER_FEATURE_MACRO_CONFIG
/** @brief SPI master pad macro select definition */
typedef enum {
    HAL_SPI_MASTER_MACRO_GROUP_A = 0,                   /**< SPI use PAD group A */
    HAL_SPI_MASTER_MACRO_GROUP_B = 1,                   /**< SPI use PAD group B */
    HAL_SPI_MASTER_MACRO_GROUP_C = 2                    /**< SPI use PAD group C */
} hal_spi_master_macro_select_t;
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/** @brief SPI master callback event definition*/
typedef enum {
    HAL_SPI_MASTER_EVENT_SEND_FINISHED      = 0,             /**< SPI master send finish */
    HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED   = 1              /**< SPI master receive finish */
} hal_spi_master_callback_event_t;
#endif

/** @brief SPI master status */
typedef enum {
    HAL_SPI_MASTER_STATUS_ERROR             = -4,            /**< SPI master function error */
    HAL_SPI_MASTER_STATUS_ERROR_BUSY        = -3,            /**< SPI master function error busy */
    HAL_SPI_MASTER_STATUS_ERROR_PORT        = -2,            /**< SPI master error port */
    HAL_SPI_MASTER_STATUS_INVALID_PARAMETER = -1,            /**< SPI master error invalid parameter */
    HAL_SPI_MASTER_STATUS_OK                = 0              /**< SPI master function OK*/
} hal_spi_master_status_t;


/** @brief SPI master running status */
typedef enum {
    HAL_SPI_MASTER_BUSY              = 0,             /**< SPI master busy */
    HAL_SPI_MASTER_IDLE              = 1              /**< SPI master not busy, in idle state */
} hal_spi_master_running_status_t;

/**
  * @}
  */

/** @defgroup hal_spi_master_struct Struct
  * @{
  */
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/** @brief SPI master configure structure */
typedef struct {
    uint32_t clock_frequency;                                /**< SPI master clock frequency setting, range from 30000Hz to 13000000Hz. */
    hal_spi_master_slave_port_t slave_port;                  /**< SPI slave device selection. */
    hal_spi_master_bit_order_t bit_order;                    /**< SPI master bit order setting. */
    hal_spi_master_clock_polarity_t polarity;                /**< SPI master clock polarity setting. */
    hal_spi_master_clock_phase_t phase;                      /**< SPI master clock phase setting. */
} hal_spi_master_config_t;
#else
/** @brief SPI master configure structure */
typedef struct {
    uint32_t clock_frequency;                                /**< SPI master clock frequency setting, range from 30000Hz to 60000000Hz. */
    hal_spi_master_slave_port_t slave_port;                  /**< SPI slave device selection. */
    hal_spi_master_bit_order_t bit_order;                    /**< SPI master bit order setting. */
    hal_spi_master_clock_polarity_t polarity;                /**< SPI master clock polarity setting. */
    hal_spi_master_clock_phase_t phase;                      /**< SPI master clock phase setting. */
} hal_spi_master_config_t;
#endif

#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
/** @brief SPI master advanced configure structure */
typedef struct {
    hal_spi_master_byte_order_t byte_order;                     /**< SPI master byte order setting. */
    hal_spi_master_chip_select_polarity_t chip_polarity;        /**< SPI master chip select active polarity setting. */
    hal_spi_master_get_tick_mode_t get_tick;                    /**< SPI clock timing tolerance setting. User may need to change this setting when high frequency is used. */
    hal_spi_master_sample_select_t sample_select;               /**< SPI MISO sample edge setting. */
} hal_spi_master_advanced_config_t;
#endif


#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
/** @brief SPI master chip select configure structure */
typedef struct {
    uint32_t chip_select_setup_count;                 /**< SPI master chip select setup count setting. Unit in count of SPI
                                                           base clock. Range(0~32767). The chip select setup time is
                                                           (cs_setup_count+1)*CLK_PERIOD, where CLK_PERIOD is the cycle time
                                                           of the clock the SPI engine adopts. */
    uint32_t chip_select_hold_count;                  /**< SPI master chip select hold count setting. Unit in count of SPI
                                                           base clock. Range(0~32767). The chip select hold time is
                                                           (cs_hold_count+1)*CLK_PERIOD. */
    uint32_t chip_select_idle_count;                  /**< SPI master chip select idle count setting. Unit in count of SPI
                                                           base clock. Range(0~255). The chip select idle time between consecutive
                                                           transaction is (cs_idle_count+1)*CLK_PERIOD. */
} hal_spi_master_chip_select_timing_t;
#endif


/** @brief SPI master send and receive configure structure */
typedef struct {
    uint8_t *send_data;                               /**< Buffer of data to be sent, this parameter cannot be NULL. */
    uint32_t send_length;                             /**< The number of bytes to send, this shouldn't larger than 4. */
    uint8_t *receive_buffer;                          /**< Buffer where received data are stored, this parameter cannot be NULL. */
    uint32_t receive_length;                          /**< The valid number of bytes received plus the number of bytes to send. */
} hal_spi_master_send_and_receive_config_t;


/**
  * @}
  */
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/** @defgroup hal_spi_master_typedef Typedef
  * @{
  */

/** @brief  This define the callback function prototype.
 *          User should register a callback function when using SPI DMA mode, this function will be called in SPI interrupt
 *          service routine after a transaction completes. More details about the callback architecture, please refer to
 *          @ref HAL_SPI_MASTER_Architecture_Chapter.
 *  @param [in] event is the transaction event for the current transaction, user can get the transaction result from this parameter, please
 *              refer to #hal_spi_master_callback_event_t for details about the event type.
 *  @param [in] user_data is the parameter which is given by user via #hal_spi_master_register_callback().
 *  @sa  #hal_spi_master_register_callback()
 */
typedef void (*hal_spi_master_callback_t)(hal_spi_master_callback_event_t event, void *user_data);

/**
  * @}
  */
#endif

/**
 * @brief     This function is mainly used to initialize the SPI master and set user defined common parameters like clock frequency,
 *            bit order, clock polarity, clock phase and default settings.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] spi_config is the SPI master configure parameters. Details are described at #hal_spi_master_config_t.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR means function error; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY means this port of SPI master is busy and not available for use; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid parameter is given by user; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * @code
 *       hal_spi_master_config_t spi_config;
 *       spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
 *       spi_config.clock_frequency = 0x400000;
 *       spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE1;
 *       spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY1;
 *       ret = hal_spi_master_init(HAL_SPI_MASTER_1, &spi_config);
 * @endcode
 * @sa #hal_spi_master_deinit()
 */
hal_spi_master_status_t hal_spi_master_init(hal_spi_master_port_t master_port,
        hal_spi_master_config_t *spi_config);


#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
/**
 * @brief     SPI master advanced configuration function.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] advanced_config provides the advanced configuration parameters for the SPI master. Details are described at #hal_spi_master_advanced_config_t.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY means this port of SPI master is busy and not available for use; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid parameter is given by user; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * @code
 *       hal_spi_master_advanced_config_t advanced_config;
 *       advanced_config.byte_order = HAL_SPI_MASTER_BIG_ENDIAN;
 *       advanced_config.chip_polarity = HAL_SPI_MASTER_CHIP_SELECT_LOW;
 *       advanced_config.get_tick = HAL_SPI_MASTER_GET_TICK_DELAY1;
 *       advanced_config.sample_select = HAL_SPI_MASTER_SAMPLE_NEGATIVE;
 *       ret = hal_spi_master_set_advanced_config(HAL_SPI_MASTER_1, &advanced_config);
 * @endcode
 */
hal_spi_master_status_t hal_spi_master_set_advanced_config(hal_spi_master_port_t master_port,
        hal_spi_master_advanced_config_t *advanced_config);
#endif


/**
 * @brief     This function resets the SPI master, gates its clock, disables interrupts.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 * @sa #hal_spi_master_init()
 */
hal_spi_master_status_t hal_spi_master_deinit(hal_spi_master_port_t master_port);

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
 * @brief     This function is used to register user's callback to SPI master driver. This function should be called when user wants to use
 *            the DMA mode, the callback will be called in SPI interrupt service routine.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] callback is the callback function given by user, which will be called at SPI master interrupt service routine.
 * @param[in] user_data is a parameter given by user and will pass to user while the callback function is called. See the last parameter of #hal_spi_master_callback_t.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid parameter is given by user; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 * @sa #hal_spi_master_send_dma(), #hal_spi_master_send_and_receive_dma()
 */
hal_spi_master_status_t hal_spi_master_register_callback(hal_spi_master_port_t master_port,
        hal_spi_master_callback_t callback,
        void *user_data);
#endif

/**
 * @brief     This function is used to send data synchronously with FIFO mode. This function doesn't return until the transfer is complete.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] data is the buffer of data to be sent, this parameter cannot be NULL.
 * @param[in] size is the number of bytes to send. Note if using chip with DMA mode, like MT2523, this size shouldn't larger than #HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE bytes,
 *            if user still wants to send data size larger than #HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE bytes, we recommend user to use DMA mode.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR means the byte_order parameter is not configured rightly for the FIFO mode; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY means this port of SPI master is busy and not available for use; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid parameter is given by user; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * @code
 *       ret = hal_spi_master_send_polling(HAL_SPI_MASTER_1,data,size);
 * @endcode
 * @sa #hal_spi_master_send_and_receive_polling()
 */
hal_spi_master_status_t hal_spi_master_send_polling(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size);

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
 * @brief     This function is used to send data asynchronously with DMA mode. This function returns immediately, before calling this function,
 *            user should call #hal_spi_master_register_callback() firstly to register callback, once the transaction finished, the callback
 *            will be called in SPI interrupt service routine.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] data is the buffer of data to be sent, this parameter cannot be NULL, also the address must be as non-cacheable address.
 * @param[in] size is the number of bytes to send.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY means this port of SPI master is busy and not available for use; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid parameter is given by user; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 * @sa #hal_spi_master_send_and_receive_dma()
 */
hal_spi_master_status_t hal_spi_master_send_dma(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size);
#endif

/**
 * @brief     This function simultaneously sends and receives data in the FIFO mode. This function doesn't return until the transfer is complete.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] spi_send_and_receive_config is the structure that contains data buffer and data size, please refer to #hal_spi_master_send_and_receive_config_t
 *            definition for details. But user should judge whether the data received while sending command is valid or not, and this information can
 *            get from SPI device's datasheet. For example, if user uses <a href="http://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16375.pdf"> ADIS16375 </a>
 *            as full duplex communication mode, the data received while sending command is valid. If user uses
 *            <a href="http://www.thaieasyelec.com/downloads/EFDV423/PAH8001EI-2G.pdf"> PAH8001EI-2G </a> sensor, the data received while
 *            sending command is invalid.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR means the byte_order parameter is not configured rightly for the FIFO mode; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY means this port of SPI master is busy and not available for use; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid parameter in spi_send_and_receive_config is given; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * @code
 *       hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
 *       spi_send_and_receive_config.receive_buffer = receive_buffer;
 *       spi_send_and_receive_config.send_data = send_data;
 *       spi_send_and_receive_config.send_length = send_length;
 *       spi_send_and_receive_config.receive_length = receive_length;
 *       ret = hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_1, &spi_send_and_receive_config);
 * @endcode
 * @sa  #hal_spi_master_send_polling()
 */
hal_spi_master_status_t hal_spi_master_send_and_receive_polling(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config);

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
 * @brief     This function is used to send and receive data asynchronously with DMA mode. This function returns immediately, before
 *            calling this function, user should call #hal_spi_master_register_callback() firstly to register callback, once the transaction
 *            finished, the callback will be called in SPI interrupt service routine.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] spi_send_and_receive_config is the structure that contains data buffer and data size, please refer to #hal_spi_master_send_and_receive_config_t
 *            definition for details. But user should judge whether the data received while sending command is valid or not, and this information can
 *            get from SPI device's datasheet. For example, if user use <a href="http://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16375.pdf"> ADIS16375 </a>
 *            as full duplex communication mode, the data received while sending command is valid. If user use
 *            <a href="http://www.thaieasyelec.com/downloads/EFDV423/PAH8001EI-2G.pdf"> PAH8001EI-2G </a> sensor, the data received while
 *            sending command is invalid. The address parameters cannot be NULL, also must be as non-cacheable address.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY means this port of SPI master is busy and not available for use; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid parameter in spi_send_and_receive_config is given; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 * @sa #hal_spi_master_send_dma()
 */
hal_spi_master_status_t hal_spi_master_send_and_receive_dma(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config);
#endif

/**
 * @brief      This function gets current running status of the SPI master.
 * @param[in]  master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[out] running_status is the current running status.
 *             #HAL_SPI_MASTER_BUSY means the SPI master is in busy status; \n
 *             #HAL_SPI_MASTER_IDLE means the SPI master is in idle status, user can use it to transfer data now.
 * @return     #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *             #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means running_status parameter is NULL; \n
 *             #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par        Example
 * Sample code please refers to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 */
hal_spi_master_status_t hal_spi_master_get_running_status(hal_spi_master_port_t master_port,
        hal_spi_master_running_status_t *running_status);


#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
/**
 * @brief     This function is used to configure SPI master chip select timing parameter.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] chip_select_timing is the parameter settings for chip select timing.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY means this port of SPI master is busy and not available for use; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid chip select timing parameter is given; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 */
hal_spi_master_status_t hal_spi_master_set_chip_select_timing(hal_spi_master_port_t master_port,
        hal_spi_master_chip_select_timing_t chip_select_timing);
#endif


#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
/**
 * @brief     SPI master chip select de-assertion mode configuration.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] deassert is the parameter to set SPI master chip select signal behavior after sending one byte.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY means this port of SPI master is busy and not available for use; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid deassert parameter is given; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 */
hal_spi_master_status_t hal_spi_master_set_deassert(hal_spi_master_port_t master_port,
        hal_spi_master_deassert_t deassert);
#endif


#ifdef HAL_SPI_MASTER_FEATURE_MACRO_CONFIG
/**
 * @brief     SPI master macro group configuration.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] macro_select is the parameter for macro group selection.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT means master_port parameter is an invalid port number; \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY means this port of SPI master is busy and not available for use; \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER means an invalid macro selection parameter is given; \n
 *            #HAL_SPI_MASTER_STATUS_OK means this function return successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 */
hal_spi_master_status_t hal_spi_master_set_macro_selection(hal_spi_master_port_t master_port,
        hal_spi_master_macro_select_t macro_select);
#endif

#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/
#endif /*HAL_SPI_MASTER_MODULE_ENABLED*/
#endif /* __HAL_SPI_MASTER_H__ */

