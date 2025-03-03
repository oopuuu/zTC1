/*! \file mfg_psm.h
 * \brief APIs for using Manufacturing PSM (Persistent Storage Manager)
 * partition.
 */

/* Copyright (C) 2015, Marvell International Ltd.
 * All Rights Reserved.
 */
#ifndef __MFG_PSM_H__
#define __MFG_PSM_H__

/**
 * Function to initialize MFG CLI
 * @return WM_SUCCESS if successful
 * @return -WM_FAIL otherwise
*/
int mfg_cli_init();

/**
 * Function to initialize MFG partition
 * @return WM_SUCCESS if successful
 * @return -WM_FAIL otherwise
*/
int mfg_psm_init();

/** Read a Manufacturing PSM variable
 *
 * This function reads the value of the requested variable from the
 * manufacturing PSM partition.
 *
 * \param [in] var_name Name of the variable whose value is required.
 * \param [out] buf Pointer to a buffer allocated to hold the value of the PSM
 * variable.
 * \param [in] max_len Size of the buffer allocated.
 *
 * \return  If operation is successful then length of
 * the value field will be returned.
 *
 * \return -WM_E_INVAL Invalid arguments.
 * \return -WM_E_NOSPC If variable value exceeds buffer length given.
 * \return -WM_FAIL If any other error occurs.
 */
int mfg_get_variable(const char *var_name, char *buf, int max_len);
#endif /* __MFG_PSM_H__ */
