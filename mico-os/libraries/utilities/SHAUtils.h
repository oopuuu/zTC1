/**
 ******************************************************************************
 * @file    SHAUtils.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   SHA Utilities
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


#ifndef __SHAUtils_h_
#define __SHAUtils_h_

#include "common.h"


//===========================================================================================================================
//  SHA-1
//===========================================================================================================================

typedef struct
{
    uint64_t        length;
    uint32_t        state[ 5 ];
    uint32_t        curlen;
    uint8_t         buf[ 64 ];
    
}   SHA_CTX_compat;

int SHA1_Init_compat( SHA_CTX_compat *ctx );
int SHA1_Update_compat( SHA_CTX_compat *ctx, const void *inData, size_t inLen );
int SHA1_Final_compat( unsigned char *outDigest, SHA_CTX_compat *ctx );
unsigned char * SHA1_compat( const void *inData, size_t inLen, unsigned char *outDigest );

//===========================================================================================================================
//  SHA-512
//===========================================================================================================================

typedef struct
{
    uint64_t        length;
    uint64_t        state[ 8 ];
    size_t          curlen;
    uint8_t         buf[ 128 ];
    
}   SHA512_CTX_compat;

int SHA512_Init_compat( SHA512_CTX_compat *ctx );
int SHA512_Update_compat( SHA512_CTX_compat *ctx, const void *inData, size_t inLen );
int SHA512_Final_compat( unsigned char *outDigest, SHA512_CTX_compat *ctx );
unsigned char * SHA512_compat( const void *inData, size_t inLen, unsigned char *outDigest );

//===========================================================================================================================
//  SHA-3 (Keccak)
//===========================================================================================================================

#define SHA3_DIGEST_LENGTH      64
#define SHA3_F                  1600
#define SHA3_C                  ( SHA3_DIGEST_LENGTH * 8 * 2 )  // 512=1024
#define SHA3_R                  ( SHA3_F - SHA3_C )             // 512=576
#define SHA3_BLOCK_SIZE         ( SHA3_R / 8 )

typedef struct
{
    uint64_t        state[ SHA3_F / 64 ];
    size_t          leftover;
    uint8_t         buffer[ SHA3_BLOCK_SIZE ];
    
}   SHA3_CTX_compat;

int SHA3_Init_compat( SHA3_CTX_compat *ctx );
int SHA3_Update_compat( SHA3_CTX_compat *ctx, const void *inData, size_t inLen );
int SHA3_Final_compat( unsigned char *outDigest, SHA3_CTX_compat *ctx );
uint8_t *   SHA3_compat( const void *inData, size_t inLen, uint8_t outDigest[ 64 ] );

#endif // __SHAUtils_h_


