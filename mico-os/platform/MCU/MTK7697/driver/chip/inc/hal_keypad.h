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

#ifndef __HAL_KEYPAD_H__
#define __HAL_KEYPAD_H__

#include "hal_platform.h"


#ifdef HAL_KEYPAD_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup KEYPAD
 * @{
 * This section introduces the keypad driver APIs including terms and acronyms, supported features,
 * software architecture, details on how to use this driver, enums, structures and functions.

 * @section HAL_KEYPAD_Terms_Chapter Terms and acronyms
 *
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b keypad                     | The keypad is an input device controller that can get input events.|
 * |\b GPIO                       | For an introduction to General Purpose Inputs-Outputs, please refer to the @ref GPIO module in HAL. |
 * |\b NVIC                       | The Nested Vectored Interrupt Controller (NVIC) is the interrupt controller of ARM Cortex-M.For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual </a>.|
 *
 * @section HAL_KEYPAD_Features_Chapter Supported features
 *
 * This module provides a generic design to get external key events.
 * The keypad provides two different modes, the keypad mode and the power key mode.
 *
 * - \b Support \b keypad \b mode.\n
 *  In this mode, an interrupt is triggered whenever a normal key is pressed or released.
 *  A callback function can be registered for the interrupt. The callback function is invoked whenever a key is pressed or released.
 *  Inside the callback function, #hal_keypad_get_key() can be used to get the key event and the key position number.\n
 *
 * - \b Support \b powerkey \b mode.\n
 *  In this mode, an interrupt is triggered whenever the powerkey is pressed or released.
 *  A callback function can be registered for the interrupt. The callback function is invoked whenever the powerkey is pressed or released.
 *  Inside the callback function, #hal_keypad_powerkey_get_key() can be used to get the key event and the key data.\n
 * @}
 * @}
 */

/***
 * @addtogroup HAL
 * @{
 * @addtogroup KEYPAD
 * @{
 * @section HAL_KEYPAD_Driver_Usage_Chapter How to use this driver
 *
 * - \b Using \b the \b keypad \b mode.\n
 *    To use keypad driver in normal keypad mode, look up @ref GPIO datasheet to determine which GPIOs should be selected to pinmux to keypad column and row pins.
 *    Notice that if you has used EPT tool to configure keypad pin setting, it's no longer to set keypad GPIO pinmux anymore.
 *    Then call #hal_keypad_init() function to manually set the keypad mode, the key column and row and the debounce time.
 *    Call #hal_keypad_init() function, then call #hal_keypad_register_callback() function to register a callback.
 *    Call #hal_keypad_enable() to start the keypad module.
 *    If a key is pressed or released on the keyboard, the keypad triggers an interrupt and calls a callback function.
 *    User should use #hal_keypad_get_key() in the callback function to get the key event and the key position number.
 *    To ensure a smooth key scan, do not overload the callback function. Let the callback return as quick as possible.
 *  - Step1: Call #hal_keypad_init() to  configure the base environment.
 *  - Step2: Call #hal_keypad_register_callback() to register a callback function.
 *  - Step3: Call #hal_keypad_enable() to start the keypad module.
 *  - Step4: Call #hal_keypad_get_key() to get the key event and the key position number inside the callback function.
 *  - Step5: Call #hal_keypad_deinit() to de-initilize the keypad module if it's no longer in use.
 *  - Sample code:
 *    @code
 *      hal_keypad_config_t keypad_config;
 *      hal_keypad_status_t ret_status;
 *
 *      keypad_config.mode  = HAL_KEYPAD_MODE_DOUBLE_KEY;                   //Chooses double key.
 *      keypad_config.key_map.column_bitmap = 0x07;                         //Column0~2 is selected.
 *      keypad_config.key_map.row_bitmap    = 0x07;                         //Row0~2 is selected.
 *      keypad_config.debounce              = 16;                           //Sets the debounce time to 16 milliseconds.
 *
 *      ret_status = hal_keypad_init(&keypad_config);                       //Sets the keypad base environment.
 *      if ( ret_status != HAL_KEYPAD_STATUS_OK ) {                         //If the configuration is not #HAL_KEYPAD_STATUS_OK, exception handler.
 *         //Call the exception handler
 *      }
 *
 *      //Register a user callback.
 *      ret_status  = hal_keypad_register_callback(user_keypad_callback,user_data);
 *      if ( ret_status != HAL_KEYPAD_STATUS_OK ) {
 *         //Call the exception handler
 *      }
 *
 *      hal_keypad_enable();                                                //Enables the keypad module.
 *
 *      ...
 *
 *      hal_keypad_deinit();                                                //Deinitializes the keypad module, if it's no longer in use.
 *    @endcode
 *    @code
 *       // Callback function. This function should be registered with #hal_keypad_register_callback().
 *       void user_keypad_callback (void *user_data)
 *       {
 *           // User's handler
 *           hal_keypad_event_t *keypad_event;
 *
 *           hal_keypad_get_key(keypad_event);
 *
 *       }
 *
 *    @endcode
 *
 * - \b Using \b powerkey \b mode.\n
 *    Call #hal_keypad_powerkey_init() function to set the base environment and the key data value to use the powerkey mode.
 *    Call #hal_keypad_powerkey_init() function, then call #hal_keypad_powerkey_register_callback() function to register a callback.
 *    The powerkey module starts working once a callback function is registered.
 *    If the power key is pressed or released, the key triggers an interrupt and calls a callback function.
 *    User should use #hal_keypad_powerkey_get_key() in callback function to get the key event and the key data values.
 *    To ensure a smooth key scan, do not overload the callback function. Let the callback return as quick as possible.
 *  - Step1: Call #hal_keypad_powerkey_init() to  configure the base environment and the key data value.
 *  - Step2: Call #hal_keypad_powerkey_register_callback() to register a callback function and the powerkey driver starts working.
 *  - Step3: Call #hal_keypad_powerkey_get_key() to get key event and key data value.
 *  - Step4: Call #hal_keypad_powerkey_deinit() to de-initilize the powerkey if it's no longer in use.
 *  - Sample code:
 *    @code
 *      hal_keypad_config_t keypad_config;
 *      hal_keypad_status_t ret_status;
 *      uint32_t powerkey_data;
 *
 *      powerkey_data = 0xfe;                                                //Sets the key value to 0xfe.
 *
 *      ret_status = hal_keypad_powerkey_init(&powerkey_data);               //Sets the keypad base environment.
 *      if ( ret_status != HAL_KEYPAD_STATUS_OK ) {                          //If the configuration is not #HAL_KEYPAD_STATUS_OK, call the exception handler.
 *         //Exception handler
 *      }
 *
 *      //Register a user callback.
 *      ret_status  = hal_keypad_powerkey_register_callback(user_keypad_callback,user_data);
 *      if ( ret_status != HAL_KEYPAD_STATUS_OK ) {
 *         //Exception handler
 *      }
 *      ...
 *
 *      hal_keypad_powerkey_deinit();                                          //Deinitializes the keypad powerkey module, if it's no longer in use.
 *    @endcode
 *    @code
 *       // Callback function. This function should be registered with #hal_keypad_powerkey_register_callback().
 *       void user_keypad_callback (void *user_data)
 *       {
 *           // User's handler
 *           hal_powerkey_event_t *powerkey_event;
 *
 *           hal_keypad_powerkey_get_key(powerkey_event);
 *
 *       }
 *
 *    @endcode
 *
 * @}
 * @}
 */
 
/***
 * @addtogroup HAL
 * @{
 * @addtogroup KEYPAD
 * @{
 * @section HAL_KEYPAD_Driver_EPT_Chapter How to connect the EPT tool
 * 
 * - \b Normal \b keypad \b mode.\n
 *   The normal keypad mode only supports keypad pins. The other GPIO pins cannot be used with the keypad driver.
 *   Two types of keypads are supported in the normal keypad mode: 3x3 single keys and 3x3 configurable double keys.
 *   The EPT tool configures the keypad pin columns and rows in GPIO setting page, it maps the symbol of key data to hardware key data, and selects the single or double key mode in keypad settings page.
 * - \b Powerkey \b mode.\n
 *   The powerkey mode supports the powerkey pin only. The other pins cannot be used with the keypad driver.
 *   The powerkey driver supports power key only. The EPT tool maps the hardware key data to the powerkey symbol. 
 * @}
 * @}
 */

/***
 * @addtogroup HAL
 * @{
 * @addtogroup KEYPAD
 * @{
 * @section HAL_KEYPAD_Driver_Custmozation_Chapter How to customize the keypad driver
 * 
 * The configurations related to the keypad are in the custom files under driver\board\<mtxxxx_hdk>\keypad.
 * In these files, user can modify the debounce time, longpress and repeat time for the keys in the normal keypad mode. 
 * Modify the longpress and repeat time for the keys in the powerkey mode.
 * These files provide functions to translate hardware key data to the symbol of EPT tool keypad data in normal keypad mode. 
 * @}
 * @}
 */
 

/**
 * @ingroup HAL
 * @addtogroup KEYPAD
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_keypad_typedef Typedef
  * @{
  */

/** @brief  This function registers a callback function when in a normal key mode.
 *          This function is called after the key is  pressed or released in the keypad ISR routine.
 *          For more details about the callback function, please refer to #hal_keypad_register_callback().
 * @param [in] user_data: This variable pointer is defined by the user to record the data.
 */
typedef void (*hal_keypad_callback_t)(void *user_data);


/** @brief  This function registers a callback function when in a powerkey mode.
 *          This function is called after the key is  pressed or released in the powerkey ISR routine.
 *          For more details about the callback function, please refer to #hal_keypad_powerkey_register_callback().
 * @param [in] user_data: This variable pointer is defined by the user to record the data.
 */
typedef void (*hal_powerkey_callback_t)(void *user_data);
/**
  * @}
  */

/** @defgroup hal_keypad_enum Enum
  * @{
  */

/** @brief This enum defines keypad mode seletion */
typedef enum {
    HAL_KEYPAD_MODE_SINGLE_KEY = 0,          /**< Sets the keypad single key mode.  */
    HAL_KEYPAD_MODE_DOUBLE_KEY = 1          /**< Sets the keypad double key mode.  */
} hal_keypad_mode_t;

/** @brief This enum defines keypad status */
typedef enum {
    HAL_KEYPAD_STATUS_ERROR      = -2,      /**< A keypad function error. */
    HAL_KEYPAD_INVALID_PARAMETER = -1,      /**< An invalid parameter. */
    HAL_KEYPAD_STATUS_OK   = 0              /**< No error occurred during the function call. */
} hal_keypad_status_t;

/** @brief This enum defines keypad event status */
typedef enum {
    HAL_KEYPAD_KEY_RELEASE        = 0,        /**< A key is released. */
    HAL_KEYPAD_KEY_PRESS          = 1,        /**< A key is pressed. */
    HAL_KEYPAD_KEY_LONG_PRESS     = 2,        /**< A key is long pressed. */
    HAL_KEYPAD_KEY_REPEAT         = 3,        /**< A key is repeat pressed. */
    HAL_KEYPAD_KEY_PMU_LONG_PRESS = 4		  /**< PMU hardware powerkey is long pressed. */
} hal_keypad_key_state_t;
/**
  * @}
  */

/** @defgroup hal_keypad_struct Struct
  * @{
  */

/** @brief This structure defines keypad column and row parameter*/
typedef struct {
    uint8_t column_bitmap;                  /**< Specifies the keypad column bitmap [2:0]. */
    uint8_t row_bitmap;                     /**< Specifies the keypad row   bitmap [2:0]. */
} hal_keypad_column_row_t;


/** @brief This structure defines keypad configuration. More infomation please refer to #hal_keypad_init() */
typedef struct {
    hal_keypad_mode_t       mode;           /**< Specifies the keypad single or double key mode. */
    hal_keypad_column_row_t key_map;        /**< Specifies the keypad column and row bitmaps. */
    uint32_t    debounce;                   /**< Specifies the keypad debounce time, the unit is millisecond. */
    uint32_t    repeat_time;                /**< Specifies the keypad repeat event time, the unit is millisecond. */
    uint32_t    longpress_time;             /**< Specifies the keypad longpress event time, the unit is millisecond. */
} hal_keypad_config_t;


/** @brief This structure defines keypad scan timing. More infomation please refer to #hal_keypad_set_scan_timing()*/
typedef struct {
    uint8_t column_high_pulse;              /**< Specifies the high level of column pulses will stay clocks. */
    uint8_t row_high_pulse;                 /**< Specifies the high level of row pulses will stay clocks. */
    uint8_t column_scan_div;                /**< Specifies the period of column pulses will stay clocks. */
    uint8_t row_scan_div;                   /**< Specifies the period of row    pulses will stay clocks. */
} hal_keypad_scan_timing_t;

/** @brief This structure defines keypad key data*/
typedef struct {
    hal_keypad_key_state_t   state;          /**< Specifies the key state as either pressed or released. */
    uint32_t             key_data;           /**< Specifies the key position number on the keypad.*/
} hal_keypad_event_t;

/** @brief This structure defines powerkey key data*/
typedef struct {
    hal_keypad_key_state_t   state;          /**< Specifies the powerkey state is pressed or released. */
    uint32_t             key_data;           /**< Specifies the powerkey data. */
} hal_keypad_powerkey_event_t;

/**
  * @}
  */

/**
 * @brief     This function initializes the keypad base environment.
 *            Call this function if keypad is required.
 * @param[in] keypad_config is the pointer to configuration. For mode details, please refer to #hal_keypad_config_t.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.\n
 *            HAL_KEYPAD_STATUS_ERROR, if #()hal_keypad_init has not been initialized.
 * @sa        #hal_keypad_deinit()
 */
hal_keypad_status_t hal_keypad_init(const hal_keypad_config_t *keypad_config);


/**
 * @brief     This function deinitializes the keypad module.
 *            After calling this function, the callback is cleared, interrupts and keypad module are disabled.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.\n
 *            #HAL_KEYPAD_STATUS_ERROR, if #()hal_keypad_init has not been initialized or if there still has data in keypad buffer.
 * @sa        #hal_keypad_init()
 */
hal_keypad_status_t hal_keypad_deinit(void);


/**
 * @brief     This function sets the keypad debounce time, the unit is in milliseconds.
 * @param[in] keypad_debounce is a pointer to the debounce time setting, it can not be larger than 255.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.\n
 *            #HAL_KEYPAD_INVALID_PARAMETER, if the value is larger than 255.
 * @sa        #hal_keypad_get_debounce()
 */
hal_keypad_status_t hal_keypad_set_debounce(const uint32_t *keypad_debounce);

/**
 * @brief     This function gets the keypad debounce time, the unit is in milliseconds.
 * @param[in] keypad_debounce is a user defined pointer to get the current debounce time.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.\n
 * @sa        #hal_keypad_get_debounce()
 */
hal_keypad_status_t hal_keypad_get_debounce(uint32_t *keypad_debounce);


/**
 * @brief     This function enables the keypad module..
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.\n
 * @sa        #hal_keypad_disable()
 */
hal_keypad_status_t hal_keypad_enable(void);

/**
 * @brief     This function disables the keypad module.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.\n
 *            HAL_KEYPAD_STATUS_ERROR, if #()hal_keypad_init has not been initialized.
 * @sa        #hal_keypad_enable()
 */
hal_keypad_status_t hal_keypad_disable(void);

/**
 * @brief     Set the keypad scan timing.
 * @param[in] keypad_scan_timing is a pointer to the configuration.
 *            For more details, please refer to #hal_keypad_scan_timing_t.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.
 *            #HAL_KEYPAD_INVALID_PARAMETER, if paramter is wrong.
 * @sa        #hal_keypad_get_scan_timing()
 */
hal_keypad_status_t hal_keypad_set_scan_timing(const hal_keypad_scan_timing_t *keypad_scan_timing);


/**
 * @brief     Get the keypad scan timing.
 * @param[in] keypad_scan_timing is a pointer of the current configuration.
 *            For more details, please refer to #hal_keypad_scan_timing_t.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.
 *            #HAL_KEYPAD_INVALID_PARAMETER, if paramter is wrong.
 * @sa        #hal_keypad_set_scan_timing().
 */
hal_keypad_status_t hal_keypad_get_scan_timing(hal_keypad_scan_timing_t *keypad_scan_timing);


/**
 * @brief     This function registers a callback function when in a normal key mode.
 *            The callback can only be registered after calling the function #hal_keypad_init().
 *            The callback function is called after a key is pressed or released  in the ISR routine.
 * @param[in] callback is a function pointer to the callback.
 *            This callback is called when the keypad key is released or pressd.
 * @param[in] user_data: This variable pointer is defined by the user to record the data.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.
 *            #HAL_KEYPAD_STATUS_ERROR, #hal_keypad_init() has not been called or callback is null.
 */
hal_keypad_status_t hal_keypad_register_callback(hal_keypad_callback_t callback, void *user_data);


/**
 * @brief     This function gets the key event. The key event specifies if the current key state is pressed or released,
 *            and the data is the key position number, not the key value.
 * @param[in] keypad_event is a pointer to the key event. For more details, please refer to #hal_keypad_event_t.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.
 *            #HAL_KEYPAD_STATUS_ERROR, if there is no data in buffer.
 */
hal_keypad_status_t hal_keypad_get_key(hal_keypad_event_t *keypad_event);


/**
 * @brief     This function  initializes the powerkey base environment.
 *            After calling this function, the inrrupt is enabled.
 * @param[in] powerkey_data is the powerkey data defined by user.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.
 *            HAL_KEYPAD_STATUS_ERROR, powerkey initialized fail.
 * @sa        #hal_keypad_powerkey_deinit().
 */
hal_keypad_status_t hal_keypad_powerkey_init(uint32_t powerkey_data);


/**
 * @brief     This function de-initializes the powerkey base environment.
 *            After calling this function, the inrrupt is disabled, the callback function is cleared.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.
 *            HAL_KEYPAD_STATUS_ERROR, powerkey deinitialized fail.
 * @sa        #hal_keypad_powerkey_init().
 */
hal_keypad_status_t hal_keypad_powerkey_deinit(void);


/**
 * @brief     This function registers a callback function when in a powerkey mode.
 *            The callback can only be registered after calling the function #hal_keypad_powerkey_init().
 *            The callback function will be called after the powerkey is pressed or released in the ISR routine.
 *            After register a callback function,  the powerkey module is started.
 * @param[in] callback is a  pointer to the callback.
 *            This callback will be called when the powerkey release or press a key.
 * @param[in] user_data: This variable pointer is defined by the user to record the data.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.
 *            #HAL_KEYPAD_STATUS_ERROR, #hal_keypad_powerkey_init() has not been called or callback is null.
 *
 */
hal_keypad_status_t hal_keypad_powerkey_register_callback(hal_powerkey_callback_t callback, void *user_data);


/**
 * @brief     This function gets the powerkey event. The powerkey event specifies if the current key state is pressed or released,
 *            and the key data.
 * @param[in] powerkey_event is a pointer to the powerkey event. For more details, please refer to #hal_keypad_powerkey_event_t.
 * @return    #HAL_KEYPAD_STATUS_OK, if operation is successful.
 *            #HAL_KEYPAD_STATUS_ERROR, if all of the key data and event has been read.
 */
hal_keypad_status_t hal_keypad_powerkey_get_key(hal_keypad_powerkey_event_t *powerkey_event);


#ifdef __cplusplus
}
#endif
/**
* @}
* @}
*/

#endif /* HAL_KEYPAD_MODULE_ENABLED */
#endif /* __HAL_KEYPAD_H__ */


