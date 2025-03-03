/**
 ******************************************************************************
 * @file    qc_test_gpio.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Dec-2016
 * @brief   This file provide the GPIO test functions. Connect two GPIO into a
 *          pair, one input GPIO read another output GPIO.
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


#include "mico.h"
#include "qc_test_internal.h"

/******************************************************
*               Variables Definitions
******************************************************/

extern const qc_test_gpio_pair_t qc_test_gpio_pairs[];
extern const int qc_test_gpio_pairs_num;

static int gpio_result = 0;

/******************************************************
*               Function Definitions
******************************************************/

static OSStatus _gpio_test_one( const qc_test_gpio_pair_t* gpio_test_pair )
{
    mico_gpio_t in, out;
    OSStatus err = kGeneralErr;

    in  = gpio_test_pair->input_pin;
    out = gpio_test_pair->output_pin;

    MicoGpioInitialize(out, OUTPUT_PUSH_PULL);

    MicoGpioInitialize(in, INPUT_PULL_DOWN);
    MicoGpioOutputHigh(out);
    mico_rtos_thread_msleep(1);
    if (MicoGpioInputGet(in) != true)
        goto EXIT;

    MicoGpioInitialize(in, INPUT_PULL_UP);
    MicoGpioOutputLow(out);
    mico_rtos_thread_msleep(1);
    if (MicoGpioInputGet(in) != false)
        goto EXIT;

    err = kNoErr;

EXIT:
    MicoGpioInitialize(in, INPUT_HIGH_IMPEDANCE);
    MicoGpioInitialize(out, INPUT_HIGH_IMPEDANCE);
    return err;
}

static OSStatus _gpio_test( const qc_test_gpio_pair_t* gpio_test_pair, int num )
{
    int i;
    OSStatus err = kNoErr;
    mico_gpio_t in, out;
    qc_test_gpio_pair_t * gpio_test = gpio_test_pair;
    
    gpio_result = 0;
    for ( i = 0; i < num; i++ )
    {
        in  = gpio_test->input_pin;
        out = gpio_test->output_pin;
        MicoGpioInitialize(in,OUTPUT_OPEN_DRAIN_NO_PULL);
        MicoGpioInitialize(out,OUTPUT_OPEN_DRAIN_NO_PULL);
        gpio_test++;
    }
    
    for ( i = 0; i < num; i++ )
    {
        if ( kNoErr == _gpio_test_one( &gpio_test_pair[i] ) )
        {
            gpio_result |= (1 << i);
        }
        else
        {
            err = kGeneralErr;
        }
    }
    
    return err;
}

void qc_test_gpio( void )
{
    char test_result[32];
    int gpio_num = 0;

    if ( kNoErr == _gpio_test( qc_test_gpio_pairs, qc_test_gpio_pairs_num ) )
    {
        QC_TEST_PRINT_STRING( "GPIO TEST:", "PASS" );
    }
    else
    {
        sprintf( test_result, "Fail: %d:%x", gpio_num, gpio_result );
        QC_TEST_PRINT_STRING( "GPIO TEST:", test_result );
    }

    return;
}














