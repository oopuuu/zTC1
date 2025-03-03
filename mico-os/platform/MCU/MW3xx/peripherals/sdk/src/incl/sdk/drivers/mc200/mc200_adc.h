/****************************************************************************//**
 * @file     mc200_adc.h
 * @brief    ADC driver module header file.
 * @version  V1.2.0
 * @date     29-May-2013
 * @author   CE Application Team
 *
 * @note
 * Copyright (C) 2012 Marvell Technology Group Ltd. All rights reserved.
 *
 * @par
 * Marvell is supplying this software which provides customers with programming
 * information regarding the products. Marvell has no responsibility or 
 * liability for the use of the software. Marvell not guarantee the correctness 
 * of this software. Marvell reserves the right to make changes in the software 
 * without notification. 
 * 
 *******************************************************************************/

#ifndef __MC200_ADC_H__
#define __MC200_ADC_H__

/* Includes ------------------------------------------------------------------ */
#include "mc200_driver.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @addtogroup  ADC 
 *  @{
 */
  
/** @defgroup ADC_Public_Types ADC_Public_Types
 *  @{
 */

/**  
 *  @brief ADC ID type 
 */
typedef enum
{
  ADC0_ID = 0,                 /*!< ADC0 module */
  ADC1_ID = 1                  /*!< ADC1 module */
}ADC_ID_Type;

/**  
 *  @brief ADC mode type 
 */
typedef enum
{
  ADC_MODE_ADC = 0,           /*!< ADC mode */
  ADC_MODE_TSENSOR = 1        /*!< Temperature sensor mode */
}ADC_Mode_Type;

/**  
 *  @brief ADC clock divider type 
 */
typedef enum
{
  ADC_CLOCK_DIVIDER_4 = 0x03,      /*!< ADC clock is divided by 4 */
  ADC_CLOCK_DIVIDER_8 = 0x07,      /*!< ADC clock is divided by 8 */
  ADC_CLOCK_DIVIDER_16 = 0x0f,     /*!< ADC clock is divided by 16 */
  ADC_CLOCK_DIVIDER_32 = 0x1f,     /*!< ADC clock is divided by 32 */
}ADC_ClockDivider_Type; 

/**  
 *  @brief ADC power up type 
 */
typedef enum
{
  ADC_POWER_UPON_START  = 0,                 /*!< Powered up when conversion */
  ADC_POWER_UPON_ENABLE = 1                  /*!< Powered up once global enable */
}ADC_PowerUp_Type;

/**  
 *  @brief ADC trigger source type 
 */
typedef enum
{
  ADC_TRIGGER_SOURCE_GPT    = 0,             /*!< Trigger source 0 */
  ADC_TRIGGER_SOURCE_1      = 1,             /*!< Trigger source 1 */
  ADC_TRIGGER_SOURCE_2      = 2,             /*!< Trigger source 2 */
  ADC_TRIGGER_SOURCE_3      = 3,             /*!< Trigger source 3 */
}ADC_TrigSource_Type;

/**  
 *  @brief ADC PGA gain type
 */
typedef enum
{
  ADC_GAIN_0P5 = 0,                          /*!< PGA gain is 0.5 */
  ADC_GAIN_1   = 1,                          /*!< PGA gain is 1 */
  ADC_GAIN_2   = 2                           /*!< PGA gain is 2 */
}ADC_Gain_Type;

/**  
 *  @brief ADC bias mode type 
 */
typedef enum
{
  ADC_BIAS_FULL = 0,                          /*!< ADC full biasing mode */
  ADC_BIAS_HALF = 1                           /*!< ADC half biasing mode */
}ADC_Bias_Type;


/**  
 *  @brief ADC osr selection 
 */
typedef enum
{
  ADC_RESOLUTION_10BIT,                       /*!< ADC 10-bit resolution */
  ADC_RESOLUTION_12BIT,                       /*!< ADC 12-bit resolution */
  ADC_RESOLUTION_14BIT,                       /*!< ADC 14-bit resolution */
  ADC_RESOLUTION_16BIT                        /*!< ADC 16-bit resolution */
}ADC_Resolution_Type;

/**  
 *  @brief ADC channel mode selection 
 */
typedef enum
{
  ADC_CH0         = 0,                        /*!< Single mode, channel[0] and vssa */
  ADC_CH1         = 1,                        /*!< Single mode, channel[1] and vssa */
  ADC_CH2         = 2,                        /*!< Single mode, channel[2] and vssa */
  ADC_CH3         = 3,                        /*!< Single mode, channel[3] and vssa */
  ADC_CH4         = 4,                        /*!< Single mode, channel[4] and vssa */
  ADC_CH5         = 5,                        /*!< Single mode, channel[5] and vssa */
  ADC_CH6         = 6,                        /*!< Single mode, channel[6] and vssa */
  ADC_CH7         = 7,                        /*!< Single mode, channel[7] and vssa */
  ADC_VBATS       = 8,                        /*!< Single mode, vbat_s and vssa */
  ADC_VREF        = 9,                        /*!< Single mode, vref and vssa */
  ADC_DACA        = 10,                       /*!< Single mode, daca and vssa */
  ADC_DACB        = 11,                       /*!< Single mode, dacb and vssa */
  ADC_VSSA        = 12,                       /*!< Single mode, vssa and vssa */
  ADC_TEMPP       = 15,                       /*!< Single mode, temp_p and vssa */
  ADC_CH0_CH1     = 16,                       /*!< Differential mode, channel[0] and channel[1] */
  ADC_CH2_CH3     = 17,                       /*!< Differential mode, channel[2] and channel[3] */
  ADC_CH4_CH5     = 18,                       /*!< Differential mode, channel[4] and channel[5] */
  ADC_CH6_CH7     = 19,                       /*!< Differential mode, channel[6] and channel[7] */
  ADC_DACA_DACB   = 20,                       /*!< Differential mode, daca and dacb */
  ADC_TEMPP_TEMPN = 31                        /*!< Differential mode, temp_p and temp_n */
}ADC_Channel_Type;

/**  
 *  @brief ADC voltage reference selection 
 */
typedef enum
{
  ADC_VREF_VCAU     = 1,                      /*!< Vaa_cau as Reference */
  ADC_VREF_EXTERNAL = 2,                      /*!< External Single-ended Reference */
  ADC_VREF_INTERNAL = 3                       /*!< Internal 1.2V Reference */
}ADC_VrefSource_Type;


/**  
 *  @brief ADC temperature sensor mode type 
 */
typedef enum
{
  ADC_SENSOR_INTERNAL,                        /*!< Internal diode mode */
  ADC_SENSOR_EXTERNAL                         /*!< External diode mode */
}ADC_TSensorMode_Type;

/**  
 *  @brief ADC interrupt type definition 
 */
typedef enum
{
  ADC_RDY,                                    /*!< ADC conversion data ready flag */
  ADC_GAINSAT,                                /*!< Gain correction saturation flag */
  ADC_OFFSAT,                                 /*!< Offset correction saturation flag */
  ADC_DMAERR,                                 /*!< DMA data transfer failure flag */
  ADC_FILTERSAT,                              /*!< Digital filtering saturation flag */
  ADC_INT_ALL                                 /*!< All ADC interrupt flags */
}ADC_INT_Type; 

/**  
 *  @brief ADC status type definition 
 */
typedef enum
{
  ADC_STATUS_RDY,                             /*!< ADC conversion data ready status */
  ADC_STATUS_GAINSAT,                         /*!< Gain correction saturation status */
  ADC_STATUS_OFFSAT,                          /*!< Offset correction saturation status */
  ADC_STATUS_DMAERR,                          /*!< DMA data transfer failure status */
  ADC_STATUS_FILTERSAT,                       /*!< Digital filtering saturation status */
  ADC_STATUS_ACTIVE                           /*!< ADC activity status */
}ADC_Status_Type; 

/**  
 *  @brief ADC configure type 
 */
typedef struct
{
  ADC_Resolution_Type adcResolution;          /*!< Configure ADC resolution
                                                                                         ADC_RESOLUTION_10BIT (0):   ADC 10-bit resolution
                                                                                         ADC_RESOLUTION_12BIT (1):   ADC 12-bit resolution
                                                                                         ADC_RESOLUTION_14BIT (2):   ADC 14-bit resolution
                                                                                         ADC_RESOLUTION_16BIT (3):   ADC 16-bit resolution */
                                                                                         
  ADC_VrefSource_Type adcVrefSource;          /*!< Configure ADC reference source
                                                                                         ADC_VREF_VCAU (1): Vaa_cau as Reference
                                                                                         ADC_VREF_EXTERNAL (2): External single-ended reference 
                                                                                         ADC_VREF_INTERNAL (3): Internal 1.2V reference */

  ADC_Gain_Type adcGainSel;                   /*!< Configure ADC input buffer gain
                                                                                         ADC_GAIN_0P5 (0): PGA gain is 0.5 
                                                                                         ADC_GAIN_1 (1): PGA gain is 1
                                                                                         ADC_GAIN_2 (2): PGA gain is 2 */
                                                                                         
  ADC_ClockDivider_Type adcClockDivider;      /*!< Configure ADC internal clock divider. 
                                                                                          ADC_CLOCK_DIVIDER_4 (0x03): ADC clock is divided by 4 
                                                                                          ADC_CLOCK_DIVIDER_8 (0x07): ADC clock is divided by 8 
                                                                                          ADC_CLOCK_DIVIDER_16 (0x0f): ADC clock is divided by 16 
                                                                                          ADC_CLOCK_DIVIDER_32 (0x1f): ADC clock is divided by 32 */
																						 
  ADC_Bias_Type adcBiasMode;                  /*!< Configure ADC bias mode
                                                                                         ADC_BIAS_FULL (0): ADC full biasing mode 
                                                                                         ADC_BIAS_HALF (1): ADC half biasing mode */
                                                                                         
}ADC_CFG_Type;

/*@} end of group ADC_Public_Types definitions */


/** @defgroup ADC_Public_Constants
 *  @{
 */ 


/**
 *  @brief Total channel number 
 */
#define ADC_NUMBER        2

/**
 *  @brief Total interrupt number 
 */
#define ADC_INT_NUMBER            5

 /**
 *  @brief Total status number 
 */
#define ADC_STATUS_NUMBER         6

/**        
 *  @brief ADC modules check
 */
#define IS_ADC_PERIPH(PERIPH)        ((PERIPH) < ADC_NUMBER)

/**        
 *  @brief ADC interrupt type check
 */
#define IS_ADC_INT_TYPE(INT_TYPE)    ((INT_TYPE) <= ADC_INT_NUMBER)

/**        
 *  @brief ADC status type check
 */
#define IS_ADC_STATUS_TYPE(STS_TYPE)    ((STS_TYPE) < ADC_STATUS_NUMBER)
                                                                                         

/*@} end of group ADC_Public_Constants */

/** @defgroup ADC_Public_Macro
 *  @{
 */

/** 
 *  @brief ADC full scale for single mode
 */
#define ADC_SCALE_16B 0x7FFF
#define ADC_SCALE_14B 0x1FFF
#define ADC_SCALE_12B 0x07FF
#define ADC_SCALE_10B 0x01FF

/** 
 * @brief ADC internal reference voltage, unit mV
 */
#define ADC_INT_VREF 1200

/**
 * @brief ADC external reference voltage, unit mV
 */
#define ADC_EXT_VREF 1250

/*@} end of group ADC_Public_Macro */

/** @defgroup ADC_Public_FunctionDeclaration
 *  @brief ADC functions statement
 *  @{
 */
void ADC_Reset(ADC_ID_Type adcID);
void ADC_Init(ADC_ID_Type adcID, ADC_CFG_Type* adcConfigSet);
void ADC_Enable(ADC_ID_Type adcID);
void ADC_Disable(ADC_ID_Type adcID);
void ADC_ConversionStart(ADC_ID_Type adcID);
void ADC_ConversionStop(ADC_ID_Type adcID);

void ADC_ChannelConfig(ADC_ID_Type adcID, ADC_Channel_Type adcChannelType);
Status ADC_Calibration(ADC_ID_Type adcID, int16_t sysOffsetCalVal);
void ADC_SetBufferMode(ADC_ID_Type adcID, FunctionalState adcInGainBuf, FunctionalState adcVrefBuf);
int16_t ADC_GetConversionResult(ADC_ID_Type adcID);
void ADC_DmaCmd(ADC_ID_Type adcID, FunctionalState newCmd);

void ADC_ModeSelect(ADC_ID_Type adcID, ADC_Mode_Type mode);
void ADC_TSensorConfig(ADC_ID_Type adcID, ADC_Channel_Type adcChannelType, ADC_TSensorMode_Type adcTSensorMode);

void ADC_TriggerCmd(ADC_ID_Type adcID, FunctionalState state);
void ADC_TriggerSourceSel(ADC_ID_Type adcID, ADC_TrigSource_Type trigSrc);

FlagStatus ADC_GetStatus(ADC_ID_Type adcID, ADC_Status_Type statusType);
IntStatus ADC_GetIntStatus(ADC_ID_Type adcID, ADC_INT_Type intType);
void ADC_IntMask(ADC_ID_Type adcID, ADC_INT_Type intType, IntMask_Type intMask);
void ADC_IntClr(ADC_ID_Type adcID, ADC_INT_Type intType);
void ADC0_IRQHandler(void);
void ADC1_IRQHandler(void);

/*@} end of group ADC_Public_FunctionDeclaration */

/*@} end of group ADC */

/*@} end of group MC200_Periph_Driver */
#endif

