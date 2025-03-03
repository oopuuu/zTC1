/****************************************************************************//**
 * @file     mc200_xflash.c
 * @brief    This file provides XFLASH functions.
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

#include "mc200.h"
#include "mc200_driver.h"
#include "mc200_xflash.h"
#include "mc200_qspi1.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @defgroup XFLASH XFLASH
 *  @brief XFLASH driver modules
 *  @{
 */

/** @defgroup XFLASH_Private_Type
 *  @{
 */

/*@} end of group XFLASH_Private_Type*/

/** @defgroup XFLASH_Private_Defines
 *  @{
 */

/**  
 *  @brief XFLASH status type
 */ 
typedef enum
{
  XFLASH_STATUS_LO,                    /*!< STATUS[7:0]  */
  XFLASH_STATUS_HI,                    /*!< STATUS[15:8] */
}XFLASH_Status_Type; 

/** @defgroup XFLASH_INSTRUCTIONS       
 *  @{
 */     
#define XFLASH_INS_CODE_WE      0x06   /*!< Write enable */
#define XFLASH_INS_CODE_WE_VSR  0x50   /*!< Write enable for volatile status register */
#define XFLASH_INS_CODE_WD      0x04   /*!< Write disable */
#define XFLASH_INS_CODE_RSR1    0x05   /*!< Read status register 1 */
#define XFLASH_INS_CODE_RSR2    0x35   /*!< Read status register 2 */
#define XFLASH_INS_CODE_WSR     0x01   /*!< Write status register */
#define XFLASH_INS_CODE_PP      0x02   /*!< Page program */
#define XFLASH_INS_CODE_QPP     0x32   /*!< Quad page program */
#define XFLASH_INS_CODE_SE      0x20   /*!< Sector(4k) erase */
#define XFLASH_INS_CODE_BE_32K  0x52   /*!< Block(32k) erase */
#define XFLASH_INS_CODE_BE_64K  0xD8   /*!< Block(64k) erase */
#define XFLASH_INS_CODE_CE      0xC7   /*!< Chip erase */
#define XFLASH_INS_CODE_PD      0xB9   /*!< Power down */

#define XFLASH_INS_CODE_RD      0x03   /*!< Read data */
#define XFLASH_INS_CODE_FR      0x0B   /*!< Fast read */
#define XFLASH_INS_CODE_FRDO    0x3B   /*!< Fast read dual output */ 
#define XFLASH_INS_CODE_FRQO    0x6B   /*!< Fast read quad output */ 
#define XFLASH_INS_CODE_FRDIO   0xBB   /*!< Fast read dual IO */
#define XFLASH_INS_CODE_FRQIO   0xEB   /*!< Fast read quad IO */
#define XFLASH_INS_CODE_WFRQIO  0xE7   /*!< Word Fast read quad IO, A0 must be zero */
#define XFLASH_INS_CODE_OWFRQIO 0xE3   /*!< Octal word Fast read quad IO, A[3:0] must be zero */

#define XFLASH_INS_CODE_RPD_DI  0xAB   /*!< Release power down or device ID */
#define XFLASH_INS_CODE_RUID    0x4B   /*!< Read unique ID number */

/*@} end of group XFLASH_INSTRUCTIONS */

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

/* Get the page number according to the address with the page space */
#define XFLASH_PAGE_NUM(addr)        ((addr)>>8)

/* Get the page begin and end address according to the address with the page space */
#define XFLASH_PAGE_BEGIN_ADDR(addr) (XFLASH_PAGE_SIZE * XFLASH_PAGE_NUM(addr))
#define XFLASH_PAGE_END_ADDR(addr)   (XFLASH_PAGE_SIZE * (XFLASH_PAGE_NUM(addr)+1) - 1)

/*@} end of group XFLASH_Private_Defines */

/** @defgroup XFLASH_Private_Variables
 *  @{
 */

/*@} end of group XFLASH_Private_Variables */

/** @defgroup XFLASH_Global_Variables
 *  @{
 */

/*@} end of group XFLASH_Global_Variables */

/** @defgroup XFLASH_Private_FunctionDeclaration
 *  @{
 */
void XFLASH_SetWriteEnableBit(FunctionalState newCmd);
void XFLASH_WriteEnableVSR(void);
FlagStatus XFLASH_GetBusyStatus(void);
uint8_t XFLASH_GetStatus(XFLASH_Status_Type statusIdx);
Status XFLASH_WriteStatus(uint16_t status);

Status XFLASH_IsSectorBlank(uint32_t sectorNumber);
Status XFLASH_IsBlank(void);

/*@} end of group XFLASH_Private_FunctionDeclaration */

/** @defgroup XFLASH_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Set flash write enable / disable 
 *
 * @param[in]  newCmd:  Enable/disable flash write
 *
 * @return none
 *
 *******************************************************************************/
void XFLASH_SetWriteEnableBit(FunctionalState newCmd) 
{
  /* Clear QSPI1 FIFO */ 
  QSPI1_FlushFIFO();
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI1_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_0BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
  
  /* Set data in counter */
  QSPI1_SetDInCnt(0);
  
  if(newCmd == ENABLE)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_WE);
  }
  else
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_WD);
  }
        
  /* Set QSPI1 write */
  QSPI1_StartTransfer(QSPI_W_EN);
  
  /* Stop QSPI1 transfer */
  QSPI1_StopTransfer();
}

/****************************************************************************//**
 * @brief      Write enable for volatile status register
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void XFLASH_WriteEnableVSR(void)
{
  /* Clear QSPI1 FIFO */ 
  QSPI1_FlushFIFO();
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI1_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_0BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
  
  /* Set data in counter */
  QSPI1_SetDInCnt(0);
  
  /* Set instruction */
  QSPI1_SetInstr(XFLASH_INS_CODE_WE_VSR);
  
  /* Set QSPI1 write */
  QSPI1_StartTransfer(QSPI_W_EN);
  
  /* Stop QSPI1 transfer */
  QSPI1_StopTransfer();  
}

/****************************************************************************//**
 * @brief      Get the flash busy status
 *
 * @param none
 *
 * @return     Flash busy status
 *
 *******************************************************************************/
FlagStatus XFLASH_GetBusyStatus(void)
{
  FlagStatus funcStatus;  
  
  /* Clear QSPI1 FIFO */ 
  QSPI1_FlushFIFO();  
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI1_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_0BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
    
  /* Set data in counter */
  QSPI1_SetDInCnt(1);
  
  /* Set QSPI1 data pin mode */
  QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE; 
  
  /* Set instruction */
  QSPI1_SetInstr(XFLASH_INS_CODE_RSR1);
  
  /* Set QSPI1 read */
  QSPI1_StartTransfer(QSPI_R_EN);
  
  /* Get flash busy status */
  funcStatus = (QSPI1_ReadByte() & 0x01) ? SET : RESET;
  
  /* Disable QSPI1 */
  QSPI1_SetSSEnable(DISABLE);  
  
  return funcStatus;
}


/****************************************************************************//**
 * @brief      Get the flash status
 *
* @param[in]   statusIdx:  Status[7:0] or Status[15:8]
 *
 * @return     Specified status
 *
 *******************************************************************************/
uint8_t XFLASH_GetStatus(XFLASH_Status_Type statusIdx)
{
  uint8_t status;
  
  /* Clear QSPI1 FIFO */ 
  QSPI1_FlushFIFO();  
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI1_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_0BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
  
  /* Set data in counter */
  QSPI1_SetDInCnt(1);
  
  /* Set QSPI1 data pin mode */
  QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE; 
  
  if(statusIdx == XFLASH_STATUS_LO)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_RSR1);
  }
  else if(statusIdx == XFLASH_STATUS_HI)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_RSR2);
  }
  
  /* Set QSPI1 read */
  QSPI1_StartTransfer(QSPI_R_EN);
  
  /* Get flash busy status */
  status = QSPI1_ReadByte();
  
  /* Disable QSPI1 */
  QSPI1_SetSSEnable(DISABLE);  
  
  return status;
}

/****************************************************************************//**
 * @brief      Write flash status register
 *
 * @param[in]  status:  status
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_WriteStatus(uint16_t status)
{
  volatile uint32_t localCnt = 0;
  uint8_t byte;
   
  /* Enable flash write */
  XFLASH_WriteEnableVSR();
  
  /* Set QSPI1 data pin mode */
  QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE; 
  
  /* Set instruction */
  QSPI1_SetInstr(XFLASH_INS_CODE_WSR);    
  
  /* Set QSPI1 write */
  QSPI1_StartTransfer(QSPI_W_EN);
  
  /* Write status[7:0] */
  byte = status & 0xFF;
  QSPI1_WriteByte(byte);
  
  /* Write status[15:8] */
  byte = (status >> 8) & 0xFF;
  QSPI1_WriteByte(byte);
  
  /* Stop QSPI1 transfer */
  QSPI1_StopTransfer();
  
  while(localCnt++ < 100000)
  {
    /* Check flash busy status */ 
    if( XFLASH_GetBusyStatus() == RESET )
    {
      return DSUCCESS;
    }
  }
  
  return DERROR;
}

/****************************************************************************//**
 * @brief      Check sector is blank or not
 *
 * @param[in]  sectorNumber:  Sector to be checked
 *
 * @return     Status
 *
 *******************************************************************************/
Status XFLASH_IsSectorBlank(uint32_t sectorNumber) 
{
  Status funcStatus = DSUCCESS;
  uint32_t sectorAddress;
  uint32_t count;
  uint32_t data;

  if(!(sectorNumber > XFLASH_LAST_SECTOR)) 
  {
    /* Get sector start address */
    sectorAddress = sectorNumber * XFLASH_SECTOR_SIZE;
    
    for(count = 0; count < XFLASH_SECTOR_SIZE; count++)
    {
      data = XFLASH_WordRead(XFLASH_NORMAL_READ, sectorAddress+count);
      if(data != 0xFFFFFFFF) 
      {
        funcStatus = DERROR;
        break;
      }
    }
  } 
  else
  {
    funcStatus = DERROR;
  }

  return funcStatus;
}

/****************************************************************************//**
 * @brief      Check flash is blank or not
 *
 * @param none
 *
 * @return     Status
 *
 *******************************************************************************/
Status XFLASH_IsBlank(void)
{
  Status funcStatus = DSUCCESS;
  uint32_t flashData;
  uint32_t i;
  uint32_t maxWordAddr;
  
  maxWordAddr = XFLASH_CHIP_SIZE >> 2;
  
  for(i=0; i<maxWordAddr; i++)
  {
    flashData = XFLASH_WordRead(XFLASH_NORMAL_READ, i<<2);
    if(flashData != 0xFFFFFFFF)
    {
      funcStatus = DERROR;
      break;
    }
  }
  
  return funcStatus;
}

/*@} end of group XFLASH_Private_Functions */

/** @defgroup XFLASH_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Select the interface to the flash
 *
 * @param[in]  interface: the interface type
 *
 * @return     none
 *
 *******************************************************************************/
void XFLASH_SelectInterface(XFLASH_Interface_Type interface)
{
}

/****************************************************************************//**
 * @brief      Set flash power down mode
 *
 * @param[in]  newCmd:  Enable/disable power down mode
 *
 * @return none
 *
 *******************************************************************************/
void XFLASH_PowerDown(FunctionalState newCmd)
{
  /* Clear QSPI1 FIFO */ 
  QSPI1_FlushFIFO();
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI1_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_0BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
  
  /* Set data in counter */
  QSPI1_SetDInCnt(0);
  
  if(newCmd == ENABLE)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_PD);
  }
  else
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_RPD_DI);
  }
        
  /* Set QSPI1 write */
  QSPI1_StartTransfer(QSPI_W_EN);
  
  /* Stop QSPI1 transfer */
  QSPI1_StopTransfer();
}

/****************************************************************************//**
 * @brief      Set flash protection mode 
 *
 * @param[in]  protectMode:  Protection mode
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_SetProtectionMode(XFLASH_Protection_Type protectMode)
{
  volatile uint32_t localCnt = 0;
  
  /* Enable flash write */
  XFLASH_SetWriteEnableBit(ENABLE);
  
  /* Set QSPI1 data pin mode */
  QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE; 
  
  /* Set instruction */
  QSPI1_SetInstr(XFLASH_INS_CODE_WSR);    
  
  /* Set QSPI1 write */
  QSPI1_StartTransfer(QSPI_W_EN);
  
  /* Write protection mode (SEC, TB, BP2, BP1, BP0) */
  QSPI1_WriteByte(protectMode & 0x7F);
  
  /* Write protection mode (CMP) */
  QSPI1_WriteByte((protectMode & 0x80)>>1);
  
  /* Stop QSPI1 transfer */
  QSPI1_StopTransfer();
  
  while(localCnt++ < 100000)
  {
    /* Check flash busy status */ 
    if( XFLASH_GetBusyStatus() == RESET )
    {
      return DSUCCESS;
    }
  }
  
  return DERROR;
}

/****************************************************************************//**
 * @brief      Whole flash erase
 *
 * @param none
 *
 * @return     Status
 *
 *******************************************************************************/
Status XFLASH_EraseAll(void) 
{
  volatile uint32_t localCnt = 0;
    
  /* Enable flash write */
  XFLASH_SetWriteEnableBit(ENABLE);  
  
  /* Set instruction */
  QSPI1_SetInstr(XFLASH_INS_CODE_CE);
  
  /* Set QSPI1 write */
  QSPI1_StartTransfer(QSPI_W_EN);
  
  /* Stop QSPI1 transfer */
  QSPI1_StopTransfer();
  
  while(localCnt++ < 0xFFFFFFF)
  {
    /* Check flash busy status */ 
    if( XFLASH_GetBusyStatus() == RESET )
    {
      return DSUCCESS;
    }
  }
  
  return DERROR;    
}

/****************************************************************************//**
 * @brief      Flash sector erase
 *
 * @param[in]  sectorNumber:  Sector number to be erased
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_SectorErase(uint32_t sectorNumber) 
{
  uint32_t sectorAddress;
  volatile uint32_t localCnt = 0;

  if(!(sectorNumber > XFLASH_LAST_SECTOR)) 
  {
    /* Enable flash write */
    XFLASH_SetWriteEnableBit(ENABLE);
    
    /* Get start address for sector to be erased */
    sectorAddress = sectorNumber* XFLASH_SECTOR_SIZE;
    
    /* Set address counter */
    QSPI1_SetAddrCnt(QSPI_ADDR_CNT_3BYTE);
   
    /* Set QSPI1 address pin mode */
    QSPI1->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;
    
    /* Set address */ 
    QSPI1_SetAddr(sectorAddress);
    
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_SE);
    
    /* Set QSPI1 write */
    QSPI1_StartTransfer(QSPI_W_EN);
    
    /* Stop QSPI1 transfer */
    QSPI1_StopTransfer();
    
    while(localCnt++ < 1000000)
    {
      /* Check flash busy status */ 
      if( XFLASH_GetBusyStatus() == RESET )
      {
        return DSUCCESS;
      }
    }
  }
  
  return DERROR;
}

/****************************************************************************//**
 * @brief      Flash 32KB block erase
 *
 * @param[in]  sectorNumber:  block number to be erased
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_Block32KErase(uint32_t blockNumber) 
{
  uint32_t blockAddress;
  volatile uint32_t localCnt = 0;
  
  if(!(blockNumber > XFLASH_LAST_32K_BLOCK) ) 
  {
    /* Enable flash write */
    XFLASH_SetWriteEnableBit(ENABLE);
    
    /* Get start address of the block to be erased */
    blockAddress = blockNumber * XFLASH_32K_BLOCK_SIZE;
    
    /* Set address counter */
    QSPI1_SetAddrCnt(QSPI_ADDR_CNT_3BYTE);
    
    /* Set QSPI1 address pin mode */
    QSPI1->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;
    
    /* Set address */ 
    QSPI1_SetAddr(blockAddress);
    
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_BE_32K);
    
    /* Set QSPI1 write */
    QSPI1_StartTransfer(QSPI_W_EN);
    
    /* Stop QSPI1 transfer */
    QSPI1_StopTransfer();
    
    while(localCnt++ < 2000000)
    {
      /* Check flash busy status */ 
      if( XFLASH_GetBusyStatus() == RESET )
      {
        return DSUCCESS;
      }
    }
  }
  
  return DERROR;
}

/****************************************************************************//**
 * @brief      Flash 64KB block erase
 *
 * @param[in]  sectorNumber:  block number to be erased
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_Block64KErase(uint32_t blockNumber) 
{
  uint32_t blockAddress;
  volatile uint32_t localCnt = 0;
  
  if(!(blockNumber > XFLASH_LAST_64K_BLOCK) ) 
  {
    /* Enable flash write */
    XFLASH_SetWriteEnableBit(ENABLE);
    
    /* Get start address of the block to be erased */
    blockAddress = blockNumber * XFLASH_64K_BLOCK_SIZE;
    
    /* Set address counter */
    QSPI1_SetAddrCnt(QSPI_ADDR_CNT_3BYTE);
    
    /* Set QSPI1 address pin mode */
    QSPI1->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;
    
    /* Set address */ 
    QSPI1_SetAddr(blockAddress);
    
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_BE_64K);
    
    /* Set QSPI1 write */
    QSPI1_StartTransfer(QSPI_W_EN);
    
    /* Stop QSPI1 transfer */
    QSPI1_StopTransfer();
    
    while(localCnt++ < 2000000)
    {
      /* Check flash busy status */ 
      if( XFLASH_GetBusyStatus() == RESET )
      {
        return DSUCCESS;
      }
    }
  }
  
  return DERROR;
}

/****************************************************************************//**
 * @brief      Erase specfied address of the flash
 *
 * @param[in]  startAddr:  Start address to be erased
 * @param[in]  endAddr:  End address to be erased
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_Erase(uint32_t startAddr, uint32_t endAddr) 
{
  int ret;
  uint32_t sectorNumber, blockNumber, length, validStart;

  length = endAddr - startAddr + 1;

  while (length != 0) {
    if ((startAddr & (XFLASH_64K_BLOCK_SIZE - 1)) == 0 &&
		length > (XFLASH_64K_BLOCK_SIZE - XFLASH_SECTOR_SIZE)) {
	/* Address is a multiple of 64K and length is > (64K block -4K sector)
	 * So directly erase 64K from this address */
	blockNumber = startAddr / XFLASH_64K_BLOCK_SIZE;
	ret = XFLASH_Block64KErase(blockNumber);
	endAddr = startAddr + XFLASH_64K_BLOCK_SIZE;
    } else if ((startAddr & (XFLASH_32K_BLOCK_SIZE - 1)) == 0 &&
		length > (XFLASH_32K_BLOCK_SIZE - XFLASH_SECTOR_SIZE)) {
	/* Address is a multiple of 32K and length is > (32K block -4K sector)
	* So directly erase 32K from this address */
	blockNumber = startAddr / XFLASH_32K_BLOCK_SIZE;
	ret = XFLASH_Block32KErase(blockNumber);
	endAddr = startAddr + XFLASH_32K_BLOCK_SIZE;
    } else {
	/* Find 4K aligned address and erase 4K sector */
	validStart = startAddr - (startAddr &
			(XFLASH_SECTOR_SIZE - 1));
	sectorNumber = validStart / XFLASH_SECTOR_SIZE;
	ret = XFLASH_SectorErase(sectorNumber);
	endAddr = validStart + XFLASH_SECTOR_SIZE;
    }

    /* If erase operation fails then return error */
    if (ret != DSUCCESS)
	return DERROR;

    /* Calculate the remaining length that is to be erased yet */
    if (length < (endAddr - startAddr))
	length = 0;
    else
	length -= (endAddr - startAddr);
    startAddr = endAddr;

  }
  return DSUCCESS;

}

/****************************************************************************//**
 * @brief      Read flash from specified address to buffer
 *
 * @param[in]  readMode:  Flash reading mode to be set
 * @param[in]  address:  Flash address to be read
 * @param[in]  buffer:  Buffer to hold data read from flash
 * @param[in]  num:  Number of data to be read from flash
 *
 * @return     Number of data read out from flash, in byte
 *
 *******************************************************************************/
uint32_t XFLASH_Read(XFLASH_ReadMode_Type readMode, uint32_t address, uint8_t *buffer, uint32_t num) 
{
  uint32_t i;
  uint32_t readBytes;
  uint16_t statusLow, statusHigh, statusWrite; 
    
  readBytes = 0;
  
  if( (readMode == XFLASH_FAST_READ_QUAD_OUT) || (readMode == XFLASH_FAST_READ_QUAD_IO)
    ||(readMode == XFLASH_WORD_FAST_READ_QUAD_IO) || (readMode == XFLASH_OCTAL_WORD_FAST_READ_QUAD_IO) )
  {
    statusLow = XFLASH_GetStatus(XFLASH_STATUS_LO);
    statusHigh = XFLASH_GetStatus(XFLASH_STATUS_HI);
    statusWrite = ((statusHigh<<8) | statusLow) | 0x0200;
    XFLASH_WriteStatus(statusWrite);    
  }  
  
  /* Clear QSPI1 FIFO */ 
  QSPI1_FlushFIFO();  

  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI1_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_3BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
    
  /* Set read mode */
  QSPI1_SetRMode(0);  

  /* Set data in counter */
  QSPI1_SetDInCnt(num);  
  
  /* Set address */ 
  QSPI1_SetAddr(address);
  
  /* Set QSPI1 address pin mode */
  QSPI1->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;     
  
  /* Set QSPI1 data pin mode */
  QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE;   

  if(readMode == XFLASH_NORMAL_READ)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_RD);
  }
  else if(readMode == XFLASH_FAST_READ)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_FR);
    
    /* Set dummy counter */
    QSPI1_SetDummyCnt(QSPI_DUMMY_CNT_1BYTE);
  }
  else if(readMode == XFLASH_FAST_READ_DUAL_OUT)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_FRDO);
    
    /* Set dummy counter */
    QSPI1_SetDummyCnt(QSPI_DUMMY_CNT_1BYTE);
    
    /* Set QSPI1 data pin mode */
    QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_DUAL;  
  }
  else if(readMode == XFLASH_FAST_READ_DUAL_IO)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_FRDIO);
    
    /* Set read mode counter */
    QSPI1_SetRModeCnt(QSPI_RM_CNT_1BYTE);
    
    /* Set QSPI1 data pin mode */
    QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_DUAL;
    
    /* Set QSPI1 address pin mode */
    QSPI1->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_AS_DATA;   
  }    
  else if(readMode == XFLASH_FAST_READ_QUAD_OUT)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_FRQO);
    
    /* Set dummy counter */
    QSPI1_SetDummyCnt(QSPI_DUMMY_CNT_1BYTE);
    
    /* Set QSPI1 data pin mode */
    QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;  
  }
  else if(readMode == XFLASH_FAST_READ_QUAD_IO)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_FRQIO);
 
    /* Set read mode counter */
    QSPI1_SetRModeCnt(QSPI_RM_CNT_1BYTE);
        
    /* Set dummy counter */
    QSPI1_SetDummyCnt(QSPI_DUMMY_CNT_2BYTE);    
    
    /* Set QSPI1 data pin mode */
    QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;
    
    /* Set QSPI1 address pin mode */
    QSPI1->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_AS_DATA;      
  }
  else if(readMode == XFLASH_WORD_FAST_READ_QUAD_IO)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_WFRQIO);
    
    /* Set read mode counter */
    QSPI1_SetRModeCnt(QSPI_RM_CNT_1BYTE);
        
    /* Set dummy counter */
    QSPI1_SetDummyCnt(QSPI_DUMMY_CNT_1BYTE);    
    
    /* Set QSPI1 data pin mode */
    QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;
    
    /* Set QSPI1 address pin mode */
    QSPI1->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_AS_DATA;         
  }
  else if(readMode == XFLASH_OCTAL_WORD_FAST_READ_QUAD_IO)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_OWFRQIO);
    
    /* Set read mode counter */
    QSPI1_SetRModeCnt(QSPI_RM_CNT_1BYTE);
        
    /* Set QSPI1 data pin mode */
    QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;
    
    /* Set QSPI1 address pin mode */
    QSPI1->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_AS_DATA;         
  }
    
  /* QSPI1 one-byte length mode */
  QSPI1->CONF.BF.BYTE_LEN = QSPI_BYTE_LEN_1BYTE;
  
  /* Set QSPI1 read */
  QSPI1_StartTransfer(QSPI_R_EN);
    
  for (i=0; i<num; i++)
  {
    /* Waiting for RFIFO not empty */
    while(QSPI1->CNTL.BF.RFIFO_EMPTY == 1);
    
    buffer[i] = (QSPI1->DIN.WORDVAL) & 0xFF;
    readBytes++;
  } 
  
  /* Disable QSPI1 */
  QSPI1_SetSSEnable(DISABLE); 
  
  return readBytes;
}

/****************************************************************************//**
 * @brief      Read a word from specified flash address
 *
 * @param[in]  readMode:  Flash reading mode to be set
 * @param[in]  address:  Flash address to be read
 *
 * @return     Data in word
 *
 *******************************************************************************/
uint32_t XFLASH_WordRead(XFLASH_ReadMode_Type readMode, uint32_t address) 
{
  uint32_t data = 0;
  XFLASH_Read(readMode, address, (uint8_t*)&data, 4);
  return data;
}

/****************************************************************************//**
 * @brief      Read a byte from specified flash address
 *
 * @param[in]  readMode:  Flash reading mode to be set
 * @param[in]  address:  Flash address to be read
 *
 * @return     Data in byte
 *
 *******************************************************************************/
uint8_t XFLASH_ByteRead(XFLASH_ReadMode_Type readMode, uint32_t address) 
{
  uint8_t data = 0;
  XFLASH_Read(readMode, address, (uint8_t*)&data, 1);
  return data;
}

/****************************************************************************//**
 * @brief      Write flash within a page
 *
 * @param[in]  programMode:  Flash program mode to be set
 * @param[in]  address:  Page address
 * @param[in]  buffer:  Buffer data to be programmed to flash
 * @param[in]  num:  Number of data to be programmed to flash
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_PageWrite(XFLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num) 
{
  uint32_t i;
  volatile uint32_t localCnt = 0;
  uint16_t statusLow, statusHigh, statusWrite; 
  
  if(programMode == XFLASH_PROGRAM_QUAD)
  {
    statusLow = XFLASH_GetStatus(XFLASH_STATUS_LO);
    statusHigh = XFLASH_GetStatus(XFLASH_STATUS_HI);
    statusWrite = ((statusHigh<<8) | statusLow) | 0x0200;
    XFLASH_WriteStatus(statusWrite);    
  }    
  
  /* Check address validity */
  if ((XFLASH_PAGE_NUM(address+num-1) > XFLASH_PAGE_NUM(address)) || num == 0)
  {
    return DERROR;
  }
  
  /* Enable flash write */
  XFLASH_SetWriteEnableBit(ENABLE);
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI1_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_3BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
  
  /* Set QSPI1 address pin mode */
  QSPI1->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;
  
  /* Set QSPI1 data pin mode */
  QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE;   
  
  /* Set address */ 
  QSPI1_SetAddr(address);
  
  if(programMode == XFLASH_PROGRAM_NORMAL)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_PP);    
  }
  else if(programMode == XFLASH_PROGRAM_QUAD)
  {
    /* Set instruction */
    QSPI1_SetInstr(XFLASH_INS_CODE_QPP);    
    
    /* Set QSPI1 data pin mode */
    QSPI1->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;  
  }
  
  /* QSPI1 one-byte length mode */
  QSPI1->CONF.BF.BYTE_LEN = QSPI_BYTE_LEN_1BYTE;
  
  /* Set QSPI1 write */
  QSPI1_StartTransfer(QSPI_W_EN);
    
  for (i=0; i<num; i++) 
  {
    QSPI1_WriteByte(buffer[i]);
  }
  
  /* Stop QSPI1 transfer */
  QSPI1_StopTransfer();
  
  while(localCnt++ < 1000000)
  {
    /* Check flash busy status */ 
    if( XFLASH_GetBusyStatus() == RESET )
    {
      return DSUCCESS;
    }
  }  

  return DERROR;
}

/****************************************************************************//**
 * @brief      Write flash with any address and size
 *
 * @param[in]  programMode:  Flash program mode to be set
 * @param[in]  address:  Page address
 * @param[in]  buffer:  Buffer data to be programmed to flash
 * @param[in]  num:  Number of data to be programmed to flash
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_Write(XFLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num) 
{
  uint8_t *pBuf;
  uint32_t begPgNum;
  uint32_t endPgNum;
  uint32_t step;
  uint32_t addrCur;
  uint32_t i;
  Status funcStatus = DSUCCESS;  
  
  pBuf = buffer;
  addrCur = address;
  
  /* Get page number of start address */
  begPgNum = XFLASH_PAGE_NUM(address);
  /* Get page number of end address */
  endPgNum = XFLASH_PAGE_NUM(address + num - 1);

  /* Both start address and end address are within the same page */
  if(begPgNum == endPgNum)
  {
    return( XFLASH_PageWrite(programMode, address, buffer, num) );
  } 
  /* Start address and end address are not in the same page */
  else
  {
    /* For first page */
    step = XFLASH_PAGE_END_ADDR(address)-address+1;
    funcStatus = XFLASH_PageWrite(programMode, address, pBuf, step);
    if(funcStatus == DERROR)
    {
      return DERROR;
    }
    
    pBuf += step;
    addrCur += step;

    for(i=begPgNum+1; i<=endPgNum; i++)
    {
      /* For last page */
      if(i == endPgNum)
      {
        step = (address + num) & 0xFF;
        
        /* If step is 0, the last page has 256 bytes data to be writen ( num of data is 0x100 ) */
        if(step == 0)
        {
          step = 0x100;
        }
        
        return( XFLASH_PageWrite(programMode, addrCur, pBuf, step) );
      } 
      else
      {
        funcStatus = XFLASH_PageWrite(programMode, addrCur, pBuf, XFLASH_PAGE_SIZE);
        if(funcStatus == DERROR)
        {
          return DERROR;
        }
        
        pBuf += XFLASH_PAGE_SIZE;
        addrCur += XFLASH_PAGE_SIZE;
      }
    }
  }
  
  return funcStatus;
}

/****************************************************************************//**
 * @brief      Write a word to specified flash address
 *
 * @param[in]  programMode:  Flash program mode to be set
 * @param[in]  address:  Flash address to be programmed
 * @param[in]  data:  Data to be programmed to flash
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_WordWrite(XFLASH_ProgramMode_Type programMode, uint32_t address, uint32_t data) 
{
  return XFLASH_Write(programMode, address, (uint8_t*)&data, 4);
}

/****************************************************************************//**
 * @brief      Write a byte to specified flash address
 *
 * @param[in]  address:  Flash address to be programmed
 * @param[in]  data:  Data to be programmed to flash
 *
 * @return     DSUCCESS or DERROR
 *
 *******************************************************************************/
Status XFLASH_ByteWrite(XFLASH_ProgramMode_Type programMode, uint32_t address, uint8_t data) 
{
  return XFLASH_Write(programMode, address, (uint8_t*)&data, 1);
}

/****************************************************************************//**
 * @brief      Get flash unique ID
 *
 * @param none
 *
 * @return     Unique ID
 *
 *******************************************************************************/
uint64_t XFLASH_GetUniqueID(void)
{
  uint64_t uniqueID;
  
  QSPI1_FlushFIFO();  
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter*/
  QSPI1_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_1BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_3BYTE);
  
  QSPI1_SetDInCnt(8);
  
  QSPI1_SetAddr(0);
  
  /* Read Unique ID number */
  QSPI1_SetInstr(XFLASH_INS_CODE_RUID);
  
  QSPI1_StartTransfer(QSPI_R_EN);

  uniqueID = QSPI1_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI1_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI1_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI1_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI1_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI1_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI1_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI1_ReadByte();  
  
  QSPI1_SetSSEnable(DISABLE);
  
  return uniqueID;
}

/*@} end of group XFLASH_Public_Functions */

/*@} end of group XFLASH_definitions */

/*@} end of group MC200_Periph_Driver */
