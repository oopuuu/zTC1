/**
 ******************************************************************************
 * @file    light_sensor.h
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    1-May-2015
 * @brief   light sensor operation.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __LIGHT_SENSOR_H_
#define __LIGHT_SENSOR_H_

#include "mico.h"
#include "platform.h"

/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MiCO_Sensor_Driver
  * @{
  */

/** @defgroup MiCO_Light_Driver MiCO Light Driver
  * @brief Provide driver interface for Light Sensor 
  * @{
  */

//--------------------------------  pin defines --------------------------------
#ifndef LIGHT_SENSOR_ADC
  #define LIGHT_SENSOR_ADC                 MICO_ADC_NONE  // ADC1_4 (PA4)
#endif

#define LIGHT_SENSOR_ADC_SAMPLE_CYCLE    3

//------------------------------ user interfaces -------------------------------

/**
 * @brief Initialize light sensor device
 *
 * @return   0   : on success.
 * @return   1   : if an error occurred
 */
int light_sensor_init(void);


/**
 * @brief Read value of light sensor device
 *
 * @param data: light value of sensor
 * 
 * @return   0   : on success.
 * @return   1   : if an error occurred
 */
int light_sensor_read(uint16_t *data);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif  // __LIGHT_SENSOR_H_
