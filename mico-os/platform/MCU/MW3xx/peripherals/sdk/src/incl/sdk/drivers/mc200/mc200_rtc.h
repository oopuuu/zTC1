/****************************************************************************//**
 * @file     mc200_rtc.h
 * @brief    RTC driver module header file.
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

#ifndef __MC200_RTC_H__
#define __MC200_RTC_H__

#include "mc200.h"
#include "mc200_driver.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @addtogroup  RTC 
 *  @{
 */
  
/** @defgroup RTC_Public_Types RTC_Public_Types
 *  @brief RTC configuration structure type definition
 *  @{
 */


/** 
 *  @brief RTC counter value register update mode type definition   
 */
typedef enum
{
  RTC_CNT_VAL_UPDATE_OFF       = 0,   /*!< Counter value register update mode: off */
  RTC_CNT_VAL_UPDATE_AUTO      = 2,   /*!< Counter value register update mode: Auto  */
}RTC_CntValUpdateMode_Type;

/**      
 *  @brief RTC config struct type definition  
 */
typedef struct
{
  RTC_CntValUpdateMode_Type CntValUpdateMode;      /*!< Counter value register update mode:
                                                        RTC_CNT_VAL_UPDATE_OFF (0):  Off
                                                        RTC_CNT_VAL_UPDATE_AUTO (2): Auto   */
  
                                            
  uint32_t clockDivider;                          /*!< Clock divider value(range: 0~15).
                                                       The divided clock is calculated by:
                                                       CLK_div = CLK / (2^clockDivider)*/  
  
  uint32_t uppVal;                                /*!< Counter overflow value */

}RTC_Config_Type;



/*@} end of group RTC_Public_Types definitions */


/** @defgroup RTC_Public_Constants
 *  @{
 */ 

#define RTC_INT_CNT_UPP   0

/*@} end of group RTC_Public_Constants */

/** @defgroup RTC_Public_Macro
 *  @{
 */

/*@} end of group RTC_Public_Macro */


/** @defgroup RTC_Public_FunctionDeclaration
 *  @brief RTC functions statement
 *  @{
 */
void RTC_Init(RTC_Config_Type* rtcConfig);

Status RTC_CounterReset(void);
FunctionalState RTC_GetCntStatus(void);
void RTC_Start(void);
void RTC_Stop(void);

void RTC_SetCounterUppVal( uint32_t uppVal);
uint32_t RTC_GetCounterUppVal(void);
uint32_t RTC_GetCounterVal(void);

void RTC_IntMask(IntMask_Type intMsk);
FlagStatus RTC_GetStatus(void);
void RTC_IntClr(void);

void RTC_IRQHandler(void);

/*@} end of group RTC_Public_FunctionDeclaration */

/*@} end of group RTC  */

/*@} end of group MC200_Periph_Driver */
#endif /* __MC200_RTC_H__ */
 

