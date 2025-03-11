/**
 ******************************************************************************
 * @file    dc_motor.h
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    1-May-2015
 * @brief   dc motor operation.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __DC_MOTOR_H_
#define __DC_MOTOR_H_

#include "platform.h"

/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @defgroup MiCO_Motor_Driver MiCO Motor Driver
  * @brief Provide driver interface for Motor Devices 
  * @{
  */
  
/** @addtogroup MiCO_Motor_Driver
  * @{
  */

/** @defgroup MiCO_DC_Motor_Driver MiCO DC Motor Driver
  * @brief Provide driver interface for DC Motor
  * @{
  */

#ifndef DC_MOTOR
#define DC_MOTOR (MICO_GPIO_NONE)
#endif


//------------------------------ user interfaces -------------------------------


/**
 * @brief Initialize motor device.
 *
 * @return          0   : on success.
 * @return   error code : if an error occurred
 */
int dc_motor_init(void);

/**
 * @brief  Set motor state, ON or OFF.
 *
 * @param  value: 0,motor OFF; others��motor ON
 *
 * @return        0     : on success.
 * @return   error code : if an error occurred
 */
int dc_motor_set(int value);    

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif  // __DC_MOTOR_H_
