/**
 ******************************************************************************
 * @file    qc_test_internal.h
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Dec-2016
 * @brief   QC test internal header file.
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

#include "platform.h"

/******************************************************
*                      Macros
******************************************************/

#define QC_TEST_PRINT_STRING(NAME, CONTENT) do{ mf_printf(NAME); mf_printf(" "); mf_printf(CONTENT); mf_printf("\r\n");}while(0)
#define QC_TEST_PRINT_STRING_FUN(NAME, CONTENT_FUNC) do{    mf_printf(NAME); \
                                                            mf_printf(" ");  \
                                                            memset (str, 0, sizeof (str));  \
                                                            CONTENT_FUNC (str, sizeof (str));  \
                                                            mf_printf(str);  \
                                                            mf_printf("\r\n");}while(0)

/******************************************************
*                 Type Definitions
******************************************************/

typedef struct
{
    mico_gpio_t output_pin;
    mico_gpio_t input_pin;
} qc_test_gpio_pair_t;

/******************************************************
*               Function Declarations
******************************************************/

void mf_printf(char *str);
void mf_putc(char ch);
int mf_get_line( char** p_cmd_str );

void qc_test_ble(void);
void qc_test_gpio(void);
void qc_test_tcpip(void);
void qc_scan(void);




