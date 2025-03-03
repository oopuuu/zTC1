/**
 ******************************************************************************
 * @file    mico_rtos_common.h
 * @author  William Xu
 * @version V1.0.0
 * @date    22-Aug-2016
 * @brief   Definitions of the MiCO Common RTOS abstraction layer function headers
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 ******************************************************************************
 */


#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define rtos_log(M, ...) custom_log("RTOS", M, ##__VA_ARGS__)
#define rtos_log_trace() custom_log_trace("RTOS")

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/* MiCO <-> RTOS API */
extern OSStatus mico_rtos_init  ( void );
extern OSStatus mico_rtos_deinit( void );

/* Entry point for user Application */
extern void application_start          ( void );


#ifdef __cplusplus
} /* extern "C" */
#endif
