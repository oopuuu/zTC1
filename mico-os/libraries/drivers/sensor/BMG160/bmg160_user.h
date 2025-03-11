/**
******************************************************************************
* @file    bmg160_user.h 
* @author  William Xu
* @version V1.0.0
* @date    21-May-2015
* @brief   bmg160 sensor control operations.
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
#ifndef __BMG160_USER_H_
#define __BMG160_USER_H_

#include "mico_platform.h"
#include "platform.h"


#define s32 int32_t
#define u32 uint32_t


/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MiCO_Sensor_Driver
  * @{
  */

/** @defgroup MiCO_BMG160_Driver MiCO BMG160 Driver
  * @brief Provide driver interface for BMG160 Sensor 
  * @{
  */
#ifndef BMG160_I2C_DEVICE
#define BMG160_I2C_DEVICE      MICO_I2C_NONE
#endif

/**
 * @brief Initialize BMG160 sensor device.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bmg160_sensor_init(void);


/**
 * @brief Read data from BME280sensor device.
 *
 * @param   v_gyro_datax_s16  : gyro X data
 * @param   v_gyro_datay_s16  : gyro Y data
 * @param   v_gyro_dataz_s16  : gyro Z data
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bmg160_data_readout(s16 *v_gyro_datax_s16, s16 *v_gyro_datay_s16, s16 *v_gyro_dataz_s16);


/**
 * @brief Deinitialize BMG160 sensor device.
 *
 * @return none
 */
OSStatus bmg160_sensor_deinit(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif



