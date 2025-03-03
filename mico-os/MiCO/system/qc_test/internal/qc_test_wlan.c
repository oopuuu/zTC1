/**
 ******************************************************************************
 * @file    qc_test_tcpip.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Dec-2016
 * @brief   This file provide the TCPIP test functions, connect to an unsecured
 *          wlan according to user input and start tcp/udp test.
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
#include "qc_test_internal.h"

/******************************************************
*               Function Declarations
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/


static void micoNotify_ApListCallback( ScanResult *pApList )
{
    int i = 0;
    char str[48];
    mf_printf( "Scan AP Success:\r\n" );
    for ( i = 0; i < pApList->ApNum; i++ ) {
        snprintf(str, 48, "  SSID: %s, RSSI: %d\r\n", pApList->ApList[i].ssid, pApList->ApList[i].rssi);
        mf_printf( str );
    }
}

void qc_scan( void )
{
    /* Register user function when wlan scan is completed */
    mico_system_notify_register( mico_notify_WIFI_SCAN_COMPLETED, (void *) micoNotify_ApListCallback, NULL );

    micoWlanStartScan();

    mico_rtos_delay_milliseconds(2000);
}




