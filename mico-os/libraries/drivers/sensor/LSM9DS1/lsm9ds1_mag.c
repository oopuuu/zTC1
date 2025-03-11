/**
 ******************************************************************************
 * @file    lsm9ds1_mag.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief
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

/* Includes ------------------------------------------------------------------*/
#include "lsm9ds1.h"

#define lsm9ds1_mag_log(M, ...) custom_log("LSM9DS1_MAG", M, ##__VA_ARGS__)
#define lsm9ds1_mag_log_trace() custom_log_trace("LSM9DS1_MAG")


/* Address registers */
#define REG_WHOAMI_ADDR		        (0x0F)
#define CTRL_REG1_M			(0x20)
#define CTRL_REG2_M			(0x21)
#define CTRL_REG3_M			(0x22)
#define CTRL_REG4_M			(0x23)
#define CTRL_REG5_M			(0x24)
#define INT_CFG_M			(0x30)
#define INT_THS_L			(0x32)
#define INT_THS_H			(0x33)

#define REG_MAG_OUT_X_L_ADDR		(0x28) /** Mag. data low address register */
#define REG_MAG_OUT_X_H_ADDR		(0x29)
#define REG_MAG_OUT_Y_L_ADDR		(0x2A)
#define REG_MAG_OUT_Y_H_ADDR		(0x2B)
#define REG_MAG_OUT_Z_L_ADDR		(0x2C)
#define REG_MAG_OUT_Z_H_ADDR		(0x2D)

/* Sensitivity */
#define SENSITIVITY_MAG_4G		146156 /**	ngauss/LSB	*/
#define SENSITIVITY_MAG_8G		292312 /**	ngauss/LSB	*/
#define SENSITIVITY_MAG_12G		430000 /**	ngauss/LSB	*/
#define SENSITIVITY_MAG_16G		584454 /**	ngauss/LSB	*/

/* Magnetic sensor mode */
#define CTRL_REG3_M_MD_MASK		(0x03)
#define CTRL_REG3_M_MD_OFF		(0x02)
#define CTRL_REG3_M_MD_CONTINUOUS	(0x00)
#define CTRL_REG3_M_MD_SINGLE		(0x01)

/* X and Y axis operative mode selection */
#define X_Y_PERFORMANCE_MASK		(0x60)
#define X_Y_LOW_PERFORMANCE		(0x00)
#define X_Y_MEDIUM_PERFORMANCE		(0x20)
#define X_Y_HIGH_PERFORMANCE		(0x40)
#define X_Y_ULTRA_HIGH_PERFORMANCE	(0x60)

/* Z axis operative mode selection */
#define Z_PERFORMANCE_MASK		(0x0c)
#define Z_LOW_PERFORMANCE		(0x00)
#define Z_MEDIUM_PERFORMANCE		(0x04)
#define Z_HIGH_PERFORMANCE		(0x08)
#define Z_ULTRA_HIGH_PERFORMANCE	(0x0c)

/* Default values loaded in probe function */
#define DEF_ZERO			(0x00)

#define WHOIAM_VALUE			(0x3D)
#define CTRL_REG1_M_DEF		        (0x60)
#define CTRL_REG2_M_DEF		        DEF_ZERO
#define CTRL_REG3_M_DEF	        	CTRL_REG3_M_MD_CONTINUOUS
#define CTRL_REG4_M_DEF	        	DEF_ZERO
#define CTRL_REG5_M_DEF		        (0x40)
#define INT_CFG_M_DEF			DEF_ZERO
#define INT_THS_H_DEF			DEF_ZERO
#define INT_THS_L_DEF			DEF_ZERO

/* I2C device */
mico_i2c_device_t lsm9ds1_mag_i2c_device = {
  LSM9DS1_I2C_PORT, 0x1C, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};

static OSStatus LSM9DS1_MAG_IO_Init(void)
{
  // I2C init
  MicoI2cFinalize(&lsm9ds1_mag_i2c_device);   // in case error
  MicoI2cInitialize(&lsm9ds1_mag_i2c_device);
  
  if( false == MicoI2cProbeDevice(&lsm9ds1_mag_i2c_device, 5) ){
    lsm9ds1_mag_log("LSM9DS1_MAG_ERROR: no i2c device found!");
    return kNotInitializedErr;
  }
  return kNoErr;
}


/*	\Brief: The function is used as I2C bus write
*	\Return : Status of the I2C write
*	\param dev_addr : The device address of the sensor
*	\param reg_addr : Address of the first register, will data is going to be written
*	\param reg_data : It is a value hold in the array,
*		will be used for write the value into the register
*	\param cnt : The no of byte of data to be write
*/
static OSStatus LSM9DS1_MAG_IO_Write(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToWrite)
{
  mico_i2c_message_t lsm9ds1_mag_i2c_msg = {NULL, NULL, 0, 0, 0, false};
  OSStatus iError = kNoErr;
  uint8_t array[8];
  uint8_t stringpos;
  array[0] = RegisterAddr;
  for (stringpos = 0; stringpos < NumByteToWrite; stringpos++) {
    array[stringpos + 1] = *(pBuffer + stringpos);
  }
  
  iError = MicoI2cBuildTxMessage(&lsm9ds1_mag_i2c_msg, array, NumByteToWrite + 1, 3);
  iError = MicoI2cTransfer(&lsm9ds1_mag_i2c_device, &lsm9ds1_mag_i2c_msg, 1);
  if(kNoErr != iError){
    iError = kWriteErr;
  }
  
  return kNoErr;
}

/*	\Brief: The function is used as I2C bus read
*	\Return : Status of the I2C read
*	\param dev_addr : The device address of the sensor
*	\param reg_addr : Address of the first register, will data is going to be read
*	\param reg_data : This data read from the sensor, which is hold in an array
*	\param cnt : The no of byte of data to be read
*/
static OSStatus LSM9DS1_MAG_IO_Read(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToRead)
{
  mico_i2c_message_t lsm9ds1_mag_i2c_msg = {NULL, NULL, 0, 0, 0, false};
  OSStatus iError = kNoErr;
  uint8_t array[8] = {0};
  array[0] = RegisterAddr;
  
  iError = MicoI2cBuildCombinedMessage(&lsm9ds1_mag_i2c_msg, array, pBuffer, 1, NumByteToRead, 3);
  if(kNoErr != iError){
    return kReadErr; 
  }
  iError = MicoI2cTransfer(&lsm9ds1_mag_i2c_device, &lsm9ds1_mag_i2c_msg, 1);
  if(kNoErr != iError){
    return kReadErr;
  }
  return kNoErr;
}

static OSStatus LSM9DS1_MAG_POWER_ON(void)
{
  OSStatus err = kNoErr;
  uint8_t temp = 0;
  
  temp = CTRL_REG1_M_DEF;
  if((err = LSM9DS1_MAG_IO_Write(&temp, CTRL_REG1_M, 1)) != kNoErr){
    return err;
  }
  
  temp = CTRL_REG3_M_DEF;
  if((err = LSM9DS1_MAG_IO_Write(&temp, CTRL_REG3_M, 1)) != kNoErr){
    return err;
  }
  
  temp = CTRL_REG5_M_DEF;
  if((err = LSM9DS1_MAG_IO_Write(&temp, CTRL_REG5_M, 1)) != kNoErr){
    return err;
  }
  
  return err;
}

static OSStatus LSM9DS1_MAG_POWER_OFF(void)
{
  OSStatus err = kNoErr;
  uint8_t temp = 0;
  
  temp = CTRL_REG3_M_MD_OFF;
  if((err = LSM9DS1_MAG_IO_Write(&temp, CTRL_REG3_M, 1)) != kNoErr){
    return err;
  }
  
  return err;
}

static OSStatus LSM9DS1_MAG_Init(void)
{
  OSStatus err = kNoErr;
  
  if((err = LSM9DS1_MAG_IO_Init()) != kNoErr){
    return err;
  }
  
  if((err = LSM9DS1_MAG_POWER_ON()) != kNoErr){
    return err;
  }
  
  return err;
}

static OSStatus LSM9DS1_MAG_GET_XYZ(int16_t *MAG_X, int16_t *MAG_Y, int16_t *MAG_Z)
{
  OSStatus err = kNoErr;
  uint8_t temp[6] = {0};
  
  if((err = LSM9DS1_MAG_IO_Read(&temp[0], REG_MAG_OUT_X_L_ADDR, 1)) != kNoErr){
    return err;
  }
  
  if((err = LSM9DS1_MAG_IO_Read(&temp[1], REG_MAG_OUT_X_H_ADDR, 1)) != kNoErr){
    return err;
  }
  
  *MAG_X = (int32_t)(((int16_t)temp[1] << 8) | (temp[0]));
  
  if((err = LSM9DS1_MAG_IO_Read(&temp[2], REG_MAG_OUT_Y_L_ADDR, 1)) != kNoErr){
    return err;
  }
  
  if((err = LSM9DS1_MAG_IO_Read(&temp[3], REG_MAG_OUT_Y_H_ADDR, 1)) != kNoErr){
    return err;
  }
  
  *MAG_Y = (int32_t)(((int16_t)temp[3] << 8) | (temp[2])); 
  
  if((err = LSM9DS1_MAG_IO_Read(&temp[4], REG_MAG_OUT_Z_L_ADDR, 1)) != kNoErr){
    return err;
  }
  
  if((err = LSM9DS1_MAG_IO_Read(&temp[5], REG_MAG_OUT_Z_H_ADDR, 1)) != kNoErr){
    return err;
  }
  
  *MAG_Z = (int32_t)(((int16_t)temp[5] << 8) | (temp[4]));
  
  return err;
}

OSStatus lsm9ds1_mag_sensor_init(void)
{
  return LSM9DS1_MAG_Init();
}

OSStatus lsm9ds1_mag_read_data(int16_t *MAG_X, int16_t *MAG_Y, int16_t *MAG_Z)
{
  return LSM9DS1_MAG_GET_XYZ(MAG_X, MAG_Y, MAG_Z);
}

OSStatus lsm9ds1_mag_sensor_deinit(void)
{
  OSStatus err = kNoErr;
  
  if((err = LSM9DS1_MAG_POWER_OFF()) != kNoErr){
    return err;
  }
  
  return MicoI2cFinalize(&lsm9ds1_mag_i2c_device);
}
