/**
 ******************************************************************************
 * @file    temp_hum_sensor.c
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    1-May-2015
 * @brief   user interfaces for temperature && humidity sensor operation,
 *          use DHT11 if bme280 not found.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "sensor/DHT11/DHT11.h"
#include "sensor/BME280/bme280_user.h"
#include "temp_hum_sensor.h"

static volatile temp_hum_sensor_type_t temp_hum_sensor_type = MICOKIT_TEMP_HUM_SENSOR_NONE;

/*---------------------------------- function --------------------------------*/

OSStatus temp_hum_sensor_init(void)
{
  OSStatus err = kUnknownErr;
  uint8_t ret = 0;

  err = bme280_sensor_init();
  if(kNoErr != err){  // bme280 not work, use DHT11
    bme280_sensor_deinit();
    
    ret = DHT11_Init();
    if(0 != ret){  // init error
      err = kNoResourcesErr;
    }
    else{
      temp_hum_sensor_type = MICOKIT_TEMP_HUM_SENSOR_DHT11;
      err = kNoErr;
    }
  }
  else{  // use bme280
    temp_hum_sensor_type = MICOKIT_TEMP_HUM_SENSOR_BME280;
  }

  return err;
}

OSStatus temp_hum_sensor_read(int32_t *temperature,  uint32_t *humidity)
{
  OSStatus err = kUnknownErr;
  uint8_t ret = 0;
  uint8_t dht11_temp = 0;
  uint8_t dht11_hum = 0;
  int32_t bme280_temp = 0;
  uint32_t bme280_hum = 0;
  uint32_t bme280_press = 0;
  
  switch(temp_hum_sensor_type){
  case MICOKIT_TEMP_HUM_SENSOR_BME280:
    {
      err = bme280_data_readout(&bme280_temp, &bme280_press, &bme280_hum);
      *temperature = bme280_temp/100;
      *humidity = bme280_hum/1024;
      break;
    }
  case MICOKIT_TEMP_HUM_SENSOR_DHT11:
    {
      ret = DHT11_Read_Data(&dht11_temp, &dht11_hum);
      if(0 != ret || dht11_hum == 0 ){
        err = kReadErr;
      }
      else{
        *temperature = (int32_t)dht11_temp;
        *humidity = (uint32_t)dht11_hum;
        err = kNoErr;
      }
      break;
    }
  default:
    err = kUnsupportedErr;
    break;
  }
  
  return err;
}
