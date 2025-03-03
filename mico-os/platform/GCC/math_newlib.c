/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */


/**
 *  Sine function
 *
 *  Simple Taylor series approximation of sine function
 *  Since Newlib doesn't contain one
 *  see http://www.wolframalpha.com/input/?i=taylor+series+sin+x
 *
 * @param x : the value for which Sine will be computed
 *
 * @return  the Sine of x
 */
#define PI (3.1415926)

double sin( double x )
{
    int term = 1;
    double val = 0;

    x -= ( (int) ( x / ( 2 * PI ) ) ) * 2 * PI;

    if ( x > PI )
    {
        x -= 2 * PI;
    }
    if ( x < -PI )
    {
        x += 2 * PI;
    }

    for ( term = 0; term < 6; term++ )
    {
        double multval = x;
        double denval = 1;
        int ex;
        for ( ex = 0; ex < ( term * 2 ); ex++ )
        {
            multval *= x;
            denval *= ( ex + 2 );
        }
        val += ( ( term % 2 == 1 ) ? -1 : 1 ) * multval / denval;
    }
    return val;
}

/* Simple Taylor series approximation of natural logarithm function
 * see http://www.efunda.com/math/taylor_series/logarithmic.cfm
 */

double log( double x )
{
    int term = 1;
    double val = 0;

    for ( term = 1; term < 5; term++ )
    {
        double multval = ( x - 1.0 ) / ( x + 1.0 );
        int ex;
        for ( ex = 0; ex < ( term * 2 - 2 ); ex++ )
        {
            multval *= multval;
        }
        val += multval / ( term * 2 - 1 );
    }
    return 2 * val;
}
