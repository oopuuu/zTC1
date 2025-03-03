/**
 ******************************************************************************
 * @file    motion_sensor.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   motion sensor control header file.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __MOTION_SENSOR_H_
#define __MOTION_SENSOR_H_

#include "common.h"

/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MICOKIT_EXT_Driver
  * @{
  */

/** @defgroup MICOKIT_EXT_Motion_Sensor_Driver MiCOKit Ext Motion Sensor Driver
  * @brief Provide driver interface for MiCOKit Ext motion sensor
  * @{
  */

typedef struct _accel_data_t {
  int16_t      accel_datax;
  int16_t      accel_datay;
  int16_t      accel_dataz;  
} accel_data_t;

typedef struct _gyro_data_t {
  int16_t      gyro_datax;
  int16_t      gyro_datay;
  int16_t      gyro_dataz;
} gyro_data_t;

typedef struct _mag_data_t {
  int16_t      mag_datax;
  int16_t      mag_datay;
  int16_t      mag_dataz;
} mag_data_t;

typedef struct _motion_data_t {
  accel_data_t   accel_data;
  gyro_data_t    gyro_data;
  mag_data_t     mag_data;
} motion_data_t;


/**
 * @brief Initialize MiCOKit Ext - motion sensor Devices.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus motion_sensor_init(void);


/**
 * @brief Read data from MiCOKit Ext - motion sensor Devices.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus motion_sensor_readout(motion_data_t *motion_data);


/**
 * @brief Deinitialize MiCOKit Ext - motion sensor Devices.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus motion_sensor_deinit(void);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#endif  // __TEMP_HUM_SENSOR_H_



