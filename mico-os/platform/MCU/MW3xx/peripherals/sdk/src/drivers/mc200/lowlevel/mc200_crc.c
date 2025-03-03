/****************************************************************************//**
 * @file     mc200_crc.c
 * @brief    This file provides CRC functions.
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

#include "mc200.h"
#include "mc200_crc.h"
#include "mc200_driver.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @defgroup CRC CRC
 *  @brief CRC driver modules
 *  @{
 */

/** @defgroup CRC_Private_Type
 *  @{
 */

/*@} end of group CRC_Private_Type*/

/** @defgroup CRC_Private_Defines
 *  @{
 */


/*@} end of group CRC_Private_Defines */

                                 
/** @defgroup CRC_Private_Variables
 *  @{
 */


/*@} end of group CRC_Private_Variables */

/** @defgroup CRC_Global_Variables
 *  @{
 */


/*@} end of group CRC_Global_Variables */


/** @defgroup CRC_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group CRC_Private_FunctionDeclaration */

/** @defgroup CRC_Private_Functions
 *  @{
 */


/*@} end of group CRC_Private_Functions */

/** @defgroup CRC_Public_Functions
 *  @{
 */
 
/****************************************************************************//**
 * @brief      Set CRC mode 
 *
 * @param[in]  mode:  CRC mode
 *
 * @return none
 *
 *******************************************************************************/
void CRC_SetMode(CRC_Mode_Type mode)
{
  /* set CRC mode */
  CRC->CTRL.BF.MODE = mode;
}

/****************************************************************************//**
 * @brief      Enable CRC   
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CRC_Enable(void)
{
  /* enable CRC */
  CRC->CTRL.BF.ENABLE = 1;
}

/****************************************************************************//**
 * @brief      Disable CRC   
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CRC_Disable(void)
{
  /* disable CRC */
  CRC->CTRL.BF.ENABLE = 0;
}

/****************************************************************************//**
 * @brief      Feed data in CRC stream 
 *
 * @param[in]  data:  input data
 *
 * @return none
 *
 *******************************************************************************/
void CRC_FeedData(uint32_t data)
{
  /* feed data in CRC stream */
  CRC->STREAM_IN.WORDVAL = data;
}

/****************************************************************************//**
 * @brief      Set CRC stream length  
 *
 * @param[in]  strLen:  stream length in byte
 *
 * @return none
 *
 *******************************************************************************/
void CRC_SetStreamLen(uint32_t strLen)
{
  /* set CRC stream length */
  CRC->STREAM_LEN_M1.BF.LENGTH_M1 = strLen - 1;
}

/****************************************************************************//**
 * @brief      Get CRC result  
 *
 * @param  none
 *
 * @return CRC calculation result
 *
 *******************************************************************************/
uint32_t CRC_GetResult(void)
{
  /* return CRC result value */
  return CRC->RESULT.WORDVAL;
}

/****************************************************************************//**
 * @brief      Make input data stream 4 bytes aligned and feed data in CRC 
 *
 * @param[in]  dataStr:  input data stream
 * @param[in]  dataLen:  data length in byte
 *
 * @return CRC result
 *
 *******************************************************************************/
uint32_t CRC_Calculate(const uint8_t * dataStr, uint32_t dataLen)
{
  uint32_t cnt;
  
  uint32_t divisor, tail;

  /* set data length */
  CRC->STREAM_LEN_M1.BF.LENGTH_M1 = dataLen - 1;
	
  /* Padding first  and copy data in */
  divisor = dataLen / 4;
  tail = dataLen % 4;

  switch(tail)
  {
  	case 1:
        CRC->STREAM_IN.WORDVAL = (((uint32_t)dataStr[0]) << 24);
        break;
		
    case 2:
        CRC->STREAM_IN.WORDVAL = ((((uint32_t)dataStr[0]) << 16) |
                                  (((uint32_t)dataStr[1]) << 24));
        break;

    case 3:
        CRC->STREAM_IN.WORDVAL = ((((uint32_t)dataStr[0]) << 8) |
                                  (((uint32_t)dataStr[1]) << 16) |
                                  (((uint32_t)dataStr[2]) << 24));
        break;

    default:
        break;
  }

  /* compose last data from byte to word and copy data in  */
  for(cnt = 0; cnt < divisor; cnt++)
  {
    CRC->STREAM_IN.WORDVAL = (((uint32_t)dataStr[4*cnt + tail]) |
                              (((uint32_t)dataStr[4*cnt + tail + 1]) << 8) |
                              (((uint32_t)dataStr[4*cnt + tail + 2]) << 16) |
                              (((uint32_t)dataStr[4*cnt + tail + 3]) << 24));
  }
  
  /* wait for the result */
	while (CRC->IRSR.BF.STATUS_RAW == 0)
		;
  
  /* clear interrupt flag */
  CRC->ICR.BF.CLEAR  = 1;
  
  /* return the result */
  return CRC->RESULT.WORDVAL;
}

/****************************************************************************//**
 * @brief      Feed the stream data.
 *
 * @param[in]  dataStr:  input data stream
 * @param[in]  dataLen:  data length in byte
 *
 * @note This function is added specifically for this SDK. Please keep it
 * accross Saratoga driver updates.
 *
 * @return void
 *
 *******************************************************************************/
void CRC_Stream_Feed(const uint8_t * dataStr, uint32_t dataLen)
{
  uint32_t cnt;
  
  uint32_t divisor, tail;

  /* Padding first  and copy data in */
  divisor = dataLen / 4;
  tail = dataLen % 4;

  switch(tail)
  {
  	case 1:
        CRC->STREAM_IN.WORDVAL = (((uint32_t)dataStr[0]) << 24);
        break;
		
    case 2:
        CRC->STREAM_IN.WORDVAL = ((((uint32_t)dataStr[0]) << 16) |
                                  (((uint32_t)dataStr[1]) << 24));
        break;

    case 3:
        CRC->STREAM_IN.WORDVAL = ((((uint32_t)dataStr[0]) << 8) |
                                  (((uint32_t)dataStr[1]) << 16) |
                                  (((uint32_t)dataStr[2]) << 24));
        break;

    default:
        break;
  }

  /* compose last data from byte to word and copy data in  */
  for(cnt = 0; cnt < divisor; cnt++)
  {
    CRC->STREAM_IN.WORDVAL = (((uint32_t)dataStr[4*cnt + tail]) |
                              (((uint32_t)dataStr[4*cnt + tail + 1]) << 8) |
                              (((uint32_t)dataStr[4*cnt + tail + 2]) << 16) |
                              (((uint32_t)dataStr[4*cnt + tail + 3]) << 24));
  }
}

/****************************************************************************//**
 * @brief      Mask / Unmask CRC interrupt 
 *
 * @param[in]  newState:  Mask / Unmask control
 *
 * @return none
 *
 *******************************************************************************/
void CRC_IntMask(IntMask_Type newState)
{
  /* mask/unmask interrupt */
  CRC->IMR.BF.MASK = ((newState == MASK) ? 1 : 0);
}

/****************************************************************************//**
 * @brief      Clear CRC interrupt flag
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CRC_IntClr(void)
{
  /* clear interrupt flag */
  CRC->ICR.BF.CLEAR = 1;
}

/****************************************************************************//**
 * @brief      Get CRC interrupt status
 *
 * @param  none
 *
 * @return interrupt status
 *
 *******************************************************************************/
IntStatus CRC_GetIntStatus(void)
{
  /* get interrupt status */
  return (IntStatus)CRC->ISR.BF.STATUS;
}

/****************************************************************************//**
 * @brief      Get CRC calculation finish status
 *
 * @param  none
 *
 * @return CRC status
 *
 *******************************************************************************/
FlagStatus CRC_GetStatus(void)
{
  return (FlagStatus)CRC->IRSR.BF.STATUS_RAW;
}

/****************************************************************************//**
 * @brief  CRC interrupt function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void CRC_IRQHandler(void)
{
  /* clear interrupt status */
  CRC->ICR.BF.CLEAR = 1;
  
  /* check call back function availability */
  if(intCbfArra[INT_CRC][0] != NULL)
  {
    /* call the call back function */
    intCbfArra[INT_CRC][0]();
  }
  else
  {
    /* mask the interrupt if call back function not available */
    CRC->IMR.BF.MASK = 1;
  }
}

/*@} end of group CRC_Public_Functions */

/*@} end of group CRC  */

/*@} end of group MC200_Periph_Driver */

