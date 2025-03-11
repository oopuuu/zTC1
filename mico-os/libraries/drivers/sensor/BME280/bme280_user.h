/**
 ******************************************************************************
 * @file    bme280_user.h
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    17-Mar-2015
 * @brief   bme280 control operations.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __BME280_USER_H_
#define __BME280_USER_H_

#include "mico.h"
#include "platform.h"


/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MiCO_Sensor_Driver
  * @{
  */
    
/** @defgroup MiCO_BM280_Driver MiCO BME280 Driver
  * @brief Provide driver interface for BME280 Sensor 
  * @{
  */

#ifndef BME280_I2C_DEVICE
#define BME280_I2C_DEVICE      MICO_I2C_NONE
#endif

#define s32 int32_t
#define u32 uint32_t


/**
 * @brief Initialize BME280sensor device.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bme280_sensor_init(void);

/**
 * @brief Read data from BME280sensor device.
 *
 * @param   v_actual_temp_s32   : Environmental light parameter 
 * @param   v_actual_press_u32  : Environmental Atmospheric pressure parameter
 * @param   v_actual_humity_u32 : Environmental humidity parameter
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bme280_data_readout(s32 *v_actual_temp_s32, u32 *v_actual_press_u32, u32 *v_actual_humity_u32);

/**
 * @brief Read temperature data from BME280sensor device.
 *
 * @param v_actual_temp_s32: Environmental temperature parameter
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bme280_read_temperature(s32 *v_actual_temp_s32);


/**
 * @brief Read humidity data from BME280sensor device.
 *
 * @param v_actual_humity_u32: Environmental humidity parameter
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bme280_read_humidity(u32 *v_actual_humity_u32);


/**
 * @brief Read pressure data from BME280sensor device.
 *
 * @param v_actual_press_u32: Environmental pressure parameter
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bme280_data_pressure(u32 *v_actual_press_u32);


/**
 * @brief Deinitialize BME280sensor device.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus bme280_sensor_deinit(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif  // __BME280_USER_H_
