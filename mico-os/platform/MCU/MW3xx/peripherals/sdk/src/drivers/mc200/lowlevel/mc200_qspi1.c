/****************************************************************************//**
 * @file     mc200_qspi1.c
 * @brief    This file provides QSPI1 functions.
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

/** @addtogroup MC200_Periph_Driver
 *  @{
 */

/** @defgroup QSPI1 QSPI1
 *  @brief QSPI1 driver modules
 *  @{
 */

/** @defgroup QSPI1_Private_Type
 *  @{
 */

/*@} end of group QSPI1_Private_Type*/

/** @defgroup QSPI1_Private_Defines
 *  @{
 */

/*@} end of group QSPI1_Private_Defines */

/** @defgroup QSPI1_Private_Variables
 *  @{
 */

/*@} end of group QSPI1_Private_Variables */

/** @defgroup QSPI1_Global_Variables
 *  @{
 */

/*@} end of group QSPI1_Global_Variables */


/** @defgroup QSPI1_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group QSPI1_Private_FunctionDeclaration */

/** @defgroup QSPI1_Private_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief  QSPI1 interrupt function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void QSPI1_IRQHandler(void)
{
  uint32_t temp;
  
  /* Store interrupt flags for later use */
  temp = QSPI1->ISR.WORDVAL;
  
  /* Clear all interrupt flags */
  QSPI1->ISC.WORDVAL = 0x1;
  
  /* QSPI1 transfer done inerrupt */
  if( temp & (1 << QSPI_XFER_DONE) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_XFER_DONE] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_XFER_DONE]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.XFER_DONE_IM = 1;
    }    
  }

  /* QSPI1 transfer ready interrupt */
  if( temp & (1 << QSPI_XFER_RDY) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_XFER_RDY] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_XFER_RDY]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.XFER_RDY_IM = 1;
    }
  }
  
  /* QSPI1 read fifo dma burst interrupt */
  if( temp & (1 << QSPI_RFIFO_DMA_BURST) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_RFIFO_DMA_BURST] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_RFIFO_DMA_BURST]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.RFIFO_DMA_BURST_IM = 1;
    }
  }  
 
  /* QSPI1 write fifo dma burst interrupt */
  if( temp & (1 << QSPI_WFIFO_DMA_BURST) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_WFIFO_DMA_BURST] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_WFIFO_DMA_BURST]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.WFIFO_DMA_BURST_IM = 1;
    } 
  }   
  
  /* QSPI1 read fifo empty interrupt */
  if( temp & (1 << QSPI_RFIFO_EMPTY) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_RFIFO_EMPTY] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_RFIFO_EMPTY]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.RFIFO_EMPTY_IM = 1;
    }
  }   
  
  /* QSPI1 read fifo full inerrupt */
  if( temp & (1 << QSPI_RFIFO_FULL) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_RFIFO_FULL] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_RFIFO_FULL]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.RFIFO_FULL_IM = 1;
    } 
  }   

  /* QSPI1 write fifo empty interrupt */
  if( temp & (1 << QSPI_WFIFO_EMPTY) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_WFIFO_EMPTY] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_WFIFO_EMPTY]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.WFIFO_EMPTY_IM = 1;
    } 
  }   
  
  /* QSPI1 write fifo full interrupt */
  if( temp & (1 << QSPI_WFIFO_FULL) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_WFIFO_FULL] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_WFIFO_FULL]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.WFIFO_FULL_IM = 1;
    } 
  }   
  
  /* QSPI1 read fifo underflow interrupt */
  if( temp & (1 << QSPI_RFIFO_UNDRFLW) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_RFIFO_UNDRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_RFIFO_UNDRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.RFIFO_UNDRFLW_IM = 1;
    } 
  }   
  
  /* QSPI1 read fifo overflow interrupt */
  if( temp & (1 << QSPI_RFIFO_OVRFLW) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_RFIFO_OVRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_RFIFO_OVRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.RFIFO_OVRFLW_IM = 1;
    } 
  }  
  
  /* QSPI1 read fifo underflow interrupt */
  if( temp & (1 << QSPI_RFIFO_UNDRFLW) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_RFIFO_UNDRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_RFIFO_UNDRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.WFIFO_UNDRFLW_IM = 1;
    } 
  }   
  
  /* QSPI1 write fifo overflow interrupt */
  if( temp & (1 << QSPI_WFIFO_OVRFLW) )
  {
    if(intCbfArra[INT_QSPI1][QSPI_WFIFO_OVRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI1][QSPI_WFIFO_OVRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI1->IMR.BF.WFIFO_OVRFLW_IM = 1;
    } 
  }  
}

/*@} end of group QSPI1_Private_Functions */

/** @defgroup QSPI1_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      Reset QSPI1
 *
 * @param none
 *
 * @return none
 *
 * Reset QSPI1
 *******************************************************************************/
void QSPI1_Reset(void)
{
  uint32_t i;
  
  QSPI1->CONF2.BF.SRST = 1;
  
  /* Delay */
  for(i=0; i<10; i++);
  
  QSPI1->CONF2.BF.SRST = 0;
}

/****************************************************************************//**
 * @brief      Initializes the QSPI1 
 *
 * @param[in]  qspiConfigSet:  Pointer to a QSPI configuration structure
 *
 * @return none
 *
 * Initializes the QSPI1 
 *******************************************************************************/
void QSPI1_Init(QSPI_CFG_Type* qspiConfigSet)
{
  /* Set data pin number */
  QSPI1->CONF.BF.DATA_PIN = qspiConfigSet->dataPinMode;
  
  /* Set address pin number */
  QSPI1->CONF.BF.ADDR_PIN = qspiConfigSet->addrPinMode;
  
  /* Set QSPI1 clock mode */
  QSPI1->CONF.BF.CLK_PHA = (qspiConfigSet->clkMode) & 1;
  QSPI1->CONF.BF.CLK_POL = ((qspiConfigSet->clkMode)>>1) & 1;
  
  /* Set QSPI1 capture clock edge */
  QSPI1->TIMING.BF.CLK_CAPT_EDGE = qspiConfigSet->captEdge;
  
  /* Set data length mode */
  QSPI1->CONF.BF.BYTE_LEN = qspiConfigSet->byteLen;
  
  /* Set QSPI1 clock prescaler */
  QSPI1->CONF.BF.CLK_PRESCALE = qspiConfigSet->preScale;
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
void QSPI1_IntMask( QSPI_INT_Type intType, IntMask_Type intMask)
{
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_INT_TYPE(intType));
  CHECK_PARAM(IS_INTMASK(intMask));

  switch(intType)
  {
    case QSPI_XFER_DONE:
      QSPI1->IMR.BF.XFER_DONE_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_XFER_RDY:
      QSPI1->IMR.BF.XFER_RDY_IM = ((intMask == MASK)? 1 : 0);     
      break;

    case QSPI_RFIFO_DMA_BURST:
      QSPI1->IMR.BF.RFIFO_DMA_BURST_IM = ((intMask == MASK)? 1 : 0);     
      break;

    case QSPI_WFIFO_DMA_BURST:
      QSPI1->IMR.BF.WFIFO_DMA_BURST_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_RFIFO_EMPTY:
      QSPI1->IMR.BF.RFIFO_EMPTY_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_RFIFO_FULL:
      QSPI1->IMR.BF.RFIFO_FULL_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_EMPTY:
      QSPI1->IMR.BF.WFIFO_EMPTY_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_FULL:
      QSPI1->IMR.BF.WFIFO_FULL_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_RFIFO_UNDRFLW:
      QSPI1->IMR.BF.RFIFO_UNDRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_RFIFO_OVRFLW:
      QSPI1->IMR.BF.RFIFO_OVRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_UNDRFLW:
      QSPI1->IMR.BF.WFIFO_UNDRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_OVRFLW:
      QSPI1->IMR.BF.WFIFO_OVRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_INT_ALL:
      QSPI1->IMR.WORDVAL = ((intMask == MASK)? 0xFFF : 0);      
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
void QSPI1_IntClr(void)
{
  QSPI1->ISC.WORDVAL = 1;
}

/****************************************************************************//**
 * @brief      Flush Write and Read FIFOs
 *
 * @param none
 *
 * @return none
 *
 * Flush Write and Read FIFOs
 *******************************************************************************/
Status QSPI1_FlushFIFO(void)
{
  volatile uint32_t localCnt = 0;

  QSPI1->CONF.BF.FIFO_FLUSH = 1;
    
  /* Wait until Write and Read FIFOs are flushed. */ 
  while(localCnt++ < 0xFFFFFFF)
  {
    if( (QSPI1->CONF.BF.FIFO_FLUSH) == RESET )
    {
      return DSUCCESS;
    }
  }
  
  return DERROR;    
}

/****************************************************************************//**
 * @brief      Set QSPI1 serial interface header count 
 *
 * @param[in]  instrcnt:  number of bytes in INSTR register to shift out
 *                  QSPI_INSTR_CNT_0BYTE/QSPI_INSTR_CNT_1BYTE/QSPI_INSTR_CNT_2BYTE
 *
 *             addrcnt:   number of bytes in ADDR register to shift out
 *                  QSPI_ADDR_CNT_0BYTE/QSPI_ADDR_CNT_1BYTE/QSPI_ADDR_CNT_2BYTE/QSPI_ADDR_CNT_3BYTE/QSPI_ADDR_CNT_4BYTE
 *
 *             rmcnt:     number of bytes in RMODE register to shift out
 *                  QSPI_RM_CNT_0BYTE/QSPI_RM_CNT_1BYTE/QSPI_RM_CNT_2BYTE
 *
 *             dummycnt:  number of bytes as dummy to shift out
 *                  QSPI_DUMMY_CNT_0BYTE/QSPI_DUMMY_CNT_1BYTE/QSPI_DUMMY_CNT_2BYTE/QSPI_DUMMY_CNT_3BYTE
 *
 * @return none
 *
 * Set QSPI1 serial interface header count 
 *******************************************************************************/
void QSPI1_SetHdrcnt(QSPI_INSTR_CNT_TYPE instrcnt, QSPI_ADDR_CNT_TYPE addrcnt, QSPI_RM_CNT_TYPE rmcnt, QSPI_DUMMY_CNT_TYPE dummycnt)
{
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_INSTR_CNT_TYPE(instrcnt));
  CHECK_PARAM(IS_QSPI_ADDR_CNT_TYPE(addrcnt));
  CHECK_PARAM(IS_QSPI_RM_CNT_TYPE(rmcnt));
  CHECK_PARAM(IS_QSPI_DUMMY_CNT_TYPE(dummycnt));  

  QSPI1->HDRCNT.BF.INSTR_CNT = instrcnt;
  QSPI1->HDRCNT.BF.ADDR_CNT  = addrcnt;
  QSPI1->HDRCNT.BF.RM_CNT    = rmcnt;
  QSPI1->HDRCNT.BF.DUMMY_CNT = dummycnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes in INSTR register to shift out to the serial interface 
 *
 * @param[in]  instrcnt:   number of bytes in INSTR register to shift out
 *
 * @return none
 *
 * Set number of bytes in INSTR register to shift out to the serial interface 
 *******************************************************************************/
void QSPI1_SetInstrCnt(QSPI_INSTR_CNT_TYPE instrcnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_INSTR_CNT_TYPE(instrcnt));

  QSPI1->HDRCNT.BF.INSTR_CNT = instrcnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes in ADDR register to shift out to the serial interface 
 *
 * @param[in]  addrcnt:   number of bytes in ADDR register to shift out
 *
 * @return none
 *
 * Set number of bytes in ADDR register to shift out to the serial interface 
 *******************************************************************************/
void QSPI1_SetAddrCnt(QSPI_ADDR_CNT_TYPE addrcnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_ADDR_CNT_TYPE(addrcnt));

  QSPI1->HDRCNT.BF.ADDR_CNT = addrcnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes in RMODE register to shift out to the serial interface 
 *
 * @param[in]  rmcnt:   number of bytes in RMODE register to shift out
 *
 * @return none
 *
 * Set number of bytes in RMODE register to shift out to the serial interface 
 *******************************************************************************/
void QSPI1_SetRModeCnt(QSPI_RM_CNT_TYPE rmcnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_RM_CNT_TYPE(rmcnt));

  QSPI1->HDRCNT.BF.RM_CNT = rmcnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes as dummy to shift out to the serial interface 
 *
 * @param[in]  dummycnt:   number of bytes as dummy to shift out
 *
 * @return none
 *
 * Set number of bytes as dummy to shift out to the serial interface
 *******************************************************************************/
void QSPI1_SetDummyCnt(QSPI_DUMMY_CNT_TYPE dummycnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_DUMMY_CNT_TYPE(dummycnt));  

  QSPI1->HDRCNT.BF.DUMMY_CNT = dummycnt;
}

/****************************************************************************//**
 * @brief      Set QSPI1 serial interface instruction 
 *
 * @param[in]  instruct:  QSPI1 serial interface instruction
 *
 * @return none
 *
 * Set QSPI1 serial interface instruction 
 *******************************************************************************/
void QSPI1_SetInstr(uint32_t instruct)
{
  QSPI1->INSTR.BF.INSTR = instruct;
}

/****************************************************************************//**
 * @brief      Set QSPI1 serial interface address 
 *
 * @param[in]  address:  QSPI1 serial interface address
 *
 * @return none
 *
 * Set QSPI1 serial interface address  
 *******************************************************************************/
void QSPI1_SetAddr(uint32_t address)
{
  QSPI1->ADDR.WORDVAL = address;
}

/****************************************************************************//**
 * @brief      Set QSPI1 serial interface read mode 
 *
 * @param[in]  readMode:  QSPI1 serial interface read mode
 *
 * @return none
 *
 * Set QSPI1 serial interface read mode 
 *******************************************************************************/
void QSPI1_SetRMode(uint32_t readMode)
{
  QSPI1->RDMODE.BF.RMODE = readMode;
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
void QSPI1_SetDInCnt(uint32_t count)
{
  QSPI1->DINCNT.BF.DATA_IN_CNT = count;
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
 void QSPI1_SetSSEnable(FunctionalState newCmd)
{
  QSPI1->CNTL.BF.SS_EN = newCmd;
  while(0==QSPI1->CNTL.BF.XFER_RDY);
}

/****************************************************************************//**
 * @brief      Start the specified QSPI1 transfer
 *
 * @param[in]  rw:  read/write transfer
 *
 * @return none
 *
 * Start the QSPI1 transfer 
 *******************************************************************************/
void QSPI1_StartTransfer(QSPI_RW_Type rw)
{
  /* Assert QSPI1 SS */
  QSPI1_SetSSEnable(ENABLE);
  
  /* Set read/write mode */
  QSPI1->CONF.BF.RW_EN = rw;
  
  /* Start QSPI1 */
  QSPI1->CONF.BF.XFER_START = 1;
}  

/****************************************************************************//**
 * @brief      Stop QSPI1 transfer
 *
 * @param none
 *
 * @return none
 *
 * Stop QSPI1 transfer
 *******************************************************************************/
void QSPI1_StopTransfer(void) 
{
  /* Wait until QSPI1 ready */
  while(QSPI1->CNTL.BF.XFER_RDY == 0);
  
  /* Wait until wfifo empty */
  while(QSPI1->CNTL.BF.WFIFO_EMPTY == 0);
        
  /* Stop QSPI1 */
  QSPI1->CONF.BF.XFER_STOP = 1;
  
  /* Wait until QSPI1 release start signal */
  while(QSPI1->CONF.BF.XFER_START == 1);
  
  /* De-assert QSPI1 SS */
  QSPI1_SetSSEnable(DISABLE);
}

/****************************************************************************//**
 * @brief      Write a byte to QSPI1 serial interface
 *
 * @param[in]  byte:  data to be written
 *
 * @return none
 *
 * Write a byte to QSPI1 serial interface 
 *******************************************************************************/
void QSPI1_WriteByte(uint8_t byte)
{
  /* Wait unitl WFIFO is not full*/
  while(QSPI1->CNTL.BF.WFIFO_FULL == 1);
  
  QSPI1->DOUT.WORDVAL = (byte & 0xFF);
}

/****************************************************************************//**
 * @brief     Read a byte from QSPI1 serial interface
 *
 * @param none
 *
 * @return    byte from QSPI1 serial interface
 *
 * Read a byte from QSPI1 serial interface 
 *******************************************************************************/
uint8_t QSPI1_ReadByte(void)
{
  uint8_t data;
  
  /* Wait if RFIFO is empty*/
  while(QSPI1->CNTL.BF.RFIFO_EMPTY == 1);
  
  data = (QSPI1->DIN.BF.DATA_IN & 0xFF);
  return data;
}

/****************************************************************************//**
 * @brief      Write a word to QSPI1 serial interface
 *
 * @param[in]  word:  data to be written
 *
 * @return none
 *
 * Write a word to QSPI1 serial interface 
 *******************************************************************************/
void QSPI1_WriteWord(uint32_t word)
{
  /* Wait unitl WFIFO is not full*/
  while(QSPI1->CNTL.BF.WFIFO_FULL == 1);
  
  QSPI1->DOUT.WORDVAL = word;
}

/****************************************************************************//**
 * @brief     Read a word from QSPI1 serial interface
 *
 * @param none
 *
 * @return    word from QSPI1 serial interface
 *
 * Read a word from QSPI1 serial interface 
 *******************************************************************************/
uint32_t QSPI1_ReadWord(void)
{
  /* Wait if RFIFO is empty*/
  while(QSPI1->CNTL.BF.RFIFO_EMPTY == 1);
  
  return QSPI1->DIN.WORDVAL;
}

/****************************************************************************//**
 * @brief      Check whether status flag is set or not for given status type
 *
 * @param[in]  qspiStatus:  Specified status type
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
 * @return The state value of QSPI1 serial interface control register. 
 *             - SET 
 *             - RESET
 *
 *******************************************************************************/
FlagStatus QSPI1_GetStatus(QSPI_Status_Type qspiStatus)
{
  FlagStatus intBitStatus = RESET;
    
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_STATUS_BIT(qspiStatus));

  switch(qspiStatus)
  {
    case QSPI_STATUS_XFER_RDY:
      intBitStatus = (QSPI1->CNTL.BF.XFER_RDY? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_EMPTY:
      intBitStatus = (QSPI1->CNTL.BF.RFIFO_EMPTY? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_FULL:
      intBitStatus = (QSPI1->CNTL.BF.RFIFO_FULL? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_EMPTY:
      intBitStatus = (QSPI1->CNTL.BF.WFIFO_EMPTY? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_FULL:
      intBitStatus = (QSPI1->CNTL.BF.WFIFO_FULL? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_UNDRFLW:
      intBitStatus = (QSPI1->CNTL.BF.RFIFO_UNDRFLW? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_OVRFLW:
      intBitStatus = (QSPI1->CNTL.BF.RFIFO_OVRFLW? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_UNDRFLW:
      intBitStatus = (QSPI1->CNTL.BF.WFIFO_UNDRFLW? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_OVRFLW:
      intBitStatus = (QSPI1->CNTL.BF.WFIFO_OVRFLW? SET : RESET); 
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
 * @return The state value of QSPI1 serial interface control register. 
 *             - SET 
 *             - RESET
 *
 *******************************************************************************/
FlagStatus QSPI1_GetIntStatus(QSPI_INT_Type qspiIntStatus)
{
  FlagStatus intBitStatus = RESET;
    
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_INT_TYPE(qspiIntStatus));

  switch (qspiIntStatus)
  {
    case QSPI_XFER_DONE:
      intBitStatus = (QSPI1->ISR.BF.XFER_DONE_IS? SET : RESET); 
      break;
      
    case QSPI_XFER_RDY:
      intBitStatus = (QSPI1->ISR.BF.XFER_RDY_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_DMA_BURST:
      intBitStatus = (QSPI1->ISR.BF.RFIFO_DMA_BURST_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_DMA_BURST:
      intBitStatus = (QSPI1->ISR.BF.WFIFO_DMA_BURST_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_EMPTY:
      intBitStatus = (QSPI1->ISR.BF.RFIFO_EMPTY_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_FULL:
      intBitStatus = (QSPI1->ISR.BF.RFIFO_FULL_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_EMPTY:
      intBitStatus = (QSPI1->ISR.BF.WFIFO_EMPTY_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_FULL:
      intBitStatus = (QSPI1->ISR.BF.WFIFO_FULL_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_UNDRFLW:
      intBitStatus = (QSPI1->ISR.BF.RFIFO_UNDRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_OVRFLW:
      intBitStatus = (QSPI1->ISR.BF.RFIFO_OVRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_UNDRFLW:
      intBitStatus = (QSPI1->ISR.BF.WFIFO_UNDRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_OVRFLW:
      intBitStatus = (QSPI1->ISR.BF.WFIFO_OVRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_INT_ALL:
      intBitStatus = ((QSPI1->ISR.WORDVAL & 0xFFF)? SET : RESET); 
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
FlagStatus QSPI1_IsTransferCompleted(void)
{
  return (QSPI1->CONF.BF.XFER_START == 0? SET : RESET); 
}

/****************************************************************************//**
 * @brief      Enable or disable the QSPI1 DMA function and set the burst size
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
void QSPI1_DmaCmd(QSPI_DMA_Type dmaCtrl, QSPI_DMA_Data_Type dmaDataCtrl)
{
  CHECK_PARAM(IS_QSPI_DMA_TYPE(dmaCtrl));
  CHECK_PARAM(IS_QSPI_DMA_DATA_TYPE(dmaDataCtrl));

  if(dmaCtrl == QSPI_DMA_READ)
  {
    if(dmaDataCtrl == QSPI_DMA_DISABLE)
    {
      QSPI1->CONF2.BF.DMA_RD_EN = 0;
    }
    else
    {
      QSPI1->CONF2.BF.DMA_RD_EN = 1;
      QSPI1->CONF2.BF.DMA_RD_BURST = dmaDataCtrl;
    }
  }
  else
  {
    if(dmaDataCtrl == QSPI_DMA_DISABLE)
    {
      QSPI1->CONF2.BF.DMA_WR_EN = 0;
    }
    else
    {
      QSPI1->CONF2.BF.DMA_WR_EN = 1;
      QSPI1->CONF2.BF.DMA_WR_BURST = dmaDataCtrl;
    }
  }
}

void QSPI1_Init_CLK()
{
       QSPI1->CONF.BF.CLK_PRESCALE = 0x0;
}

/*@} end of group QSPI1_Public_Functions */

/*@} end of group QSPI1_definitions */

/*@} end of group MC200_Periph_Driver */
