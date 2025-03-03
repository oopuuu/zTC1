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
#include "platform.h"
#include "Platform_config.h"
#include "platform_peripheral.h"
#include "stdio.h"


//#define DEBUG_FLASH

#ifdef DEBUG_FLASH
#define APP_DBG printf
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

#ifdef DEBUG_FLASH
SPI_FLASH_INFO  FlashInfo;

void GetFlashGD(int32_t protect)
{    
	uint8_t  str[20];
	int32_t FlashCapacity = 0;
		
	switch(FlashInfo.Did)
	{
		case 0x1340:	
			strcpy((char *)str,"GD25Q40(GD25Q40B)");
			FlashCapacity = 0x00080000;
			break;

		case 0x1240:
        	strcpy((char *)str,"GD25Q20(GD25Q20B)");
			FlashCapacity = 0x00040000;
			break;       

		case 0x1540:
			strcpy((char *)str,"GD25Q16(GD25Q16B)");
			FlashCapacity = 0x00200000;			
			break; 

		case 0x1640:
        	strcpy((char *)str,"GD25Q32(GD25Q32B)");
			FlashCapacity = 0x00400000;        
			break;

		case 0x1740:
        	strcpy((char *)str,"GD25Q64B");
			FlashCapacity = 0x00800000;          
			break;

		case 0x1440:
        	strcpy((char *)str,"GD25Q80(GD25Q80B)");
			FlashCapacity = 0x00100000;         
			break;

		case 0x1840:
            strcpy((char *)str,"GD25Q128B");
            FlashCapacity = 0x01000000;         
            break;

		default:
			break;
	}
        
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashWinBound(int32_t protect)
{	
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
	switch(FlashInfo.Did)
    {
        case 0x1440:
            strcpy((char *)str,"W25Q80BV");
            FlashCapacity = 0x00100000;             
            break;
        
        case 0x1760:
            strcpy((char *)str,"W25Q64DW");
            FlashCapacity = 0x00800000;             
            break;
        				
        case 0x1740:
            strcpy((char *)str,"W25Q64CV");
            FlashCapacity = 0x00800000; 
            break;
        
        default:
            break;
    }
    
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashPct(void)
{	
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
	switch(FlashInfo.Did)
    {
        case 0x0126:
            strcpy((char *)str,"PCT26VF016");
            FlashCapacity = 0x00200000;        
			break;

        case 0x0226:       
            strcpy((char *)str,"PCT26VF032");
            FlashCapacity = 0x00400000;
            break;

        default:            
			break;
    }
      
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashEon(int32_t protect)
{
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
	switch(FlashInfo.Did)
    {
        case 0x1430:
            strcpy((char *)str,"EN25Q80A");
            FlashCapacity = 0x00100000; 
            break;

        case 0x1530:
            strcpy((char *)str,"EN25Q16A");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1830:
            strcpy((char *)str,"EN25Q128");
            FlashCapacity = 0x01000000; 
            break;

        case 0x1630:
            strcpy((char *)str,"EN25Q32A");
            FlashCapacity = 0x00400000; 
            break;

        case 0x1330:
            strcpy((char *)str,"EN25Q40");
            FlashCapacity = 0x00080000; 
            break;

        case 0x1730:
            strcpy((char *)str,"EN25Q64");
            FlashCapacity = 0x00800000; 
            break;

        case 0x1570:
            strcpy((char *)str,"EN25QH16");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1670: 
            strcpy((char *)str,"EN25QH32");
            FlashCapacity = 0x00400000; 
            break;

        default:
            break;
    }
    
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashBg(int32_t protect)
{	
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
    switch(FlashInfo.Did)
    {
        case 0x1540:
            strcpy((char *)str,"BG25Q16A");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1340:
            strcpy((char *)str,"BG25Q40A");
            FlashCapacity = 0x00080000; 
            break;
        
        case 0x1440:
            strcpy((char *)str,"BG25Q80A");
            FlashCapacity = 0x00100000; 
            break;
        
        default:
			break;         
    }
    
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashEsmt(int32_t protect)
{
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
       
	switch(FlashInfo.Did)
    {
        case 0x1440:
            strcpy((char *)str,"F25L08QA");
            FlashCapacity = 0x00100000; 
            break;

        case 0x1540:
            strcpy((char *)str,"F25L16QA");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1641:
            strcpy((char *)str,"F25L32QA");
            FlashCapacity = 0x00400000; 
            break;

        case 0x1741:
            strcpy((char *)str,"F25L64QA");
            FlashCapacity = 0x00800000; 
            break;
              
        default:
            break;
    }    
    
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetDidInfo(int32_t protect)
{
	APP_DBG("%-30s","Did:");
	APP_DBG("0x%08X\r\n",FlashInfo.Did);
	APP_DBG("%-30s","Lock Area(BP4~BP0:Bit4~Bit0):");
	APP_DBG("0x%08X\r\n",protect);
}

void GetFlashInfo(void)
{
	int32_t protect = 0;
   
	APP_DBG("\r\n\r\n****************************************************************\n");
        APP_DBG("%-30s\r\n","Flash information");
	
	if(FlashInfo.Mid != FLASH_PCT)
	{
		protect = SpiFlashIOCtl(3,0);
		protect = (protect >> 2) & 0x1F;
	}
	
    switch(FlashInfo.Mid)
    {
        case FLASH_GD:
            APP_DBG("Manufacture:                         GD\r\n");
			GetDidInfo(protect);
            GetFlashGD(protect);
            break;
        
        case FLASH_WINBOUND:
            APP_DBG("Manufacture:                         WINBOUND\r\n");
			GetDidInfo(protect);
            GetFlashWinBound(protect);
            break;
        
        case FLASH_PCT:
            APP_DBG("Manufacture:                         PCT\r\n");
            GetFlashPct();
            break;
        
        case FLASH_EON:            
            APP_DBG("Manufacture:                         EN\r\n");
			GetDidInfo(protect);
            GetFlashEon(protect);
            break;
        
        case FLASH_BG:
            APP_DBG("Manufacture:                         BG\r\n");
			GetDidInfo(protect);
            GetFlashBg(protect);
            break;
        
        case FLASH_ESMT:
            APP_DBG("Manufacture:                         ESMT\r\n");
			GetDidInfo(protect);
            GetFlashEsmt(protect);
            break;
        
        default:            
            APP_DBG("Manufacture:                         not found\r\n");
            break;
    }
	APP_DBG("\r\n");
	APP_DBG("****************************************************************\n");
}

#endif /* DEBUG_FLASH */

OSStatus platform_flash_init( const platform_flash_t *peripheral )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_SPI ){
    SpiFlashInfoInit();
#ifdef  DEBUG_FLASH 
    SpiFlashGetInfo(&FlashInfo);
    GetFlashInfo();
#endif
    //require_action(FlashUnlock(), exit, err = kUnknownErr);
  }
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

#define MAX_OPERATE_LEN (16*1024)

OSStatus platform_flash_erase( const platform_flash_t *peripheral, uint32_t StartAddress, uint32_t EndAddress  )
{
  OSStatus err = kNoErr;
  int erase_len = MAX_OPERATE_LEN, total_len = EndAddress - StartAddress +1;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( StartAddress >= peripheral->flash_start_addr 
               && EndAddress   < peripheral->flash_start_addr + peripheral->flash_length, exit, err = kParamErr);
  

  if( peripheral->flash_type == FLASH_TYPE_SPI ){
    while(total_len > 0) {
        if (total_len > MAX_OPERATE_LEN) {
            erase_len = MAX_OPERATE_LEN;
            total_len -= MAX_OPERATE_LEN;
        } else {
            erase_len = total_len;
            total_len = 0;
        }
        err = SpiFlashErase( StartAddress, erase_len );
#ifndef MICO_DISABLE_WATCHDOG
        WdgFeed();
#endif
        require_noerr(err, exit);
        StartAddress += erase_len;
    }
    
  }else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

OSStatus platform_flash_write( const platform_flash_t *peripheral, volatile uint32_t* FlashAddress, uint8_t* Data ,uint32_t DataLength  )
{
  OSStatus err = kNoErr;
  int write_len = MAX_OPERATE_LEN, total_len = DataLength;


  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( *FlashAddress >= peripheral->flash_start_addr 
               && *FlashAddress + DataLength <= peripheral->flash_start_addr + peripheral->flash_length, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_SPI ){
    while(total_len > 0) {
        if (total_len > MAX_OPERATE_LEN) {
            write_len = MAX_OPERATE_LEN;
            total_len -= MAX_OPERATE_LEN;
        } else {
            write_len = total_len;
            total_len = 0;
        }
        err = SpiFlashWrite( *FlashAddress, Data, write_len );
#ifndef MICO_DISABLE_WATCHDOG
        WdgFeed();
#endif
        require_noerr(err, exit);
        *FlashAddress += write_len;
        Data += write_len;
    }
  }else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

OSStatus platform_flash_read( const platform_flash_t *peripheral, volatile uint32_t* FlashAddress, uint8_t* Data ,uint32_t DataLength  )
{
  OSStatus err = kNoErr;
  int read_len = MAX_OPERATE_LEN, total_len = DataLength;


  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action_quiet( DataLength != 0, exit, err = kNoErr);
  require_action( *FlashAddress >= peripheral->flash_start_addr 
               && *FlashAddress + DataLength <= peripheral->flash_start_addr + peripheral->flash_length, exit, err = kParamErr);
  
  if( peripheral->flash_type == FLASH_TYPE_SPI ){
    while(total_len > 0) {
        if (total_len > MAX_OPERATE_LEN) {
            read_len = MAX_OPERATE_LEN;
            total_len -= MAX_OPERATE_LEN;
        } else {
            read_len = total_len;
            total_len = 0;
        }
        err = SpiFlashRead( *FlashAddress, Data, read_len );
#ifndef MICO_DISABLE_WATCHDOG
        WdgFeed();
#endif
        require_noerr(err, exit);
        *FlashAddress += read_len;
        Data += read_len;
    }
  }else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

#define BP_Pos  2
#define BP_Msk (0x1Fu << BP_Pos)

#define CMP_Pos 14
#define CMP_Msk (0x1u << BP_Pos)


OSStatus platform_flash_enable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  OSStatus err = kNoErr;
  int32_t spi_err = 0;
  int32_t status = SpiFlashIOCtl( IOCTL_STATUS_REGISTER );
  UNUSED_PARAMETER( end_address );

  //platform_log("protect %d, %x, %x", peripheral->flash_type, start_address, end_address);

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);

  if( ( 0x5 == ( status & BP_Msk ) >> BP_Pos ) && ( status&CMP_Msk ) )
    return kNoErr;

  spi_err = SpiFlashIOCtl(IOCTL_FLASH_PROTECT, FLASH_HALF_PROTECT);

  //platform_log("status %x", ( SpiFlashIOCtl( IOCTL_STATUS_REGISTER ) & BP_Msk ) >> BP_Pos );

  require_action_quiet(spi_err == 0, exit, err = kUnexpectedErr);

exit:
  return err;
}

OSStatus platform_flash_disable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  OSStatus err = kNoErr;
  char cmd[3] = "\x35\xBA\x69";
  int32_t spi_err = 0;
  int32_t status = SpiFlashIOCtl( IOCTL_STATUS_REGISTER );
  UNUSED_PARAMETER( end_address );

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);

  if( ( 0x0 == ( status & BP_Msk ) >> BP_Pos ) && !( status&CMP_Msk ) )
    return kNoErr;

  spi_err = SpiFlashIOCtl(IOCTL_FLASH_UNPROTECT, cmd, sizeof(cmd));

  require_action_quiet(spi_err == 0, exit, err = kUnexpectedErr);

exit:
  return err;
}

