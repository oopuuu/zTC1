/****************************************************************************//**
 * @file     mc200_qspi0.h
 * @brief    QSPI0 driver module header file.
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

#ifndef __MC200_QSPI0_H__
#define __MC200_QSPI0_H__

#include "mc200.h"
#include "mc200_driver.h"
#include "mc200_qspi1.h"

/** @addtogroup MC200_Periph_Driver
 *  @{
 */

/** @addtogroup QSPI0 
 *  @{
 */

/** @defgroup QSPI0_Public_Types QSPI0_Public_Types
 *  @{
 */

/*@} end of group QSPI0_Public_Types definitions */

/** @defgroup QSPI0_Public_Constants
 *  @{
 */ 

/*@} end of group QSPI0_Public_Constants */

/** @defgroup QSPI0_Public_Macro
 *  @{
 */

/*@} end of group QSPI0_Public_Macro */

/** @defgroup QSPI0_Public_FunctionDeclaration
 *  @brief QSPI0 functions statement
 *  @{
 */
void QSPI0_Reset(void);
void QSPI0_Init(QSPI_CFG_Type* qspiConfigSet);

void QSPI0_IntMask(QSPI_INT_Type intType, IntMask_Type intMask);
void QSPI0_IntClr(void);

Status QSPI0_FlushFIFO(void);

void QSPI0_SetHdrcnt(QSPI_INSTR_CNT_TYPE instrCnt, QSPI_ADDR_CNT_TYPE addrCnt, QSPI_RM_CNT_TYPE rmCnt, QSPI_DUMMY_CNT_TYPE dummyCnt);
void QSPI0_SetInstrCnt(QSPI_INSTR_CNT_TYPE instrCnt);
void QSPI0_SetAddrCnt(QSPI_ADDR_CNT_TYPE addrCnt);
void QSPI0_SetRModeCnt(QSPI_RM_CNT_TYPE rmCnt);
void QSPI0_SetDummyCnt(QSPI_DUMMY_CNT_TYPE dummyCnt);

void QSPI0_SetInstr(uint32_t instruct);
void QSPI0_SetAddr(uint32_t address);
void QSPI0_SetRMode(uint32_t readMode);
void QSPI0_SetDInCnt(uint32_t count);

void QSPI0_SetSSEnable(FunctionalState newCmd);

void QSPI0_StartTransfer(QSPI_RW_Type rw);
void QSPI0_StopTransfer(void); 

void QSPI0_WriteByte(uint8_t byte);
uint8_t QSPI0_ReadByte(void);
void QSPI0_WriteWord(uint32_t word);
uint32_t QSPI0_ReadWord(void);

FlagStatus QSPI0_GetStatus(QSPI_Status_Type qspiStatus);
FlagStatus QSPI0_GetIntStatus(QSPI_INT_Type qspiIntStatus);

FlagStatus QSPI0_IsTransferCompleted(void);

void QSPI0_DmaCmd(QSPI_DMA_Type dmaCtrl, QSPI_DMA_Data_Type dmaDataCtrl);

void QSPI0_Init_CLK();

/*@} end of group QSPI0_Public_FunctionDeclaration */

/*@} end of group QSPI0 */

/*@} end of group MC200_Periph_Driver */
#endif /* __MC200_QSPI0_H__ */
