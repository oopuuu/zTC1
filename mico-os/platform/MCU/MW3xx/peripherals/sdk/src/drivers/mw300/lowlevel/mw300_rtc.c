/****************************************************************************//**
 * @file     mw300_rtc.c
 * @brief    This file provides RTC functions.
 * @version  V1.0.0
 * @date     12-Aug-2013
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

#include "mw300.h"
#include "mw300_rtc.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup RTC RTC
 *  @brief RTC driver modules
 *  @{
 */

/** @defgroup RTC_Private_Type
 *  @{
 */

/*@} end of group RTC_Private_Type*/

/** @defgroup RTC_Private_Defines
 *  @{
 */

/*@} end of group RTC_Private_Defines */


/** @defgroup RTC_Private_Variables
 *  @{
 */


/*@} end of group RTC_Private_Variables */

/** @defgroup RTC_Global_Variables
 *  @{
 */


/*@} end of group RTC_Global_Variables */


/** @defgroup RTC_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group RTC_Private_FunctionDeclaration */

/** @defgroup RTC_Private_Functions
 *  @{
 */


/*@} end of group RTC_Private_Functions */

/** @defgroup RTC_Public_Functions
 *  @{
 */

 
/****************************************************************************//**
 * @brief      Reset RTC counter 
 *
 * @param[in]  None
 *
 * @return reset status
 *  
 * Reset the RTC counter
 *******************************************************************************/
Status RTC_CounterReset(void)
{  
  volatile uint32_t cnt = 0;
    
  /* Reset the RTC counter */
  RTC->CNT_EN_REG.BF.CNT_RESET = 1;  
  
  /* Wating until the counter reset is done */
  while(cnt < 0x300000)
  {
    /* Read the counter reset status */
    if(RTC->CNT_EN_REG.BF.CNT_RST_DONE)
    {
      return SUCCESS;
    }
    
    cnt++;
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Initialize the RTC 
 *
 * @param[in]  rtcConfig:  Pointer to a RTC configuration structure
 *
 * @return none
 *
 * Initialize the RTC 
 *******************************************************************************/
void RTC_Init(RTC_Config_Type * rtcConfig)
{
  /* set counter value register update mode */
  RTC->CNT_CNTL_REG.BF.CNT_UPDT_MOD = rtcConfig->CntValUpdateMode;
  
  /* set clock divider */
  RTC->CLK_CNTL_REG.BF.CLK_DIV = rtcConfig->clockDivider;
  
  /* set counter overflow value */
  RTC->CNT_UPP_VAL_REG.BF.UPP_VAL = rtcConfig->uppVal;  
}

/****************************************************************************//**
 * @brief      Check status of RTC counter
 *
 * @param[in]  None
 *
 * @return 
 *              
 * Start the RTC counter
 *******************************************************************************/
FunctionalState RTC_GetCntStatus(void)
{
  return RTC->CNT_EN_REG.BF.CNT_RUN;
}

/****************************************************************************//**
 * @brief      Start the RTC counter
 *
 * @param[in]  None
 *
 * @return none
 *              
 * Start the RTC counter
 *******************************************************************************/
void RTC_Start(void)
{
  /* start RTC counter */
  RTC->CNT_EN_REG.BF.CNT_START = 1;
}

/****************************************************************************//**
 * @brief      Stop the RTC counter
 *
 * @param[in]  None
 *
 * @return none
 *
 * Stop the RTC counter
 *******************************************************************************/
void RTC_Stop(void)
{
  /* stop RTC counter */
  RTC->CNT_EN_REG.BF.CNT_STOP = 1;
}


/****************************************************************************//**
 * @brief      Set RTC counter overflow value 
 *
 * @param[in]  uppVal: Counter overflow value.The range is 0-0xffffffff.
 *             If it is greater than the maximum, the high bits will be neglected.
 *
 * @return none
 *
 *******************************************************************************/
void RTC_SetCounterUppVal(uint32_t uppVal)
{
  /* set counter overflow value */
  RTC->CNT_UPP_VAL_REG.BF.UPP_VAL = uppVal;
}

/****************************************************************************//**
 * @brief      Get RTC counter overflow value
 *
 * @param[in]  uppVal: Counter overflow value.The range is 0-0xffffffff.
 *
 * @return none
 *
 *******************************************************************************/
uint32_t RTC_GetCounterUppVal(void)
{
  return RTC->CNT_UPP_VAL_REG.BF.UPP_VAL;
}

/****************************************************************************//**
 * @brief      Set RTC alarm value 
 *
 *@param[in]   alarmVal: Counter Alarm Value.The range is 0-0xffffffff.
 *             If it is greater than the maximum, the high bits will be neglected.
 *
 * @return none
 *
 *******************************************************************************/
void RTC_SetCounterAlarmVal(uint32_t alarmVal)
{
  RTC->CNT_ALARM_VAL_REG.BF.ALARM_VAL = alarmVal;
}

/****************************************************************************//**
 * @brief      Get RTC counter value in any counter update mode
 *
 * @param[in]  None
 *
 * @return counter value
 *
 *******************************************************************************/
uint32_t RTC_GetCounterVal(void)
{
  uint32_t cntVal;

  /* read counter value register */
  cntVal = RTC->CNT_VAL_REG.BF.CNT_VAL;
  
  return cntVal;
}

/****************************************************************************//**
 * @brief     MASK / UNMASK  RTC interrupt 
 *
 * @param[in]  intMsk:   MASK / UNMASK control
 *
 * @return none
 *
 *******************************************************************************/
void RTC_IntMask(RTC_INT_Type intType, IntMask_Type intMsk)
{
  CHECK_PARAM(IS_INTMASK(intMsk));
  CHECK_PARAM(IS_RTC_INT_TYPE(intType));
  
  switch(intType)
  {
    case RTC_INT_CNT_UPP:
      RTC->INT_MSK_REG.BF.CNT_UPP_MSK = intMsk;
      break;
    case RTC_INT_CNT_ALARM:
      RTC->INT_MSK_REG.BF.CNT_ALARM_MSK = intMsk;
      break;
    case RTC_INT_ALL:
      if(intMsk == UNMASK)
      {
        RTC->INT_MSK_REG.WORDVAL = 0;
      }
      else
      {
        RTC->INT_MSK_REG.WORDVAL |= RTC_INT_ALL_MSK; 
      }
      break;      
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Clear RTC interrupt flag
 *
 * @param[in] none
 *
 * @return none
 *
 *******************************************************************************/
void RTC_IntClr(RTC_INT_Type intType)
{
  CHECK_PARAM(IS_RTC_INT_TYPE(intType));
  
  switch(intType)
  {
    case RTC_INT_CNT_UPP:
      RTC->INT_RAW_REG.BF.CNT_UPP_INT = 1;
      break;
    case RTC_INT_CNT_ALARM:
      RTC->INT_RAW_REG.BF.CNT_ALARM_INT = 1;
      break;
    case RTC_INT_ALL:      
      RTC->INT_RAW_REG.WORDVAL |= RTC_INT_ALL_MSK;       
      break;      
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Get RTC  status
 *
 * @param[in] none
 *
 * @return status
 *
 *******************************************************************************/
FlagStatus RTC_GetStatus(RTC_INT_Type intType)
{
  uint32_t retStatus = 0;
  
  CHECK_PARAM(IS_RTC_INT_TYPE(intType));
  
  switch(intType)
  {
  case RTC_INT_CNT_UPP:    
    retStatus = RTC->INT_RAW_REG.BF.CNT_UPP_INT;
    break;
  case RTC_INT_CNT_ALARM:    
    retStatus = RTC->INT_RAW_REG.BF.CNT_ALARM_INT;
    break;
  default:
    break;
  }
 
  if(retStatus)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/****************************************************************************//**
 * @brief  Timer interrupt handler 
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void RTC_IRQHandler(void)
{
  uint32_t intStatus = RTC->INT_REG.WORDVAL;
  
  /* clear the generated interrupts */
  RTC->INT_RAW_REG.WORDVAL = intStatus;
  
  /* counter upp interrupt */
  if((intStatus & (RTC_INT_CNT_ALARM_MSK))!= 0x00)
  {
    /* counter alarm interrupt */
    if(intCbfArra[INT_RTC][RTC_INT_CNT_ALARM] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_RTC][RTC_INT_CNT_ALARM]();
    }
    else
    {
      /* Disable the interrupt if callback function is not setup */
      RTC->INT_MSK_REG.BF.CNT_ALARM_MSK = 1;
    }
  }
  
  /* counter upp interrupt */
  if((intStatus & (RTC_INT_CNT_UPP_MSK))!= 0x00)
  {
    /* counter upp interrupt */
    if(intCbfArra[INT_RTC][RTC_INT_CNT_UPP] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_RTC][RTC_INT_CNT_UPP]();
    }
    else
    {
      /* Disable the interrupt if callback function is not setup */
      RTC->INT_MSK_REG.BF.CNT_UPP_MSK = 1;
    }
  }
}

/*@} end of group RTC_Public_Functions */

/*@} end of group RTC  */

/*@} end of group MW300_Periph_Driver */

