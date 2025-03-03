/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include "build_info.h"
#include "PinNames.h"
#include "serial_api.h"

extern void serial_init       (serial_t *obj, PinName tx, PinName rx);
extern void serial_free       (serial_t *obj);
extern void serial_baud       (serial_t *obj, int baudrate);
extern void serial_format     (serial_t *obj, int data_bits, SerialParity parity, int stop_bits);
extern int main(void);

void iar_data_init_fw_loader(void);
void fw_loader_main(void);// __attribute__ ((weak));

#pragma section=".image2.start.table1"
#pragma section=".fwloader_ram.bss"

FW_LOADER_START_RAM_FUN_SECTION
RAM_START_FUNCTION gFWLoaderEntryFun0 = {fw_loader_main};

u8* __image4_entry_func__;
u8* __image4_validate_code__;
u8* __fwloader_bss_start__;
u8* __fwloader_bss_end__;

FW_LOADER_VALID_PATTEN_SECTION const u8 RAM_FW_LOADER_VALID_PATTEN[20] = {
    'R', 'T', 'K', 'W', 'i', 'n', 0x0, 0xff, 
    (FW_VERSION&0xff), ((FW_VERSION >> 8)&0xff),
    (FW_SUBVERSION&0xff), ((FW_SUBVERSION >> 8)&0xff),
    (FW_CHIP_ID&0xff), ((FW_CHIP_ID >> 8)&0xff),
    (FW_CHIP_VER),
    (FW_BUS_TYPE),
    (FW_INFO_RSV1),
    (FW_INFO_RSV2),
    (FW_INFO_RSV3),
    (FW_INFO_RSV4)    
};
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */  
void fw_loader_main(void)
{
#if defined ( __ICCARM__ )   
	iar_data_init_fw_loader();
#endif	
      	u32 Image2Len, Image2Addr, ImageIndex, SpicBitMode, SpicImageIndex;
      	u32 Image2LoadAddr = 0x13000;
	DBG_8195A("===== Enter FW Loader Image  ====\n");


#ifdef BOOTLOADER
     main();
#endif

IGMAE4:
	PRAM_START_FUNCTION Image4EntryFun=(PRAM_START_FUNCTION)__image4_entry_func__;

	Image2Len = HAL_READ32(SPI_FLASH_BASE, Image2LoadAddr);
    	Image2Addr = HAL_READ32(SPI_FLASH_BASE, (Image2LoadAddr+0x4));

    	DBG_8195A("Flash FW Loader:Addr 0x%x, Len %d, Load to SRAM 0x%x\n", Image2LoadAddr, Image2Len, Image2Addr);

    	SpicImageIndex = 0;
    	for (ImageIndex = 0x10 + Image2LoadAddr; ImageIndex < (Image2Len + Image2LoadAddr + 0x10); ImageIndex = ImageIndex + 4) {
		HAL_WRITE32(Image2Addr, SpicImageIndex,
	                HAL_READ32(SPI_FLASH_BASE, ImageIndex));

      		SpicImageIndex += 4;
    	}	



#ifdef CONFIG_SDR_EN
    u32 Image3LoadAddr;
    u32 Image3Len;
    u32 Image3Addr;

    Image3LoadAddr = Image2LoadAddr + Image2Len+0x10;
    Image3Len = HAL_READ32(SPI_FLASH_BASE, Image3LoadAddr);
    Image3Addr = HAL_READ32(SPI_FLASH_BASE, Image3LoadAddr + 0x4);

	if( (Image3Len==0xFFFFFFFF) || (Image3Len==0) || (Image3Addr!=0x30000000)){
		DBG_8195A("No Image3\n\r");
	}else{
		DBG_8195A("Image3 length: 0x%x, Image3 Addr: 0x%x\n",Image3Len, Image3Addr);
		SpicImageIndex = 0;

		for (ImageIndex = 0x10 + Image3LoadAddr; 
				ImageIndex < (Image3Len + Image3LoadAddr + 0x10);
				ImageIndex = ImageIndex + 4) {
			HAL_WRITE32(Image3Addr, SpicImageIndex,
						HAL_READ32(SPI_FLASH_BASE, ImageIndex));

			SpicImageIndex += 4;
		}
	}
#endif	
    //3 	3) Jump to image 4
    	DBG_8195A("InfraStart: %p, Img2 Sign %s \n", __image4_entry_func__, (char*)__image4_validate_code__);
    	if (_strcmp((char *)__image4_validate_code__, "RTKWin")) {
      		while (1) {
              	DBG_8195A("Invalid Image4 Signature\n");
            		RtlConsolRom(1000);//each delay is 100us
        	}
    	}
		
#ifdef BOOTLOADER
      deinit_platform_bootloader();			
#endif
    	Image4EntryFun->RamStartFun();	
}

void iar_data_init_fw_loader(void)
{
    __image4_entry_func__    =  (u8*)__section_begin(".image2.start.table1");
	__image4_validate_code__	= __image4_entry_func__+4;//(u8*)__section_begin(".image2.start.table2");
	__fwloader_bss_start__		= (u8*)__section_begin(".fwloader_ram.bss");
	__fwloader_bss_end__			= (u8*)__section_end(".fwloader_ram.bss");	
}
