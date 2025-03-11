/**
 ******************************************************************************
 * @file    micokit_ext.h
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    8-May-2015
 * @brief   micokit extension board peripherals operations..
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __MICOKIT_EXT_H_
#define __MICOKIT_EXT_H_

#include "common.h"

//------------------------- MicoKit-EXT board modules drivers ------------------
#include "rgb_led/P9813/rgb_led.h"
#include "display/VGM128064/oled.h"
#include "motor/dc_motor/dc_motor.h"
#include "keypad/gpio_button/button.h"

#include "sensor/light_adc/light_sensor.h"
#include "sensor/APDS9930/APDS9930.h"
#include "sensor/infrared_adc/infrared_reflective.h"


#include "motion_sensor.h"
#include "temp_hum_sensor.h"


/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @defgroup MICOKIT_EXT_Driver MiCOKit Ext Driver
  * @brief Provide driver interface for MiCOKit Ext devices
  * @{
  */
   
/** @addtogroup MICOKIT_EXT_Driver
  * @{
  */

/** @defgroup MICOKIT_EXT_Driver MiCOKit Ext Driver
  * @brief Provide device init driver interface for MiCOKit Ext or modules
  * @{
  */

//--------------------------- MicoKit-EXT board info ---------------------------
#define DEV_KIT_MANUFACTURER    "MXCHIP"
#define DEV_KIT_NAME            "MiCOKit3288"

#define MFG_TEST_MAX_MODULE_NUM      3


/**
 * @brief    MicoKit-EXT board init
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus micokit_ext_init(void);    


/**
 * @brief    Init modules on MicoKit-EXT board.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus user_modules_init(void);   
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
#endif  // __MICOKIT_EXT_H_
