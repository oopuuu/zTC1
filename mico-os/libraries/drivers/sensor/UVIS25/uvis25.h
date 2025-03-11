/**
 ******************************************************************************
 * @file    uvis25.h
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UVIS25_H
#define __UVIS25_H

/* Includes ------------------------------------------------------------------*/

#include "mico_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MiCO_Sensor_Driver
  * @{
  */

  
/** @defgroup MiCO_uvis25_Driver MiCO uvis25 Driver
  * @brief Provide driver interface uvis25 Sensor 
  * @{
  */
  
#ifndef UVIS25_I2C_PORT
#define UVIS25_I2C_PORT      MICO_I2C_NONE
#endif
  
#define UVIS25_WHO_AM_I                        0x0F
#define UVIS25_CTRL_REG1                       0x20 
#define UVIS25_CTRL_REG2                       0x21
#define UVIS25_CTRL_REG3                       0x22
#define UVIS25_INT_CFG                         0x23
#define UVIS25_INT_SOURCE                      0x24
#define UVIS25_THS_UV                          0x25
#define UVIS25_STATUS_REG                      0x27
#define UVIS25_UV_OUT_REG                      0x28  
  
  


/**
 * @brief Initialize uvis25 sensor device
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus uvis25_sensor_init(void);


/**
 * @brief Read value of uvis25 sensor device
 *
 * @param uv_index: ultraviolet rays value 
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus uvis25_Read_Data(float *uv_index);


/**
 * @brief Deinitialize uvis25 sensor device
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus uvis25_sensor_deinit(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif  /* __UVIS25_H */
