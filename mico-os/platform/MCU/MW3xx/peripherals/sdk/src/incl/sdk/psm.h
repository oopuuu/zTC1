/*! \file psm.h
 *
 * \brief Persistent storage module version 1 (Deprecated)
 *
 * Please use version 2 (\ref psm-v2.h)
 */

/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef _PSM_H_
#define _PSM_H_

#include <psm-v2.h>
/** Flag to ask psm to create the partition if it doesn't exist.
 * Used with psm_register_module()
 */
#define PSM_CREAT 1

/* Deprecated. Please use PSM-v2 APIs */
typedef struct psm_handle {
        /** The module name */
	char   psmh_mod_name[32];
} psm_handle_t;

/* Deprecated. Please use PSM-v2 APIs */
int psm_register_module(const char *module_name, 
			const char *partition_key, short flags);

/* Deprecated. Please use PSM-v2 APIs */
int psm_open(psm_handle_t *handle, const char *module_name);

/* Deprecated. Please use PSM-v2 APIs */
void psm_close(psm_handle_t *handle);

/* Deprecated. Please use PSM-v2 APIs */
int psm_set(psm_handle_t *handle, const char *variable, const char *value);

/* Deprecated. Please use PSM-v2 APIs */
int psm_get(psm_handle_t *handle, const char *variable, 
	    char *value, int value_len);

/* Deprecated. Please use PSM-v2 APIs */
int psm_delete(psm_handle_t *handle, const char *variable);

/* Deprecated. Please use PSM-v2 APIs */
int psm_erase_and_init();

/* Deprecated. Please use PSM-v2 APIs */
int psm_erase_partition(short partition_id);

/* Deprecated. Please use PSM-v2 APIs */
int psm_init(flash_desc_t *fl);

int psm_cli_init(void);

/* Deprecated. Please use PSM-v2 APIs */
int psm_get_single(const char *module, const char *variable,
		   char *value, unsigned max_len);

/* Deprecated. Please use PSM-v2 APIs */
int psm_set_single(const char *module, const char *variable, const char *value);

/* Deprecated. Please use PSM-v2 APIs */
int psm_delete_single(const char *module, const char *variable);

#endif /* !  _PSM_H_ */
