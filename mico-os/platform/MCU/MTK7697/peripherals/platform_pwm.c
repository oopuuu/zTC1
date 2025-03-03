/**
 ******************************************************************************
 * @file    platform_pwm.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide PWM driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "platform.h"
#include "platform_peripheral.h"
#include "debug.h"
/******************************************************
*                    Constants
******************************************************/

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Variables Definitions
******************************************************/

/******************************************************
*               Function Declarations
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/

/**
 * Initialise PWM interface
 *
 * @param[in] pwm_interface : PWM interface
 * @param[in] frequency     : PWM signal frequency in Hz
 * @param[in] duty_cycle    : PWM signal duty cycle in percentage point
 *
 * @return @ref OSStatus
 */
OSStatus platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle )
{
  uint32_t total_count = 0;
  uint32_t duty_count = 0;

  hal_pwm_source_clock_t clk_src;
  if(frequency > 400000)
  {
  	return kGeneralErr;
  }else if(frequency > 1000){
  	clk_src = HAL_PWM_CLOCK_40MHZ;
  }else{
  	clk_src = HAL_PWM_CLOCK_32KHZ;
  }

  hal_pwm_deinit();
  hal_gpio_init(HAL_GPIO_35);
  hal_pinmux_set_function(HAL_GPIO_35, HAL_GPIO_35_PWM18);
  /*Set source clock*/
  hal_pwm_init(clk_src);
  /*Set frequency*/
  hal_pwm_set_frequency(HAL_PWM_18, frequency, &total_count);
  /*Calculate and set the duty cycle*/
  duty_count = (uint32_t)((total_count * duty_cycle) / 100);
  hal_pwm_set_duty_cycle(HAL_PWM_18, duty_count);

	return kNoErr;
}


/**
 * Start generating PWM signal on the specified PWM interface
 *
 * @param[in] pwm_interface : PWM interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_pwm_start( const platform_pwm_t* pwm )
{
	hal_pwm_start(HAL_PWM_18);
	return kNoErr;
}


/**
 * Stop generating PWM signal on the specified PWM interface
 *
 * @param[in] pwm_interface : PWM interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_pwm_stop( const platform_pwm_t* pwm )
{
	hal_pwm_stop(HAL_PWM_18);
	return kNoErr;
}

