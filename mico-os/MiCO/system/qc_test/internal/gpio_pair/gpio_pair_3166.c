/**
 ******************************************************************************
 * @file    gpio_pair_3081.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Dec-2016
 * @brief   This file provide the definition of GPIO pairs used for GPIO QC test.
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


#include "qc_test_internal.h"
#include "platform.h"

const qc_test_gpio_pair_t qc_test_gpio_pairs[] =
{
    {MICO_GPIO_2,  MICO_GPIO_4},
    {MICO_GPIO_5,  MICO_GPIO_6},
    {MICO_GPIO_6,  MICO_GPIO_7},
    {MICO_GPIO_16,  MICO_GPIO_17},
    {MICO_GPIO_18,  MICO_GPIO_19},
    {MICO_GPIO_27,  MICO_GPIO_31},
    {MICO_GPIO_33,  MICO_GPIO_34},
    {MICO_GPIO_35,  MICO_GPIO_38},
   // {MICO_GPIO_35,  MICO_GPIO_38}
};

const int qc_test_gpio_pairs_num = sizeof(qc_test_gpio_pairs) / sizeof(qc_test_gpio_pair_t);









