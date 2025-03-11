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

#ifndef __HAL_EINT_H__
#define __HAL_EINT_H__
#include "hal_platform.h"

#ifdef HAL_EINT_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 * This section introduces the EINT APIs including terms and acronyms, supported features, software architecture, how to use this driver, EINT function, enums, structures and functions.
 *
 * @section HAL_EINT_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b EINT                 | External Interrupt Controller. Process the interrupt request from external source or peripheral.|
 * |\b ISR                  | Interrupt service routine.|
 * |\b GPIO                 | General Purpose Inputs-Outputs. For more information, please refer to @ref GPIO module in HAL. |
 * |\b NVIC                 | Nested Vectored Interrupt Controller. NIVC is the interrupt controller of ARM Cortex-M4. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual </a>.|
 * @section HAL_EINT_Features_Chapter Supported features
 * The EINT controller has been designed to process the intterrupts from external source or peripheral.
 *
 * - \b Support \b callback \b function \b registration. \n
 *   Call hal_eint_clear_software_trigger() function to trigger an interrupt immediately after the EINT is unmasked.
 *   \n
 * @}
 * @}
 */

#ifdef HAL_EINT_FEATURE_MASK
/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 *
 * @section HAL_EINT_Architecture_Chapter Software architecture of EINT
 *
 * For the architecture diagram please refer @ref HAL_Overview_3_Chapter.
 *
 *
 * @section HAL_EINT_Driver_Usage_Chapter How to use this driver
 *
 * - Use EINT. \n
 *  - Step1: Call hal_pinmux_set_function() to set the @ref GPIO pinmux. About the hal_pinmux_set_function(), for more information please refer to GPIO API document.\n
 *  - Step2: Call hal_eint_mask mask the selected EINT source. \n
 *  - Step3: initialize the selected EINT source. \n
 *      Call hal_eint_init() to configue the EINT number's trigger mode and debounce time, or call hal_eint_set_trigger_mode() and hal_eint_set_debounce_time() to replace.
 *  - Step4: regist a callback function \n
 *      Call hal_eint_register_callback() to register a user callback function.
 *  - Step5: unmask an eint \n
         Call hal_eint_unmask() to unmask an eint.
 *  - Sample code:
 *    @code
 *       hal_pinmux_set_function(gpio_pin, function_index);   //select GPIO and set pinmux.
 *       hal_eint_mask(eint_number);
 *       ret = hal_eint_init(eint_number, &eint_config);          //set EINT trigger mode and debounce time.
 *       if (HAL_EINT_STATUS_OK !=  ret) {
 *               //error handle;
 *       }
 *       ret = hal_eint_register_callback(eint_number, callback, user_data); // register a user callback.
 *       if (HAL_EINT_STATUS_OK !=  ret) {
 *               //error handle;
 *       }
 *       ret = hal_eint_unmask(eint_number);
 *
 *       //change trigger mode or debounce time at running time.
 *       hal_eint_mask(eint_number);
 *       ret = hal_eint_set_trigger_mode(eint_number, callback, user_data); // change trigger mode at running time.
 *       if (HAL_EINT_STATUS_OK !=  ret) {
 *               //error handle;
 *       }
 *       ret = hal_eint_set_debounce_time(eint_number, callback, user_data); //change debounce time at running time.
 *       if (HAL_EINT_STATUS_OK !=  ret) {
 *               //error handle;
 *       }
 *       ret = hal_eint_unmask(eint_number);
 *    @endcode
 *    @code
 *       //Callback function. This function should be registered with #hal_eint_register_callback().
 *       void callback(void *user_data)
 *       {
 *           //mask eint_nubmer at first.
 *           hal_eint_mask(eint_number);
 *           // user's handler
 *           //please call hal_eint_unmask to unmask eint_number if want recieve the eint interrupt.
 *           hal_eint_unmask(eint_nubmer);
 *       }
 *    @endcode
 */
#else
/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 *
 * @section HAL_EINT_Architecture_Chapter Software architecture of EINT
 *
 * For the architecture diagram please refer @ref HAL_Overview_3_Chapter.
 *
 *
 * @section HAL_EINT_Driver_Usage_Chapter How to use this driver
 *
 * - Use EINT. \n
 *  - Step1: Call hal_pinmux_set_function() to set the @ref GPIO pinmux. About the hal_pinmux_set_function(), for more information please refer to GPIO API document.\n
 *  - Step2: initialize the selected EINT source. \n
 *      Call hal_eint_init() to configue the EINT number's trigger mode and debounce time, or call hal_eint_set_trigger_mode() and hal_eint_set_debounce_time() to replace.
 *  - Step3: regist a callback function \n
 *      Call hal_eint_register_callback() to register a user callback function.
 *  - Sample code:
 *    @code
 *       hal_pinmux_set_function(gpio_pin, function_index);   //select GPIO and set pinmux
 *       ret = hal_eint_init(eint_number, &eint_config);      //set EINT trigger mode and debounce time.
 *       if (HAL_EINT_STATUS_OK !=  ret) {
 *               //error handle;
 *       }
 *       ret = hal_eint_register_callback(eint_number, callback, user_data); // register a user callback.
 *       if (HAL_EINT_STATUS_OK !=  ret) {
 *               //error handle;
 *       }
 *
 *       //change trigger mode or debounce time at running time.
 *       ret = hal_eint_set_trigger_mode(eint_number, callback, user_data); // change trigger mode at running time.
 *       if (HAL_EINT_STATUS_OK !=  ret) {
 *               //error handle;
 *       }
 *       ret = hal_eint_set_debounce_time(eint_number, callback, user_data); //change debounce time at running time.
 *       if (HAL_EINT_STATUS_OK !=  ret) {
 *               //error handle;
 *       }
 *
 *    @endcode
.*
 *    @code
 *       //Callback function. This function should be registered with #hal_eint_register_callback().
 *       void callback(void *user_data)
 *       {
 *           //mask eint_nubmer at first.
 *           hal_eint_mask(eint_number);
 *           // user's handler
 *           //please call hal_eint_unmask to unmask eint_number if want recieve the eint interrupt.
 *           hal_eint_unmask(eint_nubmer);
 *       }
 *    @endcode
 */
#endif

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_eint_enum Enum
  * @{
  */

/** @brief This emun define the eint trigger mode.  */
typedef enum {
    HAL_EINT_LEVEL_LOW     = 0,                 /**< level and low trigger */
    HAL_EINT_LEVEL_HIGH    = 1,                 /**< level and high trigger */
    HAL_EINT_EDGE_FALLING  = 2,                 /**< edge and falling trigger */
    HAL_EINT_EDGE_RISING   = 3,                 /**< edge and rising trigger */
    HAL_EINT_EDGE_FALLING_AND_RISING = 4        /**< edge and falling or rising trigger */
} hal_eint_trigger_mode_t;


/** @brief  This enum define the API return type.  */
typedef enum {
    HAL_EINT_STATUS_ERROR_EINT_NUMBER  = -3,     /**< eint error number */
    HAL_EINT_STATUS_INVALID_PARAMETER  = -2,     /**< eint error invalid parameter */
    HAL_EINT_STATUS_ERROR              = -1,     /**< eint undefined error */
    HAL_EINT_STATUS_OK                 = 0       /**< eint function ok */
} hal_eint_status_t;


/**
  * @}
  */


/** @defgroup hal_eint_struct Struct
  * @{
  */

/** @brief This structure defines the initial config structure. For more infomation please refer to #hal_eint_init. */
typedef struct {
    hal_eint_trigger_mode_t trigger_mode;      /**< eint trigger mode */
    uint32_t debounce_time;                    /**< eint hardware debounce time */
} hal_eint_config_t;

/**
  * @}
  */


/** @defgroup hal_eint_typedef Typedef
  * @{
  */
/** @brief  This defines the callback function prototype.
 *          A callback function should be registered for every EINT in use.
 *          This function will be called after an EINT interrupt is triggered in the EINT ISR.
 *          For more details about the callback function, please refer to hal_eint_register_callback().
 *  @param [out] user_data is the parameter which is set manually using hal_eint_register_callback() function.
 */
typedef void (*hal_eint_callback_t)(void *user_data);

/**
  * @}
  */


/*****************************************************************************
* Functions
*****************************************************************************/

/**
 * @brief This function is mainly used to initialize EINT number, which will set the EINT trigger mode and debounce time.
 * @param[in] eint_number is the EINT number, the value is HAL_EINT_0~HAL_EINT_MAX-1.
 * @param[in] eint_config is the initial configuration parameter. For more details, please refer to #hal_eint_config_t.
 * @return    To indicate whether this function call success or not.
 *            If the return value is #HAL_EINT_STATUS_OK,it means success;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER,it means a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_deinit()
 * @par       Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
*/
hal_eint_status_t hal_eint_init(hal_eint_number_t eint_number, const hal_eint_config_t *eint_config);


/**
 * @brief This function is mainly used to deinitialize EINT number. It sets the EINT trigger mode and debounce time.
 * @param[in] eint_number is the EINT number, the value is HAL_EINT_0~HAL_EINT_MAX-1.
 * @return    To indicate whether this function call success or not.
 *            If the return value is #HAL_EINT_STATUS_OK,it means success;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, it means a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_deinit()
 * @par       Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
*/
hal_eint_status_t hal_eint_deinit(hal_eint_number_t eint_number);


/**
 * @brief This function is used to register a callback function for EINT number.
 * @param[in] eint_number is the EINT number, the value is HAL_EINT_0~HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @param[in] callback is the function given by user, which will be called at EINT ISR routine.
 * @param[in] user_data is a reserved parameter for user.
 * @return    To indicate whether this function call success or not.
 *            If the return value is #HAL_EINT_STATUS_OK,it means success;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER,it means a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_init()
 * @par       Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_register_callback(hal_eint_number_t eint_number,
        hal_eint_callback_t callback,
        void *user_data);


/**
 * @brief This function is used to set the EINT number a trigger mode.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @param[in] trigger_mode is EINT nubmer trigger mode. For more details, please refer to #hal_eint_trigger_mode_t.
 * @return    To indicate whether this function call successful or not.
 *            If the return value is #HAL_EINT_STATUS_OK, it means success;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, it means a wrong parameter is given, the parameter must be verified.
 @par         Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_set_trigger_mode(hal_eint_number_t eint_number,
        hal_eint_trigger_mode_t trigger_mode);


/**
 * @brief This function is used to set the EINT number debounce time.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @param[in] time_ms is the EINT number's hardware debounce time in milliseconds;
 * @return    To indicate whether this function call successful or not.
 *            If the return value is #HAL_EINT_STATUS_OK, it means success;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, it means a wrong parameter is given, the parameter must be verified.
 @par         Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_set_debounce_time(hal_eint_number_t eint_number,
        uint32_t time_ms);

#ifdef HAL_EINT_FEATURE_SW_TRIGGER_EINT
/**
 * @brief This function is used to trigger EINT interrupt by software.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @return    To indicate whether this function call successful or not.
 *            If the return value is #HAL_EINT_STATUS_OK,it means success;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, it means a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_clear_software_trigger()
 @par         Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_set_software_trigger(hal_eint_number_t eint_number);


/**
 * @brief This function is used to clear EINT interrupt by software.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @return    To indicate whether this function call successful or not.
 *            If the return value is #HAL_EINT_STATUS_OK,it means success;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, it means a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_set_software_trigger()
 @par         Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_clear_software_trigger(hal_eint_number_t eint_number);
#endif

#ifdef HAL_EINT_FEATURE_MASK
/**
 * @brief This function is used to mask the dedicated EINT source.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @return    To indicate whether this function call successful or not.
 *            If the return value is #HAL_EINT_STATUS_OK, it means success;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, it means a wrong parameter is given, the parameter must be verified.
 * @sa  #hal_eint_unmask
 */
hal_eint_status_t hal_eint_mask(hal_eint_number_t eint_number);


/**
 * @brief This function is used to unmask the dedicated EINT source.
 * @param[in] eint_number is the EINT number, the value is HAL_EINT_0~HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @return    To indicate whether this function call successful or not.
 *            If the return value is #HAL_EINT_STATUS_OK, it means success;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, it means a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_mask()
 */
hal_eint_status_t hal_eint_unmask(hal_eint_number_t eint_number);
#endif


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_EINT_MODULE_ENABLED*/
#endif /* __HAL_EINT_H__ */

