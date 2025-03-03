/**
 ******************************************************************************
 * @file    hsb2rgb_led.h
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    17-Mar-2015
 * @brief   converts HSB color values to RGB colors to control RGB LED.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __HSB2RGB_LED_H_
#define __HSB2RGB_LED_H_


/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @defgroup MICO_LED_Driver MiCO LED Driver
  * @brief Provide driver interface for LED devices
  * @{
  */
    
/** @addtogroup MICO_LED_Driver
  * @{
  */

/** @defgroup MICO_HSB2RGB_Driver MiCO hsb2rgb Driver
  * @brief Provide driver interface for Hsb2rgb
  * @{
  */


/**
 * @brief Initialize hsb2rgb device.
 *
 * @return none
 */
void hsb2rgb_led_init(void);


/**
 * @brief Set light parameters for hsb2rgb.
 *
 * @param hues:         hues data of hsb2rgb
 * @param saturation:   saturation data of hsb2rgb
 * @param brightness:   brightness data of hsb2rgb
 *
 * @return none
 */
void hsb2rgb_led_open(float hues, float saturation, float brightness);

/**
 * @brief Close hsb2rgb
 *
 * @return none
 */
void hsb2rgb_led_close(void);

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

#endif   // __HSB2RGB_LED_H_
