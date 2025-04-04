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

#ifndef __HAL_DAC_H__
#define __HAL_DAC_H__
#include "hal_platform.h"

#ifdef HAL_DAC_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup DAC
 * @{
 * This section describes the programming interfaces of the DAC HAL driver.
 *
 * @section HAL_DAC_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the DAC driver and how to use the various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DAC                        | DAC is a Digital-to-Analog Converter that converts digital data (usually binary) into an analog signal (current, voltage, or electric charge). |
 *
 * @section HAL_DAC_Features_Chapter Supported features
 * The general purpose DAC acts similar to a waveform generator. Resolution of the DAC is 10 bits. Features supported by this module are listed below:
 * - \b Write \b the \b output \b data. \n
 *   Before the DAC data output is enabled, it's written into the internal RAM dedicated to the DAC, the maximum memory capacity of 128 DAC data length, having 10 bits for each DAC data. User can write the DAC data to RAM by calling the function hal_dac_write_data().
 * - \b Configure \b the \b output \b mode. \n
 *   Data written into the internal RAM is transmitted as a waveform. Call function hal_dac_configure_output(), to set the output mode either on repeat mode by defining the first parameter "mode" as HAL_DAC_REPEAT_MODE. The parameter "start_address" and "end_address" together indicate the data range that is transmitted.
 * - \b Start (Stop) \b data \b output. \n
 *   Call hal_dac_start_output(), to transmit the DAC data in a user configured output mode (only repeat mode is supported right now). Call hal_dac_stop_output(), to stop the DAC output data transmission.
 * @section HAL_DAC_Driver_Usage_Chapter How to use this driver
 * - \b Use \b DAC \b driver \b to \b output \b an \b analog \b waveform.
 *   - Step1: Call hal_dac_init() to initialize the DAC module.
 *   - Step2: Call hal_dac_write_data() to write data that will be outputed.
 *   - Step3: Call hal_dac_configure_output() to configure the data range and output mode(only repeat mode is supported right now).
 *   - Step4: Call hal_dac_start_output() to start the data conversion.
 *   - Step5: Call hal_dac_stop_output() to stop the data conversion if needed.
 *   - Sample code:
 *   @code
 *
 *   #define DATA_LEN  5
 *
 *   uint32_t start_address = 0;
 *   uint32_t data[DATA_LEN] = {0x0010, 0x0011, 0x0012, 0x0013, 0x0014};
 *
 *   hal_dac_init();//Initialize the DAC module.
 *   hal_dac_write_data(start_address, data, DATA_LEN);//Write DAC data to internal RAM.
 *   hal_dac_configure_output(HAL_DAC_REPEAT_MODE, start_address, start_address+DATA_LEN-1);//Configure the DAC output mode.
 *   hal_dac_start_output();//Start DAC conversion.
 *   hal_dac_stop_output();//Stop DAC if needed.
 *   hal_dac_deinit();//De-initialize the DAC module.
 *
 *   @endcode
 *
 */


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_dac_enum Enum
 *  @{
 */

/** @brief This enum defines the DAC API return status*/
typedef enum {
    HAL_DAC_STATUS_INVALID_PARAMETER = -3,      /**< Invalid parameter */
    HAL_DAC_STATUS_ERROR_BUSY = -2,             /**< DAC is busy */
    HAL_DAC_STATUS_ERROR = -1,                  /**< DAC error */
    HAL_DAC_STATUS_OK = 0                       /**< DAC ok */
} hal_dac_status_t;

/** @brief This enum defines the DAC output mode, only repeat mode is supported right now */
typedef enum {
    HAL_DAC_REPEAT_MODE = 0,                    /**< DAC output in repeat mode */
} hal_dac_mode_t;

/**
 * @}
 */


/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief   DAC initialization function.
 * @return
 * #HAL_DAC_STATUS_OK, the DAC is successfully initialized. \n
 * #HAL_DAC_STATUS_ERROR_BUSY, the DAC is busy. \n
 * #HAL_DAC_STATUS_ERROR, DAC clock enable failed.
 */
hal_dac_status_t hal_dac_init(void);


/**
 * @brief 	DAC deinitialization function. This function resets the DAC peripheral registers to their default values.
 * @return
 * #HAL_DAC_STATUS_OK, the DAC is successfully deinitialized. \n
 * #HAL_DAC_STATUS_ERROR, DAC clock disable failed.
 */
hal_dac_status_t hal_dac_deinit(void);


/**
 * @brief 	Start the DAC data conversion, data written into the internal RAM is transmitted as an analog waveform.
 * @return
 * #HAL_DAC_STATUS_OK, the DAC output is successfully started.
 */
hal_dac_status_t hal_dac_start_output(void);

/**
 * @brief 	Call this function to stop the DAC data conversion.
 * @return
 * #HAL_DAC_STATUS_OK, the DAC output is successfully stopped.
 */
hal_dac_status_t hal_dac_stop_output(void);

/**
 * @brief 	Write data into the internal RAM, prepared to be converted as analog waveform.
 * @param[in] start_address is the address for the data to be saved, should be 0~127.
 * @param[in] data is the base address of the data that will be written into the internal RAM.
 * @param[in] length is the length of the data that will be written.
 * @return
 * #HAL_DAC_STATUS_INVALID_PARAMETER, indicates that the combination of parameters start_address and length is out of range of the internal RAM, or the data is NULL. \n
 * #HAL_DAC_STATUS_ERROR, writing data to the internal RAM failed. \n
 * #HAL_DAC_STATUS_OK, writing data to the internal RAM is successful.
 */
hal_dac_status_t hal_dac_write_data(uint32_t start_address, const uint32_t *data, uint32_t length);


/**
 * @brief 	This function configures the output mode of the DAC as well as the data range that will be outputted.
 * @param[in] mode is the output mode of DAC, it is either repeat mode or no repeat mode, the parameter should be of type #hal_dac_mode_t.
 * @param[in] start_address is the base address of the internal RAM that contains the data will be converted.
 * @param[in] end_address is the end address of data in the internal RAM.
 * @return
 * #HAL_DAC_STATUS_INVALID_PARAMETER, the start_address or end_address is invalid. \n
 * #HAL_DAC_STATUS_ERROR, the mode is not #HAL_DAC_REPEAT_MODE. \n
 * #HAL_DAC_STATUS_OK, the DAC output is successfully configured.
 * @note
 * User should make sure that both the start_address and end_address are no more than 127.
 */
hal_dac_status_t hal_dac_configure_output(hal_dac_mode_t mode, uint32_t start_address, uint32_t end_address);


#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
*/

#endif /* HAL_DAC_MODULE_ENABLED*/
#endif /* __HAL_DAC_H__ */



