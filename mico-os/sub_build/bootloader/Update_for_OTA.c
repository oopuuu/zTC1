/**
 ******************************************************************************
 * @file    update_for_OTA.c
 * @author  William Xu
 * @version V2.0.0
 * @date    05-Oct-2014
 * @brief   This file provides functions to overwrite the target flash contents
 *          using according the stored in OTA temporary storage
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include "mico.h"
#include "platform.h"
#include "platform_config.h"
#include "CheckSumUtils.h"

typedef int Log_Status;					
#define Log_NotExist		        (1)
#define Log_NeedUpdate			    (2)
#define Log_UpdateTagNotExist		(3)
#define Log_contentTypeNotExist     (4)
#define Log_dataLengthOverFlow      (5)
#define Log_StartAddressERROR		(6)
#define Log_UnkonwnERROR            (7)
#define Log_CRCERROR                (8)

#define SizePerRW 4096   /* Bootloader need 2xSizePerRW RAM heap size to operate, 
                            but it can boost the setup. */

static uint8_t data[SizePerRW];
static uint8_t newData[SizePerRW];
uint8_t paraSaveInRam[16*1024];

#define update_log(M, ...) custom_log("UPDATE", M, ##__VA_ARGS__)
#define update_log_trace() custom_log_trace("UPDATE")

static OSStatus checkcrc(uint16_t crc_in, int partition_type, int total_len)
{
    uint16_t crc = 0;
    mico_logic_partition_t* part;
    int len;
    OSStatus err = kNoErr;
    uint32_t update_data_offset = 0x0;
    CRC16_Context contex;

    CRC16_Init( &contex );
    
    if (crc_in == 0xFFFF)
        goto exit;

    part = MicoFlashGetInfo((mico_partition_t)partition_type);
    if (part == NULL)
        goto exit;

    while(total_len > 0){
      if( SizePerRW < total_len ){
        len = SizePerRW;
      } else {
        len = total_len;
      }
      err = MicoFlashRead( MICO_PARTITION_OTA_TEMP, &update_data_offset, data , len);
      require_noerr(err, exit);

      total_len -= len;

      CRC16_Update( &contex, data, len );
    }

  CRC16_Final( &contex, &crc );
    if (crc == crc_in)
        err = kNoErr;
exit:
    update_log("CRC check return %d, got crc %x, calcuated crc %x", err, crc_in, crc);
    return err;
}

Log_Status updateLogCheck( boot_table_t *updateLog, mico_partition_t *dest_partition_type )
{
    uint32_t i;

    for ( i = 0; i < sizeof(boot_table_t); i++ )
    {
        if ( *((uint8_t *) updateLog + i) != 0xff )
            break;
    }
    if ( i == sizeof(boot_table_t) )
        return Log_NotExist;

    if ( updateLog->upgrade_type != 'U' )
        return Log_UpdateTagNotExist;

    if ( updateLog->start_address
        != MicoFlashGetInfo( MICO_PARTITION_OTA_TEMP )->partition_start_addr )
        return Log_StartAddressERROR;

    if ( updateLog->type == 'B' )
        *dest_partition_type = MICO_PARTITION_BOOTLOADER;
    else if ( updateLog->type == 'A' )
        *dest_partition_type = MICO_PARTITION_APPLICATION;
    else if ( updateLog->type == 'D' )
        *dest_partition_type = MICO_PARTITION_RF_FIRMWARE;
    else
        return Log_contentTypeNotExist;

    if ( updateLog->length > MicoFlashGetInfo( *dest_partition_type )->partition_length )
        return Log_dataLengthOverFlow;

    if ( checkcrc( updateLog->crc, *dest_partition_type, updateLog->length ) != kNoErr )
        return Log_CRCERROR;

    return Log_NeedUpdate;
}


OSStatus update(void)
{
  boot_table_t updateLog;
  uint32_t i, j, size;
  uint32_t update_data_offset = 0x0;
  uint32_t dest_offset;
  uint32_t boot_table_offset = 0x0;
  uint32_t para_offset = 0x0;
  uint32_t copyLength;
  //uint8_t *paraSaveInRam = NULL;
  mico_logic_partition_t *ota_partition_info, *dest_partition_info, *para_partition_info;
  mico_partition_t dest_partition;
  OSStatus err = kNoErr;

  ota_partition_info = MicoFlashGetInfo(MICO_PARTITION_OTA_TEMP);
  require_action( ota_partition_info->partition_owner != MICO_FLASH_NONE, exit, err = kUnsupportedErr );
  
  para_partition_info = MicoFlashGetInfo(MICO_PARTITION_PARAMETER_1);
  require_action( para_partition_info->partition_owner != MICO_FLASH_NONE, exit, err = kUnsupportedErr );
  
  memset(data, 0xFF, SizePerRW);
  memset(newData, 0xFF, SizePerRW);

  //paraSaveInRam = malloc( para_partition_info->partition_length );
  //require_action( paraSaveInRam, exit, err = kNoMemoryErr );
  memset(paraSaveInRam, 0xFF, para_partition_info->partition_length);
    
  err = MicoFlashRead( MICO_PARTITION_PARAMETER_1, &boot_table_offset, (uint8_t *)&updateLog, sizeof(boot_table_t));
  require_noerr(err, exit);

  /*Not a correct record*/
  if(updateLogCheck( &updateLog, &dest_partition) != Log_NeedUpdate){
    size = ( ota_partition_info->partition_length )/SizePerRW;
    for(i = 0; i <= size; i++){
      if( i==size ){
        err = MicoFlashRead( MICO_PARTITION_OTA_TEMP , &update_data_offset, data , ( ota_partition_info->partition_length )%SizePerRW );
        require_noerr(err, exit);
      }
      else{
        err = MicoFlashRead( MICO_PARTITION_OTA_TEMP, &update_data_offset, data , SizePerRW);
        require_noerr(err, exit);
      }
      
      for(j=0; j<SizePerRW; j++){
        if(data[j] != 0xFF){
          update_log("Update data need to be erased");
          err = MicoFlashDisableSecurity( MICO_PARTITION_OTA_TEMP, 0x0, ota_partition_info->partition_length );
          require_noerr(err, exit);
          err = MicoFlashErase( MICO_PARTITION_OTA_TEMP, 0x0, ota_partition_info->partition_length );
          require_noerr(err, exit);
          goto exit;
        }
      }
    }
    goto exit;
  }

  dest_partition_info = MicoFlashGetInfo( dest_partition );
  require_action( dest_partition_info->partition_owner != MICO_FLASH_NONE, exit, err = kUnsupportedErr );
  
  update_log("Write OTA data to partition: %s, length %ld",
    dest_partition_info->partition_description, updateLog.length);
  
  dest_offset = 0x0;
  update_data_offset = 0x0;
  
  err = MicoFlashDisableSecurity( dest_partition, 0x0, dest_partition_info->partition_length );
  require_noerr(err, exit);
  err = MicoFlashErase( dest_partition, 0x0, dest_partition_info->partition_length );
  require_noerr(err, exit);
  size = (updateLog.length)/SizePerRW;
  
  for(i = 0; i <= size; i++){
    if( i == size ){
      if( (updateLog.length)%SizePerRW )
        copyLength = (updateLog.length)%SizePerRW;
      else
        break;
    }else{
      copyLength = SizePerRW;
    }
    err = MicoFlashRead( MICO_PARTITION_OTA_TEMP, &update_data_offset, data , copyLength);
    require_noerr(err, exit);
    err = MicoFlashWrite( dest_partition, &dest_offset, data, copyLength);
    require_noerr(err, exit);
    dest_offset -= copyLength;
    err = MicoFlashRead( dest_partition, &dest_offset, newData , copyLength);
    require_noerr(err, exit);
    err = memcmp(data, newData, copyLength);
    require_noerr_action(err, exit, err = kWriteErr); 
 }

  update_log("Update start to clear data...");
    
  para_offset = 0x0;
  err = MicoFlashDisableSecurity( MICO_PARTITION_PARAMETER_1, 0x0, para_partition_info->partition_length );
  require_noerr(err, exit);
  err = MicoFlashRead( MICO_PARTITION_PARAMETER_1, &para_offset, paraSaveInRam, para_partition_info->partition_length );
  require_noerr(err, exit);
  memset(paraSaveInRam, 0xff, sizeof(boot_table_t));
  err = MicoFlashErase( MICO_PARTITION_PARAMETER_1, 0x0, para_partition_info->partition_length );
  require_noerr(err, exit);
  para_offset = 0x0;
  err = MicoFlashWrite( MICO_PARTITION_PARAMETER_1, &para_offset, paraSaveInRam, para_partition_info->partition_length );
  require_noerr(err, exit);
  

  err = MicoFlashDisableSecurity( MICO_PARTITION_OTA_TEMP, 0x0, ota_partition_info->partition_length );
  require_noerr(err, exit);  
  err = MicoFlashErase( MICO_PARTITION_OTA_TEMP, 0x0, ota_partition_info->partition_length );
  require_noerr(err, exit);
  update_log("Update success");
  
exit:
  if(err != kNoErr) update_log("Update exit with err = %d", err);
  return err;
}



