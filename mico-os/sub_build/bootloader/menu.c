/**
 ******************************************************************************
 * @file    menu.c
 * @author  William Xu
 * @version V2.0.0
 * @date    05-Oct-2014
 * @brief   his file provides the software which contains the main menu routine.
 *          The main menu gives the options of:
 *             - downloading a new binary file,
 *             - uploading internal flash memory,
 *             - executing the binary file already loaded
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

/* Includes ------------------------------------------------------------------*/
#include "mico.h"
#include "ymodem.h"
#include "platform_config.h"
#include "platform_internal.h"
#include "StringUtils.h"
#include "bootloader.h"
#include <ctype.h>                    

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CMD_STRING_SIZE       128
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern platform_flash_t platform_flash_peripherals[];

uint8_t tab_1024[1024] =
{
  0
};

char FileName[FILE_NAME_LENGTH];
char ERROR_STR [] = "\n\r*** ERROR: %s\n\r";    /* ERROR message string in code   */

extern char menu[];
extern void getline (char *line, int n);          /* input line               */
extern void startApplication( uint32_t app_addr );

/* Private function prototypes -----------------------------------------------*/
void SerialDownload(mico_flash_t flash, uint32_t flashdestination, int32_t maxRecvSize);
void SerialUpload(mico_flash_t flash, uint32_t flashdestination, char * fileName, int32_t maxRecvSize);

/* Private functions ---------------------------------------------------------*/
/**
* @brief  Analyse a command parameter
* @param  commandBody: command string address
* @param  para: The para we are looking for
* @param  paraBody: A pointer to the buffer to receive the para body.
* @param  paraBodyLength: The length, in bytes, of the buffer pointed to by the paraBody parameter.
* @retval the actual length of the paraBody received, -1 means failed to find this paras 
*/
int findCommandPara(char *commandBody, char *para, char *paraBody, int paraBodyLength)
{
  int i = 0;
  int k, j;
  int retval = -1;
  char para_in_ram[100];
  strncpy(para_in_ram, para, 100);
  
  for (i = 0; para_in_ram[i] != 0; i++)  {                /* convert to upper characters */
    para_in_ram[i] = toupper(para_in_ram[i]);
  }
  
  i = 0;
  while(commandBody[i] != 0) {
    if(commandBody[i] == '-' ){
      for(j=i+1, k=0; *(para_in_ram+k)!=0x0; j++, k++ ){
        if(commandBody[j] != *(para_in_ram+k)){
          break;
        } 
      }
      
      if(*(para+k)!=0x0 || (commandBody[j]!=' '&& commandBody[j]!=0x0)){   /* para not found!             */
        i++;
        continue;
      }
      
      retval = 0;
      for (k = j+1; commandBody[k] == ' '; k++);      /* skip blanks                 */
      for(j = 0; commandBody[k] != ' ' && commandBody[k] != 0 && commandBody[k] != '-'; j++, k++){   /* para body found!             */
        if(paraBody) paraBody[j] = commandBody[k];
        retval ++;
        if( retval == paraBodyLength) goto exit;
      }
      goto exit;
    }
    i++;
  }
  
exit:
  if(paraBody) paraBody[retval] = 0x0;
  return retval;
}


/**
* @brief  Download a file via serial port
* @param  None
* @retval None
*/
void SerialDownload(mico_flash_t flash, uint32_t flashdestination, int32_t maxRecvSize)
{
  char Number[10] = "          ";
  int32_t Size = 0;
  
  printf("Waiting for the file to be sent ... (press 'a' to abort)\r\n");
  Size = Ymodem_Receive( &tab_1024[0], flash, flashdestination, maxRecvSize );
  if (Size > 0)
  {
    printf("\n\n\r Successfully!\n\r\r\n Name: %s", FileName);
    
    Int2Str((uint8_t *)Number, Size);
    printf("\n\r Size: %s Bytes\r\n", Number);
  }
  else if (Size == -1)
  {
    printf("\n\n\rImage size is higher than memory!\n\r");
  }
  else if (Size == -2)
  {
    printf("\n\n\rVerification failed!\r\n");
  }
  else if (Size == -3)
  {
    printf("\r\n\nAborted.\r\n");
  }
  else
  {
    printf("\n\rReceive failed!\r\n");
  }
}

/**
* @brief  Upload a file via serial port.
* @param  None
* @retval None
*/
void SerialUpload(mico_flash_t flash, uint32_t flashdestination, char * fileName, int32_t maxRecvSize)
{
  uint8_t status = 0;
  uint8_t key;
  
  printf("Select Receive File\n\r");
  MicoUartRecv( STDIO_UART, &key, 1, MICO_NEVER_TIMEOUT );
  
  if (key == CRC16)
  {
    /* Transmit the flash image through ymodem protocol */
    status = Ymodem_Transmit(flash, flashdestination, (uint8_t *)fileName, maxRecvSize);
    
    if (status != 0)
    {
      printf("\n\rError while Transmitting\n\r");
    }
    else
    {
      printf("\n\rSuccessfully\n\r");
    }
  }
}

/**
* @brief  Display the Main Menu on HyperTerminal
* @param  None
* @retval None
*/
void Main_Menu(void)
{
  char cmdbuf [CMD_STRING_SIZE] = {0}, cmdname[15] = {0};     /* command input buffer        */
  int i, j;                                       /* index for command buffer    */
  char idStr[4], startAddressStr[10], endAddressStr[10], flash_dev_str[4];
  int32_t id, startAddress, endAddress;
  bool inputFlashArea = false;
  mico_logic_partition_t *partition;
  mico_flash_t flash_dev;
  OSStatus err = kNoErr;
  
  while (1)  {                                    /* loop forever                */
    printf ("\n\rMXCHIP> ");
#if defined __GNUC__
    fflush(stdout);
#endif
    getline (&cmdbuf[0], sizeof (cmdbuf));        /* input command line          */
    
    for (i = 0; cmdbuf[i] == ' '; i++);           /* skip blanks on head         */
    for (; cmdbuf[i] != 0; i++)  {                /* convert to upper characters */
      cmdbuf[i] = toupper(cmdbuf[i]);
    }
    
    for (i = 0; cmdbuf[i] == ' '; i++);        /* skip blanks on head         */
    for(j=0; cmdbuf[i] != ' '&&cmdbuf[i] != 0; i++,j++)  {         /* find command name       */
      cmdname[j] = cmdbuf[i];
    }
    cmdname[j] = '\0';
    
    /***************** Command "0" or "BOOTUPDATE": Update the application  *************************/
    if(strcmp(cmdname, "BOOTUPDATE") == 0 || strcmp(cmdname, "0") == 0) {
      partition = MicoFlashGetInfo( MICO_PARTITION_BOOTLOADER );
      if (findCommandPara(cmdbuf, "r", NULL, 0) != -1){
        printf ("\n\rRead Bootloader...\n\r");
        SerialUpload( partition->partition_owner, partition->partition_start_addr, "BootLoaderImage.bin", partition->partition_length );
        continue;
      }
      printf ("\n\rUpdating Bootloader...\n\r");
      err = MicoFlashDisableSecurity( MICO_PARTITION_BOOTLOADER, 0x0, partition->partition_length );
      require_noerr( err, exit);

      SerialDownload( partition->partition_owner, partition->partition_start_addr, partition->partition_length );
    }
    
    /***************** Command "1" or "FWUPDATE": Update the MICO application  *************************/
    else if(strcmp(cmdname, "FWUPDATE") == 0 || strcmp(cmdname, "1") == 0)	{
      partition = MicoFlashGetInfo( MICO_PARTITION_APPLICATION );
      if (findCommandPara(cmdbuf, "r", NULL, 0) != -1){
        printf ("\n\rRead application...\n\r");
        SerialUpload( partition->partition_owner, partition->partition_start_addr, "ApplicationImage.bin", partition->partition_length );
        continue;
      }
      printf ("\n\rUpdating application...\n\r");
      err = MicoFlashDisableSecurity( MICO_PARTITION_APPLICATION, 0x0, partition->partition_length );
      require_noerr( err, exit);
      SerialDownload( partition->partition_owner, partition->partition_start_addr, partition->partition_length ); 							   	
    }
    
    /***************** Command "2" or "DRIVERUPDATE": Update the RF driver  *************************/
    else if(strcmp(cmdname, "DRIVERUPDATE") == 0 || strcmp(cmdname, "2") == 0) {
      partition = MicoFlashGetInfo( MICO_PARTITION_RF_FIRMWARE );
      if( partition == NULL ){
        printf ("\n\rNo flash memory for RF firmware, exiting...\n\r");
        continue;
      }
      
      if (findCommandPara(cmdbuf, "r", NULL, 0) != -1){
        printf ("\n\rRead RF firmware...\n\r");
        SerialUpload( partition->partition_owner, partition->partition_start_addr, "DriverImage.bin", partition->partition_length );
        continue;
      }
      printf ("\n\rUpdating RF driver...\n\r");
      err = MicoFlashDisableSecurity( MICO_PARTITION_RF_FIRMWARE, 0x0, partition->partition_length );
      require_noerr( err, exit);
      SerialDownload( partition->partition_owner, partition->partition_start_addr, partition->partition_length );    
    }
    
    /***************** Command "3" or "PARAUPDATE": Update the application  *************************/
    else if(strcmp(cmdname, "PARUPDATE") == 0 || strcmp(cmdname, "3") == 0)  {
      if (findCommandPara(cmdbuf, "id", idStr, 0) != -1){
        if(Str2Int((uint8_t *)idStr, &id)==0 && id > 0 && id < MICO_PARTITION_MAX ){ //Found Flash start address
          printf ("\n\rIllegal start address.\n\r");
          continue;
        }
        partition = MicoFlashGetInfo( (mico_partition_t)id );
      }else{
        printf ("\n\rPlease input correct MiCO partition id.\n\r");
        continue;
      }

      if( findCommandPara(cmdbuf, "e", NULL, 0) != -1 ){
        printf( "\n\rErasing %s...\n\r", partition->partition_description );

        err = MicoFlashDisableSecurity( (mico_partition_t)id, 0x0, partition->partition_length );
        require_noerr( err, exit);
        MicoFlashErase( (mico_partition_t)id, 0x0, partition->partition_length );
        continue;
      }
      if (findCommandPara(cmdbuf, "r", NULL, 0) != -1){
        printf ( "\n\rRead %s...\n\r", partition->partition_description );
        SerialUpload( partition->partition_owner, partition->partition_start_addr, "Image.bin", partition->partition_length );
        continue;
      }
      printf ("\n\rUpdating %s...\n\r", partition->partition_description );
      err = MicoFlashDisableSecurity( (mico_partition_t)id, 0x0, partition->partition_length );
      require_noerr( err, exit);
      SerialDownload( partition->partition_owner, partition->partition_start_addr, partition->partition_length );                        
    }
    
    /***************** Command "4" or "FLASHUPDATE": : Update the Flash  *************************/
    else if(strcmp(cmdname, "FLASHUPDATE") == 0 || strcmp(cmdname, "4") == 0) {
      if (findCommandPara(cmdbuf, "dev", flash_dev_str, 1) == -1  ){
        printf ("\n\rUnkown target type! Exiting...\n\r");
        continue;
      }
      
      if(Str2Int((uint8_t *)flash_dev_str, (int32_t *)&flash_dev)==0){ 
        printf ("\n\rDevice Number Err! Exiting...\n\r");
        continue;
      }
      if( flash_dev >= MICO_FLASH_MAX ){
        printf ("\n\rDevice Err! Exiting...\n\r");
        continue;
      }
      
      inputFlashArea = false;
      
      if (findCommandPara(cmdbuf, "start", startAddressStr, 10) != -1){
        if(Str2Int((uint8_t *)startAddressStr, &startAddress)==0){ //Found Flash start address
          printf ("\n\rIllegal start address.\n\r");
          continue;
        }else{
          if (findCommandPara(cmdbuf, "end", endAddressStr, 10) != -1){ //Found Flash end address
            if(Str2Int((uint8_t *)endAddressStr, &endAddress)==0){
              printf ("\n\rIllegal end address.\n\r");
              continue;
            }else{
              inputFlashArea = true;
            }
          }else{
            printf ("\n\rFlash end address not found.\n\r");
            continue;
          }
        }
      }
      
      if(endAddress<startAddress && inputFlashArea == true) {
        printf ("\n\rIllegal address.\n\r");
        continue;
      }
      
      if(inputFlashArea != true){
        startAddress = platform_flash_peripherals[ flash_dev ].flash_start_addr ;
        endAddress = platform_flash_peripherals[ flash_dev ].flash_start_addr 
          + platform_flash_peripherals[ flash_dev ].flash_length - 1;
      }
      
      if (findCommandPara(cmdbuf, "e", NULL, 0) != -1){
        printf ("\n\rErasing dev%d content From 0x%lx to 0x%lx\n\r", flash_dev, startAddress, endAddress);
        platform_flash_init( &platform_flash_peripherals[ flash_dev ] );
        platform_flash_disable_protect( &platform_flash_peripherals[ flash_dev ], startAddress, endAddress );
        platform_flash_erase( &platform_flash_peripherals[ flash_dev ], startAddress, endAddress );
        continue;
      }
      
      if (findCommandPara(cmdbuf, "r", NULL, 0) != -1){
        printf ("\n\rRead dev%d content From 0x%lx to 0x%lx\n\r", flash_dev, startAddress, endAddress);
        SerialUpload(flash_dev, startAddress, "FlashImage.bin", endAddress-startAddress+1);
        continue;
      }
      
      printf ("\n\rUpdating dev%d content From 0x%lx to 0x%lx\n\r", flash_dev, startAddress, endAddress);
      platform_flash_disable_protect( &platform_flash_peripherals[ flash_dev ], startAddress, endAddress );
      SerialDownload(flash_dev, startAddress, endAddress-startAddress+1);                           
    }
    
    
    /***************** Command: MEMORYMAP *************************/
    else if(strcmp(cmdname, "MEMORYMAP") == 0 || strcmp(cmdname, "5") == 0)  {
      printf("\r");
      for( i = 0; i <= MICO_PARTITION_MAX - 1; i++ ){
        partition = MicoFlashGetInfo( (mico_partition_t)i );
        if (partition->partition_owner == MICO_FLASH_NONE || partition->partition_description == NULL )
            continue;
        printf( "|ID:%d| %11s |  Dev:%d  | 0x%08lx | 0x%08lx |\r\n", i, partition->partition_description, partition->partition_owner,
               partition->partition_start_addr, partition->partition_length);
      }
    }
    /***************** Command: Excute the application *************************/
    else if(strcmp(cmdname, "BOOT") == 0 || strcmp(cmdname, "6") == 0)	{
      printf ("\n\rBooting.......\n\r");
      partition = MicoFlashGetInfo( MICO_PARTITION_APPLICATION );
      bootloader_start_app( partition->partition_start_addr );
    }
    
    /***************** Command: Reboot *************************/
    else if(strcmp(cmdname, "REBOOT") == 0 || strcmp(cmdname, "7") == 0)  {
      printf ("\n\rReBooting.......\n\r");
      MicoSystemReboot();
      break;                              
    }
    
    else if(strcmp(cmdname, "HELP") == 0 || strcmp(cmdname, "?") == 0)	{
        printf ( menu, MODEL, Bootloader_REVISION, HARDWARE_REVISION );  /* display command menu        */
      break;
    }
    
    else if(strcmp(cmdname, "") == 0 )	{                         
      break;
    }
    else{
      printf (ERROR_STR, "UNKNOWN COMMAND");
      break;
    }

exit:
    continue;
  }
}
