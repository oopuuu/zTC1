/**
 ******************************************************************************
 * @file    qc_test_ble.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Dec-2016
 * @brief   This file provide the BLE QC test function.
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

#ifdef QC_TEST_BLUETOOTH_ENABLE

#include "qc_test_internal.h"

#include "mico_bt.h"
#include "mico_bt_cfg.h"
#include "mico_bt_dev.h"
#include "mico_bt_smart_interface.h"
#include "mico_bt_smartbridge.h"
#include "mico_bt_smartbridge_gatt.h"

#include "StringUtils.h"

/******************************************************
 *               Function Declarations
 ******************************************************/

static void ble_scan( void );

/******************************************************
 *               Function Definitions
 ******************************************************/

void qc_test_ble( void )
{
    char str[128];
    uint8_t mac[6];

    mico_bt_init( MICO_BT_HCI_MODE, "SmartBridge Device", 0, 0 );  //Client + server connections
    mico_bt_smartbridge_init( 0 );

    mico_bt_dev_read_local_addr( mac );
    sprintf( str, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
    QC_TEST_PRINT_STRING( "Local Bluetooth Address:", str );

    ble_scan( );
}

/* Scan complete handler. Scan complete event reported via this callback.
 * It runs on the MICO_BT_WORKER_THREAD context.
 */
static OSStatus scan_complete_handler( void *arg )
{
    UNUSED_PARAMETER( arg );
    OSStatus err = kNoErr;
    uint32_t count = 0;
    mico_bt_smart_scan_result_t *scan_result = NULL;

    mf_printf( "BLE scan complete\r\n" );
    err = mico_bt_smartbridge_get_scan_result_list( &scan_result, &count );
    require_noerr( err, exit );

    if ( count == 0 )
    {
        mf_printf( "No ble device found\r\n" );
        err = kNotFoundErr;
        goto exit;
    }
    mf_printf( "\r\n" );
exit:
    /* Scan duration is complete */
    return err;
}

static OSStatus ble_scan_handler( const mico_bt_smart_advertising_report_t* result )
{
    OSStatus err = kNoErr;
    char* bd_addr_str = NULL;
    char str[128];

    bd_addr_str = DataToHexStringWithColons( (uint8_t *) result->remote_device.address, 6 );
    snprintf( str, 128, "  ADDR: %s, RSSI: %d", bd_addr_str, result->signal_strength );
    mf_printf( str );
    free( bd_addr_str );
    mf_printf( "\r\n" );
    
    /* Scan duration is complete */
    return err;
}

void ble_scan( void )
{
    uint32_t count = 0;
    mico_bt_smart_scan_result_t *scan_result = NULL;
    /* Scan settings */
    mico_bt_smart_scan_settings_t scan_settings;

    scan_settings.type = BT_SMART_PASSIVE_SCAN;
    scan_settings.filter_policy = FILTER_POLICY_NONE;
    scan_settings.filter_duplicates = DUPLICATES_FILTER_ENABLED;
    scan_settings.interval = MICO_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_INTERVAL;
    scan_settings.window = MICO_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_WINDOW;
    scan_settings.duration_second = 2;
    scan_settings.type = BT_SMART_PASSIVE_SCAN;

    /* Start scan */
    mico_bt_smartbridge_start_scan( &scan_settings, scan_complete_handler, ble_scan_handler );

    mico_rtos_delay_milliseconds( 2 * 1000 );

    mico_bt_smartbridge_stop_scan( );

    mf_printf( "BLE scan complete\r\n" );
    mico_bt_smartbridge_get_scan_result_list( &scan_result, &count );

    if ( count == 0 )
    {
        mf_printf( "No BLE device found\r\n" );
    }

    mf_printf( "\r\n" );
}

#endif
