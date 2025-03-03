/**
 ******************************************************************************
 * @file    URLUtils.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This header contains function prototypes that aid in parsing URLs.
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

#ifndef __URLUtils_h__
#define __URLUtils_h__

#include "common.h"

/** @addtogroup MICO_Middleware_Interface
  * @{
  */

/** @defgroup MICO_URL_Utils MiCO URL Utils
  * @brief Provide URLParse Components.
  * @{
  */


typedef struct
{
    const char *    schemePtr;
    size_t          schemeLen;
    const char *    userPtr;
    size_t          userLen;
    const char *    passwordPtr;
    size_t          passwordLen;
    const char *    hostPtr;
    size_t          hostLen;
    const char *    pathPtr;
    size_t          pathLen;
    const char *    queryPtr;
    size_t          queryLen;
    const char *    fragmentPtr;
    size_t          fragmentLen;
    
    const char *    segmentPtr; // Ptr to the current resource path segment. Leading slash is removed, if present.
    const char *    segmentEnd; // End of the resource path segments. Trailing slash is removed, if present.
    
} URLComponents;



/**
 * @brief Parses an absolute or relative URL into the general components supported by all URI's.
 *
 * @param inSrc:   ?
 * @param inEnd:   ?
 * @param outComponents:   ?
 * @param outSrc����
 *
 * @return   ��
 */
int URLParseComponents( const char *inSrc, const char *inEnd, URLComponents *outComponents, const char **outSrc );



/**
 * @brief ?
 *
 * @param inURL:   ?
 *
 * @return None
 */
void PrintURL( URLComponents *inURL );


/**
  * @}
  */

/**
  * @}
  */

#endif // __URLUtils_h__

