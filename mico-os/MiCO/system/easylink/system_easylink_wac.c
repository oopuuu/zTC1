/**
 ******************************************************************************
 * @file    mico_easylink_wac.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide header file for start a Apple WAC (wireless accessory
 *          configuration) function thread.
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
#include "MFi_WAC.h"

#include "StringUtils.h"

#define BUNDLE_SEED_ID          "C6P64J2MZX"  
#define EA_PROTOCOL             "com.issc.datapath"

const char *eaProtocols[1] = {EA_PROTOCOL};

OSStatus mico_easylink_wac( mico_Context_t * const inContext, mico_bool_t enable )
{
    OSStatus err = kNoErr;
    IPStatusTypedef para;
    uint8_t major_ver, minor_ver, revision;
    
    if( enable == FALSE ) {
        return mfi_wac_stop();
    }

    mfi_wac_lib_version( &major_ver, &minor_ver, &revision );
    system_log( "Import MFi WAC library v%d.%d.%d", major_ver, minor_ver, revision );

    WACPlatformParameters_t* WAC_Params = NULL;
    WAC_Params = calloc(1, sizeof(WACPlatformParameters_t));
    require(WAC_Params, exit);

    micoWlanGetIPStatus(&para, Station);

    str2hex((unsigned char *)para.mac, WAC_Params->macAddress, 6);
    WAC_Params->isUnconfigured          = 1;
    WAC_Params->supportsAirPlay         = 0;
    WAC_Params->supportsAirPrint        = 0;
    WAC_Params->supports2_4GHzWiFi      = 1;
    WAC_Params->supports5GHzWiFi        = 0;
    WAC_Params->supportsWakeOnWireless  = 0;

    WAC_Params->firmwareRevision =  FIRMWARE_REVISION;
    WAC_Params->hardwareRevision =  HARDWARE_REVISION;
    WAC_Params->serialNumber =      SERIAL_NUMBER;
    WAC_Params->name =              inContext->micoSystemConfig.name;
    WAC_Params->model =             MODEL;
    WAC_Params->manufacturer =      MANUFACTURER;

    WAC_Params->numEAProtocols =    1;
    WAC_Params->eaBundleSeedID =    BUNDLE_SEED_ID;
    WAC_Params->eaProtocols =       (char **)eaProtocols;

    err = mfi_wac_start( inContext, WAC_Params, MICO_I2C_CP, 1200 );
    require_noerr(err, exit);
    
exit:
    free(WAC_Params);
    return err; 
}
