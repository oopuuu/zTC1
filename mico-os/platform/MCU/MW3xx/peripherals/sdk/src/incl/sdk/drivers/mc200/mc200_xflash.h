/****************************************************************************//**
 * @file     mc200_xflash.h
 * @brief    XFLASH driver module header file.
 * @version  V1.0.0
 * @date     18-April-2012
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

#ifndef __MC200_XFLASH_H
#define __MC200_XFLASH_H

#include "mc200.h"
#include "mc200_driver.h"

/** @addtogroup MC200_Periph_Driver
 *  @{
 */

/** @addtogroup XFLASH 
 *  @{
 */
  
/** @defgroup XFLASH_Public_Types XFLASH_Public_Types
 *  @{
 */

/**  
 *  @brief XFLASH interface type
 */
typedef enum
{
  XFLASH_INTERFACE_QSPI1,        /*!< QSPI1 is selected as the interface to access the flash */     
}XFLASH_Interface_Type;

/**  
 *  @brief XFLASH protection type
 */ 
typedef enum
{
  XFLASH_PROT_NONE         = 0x00,      /*!< None protection */  
  XFLASH_PROT_UPPER_64KB   = 0x04,      /*!< Protect upper 64KB   0x0F0000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_128KB  = 0x08,      /*!< Protect upper 128KB  0x0E0000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_256KB  = 0x0C,      /*!< Protect upper 256KB  0x0C0000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_512KB  = 0x10,      /*!< Protect upper 512KB  0x080000 ~ 0x0FFFFF */
  XFLASH_PROT_LOWER_64KB   = 0x24,      /*!< Protect lower 64KB   0x000000 ~ 0x00FFFF */
  XFLASH_PROT_LOWER_128KB  = 0x28,      /*!< Protect lower 128KB  0x000000 ~ 0x01FFFF */
  XFLASH_PROT_LOWER_256KB  = 0x2C,      /*!< Protect lower 256KB  0x000000 ~ 0x03FFFF */
  XFLASH_PROT_LOWER_512KB  = 0x30,      /*!< Protect lower 512KB  0x000000 ~ 0x07FFFF */
  XFLASH_PROT_ALL          = 0x14,      /*!< Protect all          0x000000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_4KB    = 0x44,      /*!< Protect upper 4KB    0x0FF000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_8KB    = 0x48,      /*!< Protect upper 8KB    0x0FE000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_16KB   = 0x4C,      /*!< Protect upper 16KB   0x0FC000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_32KB   = 0x50,      /*!< Protect upper 32KB   0x0F8000 ~ 0x0FFFFF */
  XFLASH_PROT_LOWER_4KB    = 0x64,      /*!< Protect lower 4KB    0x000000 ~ 0x000FFF */
  XFLASH_PROT_LOWER_8KB    = 0x68,      /*!< Protect lower 8KB    0x000000 ~ 0x001FFF */
  XFLASH_PROT_LOWER_16KB   = 0x6C,      /*!< Protect lower 16KB   0x000000 ~ 0x003FFF */
  XFLASH_PROT_LOWER_32KB   = 0x70,      /*!< Protect lower 32KB   0x000000 ~ 0x007FFF */
  XFLASH_PROT_LOWER_960KB  = 0x84,      /*!< Protect lower 960KB  0x000000 ~ 0x0EFFFF */
  XFLASH_PROT_LOWER_896KB  = 0x88,      /*!< Protect lower 896KB  0x000000 ~ 0x0DFFFF */
  XFLASH_PROT_LOWER_768KB  = 0x8C,      /*!< Protect lower 960KB  0x000000 ~ 0x0BFFFF */
  XFLASH_PROT_UPPER_960KB  = 0xA4,      /*!< Protect upper 960KB  0x010000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_896KB  = 0xA8,      /*!< Protect upper 896KB  0x020000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_768KB  = 0xAC,      /*!< Protect upper 768KB  0x040000 ~ 0x0FFFFF */
  XFLASH_PROT_LOWER_1020KB = 0xC4,      /*!< Protect lower 1020KB 0x000000 ~ 0x0FEFFF */
  XFLASH_PROT_LOWER_1016KB = 0xC8,      /*!< Protect lower 1016KB 0x000000 ~ 0x0FDFFF */
  XFLASH_PROT_LOWER_1008KB = 0xCC,      /*!< Protect lower 1008KB 0x000000 ~ 0x0FBFFF */
  XFLASH_PROT_LOWER_992KB  = 0xD0,      /*!< Protect lower 992KB  0x000000 ~ 0x0F7FFF */
  XFLASH_PROT_UPPER_1020KB = 0xE4,      /*!< Protect upper 1020KB 0x001000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_1016KB = 0xE8,      /*!< Protect upper 1016KB 0x002000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_1008KB = 0xEC,      /*!< Protect upper 1008KB 0x004000 ~ 0x0FFFFF */
  XFLASH_PROT_UPPER_992KB  = 0xF0,      /*!< Protect upper 992KB  0x008000 ~ 0x0FFFFF */
 }XFLASH_Protection_Type; 

/**  
 *  @brief XFLASH read mode
 */ 
typedef enum
{
  XFLASH_NORMAL_READ,                  /*!< Normal read mode */
  XFLASH_FAST_READ,                    /*!< Fast read mode */
  XFLASH_FAST_READ_DUAL_OUT,           /*!< Fast read dual output mode */
  XFLASH_FAST_READ_DUAL_IO,            /*!< Fast read dual IO mode */
  XFLASH_FAST_READ_QUAD_OUT,           /*!< Fast read quad output mode */
  XFLASH_FAST_READ_QUAD_IO,            /*!< Fast read quad IO mode */
  XFLASH_WORD_FAST_READ_QUAD_IO,       /*!< Word fast read quad IO mode */
  XFLASH_OCTAL_WORD_FAST_READ_QUAD_IO, /*!< Octal word fast read quad IO mode */
}XFLASH_ReadMode_Type; 

/**  
 *  @brief XFLASH program mode
 */ 
typedef enum
{
  XFLASH_PROGRAM_NORMAL,               /*!< Normal page program mode */
  XFLASH_PROGRAM_QUAD,                 /*!< Quad page program mode   */
}XFLASH_ProgramMode_Type; 

/*@} end of group XFLASH_Public_Types definitions */

/** @defgroup XFLASH_CONSTANTS
 *  @{
 */
#define XFLASH_PAGE_SIZE            0x100    /*!< 256 bytes */
#define XFLASH_SECTOR_SIZE          0x1000   /*!< 4KB */
#define XFLASH_32K_BLOCK_SIZE       0x8000   /*!< 32KB */
#define XFLASH_64K_BLOCK_SIZE       0x10000  /*!< 64KB */
#define XFLASH_CHIP_SIZE            0x100000 /*!< 1MB */
#define XFLASH_LAST_SECTOR          ((XFLASH_CHIP_SIZE/XFLASH_SECTOR_SIZE) - 1)
#define XFLASH_LAST_32K_BLOCK       ((XFLASH_CHIP_SIZE/XFLASH_32K_BLOCK_SIZE) - 1)
#define XFLASH_LAST_64K_BLOCK       ((XFLASH_CHIP_SIZE/XFLASH_64K_BLOCK_SIZE) - 1)

/*@} end of group XFLASH_CONSTANTS */

/** @defgroup XFLASH_Public_Constants
 *  @{
 */ 

/*@} end of group XFLASH_Public_Constants */

/** @defgroup XFLASH_Public_Macro
 *  @{
 */

/*@} end of group XFLASH_Public_Macro */

/** @defgroup XFLASH_Public_FunctionDeclaration
 *  @brief XFLASH functions statement
 *  @{
 */

void XFLASH_SelectInterface(XFLASH_Interface_Type interface);

void XFLASH_PowerDown(FunctionalState newCmd);
Status XFLASH_SetProtectionMode(XFLASH_Protection_Type protectMode);

Status XFLASH_EraseAll(void);
Status XFLASH_SectorErase(uint32_t sectorNumber);
Status XFLASH_Block32KErase(uint32_t blockNumber);
Status XFLASH_Block64KErase(uint32_t blockNumber);
Status XFLASH_Erase(uint32_t startAddr, uint32_t endAddr);

uint32_t XFLASH_Read(XFLASH_ReadMode_Type readMode, uint32_t address, uint8_t *buffer, uint32_t num);
uint32_t XFLASH_WordRead(XFLASH_ReadMode_Type readMode, uint32_t address);
uint8_t XFLASH_ByteRead(XFLASH_ReadMode_Type readMode, uint32_t address);

Status XFLASH_PageWrite(XFLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num);
Status XFLASH_Write(XFLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num);
Status XFLASH_WordWrite(XFLASH_ProgramMode_Type programMode, uint32_t address, uint32_t data);
Status XFLASH_ByteWrite(XFLASH_ProgramMode_Type programMode, uint32_t address, uint8_t data);

uint64_t XFLASH_GetUniqueID(void);
/*@} end of group XFLASH_Public_FunctionDeclaration */

/*@} end of group XFLASH */

/*@} end of group MC200_Periph_Driver */
#endif /* __MC200_XFLASH_H__ */
