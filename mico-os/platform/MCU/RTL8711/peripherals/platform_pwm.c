/**
 ******************************************************************************
 * @file    paltform_pwm.c
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


#include "mico_rtos.h"
#include "mico_platform.h"

#include "platform.h"
#include "platform_peripheral.h"
#include "platform_logging.h"

#include "pinmap.h"
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
static const PinMap PinMap_PWM[] = {
    {PB_4,  RTL_PIN_PERI(PWM0, 0, S0), RTL_PIN_FUNC(PWM0, S0)},
    {PB_5,  RTL_PIN_PERI(PWM1, 1, S0), RTL_PIN_FUNC(PWM1, S0)},
    {PB_6,  RTL_PIN_PERI(PWM2, 2, S0), RTL_PIN_FUNC(PWM2, S0)},
    {PB_7,  RTL_PIN_PERI(PWM3, 3, S0), RTL_PIN_FUNC(PWM3, S0)},

    {PC_0,  RTL_PIN_PERI(PWM0, 0, S1), RTL_PIN_FUNC(PWM0, S1)},
    {PC_1,  RTL_PIN_PERI(PWM1, 1, S1), RTL_PIN_FUNC(PWM1, S1)},
    {PC_2,  RTL_PIN_PERI(PWM2, 2, S1), RTL_PIN_FUNC(PWM2, S1)},
    {PC_3,  RTL_PIN_PERI(PWM3, 3, S1), RTL_PIN_FUNC(PWM3, S1)},

    {PD_3,  RTL_PIN_PERI(PWM0, 0, S2), RTL_PIN_FUNC(PWM0, S2)},
    {PD_4,  RTL_PIN_PERI(PWM1, 1, S2), RTL_PIN_FUNC(PWM1, S2)},
    {PD_5,  RTL_PIN_PERI(PWM2, 2, S2), RTL_PIN_FUNC(PWM2, S2)},
    {PD_6,  RTL_PIN_PERI(PWM3, 3, S2), RTL_PIN_FUNC(PWM3, S2)},

    {PE_0,  RTL_PIN_PERI(PWM0, 0, S3), RTL_PIN_FUNC(PWM0, S3)},
    {PE_1,  RTL_PIN_PERI(PWM1, 1, S3), RTL_PIN_FUNC(PWM1, S3)},
    {PE_2,  RTL_PIN_PERI(PWM2, 2, S3), RTL_PIN_FUNC(PWM2, S3)},
    {PE_3,  RTL_PIN_PERI(PWM3, 3, S3), RTL_PIN_FUNC(PWM3, S3)},

    {NC,    NC,     0}
};
/******************************************************
*               Function Declarations
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/
OSStatus rtk_pwm_init( platform_pwm_t* pwm, uint32_t frequency, float duty_cycle )
{
  uint32_t peripheral;
  u32 pwm_idx;
  u32 pin_sel;
  float value;  
  OSStatus                err                 = kNoErr;

  // Get the peripheral name from the pin and assign it to the object
  peripheral = pinmap_peripheral(pwm->pin, PinMap_PWM);

  if (unlikely(peripheral == NC)) {
      DBG_PWM_ERR("%s: Cannot find matched pwm for this pin(0x%x)\n", __FUNCTION__, pwm->pin);
      return kParamErr;
  }

  pwm_idx = RTL_GET_PERI_IDX(peripheral);
  pin_sel = RTL_GET_PERI_SEL(peripheral);

  pwm->pwm_idx = pwm_idx;
  pwm->pin_sel = pin_sel;
  HAL_Pwm_Init(pwm_idx, pin_sel);

  if (duty_cycle < (float)0.0) {
      value = 0.0;
  }else if (duty_cycle > (float)100.0) {
      value = 1.0;
  }else{
      value = duty_cycle/100;
  }
  pwm->period = (uint32_t)((float)1000000.0/frequency);
  pwm->pulse = (uint32_t)((float)pwm->period * value);
  HAL_Pwm_SetDuty(pwm->pwm_idx, pwm->period, pwm->pulse);

  return err;
}


OSStatus platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle )
{
  OSStatus                err                 = kNoErr;
  
  require_action_quiet( pwm != NULL, exit, err = kParamErr);

  platform_mcu_powersave_disable();

  rtk_pwm_init(pwm, frequency, duty_cycle);

exit:  
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_pwm_start( const platform_pwm_t* pwm )
{
  OSStatus err = kNoErr;
  
  platform_mcu_powersave_disable();

  require_action_quiet( pwm != NULL, exit, err = kParamErr);
  
  HAL_Pwm_Enable(pwm->pwm_idx);

exit:  
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_pwm_stop( const platform_pwm_t* pwm )
{
  OSStatus err = kNoErr;
  
  platform_mcu_powersave_disable();

  require_action_quiet( pwm != NULL, exit, err = kParamErr);
  
  HAL_Pwm_Disable(pwm->pwm_idx);
  
exit:  
  platform_mcu_powersave_enable();
  return err;
}




