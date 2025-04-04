/**
 ******************************************************************************
 * @file    system_easylink.c
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
#include "easylink_internal.h"

#include "StringUtils.h"

//#if (MICO_WLAN_CONFIG_MODE == CONFIG_MODE_EASYLINK) || (MICO_WLAN_CONFIG_MODE == CONFIG_MODE_EASYLINK_WITH_SOFTAP)

/******************************************************
 *               Function Declarations
 ******************************************************/
/* EasyLink event callback functions*/
static void easylink_wifi_status_cb( WiFiEvent event, system_context_t * const inContext );
static void easylink_complete_cb( network_InitTypeDef_st *nwkpara, system_context_t * const inContext );
static void easylink_extra_data_cb( int datalen, char* data, system_context_t * const inContext );

/* Thread perform easylink and connect to wlan */
static void easylink_thread( uint32_t inContext ); /* Perform easylink and connect to wlan */

/******************************************************
 *               Variables Definitions
 ******************************************************/
static mico_semaphore_t easylink_sem;         /**< Used to suspend thread while easylink. */
static mico_semaphore_t easylink_connect_sem; /**< Used to suspend thread while connection. */
static bool easylink_success = false;         /**< true: connect to wlan, false: start soft ap mode or roll back to previous settings */
static uint32_t easylinkIndentifier = 0;      /**< Unique for an easylink instance. */
static mico_thread_t easylink_thread_handler = NULL;
static bool easylink_thread_force_exit = false;

static mico_config_source_t source = CONFIG_BY_NONE;
static bool easylink_with_softap = false;
/******************************************************
 *               Function Definitions
 ******************************************************/

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
        default:
            break;
    }
    return;
}

/* MiCO callback when EasyLink is finished step 1, return SSID and KEY */
static void easylink_complete_cb( network_InitTypeDef_st *nwkpara, system_context_t * const inContext )
{
    OSStatus err = kNoErr;

    require_action_string( nwkpara, exit, err = kTimeoutErr, "EasyLink Timeout or terminated" );

    /* Store SSID and KEY*/
    mico_rtos_lock_mutex( &inContext->flashContentInRam_mutex );
    memcpy( inContext->flashContentInRam.micoSystemConfig.ssid, nwkpara->wifi_ssid, maxSsidLen );
    memset( inContext->flashContentInRam.micoSystemConfig.bssid, 0x0, 6 );
    memcpy( inContext->flashContentInRam.micoSystemConfig.user_key, nwkpara->wifi_key, maxKeyLen );
    inContext->flashContentInRam.micoSystemConfig.user_keyLength = strlen( nwkpara->wifi_key );
    inContext->flashContentInRam.micoSystemConfig.dhcpEnable = true;
    mico_rtos_unlock_mutex( &inContext->flashContentInRam_mutex );
    system_log("Get SSID: %s, Key: %s", inContext->flashContentInRam.micoSystemConfig.ssid, inContext->flashContentInRam.micoSystemConfig.user_key);

    source = (mico_config_source_t) nwkpara->wifi_retry_interval;
    exit:
    if ( err != kNoErr )
    {
        /*EasyLink timeout or error*/
        easylink_success = false;
        mico_rtos_set_semaphore( &easylink_sem );
    }
    return;
}
#ifdef MICO_EASYLINK_AND_SOFTAP_ENABLED
void easylink_uap_success(uint32_t id)
{
    if (easylink_with_softap == false)
        return;

    system_log("sofapt configured");
    easylinkIndentifier = id;
    easylink_success = true;
    micoWlanSuspendSoftAP();
    mico_rtos_set_semaphore( &easylink_sem );
}

#endif
/* MiCO callback when EasyLink is finished step 2, return extra data 
 data format: [AuthData#Identifier]<localIp/netMask/gateWay/dnsServer>
 Auth data: Provide to application, application will decide if this is a proter configuration for currnet device
 Identifier: Unique id for every easylink instance send by easylink mobile app
 localIp/netMask/gateWay/dnsServer: Device static ip address, use DHCP if not exist
 */
static void easylink_extra_data_cb( int datalen, char* data, system_context_t * const inContext )
{
    OSStatus err = kNoErr;
    int index;
    uint32_t ipInfoCount;
    char *debugString;
    struct in_addr ipv4_addr;

    debugString = DataToHexStringWithSpaces( (const uint8_t *) data, datalen );
    system_log("Get user info: %s", debugString);
    free( debugString );

    /* Find '#' that separate authdata and identifier*/
    for ( index = datalen - 1; index >= 0; index-- )
    {
        if ( data[index] == '#' && ((datalen - index) == 5 || (datalen - index) == 25) )
            break;
    }
    require_action( index >= 0, exit, err = kParamErr );

    /* Check auth data by device */
    data[index++] = 0x0;
    err = mico_system_delegate_config_recv_auth_data( data );
    require_noerr( err, exit );

    /* Read identifier */
	memcpy(&easylinkIndentifier, &data[index], 4);
    /* Identifier: 1 x uint32_t or Identifier/localIp/netMask/gateWay/dnsServer: 5 x uint32_t */
    ipInfoCount = (datalen - index) / sizeof(uint32_t);
    require_action( ipInfoCount >= 1, exit, err = kParamErr );

    mico_rtos_lock_mutex( &inContext->flashContentInRam_mutex );

    if ( ipInfoCount == 1 )
    { //Use DHCP to obtain local ip address
        inContext->flashContentInRam.micoSystemConfig.dhcpEnable = true;
        system_log("Get auth info: %s, EasyLink identifier: %lx", data, easylinkIndentifier);
    } else
    { //Use static ip address
        inContext->flashContentInRam.micoSystemConfig.dhcpEnable = false;
		memcpy(&ipv4_addr.s_addr, &data[index+4], 4);
        strcpy( (char *) inContext->micoStatus.localIp, inet_ntoa( ipv4_addr ) );
		memcpy(&ipv4_addr.s_addr, &data[index+8], 4);
        strcpy( (char *) inContext->micoStatus.netMask, inet_ntoa( ipv4_addr ) );
		memcpy(&ipv4_addr.s_addr, &data[index+12], 4);
        strcpy( (char *) inContext->micoStatus.gateWay, inet_ntoa( ipv4_addr ) );
		memcpy(&ipv4_addr.s_addr, &data[index+16], 4);
        strcpy( (char *) inContext->micoStatus.dnsServer, inet_ntoa( ipv4_addr ) );

        system_log("Get auth info: %s, EasyLink identifier: %lx, local IP info:%s %s %s %s ", data, easylinkIndentifier, inContext->flashContentInRam.micoSystemConfig.localIp,
            inContext->flashContentInRam.micoSystemConfig.netMask, inContext->flashContentInRam.micoSystemConfig.gateWay,inContext->flashContentInRam.micoSystemConfig.dnsServer);
    }
    mico_rtos_unlock_mutex( &inContext->flashContentInRam_mutex );
    source = CONFIG_BY_EASYLINK_V2;

    exit:
    if ( err != kNoErr )
    {
        /*EasyLink error*/
        system_log("EasyLink step 2 ERROR, err: %d", err);
        easylink_success = false;
    } else
        /* Easylink success after step 1 and step 2 */
        easylink_success = true;

    mico_rtos_set_semaphore( &easylink_sem );
    return;
}

static void easylink_thread( uint32_t arg )
{
    OSStatus err = kNoErr;
    system_context_t *context = (system_context_t *) arg;

    easylinkIndentifier = 0x0;
    easylink_success = false;
    easylink_thread_force_exit = false;

    source = CONFIG_BY_NONE;
    mico_system_notify_register( mico_notify_EASYLINK_WPS_COMPLETED,    (void *) easylink_complete_cb,      context );
    mico_system_notify_register( mico_notify_EASYLINK_GET_EXTRA_DATA,   (void *) easylink_extra_data_cb,    context );
    mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED,       (void *) easylink_wifi_status_cb,   context );

    mico_rtos_init_semaphore( &easylink_sem,            1 );
    mico_rtos_init_semaphore( &easylink_connect_sem,    1 );

restart:
    mico_system_delegate_config_will_start( );
    system_log("Start easylink combo mode");
#ifdef MICO_EASYLINK_AND_SOFTAP_ENABLED
    if (easylink_with_softap == true) {
        char wifi_ssid[32];
        sprintf( wifi_ssid, "EasyLink_%c%c%c%c%c%c",
              context->micoStatus.mac[9], context->micoStatus.mac[10], context->micoStatus.mac[12],
              context->micoStatus.mac[13], context->micoStatus.mac[15], context->micoStatus.mac[16]);

        system_log("Enable softap %s in easylink", wifi_ssid);
        OpenEasylink_softap( EasyLink_TimeOut / 1000, wifi_ssid, NULL, 6 );
        /* Start config server */
        config_server_start( );
        easylink_bonjour_start( Soft_AP, 0, context );
    } else 
#endif
    micoWlanStartEasyLinkPlus( EasyLink_TimeOut / 1000 );

    while( mico_rtos_get_semaphore( &easylink_sem, 0 ) == kNoErr );
    err = mico_rtos_get_semaphore( &easylink_sem, EasyLink_TimeOut );

    /* Easylink force exit by user, clean and exit */
    if( err != kNoErr && easylink_thread_force_exit )
    {
        system_log("EasyLink waiting for terminate");
        micoWlanStopEasyLinkPlus( );
        mico_rtos_get_semaphore( &easylink_sem, 3000 );
        system_log("EasyLink canceled by user");
        goto exit;
    }

    /* EasyLink Success */
    if ( easylink_success == true )
    {
        mico_system_delegate_config_recv_ssid( context->flashContentInRam.micoSystemConfig.ssid,
                                               context->flashContentInRam.micoSystemConfig.user_key );
        system_connect_wifi_normal( context );

        /* Wait for station connection */
        while( mico_rtos_get_semaphore( &easylink_connect_sem, 0 ) == kNoErr );
        err = mico_rtos_get_semaphore( &easylink_connect_sem, EasyLink_ConnectWlan_Timeout );
        /* Easylink force exit by user, clean and exit */
        if( err != kNoErr && easylink_thread_force_exit )
        {
            micoWlanSuspend();
            system_log("EasyLink connection canceled by user");
            goto exit;
        }

        /*SSID or Password is not correct, module cannot connect to wlan, so restart EasyLink again*/
        require_noerr_action_string( err, restart, micoWlanSuspend(), "Re-start easylink combo mode" );
        mico_system_delegate_config_success( source );

        /* Start bonjour service for new device discovery */
        err = easylink_bonjour_start( Station, easylinkIndentifier, context );
        require_noerr( err, exit );
        SetTimer( 60 * 1000, easylink_remove_bonjour );

        goto exit;
    }
    else /* EasyLink failed */
    {
        /* so roll back to previous settings  (if it has) and connect */
        if ( context->flashContentInRam.micoSystemConfig.configured != unConfigured ) {
            system_log("Roll back to previous settings");
            MICOReadConfiguration( context );
            system_connect_wifi_normal( context );
        }
        else {
            /*module should power down in default setting*/
            system_log("Wi-Fi power off");
            micoWlanPowerOff();
        }
    }

exit:
    easylink_thread_force_exit = false;

    mico_system_delegate_config_will_stop( );

    mico_system_notify_remove( mico_notify_WIFI_STATUS_CHANGED, (void *)easylink_wifi_status_cb );
    mico_system_notify_remove( mico_notify_EASYLINK_WPS_COMPLETED, (void *)easylink_complete_cb );
    mico_system_notify_remove( mico_notify_EASYLINK_GET_EXTRA_DATA, (void *)easylink_extra_data_cb );

    mico_rtos_deinit_semaphore( &easylink_sem );
    mico_rtos_deinit_semaphore( &easylink_connect_sem );
    easylink_thread_handler = NULL;
    mico_rtos_delete_thread( NULL );
}

OSStatus mico_easylink( mico_Context_t * const in_context, mico_bool_t enable, mico_bool_t softap )
{
    OSStatus err = kUnknownErr;

    require_action( in_context, exit, err = kNotPreparedErr );

    easylink_remove_bonjour( );

    /* easylink thread existed? stop! */
    if ( easylink_thread_handler ) {
        system_log("EasyLink processing, force stop..");
        easylink_thread_force_exit = true;
        mico_rtos_thread_force_awake( &easylink_thread_handler );
        mico_rtos_thread_join( &easylink_thread_handler );
    }

    if ( enable == MICO_TRUE ) {
        if (softap == MICO_TRUE)
            easylink_with_softap = true;
        else
            easylink_with_softap = false;
        err = mico_rtos_create_thread( &easylink_thread_handler, MICO_APPLICATION_PRIORITY, "EASYLINK", easylink_thread,
                                       0x1000, (mico_thread_arg_t) in_context );
        require_noerr_string( err, exit, "ERROR: Unable to start the EasyLink thread." );

        /* Make sure easylink is already running, and waiting for sem trigger */
        mico_rtos_delay_milliseconds( 100 );
    }

    exit:
    return err;
}


//#endif

