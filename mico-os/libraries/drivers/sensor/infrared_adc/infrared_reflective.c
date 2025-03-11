/**
 ******************************************************************************
 * @file    infrared_reflective.h
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    1-May-2015
 * @brief   infrared reflective sensor operation.
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
#include "infrared_reflective.h"

#define infrared_reflective_log(M, ...) custom_log("INFRARED_REFLECTIVE", M, ##__VA_ARGS__)
#define infrared_reflective_log_trace() custom_log_trace("INFRARED_REFLECTIVE")

 
/*------------------------------ USER INTERFACES -----------------------------*/

int infrared_reflective_init(void)
{
  OSStatus err = kUnknownErr;
  
  err = MicoAdcInitialize(INFARAED_REFLECTIVE_ADC, INFARAED_REFLECTIVE_ADC_SAMPLE_CYCLE);
  if(kNoErr != err){
    return -1;
  }
  
  return 0;
}

int infrared_reflective_read(uint16_t *data)
{
  int ret = 0;
  OSStatus err = kUnknownErr;
  
  // init ADC
  err = MicoAdcInitialize(INFARAED_REFLECTIVE_ADC, INFARAED_REFLECTIVE_ADC_SAMPLE_CYCLE);
  if(kNoErr != err){
    return -1;
  }
  // get ADC data
  err = MicoAdcTakeSample(INFARAED_REFLECTIVE_ADC, data);
  if(kNoErr == err){
    ret = 0;   // get data succeed
  }
  else{
    ret = -1;  // get data error
  }
  
  return ret;
}
