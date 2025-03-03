/****************************************************************************//**
 * @file     mw300_flashc.c
 * @brief    This file provides FLASHC functions.
 * @version  V1.0.0
 * @date     16-Apr-2013
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
#include "mw300_driver.h"
#include "mw300_flashc.h"

/** @addtogroup  88MW300_Periph_Driver
 *  @{
 */

/** @defgroup FLASHC FLASHC
 *  @brief FLASHC driver modules
 *  @{
 */

/** @defgroup FLASHC_Private_Type
 *  @{
 */

/*@} end of group FLASHC_Private_Type*/

/** @defgroup FLASHC_Private_Defines
 *  @{
 */

/*@} end of group FLASHC_Private_Defines */

/** @defgroup FLASHC_Private_Variables
 *  @{
 */

/*@} end of group FLASHC_Private_Variables */

/** @defgroup FLASHC_Global_Variables
 *  @{
 */

/*@} end of group FLASHC_Global_Variables */

/** @defgroup FLASHC_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group FLASHC_Private_FunctionDeclaration */

/** @defgroup FLASHC_Private_Functions
 *  @{
 */

/*@} end of group FLASHC_Private_Functions */

/** @defgroup FLASHC_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief     Flush the Cache
 *
 * @param[in]  none
 *
 * @return 
 *         - ERROR
 *         - SUCCESS
 *
 *******************************************************************************/
Status FLASHC_FlushCache(void)
{
  volatile uint32_t cnt = 0;
  
  FLASHC->FCCR.BF.CACHE_LINE_FLUSH = 1;
  
  while(cnt < 0x200000)
  {
    if(FLASHC->FCCR.BF.CACHE_LINE_FLUSH == 0)
    {
      return SUCCESS;
    }
    
    cnt++;
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief     Enable Flash Cache Counters
 *
 * @param[in]  none
 *
 * @return  none
 *
 *******************************************************************************/
void FLASHC_CacheCntEn(void)
{
	FLASHC->FCACR.BF.HIT_CNT_EN = ENABLE;
	FLASHC->FCACR.BF.MISS_CNT_EN = ENABLE;
}

/****************************************************************************//**
 * @brief     Get Flash Controller Cache Counters
 *
 * @param[out]  HitCnt Cache Hit counts
 * @param[out]  MissCnt Cache Miss counts
 *
 * @return
 *         - ERROR
 *         - SUCCESS
 *
 *******************************************************************************/
Status FLASHC_GetCacheCnt(uint32_t *HitCnt, uint32_t *MissCnt)
{
	if (!HitCnt || !MissCnt)
		return ERROR;

	*HitCnt = FLASHC->FCHCR.WORDVAL;
	*MissCnt = FLASHC->FCMCR.WORDVAL;
	return SUCCESS;
}

/****************************************************************************//**
 * @brief     Reset Flash Controller Cache Counters
 *
 * @param[in]  none
 *
 * @return  none
 *
 *******************************************************************************/
void FLASHC_ResetCacheCnt(void)
{
	FLASHC->FCMCR.WORDVAL = 0x0;
	FLASHC->FCHCR.WORDVAL = 0x0;
}

/****************************************************************************//**
 * @brief     Enable/Disable the cache mode
 *
 * @param[in]  state: enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void FLASHC_CacheModeEn(FunctionalState state)
{
  FLASHC->FCCR.BF.CACHE_EN = state;
}

/****************************************************************************//**
 * @brief     Enable/Disable the SRAM mode
 *
 * @param[in]  state: enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void FLASHC_SramModeEn(FunctionalState state)
{
  FLASHC->FCCR.BF.SRAM_MODE_EN = state;
}

/****************************************************************************//**
 * @brief     Configure the timing of the FLASHC
 *
 * @param[in]  config: Point to a configuration variable
 *
 * @return none
 *
 *******************************************************************************/
void FLASHC_TimingCfg(FLASHC_Timing_CFG_Type *config)
{
  /* Set FLASHC clock mode */
  FLASHC->FCCR.BF.CLK_PHA = (config->clkMode) & 1;
  FLASHC->FCCR.BF.CLK_POL = ((config->clkMode)>>1) & 1;
  
  /* Set FLASHC clock prescaler */
  FLASHC->FCCR.BF.CLK_PRESCALE = config->preScaler;
  
  /* Set FLASHC capture clock edge */
  FLASHC->FCTR.BF.CLK_CAPT_EDGE = config->captEdge;
  
  /* set clock out delay */
  FLASHC->FCTR.BF.CLK_OUT_DLY = config->clkOutDly;
  
  /* set clock in delay */
  FLASHC->FCTR.BF.CLK_IN_DLY = config->clkInDly;
  
  /* set data in delay */
  FLASHC->FCTR.BF.DIN_DLY = config->dinDly;
}

/****************************************************************************//**
 * @brief     Software configuration for FLASHC, it override the HW configuration
 *            (FCCR.CMD_TYPE) if it is enabled
 *
 * @param[in]  config: Point to a configuration variable
 *
 * @return none
 *
 *******************************************************************************/
void FLASHC_SWCfg(FLASHC_SW_CFG_Type *config)
{
  /* Set data pin number */
  FLASHC->FCCR2.BF.DATA_PIN = config->dataPinMode;
  
  /* Set address pin number */
  FLASHC->FCCR2.BF.ADDR_PIN = config->addrPinMode;
  
  /* Set data length mode */
  FLASHC->FCCR2.BF.BYTE_LEN = config->byteLen;
  
  /* Set dummy count */
  FLASHC->FCCR2.BF.DUMMY_CNT = config->dummyCnt;
  
  /* Set read mode count */
  FLASHC->FCCR2.BF.RM_CNT = config->rmCnt;
  
  /* Set read mode  */
  FLASHC->FRMR.BF.RDMODE = config->readMode;
  
  /* Set address count */
  FLASHC->FCCR2.BF.ADDR_CNT = config->addrCnt;
  
  /* Set instruction count */
  FLASHC->FCCR2.BF.INSTR_CNT = config->instrCnt;
  
  /* Set instruction */
  FLASHC->FINSTR.BF.INSTR = config->instrucion;
}

/****************************************************************************//**
 * @brief     Hardware command configuration for FLASHC
 *
 * @param[in]  config: command type
 *
 * @return none
 *
 *******************************************************************************/
void FLASHC_HWCfg(FLASHC_HW_CFG_Type config)
{
  FLASHC->FCCR.BF.CMD_TYPE = config;  
}

/****************************************************************************//**
 * @brief     selection hardware or software configuration for FLASHC
 *
 * @param[in]  cfgSel:HW/SW selection
 *
 * @return none
 *
 *******************************************************************************/
void FLASHC_CfgSelection(FLASHC_CFG_Selection_Type cfgSel)
{
  FLASHC->FCCR2.BF.USE_CFG_OVRD = cfgSel;
}

/****************************************************************************//**
 * @brief     do not use offset address for flash memory access
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void FLASHC_OffsetAddrDisable(void)
{
  FLASHC->FCACR.BF.OFFSET_EN = 0;
}

/****************************************************************************//**
 * @brief     Use offset address for flash memory access and set the offset address
 *
 * @param[in] offsetAddr: the offset address
 *
 * @return none
 *
 *******************************************************************************/
void FLASHC_OffsetAddrEnable(uint32_t offsetAddr)
{
  FLASHC->FCACR.BF.OFFSET_EN = 1;
  
  FLASHC->FAOFFR.WORDVAL = offsetAddr;
}

/****************************************************************************//**
 * @brief     Read exit continuous read mode status
 *
 * @param[in]  none
 *
 * @return 
 *           - RESET (0)
 *           - SET(1)
 *
 *******************************************************************************/
FlagStatus FLASHC_ReadExitContReadStat(void)
{
  return((FLASHC->FCSR.BF.CONT_RD_MD_EXIT_DONE) ? SET:RESET);
}

/****************************************************************************//**
 * @brief     Exit dual continuous read mode status
 *
 * @param[in]  none
 *
 * @return 
 *           - ERROR (0)
 *           - SUCCESS(1)
 *
 *******************************************************************************/
Status FLASHC_ExitDualContReadStat(void)
{
  volatile uint32_t cnt = 0;
  uint32_t cacheMode;
  
  cacheMode = FLASHC->FCCR.BF.CACHE_EN;
  FLASHC->FCCR.BF.CACHE_EN = 0;
  
  /* clear the status bit */
  FLASHC->FCSR.BF.CONT_RD_MD_EXIT_DONE = 1;
  
  FLASHC->FCCR.BF.CMD_TYPE = 0x0c;
    
  while(cnt < 0x200000)
  {
    if(FLASHC->FCSR.BF.CONT_RD_MD_EXIT_DONE == 1)
    {
      FLASHC->FCCR.BF.CACHE_EN = cacheMode;
      return SUCCESS;
    }
    
    cnt++;
  }
  
  FLASHC->FCCR.BF.CACHE_EN = cacheMode;
  return ERROR;
}

/****************************************************************************//**
 * @brief     Exit quad continuous read mode status
 *
 * @param[in]  none
 *
 * @return 
 *           - ERROR (0)
 *           - SUCCESS(1)
 *
 *******************************************************************************/
Status FLASHC_ExitQuadContReadStat(void)
{
  volatile uint32_t cnt = 0;
  uint32_t cacheMode;
  
  cacheMode = FLASHC->FCCR.BF.CACHE_EN;
  FLASHC->FCCR.BF.CACHE_EN = 0;
  
  /* clear the status bit */
  FLASHC->FCSR.BF.CONT_RD_MD_EXIT_DONE = 1;
  
  FLASHC->FCCR.BF.CMD_TYPE = 0x0d;
    
  while(cnt < 0x200000)
  {
    if(FLASHC->FCSR.BF.CONT_RD_MD_EXIT_DONE == 1)
    {
      FLASHC->FCCR.BF.CACHE_EN = cacheMode;
      return SUCCESS;
    }
    
    cnt++;
  }
  
  FLASHC->FCCR.BF.CACHE_EN = cacheMode;
  return ERROR;
}

__attribute__((section(".ram")))
int FLASH_SetCmdType_QuadModeRead(uint32_t jedecID)
{
	switch (jedecID) {
	case 0xef4014:
#ifndef CONFIG_VARIANT_MW310
	case 0xef4015:
#endif	
	case 0xef4016:
	case 0xef4017:
	case 0xef4018:
	case 0xc84016:
	case 0xc84018:
  case 0xa14015:
		FLASHC->FCCR.BF.CMD_TYPE = FLASHC_HW_CMD_FRQIOC;
	break;
#ifdef CONFIG_VARIANT_MW310
	case 0xef4015:
#endif
	case 0xc84015:
	case 0xc22014:
	case 0xc22315:
		FLASHC->FCCR.BF.CMD_TYPE = FLASHC_HW_CMD_FRQIO;
	break;
	default:
		return ERROR;
	}

	return SUCCESS;
}

/*@} end of group FLASHC_Public_Functions */

/*@} end of group FLASHC  */

/*@} end of group 88MW300_Periph_Driver */
