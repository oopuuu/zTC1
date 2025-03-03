/**
 ******************************************************************************
 * @file    oled.c
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    17-Mar-2015
 * @brief     OLED controller.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mico.h"
#include "platform.h"

#include "oled.h"
#include "oledfont.h"  	 

#define oled_log(M, ...) custom_log("OLED", M, ##__VA_ARGS__)

static void delay_ms(u16 nms);

#ifdef SSD1106_USE_I2C
static uint8_t _oled_avalid = 0;
/* I2C device */
mico_i2c_device_t ssd1106_i2c_device = {
  OLED_I2C_PORT, 0x3C, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};

OSStatus ssd1106_i2c_bus_write(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
  OSStatus err = kNoErr;
  require_action_quiet(_oled_avalid == 1, exit, err = kNotFoundErr);
  mico_i2c_message_t ssd1106_i2c_msg = {NULL, NULL, 0, 0, 10, false};

  uint8_t array[128 + 1];
  uint8_t stringpos;
  array[0] = reg_addr;

  if(cnt > 128) return kParamErr;
  for (stringpos = 0; stringpos < cnt; stringpos++) {
          array[stringpos + 1] = *(reg_data + stringpos);
  }

  err = MicoI2cBuildTxMessage(&ssd1106_i2c_msg, array, cnt + 1, 3);
  require_noerr( err, exit );
  err = MicoI2cTransfer(&ssd1106_i2c_device, &ssd1106_i2c_msg, 1);
  require_noerr( err, exit );
  
exit:
  return err;
}

#else
extern platform_spi_driver_t            platform_spi_drivers[];
extern const platform_gpio_t            platform_gpio_pins[];
extern const platform_spi_t             platform_spi_peripherals[];

const mico_spi_device_t micokit_spi_oled =
{
    .port        = OLED_SPI_PORT,
    .chip_select = OLED_SPI_CS,
    .speed       = 20000000,
    .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_USE_DMA | SPI_MSB_FIRST),
    .bits        = 8
};
#endif

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   



void OLED_WR_Bytes(u8 *dat, u8 len, u8 cmd)
{  
#ifdef SSD1106_USE_I2C
  if(cmd)
    ssd1106_i2c_bus_write(0x40, dat, len);
  else
    ssd1106_i2c_bus_write(0x00, dat, len);
#else

#if defined (MOC) && (MOC == 1)
  platform_spi_message_segment_t oled_spi_msg =
            { dat,            NULL,       (unsigned long) len };

  OLED_DC_INIT();
  if(cmd)
    OLED_DC_Set();
  else
    OLED_DC_Clr();

  MicoSpiTransfer( &micokit_spi_oled, &oled_spi_msg, 1 );

#else
  platform_spi_config_t config;
  platform_spi_message_segment_t oled_spi_msg =
            { dat,            NULL,       (unsigned long) len };

  if (micokit_spi_oled.port >= MICO_SPI_NONE )
    return;

  config.chip_select = &platform_gpio_pins[micokit_spi_oled.chip_select];
  config.speed       = micokit_spi_oled.speed;
  config.mode        = micokit_spi_oled.mode;
  config.bits        = micokit_spi_oled.bits;
  
  if( platform_spi_drivers[micokit_spi_oled.port].spi_mutex == NULL)
    mico_rtos_init_mutex( &platform_spi_drivers[micokit_spi_oled.port].spi_mutex );

  mico_rtos_lock_mutex( &platform_spi_drivers[micokit_spi_oled.port].spi_mutex );

  platform_spi_init( &platform_spi_drivers[micokit_spi_oled.port], &platform_spi_peripherals[micokit_spi_oled.port], &config );
  OLED_DC_INIT();   

  if(cmd)
    OLED_DC_Set();
  else 
    OLED_DC_Clr();      

  platform_spi_transfer( &platform_spi_drivers[micokit_spi_oled.port], &config, &oled_spi_msg, 1 );
  
  OLED_DC_Set();   

  mico_rtos_unlock_mutex( &platform_spi_drivers[micokit_spi_oled.port].spi_mutex );
#endif //defined (MOC) && (MOC == 1)
#endif  //SSD1106_USE_I2C
}

void OLED_WR_Byte(u8 dat, u8 cmd)
{
  OLED_WR_Bytes(&dat, 1, cmd);
}


void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
  uint8_t tmp[3] = {0xb0+y, ((x&0xf0)>>4)|0x10, (x&0x0f)|0x01};
  OLED_WR_Bytes( tmp, 3, OLED_CMD);
}   	  
//����OLED��ʾ    
void OLED_Display_On(void)
{
  uint8_t tmp[3] = {0X8D, 0X14, 0XAF};
  OLED_WR_Bytes( tmp, 3, OLED_CMD);
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
  uint8_t tmp[3] = {0X8D, 0X10, 0XAE};
  OLED_WR_Bytes( tmp, 3, OLED_CMD);
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
  u8 i;
  uint8_t tmp_cmd[3] = {0X0, 0x00, 0x10};
  uint8_t tmp[128];
  memset( tmp, 0x0, 128 );
  for(i=0;i<8;i++)  
  {  
    tmp_cmd[0] = 0xb0+i;
    OLED_WR_Bytes( tmp_cmd, 3, OLED_CMD);
    OLED_WR_Bytes( tmp, 128, OLED_DATA);
  } //������ʾ
}


//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
  unsigned char c=0;	
  c=chr-' ';//�õ�ƫ�ƺ��ֵ			
  if(x>Max_Column-1){x=0;y=y+2;}
  if(SIZE ==16)
  {
    OLED_Set_Pos(x,y);	
    OLED_WR_Bytes( (uint8_t *)&F8X16[c*16], 8, OLED_DATA );
    OLED_Set_Pos(x,y+1);
    OLED_WR_Bytes( (uint8_t *)&F8X16[c*16+8], 8, OLED_DATA );
  }
  else {	
    OLED_Set_Pos(x,y+1);
    OLED_WR_Bytes( (uint8_t *)F6x8[c], 6, OLED_DATA );
    
  }
}
//m^n����
u32 oled_pow(u8 m,u8 n)
{
  u32 result=1;	 
  while(n--)result*=m;    
  return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
  u8 t,temp;
  u8 enshow=0;						   
  for(t=0;t<len;t++)
  {
    temp=(num/oled_pow(10,len-t-1))%10;
    if(enshow==0&&t<(len-1))
    {
      if(temp==0)
      {
        OLED_ShowChar(x+(size/2)*t,y,' ');
        continue;
      }else enshow=1; 
      
    }
    OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
  }
} 
//��ʾһ���ַ��Ŵ�
void OLED_ShowString(u8 x,u8 y,char *chr)
{
  unsigned char j=0;
  u8 x_t = x,y_t = y;
  
  while (chr[j]!='\0')
  {	
    // add for CR/LF
    if( ('\r' == chr[j]) && ('\n' == chr[j+1]) ){  // CR LF
      while(x_t <= 120){  // fill rest chars in current line
        OLED_ShowChar(x_t,y_t,' ');
        x_t += 8;
      }
      j += 2;
    }
    else if( ('\r' == chr[j]) || ('\n' == chr[j]) ){   // CR or LF
      while(x_t <= 120){  // fill rest chars in current line
        OLED_ShowChar(x_t,y_t,' ');
        x_t += 8;
      }
      j += 1;
    }
    else{
      if(x_t>120){  // line end, goto next line
        x_t = 0;
        y_t += 2;
        if(y_t >= 8){  // can only display 4 line
          break;
        }
      }
      OLED_ShowChar(x_t,y_t,chr[j]);
      x_t += 8;
      j++;
    }
  }
}

//��ʾ����
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
  u8 t,adder=0;
  OLED_Set_Pos(x,y);	
  for(t=0;t<16;t++)
  {
    OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
    adder+=1;
  }	
  OLED_Set_Pos(x,y+1);	
  for(t=0;t<16;t++)
  {	
    OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
    adder+=1;
  }					
}
/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
  unsigned int j=0;
  unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {      
      OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
    }
  }
} 


//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	 
#ifdef SSD1106_USE_I2C
  if( kNoErr != MicoI2cInitialize( &ssd1106_i2c_device ) )
  {
      oled_log( "OLED_ERROR: MicoI2cInitialize err." );
      return;
  }
  if( false == MicoI2cProbeDevice(&ssd1106_i2c_device, 5) ){
      oled_log("OLED_ERROR: no i2c device found!");
      return;
  }
  _oled_avalid = 1;
#else
  MicoSpiInitialize( &micokit_spi_oled );
  OLED_DC_INIT();   
  OLED_DC_Clr();
#endif
 
  OLED_RST_Clr();
  
  OLED_RST_Set();
  delay_ms(100);
  OLED_RST_Clr();
  delay_ms(100);
  OLED_RST_Set(); 

  
  OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
  OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
  OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
  OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
  OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
  OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
  OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
  OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
  OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
  OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
  OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WR_Byte(0x00,OLED_CMD);//-not offset
  OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
  OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
  OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
  OLED_WR_Byte(0x12,OLED_CMD);
  OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
  OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
  OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WR_Byte(0x02,OLED_CMD);//
  OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
  OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
  OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
  OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7)   
  
  OLED_Clear();
  OLED_Set_Pos(0,0); 	
  OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 

  return;
}  



////////////////////////////////////////////////////////////////////////////////

static void delay_ms(u16 nms)
{
  mico_thread_msleep(nms);
}



























