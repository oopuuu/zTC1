/**
 ******************************************************************************
 * @file    platform_flash.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provides flash operation functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "debug.h"
#include "common.h"
#include "mdev.h"
#include "flash.h"
#include "partition.h"
#include "boot_flags.h"
#include "platform_peripheral.h"

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
*               Variables Definitions
******************************************************/

static mdev_t *flash_dev = NULL;

/******************************************************
*               Function Declarations
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/


OSStatus platform_flash_init( const platform_flash_t *peripheral )
{
    OSStatus err = kNoErr;

    require_action_quiet( peripheral != NULL, exit, err = kParamErr );
    require_action_quiet( peripheral->flash_type == FLASH_TYPE_SPI, exit, err = kUnsupportedErr );

    require_quiet( flash_dev == NULL, exit );

    flash_drv_init( );
    flash_dev = flash_drv_open( FL_INT );
    require_action( flash_dev, exit, err = kOpenErr );

exit:
    return err;
}

OSStatus platform_flash_erase( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
    OSStatus err = kNoErr;

    require_action_quiet( peripheral != NULL, exit, err = kParamErr );
    require_action_quiet( peripheral->flash_type == FLASH_TYPE_SPI, exit, err = kUnsupportedErr );
    require_action( start_address >= peripheral->flash_start_addr
                    && end_address <= peripheral->flash_start_addr + peripheral->flash_length - 1,
                    exit, err = kParamErr );
    require_action_quiet( flash_dev, exit, err = kNotInitializedErr );

    err = flash_drv_erase( flash_dev, start_address, end_address - start_address + 1 );

exit:
    return err;
}

OSStatus platform_flash_write( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data,
                               uint32_t length )
{
    OSStatus err = kNoErr;

    require_action_quiet( peripheral != NULL, exit, err = kParamErr );
    require_action_quiet( peripheral->flash_type == FLASH_TYPE_SPI, exit, err = kUnsupportedErr );
    require_action( *start_address >= peripheral->flash_start_addr
                    && *start_address + length <= peripheral->flash_start_addr + peripheral->flash_length,
                    exit, err = kParamErr );
    require_action_quiet( flash_dev, exit, err = kNotInitializedErr );

    err = flash_drv_write( flash_dev, data, length, *start_address );
    require_noerr( err, exit );
    *start_address += length;

exit:
    return err;
}

OSStatus platform_flash_read( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  )
{
    OSStatus err = kNoErr;

    require_action_quiet( peripheral != NULL, exit, err = kParamErr );
    require_action_quiet( peripheral->flash_type == FLASH_TYPE_SPI, exit, err = kUnsupportedErr );
    require_action( (*start_address >= peripheral->flash_start_addr)
                    && (*start_address + length) <= (peripheral->flash_start_addr + peripheral->flash_length),
                    exit, err = kParamErr );
    require_action_quiet( flash_dev, exit, err = kNotInitializedErr );


    err = flash_drv_read( flash_dev, data, length, *start_address );
    require_noerr( err, exit );
    *start_address += length;

exit:
    return err;
}

OSStatus platform_flash_enable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
    UNUSED_PARAMETER(peripheral);
    UNUSED_PARAMETER(start_address);
    UNUSED_PARAMETER(end_address);

    return kNoErr;
}

OSStatus platform_flash_disable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
    UNUSED_PARAMETER(peripheral);
    UNUSED_PARAMETER(start_address);
    UNUSED_PARAMETER(end_address);

    return kNoErr;
}


static int passive_wifi_fw = 0, passive_fw = 0;

int get_passive_wifi_firmware(void)
{
    short history = 0;
    struct partition_entry *f1, *f2;

    if (passive_wifi_fw != 0)
        return passive_wifi_fw;

    f1 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);
    f2 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);

    if (f1 == NULL || f2 == NULL) {
        return 0;
    }

    passive_wifi_fw = f1->gen_level >= f2->gen_level ? 2 : 1;
    return passive_wifi_fw;
}

int get_passive_firmware(void)
{
    short history = 0;
    struct partition_entry *f1, *f2;

    if (passive_fw != 0)
        return passive_fw;

    f1 = part_get_layout_by_id(FC_COMP_FW, &history);
    f2 = part_get_layout_by_id(FC_COMP_FW, &history);

    if (f1 == NULL || f2 == NULL) {
        return 0;
    }

    if (boot_get_partition_no() == 0) {
        /* If this is 0, it means the first entry is the booted
         * firmware. Which means we want to write to the second entry
         * and make it the active partition hence forth
         */
        passive_fw = 2;
    } else {
        passive_fw = 1;
    }

    return passive_fw;
}


//API part_set_active_partition update the active partition
//API part_init to initilize partition
int set_active_wifi_firmware(int index)
{
    short history = 0;
    struct partition_entry *f1, *f2;

    f1 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);
    f2 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);

    if (index == 1)
        f1->gen_level = f2->gen_level + 1;
    else
        f2->gen_level = f1->gen_level + 1;
    part_write_layout();
    return WM_SUCCESS;
}

int set_active_firmware(int index)
{
    short history = 0;
    struct partition_entry *f1, *f2;

    f1 = part_get_layout_by_id(FC_COMP_FW, &history);
    f2 = part_get_layout_by_id(FC_COMP_FW, &history);

    if (index == 1)
        f1->gen_level = f2->gen_level + 1;
    else
        f2->gen_level = f1->gen_level + 1;
    part_write_layout();
    return WM_SUCCESS;
}

int change_active_firmware(void)
{
    short history = 0;
    struct partition_entry *f1, *f2;

    f1 = part_get_layout_by_id(FC_COMP_FW, &history);
    f2 = part_get_layout_by_id(FC_COMP_FW, &history);

    if (passive_fw == 1)
        f1->gen_level = f2->gen_level + 1;
    else
        f2->gen_level = f1->gen_level + 1;
    part_write_layout();
    return WM_SUCCESS;
}

