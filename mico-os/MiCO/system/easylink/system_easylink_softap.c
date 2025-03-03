/**
 ******************************************************************************
 * @file    system_easylink_softap.c
 * @author  William Xu
 * @version V1.0.0
 * @date    20-July-2015
 * @brief   This file provide the easylink function for quick provisioning and
 *          first time configuration.
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
#include "system_internal.h"
#include "StringUtils.h"
#include "HTTPUtils.h"
#include "SocketUtils.h"

#include "system.h"
#include "easylink_internal.h"

/* Internal vars and functions */
static mico_semaphore_t easylink_sem;         /**< Used to suspend thread while easylink. */
static mico_semaphore_t easylink_connect_sem; /**< Used to suspend thread while connection. */
static bool easylink_success = false;         /**< true: connect to wlan, false: start soft ap mode or roll back to previous settings */
static uint32_t easylinkIndentifier = 0;      /**< Unique for an easylink instance. */
static mico_thread_t easylink_softap_thread_handler = NULL;
static bool easylink_thread_force_exit = false;

/* Perform easylink and connect to wlan */
static void easylink_softap_thread( uint32_t inContext );

/* MiCO callback when WiFi status is changed */
static void easylink_wifi_status_cb( WiFiEvent event, system_context_t * const inContext )
{
    switch ( event )
    {
        case NOTIFY_STATION_UP:
            /* Connected to AP, means that the wlan configuration is right, update configuration in flash and update
             bongjour txt record with new "easylinkIndentifier" */
            easylink_bonjour_update( Station, easylinkIndentifier, inContext );
            inContext->flashContentInRam.micoSystemConfig.configured = allConfigured;
            mico_system_context_update( &inContext->flashContentInRam ); //Update Flash content
            mico_rtos_set_semaphore( &easylink_connect_sem ); //Notify Easylink thread
            break;
        case NOTIFY_AP_DOWN:
            /* Remove bonjour service under soft ap interface */
            mdns_suspend_record( "_easylink_config._tcp.local.", Soft_AP, true );
            break;
        default:
            break;
    }
}

OSStatus ConfigIncommingJsonMessageUAP( int fd, const uint8_t *input, size_t size, system_context_t * const inContext )
{
    OSStatus err = kNoErr;
    json_object *new_obj;
    char *input_str = NULL;
    uint8_t *httpResponse = NULL;
    size_t httpResponseLen = 0;

    inContext->flashContentInRam.micoSystemConfig.easyLinkByPass = EASYLINK_BYPASS_NO;

    input_str = calloc( size + 1, sizeof(char) );
    require_action( input_str, exit, err = kNoMemoryErr );

    memcpy( input_str, input, size );
    system_log("Recv config object=%s", input_str);
    new_obj = json_tokener_parse( input_str );
    require_action( new_obj, exit, err = kUnknownErr );

    json_object_object_foreach( new_obj, key, val )
    {
        if ( !strcmp( key, "SSID" ) ) {
            strncpy( inContext->flashContentInRam.micoSystemConfig.ssid, json_object_get_string( val ), maxSsidLen );
            inContext->flashContentInRam.micoSystemConfig.channel = 0;
            memset( inContext->flashContentInRam.micoSystemConfig.bssid, 0x0, 6 );
            inContext->flashContentInRam.micoSystemConfig.security = SECURITY_TYPE_AUTO;
            memcpy( inContext->flashContentInRam.micoSystemConfig.key,  inContext->flashContentInRam.micoSystemConfig.user_key, maxKeyLen );
            inContext->flashContentInRam.micoSystemConfig.keyLength = inContext->flashContentInRam.micoSystemConfig.user_keyLength;
        }
        else if ( !strcmp( key, "PASSWORD" ) ) {
            inContext->flashContentInRam.micoSystemConfig.security = SECURITY_TYPE_AUTO;
            strncpy( inContext->flashContentInRam.micoSystemConfig.key, json_object_get_string( val ), maxKeyLen );
            strncpy( inContext->flashContentInRam.micoSystemConfig.user_key, json_object_get_string( val ), maxKeyLen );
            inContext->flashContentInRam.micoSystemConfig.keyLength = strlen( inContext->flashContentInRam.micoSystemConfig.key );
            inContext->flashContentInRam.micoSystemConfig.user_keyLength = strlen( inContext->flashContentInRam.micoSystemConfig.key );
            memcpy( inContext->flashContentInRam.micoSystemConfig.key, inContext->flashContentInRam.micoSystemConfig.user_key, maxKeyLen );
            inContext->flashContentInRam.micoSystemConfig.keyLength = inContext->flashContentInRam.micoSystemConfig.user_keyLength;
        }
        else if ( !strcmp( key, "DHCP" ) ) {
            inContext->flashContentInRam.micoSystemConfig.dhcpEnable = json_object_get_boolean( val );
        }
        else if ( !strcmp( key, "IDENTIFIER" ) ) {
            easylinkIndentifier = (uint32_t) json_object_get_int( val );
        }
        else if ( !strcmp( key, "IP" ) ) {
            strncpy( inContext->flashContentInRam.micoSystemConfig.localIp, json_object_get_string( val ), maxIpLen );
        }
        else if ( !strcmp( key, "NETMASK" ) ) {
            strncpy( inContext->flashContentInRam.micoSystemConfig.netMask, json_object_get_string( val ), maxIpLen );
        }
        else if ( !strcmp( key, "GATEWAY" ) ) {
            strncpy( inContext->flashContentInRam.micoSystemConfig.gateWay, json_object_get_string( val ), maxIpLen );
        }
        else if ( !strcmp( key, "DNS1" ) ) {
            strncpy( inContext->flashContentInRam.micoSystemConfig.dnsServer, json_object_get_string( val ), maxIpLen );
        }
    }
    json_object_put( new_obj );

    err =  CreateSimpleHTTPOKMessage( &httpResponse, &httpResponseLen );
    require_noerr( err, exit );

    err = SocketSend( fd, httpResponse, httpResponseLen );
    require_noerr( err, exit );

    mico_rtos_delay_milliseconds( 1000 );

    easylink_success = true;
    if( easylink_sem ) mico_rtos_set_semaphore( &easylink_sem );
#ifdef MICO_EASYLINK_AND_SOFTAP_ENABLED
    easylink_uap_success(easylinkIndentifier);
#endif
exit:
    if ( input_str ) free( input_str );

    if(httpResponse) free(httpResponse);
    return err;
}

void easylink_softap_thread( uint32_t inContext )
{
    system_log_trace();
    OSStatus err = kNoErr;
    system_context_t *context = (system_context_t *) inContext;
    network_InitTypeDef_st wNetConfig;

    easylinkIndentifier = 0x0;
    easylink_success = false;

    easylink_thread_force_exit = false;

    mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED, (void *) easylink_wifi_status_cb, (void *) inContext );

    mico_rtos_init_semaphore( &easylink_sem, 1 );
    mico_rtos_init_semaphore( &easylink_connect_sem, 1 );

restart:
    micoWlanSuspend( );
    mico_thread_msleep( 20 );

    mico_system_delegate_config_will_start( );

    memset( &wNetConfig, 0, sizeof(network_InitTypeDef_st) );
    wNetConfig.wifi_mode = Soft_AP;
    snprintf( wNetConfig.wifi_ssid, 32, "EasyLink_%c%c%c%c%c%c",
              context->micoStatus.mac[9], context->micoStatus.mac[10], context->micoStatus.mac[12],
              context->micoStatus.mac[13], context->micoStatus.mac[15], context->micoStatus.mac[16] );
    strcpy( (char*) wNetConfig.wifi_key, "" );
    strcpy( (char*) wNetConfig.local_ip_addr, "10.10.10.1" );
    strcpy( (char*) wNetConfig.net_mask, "255.255.255.0" );
    strcpy( (char*) wNetConfig.gateway_ip_addr, "10.10.10.1" );
    wNetConfig.dhcpMode = DHCP_Server;
    micoWlanStart( &wNetConfig );
    system_log("Establish soft ap: %s.....", wNetConfig.wifi_ssid);

    /* Start bonjour service for device discovery under soft ap mode */
    err = easylink_bonjour_start( Soft_AP, 0, context );
    require_noerr( err, exit );

    while( mico_rtos_get_semaphore( &easylink_sem, 0 ) == kNoErr );
    err = mico_rtos_get_semaphore( &easylink_sem, MICO_WAIT_FOREVER );

    micoWlanSuspendSoftAP();

    /* Easylink force exit by user, clean and exit */
    if( err != kNoErr && easylink_thread_force_exit )
    {
        system_log("EasyLink canceled by user");
        goto exit;
    }

    /* EasyLink Success */
    if ( easylink_success == true ) {
        mico_system_delegate_config_recv_ssid( context->flashContentInRam.micoSystemConfig.ssid,
                                               context->flashContentInRam.micoSystemConfig.user_key );

        mico_thread_sleep(1);
        system_connect_wifi_normal( context );

        /* Wait for station connection */
        while ( mico_rtos_get_semaphore( &easylink_connect_sem, 0 ) == kNoErr );
        err = mico_rtos_get_semaphore( &easylink_connect_sem, EasyLink_ConnectWlan_Timeout );
        /* Easylink force exit by user, clean and exit */
        if ( err != kNoErr && easylink_thread_force_exit )
        {
            micoWlanSuspend( );
            system_log("EasyLink connection canceled by user");
            goto exit;
        }

        /*SSID or Password is not correct, module cannot connect to wlan, so restart EasyLink again*/
        require_noerr_action_string( err, restart, micoWlanSuspend(), "Re-start easylink softap mode" );
        mico_system_delegate_config_success( CONFIG_BY_SOFT_AP );

        /* Start bonjour service for new device discovery */
        err = easylink_bonjour_start( Station, easylinkIndentifier, context );
        require_noerr( err, exit );
        SetTimer( 60 * 1000, easylink_remove_bonjour );

        goto exit;
    }
    else /* EasyLink failed */
    {
        /*so roll back to previous settings  (if it has) and connect*/
        if(context->flashContentInRam.micoSystemConfig.configured != unConfigured)
        {
            system_log("Roll back to previous settings");
            MICOReadConfiguration( context );
#ifdef EasyLink_Needs_Reboot
            context->flashContentInRam.micoSystemConfig.configured = allConfigured;
            mico_system_context_update( &context->flashContentInRam );
#endif
            system_connect_wifi_normal( context );
        }
        else {
            /*module should power down in default setting*/
            system_log("Wi-Fi power off");
            micoWlanPowerOff( );
        }

    }

exit:
    easylink_thread_force_exit = false;

    mico_system_delegate_config_will_stop( );

    mico_system_notify_remove( mico_notify_WIFI_STATUS_CHANGED, (void *) easylink_wifi_status_cb );

#ifndef MICO_CONFIG_SERVER_ENABLE
    config_server_stop( );
#endif 

    mico_rtos_deinit_semaphore( &easylink_sem );
    mico_rtos_deinit_semaphore( &easylink_connect_sem );
    easylink_softap_thread_handler = NULL;
    mico_rtos_delete_thread( NULL );
}

OSStatus mico_easylink_softap( mico_Context_t * const in_context, mico_bool_t enable )
{
    OSStatus err = kUnknownErr;

    require_action( in_context, exit, err = kNotPreparedErr );

    easylink_remove_bonjour( );

    /* easylink soft thread existed? stop! */
    if ( easylink_softap_thread_handler ) {
        system_log("EasyLink SoftAP processing, force stop..");
        easylink_thread_force_exit = true;
        mico_rtos_thread_force_awake( &easylink_softap_thread_handler );
        mico_rtos_thread_join( &easylink_softap_thread_handler );
    }

    if ( enable == MICO_TRUE ) {
        /* Start config server */
        err = config_server_start( );
        require_noerr( err, exit );

        err = mico_rtos_create_thread( &easylink_softap_thread_handler, MICO_APPLICATION_PRIORITY, "EASYLINK AP",
                                       easylink_softap_thread, 0x1000, (mico_thread_arg_t) in_context );
        require_noerr_string( err, exit, "ERROR: Unable to start the EasyLink thread." );

        /* Make sure easylink softap is already running, and waiting for sem trigger */
        mico_rtos_delay_milliseconds( 1000 );
    }

    exit:
    return err;
}

//#endif

