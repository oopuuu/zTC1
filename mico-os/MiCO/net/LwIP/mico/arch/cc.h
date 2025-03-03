/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __CC_H__
#define __CC_H__

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include "../../../mico/cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LWIP_NO_STDINT_H 1

typedef uint8_t          u8_t;
typedef int8_t           s8_t;
typedef uint16_t         u16_t;
typedef int16_t          s16_t;
typedef uint32_t         u32_t;
typedef int32_t          s32_t;
typedef ptrdiff_t        mem_ptr_t;
typedef int              sys_prot_t;

#define U16_F PRIu16
#define S16_F PRId16
#define X16_F PRIx16
#define U32_F PRIu32
#define S32_F PRId32
#define X32_F PRIx32

#define SZT_F "lu"

#if defined __GNUC__
#define LWIP_TIMEVAL_PRIVATE  0
#endif

#ifdef __GNUC__
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#elif defined( __IAR_SYSTEMS_ICC__ )
#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#endif


/* Select how LwIP debug will print */
#define LWIP_PLATFORM_DIAG(x)    {printf x;}

#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS

#ifdef MICO_LWIP_DEBUG
#define LWIP_PLATFORM_ASSERT(x) MICO_ASSERTION_FAIL_ACTION()
#else
#define LWIP_PLATFORM_ASSERT(x)
#endif /* ifdef MICO_LWIP_DEBUG */

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif /* __CC_H__ */
