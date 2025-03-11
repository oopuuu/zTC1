/**
 ******************************************************************************
 * @file    bma2x2_user.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   bma2x2 sensor control demo.
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
#include "bma2x2.h"
#include "bma2x2_user.h"

#define bma2x2_user_log(M, ...) custom_log("BMA2x2_USER", M, ##__VA_ARGS__)
#define bma2x2_user_log_trace() custom_log_trace("BMA2x2_USER")

#define BMA2x2_API

/* I2C device */
mico_i2c_device_t bma2x2_i2c_device = {
  BMA2x2_I2C_DEVICE, BMA2x2_I2C_ADDR1, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};

/*----------------------------------------------------------------------------*
* 	The following functions are used for reading and writing of
*	sensor data using I2C or SPI communication
*----------------------------------------------------------------------------*/
#ifdef BMA2x2_API
 /*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
s8 BMA2x2_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
 /*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BMA2x2_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*	\Brief: The function is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BMA2x2_SPI_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*	\Brief: The function is used as SPI bus read
 *	\Return : Status of the SPI read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read */
s8 BMA2x2_SPI_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*
 * \Brief: SPI/I2C init routine
*/
s8 BMA2x2_I2C_routine(void);
s8 BMA2x2_SPI_routine(void);
#endif
/********************End of I2C/SPI function declarations***********************/
/*	Brief : The delay routine
 *	\param : delay in ms
 */
void BMA2x2_delay_msek(u32 msek);
/*! 
 *	@brief This function is an example for delay
 *	@param : None
 *	@return : communication result
 */
s32 bma2x2_data_readout_template(void);
/*----------------------------------------------------------------------------*
*  struct bma2x2_t parameters can be accessed by using bma2x2
 *	bma2x2_t having the following parameters
 *	Bus write function pointer: BMA2x2_WR_FUNC_PTR
 *	Bus read function pointer: BMA2x2_RD_FUNC_PTR
 *	Burst read function pointer: BMA2x2_BRD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *	Chip id of the sensor: chip_id
 *---------------------------------------------------------------------------*/
struct bma2x2_t bma2x2;
/*----------------------------------------------------------------------------*
*  V_BMA2x2RESOLUTION_u8R used for selecting the accelerometer resolution
 *	12 bit
 *	14 bit
 *	10 bit
*----------------------------------------------------------------------------*/
extern u8 V_BMA2x2RESOLUTION_u8R;
/* This function is an example for reading sensor data
 *	\param: None
 *	\return: communication result
 */
s32 bma2x2_data_readout_template(void)
{
	/*Local variables for reading accel x, y and z data*/
	s16	v_accel_x_s16, v_accel_y_s16, v_accel_z_s16 = 0;

	/* bma2x2acc_data structure used to read accel xyz data*/
	struct bma2x2_accel_data sample_xyz;
	/* bma2x2acc_data_temp structure used to read accel xyz and temperature data*/
	struct bma2x2_accel_data_temp sample_xyzt;
	/* Local variable used to assign the bandwidth value*/
	u8 v_bw_value_u8 = 0;
	/* Local variable used to set the bandwidth value*/
	u8 banwid = 0;
	/* status of communication*/
	s32 com_rslt = 0;


/*********************** START INITIALIZATION ************************
  *	Based on the user need configure I2C or SPI interface.
  *	It is example code to explain how to use the bma2x2 API*/
	#ifdef BMA2x2_API
	BMA2x2_I2C_routine();
	/*BMA2x2_SPI_routine(); */
	#endif
 /*--------------------------------------------------------------------------*
 *  This function used to assign the value/reference of
 *	the following parameters
 *	I2C address
 *	Bus Write
 *	Bus read
 *	Chip id
 *-------------------------------------------------------------------------*/
	com_rslt = bma2x2_init(&bma2x2);

/*	For initialization it is required to set the mode of
 *	the sensor as "NORMAL"
 *	NORMAL mode is set from the register 0x11 and 0x12
 *	0x11 -> bit 5,6,7 -> set value as 0
 *	0x12 -> bit 5,6 -> set value as 0
 *	data acquisition/read/write is possible in this mode
 *	by using the below API able to set the power mode as NORMAL
 *	For the Normal/standby/Low power 2 mode Idle time of at least 2us(micro seconds)
 *	required for read/write operations*/
	/* Set the power mode as NORMAL*/
	com_rslt += bma2x2_set_power_mode(BMA2x2_MODE_NORMAL);
/*	Note:
	*	For the Suspend/Low power1 mode Idle time of at least 450us(micro seconds)
	*	required for read/write operations*/

/************************* END INITIALIZATION *************************/

/*------------------------------------------------------------------------*
************************* START GET and SET FUNCTIONS DATA ****************
*---------------------------------------------------------------------------*/
	/* This API used to Write the bandwidth of the sensor input
	value have to be given
	bandwidth is set from the register 0x10 bits from 1 to 4*/
	v_bw_value_u8 = 0x08;/* set bandwidth of 7.81Hz*/
	com_rslt += bma2x2_set_bw(v_bw_value_u8);

	/* This API used to read back the written value of bandwidth*/
	com_rslt += bma2x2_get_bw(&banwid);
/*-----------------------------------------------------------------*
************************* END GET and SET FUNCTIONS ****************
*-------------------------------------------------------------------*/
/*------------------------------------------------------------------*
************************* START READ SENSOR DATA(X,Y and Z axis) ********
*---------------------------------------------------------------------*/
	com_rslt += bma2x2_read_accel_x(&v_accel_x_s16);/* Read the accel X data*/

	com_rslt += bma2x2_read_accel_y(&v_accel_y_s16);/* Read the accel Y data*/

	com_rslt += bma2x2_read_accel_z(&v_accel_z_s16);/* Read the accel Z data*/

	/* accessing the bma2x2acc_data parameter by using sample_xyz*/
	com_rslt += bma2x2_read_accel_xyz(&sample_xyz);/* Read the accel XYZ data*/

	/* accessing the bma2x2acc_data_temp parameter by using sample_xyzt*/
	com_rslt += bma2x2_read_accel_xyzt(&sample_xyzt);/* Read the accel XYZT data*/

/*--------------------------------------------------------------------*
************************* END READ SENSOR DATA(X,Y and Z axis) ************
*-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*
************************* START DE-INITIALIZATION ***********************
*-------------------------------------------------------------------------*/
/*	For de-initialization it is required to set the mode of
 *	the sensor as "DEEP SUSPEND"
 *	DEEP SUSPEND mode is set from the register 0x11
 *	0x11 -> bit 5 -> set value as 1
 *	the device reaches the lowest power consumption only
 *	interface selection is kept alive
 *	No data acquisition is performed
 *	by using the below API able to set the power mode as DEEPSUSPEND*/
 /* Set the power mode as DEEPSUSPEND*/
	com_rslt += bma2x2_set_power_mode(BMA2x2_MODE_DEEP_SUSPEND);
/*---------------------------------------------------------------------*
************************* END DE-INITIALIZATION **********************
*---------------------------------------------------------------------*/
return com_rslt;
}

#ifdef BMA2x2_API
/*--------------------------------------------------------------------------*
*	The following function is used to map the I2C bus read, write, delay and
*	device address with global structure bma2x2_t
*-------------------------------------------------------------------------*/
s8 BMA2x2_I2C_routine(void) {
/*--------------------------------------------------------------------------*
 *  By using bma2x2 the following structure parameter can be accessed
 *	Bus write function pointer: BMA2x2_WR_FUNC_PTR
 *	Bus read function pointer: BMA2x2_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *--------------------------------------------------------------------------*/
	bma2x2.bus_write = BMA2x2_I2C_bus_write;
	bma2x2.bus_read = BMA2x2_I2C_bus_read;
	bma2x2.delay_msec = BMA2x2_delay_msek;
	bma2x2.dev_addr = BMA2x2_I2C_ADDR2;

	return 0;
}

/*---------------------------------------------------------------------------*
 * The following function is used to map the SPI bus read, write and delay
 * with global structure bma2x2_t
// *--------------------------------------------------------------------------*/
//s8 BMA2x2_SPI_routine(void) {
///*--------------------------------------------------------------------------*
// *  By using bma2x2 the following structure parameter can be accessed
// *	Bus write function pointer: BMA2x2_WR_FUNC_PTR
// *	Bus read function pointer: BMA2x2_RD_FUNC_PTR
// *	Delay function pointer: delay_msec
// *--------------------------------------------------------------------------*/

//	bma2x2.bus_write = BMA2x2_SPI_bus_write;
//	bma2x2.bus_read = BMA2x2_SPI_bus_read;
//	bma2x2.delay_msec =BMA2x2_delay_msek;

//	return 0;
//}

/************** I2C/SPI buffer length ******/
#define	I2C_BUFFER_LEN 8
#define SPI_BUFFER_LEN 5

/*-------------------------------------------------------------------*
*	This is a sample code for read and write the data by using I2C/SPI
*	Use either I2C or SPI based on your need
*
*-----------------------------------------------------------------------*/
/*	For configuring the I2C it is required to switch ON
 *	SDI, SDO and CLk and also select the device address
 * The following definition of I2C address is used for the following sensors
 * BMA255
 * BMA355
 * BMA280
 * BMA282
 * BMA223
 * BMA254
 * BMA284
 * BMA250E
 * BMA222E

 #define BMA2x2_I2C_ADDR1                0x18
 #define BMA2x2_I2C_ADDR2                0x19

 * The following definition of I2C address is used for the following sensors
 * BMC150
 * BMC056
 * BMC156

 #define BMA2x2_I2C_ADDR3                0x10
 #define BMA2x2_I2C_ADDR4                0x11
 *************************************************************************/
 /*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BMA2x2_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
        mico_i2c_message_t bma2x2_i2c_msg = {NULL, NULL, 0, 0, 0, false};
	s32 iError = 0;
	u8 array[I2C_BUFFER_LEN];
	u8 stringpos = 0;
	array[0] = reg_addr;
	for (stringpos = 0; stringpos < cnt; stringpos++) {
		array[stringpos + 1] = *(reg_data + stringpos);
	}
	/*
	* Please take the below function as your reference for
	* write the data using I2C communication
	* "IERROR = I2C_WRITE_STRING(DEV_ADDR, ARRAY, CNT+1)"
	* add your I2C write function here
	* iError is an return value of I2C read function
	* Please select your valid return value
	* In the driver SUCCESS defined as 0
    * and FAILURE defined as -1
	* Note :
	* This is a full duplex operation,
	* The first read data is discarded, for that extra write operation
	* have to be initiated. For that cnt+1 operation done in the I2C write string function
	* For more information please refer data sheet SPI communication:
	*/
        iError = MicoI2cBuildTxMessage(&bma2x2_i2c_msg, array, cnt + 1, 3);
        iError = MicoI2cTransfer(&bma2x2_i2c_device, &bma2x2_i2c_msg, 1);
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
s8 BMA2x2_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
        mico_i2c_message_t bma2x2_i2c_msg = {NULL, NULL, 0, 0, 0, false};
	s32 iError = 0;
	u8 array[I2C_BUFFER_LEN] = {0};
//	u8 stringpos = 0;
	array[0] = reg_addr;
	/* Please take the below function as your reference
	 * for read the data using I2C communication
	 * add your I2C rad function here.
	 * "IERROR = I2C_WRITE_READ_STRING(DEV_ADDR, ARRAY, ARRAY, 1, CNT)"
	 * iError is an return value of SPI write function
	 * Please select your valid return value
     * In the driver SUCCESS defined as 0
     * and FAILURE defined as -1
	 */
        
        iError = MicoI2cBuildCombinedMessage(&bma2x2_i2c_msg, array, reg_data, 1, cnt, 3);
         if(0 != iError){
          return (s8)iError; 
        }
        iError = MicoI2cTransfer(&bma2x2_i2c_device, &bma2x2_i2c_msg, 1);
        if(0 != iError){
          return (s8)iError;
        }
        
//	for (stringpos = 0; stringpos < cnt; stringpos++) {
//		*(reg_data + stringpos) = array[stringpos];
//	}
	return (s8)iError;
}

/*	\Brief: The function is used as SPI bus read
 *	\Return : Status of the SPI read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read */
s8 BMA2x2_SPI_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError=0;
	u8 array[SPI_BUFFER_LEN]={0xFF};
	u8 stringpos;
	/*	For the SPI mode only 7 bits of register addresses are used.
	The MSB of register address is declared the bit what functionality it is
	read/write (read as 1/write as 0)*/
	array[0] = reg_addr|0x80;/*read routine is initiated register address is mask with 0x80*/
	/*
	* Please take the below function as your reference for
	* read the data using SPI communication
	* " IERROR = SPI_READ_WRITE_STRING(ARRAY, ARRAY, CNT+1)"
	* add your SPI read function here
	* iError is an return value of SPI read function
	* Please select your valid return value
	* In the driver SUCCESS defined as 0
    * and FAILURE defined as -1
	* Note :
	* This is a full duplex operation,
	* The first read data is discarded, for that extra write operation
	* have to be initiated. For that cnt+1 operation done in the SPI read
	* and write string function
	* For more information please refer data sheet SPI communication:
	*/
	for (stringpos = 0; stringpos < cnt; stringpos++) {
		*(reg_data + stringpos) = array[stringpos+1];
	}
	return (s8)iError;
}
//
///*	\Brief: The function is used as SPI bus write
// *	\Return : Status of the SPI write
// *	\param dev_addr : The device address of the sensor
// *	\param reg_addr : Address of the first register, will data is going to be written
// *	\param reg_data : It is a value hold in the array,
// *		will be used for write the value into the register
// *	\param cnt : The no of byte of data to be write
// */
//s8 BMA2x2_SPI_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
//{
//	s32 iError = 0;
//	u8 array[SPI_BUFFER_LEN * 2];
//	u8 stringpos = 0;
//	for (stringpos = 0; stringpos < cnt; stringpos++) {
//		/* the operation of (reg_addr++)&0x7F done: because it ensure the
//		   0 and 1 of the given value
//		   It is done only for 8bit operation*/
//		array[stringpos * 2] = (reg_addr++) & 0x7F;
//		array[stringpos * 2 + 1] = *(reg_data + stringpos);
//	}
//	/* Please take the below function as your reference
//	 * for write the data using SPI communication
//	 * add your SPI write function here.
//	 * "IERROR = SPI_WRITE_STRING(ARRAY, CNT*2)"
//	 * iError is an return value of SPI write function
//	 * Please select your valid return value
//	 * In the driver SUCCESS defined as 0
//     * and FAILURE defined as -1
//	 */
//	return (s8)iError;
//}

/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMA2x2_delay_msek(u32 msek)
{
	/*Here you can write your own delay routine*/
  mico_thread_msleep(msek);
}
#endif


OSStatus bma2x2_sensor_init(void)
{
  OSStatus err = kUnknownErr;
  /* Local variable used to assign the bandwidth value*/
  u8 v_bw_value_u8 = 0;
  /* Local variable used to set the bandwidth value*/
  u8 banwid = 0;
  s32 com_rslt = BMA2x2_ERROR;  // result of communication results
 // u8 v_stand_by_time_u8 = BME280_INIT_VALUE;  //  The variable used to assign the standby time
  
  // I2C init
  MicoI2cFinalize(&bma2x2_i2c_device);   // in case error
  err = MicoI2cInitialize(&bma2x2_i2c_device);
  require_noerr_action( err, exit, bma2x2_user_log("BMA2x2_ERROR: MicoI2cInitialize err = %d.", err) );
  if( false == MicoI2cProbeDevice(&bma2x2_i2c_device, 5) ){
    bma2x2_user_log("BMA2x2_ERROR: no i2c device found!");
    err = kNotFoundErr;
    goto exit;
  }
  
  // sensor init

  /*********************** START INITIALIZATION ************************/
  /*	Based on the user need configure I2C or SPI interface.
  *	It is example code to explain how to use the bme280 API*/
#ifdef BMA2x2_API
  BMA2x2_I2C_routine();
  com_rslt = bma2x2_init(&bma2x2);
  com_rslt += bma2x2_set_power_mode(BMA2x2_MODE_NORMAL);
  /*------------------------------------------------------------------------*
************************* START GET and SET FUNCTIONS DATA ****************
*---------------------------------------------------------------------------*/
  /* This API used to Write the bandwidth of the sensor input
  value have to be given
  bandwidth is set from the register 0x10 bits from 1 to 4*/
  v_bw_value_u8 = 0x08;/* set bandwidth of 7.81Hz*/
  com_rslt += bma2x2_set_bw(v_bw_value_u8);

  /* This API used to read back the written value of bandwidth*/
  com_rslt += bma2x2_get_bw(&banwid);
/*-----------------------------------------------------------------*
************************* END GET and SET FUNCTIONS ****************
*-------------------------------------------------------------------*/

  if(com_rslt < 0){
    bma2x2_user_log("BMA2x2_ERROR: bme280 sensor init failed!");
    err = kNotInitializedErr;
    goto exit;
  }
  /************************* END INITIALIZATION *************************/
#endif
  return kNoErr;
  
exit:
  return err;
}


OSStatus bma2x2_data_readout(s16 *v_accel_x_s16, s16 *v_accel_y_s16, s16 *v_accel_z_s16)
{
  OSStatus err = kUnknownErr;
  
  /* result of communication results*/
  s32 com_rslt = BMA2x2_ERROR;
  
  //-------------------------- NOTE ----------------------------------
  // this is to avoid i2c pin is re-init by other module because they use the same pin.
  MicoI2cInitialize(&bma2x2_i2c_device);
  //------------------------------------------------------------------
    
  /************ START READ TRUE PRESSURE, TEMPERATURE AND HUMIDITY DATA *********/

  com_rslt = bma2x2_read_accel_x(v_accel_x_s16);/* Read the accel X data*/

  com_rslt += bma2x2_read_accel_y(v_accel_y_s16);/* Read the accel Y data*/

  com_rslt += bma2x2_read_accel_z(v_accel_z_s16);/* Read the accel Z data*/
  
  /************ END READ TRUE PRESSURE, TEMPERATURE AND HUMIDITY ********/
  
  if(0 == com_rslt){
    err = kNoErr;
  }
  return err;
}

OSStatus bma2x2_sensor_deinit(void)
{
  OSStatus err = kUnknownErr;
  s32 com_rslt = BMA2x2_ERROR;
  
  err = MicoI2cFinalize(&bma2x2_i2c_device);
  require_noerr_action( err, exit, bma2x2_user_log("BMA2x2_ERROR: MicoI2cFinalize err = %d.", err));
  
/*-----------------------------------------------------------------------*
************************* START DE-INITIALIZATION ***********************
*-------------------------------------------------------------------------*/
/*	For de-initialization it is required to set the mode of
 *	the sensor as "DEEP SUSPEND"
 *	DEEP SUSPEND mode is set from the register 0x11
 *	0x11 -> bit 5 -> set value as 1
 *	the device reaches the lowest power consumption only
 *	interface selection is kept alive
 *	No data acquisition is performed
 *	by using the below API able to set the power mode as DEEPSUSPEND*/
 /* Set the power mode as DEEPSUSPEND*/
	com_rslt = bma2x2_set_power_mode(BMA2x2_MODE_DEEP_SUSPEND);
/*---------------------------------------------------------------------*
************************* END DE-INITIALIZATION **********************
*---------------------------------------------------------------------*/
        
  if(0 == com_rslt){
    err = kNoErr;
  }
  
exit:
  return err;
}




