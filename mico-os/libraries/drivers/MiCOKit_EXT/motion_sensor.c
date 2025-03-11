/**
 ******************************************************************************
 * @file    motion_sensor.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   motion sensor control demo.
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "sensor/BMA2x2/bma2x2_user.h"
#include "sensor/BMG160/bmg160_user.h"
#include "sensor/BMM050/bmm050_user.h"
#include "motion_sensor.h"

OSStatus motion_sensor_init(void)
{
  OSStatus err = kUnknownErr;
  
  /*low-g acceleration sensor init*/
  err = bma2x2_sensor_init();
  require_noerr( err, exit );
  
  /*triaxial angular rate sensor init*/
  err = bmg160_sensor_init();
  require_noerr( err, exit );
  
  /* triaxial geomagnetic sensor init*/
  err = bmm050_sensor_init();
  require_noerr( err, exit );
  
exit:
  return err;
}

OSStatus motion_sensor_readout(motion_data_t *motion_data)
{
  OSStatus err = kUnknownErr;
  
  /*low-g acceleration sensor data read*/
  err = bma2x2_data_readout(&motion_data->accel_data.accel_datax,
                            &motion_data->accel_data.accel_datay, 
                            &motion_data->accel_data.accel_dataz);
  require_noerr( err, exit );
  
  /*triaxial angular rate sensor data read*/
  err = bmg160_data_readout(&motion_data->gyro_data.gyro_datax,
                            &motion_data->gyro_data.gyro_datay,
                            &motion_data->gyro_data.gyro_dataz);
  require_noerr( err, exit );
  
  /* triaxial geomagnetic sensor data read*/
  err = bmm050_data_readout(&motion_data->mag_data.mag_dataz,
                            &motion_data->mag_data.mag_datay,
                            &motion_data->mag_data.mag_dataz);
  require_noerr( err, exit );

exit:
  return err;
}

OSStatus motion_sensor_deinit(void)
{
  OSStatus err = kUnknownErr;
  
  /*low-g acceleration sensor deinit*/
  err = bma2x2_sensor_deinit();
  require_noerr( err, exit );
  
  /*triaxial angular rate sensor deinit*/
  err = bmg160_sensor_deinit();
  require_noerr( err, exit );
  
  /* triaxial geomagnetic sensor deinit*/
  err = bmm050_sensor_deinit();
  require_noerr( err, exit );
  
exit:
  return err;
}



