/**
 ******************************************************************************
 * @file    dc_motor.c
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

#include "mico_platform.h"
#include "dc_motor.h"

#define dc_motor_log(M, ...) custom_log("DC_MOTOR", M, ##__VA_ARGS__)
#define dc_motor_log_trace() custom_log_trace("DC_MOTOR")

 
/*------------------------------ USER INTERFACES -----------------------------*/

int dc_motor_init(void)
{
  return MicoGpioInitialize( DC_MOTOR, OUTPUT_PUSH_PULL );
}

int dc_motor_set(int value)
{
  if (0 == value) {
    MicoGpioOutputLow( DC_MOTOR );
  } 
  else {
    MicoGpioOutputHigh( DC_MOTOR );
  }
  
  return 0;
}
