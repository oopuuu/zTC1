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

#ifndef __HAL_GDMA_H__
#define __HAL_GDMA_H__
#include "hal_platform.h"

#ifdef HAL_GDMA_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GDMA
 * @{
 * This section introduces  the GDMA APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, GDMA function groups, enums, structures and functions.
 *
 * @section Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b GDMA                | General Direct Memeory Access. GDMA is an operation in which data is copied (transported) from one resource  to another in a computer system without the involvement of the CPU.
 *  For more information, please refer to <a href="https://en.wikipedia.org/wiki/Direct_memory_access"> GDMA in Wikipedia </a>.|
 * @section Features_Chapter Supported features
 *
 * - \b Support \b polling \b mode \b and \b interrupt \b modes
 *   GDMA can copy data from source memory to destination memory, instead of from  memory to I/O-device  or  I/O-device to	memory
 *  - \b polling \b mode:  In the polling mode, the execution status of the GDMA hardware returns valid during the transaction and invalid once the data transaction is finished. The status is retrieved only after the transaction is finished.
 *  - \b interrupt \b mode: In this mode, GDMA hardware generates an interrupt once the transaction  is complete. \n
 *   \n
 * @section GDMA_Architecture_Chapter Software architecture of the GDMA
 *
 * Below diagrams show the software architecture of GDMA driver.
 * -# GDMA polling mode architecture : Application  needs to call hal_gdma_init() to initialize GDMA source clock.
 *  Call hal_gdma_start_polling()  function to transfer data. Call hal_gdma_stop() to stop the GDMA data transfer.
 *  Call hal_gdma_get_running_status() to get busy or idle status. The application can also de-initialize the GDMA by calling hal_gdma_deinit() .
 *  Polling mode architecture is similar to the polling mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for polling mode architecture.
 * -# GDMA interrupt mode architecture : Application  needs to call hal_gdma_init() function to initialize GDMA source clock.
 *  Call hal_gdma_register_callback() function to register user's callback, and then call hal_gdma_start_interrupt() to transfer data. Call hal_gdma_stop() function to stop GDMA data transfer.
 *  Call hal_gdma_get_running_status() to get the GDMA status either on busy or idle. Call  hal_gdma_deinit() function to de-initialize GDMA to its original state.
 * Interrupt mode architecture is similar to the interrupt mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for interrupt mode architecture.


 * @section GDMA_Driver_Usage_Chapter How to use this driver
 *
 * - Use GDMA with polling mode. \n
 *  - step1: call hal_gdma_init() to initialize  GDMA source clock
 *  - step2: call hal_gdma_start_polling()  to transfer data
 *  - step3: call hal_gdma_stop() to stop GDMA .
 *  - step4: call hal_gdma_deinit() to de-initialize  GDMA
 *    @code
 *       ret = hal_gdma_init(channel,source_clock); //initialize  the GDMA source clock
 *       if(HAL_GDMA_STATUS_OK != ret) {
 *             //error handle
 *       }
 *       ret = hal_gdma_start_polling(channel, source_address,destination_address,data_length);
 *       if(HAL_GDMA_STATUS_OK != ret) {
 *             //error handle
 *       }
 *
 *       hal_gdma_get_running_status(channel,&running_status);
 *       //...
 *       hal_gdma_stop(gdma_channel); // stop GDMA.
 *       hal_gdma_deinit();   // de-initialize the GDMA.
 *     @endcode
 * - Use GDMA with interrupt mode. \n
 *  - step1: call hal_gdma_init() to initialize  GDMA source clock
 *  - step2: call hal_gdma_register_callback() to register a callback function
 *  - step3: call hal_gdma_start_interrupt() to transfer data
 *  - step4: call hal_gdma_stop() to stop GDMA .
 *  - step5: call hal_gdma_deinit() to  de-initialize  GDMA

 *  - sample code:
 *    @code
 *       ret = hal_gdma_init(gdma_channel,source_clock); //initialize the GDMA source clock
 *       if(HAL_GDMA_STATUS_OK != ret) {
 *             //error handle
 *       }
 *       ret = hal_gdma_register_callback( channel,  callback,(void *) &user_data); //register user's callback.
 *       if(HAL_GDMA_STATUS_OK != ret) {
 *             //error handle
 *       }
 *       ret =hal_gdma_start_interrupt(channel, source_address, destination_address,data_length); //enable GDMA to start transfer
 *       if(HAL_GDMA_STATUS_OK != ret) {
 *             //error handle
 *       }
 *       //...
 *      hal_gdma_get_running_status(channel, &running_status);
 *       //...
 *       hal_gdma_stop(gdma_channel); // stop the GDMA.
 *       hal_gdma_deinit();   // de-initialize the GDMA.
 *
 *    @endcode
 */


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_gdma_enum Enum
  * @{
  */


/** @brief GDMA status */
typedef enum {
    HAL_GDMA_STATUS_ERROR               = -3,         /**< GDMA function ERROR */
    HAL_GDMA_STATUS_ERROR_CHANNEL       = -2,         /**< GDMA error channel */
    HAL_GDMA_STATUS_INVALID_PARAMETER   = -1,         /**< GDMA error invalid parameter */
    HAL_GDMA_STATUS_OK   = 0,                         /**< GDMA function OK*/
} hal_gdma_status_t;


/** @brief GDMA transaction error */
typedef enum {
    HAL_GDMA_EVENT_TRANSACTION_ERROR = -1,       /**< GDMA transaction error */
    HAL_GDMA_EVENT_TRANSACTION_SUCCESS = 0,      /**< GDMA transaction success */
} hal_gdma_event_t;


/** @brief GDMA  running status */
typedef enum {
    HAL_GDMA_IDLE = 0,                         /**< GDMA idle */
    HAL_GDMA_BUSY = 1,                         /**< GDMA busy */
} hal_gdma_running_status_t;

/**
  * @}
  */

/** @defgroup hal_gdma_typedef Typedef
   * @{
   */

/** @brief  This defines the callback function prototype.
 *          Register a callback function when in an interrupt mode, this function will be called in GDMA interrupt.
 *          service routine after a transaction is complete. For more details about the callback, please refer to hal_gdma_register_callback() function.
 *          For more details about the callback architecture, please refer to @ref GDMA_Architecture_Chapter.
 *  @param [in] event is the transaction event for the current transaction, application can get the transaction result from this parameter.
 *              For more details about the event type, please refer to #hal_gdma_event_t
 *  @param [in] user_data is a parameter provided  by the application using hal_gdma_register_callback() function.
 *  @sa  #hal_spi_master_register_callback
 */

typedef void (*hal_gdma_callback_t)(hal_gdma_event_t event, void  *user_data);

/**
  * @}
  */

/**
 * @brief     This function initializes the GDMA based configuration.
 * @param[in] channel GDMA definition in enum hal_gdma_channel_t
 * @return   To indicate whether this function call is successful or not.
 *               If the return value is #HAL_GDMA_STATUS_OK, it means success.
 * @par       Example
 * Sample code please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa  hal_gdma_deinit()
 */
hal_gdma_status_t hal_gdma_init(hal_gdma_channel_t channel);


/**
 * @brief     This function is used to reset GDMA register and reset state.
 * @param[in] channel GDMA master name definition in enum hal_gdma_channel_t.
 * @return   To indicate whether this function call is successful or not.
 *               If the return value is #HAL_GDMA_STATUS_OK, it means success.
 * @par       Example
 * Sample code please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa  hal_gdma_init()
 */

hal_gdma_status_t hal_gdma_deinit(hal_gdma_channel_t channel);


/**
 * @brief      This function enables the GDMA to operate in a polling mode.
 * @param[in] channel GDMA master name definition in enum hal_gdma_channel_t.
 * @param[in] destination_address the address where application copies the source data into.
 * @param[in] source_address the address where application's source is stored.
 * @param[in]  data_length  the data length used in the transaction.
 * @return   To indicate whether this function call is successful or not.
 *               If the return value is #HAL_GDMA_STATUS_OK, it means success.
 * @par       Example
 * Sample code please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa  hal_gdma_start_interrupt()
 */

hal_gdma_status_t hal_gdma_start_polling(hal_gdma_channel_t channel, uint32_t destination_address, uint32_t source_address,  uint32_t data_length);


/**
 * @brief      This function enables the GDMA to operate in an interrupt mode,application can register callback when a GDMA interrupt occurs.
 * @param[in] channel GDMA master name definition in enum hal_gdma_channel_t.
 * @param[in] destination_address the address where application copies the source data into.
 * @param[in] source_address the address where application's source is stored.
 * @param[in]  data_length  the data length used in the transaction.
 * @return   To indicate whether this function call is successful or not.
 *               If the return value is #HAL_GDMA_STATUS_OK, it means success.
 * @par       Example
 * Sample code please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa  hal_gdma_start_polling()
 */

hal_gdma_status_t hal_gdma_start_interrupt(hal_gdma_channel_t channel, uint32_t destination_address, uint32_t source_address,  uint32_t data_length);


/**
 * @brief     This function is used to stop GDMA operation.
 * @param[in] channel GDMA master name definition in enum hal_gdma_channel_t.
 * @return   To indicate whether this function call is successful or not.
 *               If the return value is #HAL_GDMA_STATUS_OK, it means success.
 * @par       Example
 * Sample code please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa   hal_gdma_start_polling(), #hal_gdma_start_interrupt()
 */

hal_gdma_status_t hal_gdma_stop(hal_gdma_channel_t channel);


/**
 * @brief     This function is used to register a GDMA callback,
 * @param[in] channel GDMA master name definition in enum hal_gdma_channel_t.
 * @param[in] callback is the callback function given by the application, that is called at the GDMA interrupt service routine.
 * @param[in] user_data is a parameter provided by the application and is passed to the application once the callback function is called. See the last parameter of #hal_gdma_callback_t.
 * @par       Example
 * Sample code please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa   hal_gdma_start_interrupt()
 */

hal_gdma_status_t hal_gdma_register_callback(hal_gdma_channel_t channel, hal_gdma_callback_t callback, void *user_data);


/**
 * @brief    This function is used to get the current state of the GDMA.
 * @param[in] channel GDMA master name definition in enum hal_gdma_channel_t.
 * @param[out] running_status is the current running status.
 *             #HAL_GDMA_BUSY means the GDMA is in busy status; \n
 *             #HAL_GDMA_IDLE means the GDMA is in idle status, user can use it to transfer data now.
 * @return   To indicate whether this function call is successful or not.
 *               If the return value is #HAL_GDMA_STATUS_OK, it means success.
 * @par       Example
 * Sample code please refer to @ref GDMA_Driver_Usage_Chapter
 * @sa   hal_gdma_start_interrupt()
 */

hal_gdma_status_t hal_gdma_get_running_status(hal_gdma_channel_t channel, hal_gdma_running_status_t *running_status);


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_GDMA_MODULE_ENABLED*/
#endif /* __HAL_GDMA_H__ */


