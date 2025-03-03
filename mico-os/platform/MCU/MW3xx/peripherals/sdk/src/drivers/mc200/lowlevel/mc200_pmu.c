/****************************************************************************//**
 * @file     mc200_pmu.c
 * @brief    This file provides PMU functions.
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

#include "mc200_pmu.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @defgroup PMU
 *  @brief PMU driver modules
 *  @{
 */

/** @defgroup PMU_Private_Type
 *  @{
 */

/**
 *  @brief PMU interrupt status type definition
 */


/**
 * @brief PMU interrupt call-back function type definitions
 */



/*@} end of group PMU_Private_Type*/

/** @defgroup PMU_Private_Defines
 *  @{
 */


/* PMU interrupt status bit mask */


/*@} end of group PMU_Private_Defines */


/** @defgroup PMU_Private_Variables
 *  @{
 */


		
/*@} end of group PMU_Private_Variables */

/** @defgroup PMU_Global_Variables
 *  @{
 */


/*@} end of group PMU_Global_Variables */


/** @defgroup PMU_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group PMU_Private_FunctionDeclaration */

/** @defgroup PMU_Private_Functions
 *  @{
 */


/*@} end of group PMU_Private_Functions */


/** @defgroup PMU_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief     Set system will be in sleep mode
 *
 * @param[in]  pmuMode:  power mode option
 *
 * @return none
 *
 *******************************************************************************/
void PMU_SetSleepMode(PMU_SleepMode_Type pmuMode)
{
  /* set PMU basic mode */
  PMU->PWR_MODE.BF.PWR_MODE = pmuMode;

  /* select deepsleep or not */
  if(pmuMode == PMU_PM1)
  {
    SCB->SCR &= !SCB_SCR_SLEEPDEEP_Msk;
  }
  else
  {
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  }
}

/****************************************************************************//**
 * @brief     Get system sleep mode
 *
 * @param[in]  none
 *
 * @return  rerurn power mode
 *
 *******************************************************************************/
uint32_t PMU_GetSleepMode(void)
{
  return (uint32_t) PMU->PWR_MODE.BF.PWR_MODE;
}

/****************************************************************************//**
 * @brief      wakeup pin trigger type set
 *
 * @param[in]  wakeuppin: wakeup source
 * @param[in]  trigmode: trigger mode selector
 *
 * @return     Return none
 *
 *******************************************************************************/
void PMU_ConfigWakeupPin(PMU_WakeupPinSrc_Type wakeupPin, PMU_WakeupTriggerMode_Type trigmode)
{
  switch(wakeupPin)
  {
  /* case GPIO25 interrupt */
  case PMU_GPIO25_INT:
    if(trigmode == PMU_WAKEUP_LEVEL_HIGH)
    {
      PMU->WAKEUP_EDGE_DETECT.BF.WAKEUP0 = 1;
    }
    else if(trigmode == PMU_WAKEUP_LEVEL_LOW)
    {
      PMU->WAKEUP_EDGE_DETECT.BF.WAKEUP0 = 0;
    }
    break;
    
  /* case GPIO26 interrupt */
  case PMU_GPIO26_INT:
    if(trigmode == PMU_WAKEUP_LEVEL_HIGH)
    {
      PMU->WAKEUP_EDGE_DETECT.BF.WAKEUP1 = 1;
    }
    else if(trigmode == PMU_WAKEUP_LEVEL_LOW)
    {
      PMU->WAKEUP_EDGE_DETECT.BF.WAKEUP1 = 0;
    }
    break;
 
  /* default */
  default:
    break;

  }
}

/****************************************************************************//**
 * @brief      wakeup source clear
 *
 * @param[in]  wakeuppin: wakeup source to be cleared
 *
 * @return     Return none
 *
 *******************************************************************************/
void PMU_ClearWakeupSrc(PMU_WakeupPinSrc_Type wakeupPin)
{
  switch(wakeupPin)
  {
  /* case GPIO25 interrupt */
  case PMU_GPIO25_INT:
    PMU->WAKE_SRC_CLR.BF.CLR_PIN_INT0 = 1;
    break;
  
  /* case GPIO26 interrupt */
  case PMU_GPIO26_INT:
    PMU->WAKE_SRC_CLR.BF.CLR_PIN_INT1 = 1;
    break;
  
  /* default */
  default:
    break;  
  }
}

/****************************************************************************//**
 * @brief      wakeup pad mode set
 *
 * @param[in]  wakeuppin: wakeup source
 * @param[in]  padmode:  wakeup pad pull up / down mode selector
 *
 * @return     Return none
 *
 *******************************************************************************/
void PMU_ConfigWakeupPadMode(PMU_WakeupPinSrc_Type wakeupPin, PMU_WakeupPadMode_Type padmode)
{
  switch(wakeupPin)
  {
  case PMU_GPIO25_INT:
    if(padmode == PMU_WAKEUP_PAD_PULLUP)
    {
      PMU->WAKEUP_PUPD_CTRL.BF.WAKEUP0_PUPD_CTRL = 0;
    }
    else if(padmode == PMU_WAKEUP_PAD_PULLDOWN)
    {
      PMU->WAKEUP_PUPD_CTRL.BF.WAKEUP0_PUPD_CTRL = 1;
    }
    break;
    
  case PMU_GPIO26_INT:
    if(padmode == PMU_WAKEUP_PAD_PULLUP)
    {
      PMU->WAKEUP_PUPD_CTRL.BF.WAKEUP1_PUPD_CTRL = 0;
    }
    else if(padmode == PMU_WAKEUP_PAD_PULLDOWN)
    {
      PMU->WAKEUP_PUPD_CTRL.BF.WAKEUP1_PUPD_CTRL = 1;
    }
    break;
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Get last reset cause
 *
 * @param  none
 *
 * @return     Return last reset cause
 *
 *******************************************************************************/
uint32_t PMU_GetLastResetCause(void)
{
  return PMU->LAST_RST_CAUSE.WORDVAL;
}


/****************************************************************************//**
 * @brief      Clear last reset cause
 *
 * @param[in]  resetCause: reset cause to be cleared
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ClrLastResetCause(PMU_LastResetCause_Type resetCause)
{
  switch(resetCause)
  {
  case PMU_BROWNOUT_VBAT:
    PMU->LAST_RST_CLR.BF.BROWNOUT_VBAT_CLR = 1;
    break;
    
  case PMU_CM3_SYSRESETREQ:
    PMU->LAST_RST_CLR.BF.CM3_SYSRESETREQ_CLR = 1;
    break;
    
  case PMU_CM3_LOCKUP:
    PMU->LAST_RST_CLR.BF.CM3_LOCKUP_CLR = 1;
    break;
    
  case PMU_WDTOUT:
    PMU->LAST_RST_CLR.BF.WDT_RST_CLR = 1;
    break;
    
  case PMU_RESETSRC_ALL:
    PMU->LAST_RST_CLR.WORDVAL = 0x7F;
    break;
    
  default:
    break;    
  }
}

/****************************************************************************//**
 * @brief      power saving pad mode set
 *
 * @param[in]  pad: power saving pad source
 * @param[in]  padmode:  power saving pad normal / power save mode selector
 *
 * @return     Return none
 *
 *******************************************************************************/
void PMU_ConfigPowerSavePadMode(PMU_PowerSavePadSrc_Type pad, PMU_PowerSavePadMode_Type padmode)
{
  switch(pad)
  {
  case PMU_PAD_XTAL32K_IN:
    PMU->PAD_CTRL0_REG.BF.XTAL32K_IN_CTRL = padmode & 0x1;
    break;
    
  case PMU_PAD_XTAL32K_OUT:
    PMU->PAD_CTRL0_REG.BF.XTAL32K_OUT_CTRL = padmode & 0x1;
    break;
    
  case PMU_PAD_TDO:
    PMU->PAD_CTRL0_REG.BF.TDO_CTRL = padmode & 0x1;
    break;
    
  case PMU_PAD_ALLTHREE:
    if(padmode == PMU_PAD_MODE_NORMAL)
    {
      PMU->PAD_CTRL0_REG.WORDVAL = 0x0;
    }
    else if(padmode == PMU_PAD_MODE_POWER_SAVING)
    {
      PMU->PAD_CTRL0_REG.WORDVAL = 0xE;
    }
    break;
    
  default:
    break;
    
  }
}

/****************************************************************************//**
 * @brief      x32k output pad mode set
 *
 * @param[in]  pad: x32k output pad source
 * @param[in]  padmode:  x32k output pad normal pinmux / x32k output mode selector
 *
 * @return     Return none
 *
 *******************************************************************************/
void PMU_ConfigX32KOutputPadMode(PMU_X32KOutputPadSrc_Type pad, PMU_X32KOutputPadMode_Type padmode)
{
  switch(pad)
  {
  case PMU_PAD_GPIO27:
    PMU->PAD_CTRL1_REG.BF.GPIO_27_CTRL = padmode & 0x1;
    break;
    
  case PMU_PAD_GPIO25:
    PMU->PAD_CTRL1_REG.BF.WAKEUP0_CTRL = padmode & 0x1;
    break;
    
  case PMU_PAD_GPIO26:
    PMU->PAD_CTRL1_REG.BF.WAKEUP1_CTRL = padmode & 0x1;
    break;
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Power on specified VDDIO domain
 *
 * @param[in]  domain: VDDIO domain to be awitched on
 *
 * @return none
 *
 *******************************************************************************/
void PMU_PowerOnVDDIO(PMU_VDDIODOMAIN_Type domain)
{
  switch(domain)
  {
  case PMU_VDDIO_D0:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO1_REG_PDB_CORE = 1;
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_GPIO0_LOW_VDDB_CORE = 1;
    break;
    
  case PMU_VDDIO_AON:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO2_REG_PDB_CORE = 1;
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_AON_LOW_VDDB_CORE = 1;
    break;
    
  case PMU_VDDIO_D1:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO6_REG_PDB_CORE = 1;
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO4_REG_PDB_CORE = 1;
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_GPIO1_LOW_VDDB_CORE = 1;
    break;
    
  case PMU_VDDIO_SDIO:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO7_REG_PDB_CORE = 1;
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_SDIO_LOW_VDDB_CORE = 1;
    break;
    
  case PMU_VDDIO_D2:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO9_REG_PDB_CORE = 1;
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_GPIO2_LOW_VDDB_CORE = 1;
    break;    
    
  case PMU_VDDIO_FL:
    PMU->IO_PAD_PWR_CFG.BF.VDDO_FL_REG_PDB_CORE = 1;
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_FL_LOW_VDDB_CORE = 1;
    break;    
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Power off specified VDDIO domain
 *
 * @param[in]  domain: VDDIO domain to be awitched on
 *
 * @return none
 *
 *******************************************************************************/
void PMU_PowerOffVDDIO(PMU_VDDIODOMAIN_Type domain)
{
  switch(domain)
  {
  case PMU_VDDIO_D0:
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_GPIO0_LOW_VDDB_CORE = 0;
    break;
    
  case PMU_VDDIO_AON:
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_AON_LOW_VDDB_CORE = 0;
    break;
    
  case PMU_VDDIO_D1:
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_GPIO1_LOW_VDDB_CORE = 0;
    break;
    
  case PMU_VDDIO_SDIO:
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_SDIO_LOW_VDDB_CORE = 0;
    break;
    
  case PMU_VDDIO_D2:
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_GPIO2_LOW_VDDB_CORE = 0;
    break;    
    
  case PMU_VDDIO_FL:
    PMU->IO_PAD_PWR_CFG.BF.POR_LVL_FL_LOW_VDDB_CORE = 0;
    break;   
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Deep power down specified VDDIO domain
 *
 * @param[in]  domain: VDDIO domain to be deep powered down
 *
 * @return none
 *
 *******************************************************************************/
void PMU_PowerDeepOffVDDIO(PMU_VDDIODOMAIN_Type domain)
{
  switch(domain)
  {
  case PMU_VDDIO_D0:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO1_REG_PDB_CORE = 0;
    break;
    
  case PMU_VDDIO_AON:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO2_REG_PDB_CORE = 0;
    break;
    
  case PMU_VDDIO_D1:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO6_REG_PDB_CORE = 0;
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO4_REG_PDB_CORE = 0;
    break;
    
  case PMU_VDDIO_SDIO:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO7_REG_PDB_CORE = 0;
    break;
    
  case PMU_VDDIO_D2:
    PMU->IO_PAD_PWR_CFG.BF.VDD_IO9_REG_PDB_CORE = 0;
    break;    
    
  case PMU_VDDIO_FL:
    PMU->IO_PAD_PWR_CFG.BF.VDDO_FL_REG_PDB_CORE = 0;
    break;   
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Configure power domain level
 *
 * @param[in]  domain: VDDIO domain to be configured
 * @param[in]  level: VDDIO level
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ConfigVDDIOLevel(PMU_VDDIODOMAIN_Type domain, PMU_VDDIOLEVEL_Type level)
{
  switch(domain)
  {
  case PMU_VDDIO_D0:
    PMU->IO_PAD_PWR_CFG.BF.V18EN_LVL_GPIO0_V18EN_CORE = level & 0x1;
    break;
    
  case PMU_VDDIO_AON:
    PMU->IO_PAD_PWR_CFG.BF.V18EN_LVL_AON_V18EN_CORE = level & 0x1;
    break;
    
  case PMU_VDDIO_D1:
    PMU->IO_PAD_PWR_CFG.BF.V18EN_LVL_GPIO1_V18EN_CORE = level & 0x1;
    break;
    
  case PMU_VDDIO_SDIO:
    PMU->IO_PAD_PWR_CFG.BF.V18EN_LVL_SDIO_V18EN_CORE = level & 0x1;
    break;
    
  case PMU_VDDIO_D2:
    PMU->IO_PAD_PWR_CFG.BF.V18EN_LVL_GPIO2_V18EN_CORE = level & 0x1;
    break;   
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Configure extra interrupt
 *
 * @param[in]  gpioPin: gpio pin to be configured
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ConfigExtraInterrupt(PMU_EXTRAINT_Type gpioPin)
{
  switch(gpioPin)
  {
  case PMU_INT34_GPIO_0:
    PMU->EXT_SEL_REG0.BF.SEL_34 = 0;
    break;
    
  case PMU_INT34_GPIO_1:
    PMU->EXT_SEL_REG0.BF.SEL_34 = 1;
    break;
    
  case PMU_INT34_GPIO_2:
    PMU->EXT_SEL_REG0.BF.SEL_34 = 2;
    break;
    
  case PMU_INT35_GPIO_3:
    PMU->EXT_SEL_REG0.BF.SEL_35 = 0;
    break;
    
  case PMU_INT35_GPIO_4:
    PMU->EXT_SEL_REG0.BF.SEL_35 = 1;
    break;
    
  case PMU_INT35_GPIO_5:
    PMU->EXT_SEL_REG0.BF.SEL_35 = 2;
    break;
    
  case PMU_INT36_GPIO_6:
    PMU->EXT_SEL_REG0.BF.SEL_36 = 0;
    break;
    
  case PMU_INT36_GPIO_7:
    PMU->EXT_SEL_REG0.BF.SEL_36 = 1;
    break;
    
  case PMU_INT36_GPIO_8:
    PMU->EXT_SEL_REG0.BF.SEL_36 = 2;
    break;
    
  case PMU_INT37_GPIO_9:
    PMU->EXT_SEL_REG0.BF.SEL_37 = 0;
    break;
    
  case PMU_INT37_GPIO_10:
    PMU->EXT_SEL_REG0.BF.SEL_37 = 1;
    break;
    
  case PMU_INT37_GPIO_11:
    PMU->EXT_SEL_REG0.BF.SEL_37 = 2;
    break;    
    
  case PMU_INT38_GPIO_12:
    PMU->EXT_SEL_REG0.BF.SEL_38 = 0;
    break;
    
  case PMU_INT38_GPIO_13:
    PMU->EXT_SEL_REG0.BF.SEL_38 = 1;
    break;
    
  case PMU_INT38_GPIO_14:
    PMU->EXT_SEL_REG0.BF.SEL_38 = 2;
    break;    

  case PMU_INT39_GPIO_15:
    PMU->EXT_SEL_REG0.BF.SEL_39 = 0;
    break;
    
  case PMU_INT39_GPIO_16:
    PMU->EXT_SEL_REG0.BF.SEL_39 = 1;
    break;
    
  case PMU_INT39_GPIO_17:
    PMU->EXT_SEL_REG0.BF.SEL_39 = 2;
    break;    
   
  case PMU_INT40_GPIO_18:
    PMU->EXT_SEL_REG0.BF.SEL_40 = 0;
    break;
    
  case PMU_INT40_GPIO_19:
    PMU->EXT_SEL_REG0.BF.SEL_40 = 1;
    break;
    
  case PMU_INT40_GPIO_20:
    PMU->EXT_SEL_REG0.BF.SEL_40 = 2;
    break;       

  case PMU_INT41_GPIO_21:
    PMU->EXT_SEL_REG0.BF.SEL_41 = 0;
    break;
    
  case PMU_INT41_GPIO_22:
    PMU->EXT_SEL_REG0.BF.SEL_41 = 1;
    break;
    
  case PMU_INT41_GPIO_23:
    PMU->EXT_SEL_REG0.BF.SEL_41 = 2;
    break;  

  case PMU_INT42_GPIO_24:
    PMU->EXT_SEL_REG0.BF.SEL_42 = 0;
    break;
    
  case PMU_INT42_GPIO_28:
    PMU->EXT_SEL_REG0.BF.SEL_42 = 1;
    break;
    
  case PMU_INT42_GPIO_29:
    PMU->EXT_SEL_REG0.BF.SEL_42 = 2;
    break;  

  case PMU_INT43_GPIO_30:
    PMU->EXT_SEL_REG0.BF.SEL_43 = 0;
    break;
    
  case PMU_INT43_GPIO_31:
    PMU->EXT_SEL_REG0.BF.SEL_43 = 1;
    break;
    
  case PMU_INT43_GPIO_32:
    PMU->EXT_SEL_REG0.BF.SEL_43 = 2;
    break;       
    
  case PMU_INT44_GPIO_33:
    PMU->EXT_SEL_REG0.BF.SEL_44 = 0;
    break;
    
  case PMU_INT44_GPIO_34:
    PMU->EXT_SEL_REG0.BF.SEL_44 = 1;
    break;
    
  case PMU_INT44_GPIO_35:
    PMU->EXT_SEL_REG0.BF.SEL_44 = 2;
    break;           

  case PMU_INT45_GPIO_36:
    PMU->EXT_SEL_REG0.BF.SEL_45 = 0;
    break;
    
  case PMU_INT45_GPIO_37:
    PMU->EXT_SEL_REG0.BF.SEL_45 = 1;
    break;
    
  case PMU_INT45_GPIO_38:
    PMU->EXT_SEL_REG0.BF.SEL_45 = 2;
    break;     

  case PMU_INT46_GPIO_39:
    PMU->EXT_SEL_REG0.BF.SEL_46 = 0;
    break;
    
  case PMU_INT46_GPIO_40:
    PMU->EXT_SEL_REG0.BF.SEL_46 = 1;
    break;
    
  case PMU_INT46_GPIO_41:
    PMU->EXT_SEL_REG0.BF.SEL_46 = 2;
    break;    

  case PMU_INT47_GPIO_42:
    PMU->EXT_SEL_REG0.BF.SEL_47 = 0;
    break;
    
  case PMU_INT47_GPIO_43:
    PMU->EXT_SEL_REG0.BF.SEL_47 = 1;
    break;
    
  case PMU_INT47_GPIO_44:
    PMU->EXT_SEL_REG0.BF.SEL_47 = 2;
    break;   

  case PMU_INT48_GPIO_45:
    PMU->EXT_SEL_REG0.BF.SEL_48 = 0;
    break;
    
  case PMU_INT48_GPIO_46:
    PMU->EXT_SEL_REG0.BF.SEL_48 = 1;
    break;
    
  case PMU_INT48_GPIO_47:
    PMU->EXT_SEL_REG0.BF.SEL_48 = 2;
    break;   
    
  case PMU_INT49_GPIO_48:
    PMU->EXT_SEL_REG0.BF.SEL_49 = 0;
    break;
    
  case PMU_INT49_GPIO_49:
    PMU->EXT_SEL_REG0.BF.SEL_49 = 1;
    break;
    
  case PMU_INT49_GPIO_50:
    PMU->EXT_SEL_REG0.BF.SEL_49 = 2;
    break;   
 
  case PMU_INT50_GPIO_51:
    PMU->EXT_SEL_REG1.BF.SEL_50 = 0;
    break;
    
  case PMU_INT50_GPIO_52:
    PMU->EXT_SEL_REG1.BF.SEL_50 = 1;
    break;
    
  case PMU_INT50_GPIO_53:
    PMU->EXT_SEL_REG1.BF.SEL_50 = 2;
    break; 
    
  case PMU_INT51_GPIO_54:
    PMU->EXT_SEL_REG1.BF.SEL_51 = 0;
    break;
    
  case PMU_INT51_GPIO_55:
    PMU->EXT_SEL_REG1.BF.SEL_51 = 1;
    break;
    
  case PMU_INT51_GPIO_56:
    PMU->EXT_SEL_REG1.BF.SEL_51 = 2;
    break; 
    
  case PMU_INT52_GPIO_57:
    PMU->EXT_SEL_REG1.BF.SEL_52 = 0;
    break;
    
  case PMU_INT52_GPIO_58:
    PMU->EXT_SEL_REG1.BF.SEL_52 = 1;
    break;
    
  case PMU_INT52_GPIO_59:
    PMU->EXT_SEL_REG1.BF.SEL_52 = 2;
    break; 
    
  case PMU_INT53_GPIO_60:
    PMU->EXT_SEL_REG1.BF.SEL_53 = 0;
    break;
    
  case PMU_INT53_GPIO_61:
    PMU->EXT_SEL_REG1.BF.SEL_53 = 1;
    break;
    
  case PMU_INT53_GPIO_62:
    PMU->EXT_SEL_REG1.BF.SEL_53 = 2;
    break; 
    
  case PMU_INT54_GPIO_63:
    PMU->EXT_SEL_REG1.BF.SEL_54 = 0;
    break;
    
  case PMU_INT54_GPIO_64:
    PMU->EXT_SEL_REG1.BF.SEL_54 = 1;
    break;
    
  case PMU_INT54_GPIO_65:
    PMU->EXT_SEL_REG1.BF.SEL_54 = 2;
    break; 
    
  case PMU_INT55_GPIO_66:
    PMU->EXT_SEL_REG1.BF.SEL_55 = 0;
    break;
    
  case PMU_INT55_GPIO_67:
    PMU->EXT_SEL_REG1.BF.SEL_55 = 1;
    break;
    
  case PMU_INT55_GPIO_68:
    PMU->EXT_SEL_REG1.BF.SEL_55 = 2;
    break; 
    
  case PMU_INT56_GPIO_69:
    PMU->EXT_SEL_REG1.BF.SEL_56 = 0;
    break;
    
  case PMU_INT56_GPIO_70:
    PMU->EXT_SEL_REG1.BF.SEL_56 = 1;
    break;
    
  case PMU_INT56_GPIO_71:
    PMU->EXT_SEL_REG1.BF.SEL_56 = 2;
    break; 
    
  case PMU_INT57_GPIO_72:
    PMU->EXT_SEL_REG1.BF.SEL_57 = 0;
    break;
    
  case PMU_INT57_GPIO_73:
    PMU->EXT_SEL_REG1.BF.SEL_57 = 1;
    break;
    
  case PMU_INT57_GPIO_74:
    PMU->EXT_SEL_REG1.BF.SEL_57 = 2;
    break; 
    
  case PMU_INT58_GPIO_75:
    PMU->EXT_SEL_REG1.BF.SEL_58 = 0;
    break;
    
  case PMU_INT58_GPIO_76:
    PMU->EXT_SEL_REG1.BF.SEL_58 = 1;
    break;
    
  case PMU_INT58_GPIO_77:
    PMU->EXT_SEL_REG1.BF.SEL_58 = 2;
    break; 
    
  case PMU_INT59_GPIO_78:
    PMU->EXT_SEL_REG1.BF.SEL_59 = 0;
    break;
    
  case PMU_INT59_GPIO_79:
    PMU->EXT_SEL_REG1.BF.SEL_59 = 1;
    break;
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Select ulpcomp operation mode --- single-ended / differential
 *
* @param[in]  mode: select the mode
 *
 * @return none
 *
 *******************************************************************************/
void PMU_UlpcompModeSelect(PMU_UlpcompMode_Type ulpcompMode)
{
  PMU->PMIP_CMP_CTRL.BF.COMP_DIFF_EN = ulpcompMode;
}

/****************************************************************************//**
 * @brief      Select ulpcomp reference voltage for single-ended mode
 *
* @param[in]  refVolType: select the reference voltage level
 *
 * @return none
 *
 *******************************************************************************/
void PMU_UlpcompRefVoltageSel(PMU_UlpcompRef_Type refVolType)
{
  PMU->PMIP_CMP_CTRL.BF.COMP_REF_SEL = refVolType;
}

/****************************************************************************//**
 * @brief      Select ulpcomp hysteresis level
 *
* @param[in]  hystType: select the hysteresis level
 *
 * @return none
 *
 *******************************************************************************/
void PMU_UlpcompHysteresisSel(PMU_UlpcompHyst_Type hystType)
{
  PMU->PMIP_CMP_CTRL.BF.COMP_HYST = hystType;
}

/****************************************************************************//**
 * @brief      Enable/Disable ulpcomp
 *
 * @param[in]  state:  enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void PMU_UlpcompCmd(FunctionalState state)
{
  PMU->PMIP_CMP_CTRL.BF.COMP_EN = state;
}

/****************************************************************************//**
 * @brief      Get ulpcomp ready status
 *
 * @param[in]  none
 *
 * @return status
 *
 *******************************************************************************/
FlagStatus PMU_GetUlpcompStatus(void)
{
  uint32_t rdyStatus = 0;
  
  rdyStatus = PMU->PMIP_CMP_CTRL.BF.COMP_RDY;
  
  if(rdyStatus)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/****************************************************************************//**
 * @brief      Get ulpcomp output value
 *
 * @param[in]  none
 *
 * @return ulpcomp output value
 *
 *******************************************************************************/
uint32_t PMU_GetUlpcompOutValue(void)
{
  return (uint32_t) PMU->PMIP_CMP_CTRL.BF.COMP_OUT;
}

/****************************************************************************//**
 * @brief      Initialize vbat brown out detection
 *
* @param[in]  brndetConfig: Pointer to a vbat brndet configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ConfigVbatBrndet(PMU_VbatBrndetConfig_Type* brndetConfig)
{
  PMU->PMIP_BRNDET_VBAT.BF.BRNTRIG_VBAT_CNTL = brndetConfig->brnTrigVolt;
  
  PMU->PMIP_BRNDET_VBAT.BF.BRNHYST_VBAT_CNTL = brndetConfig->brnHyst;
  
  PMU->PMIP_BRNDET_VBAT.BF.BRNDET_VBAT_FILT = brndetConfig->brnFilter;
}

/****************************************************************************//**
 * @brief      Enable/Disable vbat brndet
 *
 * @param[in]  state:  enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void PMU_VbatBrndetCmd(FunctionalState state)
{
  PMU->PMIP_BRNDET_VBAT.BF.BRNDET_VBAT_EN = state;
}

/****************************************************************************//**
 * @brief      Enable/Disable vbat brndet reset
 *
 * @param[in]  state:  enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void PMU_VbatBrndetRstCmd(FunctionalState state)
{
  PMU->PMIP_BRN_CFG.BF.BRNDET_VBAT_RST_EN = state;
}

/****************************************************************************//**
 * @brief      Get vbat brndet ready status
 *
 * @param[in]  none
 *
 * @return status
 *
 *******************************************************************************/
FlagStatus PMU_GetVbatBrndetStatus(void)
{
  uint32_t rdyStatus = 0;
  
  rdyStatus = PMU->PMIP_BRNDET_VBAT.BF.BRNDET_VBAT_RDY;
  
  if(rdyStatus)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/****************************************************************************//**
 * @brief      Get vbat brndet output value
 *
 * @param[in]  none
 *
 * @return ulpcomp output value
 *
 *******************************************************************************/
uint32_t PMU_GetVbatBrndetOutValue(void)
{
  return (uint32_t) PMU->PMIP_BRNDET_VBAT.BF.BRNDET_VBAT_OUT;
}

/****************************************************************************//**
 * @brief      Enable/Disable vbat brndet interrupt
 *
 * @param[in]  state:  enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void PMU_VbatBrndetIntCmd(FunctionalState state)
{
  PMU->PMIP_BRN_INT_SEL.BF.PMIP_BRN_INT_SEL = state;
}

/****************************************************************************//**
 * @brief      Set VFL, AV18, V12 power ready signal delay time
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ConfigPowerReadyDelayTime(PMU_PowerReadyDelayConfig_Type* delayConfig)
{
  /* set VFL power ready delay time */
  PMU->PMIP_BRNDET_VFL.BF.DEL_VFL_SEL = delayConfig->vflDelay;
  
  /* set AV18 power ready delay time */
  PMU->PMIP_BRNDET_AV18.BF.DEL_AV18_SEL = delayConfig->v18Delay; 
  
  /* set V12 power ready delay time */
  PMU->PMIP_CHP_CTRL0.BF.DEL_V12_SEL = delayConfig->v12Delay;
}

/****************************************************************************//**
 * @brief      Set AV18 LDO, V12 LDO ramp up time
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ConfigPowerRampUpTime(PMU_PowerRampRateConfig_Type* rampConfig)
{
  /* set AV18 LDO ramp rate */
  PMU->PMIP_BRNDET_AV18.BF.LDO_AV18_RAMP_RATE = rampConfig->v18ramp;

  /* set V12 LDO ramp rate */
  PMU->PMIP_BRNDET_V12.BF.LDO_V12_RAMP_RATE = rampConfig->v12ramp;

}

/*@} end of group PMU_Public_Functions */
/*@} end of group PMU_definitions */
/*@} end of group MC200_Periph_Driver */


void __attribute__ ((interrupt)) ExtPin0_IRQHandler()
{
	/* counter upp interrupt */
	if(intCbfArra[INT_EXTPIN0][0] != NULL) {
		/* call the callback function */
		intCbfArra[INT_EXTPIN0][0]();
	}
	PMU_ClearWakeupSrc(PMU_GPIO25_INT);
	NVIC_ClearPendingIRQ(ExtPin0_IRQn);
}

void __attribute__ ((interrupt)) ExtPin1_IRQHandler()
{
	/* counter upp interrupt */
	if(intCbfArra[INT_EXTPIN1][0] != NULL) {
		/* call the callback function */
		intCbfArra[INT_EXTPIN1][0]();
	}
	PMU_ClearWakeupSrc(PMU_GPIO26_INT);
	NVIC_ClearPendingIRQ(ExtPin1_IRQn);
}
