/**
 ******************************************************************************
 * @file    platform_flash.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provides flash operation functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "platform_logging.h"
#include "platform_peripheral.h"
#include "platform.h"
#include "platform_config.h"
#include "stdio.h"

#ifdef USE_MICO_SPI_FLASH
#include "spi_flash.h"
#endif

/* Private constants --------------------------------------------------------*/
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x00000000) /* Base @ of Sector  0, 32 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x00008000) /* Base @ of Sector  1, 32 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x00010000) /* Base @ of Sector  2, 32 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x00018000) /* Base @ of Sector  3, 32 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x00020000) /* Base @ of Sector  4, 32 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x00028000) /* Base @ of Sector  5, 32 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x00030000) /* Base @ of Sector  6, 32 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x00038000) /* Base @ of Sector  7, 32 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x00040000) /* Base @ of Sector  8, 32 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x00048000) /* Base @ of Sector  9, 32 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x00050000) /* Base @ of Sector 10, 32 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x00058000) /* Base @ of Sector 11, 32 Kbyte */
#define ADDR_FLASH_SECTOR_12    ((uint32_t)0x00060000) /* Base @ of Sector 12, 32 Kbyte */
#define ADDR_FLASH_SECTOR_13    ((uint32_t)0x00068000) /* Base @ of Sector 13, 32 Kbyte */
#define ADDR_FLASH_SECTOR_14    ((uint32_t)0x00070000) /* Base @ of Sector 14, 32 Kbyte */
#define ADDR_FLASH_SECTOR_15    ((uint32_t)0x00078000) /* Base @ of Sector 15, 32 Kbyte */
#define ADDR_FLASH_SECTOR_16    ((uint32_t)0x00080000) /* Base @ of Sector 16, 32 Kbyte */

/* End of the Flash address */
#define FLASH_START_ADDRESS     (uint32_t)0x00000000
#define FLASH_END_ADDRESS       (uint32_t)0x0007FFFF
#define FLASH_SIZE              (FLASH_END_ADDRESS -  FLASH_START_ADDRESS + 1)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef USE_MICO_SPI_FLASH
static sflash_handle_t sflash_handle = {0x0, 0x0, SFLASH_WRITE_NOT_ALLOWED};
#endif
/* Private function prototypes -----------------------------------------------*/
static uint32_t _GetSector( uint32_t Address );
static OSStatus _GetAddress(uint32_t sector, uint32_t *startAddress, uint32_t *endAddress);
static OSStatus internalFlashInitialize( void );
static OSStatus internalFlashErase(uint32_t StartAddress, uint32_t EndAddress);
static OSStatus internalFlashWrite(volatile uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength);
#ifdef MCU_EBANLE_FLASH_PROTECT
static uint32_t _GetWRPSector(uint32_t Address);
static OSStatus internalFlashProtect(uint32_t StartAddress, uint32_t EndAddress, bool enable);
#endif
#ifdef USE_MICO_SPI_FLASH
static OSStatus spiFlashErase(uint32_t StartAddress, uint32_t EndAddress);
#endif


OSStatus platform_flash_init( const platform_flash_t *peripheral )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    err = internalFlashInitialize();
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = init_sflash( &sflash_handle, 0, SFLASH_WRITE_ALLOWED );
    require_noerr(err, exit);
  }
#endif
  else{
    err = kTypeErr;
    goto exit;
  }
exit:
  return err;
}

OSStatus platform_flash_erase( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( start_address >= peripheral->flash_start_addr
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    err = internalFlashErase( start_address, end_address );
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = spiFlashErase( start_address, end_address );
    require_noerr(err, exit);
  }
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

OSStatus platform_flash_write( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( *start_address >= peripheral->flash_start_addr
               && *start_address + length <= peripheral->flash_start_addr + peripheral->flash_length, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    err = internalFlashWrite( start_address, (uint32_t *)data, length);
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = sflash_write( &sflash_handle, *start_address, data, length );
    require_noerr(err, exit);
    *start_address += length;
  }
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

OSStatus platform_flash_read( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( (*start_address >= peripheral->flash_start_addr)
               && (*start_address + length) <= ( peripheral->flash_start_addr + peripheral->flash_length), exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    memcpy(data, (void *)(*start_address), length);
    *start_address += length;
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = sflash_read( &sflash_handle, *start_address, data, length );
    require_noerr(err, exit);
    *start_address += length;
  }
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

OSStatus platform_flash_enable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( start_address >= peripheral->flash_start_addr
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
#ifdef MCU_EBANLE_FLASH_PROTECT
    err = internalFlashProtect( start_address, end_address, true );
#endif
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = kNoErr;
    goto exit;
  }
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

OSStatus platform_flash_disable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( start_address >= peripheral->flash_start_addr
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
#ifdef MCU_EBANLE_FLASH_PROTECT
    err = internalFlashProtect( start_address, end_address, false );
#endif
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = kNoErr;
    goto exit;
  }
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

static uint32_t s_wpage_buff[64];

#ifdef BOOTLOADER
static uint32_t s_wlast_buff_datacount=0;
static uint32_t s_wlast_buff_flashaddr=0;
static uint32_t s_flashwritecount=0;
#endif
static uint32_t s_intflash_offset=0;
OSStatus internalFlashInitialize( void )
{
  platform_log_trace();
//  FLASH_Unlock();
//  /* Clear pending flags (if any) */
//  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
//                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

  s_intflash_offset=0;
#ifdef BOOTLOADER
  s_wlast_buff_datacount=0;
  s_wlast_buff_flashaddr=0;
  s_flashwritecount=0;
#endif
  return kNoErr;
}

OSStatus internalFlashErase(uint32_t StartAddress, uint32_t EndAddress)
{
  platform_log_trace();
  OSStatus err = kNoErr;
  uint32_t StartSector, EndSector, i = 0, j = 0, ret_code;	// Magicoe added ret_code

  /* Get the sector where start the user flash area */
  StartSector = _GetSector(StartAddress);
  EndSector = _GetSector(EndAddress);
  for(i = StartSector; i <= EndSector; i ++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */
    _GetAddress(i, &StartAddress, &EndAddress);
    for(j=StartAddress; j<=EndAddress; j+=8){
      if( (*(uint32_t *)(j))!=0xFFFFFFFF )
        break;
    }
    if( j>EndAddress )
      continue;
	ret_code = Chip_IAP_PreSectorForReadWrite(i, i);
	/* Error checking */
	if (ret_code != IAP_CMD_SUCCESS) {
		err = kWriteErr;
		return err;
		//DEBUGOUT("Command failed to execute, return code is: %x\r\n", ret_code);
	}
	ret_code = Chip_IAP_EraseSector(i, i);
	if (ret_code != IAP_CMD_SUCCESS) {
		err = kWriteErr;
		return err;
		//DEBUGOUT("Command failed to execute, return code is: %x\r\n", ret_code);
	}
   // require_action(FLASH_EraseSector(i, VoltageRange_3) == FLASH_COMPLETE, exit, err = kWriteErr);
  }

  return err;
}

#ifdef MCU_EBANLE_FLASH_PROTECT
OSStatus internalFlashProtect(uint32_t StartAddress, uint32_t EndAddress, bool enable)
{
  OSStatus err = kNoErr;
  uint16_t WRP = 0x0;
  uint32_t StartSector, EndSector, i = 0;
  bool needupdate = false;

  /* Get the sector where start the user flash area */
  StartSector = _GetWRPSector(StartAddress);
  EndSector = _GetWRPSector(EndAddress);

  for(i = StartSector; i <= EndSector; i=i<<1)
  {
    WRP = FLASH_OB_GetWRP();

    if( ( enable == true && (WRP & i) == 0x0 ) ||
        ( enable == false && (WRP & i) ) ) {
      continue;
    }
    if( needupdate == false){
      FLASH_OB_Unlock( );
      needupdate = true;
    }
    if( enable == true )
      FLASH_OB_WRPConfig( i, ENABLE );
    else
      FLASH_OB_WRPConfig( i, DISABLE );
  }

  if( needupdate == true){
    FLASH_OB_Launch( );
    FLASH_OB_Lock( );
  }

  return err;
}
#endif

#ifdef USE_MICO_SPI_FLASH
OSStatus spiFlashErase(uint32_t StartAddress, uint32_t EndAddress)
{
  platform_log_trace();
  OSStatus err = kNoErr;
  uint32_t StartSector, EndSector, i = 0;

  /* Get the sector where start the user flash area */
  StartSector = StartAddress>>12;
  EndSector = EndAddress>>12;

  for(i = StartSector; i <= EndSector; i += 1)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */
    require_action(sflash_sector_erase(&sflash_handle, i<<12) == kNoErr, exit, err = kWriteErr);
  }

exit:
  return err;
}
#endif


OSStatus internalFlashWrite(volatile uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength)
{
  platform_log_trace();
  OSStatus err = kNoErr;
  uint32_t i = 0;
  uint32_t StartSector;
  uint32_t DataLengthRemain = DataLength;
  uint32_t DataLengthWR = 0;
  uint8_t * ppage_buff=(uint8_t *)s_wpage_buff;
  uint8_t * pData = (uint8_t *) Data;

//  uint32_t wr_addr = (*FlashAddress) + s_intflash_offset;
  uint32_t wr_addr = (*FlashAddress);
   StartSector = _GetSector(wr_addr);
   {
       uint32_t dataPos=0;
       uint32_t startAddrPage = (wr_addr/256)*256;

       if(wr_addr%256)
       {
          uint32_t startAddrInPage = wr_addr%256;
        // for(i=0;i<s_wlast_page;i++)
         for(i=0;i<256;i++)
         {
           ppage_buff[i]= *((uint8_t*)(startAddrPage+i));
         }
         if(DataLength>(256-startAddrInPage))
           DataLengthWR=(256-startAddrInPage);
         else
           DataLengthWR =DataLength;
         for(i=0;i<DataLengthWR;i++)
         {
           ppage_buff[i+startAddrInPage]= *((uint8_t*)(pData+dataPos+i));
         }
         dataPos +=DataLengthWR;

         {
           StartSector = _GetSector(startAddrPage);

	   Chip_IAP_PreSectorForReadWrite(StartSector, StartSector);
//           Chip_IAP_ErasePage(startAddrPage/256,startAddrPage/256);
           Chip_IAP_PreSectorForReadWrite(StartSector, StartSector);
           Chip_IAP_CopyRamToFlash(startAddrPage, (uint32_t *)ppage_buff, 256);
           startAddrPage +=256;
           DataLengthRemain -=DataLengthWR;
         }

       }

       while(DataLengthRemain>0)
       {
         if(DataLengthRemain>=256)
         {
           StartSector = _GetSector(startAddrPage);
#ifndef BOOTLOADER
	   Chip_IAP_PreSectorForReadWrite(StartSector, StartSector);
//           Chip_IAP_ErasePage(startAddrPage/256,startAddrPage/256);
#endif
           Chip_IAP_PreSectorForReadWrite(StartSector, StartSector);
           Chip_IAP_CopyRamToFlash(startAddrPage, (uint32_t *)(pData+dataPos), 256);
           startAddrPage +=256;
           dataPos +=256;
           DataLengthRemain -=256;
         }else
         {
           for(i=0;i<256;i++)
           {
             ppage_buff[i]= *((uint8_t*)(startAddrPage+i));
           }

           for(i=0;i<DataLengthRemain;i++)
           {
             ppage_buff[i]= *((uint8_t*)(pData+dataPos+i));
           }

           StartSector = _GetSector(startAddrPage);
	   Chip_IAP_PreSectorForReadWrite(StartSector, StartSector);
//           Chip_IAP_ErasePage(startAddrPage/256,startAddrPage/256);
           Chip_IAP_PreSectorForReadWrite(StartSector, StartSector);
           //modify
//           Chip_IAP_CopyRamToFlash(startAddrPage, (uint32_t *)ppage_buff, 256);
           Chip_IAP_CopyRamToFlash(startAddrPage, (uint32_t *)ppage_buff, DataLengthRemain);
           DataLengthRemain =0;
         }
       }
   }

  s_intflash_offset +=DataLength;
  require_noerr(err, exit);

exit:
  return err;
}


OSStatus internalFlashFinalize( void )
{
#ifdef BOOTLOADER
   uint32_t StartSector = 0;
   uint32_t i=0;
   uint8_t * ppage_buff=(uint8_t *)s_wpage_buff;
   if(s_wlast_buff_datacount>0)
   {
     for(i=s_wlast_buff_datacount;i<256;i++)
       ppage_buff[i]=0xff;

     StartSector = _GetSector(s_wlast_buff_flashaddr+s_flashwritecount);
     Chip_IAP_PreSectorForReadWrite(StartSector, StartSector);
     Chip_IAP_CopyRamToFlash(s_wlast_buff_flashaddr+s_flashwritecount, (uint32_t *)ppage_buff, 256);
     s_flashwritecount =0;
     s_wlast_buff_datacount=0;
     s_wlast_buff_flashaddr=0;
   }
#endif
  //FLASH_Lock();
  return kNoErr;
}
/**
* @brief  Gets the sector of a given address
* @param  Address: Flash address
* @retval The sector of a given address
*/
static uint32_t _GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if(Address < ADDR_FLASH_SECTOR_1)/* && (Address >= ADDR_FLASH_SECTOR_0))*/
  {
    sector = 0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = 1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = 2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = 3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = 4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = 5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = 6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = 7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = 8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = 9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = 10;
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = 11;
  }
  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = 12;
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = 13;
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = 14;
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = 15;
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = 15;
  }
  return sector;
}


#ifdef MCU_EBANLE_FLASH_PROTECT
/**
* @brief  Gets the sector of a given address
* @param  Address: Flash address
* @retval The sector of a given address
*/
static uint32_t _GetWRPSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = OB_WRP_Sector_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = OB_WRP_Sector_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = OB_WRP_Sector_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = OB_WRP_Sector_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = OB_WRP_Sector_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = OB_WRP_Sector_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = OB_WRP_Sector_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = OB_WRP_Sector_7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = OB_WRP_Sector_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = OB_WRP_Sector_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = OB_WRP_Sector_10;
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = OB_WRP_Sector_11;
  }
  return sector;
}
#endif

/**
* @brief  Gets the address of a given sector
* @param  Sector: The sector of a given address
* @retval Flash address if the sector start
*/
static OSStatus _GetAddress(uint32_t sector, uint32_t *startAddress, uint32_t *endAddress)
{
  OSStatus err = kNoErr;
  if(sector == 0)
  {
    *startAddress = ADDR_FLASH_SECTOR_0;
    *endAddress = ADDR_FLASH_SECTOR_1 - 1;
  }
  else if(sector == 1)
  {
    *startAddress = ADDR_FLASH_SECTOR_1;
    *endAddress = ADDR_FLASH_SECTOR_2 - 1;
  }
  else if(sector == 2)
  {
    *startAddress = ADDR_FLASH_SECTOR_2;
    *endAddress = ADDR_FLASH_SECTOR_3 - 1;
  }
  else if(sector == 3)
  {
    *startAddress = ADDR_FLASH_SECTOR_3;
    *endAddress = ADDR_FLASH_SECTOR_4 - 1;
  }
  else if(sector == 4)
  {
    *startAddress = ADDR_FLASH_SECTOR_4;
    *endAddress = ADDR_FLASH_SECTOR_5 - 1;
  }
  else if(sector == 5)
  {
    *startAddress = ADDR_FLASH_SECTOR_5;
    *endAddress = ADDR_FLASH_SECTOR_6 - 1;
  }
  else if(sector == 6)
  {
    *startAddress = ADDR_FLASH_SECTOR_6;
    *endAddress = ADDR_FLASH_SECTOR_7 - 1;
  }
  else if(sector == 7)
  {
    *startAddress = ADDR_FLASH_SECTOR_7;
    *endAddress = ADDR_FLASH_SECTOR_8 - 1;
  }
  else if(sector == 8)
  {
    *startAddress = ADDR_FLASH_SECTOR_8;
    *endAddress = ADDR_FLASH_SECTOR_9 - 1;
  }
  else if(sector == 9)
  {
    *startAddress = ADDR_FLASH_SECTOR_9;
    *endAddress = ADDR_FLASH_SECTOR_10 - 1;
  }
  else if(sector == 10)
  {
    *startAddress = ADDR_FLASH_SECTOR_10;
    *endAddress = ADDR_FLASH_SECTOR_11 - 1;
  }
  else if(sector == 11)
  {
    *startAddress = ADDR_FLASH_SECTOR_11;
    *endAddress = FLASH_END_ADDRESS - 1;
  }
  else if(sector == 12)
  {
    *startAddress = ADDR_FLASH_SECTOR_12;
    *endAddress = FLASH_END_ADDRESS - 1;
  }
  else if(sector == 13)
  {
    *startAddress = ADDR_FLASH_SECTOR_13;
    *endAddress = FLASH_END_ADDRESS - 1;
  }
  else if(sector == 14)
  {
    *startAddress = ADDR_FLASH_SECTOR_14;
    *endAddress = FLASH_END_ADDRESS - 1;
  }
  else if(sector == 15)
  {
    *startAddress = ADDR_FLASH_SECTOR_15;
    *endAddress = FLASH_END_ADDRESS - 1;
  }
  else
    err = kNotFoundErr;

  return err;
}

