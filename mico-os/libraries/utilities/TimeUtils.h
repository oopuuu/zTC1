/**
 ******************************************************************************
 * @file    TimeUtils.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This header contains function prototypes which aid in time calculations.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */


#ifndef __TimeUtils_h__
#define __TimeUtils_h__

#include "common.h"


/** @addtogroup MICO_Middleware_Interface
  * @{
  */

/** @defgroup MICO_Time_Utils MiCO Time Utils
  * @brief Provide APIs for Time Utils
  * @{
  */


// ==== SECOND CONVERSION CONSTANTS ====
//#define NANOSECONDS  1000000UL
//#define MICROSECONDS 1000
//#define MILLISECONDS 1


// ==== TIME DIFF UTILS ====
/**
 * @brief ?
 *
 * @param inStart:   ?
 * @param inEnd:   ?
 *
 * @return ?
 */

uint32_t TimeDifference(uint32_t inStart, uint32_t inEnd);


// ==== ELAPSED TIME UTILS ====
/**
 * @brief ?
 *
 * @param inDiff:   ?
 *
 * @return ?
 */
long long ElapsedTimeInMilliseconds(uint32_t inDiff);

/**
 * @brief ?
 *
 * @param inDiff:   ?
 *
 * @return ?
 */
long long ElapsedTimeInMicroseconds(uint32_t inDiff);


/**
 * @brief ?
 *
 * @param inDiff:   ?
 *
 * @return ?
 */
long long ElapsedTimeInNanoseconds(uint32_t inDiff);



// ==== TICK UTILS ====
/**
 * @brief ?
 *
 * @param inDiff:   ?
 *
 * @return ?
 */
uint64_t UpTicks( void );


/**
 * @brief ?
 *
 * @param inDiff:   ?
 *
 * @return ?
 */
uint64_t UpTicksPerSecond( void );


/**
 * @brief ?
 *
 * @param inDiff:   ?
 *
 * @return none
 */
void SleepForUpTicks( uint64_t inTicks );


/**
  * @}
  */

/**
  * @}
  */

#endif


