/**
 ******************************************************************************
 * @file    bma2x2_user.h
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   bma2x2 sensor control operations.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __BMA2x2_USER_H_
#define __BMA2x2_USER_H_

#include "mico_platform.h"
#include "platform.h"


/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MiCO_Sensor_Driver
  * @{
  */
    
/** @defgroup MiCO_BMA2x2_Driver MiCO BMA2x2 Driver
  * @brief Provide driver interface for BMA2x2 Sensor 
  * @{
  */

#define s16 int16_t
#define u32 uint32_t

#ifndef BMA2x2_I2C_DEVICE
#define BMA2x2_I2C_DEVICE      MICO_I2C_NONE
#endif


/**
 * @brief Initialize BMA2X2 sensor device.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bma2x2_sensor_init(void);


/**
 * @brief Read data from BMA2X2 sensor device.
 *
 * @param  v_accel_x_s16:  Acceleration of X axis
 * @param  v_accel_y_s16:  Acceleration of Y axis
 * @param  v_accel_z_s16:  Acceleration of Z axis
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bma2x2_data_readout(s16 *v_accel_x_s16, s16 *v_accel_y_s16, s16 *v_accel_z_s16);


/**
 * @brief Deinitialize BMA2X2 sensor device.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bma2x2_sensor_deinit(void);

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
