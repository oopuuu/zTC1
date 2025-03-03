/****************************************************************************//**
 * @file     mw300_bg.h
 * @brief    BG driver module header file.
 * @version  V1.3.0
 * @date     12-Aug-2013
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

#ifndef __MW300_BG_H__
#define __MW300_BG_H__

/* Includes ------------------------------------------------------------------ */
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  BG 
 *  @{
 */
  
/** @defgroup BG_Public_Types BG_Public_Types
 *  @{
 */

/*@} end of group BG_Public_Types definitions */

/** @defgroup BG_Public_Constants
 *  @{
 */ 

/*@} end of group BG_Public_Constants */

/** @defgroup BG_Public_Macro
 *  @{
 */

/*@} end of group BG_Public_Macro */

/** @defgroup BG_Public_FunctionDeclaration
 *  @brief BG functions statement
 *  @{
 */
void BG_PowerUp(void);
void BG_PowerDown(void);

/*@} end of group BG_Public_FunctionDeclaration */

/*@} end of group BG */

/*@} end of group MW300_Periph_Driver */
#endif

