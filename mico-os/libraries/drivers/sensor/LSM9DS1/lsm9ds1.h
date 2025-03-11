/**
 ******************************************************************************
 * @file    uvis25.h
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LSM9DS1_H
#define __LSM9DS1_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "mico_platform.h"
#include "platform.h"
  
/** @addtogroup MICO_Drivers_interface
  * @{
  */
  
/** @addtogroup MiCO_Sensor_Driver
  * @{
  */

/** @defgroup MiCO_lsm9ds1_Driver MiCO lsm9ds1 Driver
  * @brief Provide driver interface for lsm9ds1 Sensor 
  * @{
  */

#ifndef LSM9DS1_I2C_PORT
  #define LSM9DS1_I2C_PORT      MICO_I2C_NONE
#endif

/************************************************/
/* 	Magnetometer Section  *******************/		 	  
/************************************************/
/* Magnetometer Sensor Full Scale */
#define LSM9DS1_MAG_FS_MASK		(0x60)
#define LSM9DS1_MAG_FS_4G		(0x00)	/* Full scale 4 Gauss */
#define LSM9DS1_MAG_FS_8G		(0x20)	/* Full scale 8 Gauss */
#define LSM9DS1_MAG_FS_12G		(0x40)	/* Full scale 10 Gauss */
#define LSM9DS1_MAG_FS_16G		(0x60)	/* Full scale 16 Gauss */

/* ODR */
#define ODR_MAG_MASK			(0X1C)	/* Mask for odr change on mag */
#define LSM9DS1_MAG_ODR0_625		(0x00)	/* 0.625Hz output data rate */
#define LSM9DS1_MAG_ODR1_25		(0x04)	/* 1.25Hz output data rate */
#define LSM9DS1_MAG_ODR2_5		(0x08)	/* 2.5Hz output data rate */
#define LSM9DS1_MAG_ODR5		(0x0C)	/* 5Hz output data rate */
#define LSM9DS1_MAG_ODR10		(0x10)	/* 10Hz output data rate */
#define LSM9DS1_MAG_ODR20		(0x14)	/* 20Hz output data rate */
#define LSM9DS1_MAG_ODR40		(0x18)	/* 40Hz output data rate */
#define LSM9DS1_MAG_ODR80		(0x1C)	/* 80Hz output data rate */

#define MAG_ENABLE_ON_INPUT_OPEN 	0
#define LSM9DS1_MAG_MIN_POLL_PERIOD_MS	13
#define LSM9DS1_INT_M_GPIO_DEF		(-1)
#define LSM9DS1_M_POLL_INTERVAL_DEF	(100)
  
/**********************************************/
/* 	Accelerometer section defines	 	*/
/**********************************************/
#define LSM9DS1_ACC_MIN_POLL_PERIOD_MS	1

/* Accelerometer Sensor Full Scale */
#define LSM9DS1_ACC_FS_MASK		(0x18)
#define LSM9DS1_ACC_FS_2G 		(0x00)	/* Full scale 2g */
#define LSM9DS1_ACC_FS_4G 		(0x08)	/* Full scale 4g */
#define LSM9DS1_ACC_FS_8G 		(0x10)	/* Full scale 8g */

/* Accelerometer Anti-Aliasing Filter */
#define LSM9DS1_ACC_BW_408		(0X00)
#define LSM9DS1_ACC_BW_211		(0X01)
#define LSM9DS1_ACC_BW_105		(0X02)
#define LSM9DS1_ACC_BW_50		(0X03)
#define LSM9DS1_ACC_BW_MASK		(0X03)

#define LSM9DS1_INT1_GPIO_DEF		(-1)
#define LSM9DS1_INT2_GPIO_DEF		(-1)

#define LSM9DS1_ACC_ODR_OFF		(0x00)
#define LSM9DS1_ACC_ODR_MASK		(0xE0)
#define LSM9DS1_ACC_ODR_10		(0x20)
#define LSM9DS1_ACC_ODR_50		(0x40)
#define LSM9DS1_ACC_ODR_119		(0x60)
#define LSM9DS1_ACC_ODR_238		(0x80)
#define LSM9DS1_ACC_ODR_476		(0xA0)
#define LSM9DS1_ACC_ODR_952		(0xC0)

/**********************************************/
/* 	Gyroscope section defines	 	*/
/**********************************************/
#define LSM9DS1_GYR_MIN_POLL_PERIOD_MS	1

#define LSM9DS1_GYR_FS_MASK		(0x18)
#define LSM9DS1_GYR_FS_245DPS		(0x00)
#define LSM9DS1_GYR_FS_500DPS		(0x08)
#define LSM9DS1_GYR_FS_2000DPS		(0x18)

#define LSM9DS1_GYR_ODR_OFF		(0x00)
#define LSM9DS1_GYR_ODR_MASK		(0xE0)
#define LSM9DS1_GYR_ODR_14_9		(0x20)
#define LSM9DS1_GYR_ODR_59_5		(0x40)
#define LSM9DS1_GYR_ODR_119		(0x60)
#define LSM9DS1_GYR_ODR_238		(0x80)
#define LSM9DS1_GYR_ODR_476		(0xA0)
#define LSM9DS1_GYR_ODR_952		(0xC0)

#define LSM9DS1_GYR_BW_0		(0x00)
#define LSM9DS1_GYR_BW_1		(0x01)
#define LSM9DS1_GYR_BW_2		(0x02)
#define LSM9DS1_GYR_BW_3		(0x03)

#define LSM9DS1_GYR_POLL_INTERVAL_DEF	(100)
#define LSM9DS1_ACC_POLL_INTERVAL_DEF	(100)

  

/**
 * @brief Initialize 3D magnetometer function of lsm9ds1 sensor device
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus lsm9ds1_mag_sensor_init(void);


/**
 * @brief Read magnetometer value of lsm9ds1 sensor device
 *
 * @param  MAG_X : magnetometer value of X axis 
 * @param  MAG_Y : magnetometer value of Y axis 
 * @param  MAG_Z : magnetometer value of Z axis 
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus lsm9ds1_mag_read_data(int16_t *MAG_X, int16_t *MAG_Y, int16_t *MAG_Z);



/**
 * @brief Deinitialize 3D magnetometer function of lsm9ds1 sensor device
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus lsm9ds1_mag_sensor_deinit(void);


/**
 * @brief Initialize 3D  accelerometer and gyroscope function of lsm9ds1 sensor device
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus lsm9ds1_acc_gyr_sensor_init(void);


/**
 * @brief Read accelerometer value of lsm9ds1 sensor device
 *
 * @param  ACC_X : accelerometer value of X axis 
 * @param  ACC_Y : accelerometer value of Y axis 
 * @param  ACC_Z : accelerometer value of Z axis 
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus lsm9ds1_acc_read_data(int16_t *ACC_X, int16_t *ACC_Y, int16_t *ACC_Z);


/**
 * @brief Read gyroscope value of lsm9ds1 sensor device
 *
 * @param  GYR_X : gyroscope value of X axis 
 * @param  GYR_Y : gyroscope value of Y axis 
 * @param  GYR_Z : gyroscope value of Z axis 
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus lsm9ds1_gyr_read_data(int16_t *GYR_X, int16_t *GYR_Y, int16_t *GYR_Z);



/**
 * @brief Deinitialize 3D  accelerometer and gyroscope function of lsm9ds1 sensor device
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
OSStatus lsm9ds1_acc_gyr_sensor_deinit(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif  /* __LSM9DS1_H */
