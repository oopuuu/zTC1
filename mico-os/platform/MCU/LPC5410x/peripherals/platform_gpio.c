/**
 ******************************************************************************
 * @file    MicoDriverGpio.c
 * @author  Magicoe.Niu
 * @version V0.0.1
 * @date    05-May-2015
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
#include "chip.h"
#include "power_control.h"
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

OSStatus platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config )
{
  	uint32_t          gpio_cfg_value;
  	OSStatus          err = kNoErr;
	uint32_t port, pin;
   platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  	port = gpio->port , pin = gpio->pin_number;

  	// GPIO are always func0, need digital enabled, and defaults to no need input filter
	gpio_cfg_value = IOCON_FUNC0 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF;


	if (config == INPUT_PULL_UP || config == OUTPUT_OPEN_DRAIN_PULL_UP)
		gpio_cfg_value |= IOCON_MODE_PULLUP;
	else if (config == INPUT_PULL_DOWN )
		gpio_cfg_value |= IOCON_MODE_PULLDOWN;
	else if (config == INPUT_HIGH_IMPEDANCE || config == OUTPUT_PUSH_PULL)
		gpio_cfg_value |= IOCON_MODE_INACT;


	if (port == 0 && (pin >=23 && pin <= 28)) {
		// I2C pins
		gpio_cfg_value |= IOCON_GPIO_MODE;
	} else {
		// non-I2C pins, process open drain settings.
		if (config == OUTPUT_OPEN_DRAIN_NO_PULL || config == OUTPUT_OPEN_DRAIN_PULL_UP)
			gpio_cfg_value |= IOCON_OPENDRAIN_EN;
	}

	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, gpio_cfg_value);
	if (config == INPUT_PULL_UP || config == INPUT_PULL_DOWN || config == INPUT_HIGH_IMPEDANCE)
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
	else
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);

exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_deinit( const platform_gpio_t* gpio )
{
	return platform_gpio_init(gpio, INPUT_HIGH_IMPEDANCE);
}

OSStatus platform_gpio_output_high( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;
  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);
  Chip_GPIO_SetPinState(LPC_GPIO, gpio->port, gpio->pin_number, 1); // Magicoe gpio->port->BSRRL = (uint16_t) ( 1 << gpio->pin_number );
exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_output_low( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;
  require_action_quiet( gpio != NULL, exit, err = kParamErr);
  platform_mcu_powersave_disable();
  Chip_GPIO_SetPinState(LPC_GPIO, gpio->port, gpio->pin_number, 0); // Magicoe gpio->port->BSRRH = (uint16_t) ( 1 << gpio->pin_number );
exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_output_trigger( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;
  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);
  Chip_GPIO_SetPinToggle(LPC_GPIO, gpio->port, gpio->pin_number); // Magicoe gpio->port->ODR ^= (uint16_t) ( 1 << gpio->pin_number );
exit:
  platform_mcu_powersave_enable();
  return err;
}

bool platform_gpio_input_get( const platform_gpio_t* gpio )
{
  bool result = false;
  platform_mcu_powersave_disable();
  require_quiet( gpio != NULL, exit);
  result =  Chip_GPIO_GetPinState(LPC_GPIO, gpio->port, gpio->pin_number);
exit:
  platform_mcu_powersave_enable();
  return result;
}


const IRQn_Type s_nvicIrqNdxTab[] =
{
	PIN_INT0_IRQn,	PIN_INT1_IRQn,	PIN_INT2_IRQn,	PIN_INT3_IRQn,
	PIN_INT4_IRQn,	PIN_INT5_IRQn,	PIN_INT6_IRQn,	PIN_INT7_IRQn,
};
#define PININT_SLOT_CNT		8
typedef struct _DS_PinIntAlloc
{
	uint8_t inUses[PININT_SLOT_CNT];
	uint8_t portPins[PININT_SLOT_CNT];
	platform_gpio_irq_callback_t pfnCBs[PININT_SLOT_CNT];
	void *pArgs[PININT_SLOT_CNT];

}DS_PinIntAlloc;

static DS_PinIntAlloc s_irqAlc;

OSStatus platform_gpio_irq_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg )
{
  OSStatus err = kNoErr;

  LPC_PIN_INT_T *pInt = LPC_PININT;
  platform_mcu_powersave_disable();

	uint32_t i, bv;
	IRQn_Type irqNdx;
  	uint32_t portPins;
  require_action_quiet( gpio != NULL, exit, err = kParamErr);

	portPins = (gpio->port << 5) + gpio->pin_number;
	// check if already registered
	for (i=0; i<PININT_SLOT_CNT; i++) {
		if (s_irqAlc.inUses[i])
		{
			if (s_irqAlc.portPins[i] == portPins)
				break;
		}
	}
	//require_action_quiet(i == PININT_SLOT_CNT, exit, err = kAlreadyInitializedErr);

  disable_interrupts();
  if(i == PININT_SLOT_CNT)
  {
      // find a free slot
      for (i=0; i<PININT_SLOT_CNT; i++) {
          if (!s_irqAlc.inUses[i])
              break;
      }
  }

	require_action_quiet(i < PININT_SLOT_CNT, exit, err = kNoResourcesErr);


  // use this slot
  bv = 1UL << i;
  irqNdx = s_nvicIrqNdxTab[i];
  NVIC_DisableIRQ(irqNdx);

  s_irqAlc.inUses[i] = 1;
  s_irqAlc.pfnCBs[i] = handler;
  s_irqAlc.pArgs[i] = arg;
  s_irqAlc.portPins[i] = (uint8_t) portPins;

  g_pInMux->PINTSEL[i] = portPins;

  pInt->IST = bv;	// clear IRQ status for PININT slot i
  pInt->ISEL &= ~(bv);	// set edge trigger
  if (trigger == IRQ_TRIGGER_RISING_EDGE)
  	pInt->SIENR = bv;
  else if (trigger == IRQ_TRIGGER_FALLING_EDGE)
  	pInt->SIENF = bv;
  else if (trigger == IRQ_TRIGGER_BOTH_EDGES)
	pInt->SIENR = bv , pInt->SIENF = bv;

  NVIC_EnableIRQ(irqNdx);

exit:
  platform_mcu_powersave_enable();
  enable_interrupts();
  return err;
}


OSStatus platform_gpio_irq_disable( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;
	uint32_t i, portPin;
  IRQn_Type irqNdx;
  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);
  disable_interrupts();
 // Check if this gpio is mapped
 	portPin = (gpio->port << 5) + gpio->pin_number;
 	for (i=0; i<PININT_SLOT_CNT; i++) {
 		if (s_irqAlc.portPins[i] == portPin)
 			break;
 	}

 	require_action_quiet(i < PININT_SLOT_CNT, exit, err = kParamErr);

	irqNdx = s_nvicIrqNdxTab[i];

 	if (s_irqAlc.inUses[i])
 	{
 		s_irqAlc.inUses[i] = 0;
 	}

 	NVIC_DisableIRQ(irqNdx);

exit:
  platform_mcu_powersave_enable();
  enable_interrupts();
  return err;
}


/******************************************************
 *      LPC54100 Internal Function Definitions
 ******************************************************/
OSStatus platform_gpio_irq_manager_init( void )
{
	Chip_PININT_Init(LPC_PININT);
  	memset(&s_irqAlc, 0, sizeof(s_irqAlc));
  	return kNoErr;
}

OSStatus platform_gpio_enable_clock( const platform_gpio_t* gpio )
{
  OSStatus    err = kNoErr;

  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_IOCON);
  if(gpio->port == 0) {
    Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_GPIO0);
  }
  if(gpio->port == 1) {
    Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_GPIO1);
  }

exit:
  return err;

}
//
//OSStatus platform_gpio_set_alternate_function( platform_gpio_port_t* gpio_port, uint8_t pin_number, platform_pin_config_t output_type, GPIOPuPd_TypeDef pull_up_down_type, uint8_t alternation_function )
//{
//    GPIO_InitTypeDef  gpio_init_structure;
//    uint8_t           port_number = platform_gpio_get_port_number( gpio_port );
//
//    // platform_mcu_powersave_disable();
//
//    if(gpio->port == 0) {
//      Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_GPIO0);
//    }
//    if(gpio->port == 1) {
//      Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_GPIO1);
//    }
//
//
//
//    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
//    gpio_init_structure.GPIO_Mode  = GPIO_Mode_AF;
//    gpio_init_structure.GPIO_OType = output_type;
//    gpio_init_structure.GPIO_PuPd  = pull_up_down_type;
//    gpio_init_structure.GPIO_Pin   = (uint32_t) ( 1 << pin_number );
//
//    GPIO_Init( gpio_port, &gpio_init_structure );
//    GPIO_PinAFConfig( gpio_port, pin_number, alternation_function );
//
//    // platform_mcu_powersave_enable();
//
//    return kNoErr;
//}

/******************************************************
 *               IRQ Handler Definitions
 ******************************************************/

void _prvPinIntHandler(uint32_t slotNdx)
{
	LPC_PIN_INT_T *pInt = LPC_PININT;

	pInt->IST = 1UL << slotNdx;	// clear IRQ flag

	// call call-back handler
	if (s_irqAlc.inUses[slotNdx]) {
		if (s_irqAlc.pfnCBs[slotNdx]) {
			s_irqAlc.pfnCBs[slotNdx](s_irqAlc.pArgs[slotNdx]);
		}
	}

}

/******************************************************
 *               IRQ Handler Mapping
 ******************************************************/
MICO_RTOS_DEFINE_ISR( PIN_INT0_IRQHandler )
{
  _prvPinIntHandler(0);
}

MICO_RTOS_DEFINE_ISR( PIN_INT1_IRQHandler )
{
  _prvPinIntHandler(1);
}

MICO_RTOS_DEFINE_ISR( PIN_INT2_IRQHandler )
{
  _prvPinIntHandler(2);
}

MICO_RTOS_DEFINE_ISR( PIN_INT3_IRQHandler )
{
  _prvPinIntHandler(3);
}

MICO_RTOS_DEFINE_ISR( PIN_INT4_IRQHandler )
{
  _prvPinIntHandler(4);
}

MICO_RTOS_DEFINE_ISR( PIN_INT5_IRQHandler )
{
  _prvPinIntHandler(5);
}

MICO_RTOS_DEFINE_ISR( PIN_INT6_IRQHandler )
{
  _prvPinIntHandler(6);
}

MICO_RTOS_DEFINE_ISR( PIN_INT7_IRQHandler )
{
  _prvPinIntHandler(7);
}



// end file --- MG.Niu ---
