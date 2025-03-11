/**
 ******************************************************************************
 * @file    lsm9ds1_acc_gyr.c
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

#define lsm9ds1_acc_gyr_log(M, ...) custom_log("LSM9DS1_ACC_GYR", M, ##__VA_ARGS__)
#define lsm9ds1_acc_gyr_log_trace() custom_log_trace("LSM9DS1_ACC_GYR")


/* TODO: check the following values */
/* Sensitivity */
#define SENSITIVITY_ACC_2G		(60)	/** ug/LSB */
#define SENSITIVITY_ACC_4G		(120)	/** ug/LSB */
#define SENSITIVITY_ACC_8G		(240)	/** ug/LSB */
#define SENSITIVITY_GYR_250		(8750)	/** udps/LSB */
#define SENSITIVITY_GYR_500		(17500)	/** udps/LSB */
#define SENSITIVITY_GYR_2000		(70000)	/** udps/LSB */

#define ACC_G_MAX_POS			(1495040)/** max positive value acc [ug] */
#define ACC_G_MAX_NEG			(1495770)/** max negative value acc [ug] */
#define MAG_G_MAX_POS			(983520)/** max positive value mag [ugauss] */
#define MAG_G_MAX_NEG			(983040)/** max negative value mag [ugauss] */
#define GYR_FS_MAX			(32768)

#define FUZZ				(0)
#define FLAT				(0)

#define FILTER_50			(50)/** Anti-Aliasing 50 Hz */
#define FILTER_105			(105)/** Anti-Aliasing 105 Hz */
#define FILTER_211			(211)/** Anti-Aliasing 211 Hz */
#define FILTER_408			(408)/** Anti-Aliasing 408 Hz */

#define RANGE_245DPS			(245)
#define RANGE_500DPS			(500)
#define RANGE_2000DPS			(2000)

#define ACT_THS				(0x04)
#define ACT_DUR				(0x05)
#define WHO_AM_I			(0x0F)
#define WHO_AM_I_VAL			(0x68)

/* Angular rate sensor Control Register 1 */
#define CTRL_REG1_G			(0x10)
#define CTRL_REG1_G_DEF                 (0x20)
#define CTRL_REG1_G_OFF                 (0x20)

#define BW_G_SHIFT			(0)
#define BW_G_MASK			(0x03)

#define FS_G_SHIFT			(3)
#define FS_G_MASK			(0x18) 

/* Angular rate sensor Control Register 2 */
#define CTRL_REG2_G			(0x11)

#define OUT_SEL_SHIFT			(0)
#define OUT_SEL_MASK			(0x03)

#define INT_SEL_SHIFT			(2)
#define INT_SEL_MASK			(0x0C)

#define SEL_LPF1			(0x00)
#define SEL_HPF				(0x01)
#define SEL_LPF2			(0x02)

#define CTRL_REG3_G			(0x12)

/* Angular rate sensor sign and orientation register. */
#define ORIENT_CFG_G			(0x13)
#define ORIENT_CFG_G_SIGN_X_MASK	(0x20)
#define ORIENT_CFG_G_SIGN_Y_MASK	(0x10)
#define ORIENT_CFG_G_SIGN_Z_MASK	(0x08)
#define ORIENT_CFG_G_SIGN_ORIENT_MASK	(0x07)

#define OUT_TEMP_L			(0x15)
#define OUT_TEMP_H			(0x16)
#define STATUS_REG1			(0x17)
#define	OUT_X_L_G			(0x18) /* 1st AXIS OUT REG of 6 */
#define	OUT_X_H_G			(0x19)
#define	OUT_Y_L_G			(0x1A)
#define	OUT_Y_H_G			(0x1B)
#define	OUT_Z_L_G			(0x1C)
#define	OUT_Z_H_G			(0x1D)

#define CTRL_REG4			(0x1E)
#define CTRL_REG4_DEF			(0x38)
#define CTRL_REG4_X_EN			(0x08)
#define CTRL_REG4_Y_EN			(0x10)
#define CTRL_REG4_Z_EN			(0x20)
#define CTRL_REG4_ALL_AXES_EN		(0x38)
#define CTRL_REG4_AXES_EN_MASK		(0x38)

#define CTRL_REG5_XL			(0x1F)
#define CTRL_REG5_XL_DEF		(0x38)

/* Linear acceleration sensor Control Register 6 */
#define CTRL_REG6_XL			(0x20)

#define LSM9DS1_ACC_FS_DEF		(LSM9DS1_ACC_FS_2G)

#define BW_SCAL_ODR_SHIFT		(2)
#define BW_SCAL_ODR_MASK		(0x04)

#define BW_XL_50			(0x0C)
#define BW_XL_105			(0x08)
#define BW_XL_211			(0x04)
#define BW_XL_OFF			(0x00)
#define BW_XL_DEF			(BW_XL_OFF)


#define CTRL_REG7_XL			(0x21)

#define CTRL_REG8			(0x22)
#define CTRL_REG8_DEF			(0x44)

#define CTRL_REG9			(0x23)
#define CTRL_REG10			(0x24)


#define STATUS_REG2			(0x27)
#define OUT_X_L_XL			(0x28) /* 1st AXIS OUT REG of 6 */
#define OUT_X_H_XL			(0x29)
#define OUT_Y_L_XL			(0x2A)
#define OUT_Y_H_XL			(0x2B)
#define OUT_Z_L_XL			(0x2C)
#define OUT_Z_H_XL			(0x2D)


#define FIFO_CTRL			(0x2E)
#define FIFO_SRC			(0x2F)

/* INT1_A/G pin control register. */
#define INT1_CTRL			(0x0C)
#define INT1_CTRL_IG_G_MASK		(0x80)
#define INT1_CTRL_IG_XL_MASK		(0x40)
#define INT1_CTRL_FSS5_MASK		(0x20)
#define INT1_CTRL_OVR_MASK		(0x10)
#define INT1_CTRL_FTH_MASK		(0x08)
#define INT1_CTRL_BOOT_MASK		(0x04)
#define INT1_CTRL_DRDY_G_MASK		(0x02)
#define INT1_CTRL_DRDY_XL_MASK		(0x01)

/* INT2_A/G pin control register. */
#define INT2_CTRL			(0x0D)
#define INT2_CTRL_INACT_MASK		(0x80)
#define INT2_CTRL_FSS5_MASK		(0x20)
#define INT2_CTRL_OVR_MASK		(0x10)
#define INT2_CTRL_FTH_MASK		(0x08)
#define INT2_CTRL_DRDY_TEMP_MASK	(0x04)
#define INT2_CTRL_DRDY_G_MASK		(0x02)
#define INT2_CTRL_DRDY_XL_MASK		(0x01)

/* Linear acceleration sensor interrupt source register. */
#define INT_GEN_SRC_XL			(0x26)
#define INT_GEN_SRC_XL_IA_MASK		(0x40)
#define INT_GEN_SRC_XL_ZH_MASK		(0x20)
#define INT_GEN_SRC_XL_ZL_MASK		(0x10)
#define INT_GEN_SRC_XL_YH_MASK		(0x08)
#define INT_GEN_SRC_XL_YL_MASK		(0x04)
#define INT_GEN_SRC_XL_XH_MASK		(0x02)
#define INT_GEN_SRC_XL_XL_MASK		(0x01)

/* Linear acceleration sensor interrupt generator configuration register. */
#define INT_GEN_CFG_XL			(0x06)
#define INT_GEN_CFG_XL_AOI_MASK	        (0x80)
#define INT_GEN_CFG_XL_6D_MASK		(0x40)
#define INT_GEN_CFG_XL_ZHIE_MASK	(0x20)
#define INT_GEN_CFG_XL_ZLIE_MASK	(0x10)
#define INT_GEN_CFG_XL_YHIE_MASK	(0x08)
#define INT_GEN_CFG_XL_YLIE_MASK	(0x04)
#define INT_GEN_CFG_XL_XHIE_MASK	(0x02)
#define INT_GEN_CFG_XL_XLIE_MASK	(0x01)

/* Linear acceleration sensor interrupt threshold registers. */
#define INT_GEN_THS_X_XL		(0x07)
#define INT_GEN_THS_Y_XL		(0x08)
#define INT_GEN_THS_Z_XL		(0x09)

/* Linear acceleration sensor interrupt duration register. */
#define INT_GEN_DUR_XL			(0x0A)
#define INT_GEN_DUR_XL_WAIT_MASK	(0x80)
#define INT_GEN_DUR_XL_DUR_MASK	        (0x7F)

/* Angular rate sensor interrupt source register. */
#define INT_GEN_SRC_G			(0x14)
#define INT_GEN_SRC_G_IA_MASK		(0x40)
#define INT_GEN_SRC_G_ZH_MASK		(0x20)
#define INT_GEN_SRC_G_ZL_MASK		(0x10)
#define INT_GEN_SRC_G_YH_MASK		(0x08)
#define INT_GEN_SRC_G_YL_MASK		(0x04)
#define INT_GEN_SRC_G_XH_MASK		(0x02)
#define INT_GEN_SRC_G_XL_MASK		(0x01)

/* Angular rate sensor interrupt generator configuration register. */
#define INT_GEN_CFG_G			(0x30)
#define INT_GEN_CFG_G_AOI_MASK		(0x80)
#define INT_GEN_CFG_G_LIR_MASK		(0x40)
#define INT_GEN_CFG_G_ZHIE_MASK	        (0x20)
#define INT_GEN_CFG_G_ZLIE_MASK	        (0x10)
#define INT_GEN_CFG_G_YHIE_MASK	        (0x08)
#define INT_GEN_CFG_G_YLIE_MASK	        (0x04)
#define INT_GEN_CFG_G_XHIE_MASK	        (0x02)
#define INT_GEN_CFG_G_XLIE_MASK	        (0x01)

/* Angular rate sensor interrupt generator threshold registers. */
#define INT_GEN_THS_XH_G		(0x31)
#define INT_GEN_THS_XL_G		(0x32)
#define INT_GEN_THS_YH_G		(0x33)
#define INT_GEN_THS_YL_G		(0x34)
#define INT_GEN_THS_ZH_G		(0x35)
#define INT_GEN_THS_ZL_G		(0x36)

/* Angular rate sensor interrupt generator duration register. */
#define INT_GEN_DUR_G			(0x37)
#define INT_GEN_DUR_G_WAIT_MASK	        (0x80)
#define INT_GEN_DUR_G_DUR_MASK		(0x7F)

#define DEF_ZERO			(0x00)
#define UNDEF				(0x00)
#define NDTEMP				(1000)	/* Not Available temperature */

/* I2C device */
mico_i2c_device_t lsm9ds1_acc_gyr_i2c_device = {
  LSM9DS1_I2C_PORT, 0x6A, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};

static OSStatus LSM9DS1_ACC_GYR_IO_Init(void)
{
  // I2C init
  MicoI2cFinalize(&lsm9ds1_acc_gyr_i2c_device);   // in case error
  MicoI2cInitialize(&lsm9ds1_acc_gyr_i2c_device);
  
  if( false == MicoI2cProbeDevice(&lsm9ds1_acc_gyr_i2c_device, 5) ){
    lsm9ds1_acc_gyr_log("LSM9DS1_ACC_GYR_ERROR: no i2c device found!");
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
static OSStatus LSM9DS1_ACC_GYR_IO_Write(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToWrite)
{
  mico_i2c_message_t lsm9ds1_acc_gyr_i2c_msg = {NULL, NULL, 0, 0, 0, false};
  OSStatus iError = kNoErr;
  uint8_t array[8];
  uint8_t stringpos;
  array[0] = RegisterAddr;
  for (stringpos = 0; stringpos < NumByteToWrite; stringpos++) {
    array[stringpos + 1] = *(pBuffer + stringpos);
  }
  
  iError = MicoI2cBuildTxMessage(&lsm9ds1_acc_gyr_i2c_msg, array, NumByteToWrite + 1, 3);
  iError = MicoI2cTransfer(&lsm9ds1_acc_gyr_i2c_device, &lsm9ds1_acc_gyr_i2c_msg, 1);
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
static OSStatus LSM9DS1_ACC_GYR_IO_Read(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToRead)
{
  mico_i2c_message_t lsm9ds1_acc_gyr_i2c_msg = {NULL, NULL, 0, 0, 0, false};
  OSStatus iError = kNoErr;
  uint8_t array[8] = {0};
  array[0] = RegisterAddr;
  
  iError = MicoI2cBuildCombinedMessage(&lsm9ds1_acc_gyr_i2c_msg, array, pBuffer, 1, NumByteToRead, 3);
  if(kNoErr != iError){
    return kReadErr; 
  }
  iError = MicoI2cTransfer(&lsm9ds1_acc_gyr_i2c_device, &lsm9ds1_acc_gyr_i2c_msg, 1);
  if(kNoErr != iError){
    return kReadErr;
  }
  return kNoErr;
}

static OSStatus LSM9DS1_ACC_POWER_ON(void)
{
  OSStatus err = kNoErr;
  uint8_t temp = 0;
  
  temp = BW_XL_50;
  if((err = LSM9DS1_ACC_GYR_IO_Write(&temp, CTRL_REG6_XL, 1)) != kNoErr){
    return err;
  }
  
  return err;
}

static OSStatus LSM9DS1_GYR_POWER_ON(void)
{
  OSStatus err = kNoErr;
  uint8_t temp = 0;
  
  temp = CTRL_REG1_G_DEF;
  if((err = LSM9DS1_ACC_GYR_IO_Write(&temp, CTRL_REG1_G, 1)) != kNoErr){
    return err;
  }
  
  return err;
}

static OSStatus LSM9DS1_GYR_POWER_OFF(void)
{
  OSStatus err = kNoErr;
  uint8_t temp = 0;
  
  temp = LSM9DS1_ACC_ODR_OFF;
  if((err = LSM9DS1_ACC_GYR_IO_Write(&temp, CTRL_REG6_XL, 1)) != kNoErr){
    return err;
  }
  
  return err;
}

static OSStatus LSM9DS1_ACC_POWER_OFF(void)
{
  OSStatus err = kNoErr;
  uint8_t temp = 0;
  
  temp = CTRL_REG1_G_OFF;
  if((err = LSM9DS1_ACC_GYR_IO_Write(&temp, CTRL_REG1_G, 1)) != kNoErr){
    return err;
  }
  
  return err;
}

static OSStatus LSM9DS1_ACC_GYR_Init(void)
{
  OSStatus err = kNoErr;
  
  if((err = LSM9DS1_ACC_GYR_IO_Init()) != kNoErr){
    return err;
  }
  
  if((err = LSM9DS1_ACC_POWER_ON()) != kNoErr){
    return err;
  }
  
  if((err = LSM9DS1_GYR_POWER_ON()) != kNoErr){
    return err;
  }
  
  return err;
}

static OSStatus LSM9DS1_ACC_GET_XYZ(int16_t *ACC_X, int16_t *ACC_Y, int16_t *ACC_Z)
{
  OSStatus err = kNoErr;
  uint8_t temp[6] = {0};
  
  if((err = LSM9DS1_ACC_GYR_IO_Read(&temp[0], OUT_X_L_XL, 6)) != kNoErr){
    return err;
  }
  *ACC_X = (int32_t)(((int16_t)temp[1] << 8) | (temp[0]));
  *ACC_Y = (int32_t)(((int16_t)temp[3] << 8) | (temp[2])); 
  *ACC_Z = (int32_t)(((int16_t)temp[5] << 8) | (temp[4]));
  
  
  return err;
}

static OSStatus LSM9DS1_GYR_GET_XYZ(int16_t *GYR_X, int16_t *GYR_Y, int16_t *GYR_Z)
{
  OSStatus err = kNoErr;
  uint8_t temp[6] = {0};

  if((err = LSM9DS1_ACC_GYR_IO_Read(&temp[0], OUT_X_L_G, 6)) != kNoErr){
    return err;
  }
  *GYR_X = (int32_t)(((int16_t)temp[1] << 8) | (temp[0]));
  *GYR_Y = (int32_t)(((int16_t)temp[3] << 8) | (temp[2])); 
  *GYR_Z = (int32_t)(((int16_t)temp[5] << 8) | (temp[4]));
  
  return err;
}

OSStatus lsm9ds1_acc_gyr_sensor_init(void)
{
  return LSM9DS1_ACC_GYR_Init();
}

OSStatus lsm9ds1_acc_read_data(int16_t *ACC_X, int16_t *ACC_Y, int16_t *ACC_Z)
{
  return LSM9DS1_ACC_GET_XYZ(ACC_X, ACC_Y, ACC_Z);
}

OSStatus lsm9ds1_gyr_read_data(int16_t *GYR_X, int16_t *GYR_Y, int16_t *GYR_Z)
{
  return LSM9DS1_GYR_GET_XYZ(GYR_X, GYR_Y, GYR_Z);
}

OSStatus lsm9ds1_acc_gyr_sensor_deinit(void)
{
  OSStatus err = kNoErr;
  
  if((err = LSM9DS1_ACC_POWER_OFF()) != kNoErr){
    return err;
  }
  
  if((err = LSM9DS1_GYR_POWER_OFF()) != kNoErr){
    return err;
  }
  
  return MicoI2cFinalize(&lsm9ds1_acc_gyr_i2c_device);
}
