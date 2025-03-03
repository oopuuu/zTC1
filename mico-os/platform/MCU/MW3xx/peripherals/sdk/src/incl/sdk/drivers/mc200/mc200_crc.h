/****************************************************************************//**
 * @file     mc200_crc.h
 * @brief    CRC driver module header file.
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

#ifndef __MC200_CRC_H__
#define __MC200_CRC_H__

#include "mc200.h"
#include "mc200_driver.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @addtogroup  CRC 
 *  @{
 */
  
/** @defgroup CRC_Public_Types CRC_Public_Types
 *  @brief CRC configuration structure type definition
 *  @{
 */
 
/**  
 *  @brief CRC mode type definition 
 */
typedef enum
{
  CRC_16_CCITT   = 0,                  /*!< CRC mode: CRC-16-CCITT */
  CRC_16_IBM     = 1,                  /*!< CRC mode: CRC-16-IBM */
  CRC_16_T10_DIF = 2,                  /*!< CRC mode: CRC-16-T10-DIF */
  CRC_32_IEEE    = 3,                  /*!< CRC mode: CRC-32-IEEE */
  CRC_16_DNP     = 4,                  /*!< CRC mode: CRC-16-DNP */
}CRC_Mode_Type;

/*@} end of group CRC_Public_Types definitions */


/** @defgroup CRC_Public_Constants
 *  @{
 */ 


/*@} end of group CRC_Public_Constants */

/** @defgroup CRC_Public_Macro
 *  @{
 */


/*@} end of group CRC_Public_Macro */

/** @defgroup CRC_Public_Function_Declaration
 *  @brief CRC functions statement
 *  @{
 */

void CRC_SetMode(CRC_Mode_Type mode);
void CRC_Enable(void);
void CRC_Disable(void);
void CRC_FeedData(uint32_t data);
void CRC_SetStreamLen(uint32_t strLen);
uint32_t CRC_GetResult(void);
uint32_t CRC_Calculate(const uint8_t * dataStr, uint32_t dataLen);
void CRC_Stream_Feed(const uint8_t * dataStr, uint32_t dataLen);

void CRC_IntMask(IntMask_Type newState);
IntStatus CRC_GetIntStatus(void);
FlagStatus CRC_GetStatus(void);
void CRC_IntClr(void);

void CRC_IRQHandler(void);

/*@} end of group CRC_Public_Function_Declaration */

/*@} end of group CRC  */

/*@} end of group MC200_Periph_Driver */
#endif /* __MC200_CRC_H__ */

