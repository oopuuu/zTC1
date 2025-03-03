/**
 ******************************************************************************
 * @file    qc_test.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Dec-2016
 * @brief   This file provide the QC test functions called before application's
 *          entrance: application_start().
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mico.h"

#include "qc_test.h"
#include "qc_test_internal.h"

#include "StringUtils.h"
#include "CheckSumUtils.h"

/******************************************************
*                      Macros
******************************************************/

#define QC_UART_BUFFER_SIZR     (50)
#define QC_UART_BAUDRATE        (921600)

/******************************************************
*                    Constants
******************************************************/

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Function Declarations
******************************************************/

extern int mfg_scan(void); //TODO: Get it from MiCO core libraries

static uint8_t* _qc_test_uart_init( void );
static void _qc_test_thread( mico_thread_arg_t arg );

/******************************************************
*               Variables Definitions
******************************************************/

extern int mico_debug_enabled;
static volatile ring_buffer_t  rx_buffer;

/******************************************************
*               Function Definitions
******************************************************/

/* Initialize QC test UART interface */
static uint8_t* _qc_test_uart_init( void )
{
    mico_uart_config_t uart_config;

    uint8_t* pbuffer = malloc( QC_UART_BUFFER_SIZR );

    if( pbuffer )
    {
        uart_config.baud_rate    = QC_UART_BAUDRATE;
        uart_config.data_width   = DATA_WIDTH_8BIT;
        uart_config.parity       = NO_PARITY;
        uart_config.stop_bits    = STOP_BITS_1;
        uart_config.flow_control = FLOW_CONTROL_DISABLED;
        uart_config.flags        = UART_WAKEUP_DISABLE;

        ring_buffer_init( (ring_buffer_t *) &rx_buffer, (uint8_t *) pbuffer, QC_UART_BUFFER_SIZR );
        MicoUartInitialize( MFG_TEST, &uart_config, (ring_buffer_t *) &rx_buffer );
    }
    return pbuffer;
}

/* Calculate Application firmware's CRC */
static void _qc_test_calculate_app_crc( char *str, int len )
{
    mico_logic_partition_t *partition_flash = MicoFlashGetInfo( MICO_PARTITION_APPLICATION );
    uint8_t *mfgbuf = malloc( 1024 );
    uint16_t crc = 0;
    uint32_t flash_addr = 0x0;
    int flash_len = partition_flash->partition_length;

    CRC16_Context mfg_context;
    CRC16_Init( &mfg_context );

    while ( flash_len > 0 )
    {
        int buf_len = (flash_len > 1024) ? 1024 : flash_len;
        flash_len -= buf_len;

        MicoFlashRead( MICO_PARTITION_APPLICATION, &flash_addr, (uint8_t *) mfgbuf, buf_len );
        CRC16_Update( &mfg_context, (uint8_t *) mfgbuf, buf_len );
    }

    CRC16_Final( &mfg_context, &crc );
    snprintf( str, len, "%04X", crc );
    if ( mfgbuf ) free( mfgbuf );
}

void mico_system_qc_test( void )
{
    mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "QC Test", _qc_test_thread, (2048 * 4), 0 );
}


/* MXCHIP standard QC test function main entrance, available for all modules */
static void _qc_test_thread( mico_thread_arg_t arg )
{
    char str[128];
    uint8_t mac[6];
    uint8_t * rx_data = NULL;

    mico_debug_enabled = 0;
    mxchipInit( );

    rx_data = _qc_test_uart_init( );
    require( rx_data, exit );

    mf_printf( "==== MXCHIP Manufacture Test ====\r\n" );
    QC_TEST_PRINT_STRING( "Serial Number:", SERIAL_NUMBER );
    QC_TEST_PRINT_STRING_FUN( "App CRC:", _qc_test_calculate_app_crc );
    QC_TEST_PRINT_STRING( "Bootloader Version:", mico_get_bootloader_ver( ) );
    QC_TEST_PRINT_STRING( "Library Version:", MicoGetVer() );
    QC_TEST_PRINT_STRING_FUN( "APP Version:", mico_app_info );
    QC_TEST_PRINT_STRING_FUN( "Driver:", wlan_driver_version );

#ifdef QC_TEST_GPIO_ENABLE
    qc_test_gpio();
#endif

#ifdef QC_TEST_BLUETOOTH_ENABLE
    qc_test_ble();
#endif

    mico_wlan_get_mac_address( mac );
    sprintf( str, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
    QC_TEST_PRINT_STRING( "MAC:", str );

    qc_scan( );

    qc_test_tcpip( );

exit:
    free( rx_data );
    mico_thread_sleep( MICO_NEVER_TIMEOUT );
    mico_rtos_delete_thread( NULL );
}

#ifdef MFG_MODE_AUTO
static void uartRecvMfg_thread( mico_thread_arg_t arg );
static size_t _uart_get_one_packet(uint8_t* inBuf, int inBufLen);

void mico_mfg_test(mico_Context_t *inContext)
{
  network_InitTypeDef_adv_st wNetConfig;
  int testCommandFd, scanFd;
  uint8_t *buf = NULL;
  int recvLength = -1;
  fd_set readfds;
  struct timeval t;
  struct sockaddr_in addr;
  socklen_t addrLen = sizeof(addr);
  mico_uart_config_t uart_config;
  volatile ring_buffer_t  rx_buffer;
  volatile uint8_t *       rx_data;
  OSStatus err;
  mico_system_status_wlan_t* wlan_status;
  
  mico_system_get_status_wlan( &wlan_status );

  buf = malloc(1500);
  require_action(buf, exit, err = kNoMemoryErr);
  rx_data = malloc(2048);
  require_action(rx_data, exit, err = kNoMemoryErr);
  
  /* Connect to a predefined Wlan */
  memset( &wNetConfig, 0x0, sizeof(network_InitTypeDef_adv_st) );
  
  strncpy( (char*)wNetConfig.ap_info.ssid, "William Xu", maxSsidLen );
  wNetConfig.ap_info.security = SECURITY_TYPE_AUTO;
  memcpy( wNetConfig.key, "mx099555", maxKeyLen );
  wNetConfig.key_len = strlen( "mx099555" );
  wNetConfig.dhcpMode = DHCP_Client;
  
  wNetConfig.wifi_retry_interval = 100;
  micoWlanStartAdv(&wNetConfig);
  
  /* Initialize UART interface */
  uart_config.baud_rate    = 115200;
  uart_config.data_width   = DATA_WIDTH_8BIT;
  uart_config.parity       = NO_PARITY;
  uart_config.stop_bits    = STOP_BITS_1;
  uart_config.flow_control = FLOW_CONTROL_DISABLED;
  uart_config.flags = UART_WAKEUP_DISABLE;
  
  ring_buffer_init  ( (ring_buffer_t *)&rx_buffer, (uint8_t *)rx_data, 2048 );
  MicoUartInitialize( UART_FOR_APP, &uart_config, (ring_buffer_t *)&rx_buffer );
  err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "MFG UART Recv", uartRecvMfg_thread, 0x300, 0 );
  
  /* Initialize UDP interface */
  t.tv_sec = 5;
  t.tv_usec = 0;
  
  scanFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  require_action(IsValidSocket( scanFd ), exit, err = kNoResourcesErr );
  
  addr.sin_family = AF_INET;
  addr.sin_port = htons(23230);
  addr.sin_addr.s_addr = INADDR_ANY;
  err = bind(scanFd, (struct sockaddr *)&addr, sizeof(addr));
  require_noerr(err, exit);
  
  testCommandFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  require_action(IsValidSocket( testCommandFd ), exit, err = kNoResourcesErr );
  
  addr.sin_port = htons(23231);
  err = bind(testCommandFd, (struct sockaddr *)&addr, sizeof(addr));
  require_noerr(err, exit);
  
  while(1) {
    /*Check status on erery sockets on bonjour query */
    FD_ZERO( &readfds );
    FD_SET( testCommandFd, &readfds );
    FD_SET( scanFd, &readfds );
    select( 1, &readfds, NULL, NULL, &t );
    
    /* Scan and return MAC address */ 
    if (FD_ISSET(scanFd, &readfds)) {
      recvLength = recvfrom(scanFd, buf, 1500, 0, (struct sockaddr *)&addr, &addrLen);
      sendto(scanFd, wlan_status->mac, sizeof(wlan_status->mac), 0, (struct sockaddr *)&addr, addrLen);
    }
    
    /* Recv UDP data and send to COM */
    if (FD_ISSET(testCommandFd, &readfds)) {
      recvLength = recvfrom(testCommandFd, buf, 1500, 0, (struct sockaddr *)&addr, &addrLen);
      MicoUartSend(UART_FOR_APP, buf, recvLength);
    }
  }
  
exit:
  if(buf) free(buf);  
}

void uartRecvMfg_thread( mico_thread_arg_t arg)
{
  mico_Context_t *Context = mico_system_context_get();
  int recvlen;
  uint8_t *inDataBuffer;
  
  inDataBuffer = malloc(500);
  require(inDataBuffer, exit);
  
  while(1) {
    recvlen = _uart_get_one_packet(inDataBuffer, 500);
    if (recvlen <= 0)
      continue; 
    else{
      /* if(......)   Should valid the UART input */
      Context->micoSystemConfig.configured = unConfigured;
      mico_system_context_update ( Context );
    }
  }
  
exit:
  if(inDataBuffer) free(inDataBuffer);
}


static size_t _uart_get_one_packet(uint8_t* inBuf, int inBufLen)
{
  
  int datalen;
  
  while(1) {
    if( MicoUartRecv( UART_FOR_APP, inBuf, inBufLen, 500) == kNoErr){
      return inBufLen;
    }
    else{
      datalen = MicoUartGetLengthInBuffer( UART_FOR_APP );
      if(datalen){
        MicoUartRecv(UART_FOR_APP, inBuf, datalen, 500);
        return datalen;
      }
    }
    
  }
}
#endif

/* QC test demo END */














