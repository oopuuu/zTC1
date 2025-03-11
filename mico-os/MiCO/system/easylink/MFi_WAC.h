/**
 ******************************************************************************
 * @file    MFi_WAC.h
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

#pragma once

#include "mico.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUNDLE_SEED_ID          "C6P64J2MZX"  
#define EA_PROTOCOL             "com.issc.datapath"

/**
 *   @brief Parameters controlled by the platform to configure the WAC process. 
 */
typedef struct
{
    uint8_t macAddress[ 6 ];        /**< REQUIRED: Accessory MAC address, e.g. 00:11:22:33:44:55. */
    
    bool    isUnconfigured;         /**< TRUE/FALSE: whether the accessory is unconfigured. Should be true for current cases. */
    bool    supportsAirPlay;        /**< TRUE/FALSE: whether the accessory supports AirPlay. */
    bool    supportsAirPrint;       /**< TRUE/FALSE: whether the accessory supports AirPrint. */
    bool    supports2_4GHzWiFi;     /**< TRUE/FALSE: whether the accessory supports 2.4 GHz Wi-Fi. */
    bool    supports5GHzWiFi;       /**< TRUE/FALSE: whether the accessory supports 5 GHz Wi-Fi. */
    bool    supportsWakeOnWireless; /**< TRUE/FALSE: whether the accessory supports Wake On Wireless. */
    
    char    *firmwareRevision;      /**< REQUIRED: Version of the accessory's firmware, e.g. 1.0.0. */
    char    *hardwareRevision;      /**< REQUIRED: Version of the accessory's hardware, e.g. 1.0.0. */
    char    *serialNumber;          /**< OPTIONAL: Accessory's serial number. */
    
    char    *name;                  /**< REQUIRED: Name of the accessory. */
    char    *model;                 /**< REQUIRED: Model name of the accessory. */
    char    *manufacturer;          /**< REQUIRED: Manufacturer name of the accessory. */
    
    char    **eaProtocols;          /**< OPTIONAL: Array of EA Protocol strings. */
    uint8_t numEAProtocols;         /**< OPTIONAL: Number of EA Protocol strings contained in the eaProtocols array. */
    char    *eaBundleSeedID;        /**< OPTIONAL: Accessory manufacturer's BundleSeedID. */
    
} WACPlatformParameters_t;

void mfi_wac_lib_version( uint8_t *major, uint8_t *minor, uint8_t *revision );

OSStatus mfi_wac_start( mico_Context_t * const inContext, WACPlatformParameters_t *inWACPara, mico_i2c_t i2c, int timeOut );

OSStatus mfi_wac_stop( void );


#ifdef __cplusplus
} /*extern "C" */
#endif


