/****************************************************************************//**
 * @file     mw300_wdt.h
 * @brief    WDT driver module header file.
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

#ifndef __MW300_WDT_H__
#define __MW300_WDT_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  WDT 
 *  @{
 */
  
/** @defgroup WDT_Public_Types WDT_Public_Types
 *  @brief WDT configuration structure type definition
 *  @{
 */
 
/**  
 *  @brief WDT response mode type definition 
 */
typedef enum
{
  WDT_MODE_RESET = 0,                    /*!< WDT resposne mode: system reset */
  WDT_MODE_INT = 1,                      /*!< WDT response mode: generate interrupt */
}WDT_Mode_Type;

/**  
 *  @brief WDT reset pulse length type definition 
 */
typedef enum
{
  WDT_RESET_PULSE_LEN_2   = 0,           /*!< WDT reset pulse length: 2 pclk */
  WDT_RESET_PULSE_LEN_4   = 1,           /*!< WDT reset pulse length: 4 pclk */
  WDT_RESET_PULSE_LEN_8   = 2,           /*!< WDT reset pulse length: 8 pclk */
  WDT_RESET_PULSE_LEN_16  = 3,           /*!< WDT reset pulse length: 16 pclk */
  WDT_RESET_PULSE_LEN_32  = 4,           /*!< WDT reset pulse length: 32 pclk */
  WDT_RESET_PULSE_LEN_64  = 5,           /*!< WDT reset pulse length: 64 pclk */
  WDT_RESET_PULSE_LEN_128 = 6,           /*!< WDT reset pulse length: 128 pclk */
  WDT_RESET_PULSE_LEN_256 = 7,           /*!< WDT reset pulse length: 256 pclk */
}WDT_ResetPulseLen_Type;

/**      
 *  @brief WDT config struct type definition  
 */
typedef struct
{
  uint32_t timeoutVal;                   /*!< WDT timeout value */

  WDT_Mode_Type mode;                    /*!< WDT working mode */
  
  WDT_ResetPulseLen_Type resetPulseLen;  /*!< WDT reset pulse length */
  
}WDT_Config_Type;

/*@} end of group WDT_Public_Types definitions */


/** @defgroup WDT_Public_Constants
 *  @{
 */ 


/*@} end of group WDT_Public_Constants */

/** @defgroup WDT_Public_Macro
 *  @{
 */


/*@} end of group WDT_Public_Macro */

/** @defgroup WDT_Public_FunctionDeclaration
 *  @brief WDT functions statement
 *  @{
 */

void WDT_Init(WDT_Config_Type* wdtConfig);
void WDT_Enable(void);
void WDT_Disable(void);

void WDT_SetMode(WDT_Mode_Type mode);
void WDT_SetTimeoutVal(uint32_t timeoutVal);
void WDT_SetResetPulseLen(WDT_ResetPulseLen_Type resetPulseLen);
uint32_t WDT_GetCounterVal(void);
void WDT_RestartCounter(void); 

IntStatus WDT_GetIntStatus(void);
void WDT_IntClr(void);

void WDT_IRQHandler(void);

/*@} end of group WDT_Public_FunctionDeclaration */

/*@} end of group WDT  */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_WDT_H__ */

