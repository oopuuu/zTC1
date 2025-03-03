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
#include "StringUtils.h"

#include "system.h"
#include "easylink_internal.h"

//#if (MICO_WLAN_CONFIG_MODE == CONFIG_MODE_USER)

/******************************************************
 *               Function Declarations
 ******************************************************/
/* Perform easylink and connect to wlan */
static void easylink_usr_thread( uint32_t inContext );

/******************************************************
 *               Variables Definitions
 ******************************************************/
static mico_semaphore_t easylink_connect_sem; /**< Used to suspend thread while connection. */
static mico_thread_t easylink_usr_thread_handler = NULL;
static bool easylink_thread_force_exit = false;



/* MiCO callback when WiFi status is changed */
static void easylink_wifi_status_cb( WiFiEvent event, system_context_t * const inContext )
{
    system_log_trace();
    require( inContext, exit );

    switch ( event )
    {
        case NOTIFY_STATION_UP:
            /* Connected to AP, means that the wlan configuration is right, update configuration in flash */
            inContext->flashContentInRam.micoSystemConfig.configured = allConfigured;
            mico_system_context_update( &inContext->flashContentInRam ); //Update Flash content
            mico_rtos_set_semaphore( &easylink_connect_sem ); //Notify Easylink thread
            break;
        default:
            break;
    }
    exit:
    return;
}

static void easylink_usr_thread( uint32_t inContext )
{
    system_log_trace();
    OSStatus err = kNoErr;
    system_context_t *Context = (system_context_t *) inContext;

    easylink_thread_force_exit = false;

    mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED, (void *)easylink_wifi_status_cb, (void *) Context );
    mico_rtos_init_semaphore( &easylink_connect_sem, 1 );

    system_log("Start easylink user mode");
    mico_system_delegate_config_will_start( );

    /* Developer should save the ssid/key to system_context_t and connect to AP.
     * NOTIFY_STATION_UP event will save the new ssid, key to flash and wake up the easylink routine */
    while ( mico_rtos_get_semaphore( &easylink_connect_sem, 0 ) == kNoErr );
    err = mico_rtos_get_semaphore( &easylink_connect_sem, MICO_WAIT_FOREVER );

    /* Easylink force exit by user, clean and exit */
    if ( err != kNoErr && easylink_thread_force_exit ){
        micoWlanSuspend( );
        system_log("EasyLink connection canceled by user");
        easylink_thread_force_exit = false;
        goto exit;
    }

    mico_system_delegate_config_success( CONFIG_BY_USER );

exit:
    mico_system_delegate_config_will_stop( );
    mico_system_notify_remove( mico_notify_WIFI_STATUS_CHANGED, (void *) easylink_wifi_status_cb );

    mico_rtos_deinit_semaphore( &easylink_connect_sem );
    easylink_usr_thread_handler = NULL;
    mico_rtos_delete_thread( NULL );
}

OSStatus mico_easylink_usr( mico_Context_t * const in_context, mico_bool_t enable )
{
    OSStatus err = kUnknownErr;

    require_action( in_context, exit, err = kNotPreparedErr );

    easylink_remove_bonjour( );

    /* easylink thread existed? stop! */
    if ( easylink_usr_thread_handler ) {
        /* easylink usr thread existed? stop! */
        system_log("EasyLink usr processing, force stop..");
        easylink_thread_force_exit = true;
        mico_rtos_thread_force_awake( &easylink_usr_thread_handler );
        mico_rtos_thread_join( &easylink_usr_thread_handler );
    }

    if ( enable == MICO_TRUE ) {
        err = mico_rtos_create_thread( &easylink_usr_thread_handler, MICO_APPLICATION_PRIORITY, "EASYLINK USR",
                                       easylink_usr_thread, 0x1000, (mico_thread_arg_t) in_context );
        require_noerr_string( err, exit, "ERROR: Unable to start the EasyLink usr thread." );

        /* Make sure easylink is already running, and waiting for sem trigger */
        mico_rtos_delay_milliseconds( 100 );
    }

    exit:
    return err;
}

//#endif

