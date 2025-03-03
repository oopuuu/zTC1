/****************************************************************************//**
 * @file     mc200_adc.c
 * @brief    This file provides ADC functions.
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

#include "mc200.h"
#include "mc200_adc.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @defgroup ADC ADC
 *  @brief ADC driver modules
 *  @{
 */

/** @defgroup ADC_Private_Type
 *  @{
 */
const static uint32_t adcAddr[2] = {ADC0_BASE, ADC1_BASE};

/*@} end of group ADC_Private_Type*/

/** @defgroup ADC_Private_Defines
 *  @{
 */

/*@} end of group ADC_Private_Defines */


/** @defgroup ADC_Private_Variables
 *  @{
 */


/*@} end of group ADC_Private_Variables */

/** @defgroup ADC_Global_Variables
 *  @{
 */


/*@} end of group ADC_Global_Variables */


/** @defgroup ADC_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group ADC_Private_FunctionDeclaration */

/** @defgroup ADC_Private_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief  ADC interrupt function 
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
static void ADC_IRQHandler(ADC_ID_Type adcID, INT_Peripher_Type intPeriph)
{
  uint32_t intStatus;
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  /* Store unmasked interrupt flags for later use */
  intStatus = (~ADCx->IMR.WORDVAL) & 0x1F;
  intStatus &= ADCx->ISR.WORDVAL;
    
  /* Clear all unmasked interrupt flags */
  ADCx->ICR.WORDVAL = intStatus;
  
  /* ADC data ready inerrupt */
  if( intStatus & (1 << ADC_RDY) )
  {
    if(intCbfArra[intPeriph][ADC_RDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_RDY]();
    }
    else
    {
      ADCx->IMR.BF.RDY_MASK = 1;
    }
  }

  /* ADC gain correction saturation interrupt */
  if( intStatus & (1 << ADC_GAINSAT) )
  {
    if(intCbfArra[intPeriph][ADC_GAINSAT] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_GAINSAT]();
    }
    else
    {
      ADCx->IMR.BF.GAINSAT_MASK = 1;
    }
  }
  
  /* ADC offset correcion saturation interrupt */
  if( intStatus & (1 << ADC_OFFSAT) )
  {
    if(intCbfArra[intPeriph][ADC_OFFSAT] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_OFFSAT]();
    }
    else
    {
      ADCx->IMR.BF.OFFSAT_MASK = 1;
    } 
  }  
 
  /* ADC DMA data transfer failure interrupt */
  if( intStatus & (1 << ADC_DMAERR) )
  {
    if(intCbfArra[intPeriph][ADC_DMAERR] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_DMAERR]();
    }
    else
    {
      ADCx->IMR.BF.DMA_ERR_MASK = 1;
    } 
  }  

  /* ADC digital filtering saturation interrupt */
  if( intStatus & (1 << ADC_FILTERSAT) )
  {
    if(intCbfArra[intPeriph][ADC_FILTERSAT] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_FILTERSAT]();
    }
    else
    {
      ADCx->IMR.BF.FILTERSAT_MASK = 1;
    } 
  }    
}

/*@} end of group ADC_Private_Functions */

/** @defgroup ADC_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Reset ADC block 
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Reset(ADC_ID_Type adcID)
{
  volatile uint32_t i;
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));

  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  /* Software reset the ADC block */
  ADCx->CLKRST.BF.SOFT_RST = 0x1;  
  
  /* Delay */
  for(i=0; i<10; i++);
  
  ADCx->CLKRST.BF.SOFT_RST = 0x0;  
}

/****************************************************************************//**
 * @brief      Initializes the ADC with pre-defined ADC configuration structure
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcConfigSet:  Pointer to a ADC configuration structure
 *
 * @return none
 *
 * Initializes the ADC 
 *******************************************************************************/
void ADC_Init(ADC_ID_Type adcID, ADC_CFG_Type* adcConfigSet)
{
  volatile uint32_t i;
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  /* Set ADC conversion resolution */
  ADCx->ANA.BF.OSR = adcConfigSet->adcResolution;
  
  /* Set ADC reference source */
  ADCx->ANA.BF.VREF_SEL = adcConfigSet->adcVrefSource; 
  
  /* Set ADC PGA gain */
  ADCx->ANA.BF.PGA = adcConfigSet->adcGainSel; 
  
  /* Set ADC internal clock divider */
  ADCx->CLKRST.BF.SOFT_CLK_RST = 1;
  
  /* Delay */
  for(i=0; i<10; i++);
  
  ADCx->CLKRST.BF.SOFT_CLK_RST = 0;
  
  ADCx->CLKRST.BF.INT_CLK_DIV = adcConfigSet->adcClockDivider;
  
  /* Set ADC bias mode */
  ADCx->ANA.BF.BIAS_SEL = adcConfigSet->adcBiasMode;
}

/****************************************************************************//**
 * @brief      Enable the ADC
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Enable(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  ADCx->PWR.BF.GLOBAL_EN = 1;
}

/****************************************************************************//**
 * @brief      Disable the ADC
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Disable(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));

  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->PWR.BF.GLOBAL_EN = 0;
}

/****************************************************************************//**
 * @brief      Start the ADC
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void ADC_ConversionStart(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  ADCx->CMD.BF.CONV_START = 1;
}

/****************************************************************************//**
 * @brief      Stop the ADC
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void ADC_ConversionStop(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  ADCx->CMD.BF.CONV_START = 0;
}

/****************************************************************************//**
 * @brief      Set the ADC channel
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcChannelType:  ADC channel selection
 *
 * @return none
 *
 * Set the ADC channel
 *******************************************************************************/
void ADC_ChannelConfig(ADC_ID_Type adcID, ADC_Channel_Type adcChannelType)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  /* single or differential mode */
  ADCx->ANA.BF.SINGLEDIFF = (adcChannelType >> 4);
  
  /* Assign channel */
  ADCx->ANA.BF.AMUX_SEL = (adcChannelType & 0xF);
}


/****************************************************************************//**
 * @brief      Get the ADC conversion final result
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return     ADC conversion final result
 *
 *******************************************************************************/
int16_t ADC_GetConversionResult(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  return (int16_t) ADCx->RESULT.BF.DATA;
}

/****************************************************************************//**
* @brief      Select the operation mode --- ADC / Tsensor 
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  mode: select the mode.
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_ModeSelect(ADC_ID_Type adcID, ADC_Mode_Type mode)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));

  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->ANA.BF.TS_EN = mode;
}

/****************************************************************************//**
 * @brief      Configure the ADC temparature sensor
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcChannelType:  ADC temparature sensor channel selection
 * @param[in]  adcTSensorMode:  ADC temparature internal/external sensor mode selection
 *
 * @return none
 *
 * Configure the ADC temparature sensor
 *******************************************************************************/
void ADC_TSensorConfig(ADC_ID_Type adcID, ADC_Channel_Type adcChannelType, ADC_TSensorMode_Type adcTSensorMode)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  /* single or differential mode */
  ADCx->ANA.BF.SINGLEDIFF = (adcChannelType >> 4);
  
  /* Assign TSensor channel */
  ADCx->ANA.BF.AMUX_SEL = (adcChannelType & 0xF);
  
  /* select the diode for Tsensor */
  ADCx->ANA.BF.EXT_SEL = adcTSensorMode;
}

/****************************************************************************//**
 * @brief      Set ADC DMA mode 
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  newCmd:  Enable/disable ADC DMA mode
 *
 * @return none
 *
 *******************************************************************************/
void ADC_DmaCmd(ADC_ID_Type adcID, FunctionalState newCmd)
{  
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));

  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->DMAR.BF.DMA_EN = newCmd;
}

/****************************************************************************//**
 * @brief      Set ADC buffer mode 
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcInBuf:  ADC inbut gain buffer mode
 * @param[in]  adcVrefBuf:  ADC Vref buffer mode
 *
 * @return none
 *
 *******************************************************************************/
void ADC_SetBufferMode(ADC_ID_Type adcID, FunctionalState adcInGainBuf, FunctionalState adcVrefBuf)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
    
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  /* Set input buffer mode */
  ADCx->ANA.BF.IN_BFSEL = adcInGainBuf;
  
  /* Set reference buffer mode */
  ADCx->ANA.BF.VREF_BFSEL = adcVrefBuf;
}

/****************************************************************************//**
 * @brief      Start ADC calibration 
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  sysOffsetCalVal: system offset calibration value
 *
 * @return     ADC calibration status
 *
 *******************************************************************************/
Status ADC_Calibration(ADC_ID_Type adcID, int16_t sysOffsetCalVal)
{
  adc_reg_t * ADCx;

  uint32_t adcScale;
  
  uint32_t internalGainConst;
  uint32_t externalGainConst = 0;
  
  uint32_t adcVrefSource;
  uint32_t adcResolution;
  uint32_t adcGainSel;
  uint32_t adcModeSel;
  
  uint8_t sysGainCal;
  uint32_t fullScale;
  
  /* save ANA DMACmd MASK register value*/
  uint32_t tempConfig;
  uint32_t tempDmaCmdConfig;
  uint32_t tempMaskConfig;
  
  Status statusOverflow;
  uint16_t adcResult1, adcResult2;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));

  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  statusOverflow = DSUCCESS;
  
  adcResolution = ADCx->ANA.BF.OSR;
  adcVrefSource = ADCx->ANA.BF.VREF_SEL;
  adcGainSel = ADCx->ANA.BF.PGA;
  adcModeSel = ADCx->ANA.BF.TS_EN;
  
  /* judge internal Vref or external Vref*/
  if( (adcVrefSource != ADC_VREF_EXTERNAL) || (ADC_EXT_VREF < 1200) )
  {
    sysGainCal = 0;
  }
  else
  {
    sysGainCal = 1;    
  }
  
  /* Adjust adc scale constant according to resolution */
  switch(adcResolution)
  {
    case ADC_RESOLUTION_16BIT:
      adcScale = ADC_SCALE_16B;
      break;
    case ADC_RESOLUTION_14BIT:
      adcScale = ADC_SCALE_14B;
      break;
    case ADC_RESOLUTION_12BIT:
      adcScale = ADC_SCALE_12B;
      break;
    case ADC_RESOLUTION_10BIT:
      adcScale = ADC_SCALE_10B;
      break;
    default:
      return DERROR;
  }

  /* if in Temp_Sensor mode, resolution = 14 bit*/
  if(adcModeSel == ADC_MODE_TSENSOR)
  {
    adcScale = ADC_SCALE_14B;
  }

  /* when gain is 0.5, when Vref input, it value is half of full scale*/
  if(adcGainSel == ADC_GAIN_0P5)
  {
    adcScale = adcScale>>1;
  }

  /*calculate internal gain const and external gain const*/
  internalGainConst = adcScale << 15;
  fullScale = adcScale;
  if(sysGainCal)
  {
    externalGainConst = ((ADC_INT_VREF * adcScale) / ADC_EXT_VREF ) << 15;
  }
  

  /********************** system offset calibration *****************************/
  
  /* write system offset calibrition value */
  ADCx->OFF_CAL.BF.SYS_CAL = sysOffsetCalVal;
    
  /********************** self offset calibration *****************************/
  
  /* Save ADC setting */
  tempConfig = ADCx->ANA.WORDVAL;
  tempDmaCmdConfig = ADCx->DMAR.WORDVAL;
  tempMaskConfig = ADCx->IMR.WORDVAL;

  /* Stop Conversion */
  ADC_ConversionStop(adcID);

  /* mask all interrupt*/
  ADC_IntMask(adcID,ADC_INT_ALL,MASK);

  /* Disbale dma cmd*/
  ADCx->DMAR.BF.DMA_EN = 0;

  /* adc mode*/
  ADCx->ANA.BF.TS_EN = 0;

  /* if in Temp_Sensor mode, resolution = 14 bit*/
  if(adcModeSel == ADC_MODE_TSENSOR)
  {
    ADCx->ANA.BF.OSR = ADC_RESOLUTION_14BIT;
  }
  
  /* Set ADC calibration mode */
  ADCx->ANA.BF.CAL = 1;
  
  /* Assign channel VSSA for self offset calibration */
  ADC_ChannelConfig(adcID, ADC_VSSA);
  
  /* Set ADC PGA gain */
  if(adcGainSel == ADC_GAIN_2)
  {
    ADCx->ANA.BF.PGA = ADC_GAIN_1;
  }
  
  /* Clear GPADC_RDY_RAW */
  ADC_IntClr(adcID, ADC_RDY);
  
  /* Start ADC conversion */  
  ADC_ConversionStart(adcID);
  
  /* Waitting for ADC conversion done */
  while(ADC_GetStatus(adcID, ADC_STATUS_RDY) != SET);
  
  /* Stop Conversion */
  ADC_ConversionStop(adcID);
  
  ADCx->ANA.BF.CAL = 0;
  
  /********************** gain calibration *****************************/
  
  /********************** self gain calibration ********************/  
  /* Assign channel VREF for self gain calibration */
  ADC_ChannelConfig(adcID, ADC_VREF);
  
  /* select internal 1.2V reference */
  ADCx->ANA.BF.VREF_SEL = ADC_VREF_INTERNAL;
  
  /* Clear RDY_RAW */
  ADC_IntClr(adcID, ADC_RDY);
  
  /* Start ADC conversion */ 
  ADC_ConversionStart(adcID);
  
  /* Waitting for ADC conversion done */
  while(ADC_GetStatus(adcID, ADC_STATUS_RDY) != SET);
  
  /* Get ADC conversion result via self gain calibration */
  adcResult1 = ADC_GetConversionResult(adcID);
  
  /* Stop Conversion */
  ADC_ConversionStop(adcID);
  
  if ( adcResult1 < (fullScale >>1) )
  {
    /* skip the system calibration */
    statusOverflow = DERROR;
  }
  else
  {
    if(sysGainCal == 0)
    {
      /* Fill in gain calibration coefficient */
      ADCx->GAIN_CAL.BF.GAIN_CAL = ((internalGainConst / adcResult1) & 0xFFFF);
    }
    else
    {
      /* calculate self gain calibration result */
      adcResult1 = ((internalGainConst / adcResult1) & 0xFFFF);
    
      /*********************** system gain calibration ***********************/  
      /* Assign channel VREF for self gain calibration */
      ADC_ChannelConfig(adcID, ADC_VREF);
  
      /* select external 1.25V reference */
      ADCx->ANA.BF.VREF_SEL = ADC_VREF_EXTERNAL;
  
      /* Clear RDY_RAW */
      ADC_IntClr(adcID, ADC_RDY);
  
      /* Start ADC conversion */    
      ADC_ConversionStart(adcID);
  
      /* Waitting for ADC conversion done */
      while(ADC_GetStatus(adcID, ADC_STATUS_RDY) != SET);
  
      /* Get ADC conversion result via offset calibration */
      adcResult2 = ADC_GetConversionResult(adcID);
      
      /* Stop Conversion */
      ADC_ConversionStop(adcID);  
    
      /* calculate gain calibration coefficient */
      if ( adcResult2 < (fullScale >>1) )
      {
        statusOverflow = DERROR;
      }
      else
      {              
        /* calculate system gain calibration result */
        adcResult2 = ((externalGainConst / adcResult2) & 0xFFFF);
      
        if( ( (uint32_t) adcResult1 * (uint32_t) adcResult2 ) > 0x7FFFFFFF )
        { 
          statusOverflow = DERROR;
        }
        else
        {
          /* Fill in gain calibration coefficient */
          ADCx->GAIN_CAL.BF.GAIN_CAL = (((uint32_t) adcResult1) * adcResult2)>>15;
        }
      }
    }    
  }
  
  /* clear all interrupt*/
  ADC_IntClr(adcID, ADC_INT_ALL);

  /* Restore ADC setting */
  ADCx->ANA.WORDVAL = tempConfig;
  ADCx->DMAR.WORDVAL = tempDmaCmdConfig;
  ADCx->IMR.WORDVAL = tempMaskConfig;
  
  return statusOverflow;
}

/****************************************************************************//**
 * @brief      Enable/Disable event trigger signals to ADC 
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  state:  enable/disable
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_TriggerCmd(ADC_ID_Type adcID, FunctionalState state)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  ADCx->CMD.BF.TRIGGER_EN = state;
}


/****************************************************************************//**
 * @brief      Select event trigger source for ADC 
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  trigSrc:  select the trigger source
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_TriggerSourceSel(ADC_ID_Type adcID, ADC_TrigSource_Type trigSrc)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  ADCx->CMD.BF.TRIGGER_SEL = trigSrc;
}

/****************************************************************************//**
 * @brief      Get ADC status 
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  statusType:  Specifies the status type
 *
 * @return     State value of the specified ADC status type
 *
 *******************************************************************************/
FlagStatus ADC_GetStatus(ADC_ID_Type adcID, ADC_Status_Type statusType)
{
  FlagStatus bitStatus;
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  CHECK_PARAM(IS_ADC_STATUS_TYPE(statusType));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  bitStatus = RESET;

  switch(statusType)
  {
    /* ADC ready status */
    case ADC_STATUS_RDY:    
      if (ADCx->IRSR.BF.RDY_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;

    /* ADC gain saturation status */
    case ADC_STATUS_GAINSAT:    
      if (ADCx->IRSR.BF.GAINSAT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;
    
    /* ADC offset saturation status */
    case ADC_STATUS_OFFSAT:    
      if (ADCx->IRSR.BF.OFFSAT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;      

    /* ADC dma error status */
    case ADC_STATUS_DMAERR:    
      if (ADCx->IRSR.BF.DMA_ERR_RAW == 1)
      {
        bitStatus = SET;
      }   
      break; 
      
    /* ADC filter saturation status */
    case ADC_STATUS_FILTERSAT:    
      if (ADCx->IRSR.BF.FILTERSAT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break; 
      
    /* ADC active status */
    case ADC_STATUS_ACTIVE:
      if(ADCx->STATUS.BF.ACT == 1)
      {
        bitStatus = SET;
      }
      break;      
      
    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Get ADC interrupt status 
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return     The state value of ADC interrupt status register
 *
 *******************************************************************************/
IntStatus ADC_GetIntStatus(ADC_ID_Type adcID, ADC_INT_Type intType)
{
  IntStatus bitStatus;
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  CHECK_PARAM(IS_ADC_INT_TYPE(intType));
  
  bitStatus = RESET;
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  switch(intType)
  {
    /* ADC ready interrupt */
    case ADC_RDY:    
      if (ADCx->ISR.BF.RDY == 1)
      {
        bitStatus = SET;
      }   
      break;

    /* ADC gain saturation interrupt */
    case ADC_GAINSAT:    
      if (ADCx->ISR.BF.GAINSAT == 1)
      {
        bitStatus = SET;
      }   
      break;
      
    /* ADC offset saturation interrupt */
    case ADC_OFFSAT:    
      if (ADCx->ISR.BF.OFFSAT == 1)
      {
        bitStatus = SET;
      }   
      break;      
 
    /* ADC dma error interrupt */
    case ADC_DMAERR:    
      if (ADCx->ISR.BF.DMA_ERR == 1)
      {
        bitStatus = SET;
      }   
      break;  
     
    /* ADC filter saturation interrupt */
    case ADC_FILTERSAT:    
      if (ADCx->ISR.BF.FILTERSAT == 1)
      {
        bitStatus = SET;
      }   
      break;     
      
    /* Any ADC interrupt */
    case ADC_INT_ALL:    
      if (ADCx->ISR.WORDVAL & 0x1F)
      {
        bitStatus = SET;
      }   
      break;       
      
    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Mask/Unmask specified ADC interrupt type 
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  intType:  Specifies the interrupt type
 * @param[in]  intMask:  Mask/Unmask specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void ADC_IntMask(ADC_ID_Type adcID, ADC_INT_Type intType, IntMask_Type intMask)
{
  uint32_t cmdAll;
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  CHECK_PARAM(IS_ADC_INT_TYPE(intType));  
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  if (intMask == UNMASK)
  {
    cmdAll = 0;
  }
  else
  {
    cmdAll = 0x1F;
  }  
  
  switch(intType)
  {
    /* ADC ready interrupt */
    case ADC_RDY:
      ADCx->IMR.BF.RDY_MASK = intMask;
      break;
     
    /* ADC gain saturation interrupt */
    case ADC_GAINSAT:
      ADCx->IMR.BF.GAINSAT_MASK = intMask;
      break;   
      
    /* ADC offset saturation interrupt */
    case ADC_OFFSAT:
      ADCx->IMR.BF.OFFSAT_MASK = intMask;
      break;      
      
    /* ADC dma error interrupt */
    case ADC_DMAERR:
      ADCx->IMR.BF.DMA_ERR_MASK = intMask;
      break;  
      
    /* ADC filter saturation interrupt */
    case ADC_FILTERSAT:
      ADCx->IMR.BF.FILTERSAT_MASK = intMask;
      break;    

    /* All ADC interrupts */
    case ADC_INT_ALL:  
      ADCx->IMR.WORDVAL = cmdAll;
      break; 
      
    default:
      break;
  }
}

 /****************************************************************************//**
 * @brief      Clear ADC interrupt flag 
 *
 * @param[in]  adcID:  select the ADC module 
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void ADC_IntClr(ADC_ID_Type adcID, ADC_INT_Type intType)
{
  adc_reg_t * ADCx;

  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  CHECK_PARAM(IS_ADC_INT_TYPE(intType));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);

  switch(intType)
  {
    /* ADC ready interrupt */
    case ADC_RDY:    
      ADCx->ICR.BF.RDY_CLR = 1;
      break;
    
    /* ADC gain saturation interrupt */
    case ADC_GAINSAT:    
      ADCx->ICR.BF.GAINSAT_CLR = 1;
      break;   
      
    /* ADC offset saturation interrupt */
    case ADC_OFFSAT:    
      ADCx->ICR.BF.OFFSAT_CLR = 1;
      break;      

    /* ADC dma error interrupt */
    case ADC_DMAERR:    
      ADCx->ICR.BF.DMA_ERR_CLR = 1;
      break;   
      
    /* ADC filter saturation interrupt */
    case ADC_FILTERSAT:    
      ADCx->ICR.BF.FILTERSAT_CLR = 1;
      break;   

    /* All ADC interrupts */
    case ADC_INT_ALL:    
      ADCx->ICR.WORDVAL = 0x1F;
      break;          
      
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief  ADC0 interrupt function 
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC0_IRQHandler(void)
{
  ADC_IRQHandler(ADC0_ID, INT_ADC0);
}

/****************************************************************************//**
 * @brief  ADC1 interrupt function 
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC1_IRQHandler(void)
{
  ADC_IRQHandler(ADC1_ID, INT_ADC1);
}


/*@} end of group UART_Public_Functions */

/*@} end of group UART_definitions */

/*@} end of group MC200_Periph_Driver */

