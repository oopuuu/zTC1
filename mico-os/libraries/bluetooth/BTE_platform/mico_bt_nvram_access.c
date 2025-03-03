/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */

/*****************************************************************************
**
**  Name:          mico_bt_nvram_access.c
**
**  Description:   general method of store/retrieve informations with searching key
**                     handling linkkey storage
**
******************************************************************************/

#include "common.h"
#include "mico_system.h"
#include "StringUtils.h"

#define EXPORT_API

#include "buildcfg.h"
#include "bt_types.h"   /* This must be defined AFTER buildcfg.h */
#include "bt_trace.h"

#include "mico_bt_dev.h"

#include "mico_bt_nvram_access.h"

/** NVRAM entry for bonded device */
#pragma pack(1)
typedef struct {
    mico_bt_device_address_t    bd_addr;        /**< Device address */
    uint8_t                     addr_type;      /**< BLE_ADDR_PUBLIC or BLE_ADDR_RANDOM */
    uint8_t                     device_type;    /**< BT_DEVICE_TYPE_BREDR or BT_DEVICE_TYPE_BLE */
    uint16_t                    length;         /**< Length of key_blobs (link key information) */
    uint8_t                     key_blobs[1];   /**< Link keys (actual length specified by 'length' field) */
} mico_bt_nvram_access_entry_t;
#pragma pack()

#if defined(BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED) && (BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED == TRUE)

/**
 *      definition
 *
 **/

static mico_bool_t  dct_initialized = FALSE;

/*
   DCT control block and cached information
   for storing link key DB
*/
typedef struct
{
/**
       Physical DCT information
*/
    int start_offset;          /* start offset of DCT key storage */
    int start_physcal_addres;  /* actual physical address in NAND flash ram */
    int dct_size;              /* size allocated for DB of bonded devices*/

/**
      Logical R/W variables
*/
    int cur_offset;            /* offset recently read */
    int end_offset;            /* offset of last data written to DCT */
    int total_devices ;        /* total # of bonded devices */
    int index_enum;            /* current enumerated index */

/**
     max # of bonded devices assigned by app config
*/
    int max_devices;           /* max # of bonded devices to be stored */

/**
     Local Keys DCT information
*/
    int lkey_offset;           /*  N/A */
} mico_bt_dct_cb_t;

static mico_bt_dct_cb_t  dct_cb;

/**
 * Function    mico_bt_nvram_access_valid_entry
 *
 *             validate if p_entry has correct value of bonded device
 *
*/
static mico_bool_t mico_bt_nvram_access_valid_entry(mico_bt_nvram_access_entry_t *p_entry)
{
    mico_bool_t ret = FALSE;
    const mico_bt_device_address_t bd_addr_any = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, null_addr = {0,0,0,0,0,0,};

    if (!p_entry) return ret;

    APPL_TRACE_DEBUG3 ("mico_bt_nvram_access_valid_entry  BDA: %08x%04x, key length: %d",
                      (p_entry->bd_addr[0]<<24)+(p_entry->bd_addr[1]<<16)+(p_entry->bd_addr[2]<<8)+p_entry->bd_addr[3],
                      (p_entry->bd_addr[4]<<8)+p_entry->bd_addr[5], 
                      p_entry->length);

    if(memcmp(p_entry->bd_addr, bd_addr_any, sizeof(bd_addr_any)) 
        && memcmp(p_entry->bd_addr, null_addr, sizeof(mico_bt_device_address_t)) 
        && p_entry->length <= (MICO_BT_DCT_MAX_KEYBLOBS + MICO_BT_DCT_ENTRY_HDR_LENGTH ))
    {
         ret = TRUE;
    }
    return ret;
}


/**
 * Function     mico_bt_nvram_access_init
 *
 * initialize nvram control block
 *
 * @param[in]  max_num_bonded_devices : maximum limit of number of bonded devices
 *
 * @return  void
 */
void mico_bt_nvram_access_init()
{
    int max_num_bonded_devices;
    int dct_size;
    mico_bt_nvram_access_entry_t *p_entry;

    if(dct_initialized == TRUE)
        return;

    memset(&dct_cb, 0, sizeof(dct_cb));

    max_num_bonded_devices = MICO_BT_DCT_MAX_DEVICES;

    dct_size = max_num_bonded_devices * (MICO_BT_DCT_ENTRY_HDR_LENGTH + MICO_BT_DCT_MAX_KEYBLOBS);
    dct_cb.max_devices = max_num_bonded_devices;

    dct_cb.start_offset = MICO_BT_DCT_REMOTE_KEY_OFFSET;

    /*
      For application based on MiCO SDK, application needs to know the information of
      dct area used by bluetooth stack to use the remaining area for application own storage
      and configuration, but for more general configuration independent of platform specific features,
      configuration of application needs to be set as maximum number of bonded devices, and
      dct size and offset needs to be hidden under callout implementation.
    */
    dct_cb.dct_size = dct_size;

    dct_cb.cur_offset = dct_cb.start_offset;

    do
    {
        mico_system_para_read( (void**) &p_entry, PARA_BT_DATA_SECTION, dct_cb.cur_offset, MICO_BT_DCT_ENTRY_HDR_LENGTH );
        if(!mico_bt_nvram_access_valid_entry(p_entry))
        {
            mico_system_para_read_release( (void*) p_entry );
            break;
        }
        dct_cb.cur_offset += p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH;
        mico_system_para_read_release( (void*) p_entry );
        dct_cb.total_devices++;

    } while( dct_cb.cur_offset < (dct_cb.start_offset + dct_size) );

    dct_cb.end_offset = dct_cb.cur_offset;
    dct_initialized = TRUE;

    APPL_TRACE_DEBUG4("%s start offset %d %d, end_offset: %d", __FUNCTION__, dct_cb.start_offset, dct_size, dct_cb.end_offset);
}


/**
 * Function     mico_bt_nvram_access_find_offset
 *
 *
 * @param[in]  *key_bdaddr : bd_addr of bonded device
 * @param[in]  start_offset : start offset where to start searching
 * @param[out] p_offset : stored offset  of bonded device of bd_addr
 *
 * @return      TRUE if found or FALSE
 */
static mico_bool_t mico_bt_nvram_access_find_offset(mico_bt_device_address_t key_bdaddr, int start_offset , int *p_offset)
{
     int  of_dct = start_offset;
     mico_bt_nvram_access_entry_t *p_entry = NULL;
     mico_bool_t found_keyblobs = FALSE;

     if(!dct_initialized)
         mico_bt_nvram_access_init();

     APPL_TRACE_DEBUG3 ("mico_bt_nvram_access_find_offset  BDA: %08x%04x, key length: %d",
                       ((key_bdaddr)[0]<<24)+((key_bdaddr)[1]<<16)+((key_bdaddr)[2]<<8)+(key_bdaddr)[3],
                       ((key_bdaddr)[4]<<8)+(key_bdaddr)[5], 
                       0);

    do
    {
        mico_system_para_read( (void**) &p_entry, 
                                PARA_BT_DATA_SECTION, 
                                of_dct, 
                                MICO_BT_DCT_ADDR_FIELD + MICO_BT_DCT_LENGTH_FIELD + MICO_BT_DCT_ADDR_TYPE + MICO_BT_DCT_DEVICE_TYPE );

        if( !mico_bt_nvram_access_valid_entry( p_entry ) )
        {
            mico_system_para_read_release( (void*) p_entry );
            break;
        }

        if( !memcmp(p_entry->bd_addr, key_bdaddr, sizeof(mico_bt_device_address_t) ))
        {
            APPL_TRACE_DEBUG2("%s found keyblobs: %d ",__FUNCTION__, of_dct);

            found_keyblobs = TRUE;
            mico_system_para_read_release( (void*) p_entry );
            break;
        }

        of_dct += MICO_BT_DCT_ENTRY_HDR_LENGTH;
        of_dct += p_entry->length;

        mico_system_para_read_release( (void*) p_entry );
    } while( of_dct < dct_cb.end_offset );

    if( p_offset != NULL )
       *p_offset = of_dct;

    return found_keyblobs;
}



/**
 * Function     mico_bt_nvram_access_get_bonded_devices
 *
 * get lists of bonded devices stored in nvram
 *
 * @param[in]  paired_device_list : array for getting bd address of bonded devices
 * @param[in/out] p_num_devices :  list size of paired_device_list/total number of bonded devices stored
 *
 * @return      MICO_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_get_bonded_devices(mico_bt_dev_bonded_device_info_t bonded_device_list[], uint16_t *p_num_devices)
{
    int list_size = *p_num_devices, index;
    int of_dct = dct_cb.start_offset;
    mico_bt_nvram_access_entry_t *p_entry;

    *p_num_devices = 0;
    for(index = 0 ; index < list_size ; index++)
    {
        mico_system_para_read( (void**) &p_entry, PARA_BT_DATA_SECTION, of_dct, MICO_BT_DCT_ENTRY_HDR_LENGTH);
        if(!mico_bt_nvram_access_valid_entry(p_entry))
        {
            mico_system_para_read_release( (void*) p_entry );
            break;
        }

        memcpy(bonded_device_list[index].bd_addr, p_entry->bd_addr, sizeof(p_entry->bd_addr));
        bonded_device_list[index].addr_type = p_entry->addr_type;
        bonded_device_list[index].device_type = p_entry->device_type;

        of_dct += (p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH);
        mico_system_para_read_release( (void*) p_entry );
    }

    *p_num_devices = index;
    APPL_TRACE_DEBUG2("%s num bonded devices : %d", __FUNCTION__, *p_num_devices );

    return MICO_BT_SUCCESS;
}



/**
 * Function     mico_bt_nvram_access_save_bonded_device_key
 *
 *  save link key information of bonded device
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[in]  p_keyblobs : key blobs including key header, link keys and key length
 * @param[in]  key_len :  total length of p_keyblobs
 *
 * @return      MICO_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_save_bonded_device_key(mico_bt_device_address_t bd_addr, mico_bt_ble_address_type_t addr_type, uint8_t device_type, uint8_t *p_keyblobs, uint16_t key_len)

{
     int of_key = 0;
     mico_bt_nvram_access_entry_t *p_entry, *p_next_entry, *p_updated;
     mico_bt_nvram_access_entry_t  l_entry;
     int len_updated = 0;

     APPL_TRACE_DEBUG3 ("mico_bt_nvram_access_save_bonded_device_key  BDA: %08x%04x, key length: %d",
                        ((bd_addr)[0]<<24)+((bd_addr)[1]<<16)+((bd_addr)[2]<<8)+(bd_addr)[3],
                        ((bd_addr)[4]<<8)+(bd_addr)[5], 
                        key_len );

     if(mico_bt_nvram_access_find_offset(bd_addr, dct_cb.start_offset, &of_key))
     {
         /* read bd_addr key*/
         mico_system_para_read( (void**) &p_entry, PARA_BT_DATA_SECTION, of_key, MICO_BT_DCT_ENTRY_HDR_LENGTH);

         APPL_TRACE_DEBUG4("%s found offset:%d , key length:%d , entry len: %d",__FUNCTION__,
                            of_key, key_len, p_entry->length);
         
         mico_system_para_read_release( (void*) p_entry );
         if(p_entry->length != key_len)
         {
            /*update area between of_key offset and end offset*/
            /* Read from next entry to endoffset*/
            mico_system_para_read( (void**) &p_next_entry, PARA_BT_DATA_SECTION, 
                                    of_key + p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH,
                                    MICO_BT_DCT_ENTRY_HDR_LENGTH );   /* read bd_addr key*/
                
            mico_system_para_read_release( (void*) p_entry );
            if(mico_bt_nvram_access_valid_entry(p_next_entry))
            {
                len_updated = dct_cb.end_offset - (of_key + p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH);
                mico_system_para_read( (void**) &p_updated, PARA_BT_DATA_SECTION, of_key + p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH, len_updated );
                /* dct_write of current key entry with changed length */
                mico_system_para_write(p_updated, PARA_BT_DATA_SECTION, of_key + key_len + MICO_BT_DCT_ENTRY_HDR_LENGTH, len_updated);

                mico_system_para_read_release( (void*) p_updated );
            }

             
            mico_system_para_write(&key_len, PARA_BT_DATA_SECTION, of_key + MICO_BT_DCT_ADDR_FIELD , sizeof(uint16_t));
            mico_system_para_write(&addr_type, PARA_BT_DATA_SECTION, of_key + MICO_BT_DCT_ADDR_FIELD + MICO_BT_DCT_LENGTH_FIELD , sizeof(mico_bt_ble_address_type_t));
            mico_system_para_write(&device_type, PARA_BT_DATA_SECTION, of_key + MICO_BT_DCT_ADDR_FIELD + MICO_BT_DCT_LENGTH_FIELD + MICO_BT_DCT_ADDR_TYPE , MICO_BT_DCT_DEVICE_TYPE);
            mico_system_para_write(p_keyblobs, PARA_BT_DATA_SECTION, of_key + MICO_BT_DCT_ENTRY_HDR_LENGTH, key_len);

            /* update from current offset over read next entry of previous end offset */
            /* update end offset */
            dct_cb.end_offset = dct_cb.end_offset + ((int)key_len - (int)p_entry->length);
            dct_cb.cur_offset = dct_cb.end_offset;
         }
         else
         {
             APPL_TRACE_DEBUG0("mico_bt_callout_save_bonded_device_key updated!!");
             if( memcmp( p_entry->key_blobs, p_keyblobs, key_len ))
             {
                mico_system_para_write(p_keyblobs, PARA_BT_DATA_SECTION, of_key + MICO_BT_DCT_ENTRY_HDR_LENGTH, key_len );
             }
             else
             {
                APPL_TRACE_DEBUG0( "Same key, write ignore..." );
             }
             //mico_dct_write(p_keyblobs, DCT_APP_SECTION, of_key + MICO_BT_DCT_ENTRY_HDR_LENGTH, key_len );
         }

     }
     else
     {
         APPL_TRACE_DEBUG2("%s offset %d", __FUNCTION__, dct_cb.end_offset);
         memcpy(l_entry.bd_addr,bd_addr,MICO_BT_DCT_ADDR_FIELD);
         l_entry.addr_type = addr_type;
         l_entry.device_type = device_type;
         l_entry.length = key_len;
         mico_system_para_write(&l_entry, PARA_BT_DATA_SECTION, dct_cb.end_offset, MICO_BT_DCT_ENTRY_HDR_LENGTH );
         //mico_dct_write(&l_entry, DCT_APP_SECTION,dct_cb.end_offset, MICO_BT_DCT_ENTRY_HDR_LENGTH);
         dct_cb.end_offset += MICO_BT_DCT_ENTRY_HDR_LENGTH;
         mico_system_para_write(p_keyblobs, PARA_BT_DATA_SECTION, dct_cb.end_offset, key_len );
         //mico_dct_write(p_keyblobs,DCT_APP_SECTION,dct_cb.end_offset,key_len);
         dct_cb.end_offset += key_len;
         APPL_TRACE_DEBUG2("%s key_len %d", __FUNCTION__, key_len);

         dct_cb.total_devices++;
     }

     APPL_TRACE_DEBUG3("%s end_offset:%d , total_devices:%d",__FUNCTION__, dct_cb.end_offset,dct_cb.total_devices );

     return MICO_BT_SUCCESS;

}


/**
 * Function     mico_bt_nvram_access_load_bonded_device_keys
 *
 *  loads stored key information for bonded device having @bd_addr
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[out]  p_key_entry :  key information stored
 *
 * @return      MICO_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_load_bonded_device_keys(mico_bt_device_address_t bd_addr, mico_bt_nvram_access_entry_t *p_key_entry,uint8_t entry_max_length)
{
    int of_key = 0;
    mico_bt_nvram_access_entry_t *p_entry;
    uint8_t *p_keyblobs;
    mico_bt_result_t status;

    APPL_TRACE_DEBUG3("mico_bt_nvram_access_load_linkkeys  BDA: %08x%04x, key length: %d",
                       ((bd_addr)[0]<<24)+((bd_addr)[1]<<16)+((bd_addr)[2]<<8)+(bd_addr)[3],
                       ((bd_addr)[4]<<8)+(bd_addr)[5], 0);

    if(mico_bt_nvram_access_find_offset(bd_addr, dct_cb.start_offset, &of_key))
    {
        APPL_TRACE_DEBUG4("%s  start offset %d %d  %d",__FUNCTION__, dct_cb.start_offset, dct_cb.index_enum, of_key );

        /* read key entry*/
        mico_system_para_read( (void**) &p_entry, PARA_BT_DATA_SECTION, of_key, MICO_BT_DCT_ENTRY_HDR_LENGTH);

        if(entry_max_length < MICO_BT_DCT_ENTRY_HDR_LENGTH)
        {
            memcpy(p_key_entry,p_entry, entry_max_length);
            mico_system_para_read_release( (void*) p_entry );
            return MICO_BT_ILLEGAL_VALUE;
        }
        else
        {
            memcpy(p_key_entry,p_entry, MICO_BT_DCT_ENTRY_HDR_LENGTH);
        }

        APPL_TRACE_DEBUG2("%s  key_length : %d",__FUNCTION__, p_entry->length);

        mico_system_para_read_release( (void*) p_entry );
        mico_system_para_read( (void**) &p_keyblobs, PARA_BT_DATA_SECTION, of_key + MICO_BT_DCT_ENTRY_HDR_LENGTH, p_entry->length);

        if( entry_max_length < MICO_BT_DCT_ENTRY_HDR_LENGTH + p_entry->length)
        {
            memcpy(p_key_entry->key_blobs, p_keyblobs, (entry_max_length - MICO_BT_DCT_ENTRY_HDR_LENGTH));
            status = MICO_BT_ILLEGAL_VALUE;
        }
        else
        {
            memcpy(p_key_entry->key_blobs, p_keyblobs, p_entry->length);
            status = MICO_BT_SUCCESS;
        }

        mico_system_para_read_release( p_keyblobs );

        return status;
    }
    else
    {
        APPL_TRACE_DEBUG3("%s  not found start offset %d %d",__FUNCTION__, dct_cb.start_offset,dct_cb.index_enum );
        return MICO_BT_NO_RESOURCES;
    }
}



/**
 * Function     mico_bt_nvram_access_delete_bonded_device
 *
 * remove key information from storage by releasing bonding with remote device having @bd_addr
 *
 * @param[in]  bd_addr : bd_addr of bonded device to be removed

 * @return      MICO_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_delete_bonded_device(mico_bt_device_address_t bd_addr)
{
    int of_key = 0 , next_offset= 0 , del_length;
    mico_bt_nvram_access_entry_t *p_entry;
    mico_bool_t found = FALSE;
    int total_space;
    uint8_t invalidate[MICO_BT_DCT_MAX_KEYBLOBS + MICO_BT_DCT_ENTRY_HDR_LENGTH];

    found = mico_bt_nvram_access_find_offset(bd_addr, dct_cb.start_offset, &of_key);
    if(!found)
    {
        return MICO_BT_NO_RESOURCES;
    }

    memset(invalidate,0xff,sizeof(invalidate));
    APPL_TRACE_DEBUG2("%s found offset:%d",__FUNCTION__, of_key );

    mico_system_para_read( (void**) &p_entry, PARA_BT_DATA_SECTION, of_key, MICO_BT_DCT_ENTRY_HDR_LENGTH);
    next_offset = of_key + MICO_BT_DCT_ENTRY_HDR_LENGTH + p_entry->length;
    del_length = MICO_BT_DCT_ENTRY_HDR_LENGTH + p_entry->length;
    mico_system_para_read_release( (void*) p_entry );

    total_space = dct_cb.end_offset - next_offset;
    if(total_space > 0 )
    {
        mico_system_para_read( (void**) &p_entry, PARA_BT_DATA_SECTION, next_offset, total_space );
        mico_system_para_write(p_entry, PARA_BT_DATA_SECTION, of_key, total_space );
        mico_system_para_read_release( (void*)p_entry );
    }

    APPL_TRACE_DEBUG2("total_space %d del_length:%d",total_space, del_length );
    dct_cb.end_offset -= del_length;
    mico_system_para_write(invalidate, PARA_BT_DATA_SECTION, dct_cb.end_offset, del_length );
    dct_cb.total_devices--;

    return MICO_BT_SUCCESS;

}



/**
 * Function     mico_bt_nvram_access_load_local_identity_keys
 *
 * load local identity keys including ir/irk/dhk stored in nvram
 *
 * @param[out]  p_lkeys: local identity key information
 *
 * @return      MICO_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_load_local_identity_keys(mico_bt_local_identity_keys_t *p_lkeys)
{
    uint8_t *p_local_key;
    mico_bt_result_t result;
    uint32_t invalid_lkey = 0xffffffff;

    if(!dct_initialized)
        mico_bt_nvram_access_init();

    /* Get Local Keys from top of the DCT area */
    mico_system_para_read( (void**) &p_local_key, 
                            PARA_BT_DATA_SECTION, 
                            dct_cb.start_offset - sizeof(mico_bt_local_identity_keys_t), 
                            BTM_SECURITY_LOCAL_KEY_DATA_LEN );

    if(memcmp(p_local_key,&invalid_lkey,sizeof(uint32_t)))
    {
        APPL_TRACE_DEBUG1("%s found saved lkeys", __FUNCTION__);
        memcpy(p_lkeys, p_local_key, sizeof(mico_bt_local_identity_keys_t));
        result = MICO_BT_SUCCESS;
    }
    else
    {
        result = MICO_BT_NO_RESOURCES;
    }

    mico_system_para_read_release( (void*) p_local_key );

    return result;
}


/**
 * Function     mico_bt_nvram_access_save_local_identity_keys
 *
 * save local identity keys including ir/irk/dhk to nvram
 *
 * @param[in]  p_lkeys : local identity key information
 *
 * @return      MICO_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_save_local_identity_keys(mico_bt_local_identity_keys_t *p_lkeys)
{
    if(!dct_initialized)
        mico_bt_nvram_access_init();

    APPL_TRACE_DEBUG2("%s save local keys %d",__FUNCTION__,dct_cb.start_offset  );


    /* store Local Key at top of the DCT area */
    mico_system_para_write(p_lkeys, 
                            PARA_BT_DATA_SECTION, 
                            dct_cb.start_offset - sizeof(mico_bt_local_identity_keys_t), 
                            sizeof(mico_bt_local_identity_keys_t) );
    return MICO_BT_SUCCESS;
}


/**
 * Function     mico_bt_nvram_access_key_storage_available
 *
 * query if there are available spaces for storing key information
 * for device with @bd_addr with requested @req_size
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[in]  req_size : requested size to be stored
 *
 * @return      TRUE if there is available space or FALSE
 */
BOOLEAN mico_bt_nvram_access_key_storage_available(mico_bt_device_address_t bd_addr, int req_size)
{

    int mem_available;
    int of_key = 0;
    mico_bt_nvram_access_entry_t *p_entry;
    mico_bool_t found = FALSE;

    if(req_size == 0)
        req_size = MICO_BT_DCT_MAX_KEYBLOBS;

    APPL_TRACE_DEBUG3( "mico_bt_nvram_access_key_storage_available :%d  %d  %d",
                       dct_cb.dct_size,
                       dct_cb.end_offset,
                       dct_cb.start_offset );

    found = mico_bt_nvram_access_find_offset(bd_addr, dct_cb.start_offset, &of_key);
    if(found)
    {
        mico_system_para_read((void**) &p_entry, PARA_BT_DATA_SECTION, of_key, MICO_BT_DCT_ENTRY_HDR_LENGTH );
        mem_available = p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH;
        if(req_size > mem_available) 
        {
            mem_available += dct_cb.dct_size - (dct_cb.end_offset - dct_cb.start_offset);
        }
        mico_system_para_read_release( (void*) p_entry );

        APPL_TRACE_DEBUG1("mem avaialable :%d",mem_available);
    }
    else
    {
        if(dct_cb.max_devices == dct_cb.total_devices)
           return FALSE;

        mem_available = dct_cb.dct_size - (dct_cb.end_offset - dct_cb.start_offset);
        APPL_TRACE_DEBUG1("not found mem avaialable :%d",mem_available);
    }

    APPL_TRACE_DEBUG3( "mico_bt_callout_key_storage_available : found %d, mem_av:%d, req_size:%d", 
                       found, mem_available, req_size );

    if(mem_available < req_size)
        return FALSE;
    else
        return TRUE;
}


/**
 * Function     mico_bt_callout_get_extra
 *
 *    get extra information of address type and device type
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[out]  p_addr_type : addr type
  * @param[out]  p_device_type : device type
 *
 * @return      TRUE if there is available space or FALSE
 */
mico_bt_result_t mico_bt_callout_get_extra(mico_bt_device_address_t bd_addr,  mico_bt_ble_address_type_t *p_addr_type, uint8_t *p_device_type)
{
    mico_bool_t found = FALSE;
    int of_key = 0;
    mico_bt_nvram_access_entry_t *p_entry;

    found = mico_bt_nvram_access_find_offset(bd_addr, dct_cb.start_offset, &of_key);
    if(!found)
        return MICO_BT_NO_RESOURCES;

    mico_system_para_read((void**) &p_entry, PARA_BT_DATA_SECTION, of_key, MICO_BT_DCT_ENTRY_HDR_LENGTH );

    if(p_addr_type)
        *p_addr_type = p_entry->addr_type;
    if(p_device_type)
        *p_device_type = p_entry->device_type;
    mico_system_para_read_release( (void*) p_entry );

    return MICO_BT_SUCCESS;
}

/**
 * Function     mico_bt_nvram_access_enum_bonded_device_keys
 *
 *   load stored key information by enumeration
 *
 * @param[out]  p_index : index of stored key
 * @param[out]  p_key_entry : key information stored
 *
 * @return      MICO_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_enum_bonded_device_keys(int8_t *p_index, mico_bt_nvram_access_entry_t * p_key_entry, uint8_t entry_max_length)
{
    mico_bt_nvram_access_entry_t *p_entry;
    uint8_t *p_key_blobs;
    mico_bt_result_t status = MICO_BT_SUCCESS;

    if(!dct_initialized)
        mico_bt_nvram_access_init();

    if(dct_cb.index_enum == 0)
       dct_cb.cur_offset = dct_cb.start_offset;

    mico_system_para_read((void**) &p_entry, PARA_BT_DATA_SECTION, dct_cb.cur_offset, MICO_BT_DCT_ENTRY_HDR_LENGTH );
    if(!mico_bt_nvram_access_valid_entry(p_entry))
    {
        dct_cb.cur_offset = dct_cb.end_offset;
        mico_system_para_read_release( (void*) p_entry );
        return MICO_BT_NO_RESOURCES;
    }

    if(entry_max_length < MICO_BT_DCT_ENTRY_HDR_LENGTH)
    {
        memcpy(p_key_entry,p_entry, entry_max_length);
        mico_system_para_read_release( (void*) p_entry );
        return MICO_BT_ILLEGAL_VALUE;
    }
    else
    {
        memcpy(p_key_entry, p_entry, MICO_BT_DCT_ENTRY_HDR_LENGTH); 
    }

    mico_system_para_read_release( (void*) p_entry );
    mico_system_para_read((void**) &p_key_blobs, PARA_BT_DATA_SECTION, dct_cb.cur_offset + MICO_BT_DCT_ENTRY_HDR_LENGTH, p_entry->length );

    if(entry_max_length < MICO_BT_DCT_ENTRY_HDR_LENGTH + p_entry->length)
    {
        memcpy(p_key_entry->key_blobs, p_key_blobs, entry_max_length - MICO_BT_DCT_ENTRY_HDR_LENGTH);
        status = MICO_BT_ILLEGAL_VALUE;
    }
    else
    {
        memcpy(p_key_entry->key_blobs, p_key_blobs, p_entry->length);
        status = MICO_BT_SUCCESS;
    }

    *p_index = ++dct_cb.index_enum;
    dct_cb.cur_offset += p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH;

    mico_system_para_read_release( (void*) p_key_blobs );

    return status;
}

/**
 * Function     mico_bt_nvram_access_find_offset
 *
 *
 * @param[in]  *key_bdaddr : bd_addr of bonded device
 * @param[in]  start_offset : start offset where to start searching
 * @param[out] p_offset : stored offset  of bonded device of bd_addr
 *
 * @return      TRUE if found or FALSE
 */
mico_bool_t mico_bt_nvram_access_find_device( mico_bt_device_address_t key_bdaddr )
{
     int  of_dct = dct_cb.start_offset;
     mico_bt_nvram_access_entry_t *p_entry;
     mico_bool_t found_keyblobs = FALSE;

     if(!dct_initialized)
         mico_bt_nvram_access_init();

     APPL_TRACE_DEBUG3 ("mico_bt_nvram_access_find_offset  BDA: %08x%04x, key length: %d",
                       ((key_bdaddr)[0]<<24)+((key_bdaddr)[1]<<16)+((key_bdaddr)[2]<<8)+(key_bdaddr)[3],
                       ((key_bdaddr)[4]<<8)+(key_bdaddr)[5], 0);

     do
     {
        mico_system_para_read( (void**) &p_entry, PARA_BT_DATA_SECTION, of_dct, MICO_BT_DCT_ADDR_FIELD + MICO_BT_DCT_LENGTH_FIELD + MICO_BT_DCT_ADDR_TYPE + MICO_BT_DCT_DEVICE_TYPE);
        if(!memcmp(p_entry->bd_addr,key_bdaddr,sizeof(mico_bt_device_address_t)))
        {
            APPL_TRACE_DEBUG2("%s found keyblobs: %d ",__FUNCTION__, of_dct);

            found_keyblobs = TRUE;
            mico_system_para_read_release( (void*) p_entry );
            break;
        }

        of_dct += MICO_BT_DCT_ENTRY_HDR_LENGTH;
        of_dct += p_entry->length;

        mico_system_para_read_release( (void*) p_entry );
     } while(of_dct < dct_cb.end_offset);

    return found_keyblobs;
}
#if 0
/**
 * Function     mico_bt_nvram_access_save_bonded_device_key
 *
 *  save link key information of bonded device
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[in]  p_keyblobs : key blobs including key header, link keys and key length
 * @param[in]  key_len :  total length of p_keyblobs
 *
 * @return      MICO_SUCCESS or ERROR
 */
mico_bt_result_t mico_bt_nvram_access_delete_bonded_device(mico_bt_device_address_t bd_addr)
{
     int of_key = 0;
     mico_bt_nvram_access_entry_t *p_entry, *p_next_entry, *p_updated;
     mico_bt_nvram_access_entry_t  l_entry;
     int len_updated = 0;
     uint16_t length;

     APPL_TRACE_DEBUG2 ("mico_bt_nvram_access_delete_bonded_device_key  BDA: %08x%04x",
                       ((bd_addr)[0]<<24)+((bd_addr)[1]<<16)+((bd_addr)[2]<<8)+(bd_addr)[3],
                       ((bd_addr)[4]<<8)+(bd_addr)[5] );


     if(mico_bt_nvram_access_find_offset(bd_addr, dct_cb.start_offset, &of_key))
     {

         /* read bd_addr key*/
         mico_system_para_read( (void**) &p_entry, PARA_BT_DATA_SECTION, of_key, MICO_BT_DCT_ENTRY_HDR_LENGTH);
         APPL_TRACE_DEBUG4("%s found offset:%d , key length:%d , entry len: %d",__FUNCTION__,
            of_key, key_len, p_entry->length);
         mico_system_para_read_release( (void*) p_entry );
         length = p_entry->length;

             /*update area between of_key offset and end offset*/
             /* Read from next entry to endoffset*/
            mico_system_para_read( (void**) &p_next_entry, PARA_BT_DATA_SECTION, 
                                        of_key + p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH,
                                        MICO_BT_DCT_ENTRY_HDR_LENGTH );   /* read bd_addr key*/
                
            mico_system_para_read_release( (void*) p_entry );

            /* Find next record, move them to current record */
             if(mico_bt_nvram_access_valid_entry(p_next_entry))
             {
                len_updated = dct_cb.end_offset - (of_key + p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH);
                mico_system_para_read( (void**) &p_updated, PARA_BT_DATA_SECTION, of_key + p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH, len_updated );
                /* dct_write of current key entry with changed length */
                mico_system_para_write(p_updated, PARA_BT_DATA_SECTION, of_key, len_updated);

                mico_system_para_read_release( (void*) p_updated );

             }else /* Not find next record, just erase the record */
             {
                p_updated = malloc( p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH );
                memset( (uint8_t *)p_updated, 0xff, p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH );
                mico_system_para_write(p_updated, PARA_BT_DATA_SECTION, of_key, p_entry->length + MICO_BT_DCT_ENTRY_HDR_LENGTH);
                free( p_updated );
             }

            /* update from current offset over read next entry of previous end offset */
              /* update end offset */
            dct_cb.end_offset = dct_cb.end_offset - (length + MICO_BT_DCT_ENTRY_HDR_LENGTH);
            dct_cb.cur_offset = dct_cb.end_offset;

            dct_cb.total_devices--;

     }else
     {
        APPL_TRACE_DEBUG0("mico_bt_nvram_access_delete_bonded_device_key record not found!!");
        return kNotFoundErr;
     }

     APPL_TRACE_DEBUG3("%s end_offset:%d , total_devices:%d",__FUNCTION__, dct_cb.end_offset,dct_cb.total_devices );

     return MICO_BT_SUCCESS;

}
#endif

#endif /* #if defined(BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED) && (BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED == TRUE) */
