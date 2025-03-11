/**
 ******************************************************************************
 * @file    apds9930.c
 * @author  William Xu
 * @version V1.0.0
 * @date    17-Mar-2015
 * @brief   apds9930 user controller operation
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mico.h"
#include "APDS9930.h"

#define apds9930_log(M, ...) custom_log("APDS9930", M, ##__VA_ARGS__)

/************** I2C/SPI buffer length ******/
#define	APDS_BUFFER_LEN 3

/* I2C device */
mico_i2c_device_t apds_i2c_device = {
  APDS9930_I2C_DEVICE, APDS9930_ID, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};

OSStatus APDS9930_I2C_bus_write(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
  OSStatus err = kNoErr;
  mico_i2c_message_t apds_i2c_msg = {NULL, NULL, 0, 0, 0, false};

  uint8_t array[APDS_BUFFER_LEN];
  uint8_t stringpos;
  array[0] = reg_addr;
  for (stringpos = 0; stringpos < cnt; stringpos++) {
          array[stringpos + 1] = *(reg_data + stringpos);
  }

  err = MicoI2cBuildTxMessage(&apds_i2c_msg, array, cnt + 1, 3);
  require_noerr( err, exit );
  err = MicoI2cTransfer(&apds_i2c_device, &apds_i2c_msg, 1);
  require_noerr( err, exit );
  
exit:  
  return err;
}

OSStatus APDS9930_I2C_bus_read(uint8_t *reg_data, uint8_t cnt)
{
  OSStatus err = kNoErr;
  mico_i2c_message_t apds_i2c_msg = {NULL, NULL, 0, 0, 0, false};

  err = MicoI2cBuildRxMessage(&apds_i2c_msg, reg_data, cnt, 3);
  require_noerr( err, exit );
  err = MicoI2cTransfer(&apds_i2c_device, &apds_i2c_msg, 1);
  require_noerr( err, exit );

exit:
  return err;
}

OSStatus APDS9930_Write_RegData(uint8_t reg_addr, uint8_t reg_data)
{
  OSStatus err = kNoErr;
  err = APDS9930_I2C_bus_write(0x80|reg_addr, &reg_data, 1);
  return err;
}

OSStatus APDS9930_Read_RegData(uint8_t reg_addr, uint8_t *reg_data)
{
  OSStatus err = kNoErr;
  err = APDS9930_I2C_bus_write(0xA0|reg_addr, NULL, 0);
  err = APDS9930_I2C_bus_read(reg_data, 1);
  return err;  
}

OSStatus APDS9930_Clear_intrtrupt( void )
{
  OSStatus err = kNoErr;
  err = APDS9930_I2C_bus_write(0x80|CLIT_ADDR, NULL, 0);
  return err;
}

void apds9930_enable()
{
  //Disable and Powerdown
  APDS9930_Write_RegData(ENABLE_ADDR, APDS9930_DISABLE); 
  APDS9930_Write_RegData(ATIME_ADDR, ATIME_256C);
  APDS9930_Write_RegData(PTIME_ADDR, PTIME_10C);
  APDS9930_Write_RegData(WTIME_ADDR, WTIME_74C);
  APDS9930_Write_RegData(CONFIG_ADDR, RECONFIG);
  APDS9930_Write_RegData(PPULSE_ADDR, PPULSE_MIN);
  //Config
  APDS9930_Write_RegData(CONTROL_ADDR,PDRIVE_100|PDIODE_CH1|PGAIN_1x|AGAIN_1x);
  //Enable APDS9930
  APDS9930_Write_RegData(ENABLE_ADDR, WEN|PEN|AEN|PON);
  //must delay > 12ms
  mico_thread_msleep(12);
}
 
OSStatus apds9930_data_readout(uint16_t *Prox_data, uint16_t *Lux_data)
{
  OSStatus err = kNoErr;
  uint8_t  CH0L_data = 0, CH0H_data = 0, CH1L_data = 0, CH1H_data = 0, ProxL_data = 0, ProxH_data = 0, status = 0; 
  uint16_t CH0_data = 0, CH1_data = 0; 
  int IAC1 = 0, IAC2 = 0, IAC = 0;
  float B = 1.862, C = 0.746, D = 1.296, ALSIT = 400, AGAIN = 1;
  float LPC = 0;
  
  err = MicoI2cInitialize(&apds_i2c_device);
  require_noerr_action( err, exit, apds9930_log("APDS9930_ERROR: MicoI2cInitialize err = %d.", err) );
  
  err = APDS9930_Read_RegData(STATUS_ADDR, &status);
  require_noerr( err, exit );
  
  err = APDS9930_Read_RegData(Ch0DATAL_ADDR, &CH0L_data);
  require_noerr( err, exit );
  
  err = APDS9930_Read_RegData(Ch0DATAH_ADDR, &CH0H_data);
  require_noerr( err, exit );
  
  err = APDS9930_Read_RegData(Ch1DATAL_ADDR, &CH1L_data);
  require_noerr( err, exit );
  
  err = APDS9930_Read_RegData(Ch1DATAH_ADDR, &CH1H_data);
  require_noerr( err, exit );
  
  err = APDS9930_Read_RegData(PDATAL_ADDR, &ProxL_data);
  require_noerr( err, exit );
  
  err = APDS9930_Read_RegData(PDATAH_ADDR, &ProxH_data);
  require_noerr( err, exit );
  
  *Prox_data = ProxH_data<<8 | ProxL_data;
    
  LPC = GA*DF/(ALSIT*AGAIN);
  
  CH0_data = CH0H_data<<8 | CH0L_data;
  CH1_data = CH1H_data<<8 | CH1L_data;
  
  IAC1 = (int)(CH0_data - B*CH1_data);
  IAC2 = (int)(C*CH0_data - D*CH1_data);
  IAC = Max(IAC1, IAC2);
  
  *Lux_data = (int)(IAC*LPC);
  
  APDS9930_Clear_intrtrupt();
exit:
  return err;
}

OSStatus apds9930_sensor_init(void)
{
  OSStatus err = kNoErr;
  uint8_t device_id;
  
  MicoI2cFinalize(&apds_i2c_device); 
  
  /*int apds9930 sensor i2c device*/
  err = MicoI2cInitialize(&apds_i2c_device);
  require_noerr_action( err, exit, apds9930_log("APDS9930_ERROR: MicoI2cInitialize err = %d.", err) );
  
  if( false == MicoI2cProbeDevice(&apds_i2c_device, 5) ){
    apds9930_log("APDS9930_ERROR: no i2c device found!");
    err = kNotFoundErr;
    goto exit;
  }
  
  APDS9930_Clear_intrtrupt();
  
  err = APDS9930_Read_RegData(ID_ADDR, &device_id);
  require_noerr( err, exit );
  
  if(APDS9930_ID != device_id){
    apds9930_log("APDS9930_ERROR: device id err");
    err = kNotFoundErr;
    goto exit;
  }
  
  apds9930_enable();
  
exit:
  return err;
}

OSStatus apds9930_sensor_deinit(void)
{
  OSStatus err = kUnknownErr;
  
  err = MicoI2cFinalize(&apds_i2c_device);
  require_noerr_action( err, exit, apds9930_log("APDS9930_ERROR: MicoI2cFinalize err = %d.", err));
  
exit:
  return err;
}


