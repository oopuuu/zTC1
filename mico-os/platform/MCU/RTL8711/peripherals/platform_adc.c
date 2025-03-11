/**
 ******************************************************************************
 * @file    paltform_adc.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide ADC driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */


#include "mico_platform.h"
#include "mico_rtos.h"

#include "platform.h"
#include "platform_peripheral.h"
//#include "stm32f2xx.h"
#include "platform_logging.h"
//#include "hal_adc.h"

/******************************************************
 *                    Constants
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
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/


/******************************************************
 *               Function Definitions
 ******************************************************/

void rtk_adc_init(analogin_t *obj){

    uint32_t adc_idx;
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_USERCB_ADPT    pSalADCUserCBAdpt   = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;

    //adc_idx = adc->pin & 0x0F;
    
    /* Get I2C device handler */
    pSalADCMngtAdpt     = &(obj->SalADCMngtAdpt);
    pSalADCUserCBAdpt   = (PSAL_ADC_USERCB_ADPT)&(obj->SalADCUserCBAdpt);   

    /*To assign the rest pointers*/
    pSalADCMngtAdpt->pSalHndPriv    = &(obj->SalADCHndPriv);
    pSalADCMngtAdpt->pSalHndPriv->ppSalADCHnd = (void**)&(pSalADCMngtAdpt->pSalHndPriv);

    /* To assign the default (ROM) HAL OP initialization function */
    pSalADCMngtAdpt->pHalOpInit         = &HalADCOpInit;

    /* To assign the default (ROM) HAL GDMA OP initialization function */
    pSalADCMngtAdpt->pHalGdmaOpInit     = &HalGdmaOpInit;

    /* To assign the default (ROM) SAL interrupt function */
    pSalADCMngtAdpt->pSalIrqFunc        = &ADCISRHandle;

    /* To assign the default (ROM) SAL DMA TX interrupt function */
    pSalADCMngtAdpt->pSalDMAIrqFunc     = &ADCGDMAISRHandle;

    pSalADCMngtAdpt->pHalInitDat        = &(obj->HalADCInitData);
    pSalADCMngtAdpt->pHalOp             = &(obj->HalADCOp);
    pSalADCMngtAdpt->pIrqHnd            = &(obj->ADCIrqHandleDat);
    pSalADCMngtAdpt->pHalGdmaAdp        = &(obj->HalADCGdmaAdpt);
    pSalADCMngtAdpt->pHalGdmaOp         = &(obj->HalADCGdmaOp);
    pSalADCMngtAdpt->pIrqGdmaHnd        = &(obj->ADCGdmaIrqHandleDat);
    pSalADCMngtAdpt->pUserCB            = &(obj->SalADCUserCB);
    
    /* Assign the private SAL handle to public SAL handle */
    pSalADCHND      = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);

    /* Assign the internal HAL initial data pointer to the SAL handle */
    pSalADCHND->pInitDat    = pSalADCMngtAdpt->pHalInitDat;

    /* Assign the internal user callback pointer to the SAL handle */
    pSalADCHND->pUserCB     = pSalADCMngtAdpt->pUserCB;

    /*To assign user callback pointers*/
    pSalADCMngtAdpt->pUserCB->pTXCB     = pSalADCUserCBAdpt;
    pSalADCMngtAdpt->pUserCB->pTXCCB    = (pSalADCUserCBAdpt+1);
    pSalADCMngtAdpt->pUserCB->pRXCB     = (pSalADCUserCBAdpt+2);
    pSalADCMngtAdpt->pUserCB->pRXCCB    = (pSalADCUserCBAdpt+3);
    pSalADCMngtAdpt->pUserCB->pRDREQCB  = (pSalADCUserCBAdpt+4);
    pSalADCMngtAdpt->pUserCB->pERRCB    = (pSalADCUserCBAdpt+5);
    pSalADCMngtAdpt->pUserCB->pDMATXCB  = (pSalADCUserCBAdpt+6);
    pSalADCMngtAdpt->pUserCB->pDMATXCCB = (pSalADCUserCBAdpt+7);
    pSalADCMngtAdpt->pUserCB->pDMARXCB  = (pSalADCUserCBAdpt+8);
    pSalADCMngtAdpt->pUserCB->pDMARXCCB = (pSalADCUserCBAdpt+9);
        
    /* Set ADC Device Number */
    pSalADCHND->DevNum = adc_idx;

    /* Load ADC default value */
    RtkADCLoadDefault(pSalADCHND);

    /* Assign ADC Pin Mux */
    pSalADCHND->PinMux        = 0;
    pSalADCHND->OpType        = ADC_RDREG_TYPE;
    
    /* Init ADC now */
    pSalADCHND->pInitDat->ADCBurstSz       =   8;
    pSalADCHND->pInitDat->ADCOneShotTD     =   8;
    RtkADCInit(pSalADCHND); 
}

PSAL_ADC_MNGT_ADPT get_adc_adaptor(platform_adc_t* adc){
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    pSalADCMngtAdpt         = &(adc->obj.SalADCMngtAdpt);
    return pSalADCMngtAdpt;
}	

OSStatus platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle )
{

    OSStatus    err = kNoErr;

    platform_mcu_powersave_disable();

    require_action_quiet( adc != NULL, exit, err = kParamErr);

    analogin_init( &adc->obj, adc->pin );


exit:
    platform_mcu_powersave_enable();
    return err;
}

OSStatus platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output )
{
    OSStatus    err = kNoErr;
    float value;
    uint32_t AnaloginTmp[2]      = {0,0};
    uint32_t AnaloginDatMsk      = 0xFFFF;
    uint8_t  AnaloginIdx         = 0;
    uint32_t AnalogDat           = 0;   
	
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;
	
    platform_mcu_powersave_disable();

    require_action_quiet( adc != NULL, exit, err = kParamErr);
    
    pSalADCMngtAdpt         = get_adc_adaptor(adc);
    pSalADCHND              = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);
    AnaloginIdx             = pSalADCHND->DevNum;
    RtkADCReceiveBuf(pSalADCHND,&AnaloginTmp[0]);
    
    AnaloginDatMsk = (u32)(AnaloginDatMsk<<((u32)(16*(AnaloginIdx&0x01))));
    AnalogDat = AnaloginTmp[(AnaloginIdx/2)];
    AnalogDat = (AnalogDat & AnaloginDatMsk);    
    AnalogDat = (AnalogDat>>((u32)(16*(AnaloginIdx&0x01))));
    
    *output = (uint16_t)AnalogDat;

exit:
    platform_mcu_powersave_enable();
    return err;
}

OSStatus platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length )
{
    UNUSED_PARAMETER(adc);
    UNUSED_PARAMETER(buffer);
    UNUSED_PARAMETER(buffer_length);
    platform_log("unimplemented");
    return kNotPreparedErr;
}

OSStatus platform_adc_deinit( const platform_adc_t* adc )
{
    OSStatus    err = kNoErr;
    PSAL_ADC_MNGT_ADPT      pSalADCMngtAdpt     = NULL;
    PSAL_ADC_HND            pSalADCHND          = NULL;

    require_action_quiet( adc != NULL, exit, err = kParamErr);
    
    //pSalADCMngtAdpt         = &(adc->obj.SalADCMngtAdpt);
    pSalADCMngtAdpt         = get_adc_adaptor(adc);    
    pSalADCHND              = &(pSalADCMngtAdpt->pSalHndPriv->SalADCHndPriv);

    /* To deinit analogin */
    RtkADCDeInit(pSalADCHND);    
	
exit:	
    return err;
}


