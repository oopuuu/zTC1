/****************************************************************************//**
 * @file     mc200_flash.h
 * @brief    FLASH driver module header file.
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

#ifndef __MC200_FLASH_H
#define __MC200_FLASH_H

#include "mc200.h"
#include "mc200_driver.h"

/** @addtogroup MC200_Periph_Driver
 *  @{
 */

/** @addtogroup FLASH 
 *  @{
 */
  
/** @defgroup FLASH_Public_Types FLASH_Public_Types
 *  @{
 */

/**  
 *  @brief FLASH interface type
 */
typedef enum
{
  FLASH_INTERFACE_QSPI0,        /*!< QSPI0 is selected as the interface to access the flash */     
  FLASH_INTERFACE_FLASHC        /*!< Flash controller is selected as the interface to the flash */
}FLASH_Interface_Type;

/**  
 *  @brief FLASH protection type
 */ 
typedef enum
{
  FLASH_PROT_NONE         = 0x00,      /*!< None protection */  
  FLASH_PROT_UPPER_64KB   = 0x04,      /*!< Protect upper 64KB   0x0F0000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_128KB  = 0x08,      /*!< Protect upper 128KB  0x0E0000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_256KB  = 0x0C,      /*!< Protect upper 256KB  0x0C0000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_512KB  = 0x10,      /*!< Protect upper 512KB  0x080000 ~ 0x0FFFFF */
  FLASH_PROT_LOWER_64KB   = 0x24,      /*!< Protect lower 64KB   0x000000 ~ 0x00FFFF */
  FLASH_PROT_LOWER_128KB  = 0x28,      /*!< Protect lower 128KB  0x000000 ~ 0x01FFFF */
  FLASH_PROT_LOWER_256KB  = 0x2C,      /*!< Protect lower 256KB  0x000000 ~ 0x03FFFF */
  FLASH_PROT_LOWER_512KB  = 0x30,      /*!< Protect lower 512KB  0x000000 ~ 0x07FFFF */
  FLASH_PROT_ALL          = 0x14,      /*!< Protect all          0x000000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_4KB    = 0x44,      /*!< Protect upper 4KB    0x0FF000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_8KB    = 0x48,      /*!< Protect upper 8KB    0x0FE000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_16KB   = 0x4C,      /*!< Protect upper 16KB   0x0FC000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_32KB   = 0x50,      /*!< Protect upper 32KB   0x0F8000 ~ 0x0FFFFF */
  FLASH_PROT_LOWER_4KB    = 0x64,      /*!< Protect lower 4KB    0x000000 ~ 0x000FFF */
  FLASH_PROT_LOWER_8KB    = 0x68,      /*!< Protect lower 8KB    0x000000 ~ 0x001FFF */
  FLASH_PROT_LOWER_16KB   = 0x6C,      /*!< Protect lower 16KB   0x000000 ~ 0x003FFF */
  FLASH_PROT_LOWER_32KB   = 0x70,      /*!< Protect lower 32KB   0x000000 ~ 0x007FFF */
  FLASH_PROT_LOWER_960KB  = 0x84,      /*!< Protect lower 960KB  0x000000 ~ 0x0EFFFF */
  FLASH_PROT_LOWER_896KB  = 0x88,      /*!< Protect lower 896KB  0x000000 ~ 0x0DFFFF */
  FLASH_PROT_LOWER_768KB  = 0x8C,      /*!< Protect lower 960KB  0x000000 ~ 0x0BFFFF */
  FLASH_PROT_UPPER_960KB  = 0xA4,      /*!< Protect upper 960KB  0x010000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_896KB  = 0xA8,      /*!< Protect upper 896KB  0x020000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_768KB  = 0xAC,      /*!< Protect upper 768KB  0x040000 ~ 0x0FFFFF */
  FLASH_PROT_LOWER_1020KB = 0xC4,      /*!< Protect lower 1020KB 0x000000 ~ 0x0FEFFF */
  FLASH_PROT_LOWER_1016KB = 0xC8,      /*!< Protect lower 1016KB 0x000000 ~ 0x0FDFFF */
  FLASH_PROT_LOWER_1008KB = 0xCC,      /*!< Protect lower 1008KB 0x000000 ~ 0x0FBFFF */
  FLASH_PROT_LOWER_992KB  = 0xD0,      /*!< Protect lower 992KB  0x000000 ~ 0x0F7FFF */
  FLASH_PROT_UPPER_1020KB = 0xE4,      /*!< Protect upper 1020KB 0x001000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_1016KB = 0xE8,      /*!< Protect upper 1016KB 0x002000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_1008KB = 0xEC,      /*!< Protect upper 1008KB 0x004000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_992KB  = 0xF0,      /*!< Protect upper 992KB  0x008000 ~ 0x0FFFFF */
 }FLASH_Protection_Type; 

/**  
 *  @brief FLASH read mode
 */ 
typedef enum
{
  FLASH_NORMAL_READ,                  /*!< Normal read mode */
  FLASH_FAST_READ,                    /*!< Fast read mode */
  FLASH_FAST_READ_DUAL_OUT,           /*!< Fast read dual output mode */
  FLASH_FAST_READ_DUAL_IO,            /*!< Fast read dual IO mode */
  FLASH_FAST_READ_QUAD_OUT,           /*!< Fast read quad output mode */
  FLASH_FAST_READ_QUAD_IO,            /*!< Fast read quad IO mode */
  FLASH_WORD_FAST_READ_QUAD_IO,       /*!< Word fast read quad IO mode */
  FLASH_OCTAL_WORD_FAST_READ_QUAD_IO, /*!< Octal word fast read quad IO mode */
}FLASH_ReadMode_Type; 

/**  
 *  @brief FLASH program mode
 */ 
typedef enum
{
  FLASH_PROGRAM_NORMAL,               /*!< Normal page program mode */
  FLASH_PROGRAM_QUAD,                 /*!< Quad page program mode   */
}FLASH_ProgramMode_Type; 

/*@} end of group FLASH_Public_Types definitions */

/** @defgroup FLASH_CONSTANTS
 *  @{
 */
#define FLASH_PAGE_SIZE            0x100    /*!< 256 bytes */
#define FLASH_SECTOR_SIZE          0x1000   /*!< 4KB */
#define FLASH_32K_BLOCK_SIZE       0x8000   /*!< 32KB */
#define FLASH_64K_BLOCK_SIZE       0x10000  /*!< 64KB */
#define FLASH_CHIP_SIZE            0x100000 /*!< 1MB */
#define FLASH_LAST_SECTOR          ((FLASH_CHIP_SIZE/FLASH_SECTOR_SIZE) - 1)
#define FLASH_LAST_32K_BLOCK       ((FLASH_CHIP_SIZE/FLASH_32K_BLOCK_SIZE) - 1)
#define FLASH_LAST_64K_BLOCK       ((FLASH_CHIP_SIZE/FLASH_64K_BLOCK_SIZE) - 1)

/*@} end of group FLASH_CONSTANTS */

/** @defgroup FLASH_Public_Constants
 *  @{
 */ 

/*@} end of group FLASH_Public_Constants */

/** @defgroup FLASH_Public_Macro
 *  @{
 */

/*@} end of group FLASH_Public_Macro */

/** @defgroup FLASH_Public_FunctionDeclaration
 *  @brief FLASH functions statement
 *  @{
 */

void FLASH_SelectInterface(FLASH_Interface_Type interface);
FLASH_Interface_Type FLASH_GetInterface(void);

FlagStatus FLASH_GetBusyStatus(void);

Status FLASH_SetProtectionMode(FLASH_Protection_Type protectMode);

Status FLASH_EraseAll(void);
Status FLASH_SectorErase(uint32_t sectorNumber);
Status FLASH_Block32KErase(uint32_t blockNumber);
Status FLASH_Block64KErase(uint32_t blockNumber);
Status FLASH_Erase(uint32_t startAddr, uint32_t endAddr);

uint32_t FLASH_Read(FLASH_ReadMode_Type readMode, uint32_t address, uint8_t *buffer, uint32_t num);
uint32_t FLASH_WordRead(FLASH_ReadMode_Type readMode, uint32_t address);
uint8_t FLASH_ByteRead(FLASH_ReadMode_Type readMode, uint32_t address);

Status FLASH_PageWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num);
Status FLASH_Write(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num);
Status FLASH_WordWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint32_t data);
Status FLASH_ByteWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t data);

uint64_t FLASH_GetUniqueID(void);

/*@} end of group FLASH_Public_FunctionDeclaration */

/*@} end of group FLASH */

/*@} end of group MC200_Periph_Driver */
#endif /* __MC200_FLASH_H__ */

