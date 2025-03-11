/**
 ******************************************************************************
 * @file    hts221.h
 * @author  MEMS Application Team
 * @version V1.2.0
 * @date    28-January-2015
 * @brief   This file contains definitions for the hts221.c
 *          firmware driver.
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
#ifndef __HTS221_H
#define __HTS221_H

#include <stdint.h>
#include "common.h"
#include "platform.h"
    
#ifdef __cplusplus
 extern "C" {
#endif

   
/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @addtogroup MiCO_Sensor_Driver
  * @{
  */

/** @defgroup MiCO_HTS221_Driver MiCO HTS221 Driver
  * @brief Provide driver interface for HTS221 Sensor 
  * @{
  */
   
   
#ifndef HTS221_I2C_PORT
#define HTS221_I2C_PORT             MICO_I2C_NONE
#endif
   
   /** 
  * @brief  Humidity and temperature init structure definition  
  */ 
typedef struct
{
  uint8_t Power_Mode;                         /* Power-down/Sleep/Normal Mode */
  uint8_t Data_Update_Mode;                   /* continuous update/output registers not updated until MSB and LSB reading*/
  uint8_t Reboot_Mode;                        /* Normal Mode/Reboot memory content */
  uint8_t Humidity_Resolutin;                 /* Humidity Resolution */
  uint8_t Temperature_Resolution;             /* Temperature Resolution */
  uint8_t OutputDataRate;                     /* One-shot / 1Hz / 7 Hz / 12.5 Hz */
}HUM_TEMP_InitTypeDef;

/** 
  * @brief  Humidity and temperature status enumerator definition  
  */ 
typedef enum {
    HUM_TEMP_OK = 0,
    HUM_TEMP_ERROR = 1,
    HUM_TEMP_TIMEOUT = 2,
    HUM_TEMP_NOT_IMPLEMENTED = 3
} HUM_TEMP_StatusTypeDef;

/**
 * @brief  Humidity and temperature component id enumerator definition
 */
typedef enum {
    HUM_TEMP_NONE_COMPONENT = 0,
    HUM_TEMP_HTS221_COMPONENT = 1
} HUM_TEMP_ComponentTypeDef;

/**
 * @brief  Humidity and temperature driver extended structure definition
 */
typedef struct {
    HUM_TEMP_ComponentTypeDef id; /* This id must be unique for each component belonging to this class that wants to extend common class */
    void *pData; /* This pointer is specific for each component */
}HUM_TEMP_DrvExtTypeDef;

/** 
  * @brief  Humidity and temperature driver structure definition  
  */ 
typedef struct
{  
  HUM_TEMP_StatusTypeDef       (*Init)(HUM_TEMP_InitTypeDef *);
  HUM_TEMP_StatusTypeDef       (*PowerOFF)(void);
  HUM_TEMP_StatusTypeDef       (*ReadID)(uint8_t *);
  HUM_TEMP_StatusTypeDef       (*Reset)(void);
  void                         (*ConfigIT)(uint16_t);
  void                         (*EnableIT)(uint8_t);
  void                         (*DisableIT)(uint8_t);
  uint8_t                      (*ITStatus)(uint16_t, uint16_t);
  void                         (*ClearIT)(uint16_t, uint16_t);
  HUM_TEMP_StatusTypeDef       (*GetHumidity)(float *);
  HUM_TEMP_StatusTypeDef       (*GetTemperature)(float *);
  HUM_TEMP_DrvExtTypeDef       *extData;
}HUM_TEMP_DrvTypeDef;



#ifndef NULL
  #define NULL      (void *) 0
#endif
   
/**
  * @brief Device Address
  */
#define HTS221_ADDRESS                              0xBE

/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/


/**
 * @brief Device identification register.
 * \code
   * Read
 * Default value: 0xBC
 * 7:0 This read-only register contains the device identifier that, for HTS221, is set to BCh.
 * \endcode
*/
#define HTS221_WHO_AM_I_ADDR                        0x0F
  

   /**
    * @brief Humidity resolution Register
    * \code
    * Read/write
    * Default value: 0x1B
    * 7:6 RFU
    * 5:3 AVGT2-AVGT0: Temperature internal average.
    *     AVGT2 | AVGT1 | AVGT0 | Nr. Internal Average
    *   ------------------------------------------------------
    *      0    |  0    |  0    |     2
    *      0    |  0    |  1    |     4
    *      0    |  1    |  0    |     8
    *      0    |  1    |  1    |     16
    *      1    |  0    |  0    |     32
    *      1    |  0    |  1    |     64
    *      1    |  1    |  0    |     128
    *      1    |  1    |  1    |     256
    *
    * 2:0 AVGH2-AVGH0: Humidity internal average.
    *     AVGH2 | AVGH1 | AVGH0 | Nr. Internal Average
    *   ------------------------------------------------------
    *      0    |  0    |  0    |     4
    *      0    |  0    |  1    |     8
    *      0    |  1    |  0    |     16
    *      0    |  1    |  1    |     32
    *      1    |  0    |  0    |     64
    *      1    |  0    |  1    |     128
    *      1    |  1    |  0    |     256
    *      1    |  1    |  1    |     512
    *
    * \endcode
    */
#define HTS221_RES_CONF_ADDR                        0x10


    /**
    * @brief INFO Register  (LSB data)
    * \code
    * Read/write
    * Default value: 0x00
    * 7:0 INFO7-INFO0: Lower part of the INFO reference
    *                  used for traceability of the sample.
    * \endcode
    */
#define HTS221_INFO_L_ADDR                          0x1E


    /**
    * @brief INFO & Calibration Version Register  (LSB data)
    * \code
    * Read/write
    * Default value: 0x00
    * 7:6 CALVER1:CALVER0
    * 5:0 INFO13-INFO8: Higher part of the INFO reference
    *                  used for traceability of the sample.
    * \endcode
    */
#define HTS221_INFO_H_ADDR                          0x1F


    /**
    * @brief Humidity sensor control register 1
    * \code
    * Read/write
    * Default value: 0x00
    * 7    PD: power down control. 0 - disable; 1 - enable
    * 6:3  RFU
    * 2    BDU: block data update. 0 - disable; 1 - enable
    * 1:0  RFU
    * \endcode
    */

#define HTS221_CTRL_REG1_ADDR                       0x20


    /**
    * @brief Humidity sensor control register 2
    * \code
    * Read/write
    * Default value: 0x00
    * 7    BOOT:  Reboot memory content. 0: normal mode; 1: reboot memory content
    * 6:3  Reserved.
    * 2    Reserved.
    * 1    Reserved.
    * 0    ONE_SHOT: One shot enable. 0: waiting for start of conversion; 1: start for a new dataset
    * \endcode
    */
#define HTS221_CTRL_REG2_ADDR                       0x21

     
     /**
     * @brief Humidity sensor control register 3
     * \code
     * Read/write
     * Default value: 0x00
     * [7]   DRDY_H_L: Data Ready output signal active high, low (0: active high -default;1: active low)
     * [6]   PP_OD: Push-pull / Open Drain selection on pin 3 (DRDY) (0: push-pull - default; 1: open drain)
     * [5:3] Reserved
     * [2]   DRDY_EN: Data Ready enable (0: Data Ready disabled - default;1: Data Ready signal available on pin 3)
     * [1:0] Reserved
     * \endcode
     */
#define HTS221_CTRL_REG3_ADDR                       0x22
     
 
     /**
    * @brief  Status Register
    * \code
    * Read
    * Default value: 0x00
    * 7:2  RFU
    * 1    H_DA: Humidity data available. 0: new data for Humidity is not yet available; 1: new data for Humidity is available.
    * 0    T_DA: Temperature data available. 0: new data for temperature is not yet available; 1: new data for temperature is available.
    * \endcode
    */
#define HTS221_STATUS_REG_ADDR                      0x27


    /**
    * @brief  Humidity data (LSB).
    * \code
    * Read
    * Default value: 0x00.
    * POUT7 - POUT0: Humidity data LSB (2's complement) => signed 16 bits
    * RAW Humidity output data: Hout(%)=(HUMIDITY_OUT_H & HUMIDITY_OUT_L).
    * \endcode
    */
#define HTS221_HUMIDITY_OUT_L_ADDR                  0x28


    /**
    * @brief  Humidity data (MSB).
    * \code
    * Read
    * Default value: 0x00.
    * POUT7 - POUT0: Humidity data LSB (2's complement) => signed 16 bits
    * RAW Humidity output data: Hout(%)=(HUMIDITY_OUT_H & HUMIDITY_OUT_L).
    * \endcode
    */
#define HTS221_HUMIDITY_OUT_H_ADDR                  0x29


    /**
    * @brief  Temperature data (LSB).
    * \code
    * Read
    * Default value: 0x00.
    * TOUT7 - TOUT0: temperature data LSB (2's complement) => signed 16 bits
    * RAW Temperature output data: Tout (LSB)=(TEMP_OUT_H & TEMP_OUT_L).
    * \endcode
    */
#define HTS221_TEMP_OUT_L_ADDR                      0x2A


    /**
    * @brief  Temperature data (MSB).
    * \code
    * Read
    * Default value: 0x00.
    * TOUT15 - TOUT8: temperature data MSB (2's complement) => signed 16 bits
    * RAW Temperature output data: Tout (LSB)=(TEMP_OUT_H & TEMP_OUT_L).
    * \endcode
    */
#define HTS221_TEMP_OUT_H_ADDR                      0x2B


    /**
    *@brief Humidity 0 Register in %RH with sensitivity=2
    *\code
    * Read
    * Value: (Unsigned 8 Bit)/2
    *\endcode
    */
#define HTS221_H0_RH_X2_ADDR                        0x30


    /**
    *@brief Humidity 1 Register in %RH with sensitivity=2
    *\code
    * Read
    * Value: (Unsigned 8 Bit)/2
    *\endcode
    */
#define HTS221_H1_RH_X2_ADDR                        0x31


    /**
    *@brief Temperature 0 Register in deg with sensitivity=8
    *\code
    * Read
    * Value: (Unsigned 16 Bit)/2
    *\endcode
    */
#define HTS221_T0_degC_X8_ADDR                      0x32


    /**
    *@brief Temperature 1 Register in deg with sensitivity=8
    *\code
    * Read
    * Value: (Unsigned 16 Bit)/2
    *\endcode
    */
#define HTS221_T1_degC_X8_ADDR                      0x33


    /**
    *@brief Temperature 1/0 MSB Register in deg with sensitivity=8
    *\code
    * Read
    * Value: (Unsigned 16 Bit)/2
    * 3:2  T1(9):T1(8) MSB T1_degC_X8 bits
    * 1:0  T0(9):T0(8) MSB T0_degC_X8 bits
    *\endcode
    */
#define HTS221_T1_T0_MSB_X8_ADDR                    0x35


    /**
    *@brief Humidity LOW CALIBRATION Register
    *\code
    * Read
    * Default value: 0x00.
    * H0_T0_TOUT7 - H0_T0_TOUT0: HUMIDITY data lSB (2's complement) => signed 16 bits
    *\endcode
    */
#define HTS221_H0_T0_OUT_L_ADDR                     0x36


    /**
    *@brief Humidity LOW CALIBRATION Register
    *\code
    * Read
    * Default value: 0x00.
    * H0_T0_TOUT15 - H0_T0_TOUT8: HUMIDITY data mSB (2's complement) => signed 16 bits
    *\endcode
    */
#define HTS221_H0_T0_OUT_H_ADDR                       0x37


    /**
    *@brief Humidity HIGH CALIBRATION Register
    *\code
    * Read
    * Default value: 0x00.
    * H1_T0_TOUT7 - H1_T0_TOUT0: HUMIDITY data lSB (2's complement) => signed 16 bits
    *\endcode
    */
#define HTS221_H1_T0_OUT_L_ADDR                       0x3A


    /**
    *@brief Humidity HIGH CALIBRATION Register
    *\code
    * Read
    * Default value: 0x00.
    * H1_T0_TOUT15 - H1_T0_TOUT8: HUMIDITY data mSB (2's complement) => signed 16 bits
    *\endcode
    */
#define HTS221_H1_T0_OUT_H_ADDR                       0x3B


    /**
    * @brief  Low Calibration Temperature Register (LSB).
    * \code
    * Read
    * Default value: 0x00.
    * T0_OUT7 - T0_OUT0: temperature data LSB (2's complement) => signed 16 bits
    *  RAW LOW Calibration data: T0_OUT (LSB)=(T0_OUT_H & T0_OUT_L).
    * \endcode
    */
#define HTS221_T0_OUT_L_ADDR                        0x3C


    /**
    * @brief  Low Calibration Temperature Register (MSB)
    * \code
    * Read
    * Default value: 0x00.
    * T0_OUT15 - T0_OUT8: temperature data MSB (2's complement) => signed 16 bits
    * RAW LOW Calibration data: T0_OUT (LSB)=(T0_OUT_H & T0_OUT_L).
    * \endcode
    */
#define HTS221_T0_OUT_H_ADDR                        0x3D


    /**
    * @brief  Low Calibration Temperature Register (LSB).
    * \code
    * Read
    * Default value: 0x00.
    * T1_OUT7 - T1_OUT0: temperature data LSB (2's complement) => signed 16 bits
    *  RAW LOW Calibration data: T1_OUT (LSB)=(T1_OUT_H & T1_OUT_L).
    * \endcode
    */
#define HTS221_T1_OUT_L_ADDR                        0x3E


    /**
    * @brief  Low Calibration Temperature Register (MSB)
    * \code
    * Read
    * Default value: 0x00.
    * T1_OUT15 - T1_OUT8: temperature data MSB (2's complement) => signed 16 bits
    * RAW LOW Calibration data: T1_OUT (LSB)=(T1_OUT_H & T1_OUT_L).
    * \endcode
    */
#define HTS221_T1_OUT_H_ADDR                        0x3F


/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/

/**
 * @brief Multiple Byte. Mask for enabling multiple byte read/write command.
 */   
#define HTS221_I2C_MULTIPLEBYTE_CMD                      ((uint8_t)0x80)
   
/**
 * @brief Device Identifier. Default value of the WHO_AM_I register.
 */
#define I_AM_HTS221                         ((uint8_t)0xBC)



#define HTS221_MODE_POWERDOWN               ((uint8_t)0x00)
#define HTS221_MODE_ACTIVE                  ((uint8_t)0x80)

#define HTS221_MODE_MASK                    ((uint8_t)0x80)




#define HTS221_BDU_CONTINUOUS               ((uint8_t)0x00)
#define HTS221_BDU_NOT_UNTIL_READING        ((uint8_t)0x04)

#define HTS221_BDU_MASK                     ((uint8_t)0x04)



#define HTS221_ODR_ONE_SHOT             ((uint8_t)0x00) /*!< Output Data Rate: H - one shot, T - one shot */
#define HTS221_ODR_1Hz                  ((uint8_t)0x01) /*!< Output Data Rate: H - 1Hz, T - 1Hz */
#define HTS221_ODR_7Hz                  ((uint8_t)0x02) /*!< Output Data Rate: H - 7Hz, T - 7Hz */
#define HTS221_ODR_12_5Hz               ((uint8_t)0x03) /*!< Output Data Rate: H - 12.5Hz, T - 12.5Hz */

#define HTS221_ODR_MASK                 ((uint8_t)0x03)


#define HTS221_BOOT_NORMALMODE              ((uint8_t)0x00)
#define HTS221_BOOT_REBOOTMEMORY            ((uint8_t)0x80)

#define HTS221_BOOT_MASK                    ((uint8_t)0x80)



#define HTS221_ONE_SHOT_START               ((uint8_t)0x01)

#define HTS221_ONE_SHOT_MASK                ((uint8_t)0x01)



#define HTS221_PP_OD_PUSH_PULL              ((uint8_t)0x00)
#define HTS221_PP_OD_OPEN_DRAIN             ((uint8_t)0x40)

#define HTS221_PP_OD_MASK                   ((uint8_t)0x40)




#define HTS221_DRDY_DISABLE                 ((uint8_t)0x00)
#define HTS221_DRDY_AVAILABLE               ((uint8_t)0x04)

#define HTS221_DRDY_MASK                    ((uint8_t)0x04)



#define HTS221_H_RES_AVG_4                  ((uint8_t)0x00)
#define HTS221_H_RES_AVG_8                  ((uint8_t)0x01)
#define HTS221_H_RES_AVG_16                 ((uint8_t)0x02)
#define HTS221_H_RES_AVG_32                 ((uint8_t)0x03)
#define HTS221_H_RES_AVG_64                 ((uint8_t)0x04)
#define HTS221_H_RES_AVG_128                ((uint8_t)0x05)

#define HTS221_H_RES_MASK                   ((uint8_t)0x07)

#define HTS221_T_RES_AVG_2                  ((uint8_t)0x00)
#define HTS221_T_RES_AVG_4                  ((uint8_t)0x08)
#define HTS221_T_RES_AVG_8                  ((uint8_t)0x10)
#define HTS221_T_RES_AVG_16                 ((uint8_t)0x18)
#define HTS221_T_RES_AVG_32                 ((uint8_t)0x20)
#define HTS221_T_RES_AVG_64                 ((uint8_t)0x28)

#define HTS221_T_RES_MASK                   ((uint8_t)0x38)

#define HTS221_H_DATA_AVAILABLE_MASK        ((uint8_t)0x02)
#define HTS221_T_DATA_AVAILABLE_MASK        ((uint8_t)0x01)


/* Data resolution */
#define HUM_DECIMAL_DIGITS                  (2)
#define TEMP_DECIMAL_DIGITS                 (2)
  


/* ------------------------------------------------------- */ 
/* Here you should declare the internal struct of          */
/* extended features of HTS221. See the example of         */
/* LSM6DS3 in lsm6ds3.h                                    */
/* ------------------------------------------------------- */


/* HUM_TEMP sensor driver structure */
//HUM_TEMP_DrvTypeDef Hts221Drv;
//HUM_TEMP_DrvExtTypeDef Hts221Drv_ext;



/**
 * @brief Initialize HTS221.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus hts221_sensor_init(void);


/**
 * @brief Read HTS221 value.
 *
 * @param temperature: value of temperature
 * @param humidity:    value of humidity
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus hts221_Read_Data(float *temperature,float *humidity);


/**
 * @brief Deinitialize HTS221.
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
OSStatus hts221_sensor_deinit(void);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
  
#ifdef __cplusplus
  }
#endif
  
#endif /* __HTS221_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/ 
