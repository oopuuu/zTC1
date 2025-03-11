/**
 ******************************************************************************
 * @file    paltform_rng.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide RNG driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "platform.h"
#include "stm32f2xx.h"
#include "common.h"

/******************************************************
 *                   Macros
 ******************************************************/


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
 *                     Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus platform_random_number_read( void *inBuffer, int inByteCount )
{
    // PLATFORM_TO_DO
    int idx;
    uint32_t *pWord = inBuffer;
    uint32_t tempRDM;
    uint8_t *pByte = NULL;
    int inWordCount;
    int remainByteCount;

    inWordCount = inByteCount/4;
    remainByteCount = inByteCount%4;
    pByte = (uint8_t *)pWord+inWordCount*4;

    RNG_DeInit();
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RNG_Cmd(ENABLE);

    for(idx = 0; idx<inWordCount; idx++, pWord++){
        while(RNG_GetFlagStatus(RNG_FLAG_DRDY)!=SET);
        *pWord = RNG_GetRandomNumber();
    }

    if(remainByteCount){
        while(RNG_GetFlagStatus(RNG_FLAG_DRDY)!=SET);
        tempRDM = RNG_GetRandomNumber();
        memcpy(pByte, &tempRDM, (size_t)remainByteCount);
    }
    
    RNG_DeInit();
    return kNoErr;
}
