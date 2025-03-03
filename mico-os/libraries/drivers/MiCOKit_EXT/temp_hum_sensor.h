/**
 ******************************************************************************
 * @file    temp_hum_sensor.h
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    1-May-2015
 * @brief   DHT11 operation.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __TEMP_HUM_SENSOR_H_
#define __TEMP_HUM_SENSOR_H_

#include "common.h"


/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MICOKIT_EXT_Driver
  * @{
  */

/** @defgroup MICOKIT_EXT_Temp_Hum_Driver MiCOKit Ext Temp Hum Driver
  * @brief Provide driver interface for MiCOKit Ext Temp Hum Sensor
  * @{
  */
typedef enum {
  MICOKIT_TEMP_HUM_SENSOR_NONE = 0, 
  MICOKIT_TEMP_HUM_SENSOR_BME280,
  MICOKIT_TEMP_HUM_SENSOR_DHT11
}temp_hum_sensor_type_t;


/**
 * @brief Initialize MiCOKit Ext - temp hum sensor Devices.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus temp_hum_sensor_init(void);


/**
 * @brief Read tmperature and humidity from sensor device
 *
 * @param  temperature: temperature data from sensor
 * @param  humidity:    humidity data from sensor
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus temp_hum_sensor_read(int32_t *temperature,  uint32_t *humidity);
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

