/****************************************************************************//**
 * @file     mc200_sdio.c
 * @brief    This file provides SDIO functions.
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

#include "mc200_sdio.h"
#include "mc200.h"
#include "mc200_driver.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @defgroup SDIO SDIO
 *  @brief SDIO driver module
 *  @{
 */

/** @defgroup SDIO_Private_Type
 *  @{
 */

/*@} end of group SDIO_Private_Type*/


/** @defgroup SDIO_Private_Defines
 *  @{
 */

/**
  * @brief  Separate command and transfer mode control for host
  */
#define SDIOC_TRANS_MODE                 (*(volatile uint16_t *)(SDIO_BASE+ 0x0C))
#define SDIOC_CMD                        (*(volatile uint16_t *)(SDIO_BASE+ 0x0E))

/*@} end of group SDIO_Private_Defines */


/** @defgroup SDIO_Private_Variables
 *  @{
 */

/*@} end of group SDIO_Private_Variables */


/** @defgroup SDIO_Global_Variables
 *  @{
 */

/*@} end of group SDIO_Global_Variables */


/** @defgroup SDIO_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group SDIO_Private_FunctionDeclaration */

/** @defgroup SDIO_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Initializes SDIO module
 *
 * @param[in]  sdioCfgStruct:  Pointer to a SDIO configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_Init(SDIOC_CFG_Type* sdioCfgStruct)
{
  /* Enable/ Disable Clock */
  SDIO->CNTL2.BF.CLKEN = sdioCfgStruct->clkEnable;

  /* Enable internal clock */
  SDIO->CNTL2.BF.INTCLKEN = 0x1;

  /* Set the data width */
  SDIO->CNTL1.BF._4BITMD = sdioCfgStruct->busWidth;

  /* Set the speed mode */
  SDIO->CNTL1.BF.HISPEED = sdioCfgStruct->speedMode;

  /* Set the bus voltage */
  SDIO->CNTL1.BF.VLTGSEL = sdioCfgStruct->busVoltSel;

  /* Enable/ Disable read wait control */
  SDIO->CNTL1.BF.RDWTCNTL = sdioCfgStruct->readWaitCtrl;

  /* Enable/ Disable interrupt at blosk gap */
  SDIO->CNTL1.BF.BGIRQEN = sdioCfgStruct->blkGapInterrupt;
}

/****************************************************************************//**
 * @brief      Send SDIO command
 *
 * @param[in]  cmd  : Pointer to a SDIO CMD configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_SendCmd(SDIOC_CmdCfg_Type* cmd)
{
  uint16_t regVal = 0;

  regVal |= cmd->resp;
  regVal |= (cmd->cmdCrcCheck << 3);
  regVal |= (cmd->cmdIndexCheck << 4);
  regVal |= (cmd->dataPreSel << 5);
  regVal |= (cmd->cmdType << 6);
  regVal |= (cmd->cmdIndex << 8);

  /* Set the cmd argument */
  SDIO->ARG.BF.ARG = cmd->arg;

  /* Fill upper two bytes of CMD_XFRMD register */
  SDIOC_CMD = regVal;
}

/****************************************************************************//**
 * @brief      Set SDIO transfer mode
 *
 * @param[in]  transfMode:  Pointer to a SDIO transfer type structure
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_SetTransferMode(SDIOC_Transfer_Type* transfMode)
{
  uint16_t regVal = 0;

  regVal |= transfMode->dmaEnabe;
  regVal |= (transfMode->blkCntEnable << 1);
  regVal |= (transfMode->transferDir << 4);
  regVal |= (transfMode->blkSel << 5);

  /* Fill lower two bytes of CMD_XFRMD register */
  SDIOC_TRANS_MODE = regVal;

  /* Set the block count */
  SDIO->BLK_CNTL.BF.BLK_CNT = transfMode->blkCnt;

  /* Set the block size */
  SDIO->BLK_CNTL.BF.XFR_BLKSZ = transfMode->blkSize;

  /* Set the data timeout counter value */
  SDIO->CNTL2.BF.DTOCNTR = transfMode->dataTimeoutCnt;
}

/****************************************************************************//**
 * @brief      Get SDIO command response
 *
 * @param[in]  rpx:  SDIO response number
 *
 * @return SDIO response register
 *
 *******************************************************************************/
uint32_t SDIOC_GetResponse(SDIOC_ResponseNum_Type rpx)
{
  /* Get SDIO response */
  return SDIO->RESP[rpx].WORDVAL;
}

/****************************************************************************//**
 * @brief      Config SDIO DMA
 *
 * @param[in]  dmaCfg:  Pointer to a SDIO DMA configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_DmaConfig(SDIOC_DmaCfg_Type* dmaCfg)
{
  /* Set host DMA buffer size */
  SDIO->BLK_CNTL.BF.DMA_BUFSZ = dmaCfg->dmaBufSize;

  /* Set DMA system address */
  SDIO->SYSADDR.WORDVAL = dmaCfg->dmaSystemAddr;
}

/****************************************************************************//**
 * @brief      Read data from SDIO buffer
 *
 * @param  none
 *
 * @return SDIO buffer data
 *
 *******************************************************************************/
uint32_t SDIOC_ReadData(void)
{
  /* Read SDIO buffer data */
  return SDIO->DP.WORDVAL;
}

/****************************************************************************//**
 * @brief      Write data to SDIO buffer
 *
 * @param[in]  data: transfer data
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_WriteData(uint32_t data)
{
  /* Write data to SDIO buffer */
  SDIO->DP.WORDVAL = data;
}

/****************************************************************************//**
 * @brief      Update the DMA system address
 *
 * @param[in]  addr: DMA system address
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_UpdateSysAddr(uint32_t addr)
{
  /* Update the DMA system address */
  SDIO->SYSADDR.WORDVAL = addr;
}

/****************************************************************************//**
 * @brief      Get the host DMA buffer size
 *
 * @param  none
 *
 * @return DMA buffer size
 *
 *******************************************************************************/
uint32_t SDIOC_GetDmaBufsz(void)
{
  /* Get the host DMA buffer size */
  return (uint32_t)(1 << (SDIO->BLK_CNTL.BF.DMA_BUFSZ + 12));
}

/****************************************************************************//**
 * @brief      Bus power on
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_PowerOn(void)
{
  /* Bus power on */
  SDIO->CNTL1.BF.BUSPWR = 1;
}

/****************************************************************************//**
 * @brief      Bus power off
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_PowerOff(void)
{
  /* Bus power off */
  SDIO->CNTL1.BF.BUSPWR = 0;
}

/****************************************************************************//**
 * @brief      Reset SDIO module
 *
 * @param  none
 *
 * @return Staus
 *
 *******************************************************************************/
Status SDIOC_Reset(void)
{
  volatile uint32_t timeout = 100;

  /* Software reset for all */
  SDIO->CNTL2.BF.MSWRST = 1;

  /* Assert the software reset complete */
  while(timeout--)
  {
    if(SDIO->CNTL2.BF.MSWRST == 0)
      return DSUCCESS;
  }

  return DERROR;
}

/****************************************************************************//**
 * @brief      Reset SDIO data lines
 *
 * @param  none
 *
 * @return Status
 *
 *******************************************************************************/
Status SDIOC_DataxLineReset(void)
{
  volatile uint32_t timeout = 100;

  /* Softwate reset data lines */
  SDIO->CNTL2.BF.DATSWRST = 1;

  /* Assert the software reset complete */
  while(timeout--)
  {
    if(SDIO->CNTL2.BF.DATSWRST == 0)
      return DSUCCESS;
  }

  return DERROR;
}

/****************************************************************************//**
 * @brief      Reset SDIO command line
 *
 * @param  none
 *
 * @return Status
 *
 *******************************************************************************/
Status SDIOC_CmdLineReset(void)
{
  volatile uint32_t timeout = 100;

  /* Software reset command line */
  SDIO->CNTL2.BF.CMDSWRST = 1;

  /* Assert the software reset complete */
  while(timeout--)
  {
    if(SDIO->CNTL2.BF.CMDSWRST == 0)
      return DSUCCESS;
  }

  return DERROR;
}

/****************************************************************************//**
 * @brief      Control LED on or off
 *
 * @param[in]  ledCtrl: led control
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_LedCtrl(SDIOC_LED_Type ledCtrl)
{
  /* Led control */
  SDIO->CNTL1.WORDVAL = ledCtrl;
}

/****************************************************************************//**
 * @brief      Get SDIO status
 *
 * @param[in]  statusType:  Status type
 *
 * @return SDIO status
 *
 *******************************************************************************/
FlagStatus SDIOC_GetStatus(SDIOC_STATUS_Type statusType)
{
 return (SDIO->STATE.WORDVAL & (1 << statusType)) ? SET : RESET;
}

/****************************************************************************//**
 * @brief      Get SDIO capabilities register value
 *
 * @param  none
 *
 * @return SDIO capabilities register value
 *
 *******************************************************************************/
uint32_t SDIOC_GetCapabilities(void)
{
  /* Get capabilities register value */
  return SDIO->CAP0.WORDVAL;
}

/****************************************************************************//**
 * @brief      Get SDIO interrupt status
 *
 * @param[in]  intType:  Interrupt type
 *
 * @return SDIO interrupt status
 *
 *******************************************************************************/
FlagStatus SDIOC_GetIntStatus(SDIOC_INT_Type intType)
{
    int status = SDIO->I_STAT.WORDVAL;

    if (SDIOC_INT_ALL == intType)
        status &= 0x10FF81FF;
    else
        status &= (1 << intType);

  return status ? SET : RESET;
}

/****************************************************************************//**
 * @brief      Mask/ Unmask SDIO interrupt status
 *
 * @param[in]  intType  : Interrupt type
 * @param[in]  maskState: Mask / Unmask control
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_IntMask(SDIOC_INT_Type intType, IntMask_Type maskState)
{
    int status = SDIO->I_STAT_EN.WORDVAL;

    if (SDIOC_INT_ALL == intType) {
        if (UNMASK == maskState)
            status |= 0x10FF01FF;
        else
            status = 0;
    }
    else {
        if (UNMASK == maskState)
            status |= (1 << intType);
        else
            status &= ~(1 << intType);
    }

    SDIO->I_STAT_EN.WORDVAL = status;
}

/****************************************************************************//**
 * @brief      Mask/ Unmask SDIO interrupt signal
 *
 * @param[in]  intType  : Interrupt type
 * @param[in]  maskState: Mask / Unmask control
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_IntSigMask(SDIOC_INT_Type intType, IntMask_Type maskState)
{
    int status = SDIO->I_SIG_EN.WORDVAL;

    if (SDIOC_INT_ALL == intType) {
        if (UNMASK == maskState)
            status |= 0x10FF01FF;
        else
            status = 0;
    }
    else {
        if (UNMASK == maskState)
            status |= (1 << intType);
        else
            status &= ~(1 << intType);
    }

    SDIO->I_SIG_EN.WORDVAL = status;
}

/****************************************************************************//**
 * @brief      Clear SDIO interrupt status
 *
 * @param[in]  intType  : Interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_IntClr(SDIOC_INT_Type intType)
{
	SDIO->I_STAT.WORDVAL |= (SDIOC_INT_ALL == intType) ? 0x10FF00FF : (1 << intType);
}
/*@} end of group SDIO_Public_Functions */

/*@} end of group SDIO_definitions */

/*@} end of group MC200_Periph_Driver */
