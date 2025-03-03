/**
 ******************************************************************************
 * @file    airkiss_discovery.h
 * @author  William Xu
 * @version V1.0.0
 * @date    28-10-2015
 * @brief   Head file for WECHAT device discovery protocol
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

#ifndef __EASYLINK_INTERNAL_H
#define __EASYLINK_INTERNAL_H

#include "mico.h"
#include "system_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

OSStatus easylink_bonjour_start( WiFi_Interface interface, uint32_t easyLink_id, system_context_t * const inContext );
OSStatus easylink_bonjour_update( WiFi_Interface interface, uint32_t easyLink_id, system_context_t * const inContext );
void easylink_remove_bonjour( void );


OSStatus easylink_softap_start( system_context_t * const inContext );
uint32_t easylink_softap_get_identifier( void );


#ifdef __cplusplus
} /*extern "C" */
#endif

#endif //__EASYLINK_INTERNAL_H


