/****************************************************************************//**
 * @file     mc200_rtc.c
 * @brief    This file provides RTC functions.
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
#include "mc200_rtc.h"
#include "mc200_driver.h"

/** @addtogroup  MC200_Periph_Driver
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
  RTC->CNT_EN.BF.CNT_RESET = 1;  
  
  /* Wating until the counter reset is done */
  while(cnt < 0x300000)
  {
    /* Read the counter reset status */
    if(RTC->CNT_EN.BF.CNT_RST_DONE)
    {
      return DSUCCESS;
    }
    
    cnt++;
  }
  
  return DERROR;
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
  RTC->CNT_CNTL.BF.CNT_UPDT_MOD = rtcConfig->CntValUpdateMode;
  
  /* set clock divider */
  RTC->CLK_CNTL.BF.CLK_DIV = rtcConfig->clockDivider;
  
  /* set counter overflow value */
  RTC->CNT_UPP_VAL.BF.UPP_VAL = rtcConfig->uppVal;  
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
  return RTC->CNT_EN.BF.CNT_RUN;
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
  RTC->CNT_EN.BF.CNT_START = 1;
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
  RTC->CNT_EN.BF.CNT_STOP = 1;
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
  RTC->CNT_UPP_VAL.BF.UPP_VAL = uppVal;
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
  return RTC->CNT_UPP_VAL.BF.UPP_VAL;
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
  cntVal = RTC->CNT_VAL.BF.CNT_VAL;
  
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
void RTC_IntMask(IntMask_Type intMsk)
{
  CHECK_PARAM(IS_INTMASK(intMsk));

  RTC->INT_MSK.BF.CNT_UPP_MSK = intMsk;
}

/****************************************************************************//**
 * @brief      Clear RTC interrupt flag
 *
 * @param[in] none
 *
 * @return none
 *
 *******************************************************************************/
void RTC_IntClr(void)
{
  RTC->INT_RAW.WORDVAL = 0x010000;
}

/****************************************************************************//**
 * @brief      Get RTC  status
 *
 * @param[in] none
 *
 * @return status
 *
 *******************************************************************************/
FlagStatus RTC_GetStatus(void)
{
  uint32_t retStatus = 0;
  
  retStatus = RTC->INT_RAW.BF.CNT_UPP_INT;
 
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
void __attribute__ ((interrupt)) RTC_IRQHandler(void)
{
  /* clear  RTC interrupt flag */
  RTC->INT_RAW.WORDVAL = 0x010000;
  
  /* counter upp interrupt */
  if(intCbfArra[INT_RTC][RTC_INT_CNT_UPP] != NULL)
  {
    /* call the callback function */
    intCbfArra[INT_RTC][RTC_INT_CNT_UPP]();
  }
  else
  {
    /* Disable the interrupt if callback function is not setup */
    RTC->INT_MSK.BF.CNT_UPP_MSK = 1;
  }
  
}

/*@} end of group RTC_Public_Functions */

/*@} end of group RTC  */

/*@} end of group MC200_Periph_Driver */
