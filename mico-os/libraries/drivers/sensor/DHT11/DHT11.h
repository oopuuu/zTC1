/**
 ******************************************************************************
 * @file    DHT11.h
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    1-May-2015
 * @brief   DHT11 operations.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef __DHT11_H_
#define __DHT11_H_

#include "common.h"
#include "platform.h"


/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MiCO_Sensor_Driver
  * @{
  */

/** @defgroup MiCO_DHT11_Driver MiCO DHT11 Driver
  * @brief Provide driver interface for DHT11 Sensor 
  * @{
  */

#ifndef DHT11_DATA
#define DHT11_DATA             MICO_GPIO_NONE
#endif

// Set GPIO Direction
#define DHT11_IO_IN()          MicoGpioInitialize( (mico_gpio_t)DHT11_DATA, INPUT_PULL_UP );										 
#define DHT11_IO_OUT()         MicoGpioInitialize( (mico_gpio_t)DHT11_DATA, OUTPUT_PUSH_PULL );
	
// Set Data output state
#define DHT11_DATA_Clr()       MicoGpioOutputLow(DHT11_DATA) 
#define DHT11_DATA_Set()       MicoGpioOutputHigh(DHT11_DATA)

// get DATA input state
#define	DHT11_DQ_IN            MicoGpioInputGet(DHT11_DATA)

//-------------------------------- USER INTERFACES -----------------------------

/**
 * @brief Initialize DHT11.
 *
 * @return   0      :  on success.
 * @return   others :  if an error occurred
 */
uint8_t DHT11_Init(void); //Init DHT11


/**
 * @brief Read DHT11 value.
 *
 * @param temperature :  Temperature value of DHT11
 * @param humidity    :  Humidity value of DHT11
 *
 * @return   0      :  on success.
 * @return   others :  if an error occurred
 */
uint8_t DHT11_Read_Data(uint8_t *temperature,uint8_t *humidity); 

/**
 * @brief Read One byte from DHT11 value.
 *
 * @return   0      :  on success.
 * @return   others :  if an error occurred
 */
uint8_t DHT11_Read_Byte(void);//Read One Byte


/**
 * @brief Read One bit from DHT11 value.
 *
 * @return   0      :  on success.
 * @return   others :  if an error occurred
 */
uint8_t DHT11_Read_Bit(void);//Read One Bit


/**
 * @brief Check DHT11.
 *
 * @return   0      :  on success.
 * @return   others :  if an error occurred
 */
uint8_t DHT11_Check(void);


/**
 * @brief Reset DHT11
 *
 * @return none
 */
void DHT11_Rst(void);


/**
 * @brief Millisecond delay function
 *
 * @param nms: Delay time
 *
 * @return none
 */
void Delay_ms(uint16_t nms);


/**
 * @brief Microsecond delay function
 *
 * @param nms: Delay time
 *
 * @return none
 */
void Delay_us(uint32_t nus);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

// delay func
//void Delay_Init(uint8_t SYSCLK);
#endif  // __DHT11_H_
