/**
 ******************************************************************************
 * @file    infrared_reflective.h
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    1-May-2015
 * @brief   infrared reflective sensor operations.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __INFRARED_REFLECTIVE_H_
#define __INFRARED_REFLECTIVE_H_


#include "platform.h"

/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MiCO_Sensor_Driver
  * @{
  */

/** @defgroup MiCO_Infrared_Reflective_Driver MiCO Infrared Reflective Driver
  * @brief Provide driver interface for Infrared Reflective Sensor 
  * @{
  */

//--------------------------------  pin defines --------------------------------
#ifndef INFARAED_REFLECTIVE_ADC
  #define INFARAED_REFLECTIVE_ADC                 MICO_ADC_NONE  // ADC1_1 (PA1)
#endif

#define INFARAED_REFLECTIVE_ADC_SAMPLE_CYCLE    3

//------------------------------ user interfaces -------------------------------



/**
 * @brief Initialize infrared reflective sensor device
 *
 * @return   0   : on success.
 * @return   1   : if an error occurred
 */
int infrared_reflective_init(void);


/**
 * @brief Read value of infrared reflective device
 * 
 * @param data:value of sensor
 *
 * @return   0   : on success.
 * @return   -1  : if an error occurred
 */
int infrared_reflective_read(uint16_t *data);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif  // __INFRARED_REFLECTIVE_H_
