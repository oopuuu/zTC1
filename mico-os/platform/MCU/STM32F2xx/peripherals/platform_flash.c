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
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */

/* End of the Flash address */
#define FLASH_START_ADDRESS     (uint32_t)0x08000000  
#define FLASH_END_ADDRESS       (uint32_t)0x080FFFFF
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
static uint32_t _GetWRPSector(uint32_t Address);
static OSStatus _GetAddress(uint32_t sector, uint32_t *startAddress, uint32_t *endAddress);
static OSStatus internalFlashInitialize( void );
static OSStatus internalFlashErase(uint32_t StartAddress, uint32_t EndAddress);
static OSStatus internalFlashWrite(volatile uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength);
static OSStatus internalFlashByteWrite( volatile uint32_t* FlashAddress, uint8_t* Data ,uint32_t DataLength );
static OSStatus internalFlashProtect(uint32_t StartAddress, uint32_t EndAddress, bool enable);

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
    err = internalFlashProtect( start_address, end_address, false );    
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

OSStatus internalFlashInitialize( void )
{ 
  platform_log_trace();
  FLASH_Unlock(); 
  /* Clear pending flags (if any) */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
  return kNoErr;
}

OSStatus internalFlashErase(uint32_t StartAddress, uint32_t EndAddress)
{
  platform_log_trace();
  OSStatus err = kNoErr;
  uint32_t StartSector, EndSector, i = 0, j = 0;
  
  /* Get the sector where start the user flash area */
  StartSector = _GetSector(StartAddress);
  EndSector = _GetSector(EndAddress);
  
  for(i = StartSector; i <= EndSector; i += 8)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */
    _GetAddress(i, &StartAddress, &EndAddress);
    for(j=StartAddress; j<=EndAddress; j+=4){
      if( (*(uint32_t *)(j))!=0xFFFFFFFF )
        break;
    }
    if( j>EndAddress ) 
      continue;
    require_action(FLASH_EraseSector(i, VoltageRange_3) == FLASH_COMPLETE, exit, err = kWriteErr); 
  }
  
exit:
  return err;
}

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
  uint32_t dataInRam;
  uint8_t startNumber;
  uint32_t DataLength32 = DataLength;
  
  /*First bytes that are not 32bit align*/
  if(*FlashAddress%4){
    startNumber = 4-(*FlashAddress)%4;
    err = internalFlashByteWrite(FlashAddress, (uint8_t *)Data, startNumber);
    require_noerr(err, exit);
    DataLength32 = DataLength - startNumber;
    Data = (uint32_t *)((uint32_t)Data + startNumber);
  }
  
  /*Program flash by words*/
  for (i = 0; (i < DataLength32/4) && (*FlashAddress <= (FLASH_END_ADDRESS-3)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */ 
    dataInRam = *(Data+i);
    require_action(FLASH_ProgramWord(*FlashAddress, dataInRam) == FLASH_COMPLETE, exit, err = kWriteErr); 
    require_action(*(uint32_t*)*FlashAddress == dataInRam, exit, err = kChecksumErr); 
    /* Increment FLASH destination address */
    *FlashAddress += 4;
  }
  
  /*Last bytes that cannot be write by a 32 bit word*/
  err = internalFlashByteWrite(FlashAddress, (uint8_t *)Data + i*4, DataLength32-i*4);
  require_noerr(err, exit);
  
exit:
  return err;
}

OSStatus internalFlashByteWrite(__IO uint32_t* FlashAddress, uint8_t* Data ,uint32_t DataLength)
{
  uint32_t i = 0;
  uint32_t dataInRam;
  OSStatus err = kNoErr;
  
  for (i = 0; (i < DataLength) && (*FlashAddress <= (FLASH_END_ADDRESS)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */ 
    dataInRam = *(uint8_t*)(Data+i);
    
    require_action(FLASH_ProgramByte(*FlashAddress, dataInRam) == FLASH_COMPLETE, exit, err = kWriteErr); 
    require_action(*(uint8_t*)*FlashAddress == dataInRam, exit, err = kChecksumErr); 
    *FlashAddress +=1;
  }
  
exit:
  return err;
}

/**
* @brief  Gets the sector of a given address
* @param  Address: Flash address
* @retval The sector of a given address
*/
static uint32_t _GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }
  return sector;
}

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

/**
* @brief  Gets the address of a given sector
* @param  Sector: The sector of a given address
* @retval Flash address if the sector start
*/
static OSStatus _GetAddress(uint32_t sector, uint32_t *startAddress, uint32_t *endAddress)
{
  OSStatus err = kNoErr; 
  if(sector == FLASH_Sector_0)
  {
    *startAddress = ADDR_FLASH_SECTOR_0;
    *endAddress = ADDR_FLASH_SECTOR_1 - 1;
  }
  else if(sector == FLASH_Sector_1)
  {
    *startAddress = ADDR_FLASH_SECTOR_1;
    *endAddress = ADDR_FLASH_SECTOR_2 - 1;
  }
  else if(sector == FLASH_Sector_2)
  {
    *startAddress = ADDR_FLASH_SECTOR_2;
    *endAddress = ADDR_FLASH_SECTOR_3 - 1;
  }
  else if(sector == FLASH_Sector_3)
  {
    *startAddress = ADDR_FLASH_SECTOR_3;
    *endAddress = ADDR_FLASH_SECTOR_4 - 1;
  }
  else if(sector == FLASH_Sector_4)
  {
    *startAddress = ADDR_FLASH_SECTOR_4;
    *endAddress = ADDR_FLASH_SECTOR_5 - 1;
  }
  else if(sector == FLASH_Sector_5)
  {
    *startAddress = ADDR_FLASH_SECTOR_5;
    *endAddress = ADDR_FLASH_SECTOR_6 - 1;
  }
  else if(sector == FLASH_Sector_6)
  {
    *startAddress = ADDR_FLASH_SECTOR_6;
    *endAddress = ADDR_FLASH_SECTOR_7 - 1;
  }
  else if(sector == FLASH_Sector_7)
  {
    *startAddress = ADDR_FLASH_SECTOR_7;
    *endAddress = ADDR_FLASH_SECTOR_8 - 1;
  }
  else if(sector == FLASH_Sector_8)
  {
    *startAddress = ADDR_FLASH_SECTOR_8;
    *endAddress = ADDR_FLASH_SECTOR_9 - 1;
  }
  else if(sector == FLASH_Sector_9)
  {
    *startAddress = ADDR_FLASH_SECTOR_9;
    *endAddress = ADDR_FLASH_SECTOR_10 - 1;
  }
  else if(sector == FLASH_Sector_10)
  {
    *startAddress = ADDR_FLASH_SECTOR_10;
    *endAddress = ADDR_FLASH_SECTOR_11 - 1;
  }
  else if(sector == FLASH_Sector_11)
  {
    *startAddress = ADDR_FLASH_SECTOR_11;
    *endAddress = FLASH_END_ADDRESS - 1;
  }
  else
    err = kNotFoundErr;
  
  return err;
}

