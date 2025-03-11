/**
 ******************************************************************************
 * @file    hts221.c
 * @author  MEMS Application Team
 * @version V1.2.0
 * @date    11-February-2015
 * @brief   This file provides a set of functions needed to manage the hts221.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "hts221.h"
#include "mico.h"
#include <math.h>

#define hts221_log(M, ...) custom_log("HTS221", M, ##__VA_ARGS__)
#define hts221_log_trace() custom_log_trace("HTS221")
   


static HUM_TEMP_StatusTypeDef      HTS221_Init(HUM_TEMP_InitTypeDef *HTS221_Init);
static HUM_TEMP_StatusTypeDef      HTS221_Power_OFF(void);
static HUM_TEMP_StatusTypeDef      HTS221_ReadID(uint8_t *ht_id);
static HUM_TEMP_StatusTypeDef      HTS221_RebootCmd(void);
static HUM_TEMP_StatusTypeDef      HTS221_GetHumidity(float* pfData);
static HUM_TEMP_StatusTypeDef      HTS221_GetTemperature(float* pfData);

/* I2C device */
mico_i2c_device_t hts221_i2c_device = {
  HTS221_I2C_PORT, 0x5F, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};

HUM_TEMP_StatusTypeDef HTS221_IO_Init(void)
{
  // I2C init
  MicoI2cFinalize(&hts221_i2c_device);   // in case error
  MicoI2cInitialize(&hts221_i2c_device);
  if( false == MicoI2cProbeDevice(&hts221_i2c_device, 5) ){
    hts221_log("HTS221_ERROR: no i2c device found!");
    return HUM_TEMP_ERROR;
  }
  return HUM_TEMP_OK;
}


/*	\Brief: The function is used as I2C bus write
*	\Return : Status of the I2C write
*	\param dev_addr : The device address of the sensor
*	\param reg_addr : Address of the first register, will data is going to be written
*	\param reg_data : It is a value hold in the array,
*		will be used for write the value into the register
*	\param cnt : The no of byte of data to be write
*/
HUM_TEMP_StatusTypeDef HTS221_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr, uint16_t NumByteToWrite)
{
  mico_i2c_message_t hts221_i2c_msg = {NULL, NULL, 0, 0, 0, false};
  int iError = 0;
  uint8_t array[8];
  uint8_t stringpos;
  array[0] = RegisterAddr;
  for (stringpos = 0; stringpos < NumByteToWrite; stringpos++) {
    array[stringpos + 1] = *(pBuffer + stringpos);
  }
  
  iError = MicoI2cBuildTxMessage(&hts221_i2c_msg, array, NumByteToWrite + 1, 3);
  iError = MicoI2cTransfer(&hts221_i2c_device, &hts221_i2c_msg, 1);
  if(0 != iError){
    iError = HUM_TEMP_ERROR;
  }
  
  return (HUM_TEMP_StatusTypeDef)iError;
}

/*	\Brief: The function is used as I2C bus read
*	\Return : Status of the I2C read
*	\param dev_addr : The device address of the sensor
*	\param reg_addr : Address of the first register, will data is going to be read
*	\param reg_data : This data read from the sensor, which is hold in an array
*	\param cnt : The no of byte of data to be read
*/
HUM_TEMP_StatusTypeDef HTS221_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr, uint16_t NumByteToRead)
{
  mico_i2c_message_t hts221_i2c_msg = {NULL, NULL, 0, 0, 0, false};
  int iError = 0;
  uint8_t array[8] = {0};
  array[0] = RegisterAddr;
  
  iError = MicoI2cBuildCombinedMessage(&hts221_i2c_msg, array, pBuffer, 1, NumByteToRead, 3);
  if(0 != iError){
    return HUM_TEMP_ERROR; 
  }
  iError = MicoI2cTransfer(&hts221_i2c_device, &hts221_i2c_msg, 1);
  if(0 != iError){
    return HUM_TEMP_ERROR;
  }
  return (HUM_TEMP_StatusTypeDef)iError;
}


   
HUM_TEMP_DrvTypeDef Hts221Drv =
{
 HTS221_Init,
 HTS221_Power_OFF,
 HTS221_ReadID,
 HTS221_RebootCmd,
 0,
 0,
 0,
 0,
 0,
 HTS221_GetHumidity,
 HTS221_GetTemperature,
 NULL
};

/* ------------------------------------------------------- */ 
/* Here you should declare the variable that implements    */
/* the internal struct of extended features of HTS221.     */
/* Then you must update the NULL pointer in the variable   */
/* of the extended features below.                         */
/* See the example of LSM6DS3 in lsm6ds3.c                 */
/* ------------------------------------------------------- */ 
 
HUM_TEMP_DrvExtTypeDef Hts221Drv_ext = {
    HUM_TEMP_HTS221_COMPONENT, /* unique ID for HTS221 in the humidity and temperature driver class */
    NULL /* pointer to internal struct of extended features of HTS221 */
};


/* Temperature in degree for calibration  */
float T0_degC, T1_degC;

/* Output temperature value for calibration */
int16_t T0_out, T1_out;


/* Humidity for calibration  */
float H0_rh, H1_rh;

/* Output Humidity value for calibration */
int16_t H0_T0_out, H1_T0_out;



static HUM_TEMP_StatusTypeDef HTS221_Power_On(void);
static HUM_TEMP_StatusTypeDef HTS221_Calibration(void);



/**
 * @brief  HTS221 Calibration procedure
 * @param  None
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HTS221_Calibration(void)
{
    /* Temperature Calibration */
    /* Temperature in degree for calibration ( "/8" to obtain float) */
    uint16_t T0_degC_x8_L, T0_degC_x8_H, T1_degC_x8_L, T1_degC_x8_H;
    uint8_t H0_rh_x2, H1_rh_x2;
    uint8_t tempReg[2] = {0,0};

    if(HTS221_IO_Read(tempReg, HTS221_ADDRESS, HTS221_T0_degC_X8_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    T0_degC_x8_L = (uint16_t)tempReg[0];

    if(HTS221_IO_Read(tempReg, HTS221_ADDRESS, HTS221_T1_T0_MSB_X8_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    T0_degC_x8_H = (uint16_t) (tempReg[0] & 0x03);
    T0_degC = ((float)((T0_degC_x8_H<<8) | (T0_degC_x8_L)))/8;

    if(HTS221_IO_Read(tempReg, HTS221_ADDRESS, HTS221_T1_degC_X8_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    T1_degC_x8_L = (uint16_t)tempReg[0];

    if(HTS221_IO_Read(tempReg, HTS221_ADDRESS, HTS221_T1_T0_MSB_X8_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    T1_degC_x8_H = (uint16_t) (tempReg[0] & 0x0C);
    T1_degC_x8_H = T1_degC_x8_H >> 2;
    T1_degC = ((float)((T1_degC_x8_H<<8) | (T1_degC_x8_L)))/8;

    if(HTS221_IO_Read(tempReg, HTS221_ADDRESS, (HTS221_T0_OUT_L_ADDR | HTS221_I2C_MULTIPLEBYTE_CMD), 2) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    T0_out = ((((int16_t)tempReg[1]) << 8)+(int16_t)tempReg[0]);

    if(HTS221_IO_Read(tempReg, HTS221_ADDRESS, (HTS221_T1_OUT_L_ADDR | HTS221_I2C_MULTIPLEBYTE_CMD), 2) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    T1_out = ((((int16_t)tempReg[1]) << 8)+(int16_t)tempReg[0]);

    /* Humidity Calibration */
    /* Humidity in degree for calibration ( "/2" to obtain float) */

    if(HTS221_IO_Read(&H0_rh_x2, HTS221_ADDRESS, HTS221_H0_RH_X2_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    if(HTS221_IO_Read(&H1_rh_x2, HTS221_ADDRESS, HTS221_H1_RH_X2_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    if(HTS221_IO_Read(&tempReg[0], HTS221_ADDRESS, (HTS221_H0_T0_OUT_L_ADDR | HTS221_I2C_MULTIPLEBYTE_CMD), 2) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    H0_T0_out = ((((int16_t)tempReg[1]) << 8)+(int16_t)tempReg[0]);

    if(HTS221_IO_Read(&tempReg[0], HTS221_ADDRESS, (HTS221_H1_T0_OUT_L_ADDR  | HTS221_I2C_MULTIPLEBYTE_CMD), 2) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    H1_T0_out = ((((int16_t)tempReg[1]) << 8)+(int16_t)tempReg[0]);

    H0_rh = ((float)H0_rh_x2)/2;
    H1_rh = ((float)H1_rh_x2)/2;
    
    return HUM_TEMP_OK;
}


/**
 * @brief  Set HTS221 Initialization
 * @param  HTS221_Init the configuration setting for the HTS221
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HTS221_Init(HUM_TEMP_InitTypeDef *HTS221_Init)
{  
    uint8_t tmp = 0x00;

    /* Configure the low level interface ---------------------------------------*/
    if(HTS221_IO_Init() != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    if(HTS221_Power_On() != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    if(HTS221_Calibration() != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(HTS221_IO_Read(&tmp, HTS221_ADDRESS, HTS221_RES_CONF_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    /* Resolution Register */
    tmp &= ~(HTS221_H_RES_MASK);
    tmp |= HTS221_Init->Humidity_Resolutin;
    
    tmp &= ~(HTS221_T_RES_MASK);
    tmp |= HTS221_Init->Temperature_Resolution;

    if(HTS221_IO_Write(&tmp, HTS221_ADDRESS, HTS221_RES_CONF_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    //////////////////////////////////////////////////////////////////////////////////
    if(HTS221_IO_Read(&tmp, HTS221_ADDRESS, HTS221_CTRL_REG1_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    /* Output Data Rate selection */
    tmp &= ~(HTS221_ODR_MASK);
    tmp |= HTS221_Init->OutputDataRate;

    if(HTS221_IO_Write(&tmp, HTS221_ADDRESS, HTS221_CTRL_REG1_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    return HUM_TEMP_OK;
}

/**
 * @brief  Read ID address of HTS221
 * @param  ht_id the pointer where the ID of the device is stored
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HTS221_ReadID(uint8_t *ht_id)
{
    if(!ht_id)
    { 
      return HUM_TEMP_ERROR; 
    }
 
    return HTS221_IO_Read(ht_id, HTS221_ADDRESS, HTS221_WHO_AM_I_ADDR, 1);
}

/**
 * @brief  Reboot memory content of HTS221
 * @param  None
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HTS221_RebootCmd(void)
{
    uint8_t tmpreg;

    /* Read CTRL_REG2 register */
    if(HTS221_IO_Read(&tmpreg, HTS221_ADDRESS, HTS221_CTRL_REG2_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    /* Enable or Disable the reboot memory */
    tmpreg |= HTS221_BOOT_REBOOTMEMORY;

    /* Write value to MEMS CTRL_REG2 regsister */
    if(HTS221_IO_Write(&tmpreg, HTS221_ADDRESS, HTS221_CTRL_REG2_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    return HUM_TEMP_OK;
}


/**
 * @brief  Read HTS221 output register, and calculate the humidity
 * @param  pfData the pointer to data output
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HTS221_GetHumidity(float* pfData)
{
    int16_t H_T_out, humidity_t;
    uint8_t tempReg[2] = {0,0};
    uint8_t tmp = 0x00;
    float H_rh;
    
    if(HTS221_IO_Read(&tmp, HTS221_ADDRESS, HTS221_CTRL_REG1_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    /* Output Data Rate selection */
    tmp &= (HTS221_ODR_MASK);
    
    if(tmp == 0x00)
    {
      if(HTS221_IO_Read(&tmp, HTS221_ADDRESS, HTS221_CTRL_REG2_ADDR, 1) != HUM_TEMP_OK)
      {
        return HUM_TEMP_ERROR;
      }

      /* Serial Interface Mode selection */
      tmp &= ~(HTS221_ONE_SHOT_MASK);
      tmp |= HTS221_ONE_SHOT_START;

      if(HTS221_IO_Write(&tmp, HTS221_ADDRESS, HTS221_CTRL_REG2_ADDR, 1) != HUM_TEMP_OK)
      {
        return HUM_TEMP_ERROR;
      }
    
      do{
      
        if(HTS221_IO_Read(&tmp, HTS221_ADDRESS, HTS221_STATUS_REG_ADDR, 1) != HUM_TEMP_OK)
        {
          return HUM_TEMP_ERROR;
        }
         
      }while(!(tmp&&0x02));
    }
    
    
    if(HTS221_IO_Read(&tempReg[0], HTS221_ADDRESS, (HTS221_HUMIDITY_OUT_L_ADDR | HTS221_I2C_MULTIPLEBYTE_CMD), 2) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    H_T_out = ((((int16_t)tempReg[1]) << 8)+(int16_t)tempReg[0]);

    H_rh = ( float )(((( H_T_out - H0_T0_out ) * ( H1_rh - H0_rh )) / ( H1_T0_out - H0_T0_out )) + H0_rh );

    // Truncate to specific number of decimal digits
    humidity_t = (uint16_t)(H_rh * pow(10,HUM_DECIMAL_DIGITS));
    *pfData = ((float)humidity_t)/pow(10,HUM_DECIMAL_DIGITS);
    
    // Prevent data going below 0% and above 100% due to linear interpolation
    if ( *pfData <   0.0f ) *pfData =   0.0f;
    if ( *pfData > 100.0f ) *pfData = 100.0f;
    
    return HUM_TEMP_OK;
}

/**
 * @brief  Read HTS221 output register, and calculate the temperature
 * @param  pfData the pointer to data output
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HTS221_GetTemperature(float* pfData)
{
    int16_t T_out, temperature_t;
    uint8_t tempReg[2] = {0,0};
    uint8_t tmp = 0x00;
    float T_degC;
    
    if(HTS221_IO_Read(&tmp, HTS221_ADDRESS, HTS221_CTRL_REG1_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    /* Output Data Rate selection */
    tmp &= (HTS221_ODR_MASK);
    
    if(tmp == 0x00)
    {
      if(HTS221_IO_Read(&tmp, HTS221_ADDRESS, HTS221_CTRL_REG2_ADDR, 1) != HUM_TEMP_OK)
      {
        return HUM_TEMP_ERROR;
      }

      /* Serial Interface Mode selection */
      tmp &= ~(HTS221_ONE_SHOT_MASK);
      tmp |= HTS221_ONE_SHOT_START;

      if(HTS221_IO_Write(&tmp, HTS221_ADDRESS, HTS221_CTRL_REG2_ADDR, 1) != HUM_TEMP_OK)
      {
        return HUM_TEMP_ERROR;
      }
    
      do{
      
        if(HTS221_IO_Read(&tmp, HTS221_ADDRESS, HTS221_STATUS_REG_ADDR, 1) != HUM_TEMP_OK)
        {
          return HUM_TEMP_ERROR;
        }
       
      }while(!(tmp&&0x01));
    }

    if(HTS221_IO_Read(&tempReg[0], HTS221_ADDRESS, (HTS221_TEMP_OUT_L_ADDR | HTS221_I2C_MULTIPLEBYTE_CMD), 2) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    T_out = ((((int16_t)tempReg[1]) << 8)+(int16_t)tempReg[0]);

    T_degC = ((float)(T_out - T0_out))/(T1_out - T0_out) * (T1_degC - T0_degC) + T0_degC;

    temperature_t = (int16_t)(T_degC * pow(10,TEMP_DECIMAL_DIGITS));

    *pfData = ((float)temperature_t)/pow(10,TEMP_DECIMAL_DIGITS);
    
    return HUM_TEMP_OK;
}


/**
 * @brief  Exit the shutdown mode for HTS221
 * @param  None
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HTS221_Power_On(void)
{
    uint8_t tmpReg;

    /* Read the register content */
    if(HTS221_IO_Read(&tmpReg, HTS221_ADDRESS, HTS221_CTRL_REG1_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    /* Set the power down bit */
    tmpReg |= HTS221_MODE_ACTIVE;

    /* Write register */
    if(HTS221_IO_Write(&tmpReg, HTS221_ADDRESS, HTS221_CTRL_REG1_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    return HUM_TEMP_OK;
}

/**
 * @brief  Enter the shutdown mode for HTS221
 * @param  None
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HTS221_Power_OFF(void)
{
    uint8_t tmpReg;

    /* Read the register content */
    if(HTS221_IO_Read(&tmpReg, HTS221_ADDRESS, HTS221_CTRL_REG1_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }

    /* Reset the power down bit */
    tmpReg &= ~(HTS221_MODE_ACTIVE);

    /* Write register */
    if(HTS221_IO_Write(&tmpReg, HTS221_ADDRESS, HTS221_CTRL_REG1_ADDR, 1) != HUM_TEMP_OK)
    {
      return HUM_TEMP_ERROR;
    }
    
    return HUM_TEMP_OK;
}



OSStatus hts221_sensor_init(void)
{
  HUM_TEMP_InitTypeDef hts221;
  hts221.Data_Update_Mode = HTS221_BDU_CONTINUOUS;
  hts221.Humidity_Resolutin = HTS221_H_RES_AVG_32;
  hts221.Temperature_Resolution = HTS221_H_RES_AVG_32;
  hts221.OutputDataRate = HTS221_ODR_ONE_SHOT;
  hts221.Power_Mode = HTS221_MODE_ACTIVE;
  hts221.Reboot_Mode = HTS221_BOOT_NORMALMODE;
  
  if(HTS221_Init(&hts221) != HUM_TEMP_OK){
    return -1;
  }
  return 0;
}

OSStatus hts221_Read_Data(float *temperature,float *humidity)
{
  if(HTS221_GetTemperature(temperature) != HUM_TEMP_OK){
    return -1;
  }
  if(HTS221_GetHumidity(humidity) != HUM_TEMP_OK){
    return -1;
  }
  return 0;
}

OSStatus hts221_sensor_deinit(void)
{
  if(HTS221_Power_OFF() != HUM_TEMP_OK){
    return -1;
  }
  if(MicoI2cFinalize(&hts221_i2c_device) != HUM_TEMP_OK){
    return -1;
  }
  return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/     
