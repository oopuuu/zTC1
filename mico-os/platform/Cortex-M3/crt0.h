/**
 ******************************************************************************
 * @file    crt0.h
 * @author  William Xu
 * @version V1.0.0
 * @date    16-Sep-2014
 * @brief   This file provides functions called by MICO for initialization.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#pragma once

extern int main( void );
extern void init_clocks( void );
extern void init_memory( void );
extern void init_architecture( void );
extern void init_platform( void );

