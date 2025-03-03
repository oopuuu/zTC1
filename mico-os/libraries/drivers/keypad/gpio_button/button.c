/**
 ******************************************************************************
 * @file    keys.c
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    1-May-2015
 * @brief   user keys operation.
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
#include "button.h"

#define keys_log(M, ...) custom_log("USER_KEYS", M, ##__VA_ARGS__)
#define keys_log_trace() custom_log_trace("USER_KEYS")

/*-------------------------------- VARIABLES ---------------------------------*/

/*------------------------------ USER INTERFACES -----------------------------*/

typedef struct _button_context_t{
  mico_gpio_t gpio;
  int timeout;
  mico_timer_t _user_button_timer;
  button_pressed_cb pressed_func;
  button_long_pressed_cb long_pressed_func;
  uint32_t start_time;
} button_context_t;

static button_context_t context[5];

//typedef void (*_button_irq_handler)( void* arg );

static void button_irq_handler( void* arg )
{
  button_context_t *_context = arg;

  int interval = -1;
  
  if ( MicoGpioInputGet( _context->gpio ) == 0 ) {
    MicoGpioEnableIRQ( _context->gpio, IRQ_TRIGGER_RISING_EDGE, button_irq_handler, _context );
    _context->start_time = mico_rtos_get_time()+1;
    mico_rtos_start_timer(&_context->_user_button_timer);
  } else {
    interval = (int)mico_rtos_get_time() + 1 - _context->start_time ;
    if ( (_context->start_time  != 0) && interval > 50 && interval < _context->timeout){
      /* button clicked once */
      if( _context->pressed_func != NULL )
        (_context->pressed_func)();
    }
    MicoGpioEnableIRQ( _context->gpio, IRQ_TRIGGER_FALLING_EDGE, button_irq_handler, _context );
    mico_rtos_stop_timer(&_context->_user_button_timer);
    _context->start_time  = 0;
  }
}

void (*button_irq_handler_array[5])() = {button_irq_handler, button_irq_handler, button_irq_handler, button_irq_handler, button_irq_handler};


static void button_timeout_handler( void* arg )
{
  button_context_t *_context = arg;

  _context->start_time = 0;
  if( _context->long_pressed_func != NULL )
    (_context->long_pressed_func)();
}

void (*button_timeout_handler_array[5])() = {button_timeout_handler, button_timeout_handler, button_timeout_handler, button_timeout_handler, button_timeout_handler};



void button_init( int index, button_init_t init)
{
  context[index].gpio = init.gpio;
  context[index].start_time = 0;
  context[index].timeout = init.long_pressed_timeout;
  context[index].pressed_func = init.pressed_func;
  context[index].long_pressed_func = init.long_pressed_func;

  #ifdef NUCLEO_F412ZG_EASYLINK_BUTTON
	  MicoGpioInitialize( init.gpio, INPUT_PULL_DOWN );
	  MicoGpioEnableIRQ( init.gpio, IRQ_TRIGGER_RISING_EDGE, button_irq_handler_array[index], &context[index] );
  #else
	  MicoGpioInitialize( init.gpio, INPUT_PULL_UP );
	  MicoGpioEnableIRQ( init.gpio, IRQ_TRIGGER_FALLING_EDGE, button_irq_handler_array[index], &context[index] );
	#endif
  mico_rtos_init_timer( &context[index]._user_button_timer, init.long_pressed_timeout, button_timeout_handler_array[index], &context[index] );
}




