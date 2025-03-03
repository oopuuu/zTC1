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
#include "mico_platform.h"
#include "mico_rtos.h"

#include "platform_logging.h"
#include "mico_platform.h"
#include "platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_logging.h"
#include "stdio.h"
#include "device_lock.h"
#ifdef USE_MICO_SPI_FLASH
//#include "spi_flash.h"
//#include "hal_platform.h"
//#include "hal_spi_flash.h"
//#include "rtl8195a_spi_flash.h"
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef USE_MICO_SPI_FLASH
//static sflash_handle_t sflash_handle = {0x0, 0x0, SFLASH_WRITE_NOT_ALLOWED};
#endif
/* Private function prototypes -----------------------------------------------*/

#ifdef NO_MICO_RTOS
/* for 3081 device mutext lock */
void device_mutex_lock(int flag)
{
}

void device_mutex_unlock(int flag)
{
}
#endif

void spi_flash_init(flash_t *obj){
	SPI_FLASH_PIN_FCTRL(ON);

	if (!SpicFlashInitRtl8195A(SpicOneBitMode)){

		DBG_8195A("SPI Init Fail!!!!!!\n");
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSTBY_INFO3, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSTBY_INFO3)|0xf);
	}
	else {
		//DBG_8195A("SPI Init SUCCESS\n");
	}
	return;
}

int spi_flash_stream_read(flash_t *obj, uint32_t address, uint32_t len, uint8_t * data)
{
    u32 offset_to_align;
    u32 i;
    u32 read_word;
    uint8_t *ptr;
    uint8_t *pbuf;

    spi_flash_init(obj);

    // Wait flash busy done (wip=0)
    SpicWaitWipDoneRefinedRtl8195A(obj->SpicInitPara);

    offset_to_align = address & 0x03;
    pbuf = data;
    if (offset_to_align != 0) {
        // the start address is not 4-bytes aligned
        read_word = HAL_READ32(SPI_FLASH_BASE, (address - offset_to_align));
        ptr = (uint8_t*)&read_word + offset_to_align;
        offset_to_align = 4 - offset_to_align;
        for (i=0;i<offset_to_align;i++) {
            *pbuf = *(ptr+i);
            pbuf++;
            len--;
            if (len == 0) {
                break;
            }
        }
    }

    address = (((address-1) >> 2) + 1) << 2;    // address = next 4-bytes aligned

    ptr = (uint8_t*)&read_word;
    if ((u32)pbuf & 0x03) {
        while (len >= 4) {
            read_word = HAL_READ32(SPI_FLASH_BASE, address);
            for (i=0;i<4;i++) {
                *pbuf = *(ptr+i);
                pbuf++;
            }
            address += 4;
            len -= 4;
        }
    }
    else {
        while (len >= 4) {
            *((u32 *)pbuf) = HAL_READ32(SPI_FLASH_BASE, address);
            pbuf += 4;
            address += 4;
            len -= 4;
        }
    }

    if (len > 0) {
        read_word = HAL_READ32(SPI_FLASH_BASE, address);
        for (i=0;i<len;i++) {
            *pbuf = *(ptr+i);
            pbuf++;
        }        
    }

    SpicDisableRtl8195A();
    return 1;
}

int spi_flash_stream_write(flash_t *obj, uint32_t address, uint32_t len, uint8_t * data)
{
    u32 offset_to_align;
    u32 align_addr;
    u32 i;
    u32 write_word;
    uint8_t *ptr;
    uint8_t *pbuf;

    spi_flash_init(obj);
    
    offset_to_align = address & 0x03;
    pbuf = data;
    if (offset_to_align != 0) {
        // the start address is not 4-bytes aligned
        align_addr = (address - offset_to_align);
        write_word = HAL_READ32(SPI_FLASH_BASE, align_addr);        
        ptr = (uint8_t*)&write_word + offset_to_align;
        offset_to_align = 4 - offset_to_align;
        for (i=0;i<offset_to_align;i++) {
            *(ptr+i) = *pbuf;
            pbuf++;
            len--;
            if (len == 0) {
                break;
            }
        }
        //Write word
        HAL_WRITE32(SPI_FLASH_BASE, align_addr, write_word);
        // Wait spic busy done
        SpicWaitBusyDoneRtl8195A();
        // Wait flash busy done (wip=0)
        SpicWaitWipDoneRefinedRtl8195A(obj->SpicInitPara);
    }

    address = (((address-1) >> 2) + 1) << 2;    // address = next 4-bytes aligned

    if ((u32)pbuf & 0x03) {
        while (len >= 4) {
            write_word = (u32)(*pbuf) | ((u32)(*(pbuf+1)) << 8) | ((u32)(*(pbuf+2)) << 16) | ((u32)(*(pbuf+3)) << 24);
			//Write word
            HAL_WRITE32(SPI_FLASH_BASE, address, write_word);
            // Wait spic busy done
            SpicWaitBusyDoneRtl8195A();
            // Wait flash busy done (wip=0)
            SpicWaitWipDoneRefinedRtl8195A(obj->SpicInitPara);
            pbuf += 4;
            address += 4;
            len -= 4;
        }
    }
    else {
        while (len >= 4) {
            //Write word
            HAL_WRITE32(SPI_FLASH_BASE, address, (u32)*((u32 *)pbuf));
            // Wait spic busy done
            SpicWaitBusyDoneRtl8195A();
            // Wait flash busy done (wip=0)
            SpicWaitWipDoneRefinedRtl8195A(obj->SpicInitPara);
            pbuf += 4;
            address += 4;
            len -= 4;
        }
    }

    if (len > 0) {
        write_word = HAL_READ32(SPI_FLASH_BASE, address);
        ptr = (uint8_t*)&write_word;
        for (i=0;i<len;i++) {
            *(ptr+i) = *pbuf;
            pbuf++;
        }
        //Write word
        HAL_WRITE32(SPI_FLASH_BASE, address, write_word);
        // Wait spic busy done
        SpicWaitBusyDoneRtl8195A();
        // Wait flash busy done (wip=0)
        SpicWaitWipDoneRefinedRtl8195A(obj->SpicInitPara);
    }

    SpicDisableRtl8195A();
    return 1;
}

int spi_flash_stream_burst(flash_t *obj, uint32_t address, uint32_t Length, uint8_t * data)
{
    u32 OccuSize;
    u32 ProgramSize;
    u32 PageSize;
    u8 flashtype = 0;

    PageSize = 256;

    spi_flash_init(obj);

    flashtype = obj->SpicInitPara.flashtype;
    
    OccuSize = address & 0xFF;
    if((Length >= PageSize) ||((Length + OccuSize) >= PageSize))
        ProgramSize = PageSize - OccuSize;
    else
        ProgramSize = Length;

    obj->Length = Length;
    while(Length > 0){
        if(OccuSize){
            SpicUserProgramRtl8195A(data, obj->SpicInitPara, address, &(obj->Length));
                // Wait spic busy done
            SpicWaitBusyDoneRtl8195A();
            // Wait flash busy done (wip=0)
            if(flashtype == FLASH_MICRON){
                SpicWaitOperationDoneRtl8195A(obj->SpicInitPara);
            }
            else
                SpicWaitWipDoneRefinedRtl8195A(obj->SpicInitPara);

            address += ProgramSize;
            data+= ProgramSize;   
            Length -= ProgramSize;
            OccuSize = 0;
        }
        else{
            while((obj->Length) >= PageSize){
                SpicUserProgramRtl8195A(data, obj->SpicInitPara, address, &(obj->Length));
                    // Wait spic busy done
                SpicWaitBusyDoneRtl8195A();
                // Wait flash busy done (wip=0)
                if(flashtype == FLASH_MICRON){
                    SpicWaitOperationDoneRtl8195A(obj->SpicInitPara);
                }
                else
                    SpicWaitWipDoneRefinedRtl8195A(obj->SpicInitPara);

                address += PageSize;
                data+=PageSize;
                Length -= PageSize;
            }
            obj->Length = Length;
            if((obj->Length)  > 0){
                SpicUserProgramRtl8195A(data, obj->SpicInitPara, address, &(obj->Length));
                    // Wait spic busy done
                SpicWaitBusyDoneRtl8195A();
                // Wait flash busy done (wip=0)
                if(flashtype == FLASH_MICRON){
                    SpicWaitOperationDoneRtl8195A(obj->SpicInitPara);
                }
                else
                    SpicWaitWipDoneRefinedRtl8195A(obj->SpicInitPara);

                break;
            }
        }
        obj->Length = Length;
    }
    
    SpicDisableRtl8195A();
    return 1;

}

OSStatus platform_flash_init( const platform_flash_t *peripheral )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    //err = internalFlashInitialize();
    err = kUnsupportedErr;
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
  	device_mutex_lock(RT_DEV_LOCK_FLASH);
	spi_flash_init((flash_t*)&peripheral->flash_obj);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
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
  int section_start, section_num, section_end, section_ro_start, ro_section_num, start_sector;
  int i =0;

  require_action_quiet( end_address > start_address, exit, err = kParamErr);
  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( start_address >= peripheral->flash_start_addr 
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  section_ro_start = peripheral->flash_readonly_start/0x1000;
  ro_section_num = peripheral->flash_readonly_len/0x1000;
  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    //err = internalFlashErase( start_address, end_address );   
    err = kUnsupportedErr;  
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
  	device_mutex_lock(RT_DEV_LOCK_FLASH);
    spi_flash_init((flash_t*)&peripheral->flash_obj);
	start_sector = start_address/0x1000;
	section_start = start_sector*0x1000;
	section_end = (end_address+0x1000-1)/0x1000*0x1000;
	section_num = (section_end - section_start)/0x1000;
	section_ro_start = section_ro_start - start_sector;
	for(i =0; i < section_num; i++){
		if (i == section_ro_start) {// read only section don't write 
			i += ro_section_num;
			i--;
			continue;
		}
		SpicSectorEraseFlashRtl8195A(SPI_FLASH_BASE + section_start + i*0x1000);	
	}
    SpicDisableRtl8195A();
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
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
  uint32_t ro_start, ro_end;
  uint32_t start = *start_address, end=*start_address + length;
  uint32_t write_len;

  ro_start = peripheral->flash_readonly_start;
  ro_end = peripheral->flash_readonly_len + ro_start;
  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( *start_address >= peripheral->flash_start_addr 
               && *start_address + length <= peripheral->flash_start_addr + peripheral->flash_length, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    //err = internalFlashWrite( start_address, (uint32_t *)data, length); 
    err = kUnsupportedErr;    
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
  	device_mutex_lock(RT_DEV_LOCK_FLASH);
	//spi_flash_stream_write(peripheral, *start_address, length, data);
	if ((start > ro_end) || (end < ro_start)) {
		spi_flash_stream_write((flash_t *)&peripheral->flash_obj, start, length, data);
	} else {
		if (start < ro_start) {
			write_len = ro_start - start;
			spi_flash_stream_write((flash_t *)&peripheral->flash_obj, start, write_len, data);
		} 
		if (ro_end < end) {// start is in ro_start~ro_end
			write_len = ro_end - start;
			data += write_len;
			spi_flash_stream_write((flash_t *)&peripheral->flash_obj, ro_end, length - write_len, data);
		}
	}
    *start_address += length;
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
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
    //memcpy(data, (void *)(*start_address), length);
    //*start_address += length;
    err = kUnsupportedErr;
    require_noerr(err, exit);    
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
  	device_mutex_lock(RT_DEV_LOCK_FLASH);
    spi_flash_stream_read((flash_t*)&peripheral->flash_obj, (*start_address), length, data);
    *start_address += length;
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
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
//#ifdef MCU_EBANLE_FLASH_PROTECT
//    err = internalFlashProtect( start_address, end_address, true );  
//#endif  
    err = kUnsupportedErr;
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
    //err = internalFlashProtect( start_address, end_address, false );    
    err = kUnsupportedErr;
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

