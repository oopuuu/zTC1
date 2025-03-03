/****************************************************************************//**
 * @file     mc200_dma.c
 * @brief    This file provides DMA functions.
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
#include "mc200_driver.h"
#include "mc200_dma.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @defgroup DMA DMA
 *  @brief DMA driver modules
 *  @{
 */

/** @defgroup DMA_Private_Type
 *  @{
 */

/*@} end of group DMA_Private_Type*/

/** @defgroup DMA_Private_Defines
 *  @{
 */

/*@} end of group DMA_Private_Defines */

/** @defgroup DMA_Private_Variables
 *  @{
 */

/**
 * @brief interrupt channel array
 */
static const uint32_t intChannelArra[] = {INT_DMA_CH0, INT_DMA_CH1, INT_DMA_CH2, INT_DMA_CH3, INT_DMA_CH4, INT_DMA_CH5, INT_DMA_CH6, INT_DMA_CH7};

/*@} end of group DMA_Private_Variables */

/** @defgroup DMA_Global_Variables
 *  @{
 */

/*@} end of group DMA_Global_Variables */

/** @defgroup DMA_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group DMA_Private_FunctionDeclaration */

/** @defgroup DMA_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      DMA interrupt handler 
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void DMA_IRQHandler()
{
  uint32_t intChannel;
  uint64_t intStatusTfr, intStatusBlock, intStatusSrcTran, intStatusDstTran, intStatusErr;
      
  /* Get current unmasked interrupt status */
  intStatusTfr = DMA->STATUSTFR.WORDVAL;
  intStatusBlock = DMA->STATUSBLOCK.WORDVAL;
  intStatusSrcTran = DMA->STATUSSRCTRAN.WORDVAL;
  intStatusDstTran = DMA->STATUSDSTTRAN.WORDVAL;
  intStatusErr = DMA->STATUSERR.WORDVAL;
  
  /* Clear the unmasked generated interrupts */
  DMA->CLEARTFR.WORDVAL = intStatusTfr;
  DMA->CLEARBLOCK.WORDVAL = intStatusBlock;
  DMA->CLEARSRCTRAN.WORDVAL = intStatusSrcTran;
  DMA->CLEARDSTTRAN.WORDVAL = intStatusDstTran;
  DMA->CLEARERR.WORDVAL = intStatusErr;

  if(intStatusTfr)
  {
    /* Check interrupt channel */
    for(intChannel = 0; intChannel < 8; intChannel++)
    {
      if(intStatusTfr & (0x01 << intChannel))
      {
        if(intCbfArra[intChannelArra[intChannel]][INT_DMA_TRANS_COMPLETE] != NULL)
        {
          intCbfArra[intChannelArra[intChannel]][INT_DMA_TRANS_COMPLETE]();
        }
        /* Mask interrupt */
        else
        {
          DMA->MASKTFR.WORDVAL = (1 << (intChannel + 8));
        }
      }
    }
  }

  if(intStatusBlock)
  {
    /* Check interrupt channel */
    for(intChannel = 0; intChannel < 8; intChannel++)
    {
      if(intStatusBlock & (0x01 << intChannel))
      {
        if(intCbfArra[intChannelArra[intChannel]][INT_BLK_TRANS_COMPLETE] != NULL)
        {
          intCbfArra[intChannelArra[intChannel]][INT_BLK_TRANS_COMPLETE]();
        }
        /* Mask interrupt */
        else
        {
          DMA->MASKBLOCK.WORDVAL = (1 << (intChannel + 8));
        }
      }
    }
  }

  if(intStatusSrcTran)
  {
    /* Check interrupt channel */
    for(intChannel = 0; intChannel < 8; intChannel++)
    {
      if(intStatusSrcTran & (0x01 << intChannel))
      {
        if(intCbfArra[intChannelArra[intChannel]][INT_SRC_TRANS_COMPLETE] != NULL)
       {
         intCbfArra[intChannelArra[intChannel]][INT_SRC_TRANS_COMPLETE]();
       }
       /* Mask interrupt */
       else
       {
         DMA->MASKSRCTRAN.WORDVAL = (1 << (intChannel + 8));
       }
      }
    }
  }

  if(intStatusDstTran)
  {
    /* Check interrupt channel */
    for(intChannel = 0; intChannel < 8; intChannel++)
    {
      if(intStatusDstTran & (0x01 << intChannel))
      {
        if(intCbfArra[intChannelArra[intChannel]][INT_DEST_TRANS_COMPLETE] != NULL)
        {
          intCbfArra[intChannelArra[intChannel]][INT_DEST_TRANS_COMPLETE]();
        }
        /* Mask interrupt */
        else
        {
          DMA->MASKDSTTRAN.WORDVAL = (1 << (intChannel + 8));
        }
      }
    }
  }
  
  if(intStatusErr)
  {
    /* Check interrupt channel */
    for(intChannel = 0; intChannel < 8; intChannel++)
    {
      if(intStatusErr & (0x01 << intChannel))
      {
        if(intCbfArra[intChannelArra[intChannel]][INT_ERROR] != NULL)
        {
          intCbfArra[intChannelArra[intChannel]][INT_ERROR]();
        }
        /* Mask interrupt */
        else
        {
          DMA->MASKERR.WORDVAL = (1 << (intChannel + 8));
        }
      }
    }
  }
  
}
/*@} end of group DMA_Private_Functions */

/** @defgroup DMA_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      Initializes the DMA channel
 *
 * @param[in]  channelx:  Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5 
 * @param[in]  dmaCfgStruct:  Pointer to a DMA configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void DMA_ChannelInit(DMA_Channel_Type channelx, DMA_CFG_Type * dmaCfgStruct)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));

  /* Config the source address of DMA transfer */
  DMA->CHANNEL[channelx].SAR.BF.SAR = dmaCfgStruct->srcDmaAddr;

  /* Config the destination address of DMA transfer */
  DMA->CHANNEL[channelx].DAR.BF.DAR = dmaCfgStruct->destDmaAddr;
  
  /* Transfer type and flow control */
  DMA->CHANNEL[channelx].CTL.BF.TT_FC = dmaCfgStruct->transfType;

  /* Set source burst transaction length */
  DMA->CHANNEL[channelx].CTL.BF.SRC_MSIZE = dmaCfgStruct->srcBurstLength;

  /* Set destination burst transaction length */
  DMA->CHANNEL[channelx].CTL.BF.DEST_MSIZE = dmaCfgStruct->destBurstLength;

  /* Set Source address increment mode */
  DMA->CHANNEL[channelx].CTL.BF.SINC = dmaCfgStruct->srcAddrInc;

  /* Destination address increment */
  DMA->CHANNEL[channelx].CTL.BF.DINC = dmaCfgStruct->destAddrInc;

  /* Source transfer width */
  DMA->CHANNEL[channelx].CTL.BF.SRC_TR_WIDTH = dmaCfgStruct->srcTransfWidth;

  /* Destination transfer width */
  DMA->CHANNEL[channelx].CTL.BF.DST_TR_WIDTH = dmaCfgStruct->destTransfWidth;

  /* Source software or hardware handshaking select */
  DMA->CHANNEL[channelx].CFG.BF.HS_SEL_SRC = dmaCfgStruct->srcSwHwHdskSel;

  /* Destination software or hardware handshaking select */
  DMA->CHANNEL[channelx].CFG.BF.HS_SEL_DST = dmaCfgStruct->destSwHwHdskSel;

  /* Channel priority */
  DMA->CHANNEL[channelx].CFG.BF.CH_PRIOR = dmaCfgStruct->channelPriority;

  /* Assigns a hardware handshaking interface(0-5) to the source channelx 
   * if source hardware handshaking select */
  DMA->CHANNEL[channelx].CFG.BF.SRC_PER = dmaCfgStruct->srcHwHdskInterf;

  /* Assigns a hardware handshaking interface(0-5) to the destination of 
   * channelx if destination hardware handshaking select.*/
  DMA->CHANNEL[channelx].CFG.BF.DEST_PER = dmaCfgStruct->destHwHdskInterf; 

  /* FIFO mode select */
  DMA->CHANNEL[channelx].CFG.BF.FIFO_MODE = dmaCfgStruct->fifoMode;
}

/****************************************************************************//**
 * @brief      Set DMA handshaking mapping 
 *
 * @param[in]  hsMapping: handshaking mapping type 
 *
 * @return none
 *
 *******************************************************************************/
void DMA_SetHandshakingMapping(DMA_HsMapping_Type hsMapping)
{
  uint32_t hsx, hsxVal;

  hsx = ((hsMapping & 0xF0) >> 4);
  hsxVal = (hsMapping & 0x0F);

  switch(hsx)
  {
    case 0:
      SYS_CTRL->DMA_HS.BF.MAPPING_0 = hsxVal;
      break;

    case 1:
      SYS_CTRL->DMA_HS.BF.MAPPING_1 = hsxVal;
      break;

    case 2:
      SYS_CTRL->DMA_HS.BF.MAPPING_2 = hsxVal;
      break;

    case 3:
      SYS_CTRL->DMA_HS.BF.MAPPING_3 = hsxVal;
      break;

    case 4:
      SYS_CTRL->DMA_HS.BF.MAPPING_4 = hsxVal;
      break;

    case 5:
      SYS_CTRL->DMA_HS.BF.MAPPING_5 = hsxVal;
      break;
      
    case 6:
      SYS_CTRL->DMA_HS.BF.MAPPING_6 = hsxVal;
      break;

    case 7:
      SYS_CTRL->DMA_HS.BF.MAPPING_7 = hsxVal;
      break;

    case 8:
      SYS_CTRL->DMA_HS.BF.MAPPING_8 = hsxVal;
      break;

    case 9:
      SYS_CTRL->DMA_HS.BF.MAPPING_9 = hsxVal;
      break;

    case 10:
      SYS_CTRL->DMA_HS.BF.MAPPING_10 = hsxVal;
      break;

    case 11:
      SYS_CTRL->DMA_HS.BF.MAPPING_11 = hsxVal;
      break;
    case 12:
      SYS_CTRL->DMA_HS.BF.MAPPING_12 = hsxVal;
      break;

    case 13:
      SYS_CTRL->DMA_HS.BF.MAPPING_13 = hsxVal;
      break;

    case 14:
      SYS_CTRL->DMA_HS.BF.MAPPING_14 = hsxVal;
      break;

    case 15:
      SYS_CTRL->DMA_HS.BF.MAPPING_15 = hsxVal;
      break;    

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Enable the DMA 
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void DMA_Enable(void)
{
  /* Enable/Disable DMA */
  DMA->DMACFGREG.BF.DMA_EN = 1;
}

/****************************************************************************//**
 * @brief      Disable the DMA 
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void DMA_Disable(void)
{
  /* Enable/Disable DMA */
  DMA->DMACFGREG.BF.DMA_EN = 0;
}

/****************************************************************************//**
 * @brief      Enable or disable the DMA channel
 *
 * @param[in]  channelx:  Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5 
 * @param[in]  NewState: Enable/Disable function state 
 *
 * @return none
 *
 *******************************************************************************/
void DMA_ChannelCmd(DMA_Channel_Type channelx, FunctionalState newState)
{
  uint64_t enableCh, disableCh;
  
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(newState));

  enableCh = (1 << channelx) | (1 << (channelx + 8));

  disableCh = 1 << (channelx + 8);

  /* Enable/Disable the channel */
  if(ENABLE == newState)
  {
    DMA->CHENREG.WORDVAL = enableCh;
  }
  else
  {
    DMA->CHENREG.WORDVAL = disableCh;
  }
}

/****************************************************************************//**
 * @brief      Set DMA transfer data length
 *
 * @param[in]  channelx:  Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5 
 * @param[in]  length: data length to be transfer in byte. length should be 
 *             multiple of transfer width.
 *
 * @return none
 *
 * Note: This function can be called after DMA has finished its initialization.  
 *
 *******************************************************************************/
void DMA_SetTransfDataLength(DMA_Channel_Type channelx, uint32_t length)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));

  DMA->CHANNEL[channelx].CTL.BF.BLOCK_TS = length/(0x01 << DMA->CHANNEL[channelx].CTL.BF.SRC_TR_WIDTH);
}

/****************************************************************************//**
 * @brief      Set DMA block transfer size
 *
 * @param[in]  channelx:  Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5 
 * @param[in]  blkSize: The total number of single transactions. The value should be  
 *             less than 1023
 *
 * @return none
 *
 * Note: This function can be called after DMA has finished its initialization.  
 *
 *******************************************************************************/
void DMA_SetBlkTransfSize(DMA_Channel_Type channelx, uint32_t blkSize)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(IS_DMA_BLK_SIZE(blkSize));
  
  DMA->CHANNEL[channelx].CTL.BF.BLOCK_TS = blkSize;
}
/****************************************************************************//**
 * @brief      Set software transaction request
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5 
 * @param[in]  dir: Source/Destination request, should be DMA_SOURCE or DMA_DESTINATION 
 * @param[in]  transMode: Single/burst transaction mode, should be DMA_SINGLE or DMA_BURST  
 *
 * @return none
 *
 *******************************************************************************/
void DMA_SoftwareTransReq(DMA_Channel_Type channelx, DMA_TransDir_Type dir,
                          DMA_TransMode_Type transMode)
{
  uint64_t writeVal;
  
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(IS_DMA_TRANS_DIR(dir));
  CHECK_PARAM(IS_DMA_TRANS_MODE(transMode));

  writeVal = (1 << channelx) | (1 << (channelx + 8));

  if((dir == DMA_SOURCE) && (transMode == DMA_BURST))
  {
    DMA->REQSRCREG.WORDVAL = writeVal;
    DMA->SGLREQSRCREG.WORDVAL = writeVal; 
  }
  else if((dir == DMA_DESTINATION) && (transMode == DMA_BURST))
  {   
    DMA->REQDSTREG.WORDVAL = writeVal; 
    DMA->SGLREQDSTREG.WORDVAL = writeVal; 
  }
  else if((dir == DMA_SOURCE) && (transMode == DMA_SINGLE))
  {   
    DMA->SGLREQSRCREG.WORDVAL = writeVal; 
  }
  else
  {
    DMA->SGLREQDSTREG.WORDVAL = writeVal; 
  }
}

/****************************************************************************//**
 * @brief      Set software transaction request
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5 
 * @param[in]  NewState: Enable/Disable function state 
 *
 * @return none
 *
 *******************************************************************************/
void DMA_ChannelSuspend(DMA_Channel_Type channelx, FunctionalState newState)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(newState));

  DMA->CHANNEL[channelx].CFG.BF.CH_SUSP = ((newState == ENABLE)? 1 : 0);
}

/****************************************************************************//**
 * @brief      Enable/Disable channel interrupt 
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5 
 * @param[in]  newState:  Enable/Disable Specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void DMA_ChannelIntConfig(DMA_Channel_Type channelx, FunctionalState newState)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(newState));

  DMA->CHANNEL[channelx].CTL.BF.INT_EN = ((ENABLE == newState)? 1 : 0);
}

/****************************************************************************//**
 * @brief      Get current DMA channel interrupt Status for given interrupt type 
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5 
 * @param[in]  intType: specified interrupt, should be 
 *             - INT_DMA_TRANS_COMPLETE
 *             - INT_BLK_TRANS_COMPLETE
 *             - INT_SRC_TRANS_COMPLETE
 *             - INT_DEST_TRANS_COMPLETE
 *             - INT_ERROR
 *
 * @return The interrupt state value: RESET or SET
 *
 *******************************************************************************/
FlagStatus DMA_GetChannelIntStatus(DMA_Channel_Type channelx, DMA_Int_Type intType)
{
  FlagStatus intBitStatus = RESET;

  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(IS_DMA_INT(intType));
  
  switch(intType)
  {
    case INT_DMA_TRANS_COMPLETE:
      if(DMA->STATUSTFR.BF.STATUS & (1 << channelx))
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }
      
      break;

    case INT_BLK_TRANS_COMPLETE:
      if(DMA->STATUSBLOCK.BF.STATUS & (1 << channelx))
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;
      
    case INT_SRC_TRANS_COMPLETE:
      if(DMA->STATUSSRCTRAN.BF.STATUS & (1 << channelx))
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;

    case INT_DEST_TRANS_COMPLETE:
      if(DMA->STATUSDSTTRAN.BF.STATUS & (1 << channelx))
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;

    case INT_ERROR:
      if(DMA->STATUSERR.BF.STATUS & (1 << channelx))
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;

    default:
      break;
  }

  return intBitStatus;
}

/****************************************************************************//**
 * @brief      Get current DMA interrupt Status for given interrupt type 
 *
 * @param[in]  intType: specified interrupt, should be 
 *             - INT_DMA_TRANS_COMPLETE
 *             - INT_BLK_TRANS_COMPLETE
 *             - INT_SRC_TRANS_COMPLETE
 *             - INT_DEST_TRANS_COMPLETE
 *             - INT_ERROR
 *
 * @return The interrupt state value: RESET or SET
 *
 *******************************************************************************/
FlagStatus DMA_GetIntStatus(DMA_Int_Type intType)
{
  FlagStatus intBitStatus = RESET;

  switch(intType)
  {
    case INT_DMA_TRANS_COMPLETE:     
      if(DMA->STATUSINT.BF.TFR)
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;
  
    case INT_BLK_TRANS_COMPLETE:
      if(DMA->STATUSINT.BF.BLOCK)
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;
      
    case INT_SRC_TRANS_COMPLETE:
      if(DMA->STATUSINT.BF.SRCT)
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;

    case INT_DEST_TRANS_COMPLETE:
      if(DMA->STATUSINT.BF.DSTT)
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;

    case INT_ERROR:
      if(DMA->STATUSINT.BF.ERR)
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }
      
      break;

    default:
      break;
  }

  return intBitStatus;
}
/****************************************************************************//**
 * @brief      Set DMA interrupt mask for given interrupt type 
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5
 * @param[in]  intMask: Mask/Unmask specified interrupt type 
 * @param[in]  intType: specified interrupt, should be 
 *             - INT_DMA_TRANS_COMPLETE
 *             - INT_BLK_TRANS_COMPLETE
 *             - INT_SRC_TRANS_COMPLETE
 *             - INT_DEST_TRANS_COMPLETE
 *             - INT_ERROR
 *
 * @return none
 *
 *******************************************************************************/
void DMA_IntMask(DMA_Channel_Type channelx, DMA_Int_Type intType, IntMask_Type intMask)
{
  uint64_t unmaskInt, maskInt;
  
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(IS_DMA_INT(intType));
  CHECK_PARAM(IS_INTMASK(intMask));;

  unmaskInt = (1 << channelx) | (1 << (channelx + 8));

  maskInt = 1 << (channelx + 8);
  
  switch(intType)
  {
    case INT_DMA_TRANS_COMPLETE:
      DMA->MASKTFR.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);
      break;

    case INT_BLK_TRANS_COMPLETE:
      DMA->MASKBLOCK.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);   
      break;
      
    case INT_SRC_TRANS_COMPLETE: 
      DMA->MASKSRCTRAN.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);       
      break;

    case INT_DEST_TRANS_COMPLETE:
      DMA->MASKDSTTRAN.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);
      break;

    case INT_ERROR:      
      DMA->MASKERR.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);     
      break;

	case INT_CH_ALL:
	  DMA->MASKTFR.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);
	  DMA->MASKBLOCK.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);
	  DMA->MASKSRCTRAN.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);
	  DMA->MASKDSTTRAN.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);
	  DMA->MASKERR.WORDVAL = ((MASK == intMask)? maskInt : unmaskInt);

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Get current DMA interrupt Status for given interrupt type 
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5 
 * @param[in]  intType: specified interrupt, should be 
 *             - INT_DMA_TRANS_COMPLETE
 *             - INT_BLK_TRANS_COMPLETE
 *             - INT_SRC_TRANS_COMPLETE
 *             - INT_DEST_TRANS_COMPLETE
 *             - INT_ERROR
 *             - INT_CH_ALL
 *
 * @return none
 *
 *******************************************************************************/
void DMA_IntClr(DMA_Channel_Type channelx, DMA_Int_Type intType)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(IS_DMA_INT(intType));
  
  switch(intType)
  {
    case INT_DMA_TRANS_COMPLETE:    
      DMA->CLEARTFR.WORDVAL = (1 << channelx);      
      break;

    case INT_BLK_TRANS_COMPLETE:
      DMA->CLEARBLOCK.WORDVAL = (1 << channelx);      
      break;
      
    case INT_SRC_TRANS_COMPLETE:      
      DMA->CLEARSRCTRAN.WORDVAL = (1 << channelx);      
      break;

    case INT_DEST_TRANS_COMPLETE:      
      DMA->CLEARDSTTRAN.WORDVAL = (1 << channelx);      
      break;

    case INT_ERROR:      
      DMA->CLEARERR.WORDVAL = (1 << channelx);      
      break;
      
    case INT_CH_ALL:      
      DMA->CLEARTFR.WORDVAL = (1 << channelx);
      DMA->CLEARBLOCK.WORDVAL = (1 << channelx);      
      DMA->CLEARSRCTRAN.WORDVAL = (1 << channelx);      
      DMA->CLEARDSTTRAN.WORDVAL = (1 << channelx);      
      DMA->CLEARERR.WORDVAL = (1 << channelx);      
      break;
      
    default:
      break;
  }
}
/****************************************************************************//**
 * @brief    Set/update current DMA Source Address
 *
 * @param[in] channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5
 * @param[in] srcDmaAddr: Source address of DMA source buffer
 *
 * @return none
 *
 *******************************************************************************/
void DMA_SetSourceAddr(DMA_Channel_Type channelx, uint32_t srcDmaAddr)
{
  /* Config the source address of DMA transfer */
  DMA->CHANNEL[channelx].SAR.BF.SAR = srcDmaAddr;

}

/****************************************************************************//**
* @brief    Set/update current DMA Destination Address
*
* @param[in] channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5
* @param[in] destDmaAddr: Source address of DMA source buffer
*
* @return none
*
*******************************************************************************/

void DMA_SetDestAddr(DMA_Channel_Type channelx,uint32_t destDmaAddr)
{
 /* Config the destination address of DMA transfer */
  DMA->CHANNEL[channelx].DAR.BF.DAR = destDmaAddr;

}

/****************************************************************************//**
* @brief    Get interrupt peripheral type for corresponding DMA channel
*
* @param[in] channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_5
*
* @return Interrupt peripheral type for the DMA channel.
*
*******************************************************************************/

INT_Peripher_Type DMA_Channel_Int_Type(DMA_Channel_Type channelx)
{
	CHECK_PARAM(IS_DMA_CHANNEL(channelx));
	return (INT_Peripher_Type)(channelx + INT_DMA_CH0);
}

/*@} end of group DMA_Public_Functions */

/*@} end of group DMA_definitions */

/*@} end of group MC200_Periph_Driver */
