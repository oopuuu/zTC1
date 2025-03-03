/****************************************************************************//**
 * @file     mc200_acomp.c
 * @brief    This file provides ACOMP functions.
 * @version  V1.0.0
 * @date     06-Feb-2013
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
#include "mc200_acomp.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @defgroup ACOMP ACOMP
 *  @brief ACOMP driver modules
 *  @{
 */

/** @defgroup ACOMP_Private_Type
 *  @{
 */

/*@} end of group ACOMP_Private_Type*/


/** @defgroup ACOMP_Private_Defines
 *  @{
 */


/*@} end of group ACOMP_Private_Defines */


/** @defgroup ACOMP_Private_Variables
 *  @{
 */

/*@} end of group ACOMP_Private_Variables */

/** @defgroup ACOMP_Global_Variables
 *  @{
 */

/*@} end of group ACOMP_Global_Variables */


/** @defgroup ACOMP_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group ACOMP_Private_FunctionDeclaration */

/** @defgroup ACOMP_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief  ACOMP function interrupt handler function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_IRQHandler(void)
{
  uint32_t acomp0IntStat, acomp1IntStat;
  
  /* store interrupt flags for later use */
  acomp0IntStat = ACOMP->ISR[0].WORDVAL;  
  acomp1IntStat = ACOMP->ISR[1].WORDVAL;
  
  /* clear interrupt flags that are set to 1 */
  ACOMP->ICR[0].WORDVAL = acomp0IntStat;
  ACOMP->ICR[1].WORDVAL = acomp1IntStat;  
  
  /* ACOMP0 synchronized interrupt */
  if((acomp0IntStat & 1) != 0)
  {
    if(intCbfArra[INT_ACOMP][ACOMP_INT_OUT_0] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_ACOMP][ACOMP_INT_OUT_0]();
    }
    else
    {
      ACOMP->IMR[0].BF.OUT_INT_MASK = 1;
    }
  }
  
  /* ACOMP0 asynchronized interrupt */
  if((acomp0IntStat & 2) != 0)
  {
    if(intCbfArra[INT_ACOMP][ACOMP_INT_OUTA_0] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_ACOMP][ACOMP_INT_OUTA_0]();
    }
    else
    {
      ACOMP->IMR[0].BF.OUTA_INT_MASK = 1;
    }
  }
  
  /* ACOMP1 synchronized interrupt */
  if((acomp1IntStat & 1) != 0)
  {
    if(intCbfArra[INT_ACOMP][ACOMP_INT_OUT_1] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_ACOMP][ACOMP_INT_OUT_1]();
    }
    else
    {
      ACOMP->IMR[1].BF.OUT_INT_MASK = 1;
    }
  }
  
  /* ACOMP1 asynchronized interrupt */
  if((acomp1IntStat & 2) != 0)
  {
    if(intCbfArra[INT_ACOMP][ACOMP_INT_OUTA_1] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_ACOMP][ACOMP_INT_OUTA_1]();
    }
    else
    {
      ACOMP->IMR[1].BF.OUTA_INT_MASK = 1;
    }
  }
}

/*@} end of group ACOMP_Private_Functions */


/** @defgroup ACOMP_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Initialize the ACOMP 
 *
 * @param[in]  acompIdx:  ACOMP index to be configured
 * @param[in]  acompConfigSet:  ACOMP parameters to be configured
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_Init(ACOMP_ID_Type acompIdx, ACOMP_CFG_Type* acompConfigSet)
{
  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  
  /* Set hysteresis */
  ACOMP->CTRL[acompIdx].BF.HYST_SELP = acompConfigSet->posHyster;
  ACOMP->CTRL[acompIdx].BF.HYST_SELN = acompConfigSet->negHyster;
    
  /* Set output value for inactive state */
  ACOMP->CTRL[acompIdx].BF.INACT_VAL = acompConfigSet->inactValue;
    
  /* Set power mode */
  ACOMP->CTRL[acompIdx].BF.BIAS_PROG = acompConfigSet->powerMode;
    
  /* Set warm-up time */
  ACOMP->CTRL[acompIdx].BF.WARMTIME = acompConfigSet->warmupTime;
 
  /* Set pin output mode */
  ACOMP->ROUTE[acompIdx].BF.PE = !((acompConfigSet->outPinMode) >> 2);
  ACOMP->ROUTE[acompIdx].BF.OUTSEL = ((acompConfigSet->outPinMode) & 1);
  ACOMP->CTRL[acompIdx].BF.GPIOINV = (((acompConfigSet->outPinMode) & 2) >> 1);
}  

/****************************************************************************//**
 * @brief      Configure the ACOMP input channels
 *
 * @param[in]  acompIdx:  ACOMP index to be configured
 * @param[in]  posChannel:  Select positive input
 * @param[in]  negChannel:  Select negative input
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_ChannelConfig(ACOMP_ID_Type acompIdx, ACOMP_PosChannel_Type posChannel, ACOMP_NegChannel_Type negChannel)
{
  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  
  /* Config positive channel */
  ACOMP->CTRL[acompIdx].BF.POS_SEL = posChannel;
    
  /* Config negative channel */
  if( (negChannel == ACOMP_NEG_CH_VBAT_0P25) || (negChannel == ACOMP_NEG_CH_VBAT_0P50) || \
      (negChannel == ACOMP_NEG_CH_VBAT_0P75) || (negChannel == ACOMP_NEG_CH_VBAT_1P00) )
  {
    ACOMP->CTRL[acompIdx].BF.NEG_SEL = negChannel & 0xC;
    ACOMP->CTRL[acompIdx].BF.LEVEL_SEL = ((negChannel&0x3)<<4);
  }
  else
  {
    ACOMP->CTRL[acompIdx].BF.NEG_SEL = negChannel;
  }
}

/****************************************************************************//**
 * @brief      Congfigure the trigger source type for edge pulse
 *
 * @param[in]  acompIdx:  ACOMP index to be configured
 * @param[in]  trigSrc: Select the trigger source type
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_EdgePulseConfig(ACOMP_ID_Type acompIdx, ACOMP_EdgePulseTrigSrc_Type trigSrc)
{
  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  
  /* select the trigger source type for the edge pulse */
  ACOMP->CTRL[acompIdx].BF.RIE = trigSrc & 1;
  ACOMP->CTRL[acompIdx].BF.FIE = (trigSrc & 2) >> 1;
}

/****************************************************************************//**
 * @brief      Congfigure the interrupt trigger source type 
 *
 * @param[in]  acompIdx:  ACOMP index to be configured
 * @param[in]  intTrigSrc: Select the trigger source type
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_IntTrigSrcConfig(ACOMP_ID_Type acompIdx, ACOMP_IntTrig_Type intTrigSrc)
{
  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  CHECK_PARAM(IS_ACOMP_INT_TRIG_SRC_TYPE(intTrigSrc));
  
  /* Set interrupt trigger type */
  ACOMP->CTRL[acompIdx].BF.INT_ACT_HI = intTrigSrc & 1;
  ACOMP->CTRL[acompIdx].BF.EDGE_LEVL_SEL = (intTrigSrc & 2) >> 1;
}

/****************************************************************************//**
 * @brief      Congfigure the wake-up interrupt trigger source type 
 *
 * @param[in]  acompIdx:  ACOMP index to be configured
 * @param[in]  intTrigSrc: Select the trigger source type
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_WakeUpIntTrigSrcConfig(ACOMP_ID_Type acompIdx, ACOMP_IntTrig_Type intTrigSrc)
{
  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  CHECK_PARAM(IS_ACOMP_WAKEUP_INT_TRIG_SRC_TYPE(intTrigSrc));
  
  /* Set wake-up interrupt trigger source type */
  ACOMP->CTRL[acompIdx].BF.INT_ACT_HI = intTrigSrc & 1;
}
/****************************************************************************//**
 * @brief      Enable the selected ACOMP
 *
 * @param[in]  acompIdx:  ACOMP ID 
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_Enable(ACOMP_ID_Type acompIdx)
{
  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  
  /* Enable ACOMP */
  ACOMP->CTRL[acompIdx].BF.MUXEN = 1;
  ACOMP->CTRL[acompIdx].BF.EN = 1;
}

/****************************************************************************//**
 * @brief      Disable the selected ACOMP
 *
 * @param[in]  acompIdx:  ACOMP ID 
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_Disable(ACOMP_ID_Type acompIdx)
{
  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  
  /* Disable ACOMP */
  ACOMP->CTRL[acompIdx].BF.MUXEN = 0;
  ACOMP->CTRL[acompIdx].BF.EN = 0;

}

/****************************************************************************//**
 * @brief      Reset ACOMP
 *
 * @param[in]  acompIdx:  ACOMP ID
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_Reset(ACOMP_ID_Type acompIdx)
{
  volatile uint32_t i;
  
  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  
  ACOMP->RST[acompIdx].BF.SOFT_RST = 1;
  
  /* Delay */
  for(i=0; i<10; i++);
  
  ACOMP->RST[acompIdx].BF.SOFT_RST = 0;
}

/****************************************************************************//**
 * @brief      Get ACOMP result
 *
 * @param[in]  acompIdx:  ACOMP ID
 *
 * @return     The selected ACOMP output value
 *
 *******************************************************************************/
LogicalStatus ACOMP_GetResult(ACOMP_ID_Type acompIdx)
{
  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  
  /* Get ACOMP result */
  return (LogicalStatus)ACOMP->STATUS[acompIdx].BF.OUT;
}

/****************************************************************************//**
 * @brief      Get ACOMP status 
 *
 * @param[in]  acompIdx:  ACOMP ID 
 * @param[in]  statusType:  Specifies the status type
 *
 * @return     State value of the specified ACOMP status type
 *
 *******************************************************************************/
FlagStatus ACOMP_GetStatus(ACOMP_ID_Type acompIdx, ACOMP_Status_Type statusType)
{
  FlagStatus bitStatus = RESET;

  CHECK_PARAM(IS_ACOMP_PERIPH(acompIdx));
  CHECK_PARAM(IS_ACOMP_STATUS_TYPE(statusType));
  
  switch(statusType)
  {
    /* Synchronized output status */
    case ACOMP_STATUS_OUT:    
      if (ACOMP->IRSR[acompIdx].BF.OUT_INT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;

    /* Asynchronized output status */
    case ACOMP_STATUS_OUTA:    
      if (ACOMP->IRSR[acompIdx].BF.OUTA_INT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;

    /* active status */
    case ACOMP_STATUS_ACTIVE:    
      if (ACOMP->STATUS[acompIdx].BF.ACT == 1)
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
 * @brief      Get ACOMP interrupt status 
 *
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return     The state value of ACOMP interrupt status register
 *
 *******************************************************************************/
IntStatus ACOMP_GetIntStatus(ACOMP_INT_Type intType)
{
  IntStatus bitStatus = RESET;

  CHECK_PARAM(IS_ACOMP_INT_TYPE(intType));
  
  switch(intType)
  {
    /* ACOMP0 synchronized output interrupt */
    case ACOMP_INT_OUT_0:    
      bitStatus = (ACOMP->ISR[0].BF.OUT_INT) ? SET : RESET;
      break;

    /* ACOMP0 asynchronized output interrupt */
    case ACOMP_INT_OUTA_0:    
      bitStatus = (ACOMP->ISR[0].BF.OUTA_INT) ? SET : RESET;  
      break;
      
    /* ACOMP1 synchronized output interrupt */
    case ACOMP_INT_OUT_1:    
      bitStatus = (ACOMP->ISR[1].BF.OUT_INT) ? SET : RESET;
      break;

    /* ACOMP0 asynchronized output interrupt */
    case ACOMP_INT_OUTA_1:    
      bitStatus = (ACOMP->ISR[1].BF.OUTA_INT) ? SET : RESET;  
      break;  
      
    /* Any ACOMP interrupt */
    case ACOMP_INT_ALL:   
      bitStatus = ((ACOMP->ISR[0].WORDVAL & 0x3)||(ACOMP->ISR[1].WORDVAL & 0x3)) ? SET : RESET;  
      break;      
      
    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Mask/Unmask specified ACOMP interrupt type 
 *
 * @param[in]  intType:  Specifies the interrupt type
 * @param[in]  intMask:  Mask/Unmask specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_IntMask(ACOMP_INT_Type intType, IntMask_Type intMask)
{
  uint32_t cmdAll;

  CHECK_PARAM(IS_ACOMP_INT_TYPE(intType));
  CHECK_PARAM(IS_INTMASK(intMask));

  if (intMask == UNMASK)
  {
    cmdAll = 0;
  }
  else
  {
    cmdAll = 0x3;
  }
  
  switch(intType)
  {
    /* ACOMP0 synchronized output interrupt */
    case ACOMP_INT_OUT_0:
      ACOMP->IMR[0].BF.OUT_INT_MASK = intMask;
      break;
      
    /* ACOMP0 asynchronized output interrupt */
    case ACOMP_INT_OUTA_0:
      ACOMP->IMR[0].BF.OUTA_INT_MASK = intMask;
      break;      
      
    /* ACOMP1 synchronized output interrupt */
    case ACOMP_INT_OUT_1:
      ACOMP->IMR[1].BF.OUT_INT_MASK = intMask;
      break;
      
    /* ACOMP1 asynchronized output interrupt */
    case ACOMP_INT_OUTA_1:
      ACOMP->IMR[1].BF.OUTA_INT_MASK = intMask;
      break;  
      
    /* All ACOMP interrupts */
    case ACOMP_INT_ALL:
      ACOMP->IMR[0].WORDVAL = cmdAll;
      ACOMP->IMR[1].WORDVAL = cmdAll;
      break;   
      
    default:
      break;
  }
}

 /****************************************************************************//**
 * @brief      Clear ACOMP interrupt flag 
 *
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void ACOMP_IntClr(ACOMP_INT_Type intType)
{
  CHECK_PARAM(IS_ACOMP_INT_TYPE(intType));
    
  switch(intType)
  {
    /* ACOMP0 synchronized output interrupt */
    case ACOMP_INT_OUT_0:    
      ACOMP->ICR[0].BF.OUT_INT_CLR = 1;
      break;
    
    /* ACOMP0 asynchronized output interrupt */
    case ACOMP_INT_OUTA_0:    
      ACOMP->ICR[0].BF.OUTA_INT_CLR = 1;
      break;  
      
    /* ACOMP0 synchronized output interrupt */
    case ACOMP_INT_OUT_1:    
      ACOMP->ICR[1].BF.OUT_INT_CLR = 1;
      break;
    
    /* ACOMP0 asynchronized output interrupt */
    case ACOMP_INT_OUTA_1:    
      ACOMP->ICR[1].BF.OUTA_INT_CLR = 1;
      break;   
      
    /* All ACOMP interrupts */
    case ACOMP_INT_ALL:    
      ACOMP->ICR[0].WORDVAL = 0x3;
      ACOMP->ICR[1].WORDVAL = 0x3;
      break;         

    default:
      break;
  }
}


/*@} end of group ACOMP_Public_Functions */

/*@} end of group ACOMP_definitions */

/*@} end of group MC200_Periph_Driver */

