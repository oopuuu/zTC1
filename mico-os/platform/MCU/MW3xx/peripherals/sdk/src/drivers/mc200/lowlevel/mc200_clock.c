/****************************************************************************//**
 * @file     mc200_clock.c
 * @brief    This file provides CLOCK functions.
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

#include "mc200_clock.h"
#include "board.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @defgroup Clock
 *  @brief Clock driver modules
 *  @{
 */

/** @defgroup Clock_Private_Type
 *  @{
 */

/**  
 *  @brief Clock interrupt status type definition 
 */


/**
 * @brief Clock interrupt call-back function type definitions
 */



/*@} end of group Clock_Private_Type*/

/** @defgroup Clock_Private_Defines
 *  @{
 */


/* Clock interrupt status bit mask */


/*@} end of group Clock_Private_Defines */


/** @defgroup Clock_Private_Variables
 *  @{
 */



/*@} end of group Clock_Private_Variables */

/** @defgroup Clock_Global_Variables
 *  @{
 */


/*@} end of group Clock_Global_Variables */


/** @defgroup Clock_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group Clock_Private_FunctionDeclaration */

/** @defgroup Clock_Private_Functions
 *  @{
 */


/*@} end of group Clock_Private_Functions */


/** @defgroup Clock_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief     Select the clock source for system clock
 *
 * @param[in]  clockSource:  Clock source for system clock (RC32M/XT32M/SFLL)
 *
 * @return none
 *
 *******************************************************************************/
void CLK_SystemClkSrc(CLK_Src_Type clockSource)
{
  /* Check the parameters */
  CHECK_PARAM( IS_SYSTEM_CLOCK_SOURCE(clockSource) );
  
  switch(clockSource)
  {
    case CLK_SFLL:
      if((PMU->CLK_SRC.BF.SYS_CLK_SEL == 2)&&(PMU->CLK_RDY.BF.RC32M_RDY))
      {
        PMU->CLK_SRC.BF.SYS_CLK_SEL = 3;
      }
      if(PMU->CLK_SRC.BF.SYS_CLK_SEL == 3)
      {
        PMU->CLK_SRC.BF.SYS_CLK_SEL = 1;
      }
      PMU->CLK_SRC.BF.SYS_CLK_SEL = 0;
      break;   
      
    case CLK_RC32M:      
      if((PMU->CLK_SRC.BF.SYS_CLK_SEL == 0)||(PMU->CLK_SRC.BF.SYS_CLK_SEL == 2))
      {
        PMU->CLK_SRC.BF.SYS_CLK_SEL = (PMU->CLK_SRC.BF.SYS_CLK_SEL) + 1 ;
      }
      break;
      
    case CLK_MAINXTAL:
      if((PMU->CLK_SRC.BF.SYS_CLK_SEL == 0)&&(PMU->CLK_RDY.BF.RC32M_RDY))
      {
        PMU->CLK_SRC.BF.SYS_CLK_SEL = 1;
      }
      if(PMU->CLK_SRC.BF.SYS_CLK_SEL == 1)
      {
        PMU->CLK_SRC.BF.SYS_CLK_SEL = 3;
      }
      PMU->CLK_SRC.BF.SYS_CLK_SEL = 2;
      break;

    default:
      /* Default source RC32M */
      PMU->CLK_SRC.BF.SYS_CLK_SEL = 1;
      break;
  }  
}

/****************************************************************************//**
 * @brief     Select the clock source for cau
 *
 * @param[in]  clockSource:  Clock source for cau (RC32M/XT32M/AUPLL)
 *
 * @return none
 *
 *******************************************************************************/
void CLK_CAUClkSrc(CLK_Src_Type clockSource)
{
  /* Check the parameters */
  CHECK_PARAM( IS_CAU_Clock_Source(clockSource) );

  switch(clockSource)
  {
    case CLK_AUPLL:
      PMU->CAU_CLK_SEL.BF.CAU_CLK_SEL = 2;    
      break;   
      
    case CLK_RC32M:
      PMU->CAU_CLK_SEL.BF.CAU_CLK_SEL = 0;
      break;
      
    case CLK_MAINXTAL:
      PMU->CAU_CLK_SEL.BF.CAU_CLK_SEL = 1;
      break;

    default:
      break;
  }  
}

/****************************************************************************//**
 * @brief     Select the clock source for RTC
 *
 * @param[in]  clockSource:  Clock source for RTC (RC32K/XT32K)
 *
 * @return none
 *
 *******************************************************************************/
void CLK_RTCClkSrc(CLK_Src_Type clockSource)
{
  /* Check the parameters */
  CHECK_PARAM( IS_RTC_Clock_Source(clockSource) );

  switch(clockSource)
  {
    case CLK_RC32K:
      PMU->PERI_CLK_SRC.BF.RTC_INT_SEL = 0;
      break;
      
    case CLK_XTAL32K:
      PMU->PERI_CLK_SRC.BF.RTC_INT_SEL = 1;
      break;

    default:
      break;
  }  
}

/****************************************************************************//**
 * @brief     Set the UART fast/slow clock source
 *
 * @param[in]  uartClockSource:  UART clock source type (CLK_UART_FAST/CLK_UART_SLOW)
 * @param[in]  uartClkFraction:  dividend and divisor for UART clock
 *
 * @return none
 *
 * @note      CLK_UART_FAST/CLK_UART_SLOW = CLK_SYSTEM * clkDividend / clkDivisor
 *            To generate accurate baud rate, the recommended value of uartClkFraction as:
 *
 *            CLK_SYSTEM   Baud rate  clkDivisor  clkDividend 
 *                  200M     1500000       0x3E8         0xF2
 *                  200M     1000000       0x3E8         0xF2
 *                  200M      500000       0x3E8         0x7A
 *                  200M      256000       0x3E8         0xE2
 *                  200M      115200       0x3E8         0xE2
 *                  200M        9600       0x3E8         0xE2
 *                   32M      256000       0x3E8        0x200
 *                   32M      128000       0x3E8        0x200
 *                   32M      115200       0x3E8        0x1CD
 *                   32M       56000       0x3E8        0x1CD
 *                   32M       14400       0x3E8        0x1CD
 *                   32M        9600       0x3E8        0x1F4
 *******************************************************************************/
void CLK_UARTDividerSet(CLK_UartSrc_Type uartClockSource, CLK_Fraction_Type uartClkFraction)
{
  /* Check the parameters */
  CHECK_PARAM( IS_UART_Clock_Source(uartClockSource) );
  CHECK_PARAM( IS_UART_Dividend(uartClkFraction.clkDividend) );
  CHECK_PARAM( IS_UART_Divisor(uartClkFraction.clkDivisor) );
  
  switch(uartClockSource)
  {
    case CLK_UART_FAST:
      PMU->UART_FAST_CLK_DIV.BF.DENOMINATOR = uartClkFraction.clkDividend;
      PMU->UART_FAST_CLK_DIV.BF.NOMINATOR = uartClkFraction.clkDivisor;
      break;
      
    case CLK_UART_SLOW:
      PMU->UART_SLOW_CLK_DIV.BF.DENOMINATOR = uartClkFraction.clkDividend;
      PMU->UART_SLOW_CLK_DIV.BF.NOMINATOR = uartClkFraction.clkDivisor;
      break;

    default:
      break;
  }    
}

/****************************************************************************//**
 * @brief     Set the UART fast/slow clock source
 *
 * @param[in]  uartClockSource:  UART clock source type (CLK_UART_FAST/CLK_UART_SLOW)
 *
 * @return    uartClkFraction:  dividend and divisor for UART clock
 *
 * @note      CLK_UART_FAST/CLK_UART_SLOW = CLK_SYSTEM * clkDividend / clkDivisor
 *******************************************************************************/
void CLK_GetUARTDivider(CLK_UartSrc_Type uartClockSource, CLK_Fraction_Type * uartClkFraction)
{  
  /* Check the parameters */
  CHECK_PARAM( IS_UART_Clock_Source(uartClockSource) );
  
  switch(uartClockSource)
  {
    case CLK_UART_FAST:
      uartClkFraction->clkDividend = PMU->UART_FAST_CLK_DIV.BF.DENOMINATOR;
      uartClkFraction->clkDivisor = PMU->UART_FAST_CLK_DIV.BF.NOMINATOR;
      break;
      
    case CLK_UART_SLOW:
      uartClkFraction->clkDividend = PMU->UART_SLOW_CLK_DIV.BF.DENOMINATOR;
      uartClkFraction->clkDivisor = PMU->UART_SLOW_CLK_DIV.BF.NOMINATOR;
      break;

    default:
      break;
  }    

}


/****************************************************************************//**
 * @brief     Get the system clock frequency
 *
 * @param[in] none
 *
 * @return    System clock frequency
 *
 *******************************************************************************/
uint32_t CLK_GetSystemClk(void)
{
  uint32_t sysClkSource;
  uint32_t returnval = 0;
  uint64_t tempClkFreq;
  
  uint32_t clkSourceFreq = board_main_xtal();
  
  sysClkSource = (PMU->CLK_SRC.WORDVAL & 0x03);
  
  switch(sysClkSource)
  {
  case 0:
    /* System clock comes from CLK_SFLL */
    tempClkFreq = (uint64_t) clkSourceFreq * (PMU->SFLL_CTRL1.BF.SFLL_FBDIV);
    
    switch(PMU->SFLL_CTRL0.BF.SFLL_DIV_SEL)
    {
    case 0:
      /* SFLL post-divider is /1 */
      break;
      
    case 1:
      /* SFLL post-divider is /2 */
      tempClkFreq >>= 1;
      break;   
      
    case 2:
      /* SFLL post-divider is /4 */
      tempClkFreq >>= 2;
      break;  
      
    case 3:
      /* SFLL post-divider is /8 */
      tempClkFreq >>= 3;
      break;   
      
    default:
      break;
    }  
    
    tempClkFreq /= (PMU->SFLL_CTRL1.BF.SFLL_REFDIV);
    returnval =  (uint32_t) tempClkFreq & 0xFFFFFFFF;
    break; /* End switch(PMU->SFLL_CTRL0.BF.SFLL_DIV_SEL) */
    
  case 1:
  case 3:
    /* System clock comes from CLK_RC32M */
    /* Assume it's calibrated */
    returnval = 32000000;
    break;
    
  case 2:
    /* System clock comes from CLK_MAINXTAL */
    returnval = clkSourceFreq;
    break;
    
  default:
    break;
  } /* End switch(sysClkSource) */
  
  return returnval;
}

/****************************************************************************//**
 * @brief     Select the clock source for UART
 *
 * @param[in]  uartIndex:  Index to UART (UART0/1/2/3)
 * @param[in]  clockSource:  Clock source for UART (CLK_UART_FAST/CLK_UART_SLOW)
 *
 * @return none
 *
 *******************************************************************************/
void CLK_SetUARTClkSrc(CLK_UartID_Type uartIndex, CLK_UartSrc_Type uartClockSource)
{
  /* Check the parameters */
  CHECK_PARAM( IS_UART_Clock_Source(uartClockSource) );

  if(uartIndex == CLK_UART_ID_0)
  {
    switch(uartClockSource)
    {
    case CLK_UART_FAST:
      PMU->UART_CLK_SEL.BF.UART0_CLK_SEL = 1;
      break;
      
    case CLK_UART_SLOW:
      PMU->UART_CLK_SEL.BF.UART0_CLK_SEL = 0;
      break;

    default:
      break;
    }  
  }
  else if(uartIndex == CLK_UART_ID_1)
  {
    switch(uartClockSource)
    {
    case CLK_UART_FAST:
      PMU->UART_CLK_SEL.BF.UART1_CLK_SEL = 1;
      break;
      
    case CLK_UART_SLOW:
      PMU->UART_CLK_SEL.BF.UART1_CLK_SEL = 0;
      break;

    default:
      break;
    }  
  }
  else if(uartIndex == CLK_UART_ID_2)
  {
    switch(uartClockSource)
    {
    case CLK_UART_FAST:
      PMU->UART_CLK_SEL.BF.UART2_CLK_SEL = 1;
      break;
      
    case CLK_UART_SLOW:
      PMU->UART_CLK_SEL.BF.UART2_CLK_SEL = 0;
      break;

    default:
      break;
    }  
  }
  else if(uartIndex == CLK_UART_ID_3)
  {
    switch(uartClockSource)
    {
    case CLK_UART_FAST:
      PMU->UART_CLK_SEL.BF.UART3_CLK_SEL = 1;
      break;
      
    case CLK_UART_SLOW:
      PMU->UART_CLK_SEL.BF.UART3_CLK_SEL = 0;
      break;

    default:
      break;
    }  
  }
}

/****************************************************************************//**
 * @brief     Get the clock source for UART
 *
 * @param[in]  uartIndex:  Index to UART (UART0/1/2/3)
 *
 * @return   Clock source for UART (CLK_UART_FAST/CLK_UART_SLOW)
 *
 *******************************************************************************/
CLK_UartSrc_Type CLK_GetUARTClkSrc(CLK_UartID_Type uartIndex)
{
  CLK_UartSrc_Type uartClockSourceType;
  
  uartClockSourceType = CLK_UART_SLOW;
  
  if(uartIndex == CLK_UART_ID_0)
  {
    if(PMU->UART_CLK_SEL.BF.UART0_CLK_SEL == 1)
    {
      uartClockSourceType = CLK_UART_FAST;
    }
  }
  else if(uartIndex == CLK_UART_ID_1)
  {
    if(PMU->UART_CLK_SEL.BF.UART1_CLK_SEL == 1)
    {
      uartClockSourceType = CLK_UART_FAST;
    }
  }
  else if(uartIndex == CLK_UART_ID_2)
  {
    if(PMU->UART_CLK_SEL.BF.UART2_CLK_SEL == 1)
    {
      uartClockSourceType = CLK_UART_FAST;
    }
  }
  else if(uartIndex == CLK_UART_ID_3)
  {
    if(PMU->UART_CLK_SEL.BF.UART3_CLK_SEL == 1)
    {
      uartClockSourceType = CLK_UART_FAST;
    }
  }
  
  return uartClockSourceType;
}

/****************************************************************************//**
 * @brief     Select the clock source for GPT intrnal clock
 *
 * @param[in]  gptIndex:  Index to GPT (GPT0/1/2/3)
 * @param[in]  clockSource:  Clock source for gpt (RC32M/XT32M/RC32K/XTAL32K/SYSCLK)
 *
 * @return none
 *
 *******************************************************************************/
void CLK_GPTInternalClkSrc(CLK_GptID_Type gptIndex, CLK_Src_Type clockSource)
{
  /* Check the parameters */
  CHECK_PARAM( IS_GPT_Clock_Source(clockSource) );
  
  if(gptIndex == CLK_GPT_ID_0)
  {
    switch(clockSource)
    {
    case CLK_SYSTEM:
      PMU->GPT0_CTRL.BF.GPT0_CLK_SEL0 = 0; 
      PMU->GPT0_CTRL.BF.GPT0_CLK_SEL1 = 0; 
      break;   
      
    case CLK_RC32K:
      PMU->GPT0_CTRL.BF.GPT0_CLK_SEL1 = 2; 
      break;

    case CLK_XTAL32K:
      PMU->GPT0_CTRL.BF.GPT0_CLK_SEL1 = 3;     
      break;   
      
    case CLK_RC32M:
      PMU->GPT0_CTRL.BF.GPT0_CLK_SEL0 = 2; 
      PMU->GPT0_CTRL.BF.GPT0_CLK_SEL1 = 0; 
      break;
      
    case CLK_MAINXTAL:
      PMU->GPT0_CTRL.BF.GPT0_CLK_SEL0 = 3; 
      PMU->GPT0_CTRL.BF.GPT0_CLK_SEL1 = 0; 
      break;

    default:
      break;
    }      
  }
  else if(gptIndex == CLK_GPT_ID_1)
  {
    switch(clockSource)
    {
    case CLK_SYSTEM:
      PMU->GPT1_CTRL.BF.GPT1_CLK_SEL0 = 0; 
      PMU->GPT1_CTRL.BF.GPT1_CLK_SEL1 = 0; 
      break;   
      
    case CLK_RC32K:
      PMU->GPT1_CTRL.BF.GPT1_CLK_SEL1 = 2; 
      break;

    case CLK_XTAL32K:
      PMU->GPT1_CTRL.BF.GPT1_CLK_SEL1 = 3;     
      break;   
      
    case CLK_RC32M:
      PMU->GPT1_CTRL.BF.GPT1_CLK_SEL0 = 2; 
      PMU->GPT1_CTRL.BF.GPT1_CLK_SEL1 = 0; 
      break;
      
    case CLK_MAINXTAL:
      PMU->GPT1_CTRL.BF.GPT1_CLK_SEL0 = 3; 
      PMU->GPT1_CTRL.BF.GPT1_CLK_SEL1 = 0; 
      break;

    default:
      break;
    }      
  }
  else if(gptIndex == CLK_GPT_ID_2)
  {
    switch(clockSource)
    {
    case CLK_SYSTEM:
      PMU->GPT2_CTRL.BF.GPT2_CLK_SEL0 = 0; 
      PMU->GPT2_CTRL.BF.GPT2_CLK_SEL1 = 0; 
      break;   
      
    case CLK_RC32K:
      PMU->GPT2_CTRL.BF.GPT2_CLK_SEL1 = 2; 
      break;

    case CLK_XTAL32K:
      PMU->GPT2_CTRL.BF.GPT2_CLK_SEL1 = 3;     
      break;   
      
    case CLK_RC32M:
      PMU->GPT2_CTRL.BF.GPT2_CLK_SEL0 = 2; 
      PMU->GPT2_CTRL.BF.GPT2_CLK_SEL1 = 0; 
      break;
      
    case CLK_MAINXTAL:
      PMU->GPT2_CTRL.BF.GPT2_CLK_SEL0 = 3; 
      PMU->GPT2_CTRL.BF.GPT2_CLK_SEL1 = 0; 
      break;

    default:
      break;
    }      
  }
  else if(gptIndex == CLK_GPT_ID_3)
  {
    switch(clockSource)
    {
    case CLK_SYSTEM:
      PMU->GPT3_CTRL.BF.GPT3_CLK_SEL0 = 0; 
      PMU->GPT3_CTRL.BF.GPT3_CLK_SEL1 = 0; 
      break;   
      
    case CLK_RC32K:
      PMU->GPT3_CTRL.BF.GPT3_CLK_SEL1 = 2; 
      break;

    case CLK_XTAL32K:
      PMU->GPT3_CTRL.BF.GPT3_CLK_SEL1 = 3;     
      break;   
      
    case CLK_RC32M:
      PMU->GPT3_CTRL.BF.GPT3_CLK_SEL0 = 2; 
      PMU->GPT3_CTRL.BF.GPT3_CLK_SEL1 = 0; 
      break;
      
    case CLK_MAINXTAL:
      PMU->GPT3_CTRL.BF.GPT3_CLK_SEL0 = 3; 
      PMU->GPT3_CTRL.BF.GPT3_CLK_SEL1 = 0; 
      break;

    default:
      break;
    }      
  }
}

/****************************************************************************//**
 * @brief     Select the clock source for ssp
 *
 * @param[in]  sspIndex:  Index to SSP (SSP0/1/2)
 * @param[in]  clockSource:  Clock source for ssp (AUFLL/SYSCLK)
 *
 * @return none
 *
 *******************************************************************************/
void CLK_SSPClkSrc(CLK_SspID_Type sspIndex, CLK_Src_Type clockSource)
{
  /* Check the parameters */
  CHECK_PARAM( IS_SSP_Clock_Source(clockSource) );
  
  if(sspIndex == CLK_SSP_ID_0)
  {
    switch(clockSource)
    {
    case CLK_AUPLL:
      PMU->PERI_CLK_SRC.BF.SSP0_AUDIO_SEL = 1; 
      break;   
      
    case CLK_SYSTEM:
      PMU->PERI_CLK_SRC.BF.SSP0_AUDIO_SEL = 0; 
      break;

    default:
      break;
    }      
  }
  else if(sspIndex == CLK_SSP_ID_1)
  {
    switch(clockSource)
    {
    case CLK_AUPLL:
      PMU->PERI_CLK_SRC.BF.SSP1_AUDIO_SEL = 1; 
      break;   
      
    case CLK_SYSTEM:
      PMU->PERI_CLK_SRC.BF.SSP1_AUDIO_SEL = 0; 
      break;

    default:
      break;
    }      
  }
  else if(sspIndex == CLK_SSP_ID_2)
  {
    switch(clockSource)
    {
    case CLK_AUPLL:
      PMU->PERI_CLK_SRC.BF.SSP2_AUDIO_SEL = 1; 
      break;   
      
    case CLK_SYSTEM:
      PMU->PERI_CLK_SRC.BF.SSP2_AUDIO_SEL = 0; 
      break;

    default:
      break;
    }      
  }
}

/****************************************************************************//**
 * @brief     Configure and power up SFLL
 *
 * @param[in]  sfllConfigSet:  Configure set for SFLL
 *
 * @return none
 *
 *******************************************************************************/
void CLK_SfllEnable(CLK_SfllConfig_Type * sfllConfigSet)
{
  uint32_t fbDivBound;
  
  /* check the parameters */
  /* Accepted sfll reference clock sources are RC32M / XT32M */
  CHECK_PARAM( IS_SFLL_REF_SOURCE(sfllConfigSet->refClockSrc) );
  
  /* Accepted sfll reference divider is 2~255 */
  CHECK_PARAM( IS_SFLL_REF_DIV(sfllConfigSet->refDiv) );  
  
  /* Accepted sfll feedback divider is 1~2047 */
  CHECK_PARAM( IS_SFLL_FB_DIV(sfllConfigSet->fbDiv) );  
  
  /* Accepted sfll kvco is 0~7 */
  CHECK_PARAM( IS_SFLL_KVCO(sfllConfigSet->kvco) );  
  
  /* Accepted sfll postdiv is 0~3 */
  CHECK_PARAM( IS_SFLL_POSTDIV(sfllConfigSet->postDiv) );  
  
  switch(sfllConfigSet->refClockSrc)
  {
    case CLK_RC32M:
      PMU->SFLL_CTRL0.BF.SFLL_REFCLK_SEL = 0;
      break;
      
    case CLK_MAINXTAL:
      PMU->SFLL_CTRL0.BF.SFLL_REFCLK_SEL = 1;
      break;    
      
    default:
      PMU->SFLL_CTRL0.BF.SFLL_REFCLK_SEL = 0;
      break;    
  }
  
  PMU->SFLL_CTRL1.BF.SFLL_REFDIV = sfllConfigSet->refDiv;
  PMU->SFLL_CTRL1.BF.SFLL_FBDIV = sfllConfigSet->fbDiv;
  PMU->SFLL_CTRL0.BF.SFLL_KVCO = sfllConfigSet->kvco;
  PMU->SFLL_CTRL0.BF.SFLL_DIV_SEL = sfllConfigSet->postDiv;
  
  /* fbDivBound = 0.02 * fbDiv ~= (1/1024 + 1/256 + 1/64) * fbDiv */
  fbDivBound = sfllConfigSet->fbDiv >> 10;
  fbDivBound += (sfllConfigSet->fbDiv >> 8);
  fbDivBound += (sfllConfigSet->fbDiv >> 6);
  
  /* SFLL_READY_DET_LOW = 0.98 * fbDiv */
  PMU->ANA_GRP_CTRL0.BF.SFLL_READY_DET_LOW = (sfllConfigSet->fbDiv) - fbDivBound;
  /* SFLL_READY_DET_HIGH = 1.02 * fbDiv */
  PMU->ANA_GRP_CTRL0.BF.SFLL_READY_DET_HIGH = (sfllConfigSet->fbDiv) + fbDivBound;
  
  /* Power up */
  PMU->SFLL_CTRL0.BF.SFLL_PU = 1;  
}

/****************************************************************************//**
 * @brief     Power off SFLL
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CLK_SfllDisable(void)
{
  /* Power down */
  PMU->SFLL_CTRL0.BF.SFLL_PU = 0;  
}

/****************************************************************************//**
 * @brief     Configure and power up AUPLL
 *
 * @param[in]  aupllConfigSet:  Configure set for AUPLL
 *
 * @return none
 *
 *******************************************************************************/
void CLK_AupllEnable(CLK_AupllConfig_Type * aupllConfigSet)
{
  /* check the parameters */
  /* Accepted aupll reference divider is 1~31 */
  CHECK_PARAM( IS_AUPLL_REF_DIV(aupllConfigSet->refDiv) );  
  
  /* Accepted aupll feedback divider is 16~511 */
  CHECK_PARAM( IS_AUPLL_FB_DIV(aupllConfigSet->fbDiv) );  
  
  /* Accepted aupll icp is 3~6 */
//  CHECK_PARAM( IS_AUPLL_ICP(aupllConfigSet->icp) );  

  /* Accepted aupll update_rate is 0~1 */
  CHECK_PARAM( IS_AUPLL_UPDATE_RATE(aupllConfigSet->updateRate) );  
  
  /* Accepted aupll postdiv_usb is 0~1 */
  CHECK_PARAM( IS_AUPLL_POSTDIV_USB(aupllConfigSet->usbPostDiv) );  
  
  PMU->AUPLL_CTRL0.BF.REFDIV = aupllConfigSet->refDiv;
  PMU->AUPLL_CTRL0.BF.FBDIV = aupllConfigSet->fbDiv;
  PMU->AUPLL_CTRL1.BF.UPDATE_SEL = aupllConfigSet->updateRate;
  PMU->AUPLL_CTRL2.BF.POSTDIV_USB = aupllConfigSet->usbPostDiv;
   
  /* Power up */
  PMU->AUPLL_CTRL0.BF.PU = 1;  
}

/****************************************************************************//**
 * @brief     Power off AUPLL
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CLK_AupllDisable(void)
{
  /* Power down */
  PMU->AUPLL_CTRL0.BF.PU = 0;    
}

/****************************************************************************//**
 * @brief      AUPLL clock out enable
 *
 * @param[in]  aupllOutClk: Select the AUPLL output clock 
 *
 * @return none
 *
 *******************************************************************************/
void CLK_AupllClkOutEnable(CLK_AupllClkOut_Type aupllOutClk)
{
  /* check the parameters */
  CHECK_PARAM( IS_AUPLL_OUT_CLK(aupllOutClk) ); 
  
  switch(aupllOutClk)
  {
    case CLK_AUPLL_USB:
      /* Enable AUPLL_USB clock out */
      PMU->AUPLL_CTRL2.BF.POSTDIV_USB_EN = 1;
      break;
      
    case CLK_AUPLL_CAU:
      /* Enable AUPLL_CAU clock out */
      PMU->AUPLL_CTRL2.BF.CLKOUT_30M_EN = 1;
      break;
      
    case CLK_AUPLL_AUDIO:
      /* Enable AUPLL_AUDIO clock out */
      PMU->AUPLL_CTRL2.BF.POSTDIV_AUDIO_EN = 1;
      break;
      
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      AUPLL clock out disable
 *
 * @param[in]  aupllOutClk: Select the AUPLL output clock 
 *
 * @return none
 *
 *******************************************************************************/
void CLK_AupllClkOutDisable(CLK_AupllClkOut_Type aupllOutClk)
{
  /* check the parameters */
  CHECK_PARAM( IS_AUPLL_OUT_CLK(aupllOutClk) ); 
  
  switch(aupllOutClk)
  {
    case CLK_AUPLL_USB:
      /* Disable AUPLL_USB clock out */
      PMU->AUPLL_CTRL2.BF.POSTDIV_USB_EN = 0;
      break;
      
    case CLK_AUPLL_CAU:
      /* Disable AUPLL_CAU clock out */
      PMU->AUPLL_CTRL2.BF.CLKOUT_30M_EN = 0;
      break;
      
    case CLK_AUPLL_AUDIO:
      /* Disable AUPLL_AUDIO clock out */
      PMU->AUPLL_CTRL2.BF.POSTDIV_AUDIO_EN = 0;
      break;
      
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Set AUPLL audio clock out frequency
 *
 * @param[in]  freqOffset: frequency offset in PPM. 
 *               freqOffset[15] is sign bit:
 *                 = 0, frequency down
 *                 = 1, frequency up
 *               freqOffset[14:0] is magnitude, unit ppm.
 *               e.g.
 *               freqOffset = 0xFFFF, means up 6.67%
 *               freqOffset = 0x00FF, means down 0.0487%
 * @param[in]  audioPostDiv: Select the AUPLL audio clock post divider 
 *
 * @return none
 *
 *******************************************************************************/
void CLK_SetAupllAudioFreq(int16_t freqOffset, uint8_t audioPostDiv)
{
  volatile uint32_t delayCycle;
  
  /* check the parameters */
  /* efffective audioPostDiv: even number 2, 4, 6, 8, ... , 126, plus odd number 3 */
  CHECK_PARAM( IS_AUPLL_AUDIO_POSTDIV(audioPostDiv) ); 
  
  /* Disable interpolation block */
  PMU->AUPLL_CTRL1.BF.PI_EN = 0;
  
  /* Invalidate AUPLL audio frequency offset */
  PMU->AUPLL_CTRL2.BF.FREQ_OFFSET_VALID = 0;
  
  /* Reset AUPLL offset logic then release */
  PMU->AUPLL_CTRL2.BF.RESET_OFFSET_EXT = 1;
  PMU->AUPLL_CTRL2.BF.RESET_OFFSET_EXT = 0;
  
  /* Reset AUPLL interpolation logic then release  */
  PMU->AUPLL_CTRL1.BF.RESET_INTP_EXT = 1;
  PMU->AUPLL_CTRL1.BF.RESET_INTP_EXT = 0;
  
  /* Enable interpolation block */
  PMU->AUPLL_CTRL1.BF.PI_EN = 1;
  
  /* Set AUPLL audio frequency offset */
  /* Reserved bit FREQ_OFFSET[16] is extended by the sign bit freqOffset[15] */
  PMU->AUPLL_CTRL1.BF.FREQ_OFFSET = freqOffset;
  
  /* Wait */
  delayCycle = 20000;
  while(delayCycle--);
  
  /* Validate AUPLL audio frequency offset */
  PMU->AUPLL_CTRL2.BF.FREQ_OFFSET_VALID = 1;
  
  /* Wait */
  delayCycle = 20000;
  while(delayCycle--);
  
  /* Invalidate AUPLL audio frequency offset */
  PMU->AUPLL_CTRL2.BF.FREQ_OFFSET_VALID = 0;
  
  PMU->AUPLL_CTRL2.BF.POSTDIV_AUDIO = audioPostDiv;
  delayCycle = 20000;
  while(delayCycle--);
}

/****************************************************************************//**
 * @brief      Enable main crystal oscillator
 *
 * @param[in]  oscMode: Select the oscillator 
 *
 * @return none
 *
 *******************************************************************************/
void CLK_MainXtalEnable(CLK_CrystalMode_Type oscMode)
{
  /* check the parameters */
  CHECK_PARAM( IS_OSC_MODE(oscMode) ); 
  
  switch(oscMode)
  {
    case CLK_OSC_INTERN:
      /* Internal oscillator mode */
      PMU->ANA_GRP_CTRL1.BF.BYPASS = 0;
      break;
      
    case CLK_OSC_EXTERN:
      /* External oscillator mode */
      PMU->ANA_GRP_CTRL1.BF.BYPASS = 1;
      break;
      
    default:
      break;
  }  
  
  /* Power up */
  PMU->ANA_GRP_CTRL0.BF.PU_XTAL = 1;
  PMU->ANA_GRP_CTRL0.BF.PU = 1;
}

/****************************************************************************//**
 * @brief      Disable main crystal oscillator
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CLK_MainXtalDisable(void)
{
  /* Power down */
  PMU->ANA_GRP_CTRL0.BF.PU = 0;
  PMU->ANA_GRP_CTRL0.BF.PU_XTAL = 0;
}

/****************************************************************************//**
 * @brief      Enable 32khz crystal oscillator
 *
 * @param[in]  oscMode: Select the oscillator 
 *
 * @return none
 *
 *******************************************************************************/
void CLK_Xtal32KEnable(CLK_CrystalMode_Type oscMode)
{
  /* check the parameters */
  CHECK_PARAM( IS_OSC_MODE(oscMode) ); 
  
  switch(oscMode)
  {
    case CLK_OSC_INTERN:
      /* Internal oscillator mode */
      PMU->XTAL32K_CTRL.BF.X32K_EXT_OSC_EN = 0;
      break;
      
    case CLK_OSC_EXTERN:
      /* External oscillator mode */
      PMU->XTAL32K_CTRL.BF.X32K_EXT_OSC_EN = 1;
      break;
      
    default:
      break;
  }  
  
  /* Power up */
  PMU->XTAL32K_CTRL.BF.X32K_EN = 1;
}

/****************************************************************************//**
 * @brief      Disable 32khz crystal oscillator
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CLK_Xtal32KDisable(void)
{
  /* Power down */
  PMU->XTAL32K_CTRL.BF.X32K_EN = 0;
}

/****************************************************************************//**
 * @brief      Enable RC32M oscillator
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CLK_RC32MEnable(void)
{
  /* Power up */
  RC32M->CTRL.BF.PD = 0;
  RC32M->CTRL.BF.EN = 1;
}

/****************************************************************************//**
 * @brief      Disable RC32M oscillator
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CLK_RC32MDisable(void)
{
  /* Power down */
  RC32M->CTRL.BF.EN = 0;
  RC32M->CTRL.BF.PD = 1;
}

/****************************************************************************//**
 * @brief      Enable RC32K oscillator
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CLK_RC32KEnable(void)
{
  /* Power up */
  PMU->RC32K_CTRL.BF.RC32K_PD = 0;
}

/****************************************************************************//**
 * @brief      Disable RC32K oscillator
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CLK_RC32KDisable(void)
{
  /* Power down */
  PMU->RC32K_CTRL.BF.RC32K_PD = 1;
}

/****************************************************************************//**
 * @brief      Module clock enable
 *
 * @param[in]  clockModule: Select the clock module 
 *
 * @return none
 *
 *******************************************************************************/
void CLK_ModuleClkEnable(CLK_Module_Type clockModule)
{
  /* check the parameters */
  CHECK_PARAM( IS_GATED_CLK_MODULE(clockModule) ); 
  
  switch(clockModule)
  {
    case CLK_QSPI0:
      PMU->PERI_CLK_EN.BF.QSPI0_CLK_EN = 0;
      break;
 
    case CLK_RTC:
      PMU->PERI_CLK_EN.BF.RTC_CLK_EN = 0;
      break;

    case CLK_GPIO:
      PMU->PERI_CLK_EN.BF.GPIO_CLK_EN = 0;
      break;
 
    case CLK_UART0:
      PMU->PERI_CLK_EN.BF.UART0_CLK_EN = 0;
      break;
 
    case CLK_UART1:
      PMU->PERI_CLK_EN.BF.UART1_CLK_EN = 0;
      break;
      
    case CLK_I2C0:
      PMU->PERI_CLK_EN.BF.I2C0_CLK_EN = 0;
      break;
      
    case CLK_SSP0:
      PMU->PERI_CLK_EN.BF.SSP0_CLK_EN = 0;
      break;    
      
    case CLK_SSP1:
      PMU->PERI_CLK_EN.BF.SSP1_CLK_EN = 0;
      break;             
      
    case CLK_GPT0:
      PMU->PERI_CLK_EN.BF.GPT0_CLK_EN = 0;
      break;         
      
    case CLK_GPT1:
      PMU->PERI_CLK_EN.BF.GPT1_CLK_EN = 0;
      break;   
      
    case CLK_UART2:
      PMU->PERI_CLK_EN.BF.UART2_CLK_EN = 0;
      break;
 
    case CLK_UART3:
      PMU->PERI_CLK_EN.BF.UART3_CLK_EN = 0;
      break;      
      
    case CLK_SSP2:
      PMU->PERI_CLK_EN.BF.SSP2_CLK_EN = 0;
      break;   

    case CLK_I2C1:
      PMU->PERI_CLK_EN.BF.I2C1_CLK_EN = 0;
      break;

    case CLK_I2C2:
      PMU->PERI_CLK_EN.BF.I2C2_CLK_EN = 0;
      break; 
      
    case CLK_GPT2:
      PMU->PERI_CLK_EN.BF.GPT2_CLK_EN = 0;
      break;  
      
    case CLK_GPT3:
      PMU->PERI_CLK_EN.BF.GPT3_CLK_EN = 0;
      break;   

    case CLK_WDT:
      PMU->PERI_CLK_EN.BF.WDT_CLK_EN = 0;
      break;       
 
    case CLK_QSPI1:
      PMU->PERI_CLK_EN.BF.QSPI1_CLK_EN = 0;
      break;

    case CLK_SDIO:
      PMU->PERI_CLK_EN.BF.SDIO_CLK_EN = 0;
      break;

    case CLK_USBC:
      PMU->PERI_CLK_EN.BF.USBC_CLK_EN = 0;
      break;

    case CLK_FLASHC:
      PMU->PERI_CLK_EN.BF.FLASH_QSPI_CLK_EN = 0;
      break;

    case CLK_ACOMP:
      PMU->CAU_CTRL.BF.CAU_ACOMP_MCLK_EN = 1;
      break;  
      
    case CLK_DAC:
      PMU->CAU_CTRL.BF.CAU_GPDAC_MCLK_EN = 1;
      break;     

    case CLK_ADC1:
      PMU->CAU_CTRL.BF.CAU_GPADC1_MCLK_EN = 1;
      break;     

    case CLK_ADC0:
      PMU->CAU_CTRL.BF.CAU_GPADC0_MCLK_EN = 1;
      break;     
      
    default:
      break;
  }  
}

/****************************************************************************//**
 * @brief      Module clock disable 
 *
 * @param[in]  clockModule: Select the clock module 
 *
 * @return none
 *
 *******************************************************************************/
void CLK_ModuleClkDisable(CLK_Module_Type clockModule)
{
  /* check the parameters */
  CHECK_PARAM( IS_GATED_CLK_MODULE(clockModule) ); 
  
  switch(clockModule)
  {
    case CLK_QSPI0:
      PMU->PERI_CLK_EN.BF.QSPI0_CLK_EN = 1;
      break;
 
    case CLK_RTC:
      PMU->PERI_CLK_EN.BF.RTC_CLK_EN = 1;
      break;

    case CLK_GPIO:
      PMU->PERI_CLK_EN.BF.GPIO_CLK_EN = 1;
      break;
 
    case CLK_UART0:
      PMU->PERI_CLK_EN.BF.UART0_CLK_EN = 1;
      break;
 
    case CLK_UART1:
      PMU->PERI_CLK_EN.BF.UART1_CLK_EN = 1;
      break;
      
    case CLK_I2C0:
      PMU->PERI_CLK_EN.BF.I2C0_CLK_EN = 1;
      break;
      
    case CLK_SSP0:
      PMU->PERI_CLK_EN.BF.SSP0_CLK_EN = 1;
      break;    
      
    case CLK_SSP1:
      PMU->PERI_CLK_EN.BF.SSP1_CLK_EN = 1;
      break;             
      
    case CLK_GPT0:
      PMU->PERI_CLK_EN.BF.GPT0_CLK_EN = 1;
      break;         
      
    case CLK_GPT1:
      PMU->PERI_CLK_EN.BF.GPT1_CLK_EN = 1;
      break;   
      
    case CLK_UART2:
      PMU->PERI_CLK_EN.BF.UART2_CLK_EN = 1;
      break;
 
    case CLK_UART3:
      PMU->PERI_CLK_EN.BF.UART3_CLK_EN = 1;
      break;      
      
    case CLK_SSP2:
      PMU->PERI_CLK_EN.BF.SSP2_CLK_EN = 1;
      break;   

    case CLK_I2C1:
      PMU->PERI_CLK_EN.BF.I2C1_CLK_EN = 1;
      break;

    case CLK_I2C2:
      PMU->PERI_CLK_EN.BF.I2C2_CLK_EN = 1;
      break; 
      
    case CLK_GPT2:
      PMU->PERI_CLK_EN.BF.GPT2_CLK_EN = 1;
      break;  
      
    case CLK_GPT3:
      PMU->PERI_CLK_EN.BF.GPT3_CLK_EN = 1;
      break;   

    case CLK_WDT:
      PMU->PERI_CLK_EN.BF.WDT_CLK_EN = 1;
      break;       
 
    case CLK_QSPI1:
      PMU->PERI_CLK_EN.BF.QSPI1_CLK_EN = 1;
      break;

    case CLK_SDIO:
      PMU->PERI_CLK_EN.BF.SDIO_CLK_EN = 1;
      break;

    case CLK_USBC:
      PMU->PERI_CLK_EN.BF.USBC_CLK_EN = 1;
      break;

    case CLK_FLASHC:
      PMU->PERI_CLK_EN.BF.FLASH_QSPI_CLK_EN = 1;
      break;

    case CLK_ACOMP:
      PMU->CAU_CTRL.BF.CAU_ACOMP_MCLK_EN = 0;
      break;  
      
    case CLK_DAC:
      PMU->CAU_CTRL.BF.CAU_GPDAC_MCLK_EN = 0;
      break;     

    case CLK_ADC1:
      PMU->CAU_CTRL.BF.CAU_GPADC1_MCLK_EN = 0;
      break;     

    case CLK_ADC0:
      PMU->CAU_CTRL.BF.CAU_GPADC0_MCLK_EN = 0;
      break;     
      
    default:
      break;
  }  
}


/****************************************************************************//**
 * @brief      Set module clock divider 
 *
 * @param[in]  clockModule: Select the clock module 
 * @param[in]  clockModule: Select the clock module 
 *
 * @return none
 *
 *******************************************************************************/
void CLK_ModuleClkDivider(CLK_Module_Type clkModule, uint32_t divider)
{
  /* check the parameters */
  CHECK_PARAM( IS_DIVIDED_CLK_MODULE(clkModule) ); 
  
  switch(clkModule)
  {
    case CLK_QSPI0:
      /* check the parameters */
      CHECK_PARAM( IS_QSPI_FLASHC_GPTSAMP_DIVIDER(divider) ); 
      PMU->PERI1_CLK_DIV.BF.QSPI0_CLK_DIV = divider;
      break;
      
    case CLK_I2C0:
    case CLK_I2C1:
    case CLK_I2C2:
      /* check the parameters */
      CHECK_PARAM( IS_APB_I2C_DIVIDER(divider) ); 
      PMU->PERI2_CLK_DIV.BF.I2C_CLK_DIV = divider;
      break;
      
    case CLK_SSP0:
      /* check the parameters */
      CHECK_PARAM( IS_SSP_DIVIDER(divider) ); 
      PMU->PERI0_CLK_DIV.BF.SSP0_CLK_DIV = divider;
      break;    
      
    case CLK_SSP1:
      /* check the parameters */
      CHECK_PARAM( IS_SSP_DIVIDER(divider) ); 
      PMU->PERI0_CLK_DIV.BF.SSP1_CLK_DIV = divider;
      break;             
      
    case CLK_GPT0:
      /* check the parameters */
      CHECK_PARAM( IS_CORE_GPT_DIVIDER(divider) ); 
      PMU->GPT0_CTRL.BF.GPT0_CLK_DIV = divider;
      break;         
      
    case CLK_GPT1:
      /* check the parameters */
      CHECK_PARAM( IS_CORE_GPT_DIVIDER(divider) ); 
      PMU->GPT1_CTRL.BF.GPT1_CLK_DIV = divider;
      break;   
      
    case CLK_SSP2:
      /* check the parameters */
      CHECK_PARAM( IS_SSP_DIVIDER(divider) ); 
      PMU->PERI0_CLK_DIV.BF.SSP2_CLK_DIV = divider;
      break;   

    case CLK_GPT2:
      /* check the parameters */
      CHECK_PARAM( IS_CORE_GPT_DIVIDER(divider) ); 
      PMU->GPT2_CTRL.BF.GPT2_CLK_DIV = divider;
      break;  
      
    case CLK_GPT3:
      /* check the parameters */
      CHECK_PARAM( IS_CORE_GPT_DIVIDER(divider) ); 
      PMU->PERI2_CLK_DIV.BF.GPT3_CLK_DIV_5_3 = ((divider & 0x38)>>3);
      PMU->PERI2_CLK_DIV.BF.GPT3_CLK_DIV_2_0 = (divider & 0x37);
      break;   

    case CLK_QSPI1:
      /* check the parameters */
      CHECK_PARAM( IS_QSPI_FLASHC_GPTSAMP_DIVIDER(divider) ); 
      PMU->PERI1_CLK_DIV.BF.QSPI1_CLK_DIV = divider;
      break;

    case CLK_SDIO:
      /* check the parameters */
      CHECK_PARAM( IS_SDIO_PMU_DIVIDER(divider) ); 
      PMU->PERI0_CLK_DIV.BF.SDIO_CLK_DIV = divider;
      break;

    case CLK_FLASHC:
      /* check the parameters */
      CHECK_PARAM( IS_QSPI_FLASHC_GPTSAMP_DIVIDER(divider) ); 
      PMU->PERI1_CLK_DIV.BF.FLASH_CLK_DIV = divider;
      break;

    case CLK_CORE:
      /* check the parameters */
      CHECK_PARAM( IS_CORE_GPT_DIVIDER(divider) ); 
      PMU->MCU_CORE_CLK_DIV.BF.FCLK_DIV = divider;
      break;  
      
    case CLK_APB0:
      /* check the parameters */
      CHECK_PARAM( IS_APB_I2C_DIVIDER(divider) ); 
      PMU->PERI1_CLK_DIV.BF.APB0_CLK_TO_AHB_CLK_RATIO = divider;
      break;     

    case CLK_APB1:
      /* check the parameters */
      CHECK_PARAM( IS_APB_I2C_DIVIDER(divider) ); 
      PMU->PERI1_CLK_DIV.BF.APB1_CLK_TO_AHB_CLK_RATIO = divider;
      break;     

    case CLK_PMU:
      /* check the parameters */
      CHECK_PARAM( IS_SDIO_PMU_DIVIDER(divider) ); 
      PMU->PERI1_CLK_DIV.BF.PMU_CLK_DIV = divider;
      break;     
      
    case CLK_GPT_SAMPLE:
      /* check the parameters */
      CHECK_PARAM( IS_QSPI_FLASHC_GPTSAMP_DIVIDER(divider) ); 
      PMU->PERI2_CLK_DIV.BF.GPT_SAMPLE_CLK_DIV = divider;
      break;     
    
    case CLK_WDT:      
      /* check the parameters */
      CHECK_PARAM( IS_WDT_DIVIDER(divider) ); 
      PMU->PERI2_CLK_DIV.BF.WDT_CLK_DIV_5_3 = ((divider & 0x38)>>3);
      PMU->PERI2_CLK_DIV.BF.WDT_CLK_DIV_2_2 = ((divider & 0x04)>>2);
      PMU->PERI2_CLK_DIV.BF.WDT_CLK_DIV_1_0 = (divider & 0x03);      
      break;     
      
    default:
      break;
  }  
}

/****************************************************************************//**
 * @brief      RC32M clock calibration function 
 *
 * @param[in]  rcCalAutoManOption: Calibration option, use manual or auto way
 * @param[in]  extCalCode: Select manual way, need input the calibration code
 *
 * @return     return the calibration result
 *             -1, calibration failure
 *             else, internal calibration code
 *
 * @note       Clock CLK_AUPLL_CAU must be ready before RC32M calibration
 *
 *******************************************************************************/
int32_t CLK_RC32MCalibration(CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode)
{
  volatile uint32_t localCnt;
  
  /* check the parameters */
  CHECK_PARAM( IS_RC_CALIBRATION_MODE(rcCalAutoManOption) );   
  
  /* Soft reset RC32M */
//  RC32M->RST.BF.SOFT_RST = 1;
//  for(localCnt=0; localCnt<10; localCnt++);
//  RC32M->RST.BF.SOFT_RST = 0;
  
  /* Power up RC32M */
  RC32M->CTRL.BF.PD = 0;
  RC32M->CTRL.BF.EN = 1;
  
  localCnt = 0;
  while(1)
  {
    /* Check RC32M_RDY status */ 
    if(PMU->CLK_RDY.BF.RC32M_RDY == 1)
    {
      break;
    }
    else if(localCnt > 5000000)
    {
      return -1;
    }
    
    localCnt++;
  }
  
  switch(rcCalAutoManOption)
  { 
    case CLK_AUTO_CAL:
      /* Auto */
      RC32M->CTRL.BF.EXT_CODE_EN = 0;
      PMU->RC32M_CTRL.BF.CAL_ALLOW = 1;
      RC32M->CTRL.BF.CAL_EN = 1;
      break;


    case CLK_MANUAL_CAL:
      /* Manual */
      RC32M->CTRL.BF.CODE_FR_EXT = extCalCode;
      RC32M->CTRL.BF.EXT_CODE_EN = 1;
      break;
    
  default:
      break;
  }
  
  localCnt = 0;
  while(1)
  {
    /* Check CAL_DONE status */ 
    if( (RC32M->STATUS.BF.CAL_DONE == 1) || (RC32M->CTRL.BF.EXT_CODE_EN == 1) )
    {
      break;
    }
    else if(localCnt > 5000000)
    {
      return -1;
    }
    
    localCnt++;
  }

  RC32M->CTRL.BF.CAL_EN = 0;
  PMU->RC32M_CTRL.BF.CAL_ALLOW = 0;
  
  return RC32M->STATUS.BF.CODE_FR_CAL;
}

/****************************************************************************//**
 * @brief      RC32M clock output frequency select function 
 *
 * @param[in]  freqType: Output frequency option, 16 MHz or 32 MHz
 *
 * @return     none
 *
 *******************************************************************************/
void RC32M_SelOutput32MHz(RC32M_SELFREQ_Type freqType)
{
  switch(freqType)
  {
  case RC32M_SELFREQ_16M:    
    RC32M->CLK.BF.SEL_32M = 0;
    break;
  case RC32M_SELFREQ_32M:    
    RC32M->CLK.BF.SEL_32M = 1;
    break;
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      RC32K clock calibration function 
 *
 * @param[in]  rcCalAutoManOption: Calibration option, use manual or auto way
 * @param[in]  extCalCode: Select manual way, need input the calibration code
 *
 * @return     return the calibration result
 *             -1, calibration failure
 *             else, internal calibration code
 *
 * @note       Clock CLK_XTAL32K must be ready before RC32K calibration
 *
 *******************************************************************************/
int32_t CLK_RC32KCalibration(CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode)
{
  volatile uint32_t localCnt;
  
  /* check the parameters */
  CHECK_PARAM( IS_RC_CALIBRATION_MODE(rcCalAutoManOption) );   
  
  /* Power up RC32K */
  PMU->RC32K_CTRL.BF.RC32K_PD = 0;
  
  localCnt = 0;
  while(1)
  {
    /* Check RC32K_RDY and X32K_RDY status */ 
    if( (PMU->RC32K_CTRL.BF.RC32K_RDY == 1) && (PMU->CLK_RDY.BF.X32K_RDY == 1) )
    {
      break;
    }
    else if(localCnt > 5000000)
    {
      return -1;
    }
    
    localCnt++;
  }
  
  switch(rcCalAutoManOption)
  { 
    case CLK_AUTO_CAL:
      /* Auto */
      PMU->RC32K_CTRL.BF.RC32K_EXT_CODE_EN = 0;
      PMU->RC32K_CTRL.BF.RC32K_ALLOW_CAL = 1;
      break;

    case CLK_MANUAL_CAL:
      /* Manual */
      PMU->RC32K_CTRL.BF.RC32K_CODE_FR_EXT = extCalCode;
      PMU->RC32K_CTRL.BF.RC32K_EXT_CODE_EN = 1;
      break;
    
  default:
      break;
  }
  
  /* Start Calibration */
  PMU->RC32K_CTRL.BF.RC32K_CAL_EN = 1;
  
  /* Add a short delay to prevent auto cal fail */
//  for(volatile uint32_t i=0; i<50000; i++);
  localCnt = 0;
  
  while(1)
  {
    if((PMU->RC32K_CTRL.BF.RC32K_CAL_DONE == 0) || (PMU->RC32K_CTRL.BF.RC32K_EXT_CODE_EN == 1) )
    {
      break;
    }
    else if(localCnt > 50000)
    {
      break;
    }    
    localCnt++;
  }
  
  localCnt = 0;
  while(1)
  {
    /* Check CAL_DONE status */ 
    if( (PMU->RC32K_CTRL.BF.RC32K_CAL_DONE == 1) || (PMU->RC32K_CTRL.BF.RC32K_EXT_CODE_EN == 1) )
    {
      break;
    }
    else if(localCnt > 5000000)
    {
      return -1;
    }      
    localCnt++;
  } 
  
  localCnt = 0;
  while(localCnt<=0x1000)
  {
    localCnt++;
  }

  PMU->RC32K_CTRL.BF.RC32K_CAL_EN = 0;
  PMU->RC32K_CTRL.BF.RC32K_ALLOW_CAL = 0;
  
  return PMU->RC32K_CTRL.BF.RC32K_CODE_FR_CAL;
}


/****************************************************************************//**
 * @brief      Get the clock status 
 *
 * @param[in]  clockSource: Clock to be checked
 *
 * @return     The clock status
 *
 *******************************************************************************/
FlagStatus CLK_GetClkStatus(CLK_Src_Type clockSource)
{
  FlagStatus bitStatus = RESET;
  
  /* check the parameters */
  CHECK_PARAM( IS_CLK_SRC_STATUS(clockSource) );    
  
  switch(clockSource)
  {
    case CLK_RC32K:
      bitStatus = ((PMU->RC32K_CTRL.BF.RC32K_RDY) ? SET : RESET);
      break;
      
    case CLK_RC32M:
      bitStatus = ((PMU->CLK_RDY.BF.RC32M_RDY) ? SET : RESET);
      break;
      
    case CLK_XTAL32K:
      bitStatus = ((PMU->CLK_RDY.BF.X32K_RDY) ? SET : RESET);
      break;
      
    case CLK_MAINXTAL:
      bitStatus = ((PMU->CLK_RDY.BF.XTAL32M_CLK_RDY) ? SET : RESET);
      break;
      
    case CLK_SFLL:
      bitStatus = ((PMU->CLK_RDY.BF.PLL_CLK_RDY) ? SET : RESET);
      break;
    
    case CLK_AUPLL:
      bitStatus = ((PMU->RSVD.BF.AUPLL_LOCK_STATUS) ? SET : RESET);
      break;      
      
    default:
      break;
  }
  
  return bitStatus;  
}

/****************************************************************************//**
 * @brief      Get the RC32M interrupt status 
 *
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return     Interrupt status value of the specified interrupt type
 *
 *******************************************************************************/
IntStatus RC32M_GetIntStatus(RC32M_INT_Type intType)
{
  IntStatus bitStatus = RESET;
  
  switch(intType)
  {
    /* RC32M calibration done interrupt */
    case RC32M_CALDON:
      bitStatus = ((RC32M->ISR.BF.CALDON_INT) ? SET : RESET);
      break;
      
    /* RC32M clock out ready interrupt */
    case RC32M_CKRDY:
      bitStatus = ((RC32M->ISR.BF.CKRDY_INT) ? SET : RESET);
      break;
      
    default:
      break;
  }
  
  return bitStatus;  
}

/****************************************************************************//**
 * @brief      Mask/Unmask the specified RC32M interrupt type 
 *
 * @param[in]  intType:  Specifies the interrupt type
 * @param[in]  intMask:  Mask/Unmask the specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void RC32M_IntMask(RC32M_INT_Type intType, IntMask_Type intMask)
{
  switch(intType)
  {
    /* RC32M calibration done interrupt */
    case RC32M_CALDON:
      RC32M->IMR.BF.CALDON_INT_MSK = intMask;
      break;
      
    /* RC32M clock out ready interrupt */
    case RC32M_CKRDY:
      RC32M->IMR.BF.CKRDY_INT_MSK = intMask;
      break;
      
    default:
      break;
  }  
}

 /****************************************************************************//**
 * @brief      Clear the RC32M interrupt flag 
 *
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void RC32M_IntClr(RC32M_INT_Type intType)
{
  switch(intType)
  {
    /* RC32M calibration done interrupt */
    case RC32M_CALDON:
      RC32M->ICR.BF.CALDON_INT_CLR = 1;
      break;
      
    /* RC32M clock out ready interrupt */
    case RC32M_CKRDY:
      RC32M->ICR.BF.CKRDY_INT_CLR = 1;
      break;

    default:
      break;
  }
  
}

/****************************************************************************//**
 * @brief  RC32M interrupt function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void RC32M_IRQHandler(void)
{
  uint32_t intStatus;
  
  /* Store unmasked interrupt flags for later use */
  intStatus = (~RC32M->IMR.WORDVAL) & 0x00000003;
  intStatus &= RC32M->ISR.WORDVAL;
  
  /* Clear all unmasked interrupt flags */
  RC32M->ICR.WORDVAL = intStatus;
  
  /* RC32M calibration done interrupt */
  if( intStatus & (1 << RC32M_CALDON) )
  {
    if(intCbfArra[INT_RC32M][RC32M_CALDON] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_RC32M][RC32M_CALDON]();
    }
    else
    {
      RC32M->IMR.BF.CALDON_INT_MSK = 1;
    }
  }
  
  /* RC32M clock out ready interrupt */
  if( intStatus & (1 << RC32M_CKRDY) )
  {
    if(intCbfArra[INT_RC32M][RC32M_CKRDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_RC32M][RC32M_CKRDY]();
    }
    else
    {
      RC32M->IMR.BF.CKRDY_INT_MSK = 1;
    }
  }
  
}

static int CLK_Select_Kvco(int freq)
{
	if ((freq > 100000000) && (freq <= 110000000))
		return 0;
	if ((freq > 110000000) && (freq <= 121000000))
		return 1;
	if ((freq > 121000000) && (freq <= 133000000))
		return 2;
	if ((freq > 133000000) && (freq <= 146000000))
		return 3;
	if ((freq > 146000000) && (freq <= 160000000))
		return 4;
	if ((freq > 160000000) && (freq <= 175000000))
		return 5;
	if ((freq > 175000000) && (freq <= 192000000))
		return 6;
	if ((freq > 192000000) && (freq <= 200000000))
		return 7;

	return -1;
}

static void CLK_Config_Pll(int ref_clk, CLK_Src_Type type)
{
	int i;
	double frac = (double)board_cpu_freq() / (double)ref_clk;
	CLK_SfllConfig_Type sfllConfigSet;

	if (CLK_GetClkStatus(CLK_SFLL) == RESET) {
		sfllConfigSet.refClockSrc = type;
		for (i = 1; i < 512; i++)
			if ((frac * i) == (int)(frac * i) &&
				((ref_clk / i) >= 200000) &&
				((ref_clk / i) <= 400000))
				break;

		/* Configure the SFLL */
		sfllConfigSet.refDiv = i;
		sfllConfigSet.fbDiv = (int)(frac * i) - 1;

		sfllConfigSet.kvco = CLK_Select_Kvco(board_cpu_freq());
		sfllConfigSet.postDiv = 0;
		CLK_SfllEnable(&sfllConfigSet);
		while(CLK_GetClkStatus(CLK_SFLL) == RESET)
			;
	}
}

void CLK_Init()
{
	int freq;
	CLK_Src_Type type;

	if (board_main_xtal() > 0) {
		/* Enable Internal Oscillator */
		CLK_MainXtalEnable(CLK_OSC_INTERN);
		while (CLK_GetClkStatus(CLK_MAINXTAL) == RESET)
			;
		freq = board_main_xtal();
		type = CLK_MAINXTAL;
	} else if (board_main_osc() > 0) {
		/* Enable External Oscillator */
		CLK_MainXtalEnable(CLK_OSC_EXTERN);
		while (CLK_GetClkStatus(CLK_MAINXTAL) == RESET)
			;
		freq = board_main_osc();
		type = CLK_MAINXTAL;
	} else {
		/* TODO: Calibrate and enable internal 32MHz RC circuit */
		freq = 32000000;
		type = CLK_RC32M;
	}

	/*
	 * Check if expected cpu frequency is greater than the
	 * source clock frequency. In that case we need to enable
	 * the PLL.
	 */
	if (board_cpu_freq() > freq) {
		CLK_Config_Pll(freq, type);
		freq = board_cpu_freq();
		type = CLK_SFLL;
	}

	if (freq > 50000000) {
		/* Max APB0 freq 50MHz */
		CLK_ModuleClkDivider(CLK_APB0, 2);
		/* Max APB1 freq 25MHz */
		CLK_ModuleClkDivider(CLK_APB1, 3);
	}

	/* Select clock source */
	CLK_SystemClkSrc(type);
}

/*@} end of group Clock_Public_Functions */
  
/*@} end of group Clock */

/*@} end of group MC200_Periph_Driver */

