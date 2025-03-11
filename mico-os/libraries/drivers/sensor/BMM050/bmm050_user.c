/**
 ******************************************************************************
 * @file    bmm050_user.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   bmm050 sensor control demo.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */
/*---------------------------------------------------------------------------*/
/* Includes*/
/*---------------------------------------------------------------------------*/
#include "bmm050.h"
#include "bmm050_user.h"

#define bmm050_user_log(M, ...) custom_log("BMM050_USER", M, ##__VA_ARGS__)
#define bmm050_user_log_trace() custom_log_trace("BMM050_USER")

#define BMM050_API

/* I2C device */
mico_i2c_device_t bmm050_i2c_device = {
  BMM050_I2C_DEVICE, BMM050_I2C_ADDRESS, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};
/*----------------------------------------------------------------------------*/
/*  The following functions are used for reading and writing of
 *	sensor data using I2C or SPI communication
 *----------------------------------------------------------------------------*/
 #ifdef BMM050_API
/*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
s8 BMM050_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
 /*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BMM050_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*	\Brief: The function is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BMM050_SPI_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*	\Brief: The function is used as SPI bus read
 *	\Return : Status of the SPI read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
s8 BMM050_SPI_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*
 * \Brief: SPI/I2C init routine
*/
s8 BMM050_I2C_routine(void);
s8 BMM050_SPI_routine(void);
#endif
/********************End of I2C/SPI function declarations***********************/
/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMM050_delay_msek(u32 msek);
/* This function is an example for reading sensor data
 *	\param: None
 *	\return: communication result
 */
s32 bmm050_data_readout_template(void);
/*----------------------------------------------------------------------------*
 *  struct bmm050 parameters can be accessed by using bmm050_t
 *	bmm050 having the following parameters
 *	Bus write function pointer: BMM050_WR_FUNC_PTR
 *	Bus read function pointer: BMM050_RD_FUNC_PTR
 *	Burst read function pointer: BMM050_BRD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *	Chip id of the sensor: chip_id
 *---------------------------------------------------------------------------*/
struct bmm050 bmm050_t;
/*---------------------------------------------------------------------------*/
/* This function is an example for reading sensor data
 *	\param: None
 *	\return: communication result
 */
s32 bmm050_data_readout_template(void)
{
	/* Structure used for read the mag xyz data*/
	struct bmm050_mag_data_s16_t data;
	/* Structure used for read the mag xyz data with 32 bit output*/
	struct bmm050_mag_s32_data_t data_s32;
	/* Structure used for read the mag xyz data with float output*/
	struct bmm050_mag_data_float_t data_float;
	/* Variable used to get the data rate*/
	u8 v_data_rate_u8 = BMM050_INIT_VALUE;
	/* Variable used to set the data rate*/
	u8 v_data_rate_value_u8 = BMM050_INIT_VALUE;
	/* result of communication results*/
	s32 com_rslt = 0;

/*---------------------------------------------------------------------------*
 *********************** START INITIALIZATION ************************
 *--------------------------------------------------------------------------*/
 /*	Based on the user need configure I2C or SPI interface.
  *	It is sample code to explain how to use the bmm050 API*/
	#ifdef BMM050_API
	BMM050_I2C_routine();
	/*BMM050_SPI_routine(); */
	#endif
/*--------------------------------------------------------------------------*
 *  This function used to assign the value/reference of
 *	the following parameters
 *	I2C address
 *	Bus Write
 *	Bus read
 *	company_id
*-------------------------------------------------------------------------*/
	com_rslt = bmm050_init(&bmm050_t);

/*	For initialization it is required to set the mode of
 *	the sensor as "NORMAL"
 *	but before set the mode needs to configure the power control bit
 *	in the register 0x4B bit BMM050_INIT_VALUE should be enabled
 *	This bit is enabled by calling bmm050_init function
 *	For the Normal data acquisition/read/write is possible in this mode
 *	by using the below API able to set the power mode as NORMAL*/
	/* Set the power mode as NORMAL*/
	com_rslt += bmm050_set_functional_state(BMM050_NORMAL_MODE);
/*--------------------------------------------------------------------------*
************************* END INITIALIZATION *************************
*---------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*
************************* START GET and SET FUNCTIONS DATA ****************
*---------------------------------------------------------------------------*/
	/* This API used to Write the data rate of the sensor, input
	value have to be given
	data rate value set from the register 0x4C bit 3 to 5*/
	v_data_rate_value_u8 = BMM050_DATA_RATE_30HZ;/* set data rate of 30Hz*/
	com_rslt += bmm050_set_data_rate(v_data_rate_value_u8);

	/* This API used to read back the written value of data rate*/
	com_rslt += bmm050_get_data_rate(&v_data_rate_u8);
/*-----------------------------------------------------------------*
************************* END GET and SET FUNCTIONS ****************
*-------------------------------------------------------------------*/
/*------------------------------------------------------------------*
************************* START READ SENSOR DATA(X,Y and Z axis) ********
*------------------------------------------------------------------*/
	/* accessing the bmm050_mdata parameter by using data*/
	com_rslt += bmm050_read_mag_data_XYZ(&data);/* Reads the mag x y z data*/


	/* accessing the bmm050_mdata_float parameter by using data_float*/
	com_rslt += bmm050_read_mag_data_XYZ_float(&data_float);/* Reads mag xyz data output as 32bit value*/

	/* accessing the bmm050_mdata_s32 parameter by using data_s32*/
	com_rslt += bmm050_read_mag_data_XYZ_s32(&data_s32);/* Reads mag xyz data output as float value*/

/*--------------------------------------------------------------------*
************************* END READ SENSOR DATA(X,Y and Z axis) ************
*-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*
************************* START DE-INITIALIZATION ***********************
*-------------------------------------------------------------------------*/
/*	For de-initialization it is required to set the mode of
 *	the sensor as "SUSPEND"
 *	the SUSPEND mode set from the register 0x4B bit BMM050_INIT_VALUE should be disabled
 *	by using the below API able to set the power mode as SUSPEND*/
	/* Set the power mode as SUSPEND*/
	com_rslt += bmm050_set_functional_state(BMM050_SUSPEND_MODE);
/*---------------------------------------------------------------------*
************************* END DE-INITIALIZATION **********************
*---------------------------------------------------------------------*/
return com_rslt;
}

#ifdef BMM050_API
/*--------------------------------------------------------------------------*/
/*	The following function is used to map the I2C bus read, write, delay and
 *	device address with global structure bmm050_t
 *-------------------------------------------------------------------------*/
s8 BMM050_I2C_routine(void) {
/*--------------------------------------------------------------------------*/
/*  By using bmm050_t the following structure parameter can be accessed
 *	Bus write function pointer: BMM050_WR_FUNC_PTR
 *	Bus read function pointer: BMM050_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *--------------------------------------------------------------------------*/
	bmm050_t.bus_write = BMM050_I2C_bus_write;
	bmm050_t.bus_read = BMM050_I2C_bus_read;
	bmm050_t.delay_msec = BMM050_delay_msek;
	bmm050_t.dev_addr = BMM050_I2C_ADDRESS;

	return BMM050_INIT_VALUE;
}

/*---------------------------------------------------------------------------*/
/*	The following function is used to map the SPI bus read, write and delay
 *	with global structure bmm050_t
 *--------------------------------------------------------------------------*/
//s8 BMM050_SPI_routine(void) {
///*--------------------------------------------------------------------------*
// *  By using bmm050_t the following structure parameter can be accessed
// *	Bus write function pointer: BMM050_WR_FUNC_PTR
// *	Bus read function pointer: BMM050_RD_FUNC_PTR
// *	Delay function pointer: delay_msec
// *--------------------------------------------------------------------------*/

//	bmm050_t.bus_write = BMM050_SPI_bus_write;
//	bmm050_t.bus_read = BMM050_SPI_bus_read;
//	bmm050_t.delay_msec = BMM050_delay_msek;

//	return BMM050_INIT_VALUE;
//}

/************** SPI/I2C buffer length ******/
#define	I2C_BUFFER_LEN 8
#define SPI_BUFFER_LEN 5
#define MASK_DATA1	0xFF
#define MASK_DATA2	0x80
#define MASK_DATA3	0x7F
#define	C_BMM050_ONE_U8X	(1)
#define	C_BMM050_TWO_U8X	(2)

/*-------------------------------------------------------------------*
*
*	This is a sample code for read and write the data by using I2C/SPI
*	Use either I2C or SPI based on your need
*	The device address defined in the bmm050.h file
*
*-----------------------------------------------------------------------*/
 /*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BMM050_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
        mico_i2c_message_t bmm050_i2c_msg = {NULL, NULL, 0, 0, 0, false};
	s32 iError = BMM050_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN];
	u8 stringpos = BMM050_INIT_VALUE;
	array[BMM050_INIT_VALUE] = reg_addr;
	for (stringpos = BMM050_INIT_VALUE; stringpos < cnt; stringpos++) {
		array[stringpos + C_BMM050_ONE_U8X] = *(reg_data + stringpos);
	}
	/*
	* Please take the below function as your reference for
	* write the data using I2C communication
	* "IERROR = I2C_WRITE_STRING(DEV_ADDR, ARRAY, CNT+C_BMM050_ONE_U8X)"
	* add your I2C write function here
	* iError is an return value of I2C read function
	* Please select your valid return value
	* In the driver SUCCESS defined as BMM050_INIT_VALUE
    * and FAILURE defined as -C_BMM050_ONE_U8X
	* Note :
	* This is a full duplex operation,
	* The first read data is discarded, for that extra write operation
	* have to be initiated. For that cnt+C_BMM050_ONE_U8X operation done in the I2C write string function
	* For more information please refer data sheet SPI communication:
	*/
          
        iError = MicoI2cBuildTxMessage(&bmm050_i2c_msg, array, cnt + 1, 3);
        iError = MicoI2cTransfer(&bmm050_i2c_device, &bmm050_i2c_msg, 1);
        if(0 != iError){
          iError = -1;
        }
        
	return (s8)iError;
}

 /*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
s8 BMM050_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
        mico_i2c_message_t bmm050_i2c_msg = {NULL, NULL, 0, 0, 0, false};
	s32 iError = BMM050_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN] = {BMM050_INIT_VALUE};
//	u8 stringpos = BMM050_INIT_VALUE;
	array[BMM050_INIT_VALUE] = reg_addr;
	/* Please take the below function as your reference
	 * for read the data using I2C communication
	 * add your I2C rad function here.
	 * "IERROR = I2C_WRITE_READ_STRING(DEV_ADDR, ARRAY, ARRAY, C_BMM050_ONE_U8X, CNT)"
	 * iError is an return value of SPI write function
	 * Please select your valid return value
	 * In the driver SUCCESS defined as BMM050_INIT_VALUE
     * and FAILURE defined as -C_BMM050_ONE_U8X
	 */
        
        iError = MicoI2cBuildCombinedMessage(&bmm050_i2c_msg, array, reg_data, 1, cnt, 3);
         if(0 != iError){
          return (s8)iError; 
        }
        iError = MicoI2cTransfer(&bmm050_i2c_device, &bmm050_i2c_msg, 1);
        if(0 != iError){
          return (s8)iError;
        }
        
//	for (stringpos = BMM050_INIT_VALUE; stringpos < cnt; stringpos++) {
//		*(reg_data + stringpos) = array[stringpos];
//	}
	return (s8)iError;
}

/*	\Brief: The function is used as SPI bus read
 *	\Return : Status of the SPI read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
//s8 BMM050_SPI_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
//{
//	s32 iError=BMM050_INIT_VALUE;
//	u8 array[SPI_BUFFER_LEN]={MASK_DATA1};
//	u8 stringpos;
//	/*	For the SPI mode only 7 bits of register addresses are used.
//	The MSB of register address is declared the bit what functionality it is
//	read/write (read as C_BMM050_ONE_U8X/write as BMM050_INIT_VALUE)*/
//	array[BMM050_INIT_VALUE] = reg_addr|MASK_DATA2;/*read routine is initiated register address is mask with 0x80*/
//	/*
//	* Please take the below function as your reference for
//	* read the data using SPI communication
//	* " IERROR = SPI_READ_WRITE_STRING(ARRAY, ARRAY, CNT+C_BMM050_ONE_U8X)"
//	* add your SPI read function here
//	* iError is an return value of SPI read function
//	* Please select your valid return value
//	* In the driver SUCCESS defined as BMM050_INIT_VALUE
//    * and FAILURE defined as -1
//	* Note :
//	* This is a full duplex operation,
//	* The first read data is discarded, for that extra write operation
//	* have to be initiated. For that cnt+C_BMM050_ONE_U8X operation done in the SPI read
//	* and write string function
//	* For more information please refer data sheet SPI communication:
//	*/
//	for (stringpos = BMM050_INIT_VALUE; stringpos < cnt; stringpos++) {
//		*(reg_data + stringpos) = array[stringpos+C_BMM050_ONE_U8X];
//	}
//	return (s8)iError;
//}
//
///*	\Brief: The function is used as SPI bus write
// *	\Return : Status of the SPI write
// *	\param dev_addr : The device address of the sensor
// *	\param reg_addr : Address of the first register, will data is going to be written
// *	\param reg_data : It is a value hold in the array,
// *		will be used for write the value into the register
// *	\param cnt : The no of byte of data to be write
// */
//s8 BMM050_SPI_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
//{
//	s32 iError = BMM050_INIT_VALUE;
//	u8 array[SPI_BUFFER_LEN * C_BMM050_TWO_U8X];
//	u8 stringpos = BMM050_INIT_VALUE;
//	for (stringpos = BMM050_INIT_VALUE; stringpos < cnt; stringpos++) {
//		/* the operation of (reg_addr++)&0x7F done: because it ensure the
//		   BMM050_INIT_VALUE and C_BMM050_ONE_U8X of the given value
//		   It is done only for 8bit operation*/
//		array[stringpos * C_BMM050_TWO_U8X] = (reg_addr++) & MASK_DATA3;
//		array[stringpos * C_BMM050_TWO_U8X + C_BMM050_ONE_U8X] = *(reg_data + stringpos);
//	}
//	/* Please take the below function as your reference
//	 * for write the data using SPI communication
//	 * add your SPI write function here.
//	 * "IERROR = SPI_WRITE_STRING(ARRAY, CNT*C_BMM050_TWO_U8X)"
//	 * iError is an return value of SPI write function
//	 * Please select your valid return value
//     * In the driver SUCCESS defined as BMM050_INIT_VALUE
//     * and FAILURE defined as -1
//	 */
//	return (s8)iError;
//}

/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMM050_delay_msek(u32 msek)
{
	/*Here you can write your own delay routine*/
  mico_thread_msleep(msek);
}
#endif


OSStatus bmm050_sensor_init(void)
{
  OSStatus err = kUnknownErr;
  /* Variable used to get the data rate*/
  u8 v_data_rate_u8 = BMM050_INIT_VALUE;
  /* Variable used to set the data rate*/
  u8 v_data_rate_value_u8 = BMM050_INIT_VALUE;
  s32 com_rslt = BMM050_ERROR;  // result of communication results
 // u8 v_stand_by_time_u8 = BME280_INIT_VALUE;  //  The variable used to assign the standby time
  
  // I2C init
  MicoI2cFinalize(&bmm050_i2c_device);   // in case error
  err = MicoI2cInitialize(&bmm050_i2c_device);
  require_noerr_action( err, exit, bmm050_user_log("BMM050_ERROR: MicoI2cInitialize err = %d.", err) );
  if( false == MicoI2cProbeDevice(&bmm050_i2c_device, 5) ){
    bmm050_user_log("BMM050_ERROR: no i2c device found!");
    err = kNotFoundErr;
    goto exit;
  }
  
  // sensor init

  /*********************** START INITIALIZATION ************************/
  /*	Based on the user need configure I2C or SPI interface.
  *	It is example code to explain how to use the bme280 API*/
#ifdef BMM050_API
  BMM050_I2C_routine();
  com_rslt = bmm050_init(&bmm050_t);
  com_rslt += bmm050_set_functional_state(BMM050_NORMAL_MODE);
  /*------------------------------------------------------------------------*
  ************************* START GET and SET FUNCTIONS DATA ****************
  *---------------------------------------------------------------------------*/
  /* This API used to Write the data rate of the sensor, input
  value have to be given
  data rate value set from the register 0x4C bit 3 to 5*/
  v_data_rate_value_u8 = BMM050_DATA_RATE_30HZ;/* set data rate of 30Hz*/
  com_rslt += bmm050_set_data_rate(v_data_rate_value_u8);

  /* This API used to read back the written value of data rate*/
  com_rslt += bmm050_get_data_rate(&v_data_rate_u8);
  /*-----------------------------------------------------------------*
  ************************* END GET and SET FUNCTIONS ****************
  *-------------------------------------------------------------------*/
  if(com_rslt < 0){
    bmm050_user_log("BMM050_ERROR: bme280 sensor init failed!");
    err = kNotInitializedErr;
    goto exit;
  }
  /************************* END INITIALIZATION *************************/
#endif
  return kNoErr;
  
exit:
  return err;
}


OSStatus bmm050_data_readout(s16 *v_mag_datax_s16, s16 *v_mag_datay_s16, s16 *v_mag_dataz_s16)
{
  OSStatus err = kUnknownErr;
  struct bmm050_mag_data_s16_t data;
  
  /* result of communication results*/
  s32 com_rslt = BMM050_ERROR;
  
  //-------------------------- NOTE ----------------------------------
  // this is to avoid i2c pin is re-init by other module because they use the same pin.
  MicoI2cInitialize(&bmm050_i2c_device);
  //------------------------------------------------------------------
    
  /************ START READ TRUE PRESSURE, TEMPERATURE AND HUMIDITY DATA *********/

  /* accessing the bmm050_mdata parameter by using data*/
  com_rslt = bmm050_read_mag_data_XYZ(&data);/* Reads the mag x y z data*/
  
  *v_mag_datax_s16 = data.datax;
  *v_mag_datay_s16 = data.datay;
  *v_mag_dataz_s16 = data.dataz;
  /************ END READ TRUE PRESSURE, TEMPERATURE AND HUMIDITY ********/
  
  if(0 == com_rslt){
    err = kNoErr;
  }
  return err;
}

OSStatus bmm050_sensor_deinit(void)
{
  OSStatus err = kUnknownErr;
  s32 com_rslt = BMM050_ERROR;
  
  err = MicoI2cFinalize(&bmm050_i2c_device);
  require_noerr_action( err, exit, bmm050_user_log("BMM050_ERROR: MicoI2cFinalize err = %d.", err));
  
/*---------------------------------------------------------------------------*
*********************** START DE-INITIALIZATION *****************************
*--------------------------------------------------------------------------*/
/*	For de-initialization it is required to set the mode of
 *	the sensor as "SUSPEND"
 *	the SUSPEND mode set from the register 0x4B bit BMM050_INIT_VALUE should be disabled
 *	by using the below API able to set the power mode as SUSPEND*/
	/* Set the power mode as SUSPEND*/
	com_rslt = bmm050_set_functional_state(BMM050_SUSPEND_MODE);

/*--------------------------------------------------------------------------*
*********************** END DE-INITIALIZATION **************************
*---------------------------------------------------------------------------*/

  if(0 == com_rslt){
    err = kNoErr;
  }
  
exit:
  return err;
}


