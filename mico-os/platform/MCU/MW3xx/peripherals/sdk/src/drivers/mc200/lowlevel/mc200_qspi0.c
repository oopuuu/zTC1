/****************************************************************************//**
 * @file     mc200_qspi0.c
 * @brief    This file provides QSPI0 functions.
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
#include "mc200_qspi1.h"
#include "mc200_qspi0.h"

/** @addtogroup MC200_Periph_Driver
 *  @{
 */

/** @defgroup QSPI0 QSPI0
 *  @brief QSPI0 driver modules
 *  @{
 */

/** @defgroup QSPI0_Private_Type
 *  @{
 */

/*@} end of group QSPI0_Private_Type*/


/** @defgroup QSPI0_Private_Defines
 *  @{
 */

/*@} end of group QSPI0_Private_Defines */


/** @defgroup QSPI0_Private_Variables
 *  @{
 */

/*@} end of group QSPI0_Private_Variables */

/** @defgroup QSPI0_Global_Variables
 *  @{
 */

/*@} end of group QSPI0_Global_Variables */


/** @defgroup QSPI0_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group QSPI0_Private_FunctionDeclaration */

/** @defgroup QSPI0_Private_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief  QSPI0 interrupt function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void QSPI0_IRQHandler(void)
{
  uint32_t temp;
  
  /* Store interrupt flags for later use */
  temp = QSPI0->ISR.WORDVAL;
  
  /* Clear all interrupt flags */
  QSPI0->ISC.WORDVAL = 0x1;
  
  /* QSPI0 transfer done interrupt */
  if( temp & (1 << QSPI_XFER_DONE) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_XFER_DONE] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_XFER_DONE]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.XFER_DONE_IM = 1;
    }    
  }

  /* QSPI0 transfer ready interrupt */
  if( temp & (1 << QSPI_XFER_RDY) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_XFER_RDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_QSPI0][QSPI_XFER_RDY]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.XFER_RDY_IM = 1;
    }
  }
  
  /* QSPI0 read fifo dma burst interrupt */
  if( temp & (1 << QSPI_RFIFO_DMA_BURST) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_RFIFO_DMA_BURST] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_RFIFO_DMA_BURST]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.RFIFO_DMA_BURST_IM = 1;
    }
  }  
 
  /* QSPI0 write fifo dma burst interrupt */
  if( temp & (1 << QSPI_WFIFO_DMA_BURST) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_WFIFO_DMA_BURST] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_WFIFO_DMA_BURST]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.WFIFO_DMA_BURST_IM = 1;
    } 
  }   
  
  /* QSPI0 read fifo empty interrupt */
  if( temp & (1 << QSPI_RFIFO_EMPTY) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_RFIFO_EMPTY] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_RFIFO_EMPTY]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.RFIFO_EMPTY_IM = 1;
    }
  }   
  
  /* QSPI0 read fifo full interrupt */
  if( temp & (1 << QSPI_RFIFO_FULL) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_RFIFO_FULL] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_RFIFO_FULL]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.RFIFO_FULL_IM = 1;
    } 
  }   

  /* QSPI0 write fifo empty interrupt */
  if( temp & (1 << QSPI_WFIFO_EMPTY) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_WFIFO_EMPTY] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_WFIFO_EMPTY]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.WFIFO_EMPTY_IM = 1;
    } 
  }   
  
  /* QSPI0 write fifo full interrupt */
  if( temp & (1 << QSPI_WFIFO_FULL) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_WFIFO_FULL] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_WFIFO_FULL]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.WFIFO_FULL_IM = 1;
    } 
  }   
  
  /* QSPI0 read fifo underflow interrupt */
  if( temp & (1 << QSPI_RFIFO_UNDRFLW) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_RFIFO_UNDRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_RFIFO_UNDRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.RFIFO_UNDRFLW_IM = 1;
    } 
  }   
  
  /* QSPI0 read fifo overflow interrupt */
  if( temp & (1 << QSPI_RFIFO_OVRFLW) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_RFIFO_OVRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_RFIFO_OVRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.RFIFO_OVRFLW_IM = 1;
    } 
  }  
  
  /* QSPI0 read fifo underflow interrupt */
  if( temp & (1 << QSPI_RFIFO_UNDRFLW) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_RFIFO_UNDRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_RFIFO_UNDRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.WFIFO_UNDRFLW_IM = 1;
    } 
  }   
  
  /* QSPI0 write fifo overflow interrupt */
  if( temp & (1 << QSPI_WFIFO_OVRFLW) )
  {
    if(intCbfArra[INT_QSPI0][QSPI_WFIFO_OVRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI0][QSPI_WFIFO_OVRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI0->IMR.BF.WFIFO_OVRFLW_IM = 1;
    } 
  }  
}

/*@} end of group QSPI0_Private_Functions */

/** @defgroup QSPI0_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      Reset QSPI0
 *
 * @param none
 *
 * @return none
 *
 * Reset QSPI
 *******************************************************************************/
void QSPI0_Reset(void)
{
  uint32_t i;
  
  QSPI0->CONF2.BF.SRST = 1;
  
  /* Delay */
  for(i=0; i<10; i++);
  
  QSPI0->CONF2.BF.SRST = 0;
}

/****************************************************************************//**
 * @brief      Initializes the QSPI0 
 *
 * @param[in]  qspiConfigSet:  Pointer to a QSPI0 configuration structure
 *
 * @return none
 *
 * Initializes the QSPI0 
 *******************************************************************************/
void QSPI0_Init(QSPI_CFG_Type* qspiConfigSet)
{
  /* Set data pin number */
  QSPI0->CONF.BF.DATA_PIN = qspiConfigSet->dataPinMode;
  
  /* Set address pin number */
  QSPI0->CONF.BF.ADDR_PIN = qspiConfigSet->addrPinMode;
  
  /* Set QSPI0 clock mode */
  QSPI0->CONF.BF.CLK_PHA = (qspiConfigSet->clkMode) & 1;
  QSPI0->CONF.BF.CLK_POL = ((qspiConfigSet->clkMode)>>1) & 1;
  
  /* Set QSPI0 capture clock edge */
  QSPI0->TIMING.BF.CLK_CAPT_EDGE = qspiConfigSet->captEdge;
  
  /* Set data length mode */
  QSPI0->CONF.BF.BYTE_LEN = qspiConfigSet->byteLen;
  
  /* Set QSPI0 clock prescaler */
  QSPI0->CONF.BF.CLK_PRESCALE = qspiConfigSet->preScale;
}

/****************************************************************************//**
 * @brief      Mask/Unmask specified interrupt type
 *
 * @param[in]  intType:  Specified interrupt type
 *             - QSPI_XFER_DONE             - QSPI_XFER_RDY
 *             - QSPI_RFIFO_DMA_BURST       - QSPI_WFIFO_DMA_BURST
 *             - QSPI_RFIFO_EMPTY           - QSPI_RFIFO_FULL
 *             - QSPI_WFIFO_EMPTY           - QSPI_WFIFO_FULL
 *             - QSPI_RFIFO_UNDRFLW         - QSPI_RFIFO_OVRFLW
 *             - QSPI_WFIFO_UNDRFLW         - QSPI_WFIFO_OVRFLW
 *             - QSPI_INT_ALL
 * @param[in]  intMask:  Interrupt mask/unmask type
 *             - UNMASK
 *             - MASK
 *
 * @return none
 *
 *******************************************************************************/
void QSPI0_IntMask( QSPI_INT_Type intType, IntMask_Type intMask)
{
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_INT_TYPE(intType));
  CHECK_PARAM(IS_INTMASK(intMask));

  switch(intType)
  {
    case QSPI_XFER_DONE:
      QSPI0->IMR.BF.XFER_DONE_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_XFER_RDY:
      QSPI0->IMR.BF.XFER_RDY_IM = ((intMask == MASK)? 1 : 0);     
      break;

    case QSPI_RFIFO_DMA_BURST:
      QSPI0->IMR.BF.RFIFO_DMA_BURST_IM = ((intMask == MASK)? 1 : 0);     
      break;

    case QSPI_WFIFO_DMA_BURST:
      QSPI0->IMR.BF.WFIFO_DMA_BURST_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_RFIFO_EMPTY:
      QSPI0->IMR.BF.RFIFO_EMPTY_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_RFIFO_FULL:
      QSPI0->IMR.BF.RFIFO_FULL_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_EMPTY:
      QSPI0->IMR.BF.WFIFO_EMPTY_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_FULL:
      QSPI0->IMR.BF.WFIFO_FULL_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_RFIFO_UNDRFLW:
      QSPI0->IMR.BF.RFIFO_UNDRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_RFIFO_OVRFLW:
      QSPI0->IMR.BF.RFIFO_OVRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_UNDRFLW:
      QSPI0->IMR.BF.WFIFO_UNDRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_OVRFLW:
      QSPI0->IMR.BF.WFIFO_OVRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_INT_ALL:
      QSPI0->IMR.WORDVAL = ((intMask == MASK)? 0xFFF : 0);      
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Clear transfer done interrupt
 *
 * @param none
 *
 * @return none
 *
 * Clear transfer done interrupt
 *******************************************************************************/
void QSPI0_IntClr(void)
{
  QSPI0->ISC.WORDVAL = 1;
}

/****************************************************************************//**
 * @brief      Flush Read and Write FIFOs
 *
 * @param none
 *
 * @return none
 *
 * Flush Read and Write FIFOs
 *******************************************************************************/
Status QSPI0_FlushFIFO(void)
{
  volatile uint32_t localCnt = 0;

  QSPI0->CONF.BF.FIFO_FLUSH = 1;
    
  /* Wait until Write and Read FIFOs are flushed. */ 
  while(localCnt++ < 0xFFFFFFF)
  {
    if( (QSPI0->CONF.BF.FIFO_FLUSH) == RESET )
    {
      return DSUCCESS;
    }
  }
  
  return DERROR;    
}

/****************************************************************************//**
 * @brief      Set QSPI0 serial interface header count 
 *
 * @param[in]  instrCnt:  number of bytes in INSTR register to shift out
 *                  QSPI_INSTR_CNT_0BYTE/QSPI_INSTR_CNT_1BYTE/QSPI_INSTR_CNT_2BYTE
 *
 *             addrCnt:   number of bytes in ADDR register to shift out
 *                  QSPI_ADDR_CNT_0BYTE/QSPI_ADDR_CNT_1BYTE/QSPI_ADDR_CNT_2BYTE/QSPI_ADDR_CNT_3BYTE/QSPI_ADDR_CNT_4BYTE
 *
 *             rmCnt:     number of bytes in RMODE register to shift out
 *                  QSPI_RM_CNT_0BYTE/QSPI_RM_CNT_1BYTE/QSPI_RM_CNT_2BYTE
 *
 *             dummyCnt:  number of bytes as dummy to shift out
 *                  QSPI_DUMMY_CNT_0BYTE/QSPI_DUMMY_CNT_1BYTE/QSPI_DUMMY_CNT_2BYTE/QSPI_DUMMY_CNT_3BYTE
 *
 * @return none
 *
 * Set QSPI0 serial interface header count 
 *******************************************************************************/
void QSPI0_SetHdrcnt(QSPI_INSTR_CNT_TYPE instrCnt, QSPI_ADDR_CNT_TYPE addrCnt, QSPI_RM_CNT_TYPE rmCnt, QSPI_DUMMY_CNT_TYPE dummyCnt)
{
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_INSTR_CNT_TYPE(instrCnt));
  CHECK_PARAM(IS_QSPI_ADDR_CNT_TYPE(addrCnt));
  CHECK_PARAM(IS_QSPI_RM_CNT_TYPE(rmCnt));
  CHECK_PARAM(IS_QSPI_DUMMY_CNT_TYPE(dummyCnt));  

  QSPI0->HDRCNT.BF.INSTR_CNT = instrCnt;
  QSPI0->HDRCNT.BF.ADDR_CNT  = addrCnt;
  QSPI0->HDRCNT.BF.RM_CNT    = rmCnt;
  QSPI0->HDRCNT.BF.DUMMY_CNT = dummyCnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes in INSTR register to shift out to the serial interface 
 *
 * @param[in]  instrCnt:   number of bytes in INSTR register to shift out
 *
 * @return none
 *
 * Set number of bytes in INSTR register to shift out to the serial interface 
 *******************************************************************************/
void QSPI0_SetInstrCnt(QSPI_INSTR_CNT_TYPE instrCnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_INSTR_CNT_TYPE(instrCnt));

  QSPI0->HDRCNT.BF.INSTR_CNT = instrCnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes in ADDR register to shift out to the serial interface 
 *
 * @param[in]  addrCnt:   number of bytes in ADDR register to shift out
 *
 * @return none
 *
 * Set number of bytes in ADDR register to shift out to the serial interface 
 *******************************************************************************/
void QSPI0_SetAddrCnt(QSPI_ADDR_CNT_TYPE addrCnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_ADDR_CNT_TYPE(addrCnt));

  QSPI0->HDRCNT.BF.ADDR_CNT = addrCnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes in RMODE register to shift out to the serial interface 
 *
 * @param[in]  rmCnt:   number of bytes in RMODE register to shift out
 *
 * @return none
 *
 * Set number of bytes in RMODE register to shift out to the serial interface 
 *******************************************************************************/
void QSPI0_SetRModeCnt(QSPI_RM_CNT_TYPE rmCnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_RM_CNT_TYPE(rmCnt));

  QSPI0->HDRCNT.BF.RM_CNT = rmCnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes as dummy to shift out to the serial interface 
 *
 * @param[in]  dummyCnt:   number of bytes as dummy to shift out
 *
 * @return none
 *
 * Set number of bytes as dummy to shift out to the serial interface
 *******************************************************************************/
void QSPI0_SetDummyCnt(QSPI_DUMMY_CNT_TYPE dummyCnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_DUMMY_CNT_TYPE(dummyCnt));  

  QSPI0->HDRCNT.BF.DUMMY_CNT = dummyCnt;
}

/****************************************************************************//**
 * @brief      Set QSPI0 serial interface instruction 
 *
 * @param[in]  instruct:  QSPI0 serial interface instruction
 *
 * @return none
 *
 * Set QSPI0 serial interface instruction 
 *******************************************************************************/
void QSPI0_SetInstr(uint32_t instruct)
{
  QSPI0->INSTR.BF.INSTR = instruct;
}

/****************************************************************************//**
 * @brief      Set QSPI serial interface address 
 *
 * @param[in]  address:  QSPI serial interface address
 *
 * @return none
 *
 * Set QSPI serial interface address  
 *******************************************************************************/
void QSPI0_SetAddr(uint32_t address)
{
  QSPI0->ADDR.WORDVAL = address;
}

/****************************************************************************//**
 * @brief      Set QSPI serial interface read mode 
 *
 * @param[in]  readMode:  QSPI serial interface read mode
 *
 * @return none
 *
 * Set QSPI serial interface read mode 
 *******************************************************************************/
void QSPI0_SetRMode(uint32_t readMode)
{
  QSPI0->RDMODE.BF.RMODE = readMode;
}

/****************************************************************************//**
 * @brief      Set number of bytes of data to shift in from the serial interface 
 *
 * @param[in]  count:   number of bytes of data to shift in
 *
 * @return none
 *
 * Set number of bytes of data to shift in from the serial interface
 *******************************************************************************/
void QSPI0_SetDInCnt(uint32_t count)
{
  QSPI0->DINCNT.BF.DATA_IN_CNT = count;
}

/****************************************************************************//**
 * @brief      Activate or de-activate serial select output 
 *
 * @param[in]  newCmd:   Activate or de-activate
 *
 * @return none
 *
 * Activate or de-activate serial select output
 *******************************************************************************/
 void QSPI0_SetSSEnable(FunctionalState newCmd)
{
  QSPI0->CNTL.BF.SS_EN = newCmd;
  while(0==QSPI0->CNTL.BF.XFER_RDY);
}

/****************************************************************************//**
 * @brief      Start the specified QSPI0 transfer
 *
 * @param[in]  rw:  read/write transfer
 *
 * @return none
 *
 * Start the QSPI0 transfer 
 *******************************************************************************/
void QSPI0_StartTransfer(QSPI_RW_Type rw)
{
  /* Assert QSPI0 SS */
  QSPI0_SetSSEnable(ENABLE);
  
  /* Set read/write mode */
  QSPI0->CONF.BF.RW_EN = rw;
  
  /* Start QSPI0 */
  QSPI0->CONF.BF.XFER_START = 1;
}  

/****************************************************************************//**
 * @brief      Stop QSPI0 transfer
 *
 * @param none
 *
 * @return none
 *
 * Stop QSPI0 transfer
 *******************************************************************************/
void QSPI0_StopTransfer(void) 
{
  /* Wait until QSPI0 ready */
  while(QSPI0->CNTL.BF.XFER_RDY == 0);
  
  /* Wait until wfifo empty */
  while(QSPI0->CNTL.BF.WFIFO_EMPTY == 0);
  
  /* Stop QSPI0 */
  QSPI0->CONF.BF.XFER_STOP = 1;
  
  /* Wait until QSPI0 release start signal */
  while(QSPI0->CONF.BF.XFER_START == 1);
  
  /* De-assert QSPI0 SS */
  QSPI0_SetSSEnable(DISABLE);
}

/****************************************************************************//**
 * @brief      Write a byte to QSPI0 serial interface
 *
 * @param[in]  byte:  data to be written
 *
 * @return none
 *
 * Write a byte to QSPI serial interface 
 *******************************************************************************/
void QSPI0_WriteByte(uint8_t byte)
{
  /* Wait unitl WFIFO is not full*/
  while(QSPI0->CNTL.BF.WFIFO_FULL == 1);
  
  QSPI0->DOUT.WORDVAL = (byte & 0xFF);
}

/****************************************************************************//**
 * @brief     Read a byte from QSPI0 serial interface
 *
 * @param none
 *
 * @return    byte from QSPI serial interface
 *
 * Read a byte from QSPI serial interface 
 *******************************************************************************/
uint8_t QSPI0_ReadByte(void)
{
  uint8_t data;
  
  /* Wait if RFIFO is empty*/
  while(QSPI0->CNTL.BF.RFIFO_EMPTY == 1);
  
  data = (QSPI0->DIN.BF.DATA_IN & 0xFF);
  return data;
}

/****************************************************************************//**
 * @brief      Write a word to QSPI0 serial interface
 *
 * @param[in]  word:  data to be written
 *
 * @return none
 *
 * Write a word to QSPI0 serial interface 
 *******************************************************************************/
void QSPI0_WriteWord(uint32_t word)
{
  /* Wait unitl WFIFO is not full*/
  while(QSPI0->CNTL.BF.WFIFO_FULL == 1);
  
  QSPI0->DOUT.WORDVAL = word;
}

/****************************************************************************//**
 * @brief     Read a word from QSPI0 serial interface
 *
 * @param none
 *
 * @return    word from QSPI0 serial interface
 *
 * Read a word from QSPI0 serial interface 
 *******************************************************************************/
uint32_t QSPI0_ReadWord(void)
{
  /* Wait if RFIFO is empty*/
  while(QSPI0->CNTL.BF.RFIFO_EMPTY == 1);
  
  return QSPI0->DIN.WORDVAL;
}

/****************************************************************************//**
 * @brief      Check whether status flag is set or not for given status type
 *
 * @param[in]  spiStatus:  Specified status type
 *             - QSPI_STATUS_XFER_RDY
 *             - QSPI_STATUS_RFIFO_EMPTY
 *             - QSPI_STATUS_RFIFO_FULL
 *             - QSPI_STATUS_WFIFO_EMPTY
 *             - QSPI_STATUS_WFIFO_FULL
 *             - QSPI_STATUS_RFIFO_UNDRFLW
 *             - QSPI_STATUS_RFIFO_OVRFLW
 *             - QSPI_STATUS_WFIFO_UNDRFLW
 *             - QSPI_STATUS_WFIFO_OVRFLW
 *
 * @return The state value of QSPI0 serial interface control register. 
 *             - SET 
 *             - RESET
 *
 *******************************************************************************/
FlagStatus QSPI0_GetStatus(QSPI_Status_Type qspiStatus)
{
  FlagStatus intBitStatus = RESET;
    
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_STATUS_BIT(qspiStatus));

  switch(qspiStatus)
  {
    case QSPI_STATUS_XFER_RDY:
      intBitStatus = (QSPI0->CNTL.BF.XFER_RDY? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_EMPTY:
      intBitStatus = (QSPI0->CNTL.BF.RFIFO_EMPTY? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_FULL:
      intBitStatus = (QSPI0->CNTL.BF.RFIFO_FULL? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_EMPTY:
      intBitStatus = (QSPI0->CNTL.BF.WFIFO_EMPTY? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_FULL:
      intBitStatus = (QSPI0->CNTL.BF.WFIFO_FULL? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_UNDRFLW:
      intBitStatus = (QSPI0->CNTL.BF.RFIFO_UNDRFLW? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_OVRFLW:
      intBitStatus = (QSPI0->CNTL.BF.RFIFO_OVRFLW? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_UNDRFLW:
      intBitStatus = (QSPI0->CNTL.BF.WFIFO_UNDRFLW? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_OVRFLW:
      intBitStatus = (QSPI0->CNTL.BF.WFIFO_OVRFLW? SET : RESET); 
      break;
      
    default:
      break;
  }

  return intBitStatus;
}

/****************************************************************************//**
 * @brief      Check whether int status flag is set or not for given status type
 *
 * @param[in]  qspiIntStatus:  Specified int status type
 *             - QSPI_XFER_DONE             - QSPI_XFER_RDY
 *             - QSPI_RFIFO_DMA_BURST       - QSPI_WFIFO_DMA_BURST
 *             - QSPI_RFIFO_EMPTY           - QSPI_RFIFO_FULL
 *             - QSPI_WFIFO_EMPTY           - QSPI_WFIFO_FULL
 *             - QSPI_RFIFO_UNDRFLW         - QSPI_RFIFO_OVRFLW
 *             - QSPI_WFIFO_UNDRFLW         - QSPI_WFIFO_OVRFLW
 *             - QSPI_INT_ALL
 *
 * @return The state value of QSPI0 serial interface control register. 
 *             - SET 
 *             - RESET
 *
 *******************************************************************************/
FlagStatus QSPI0_GetIntStatus(QSPI_INT_Type qspiIntStatus)
{
  FlagStatus intBitStatus = RESET;
    
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_INT_TYPE(qspiIntStatus));

  switch (qspiIntStatus)
  {
    case QSPI_XFER_DONE:
      intBitStatus = (QSPI0->ISR.BF.XFER_DONE_IS? SET : RESET); 
      break;
      
    case QSPI_XFER_RDY:
      intBitStatus = (QSPI0->ISR.BF.XFER_RDY_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_DMA_BURST:
      intBitStatus = (QSPI0->ISR.BF.RFIFO_DMA_BURST_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_DMA_BURST:
      intBitStatus = (QSPI0->ISR.BF.WFIFO_DMA_BURST_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_EMPTY:
      intBitStatus = (QSPI0->ISR.BF.RFIFO_EMPTY_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_FULL:
      intBitStatus = (QSPI0->ISR.BF.RFIFO_FULL_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_EMPTY:
      intBitStatus = (QSPI0->ISR.BF.WFIFO_EMPTY_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_FULL:
      intBitStatus = (QSPI0->ISR.BF.WFIFO_FULL_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_UNDRFLW:
      intBitStatus = (QSPI0->ISR.BF.RFIFO_UNDRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_OVRFLW:
      intBitStatus = (QSPI0->ISR.BF.RFIFO_OVRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_UNDRFLW:
      intBitStatus = (QSPI0->ISR.BF.WFIFO_UNDRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_OVRFLW:
      intBitStatus = (QSPI0->ISR.BF.WFIFO_OVRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_INT_ALL:
      intBitStatus = ((QSPI0->ISR.WORDVAL & 0xFFF)? SET : RESET); 
      break;

  default:
      break;
  }

  return intBitStatus;
}

/****************************************************************************//**
 * @brief      Get serial interface transfer state
 *
 * @param none
 *
 * @return    serial interface transfer state
 *            - SET 
 *            - RESET
 * Get serial interface transfer state
 *******************************************************************************/
FlagStatus QSPI0_IsTransferCompleted(void)
{
  return (QSPI0->CONF.BF.XFER_START == 0? SET : RESET); 
}

/****************************************************************************//**
 * @brief      Enable or disable the QSPI0 DMA function and set the burst size
 *
 * @param[in]  dmaCtrl: DMA read or write, QSPI_DMA_READ or QSPI_DMA_WRITE
 * @param[in]  dmaDataCtrl: DMA Diable/Enable and set the data burst size
 *                          - QSPI_DMA_DISABLE
 *                          - QSPI_DMA_1DATA
 *                          - QSPI_DMA_4DATA
 *                          - QSPI_DMA_8DATA
 *
 * @return none
 *
 *******************************************************************************/
void QSPI0_DmaCmd(QSPI_DMA_Type dmaCtrl, QSPI_DMA_Data_Type dmaDataCtrl)
{
  CHECK_PARAM(IS_QSPI_DMA_TYPE(dmaCtrl));
  CHECK_PARAM(IS_QSPI_DMA_DATA_TYPE(dmaDataCtrl));

  if(dmaCtrl == QSPI_DMA_READ)
  {
    if(dmaDataCtrl == QSPI_DMA_DISABLE)
    {
      QSPI0->CONF2.BF.DMA_RD_EN = 0;
    }
    else
    {
      QSPI0->CONF2.BF.DMA_RD_EN = 1;
      QSPI0->CONF2.BF.DMA_RD_BURST = dmaDataCtrl;
    }
  }
  else
  {
    if(dmaDataCtrl == QSPI_DMA_DISABLE)
    {
      QSPI0->CONF2.BF.DMA_WR_EN = 0;
    }
    else
    {
      QSPI0->CONF2.BF.DMA_WR_EN = 1;
      QSPI0->CONF2.BF.DMA_WR_BURST = dmaDataCtrl;
    }
  }
}

void QSPI0_Init_CLK()
{
       QSPI0->CONF.BF.CLK_PRESCALE = 0x0;
}

/*@} end of group QSPI0_Public_Functions */

/*@} end of group QSPI0_definitions */

/*@} end of group MC200_Periph_Driver */
