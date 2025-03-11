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
#include "mico_platform.h"
#include "platform.h"

#include "stdio.h"
#ifdef USE_MICO_SPI_FLASH
#include "spi_flash.h"
#endif
#include "flash_efc.h"

/* Private constants --------------------------------------------------------*/
#define ADDR_FLASH_SECTOR_00     ((uint32_t)0x00400000) /* base @ of sector 0, 8 kbyte */
#define ADDR_FLASH_SECTOR_01     ((uint32_t)0x00402000) /* base @ of sector 1, 8 kbyte */
#define ADDR_FLASH_SECTOR_02     ((uint32_t)0x00404000) /* base @ of sector 2, 112 kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x00420000) /* base @ of sector 1, 128 kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x00440000) /* base @ of sector 2, 128 kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x00480000) /* base @ of sector 3, 128 kbyte */

/* End of the Flash address */
#define FLASH_START_ADDRESS     IFLASH_ADDR //internal flash
#define FLASH_END_ADDRESS       (IFLASH_ADDR + IFLASH_SIZE - 1)
#define FLASH_SIZE              IFLASH_SIZE


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* critical disable irq */
#define mem_flash_op_enter() do {\
		__DSB(); __ISB();        \
		cpu_irq_disable();       \
		__DMB();                 \
	} while (0)
/* critical enable irq */
#define mem_flash_op_exit() do {  \
		__DMB(); __DSB(); __ISB();\
		cpu_irq_enable();         \
	} while (0)
          
typedef enum
{
    SAMG55_FLASH_ERASE_4_PAGES = 0x04,
    SAMG55_FLASH_ERASE_8_PAGES = 0x08,
    SAMG55_FLASH_ERASE_16_PAGES = 0x10,
    SAMG55_FLASH_ERASE_32_PAGES = 0x20,
} samg55_flash_erase_page_amount_t;



/* Private variables ---------------------------------------------------------*/
#ifdef USE_MICO_SPI_FLASH
static sflash_handle_t sflash_handle = {0x0, 0x0, SFLASH_WRITE_NOT_ALLOWED};
#endif
/* Private function prototypes -----------------------------------------------*/
static OSStatus internalFlashInitialize( void );
static OSStatus internalFlashErase(uint32_t StartAddress, uint32_t EndAddress);
static OSStatus internalFlashWrite(volatile uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength);

#ifdef MCU_EBANLE_FLASH_PROTECT
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

static OSStatus internalFlashInitialize( void )
{ 
  platform_log_trace();
  OSStatus err = kNoErr;
  platform_mcu_powersave_disable();

  require_action( flash_init(FLASH_ACCESS_MODE_128, 6) == FLASH_RC_OK, exit, err = kGeneralErr );
  
exit:
  platform_mcu_powersave_enable();
  return err; 
}


OSStatus internalFlashErase(uint32_t start_address, uint32_t end_address)
{
  platform_log_trace();
  uint32_t i;
  OSStatus err = kNoErr;
  uint32_t page_start_address, page_end_address;
  uint32_t page_start_number, page_end_number;

  platform_mcu_powersave_disable();

  require_action( flash_unlock( start_address, end_address, &page_start_address, &page_end_address ) == FLASH_RC_OK, exit, err = kGeneralErr );

  page_start_number = page_start_address/512;
  page_end_number   = page_end_address/512;
  require_action( page_end_number >= page_start_number + 16, exit, err = kUnsupportedErr);

  for ( i = page_start_number; i <= page_end_number; i+=16 )
  {
    require_action( flash_erase_page( i * 512, IFLASH_ERASE_PAGES_16) == FLASH_RC_OK, exit, err = kGeneralErr );
  }

  require_action( flash_lock( start_address, end_address, NULL, NULL ) == FLASH_RC_OK, exit, err = kGeneralErr );

exit:
  platform_mcu_powersave_enable();
  return err;
}



OSStatus internalFlashWrite(volatile uint32_t* flash_address, uint32_t* data ,uint32_t data_length)
{
  platform_log_trace();
  OSStatus err = kNoErr;
  uint32_t start_address = * flash_address;

  platform_mcu_powersave_disable();

  //mem_flash_op_enter();
  require_action( flash_unlock( start_address, start_address + data_length - 1, NULL, NULL ) == FLASH_RC_OK, exit, err = kGeneralErr );

  require_action( flash_write((*flash_address), data, data_length, false) == FLASH_RC_OK, exit, err = kGeneralErr );
  *flash_address += data_length;

  require_action( flash_lock( start_address, start_address + data_length - 1, NULL, NULL ) == FLASH_RC_OK, exit, err = kGeneralErr );
  //mem_flash_op_exit();

exit:
  platform_mcu_powersave_enable();
  return err;
}

#ifdef MCU_EBANLE_FLASH_PROTECT 
OSStatus internalFlashProtect(uint32_t StartAddress, uint32_t EndAddress, bool enable)
{
  OSStatus err = kNoErr;
  UNUSED_PARAMETER( StartAddress );
  UNUSED_PARAMETER( EndAddress );
  UNUSED_PARAMETER( enable );

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
    require_action(sflash_sector_erase(&sflash_handle, i<<12) == kNoErr, exit, err = kWriteErr); 
  }
  
exit:
  return err;
}
#endif
