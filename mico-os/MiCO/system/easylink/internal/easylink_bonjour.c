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

#include "StringUtils.h"
#include "system_internal.h"


static void _bonjour_generate_txt_record( char *txt_record, uint32_t easyLink_id, WiFi_Interface interface,
                                          system_context_t * const inContext )
{
    char *temp_txt2 = NULL;

    temp_txt2 = __strdup_trans_dot( FIRMWARE_REVISION );
    sprintf( txt_record, "FW=%s.", temp_txt2 );
    free( temp_txt2 );

    temp_txt2 = __strdup_trans_dot( HARDWARE_REVISION );
    sprintf( txt_record, "%sHD=%s.", txt_record, temp_txt2 );
    free( temp_txt2 );

    temp_txt2 = __strdup_trans_dot( PROTOCOL );
    sprintf( txt_record, "%sPO=%s.", txt_record, temp_txt2 );
    free( temp_txt2 );

    temp_txt2 = __strdup_trans_dot( inContext->micoStatus.rf_version );
    sprintf( txt_record, "%sRF=%s.", txt_record, temp_txt2 );
    free( temp_txt2 );

    temp_txt2 = __strdup_trans_dot( inContext->micoStatus.mac );
    sprintf( txt_record, "%sMAC=%s.", txt_record, temp_txt2 );
    free( temp_txt2 );

    temp_txt2 = __strdup_trans_dot( MicoGetVer( ) );
    sprintf( txt_record, "%sOS=%s.", txt_record, temp_txt2 );
    free( temp_txt2 );

    temp_txt2 = __strdup_trans_dot( MODEL );
    sprintf( txt_record, "%sMD=%s.", txt_record, temp_txt2 );
    free( temp_txt2 );

    temp_txt2 = __strdup_trans_dot( MANUFACTURER );
    sprintf( txt_record, "%sMF=%s.", txt_record, temp_txt2 );
    free( temp_txt2 );

    if ( interface == Soft_AP )
    {
        sprintf( txt_record, "%swlan unconfigured=T.", txt_record );
        sprintf( txt_record, "%sFTC=T.", txt_record );
    }
    else
    {
        sprintf( txt_record, "%swlan unconfigured=F.", txt_record );
#ifdef MICO_CONFIG_SERVER_ENABLE
        sprintf(txt_record, "%sFTC=T.", txt_record);
#else
        sprintf( txt_record, "%sFTC=F.", txt_record );
#endif
    }

    sprintf( txt_record, "%sID=%lx.", txt_record, easyLink_id );
}

OSStatus easylink_bonjour_start( WiFi_Interface interface, uint32_t easyLink_id, system_context_t * const inContext )
{
    char *temp_txt = NULL;
    OSStatus err = kNoErr;
    net_para_st para;
    mdns_init_t init;

    temp_txt = malloc( 500 );
    require_action( temp_txt, exit, err = kNoMemoryErr );

    memset( &init, 0x0, sizeof(mdns_init_t) );

    micoWlanGetIPStatus( &para, interface );

    init.service_name = "_easylink_config._tcp.local.";

    /*   name(xxxxxx).local.  */
    snprintf( temp_txt, 100, "%s(%c%c%c%c%c%c).local.", MODEL,
              inContext->micoStatus.mac[9],
              inContext->micoStatus.mac[10],
              inContext->micoStatus.mac[12],
              inContext->micoStatus.mac[13],
              inContext->micoStatus.mac[15],
              inContext->micoStatus.mac[16] );
    init.host_name = (char*) __strdup( temp_txt );

    /*   name(xxxxxx).   */
    snprintf( temp_txt, 100, "%s(%c%c%c%c%c%c)", MODEL,
              inContext->micoStatus.mac[9],
              inContext->micoStatus.mac[10],
              inContext->micoStatus.mac[12],
              inContext->micoStatus.mac[13],
              inContext->micoStatus.mac[15],
              inContext->micoStatus.mac[16] );
    init.instance_name = (char*) __strdup( temp_txt );

    init.service_port = MICO_CONFIG_SERVER_PORT;

    _bonjour_generate_txt_record( temp_txt, easyLink_id, interface, inContext );

    init.txt_record = (char*) __strdup( temp_txt );

    if ( interface == Soft_AP )
        mdns_add_record( init, interface, 1500 );
    else
        mdns_add_record( init, interface, 10 );

    free( init.host_name );
    free( init.instance_name );
    free( init.txt_record );

    exit:
    if ( temp_txt ) free( temp_txt );
    return err;
}

OSStatus easylink_bonjour_update( WiFi_Interface interface, uint32_t easyLink_id, system_context_t * const inContext )
{
    char *temp_txt = NULL;
    OSStatus err = kNoErr;

    temp_txt = malloc( 500 );
    require_action( temp_txt, exit, err = kNoMemoryErr );

    _bonjour_generate_txt_record( temp_txt, easyLink_id, interface, inContext );

    mdns_update_txt_record( "_easylink_config._tcp.local.", interface, temp_txt );

    exit:
    if ( temp_txt ) free( temp_txt );
    return err;
}

void easylink_remove_bonjour( void )
{
    mdns_record_state_t easylink_service_state = RECORD_REMOVED;

    /* Remove previous _easylink_config service if existed */
    easylink_service_state = mdns_get_record_status( "_easylink_config._tcp.local.", Station);
    if ( easylink_service_state != RECORD_REMOVED )
    {
        UnSetTimer( easylink_remove_bonjour );
        mdns_suspend_record( "_easylink_config._tcp.local.", Station, true );
        mico_rtos_delay_milliseconds( 1500 );
    }

    /* Remove previous _easylink_config service if existed */
    easylink_service_state = mdns_get_record_status( "_easylink_config._tcp.local.", Soft_AP);
    if ( easylink_service_state != RECORD_REMOVED )
    {
        mdns_suspend_record( "_easylink_config._tcp.local.", Soft_AP, true );
        mico_rtos_delay_milliseconds( 1500 );
    }
}

