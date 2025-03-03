/**
 ******************************************************************************
 * @file    paltform_gpio.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide GPIO driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */


#include "mico_platform.h"
#include "mico_rtos.h"

#include "platform.h"
#include "platform_peripheral.h"
#include "platform_logging.h"

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

OSStatus rtk_gpio_init(platform_gpio_t* gpio, platform_pin_config_t config)
{
  OSStatus          err = kNoErr;

  switch(config){
  	case INPUT_PULL_UP:
		gpio->hal_pin.pin_mode = DIN_PULL_HIGH;
		break;
	case INPUT_PULL_DOWN:
		gpio->hal_pin.pin_mode = DIN_PULL_LOW;
		break;
	case INPUT_HIGH_IMPEDANCE:
		gpio->hal_pin.pin_mode = DIN_PULL_NONE;
		break;	
	case OUTPUT_PUSH_PULL:
		gpio->hal_pin.pin_mode = DOUT_PUSH_PULL;
		break;	
	case OUTPUT_OPEN_DRAIN_NO_PULL:
	case OUTPUT_OPEN_DRAIN_PULL_UP:		
		gpio->hal_pin.pin_mode = DOUT_OPEN_DRAIN;
		break;		
  }

  gpio->hal_pin.pin_name = gpio_set(gpio->pin);   

  HAL_GPIO_Init(&gpio->hal_pin);  

  return err;  
}

OSStatus rtk_gpio_deinit(platform_gpio_t* gpio)
{
  OSStatus          err = kNoErr;

  /* Set to Input high-impedance */
  gpio->hal_pin.pin_mode = DIN_PULL_NONE;

  HAL_GPIO_Init(&gpio->hal_pin);  

  return err;
}

OSStatus rtk_gpio_irq_enable(platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg )
{
  OSStatus          err = kNoErr;

  gpio->hal_pin.pin_name = HAL_GPIO_GetPinName((u32)gpio->pin);; // get the IP pin name

  switch ( trigger )
  {
    case IRQ_TRIGGER_RISING_EDGE:
    {
      gpio->hal_pin.pin_mode = INT_RISING;
      break;
    }
    case IRQ_TRIGGER_FALLING_EDGE:
    {
      gpio->hal_pin.pin_mode = INT_FALLING;
      break;
    }
    case IRQ_TRIGGER_BOTH_EDGES:
    {
      err =  kUnsupportedErr;
      goto exit;
    }
    default:
    {
      err =  kParamErr;
      goto exit;
    }
  }

    HAL_GPIO_Irq_Init(&gpio->hal_pin);
    HAL_GPIO_UserRegIrq(&gpio->hal_pin, (VOID*) handler, (VOID*) arg);
    HAL_GPIO_IntCtrl(&gpio->hal_pin, 1);
    HAL_GPIO_UnMaskIrq(&gpio->hal_pin);	

exit:
  return err;
}

OSStatus platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config )
{
  OSStatus          err = kNoErr;

  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  err = rtk_gpio_init((platform_gpio_t*)gpio, config);
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_deinit( const platform_gpio_t* gpio )
{
  OSStatus          err = kNoErr;

  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  err = rtk_gpio_deinit((platform_gpio_t*)gpio);

exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_ip_deinit(void)
{
  OSStatus          err = kNoErr;

  platform_mcu_powersave_disable();

  HAL_GPIO_IP_DeInit();

exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_output_high( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable();

  require_action_quiet( gpio != NULL, exit, err = kParamErr);

#if 1
  HAL_GPIO_WritePin((HAL_GPIO_PIN *)&gpio->hal_pin, 1);
#endif
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_output_low( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;

  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  platform_mcu_powersave_disable();

//  DBG_8195A("platform_gpio_output_low %d %d %d\r\n", gpio->pin, gpio->hal_pin.pin_name, gpio->hal_pin.pin_mode);

#if 1  
  HAL_GPIO_WritePin((HAL_GPIO_PIN *)&gpio->hal_pin, 0);
#endif
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_output_trigger( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable();

  require_action_quiet( gpio != NULL, exit, err = kParamErr);

#if 1
  if(1 == HAL_GPIO_ReadPin((HAL_GPIO_PIN *)&gpio->hal_pin)){
  	  HAL_GPIO_WritePin((HAL_GPIO_PIN *)&gpio->hal_pin, 0);
  }else{
  	  HAL_GPIO_WritePin((HAL_GPIO_PIN *)&gpio->hal_pin, 1);
  }
#endif
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

bool platform_gpio_input_get( const platform_gpio_t* gpio )
{
  bool result = false;

  platform_mcu_powersave_disable();

  require_quiet( gpio != NULL, exit);

  if(1 == HAL_GPIO_ReadPin((HAL_GPIO_PIN *)&gpio->hal_pin)){
  	result = true;
  }else{
  	result = false;
  }
  
exit:
  platform_mcu_powersave_enable();
  return result;
}

OSStatus platform_gpio_irq_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg )
{
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  err = rtk_gpio_irq_enable((platform_gpio_t*)gpio, trigger, handler, arg);
  
exit:
  platform_mcu_powersave_enable();
  return err;
}


OSStatus platform_gpio_irq_disable( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  HAL_GPIO_MaskIrq((HAL_GPIO_PIN *)&gpio->hal_pin);
  
exit:
  platform_mcu_powersave_enable();
  return err;
}


/******************************************************
 *      STM32F2xx Internal Function Definitions
 ******************************************************/

/******************************************************
 *               IRQ Handler Definitions
 ******************************************************/

/* Common IRQ handler for all GPIOs */


/******************************************************
 *               IRQ Handler Mapping
 ******************************************************/





