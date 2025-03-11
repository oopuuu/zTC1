/**
 ******************************************************************************
 * @file    paltform_i2c.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide I2C driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */


#include "mico_rtos.h"
#include "mico_platform.h"

#include "platform.h"
#include "platform_peripheral.h"
#include "platform_logging.h"

#include "pinmap.h"

/******************************************************
*                    Constants
******************************************************/
//#define I2C_USE_DMA           
#define  I2C_Direction_Transmitter      ((uint8_t)0x00)
#define  I2C_Direction_Receiver         ((uint8_t)0x01)
#define IS_I2C_DIRECTION(DIRECTION) (((DIRECTION) == I2C_Direction_Transmitter) || \
                                     ((DIRECTION) == I2C_Direction_Receiver))

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
static const PinMap PinMap_I2C_SDA[] = {
    {PD_4,  RTL_PIN_PERI(I2C0, 0, S0), RTL_PIN_FUNC(I2C0, S0)},
    {PH_1,  RTL_PIN_PERI(I2C0, 0, S1), RTL_PIN_FUNC(I2C0, S1)},
    {PC_8,  RTL_PIN_PERI(I2C0, 0, S2), RTL_PIN_FUNC(I2C0, S2)},
    {PE_7,  RTL_PIN_PERI(I2C0, 0, S3), RTL_PIN_FUNC(I2C0, S3)},
    
    {PC_4,  RTL_PIN_PERI(I2C1, 1, S0), RTL_PIN_FUNC(I2C1, S0)},
    {PH_3,  RTL_PIN_PERI(I2C1, 1, S1), RTL_PIN_FUNC(I2C1, S1)},
    {PD_7,  RTL_PIN_PERI(I2C1, 1, S2), RTL_PIN_FUNC(I2C1, S2)},

    {PB_7,  RTL_PIN_PERI(I2C2, 2, S0), RTL_PIN_FUNC(I2C2, S0)},
    {PE_1,  RTL_PIN_PERI(I2C2, 2, S1), RTL_PIN_FUNC(I2C2, S1)},
    {PC_7,  RTL_PIN_PERI(I2C2, 2, S2), RTL_PIN_FUNC(I2C2, S2)},

    {PB_3,  RTL_PIN_PERI(I2C3, 3, S0), RTL_PIN_FUNC(I2C3, S0)},
    {PE_3,  RTL_PIN_PERI(I2C3, 3, S1), RTL_PIN_FUNC(I2C3, S1)},
    {PE_5,  RTL_PIN_PERI(I2C3, 3, S2), RTL_PIN_FUNC(I2C3, S2)},
    {PD_9,  RTL_PIN_PERI(I2C3, 3, S3), RTL_PIN_FUNC(I2C3, S3)},

    {NC,    NC,     0}
};

static const PinMap PinMap_I2C_SCL[] = {
    {PD_5,  RTL_PIN_PERI(I2C0, 0, S0), RTL_PIN_FUNC(I2C0, S0)},
    {PH_0,  RTL_PIN_PERI(I2C0, 0, S1), RTL_PIN_FUNC(I2C0, S1)},
    {PC_9,  RTL_PIN_PERI(I2C0, 0, S2), RTL_PIN_FUNC(I2C0, S2)},
    {PE_6,  RTL_PIN_PERI(I2C0, 0, S3), RTL_PIN_FUNC(I2C0, S3)},

    {PC_5,  RTL_PIN_PERI(I2C1, 1, S0), RTL_PIN_FUNC(I2C1, S0)},
    {PH_2,  RTL_PIN_PERI(I2C1, 1, S1), RTL_PIN_FUNC(I2C1, S1)},
    {PD_6,  RTL_PIN_PERI(I2C1, 1, S2), RTL_PIN_FUNC(I2C1, S2)},

    {PB_6,  RTL_PIN_PERI(I2C2, 2, S0), RTL_PIN_FUNC(I2C2, S0)},
    {PE_0,  RTL_PIN_PERI(I2C2, 2, S1), RTL_PIN_FUNC(I2C2, S1)},
    {PC_6,  RTL_PIN_PERI(I2C2, 2, S2), RTL_PIN_FUNC(I2C2, S2)},
    
    {PB_2,  RTL_PIN_PERI(I2C3, 3, S0), RTL_PIN_FUNC(I2C3, S0)},
    {PE_2,  RTL_PIN_PERI(I2C3, 3, S1), RTL_PIN_FUNC(I2C3, S1)},
    {PE_4,  RTL_PIN_PERI(I2C3, 3, S2), RTL_PIN_FUNC(I2C3, S2)},
    {PD_8,  RTL_PIN_PERI(I2C3, 3, S3), RTL_PIN_FUNC(I2C3, S3)},

    {NC,    NC,     0}
};

static uint16_t i2c_target_addr[4];
static SAL_I2C_TRANSFER_BUF    i2ctxtranbuf[4];
static SAL_I2C_TRANSFER_BUF    i2crxtranbuf[4];
/******************************************************
*               Function Declarations
******************************************************/

static OSStatus i2c_address_device( platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries, uint8_t direction );
//static OSStatus i2c_wait_for_event( I2C_TypeDef* i2c, uint32_t event_id, uint32_t number_of_waits );
static OSStatus i2c_transfer_message_no_dma( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message );
static OSStatus i2c_tx_no_dma( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message );
static OSStatus i2c_rx_no_dma( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message );


/******************************************************
*               Function Definitions
******************************************************/
OSStatus rtk_i2c_init( platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
  OSStatus err = kNoErr;
  uint32_t i2c_sel;
  uint32_t i2c_idx;
  PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
  PSAL_I2C_USERCB_ADPT    pSalI2CUserCBAdpt   = NULL;
  PSAL_I2C_HND            pSalI2CHND          = NULL;

  
  // Determine the I2C to use
  uint32_t i2c_sda = (uint32_t)pinmap_peripheral(i2c->sda, PinMap_I2C_SDA);
  uint32_t i2c_scl = (uint32_t)pinmap_peripheral(i2c->scl, PinMap_I2C_SCL);

  i2c_sel = (uint32_t)pinmap_merge(i2c_sda, i2c_scl);
  i2c_idx = RTL_GET_PERI_IDX(i2c_sel);
  if (unlikely(i2c_idx == NC)) {
      DBG_8195A("%s: Cannot find matched UART\n", __FUNCTION__);
      return kParamErr;
  }
  
  DBG_8195A("i2c_sel:%x\n",i2c_sel);
  DBG_8195A("i2c_idx:%x\n",i2c_idx);
  
  /* Get I2C device handler */
  pSalI2CMngtAdpt     = &(i2c->SalI2CMngtAdpt);
  pSalI2CUserCBAdpt   = (PSAL_I2C_USERCB_ADPT)&(i2c->SalI2CUserCBAdpt);
  

  /*To assign the rest pointers*/
  pSalI2CMngtAdpt->MstRDCmdCnt    = 0;
  pSalI2CMngtAdpt->InnerTimeOut   = 2000; // inner time-out count, 2000 ms
  pSalI2CMngtAdpt->pSalHndPriv    = &(i2c->SalI2CHndPriv);
  pSalI2CMngtAdpt->pSalHndPriv->ppSalI2CHnd = (void**)&(pSalI2CMngtAdpt->pSalHndPriv);

  /* To assign the default (ROM) HAL OP initialization function */
  pSalI2CMngtAdpt->pHalOpInit     = &HalI2COpInit;

  /* To assign the default (ROM) HAL GDMA OP initialization function */
  pSalI2CMngtAdpt->pHalGdmaOpInit = &HalGdmaOpInit;

  /* To assign the default (ROM) SAL interrupt function */
  pSalI2CMngtAdpt->pSalIrqFunc    = &I2CISRHandle;

  /* To assign the default (ROM) SAL DMA TX interrupt function */
  pSalI2CMngtAdpt->pSalDMATxIrqFunc   = &I2CTXGDMAISRHandle;

  /* To assign the default (ROM) SAL DMA RX interrupt function */
  pSalI2CMngtAdpt->pSalDMARxIrqFunc   = &I2CRXGDMAISRHandle;

  pSalI2CMngtAdpt->pHalInitDat        = &(i2c->HalI2CInitData);
  pSalI2CMngtAdpt->pHalOp             = &(i2c->HalI2COp);
  pSalI2CMngtAdpt->pIrqHnd            = &(i2c->I2CIrqHandleDat);
  pSalI2CMngtAdpt->pHalTxGdmaAdp      = &(i2c->HalI2CTxGdmaAdpt);
  pSalI2CMngtAdpt->pHalRxGdmaAdp      = &(i2c->HalI2CRxGdmaAdpt);
  pSalI2CMngtAdpt->pHalGdmaOp         = &(i2c->HalI2CGdmaOp);
  pSalI2CMngtAdpt->pIrqTxGdmaHnd      = &(i2c->I2CTxGdmaIrqHandleDat);
  pSalI2CMngtAdpt->pIrqRxGdmaHnd      = &(i2c->I2CRxGdmaIrqHandleDat);
  pSalI2CMngtAdpt->pUserCB            = &(i2c->SalI2CUserCB);
  pSalI2CMngtAdpt->pDMAConf           = &(i2c->SalI2CDmaUserDef);
  
  /* Assign the private SAL handle to public SAL handle */
  pSalI2CHND      = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);

  /* Assign the internal HAL initial data pointer to the SAL handle */
  pSalI2CHND->pInitDat    = pSalI2CMngtAdpt->pHalInitDat;

  /* Assign the internal user callback pointer to the SAL handle */
  pSalI2CHND->pUserCB     = pSalI2CMngtAdpt->pUserCB;

  /* Assign the internal user define DMA configuration to the SAL handle */
  pSalI2CHND->pDMAConf    = pSalI2CMngtAdpt->pDMAConf;

  /*To assign user callback pointers*/
  pSalI2CMngtAdpt->pUserCB->pTXCB     = pSalI2CUserCBAdpt;
  pSalI2CMngtAdpt->pUserCB->pTXCCB    = (pSalI2CUserCBAdpt+1);
  pSalI2CMngtAdpt->pUserCB->pRXCB     = (pSalI2CUserCBAdpt+2);
  pSalI2CMngtAdpt->pUserCB->pRXCCB    = (pSalI2CUserCBAdpt+3);
  pSalI2CMngtAdpt->pUserCB->pRDREQCB  = (pSalI2CUserCBAdpt+4);
  pSalI2CMngtAdpt->pUserCB->pERRCB    = (pSalI2CUserCBAdpt+5);
  pSalI2CMngtAdpt->pUserCB->pDMATXCB  = (pSalI2CUserCBAdpt+6);
  pSalI2CMngtAdpt->pUserCB->pDMATXCCB = (pSalI2CUserCBAdpt+7);
  pSalI2CMngtAdpt->pUserCB->pDMARXCB  = (pSalI2CUserCBAdpt+8);
  pSalI2CMngtAdpt->pUserCB->pDMARXCCB = (pSalI2CUserCBAdpt+9);
  pSalI2CMngtAdpt->pUserCB->pGENCALLCB= (pSalI2CUserCBAdpt+10);
  
  /* Set I2C Device Number */
  pSalI2CHND->DevNum = i2c_idx;

  /* Load I2C default value */
  RtkI2CLoadDefault(pSalI2CHND);

  /* Assign I2C Pin Mux */
  pSalI2CHND->PinMux        = RTL_GET_PERI_SEL(i2c_sel);
  pSalI2CHND->OpType        = I2C_INTR_TYPE;
  pSalI2CHND->I2CMaster     = I2C_MASTER_MODE;
  pSalI2CHND->I2CSpdMod     = I2C_SS_MODE;
  pSalI2CHND->I2CClk        = 100;
  pSalI2CHND->I2CAckAddr    = 0;    
  pSalI2CHND->TimeOut       = 300;
  //pSalI2CHND->AddRtyTimeOut = 3000;
  pSalI2CHND->I2CExd     	|= (I2C_EXD_MTR_ADDR_RTY);
  
  pSalI2CMngtAdpt->InnerTimeOut   = pSalI2CHND->TimeOut;  

  if(config->address_width == I2C_ADDRESS_WIDTH_10BIT || config->address_width == I2C_ADDRESS_WIDTH_16BIT)
  {
  	err = kUnsupportedErr;
	goto exit;
  }

  switch ( config->speed_mode )
  {
    case I2C_LOW_SPEED_MODE     :  platform_log("Speed mode is not supported"); err = kUnsupportedErr; goto exit; break;
    case I2C_STANDARD_SPEED_MODE: pSalI2CHND->I2CSpdMod     = I2C_SS_MODE; break;
    case I2C_HIGH_SPEED_MODE    : pSalI2CHND->I2CSpdMod     = I2C_HS_MODE; break;
    default                     : platform_log("Speed mode is not supported"); err = kUnsupportedErr; goto exit; break;
  }


  /* Deinit I2C first */
  //i2c_reset(obj);

  /* Init I2C now */
  RtkI2CInitForPS(pSalI2CHND); 

exit:
  return err;
}

OSStatus rtk_i2c_deinit( platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
  PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
  PSAL_I2C_HND            pSalI2CHND          = NULL;
  UNUSED_PARAMETER( config );
  OSStatus err = kNoErr;
  
  pSalI2CMngtAdpt         = &(i2c->SalI2CMngtAdpt);
  pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
  /* Deinit I2C directly */
  RtkI2CDeInitForPS(pSalI2CHND);

  return err;
}


OSStatus platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable( );
  require_action_quiet( i2c != NULL, exit, err = kParamErr);

  err = rtk_i2c_init(i2c, config);

exit:
  platform_mcu_powersave_enable( );
  return err;
}


bool platform_i2c_probe_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries )
{
  OSStatus err = kNoErr;
  
  platform_mcu_powersave_disable();

  require_action_quiet( i2c != NULL, exit, err = kParamErr);

  err = i2c_address_device( i2c, config, retries, I2C_Direction_Transmitter );

exit:
  platform_mcu_powersave_enable();
  return ( err == kNoErr) ? true : false;
}


static OSStatus i2c_transfer_message_no_dma( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message )
{
    OSStatus err;

    if ( message->tx_buffer != NULL )
    {
        err = i2c_tx_no_dma( i2c, config, message );
        if ( err != kNoErr )
        {
            goto exit;
        }
    }

    if ( message->rx_buffer != NULL )
    {
        err = i2c_rx_no_dma( i2c, config, message );
    }

exit:
    ///* generate a stop condition */
    //I2C_GenerateSTOP( i2c->port, ENABLE );

    return err;
}



OSStatus platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries )
{
  OSStatus err = kNoErr;

  require_action_quiet( ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ), exit, err = kParamErr);

  memset(message, 0x00, sizeof(mico_i2c_message_t));
  message->tx_buffer = tx_buffer;
  message->retries = retries;
  message->tx_length = tx_buffer_length;
  
exit:  
  return err;
}

OSStatus platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries )
{
  OSStatus err = kNoErr;

  require_action_quiet( ( message != NULL ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ), exit, err = kParamErr);

  memset(message, 0x00, sizeof(mico_i2c_message_t));
  message->rx_buffer = rx_buffer;
  message->retries = retries;
  message->rx_length = rx_buffer_length;
  
exit:
  return err;
}

OSStatus platform_i2c_init_combined_message( platform_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries )
{
  OSStatus err = kNoErr;

  require_action_quiet( ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ), exit, err = kParamErr);

  memset(message, 0x00, sizeof(mico_i2c_message_t));
  message->rx_buffer = rx_buffer;
  message->tx_buffer = tx_buffer;
  message->retries = retries;
  message->tx_length = tx_buffer_length;
  message->rx_length = rx_buffer_length;
  
exit:
  return err;
}

OSStatus platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages )
{
  OSStatus err = kNoErr;
  int      i   = 0;

  platform_mcu_powersave_disable();
  
  require_action_quiet( i2c != NULL, exit, err = kParamErr);
  
  for( i=0; i < number_of_messages; i++ )
  {
    err = i2c_transfer_message_no_dma( i2c, config, &messages[ i ] );
    require_noerr(err, exit);
  }

 exit: 
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
  UNUSED_PARAMETER( config );
  OSStatus err = kNoErr;
  
  platform_mcu_powersave_disable();

  require_action_quiet( i2c != NULL, exit, err = kParamErr);

  err = rtk_i2c_deinit(i2c, config);
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

#if 0
static OSStatus i2c_wait_for_event( I2C_TypeDef* i2c, uint32_t event_id, uint32_t number_of_waits )
{
#ifdef NO_MICO_RTOS
  mico_thread_msleep_no_os(2);
#else
  mico_thread_msleep(2);
#endif
  
  while ( I2C_CheckEvent( i2c, event_id ) != SUCCESS )
  {
    number_of_waits--;
    if ( number_of_waits == 0 )
    {
      return kTimeoutErr;
    }
  }
  return kNoErr;
}
#endif

static OSStatus i2c_address_device( platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries, uint8_t direction )
{
  OSStatus err = kTimeoutErr;
  PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
  PSAL_I2C_HND            pSalI2CHND          = NULL;
  pSalI2CMngtAdpt         = &(i2c->SalI2CMngtAdpt);
  pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);

  /* Some chips( authentication and security related chips ) has to be addressed several times before they acknowledge their address */
  for ( ; retries != 0 ; --retries )
  {
    if ( config->address_width == I2C_ADDRESS_WIDTH_7BIT )
    {
      if (i2c_target_addr[pSalI2CHND->DevNum] != config->address) {
      	/* Deinit I2C directly */
        RtkI2CDeInitForPS(pSalI2CHND);
    
        /* Load the user defined I2C target slave address */
        i2c_target_addr[pSalI2CHND->DevNum] = config->address;
        pSalI2CHND->I2CAckAddr = config->address;
    
        /* Init I2C now */
        if(HAL_OK == RtkI2CInitForPS(pSalI2CHND))
          break;
      }else{
        break;
      }
    }
    /* TODO: Support other address widths */
  }

exit:
    return err;
}

int i2c_write_byte(platform_i2c_t* i2c, int data) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(i2c->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;

    pSalI2CHND->pTXBuf            = &i2ctxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pTXBuf->DataLen   = 1;
    pSalI2CHND->pTXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pTXBuf->RegAddr   = 0;
    pSalI2CHND->pTXBuf->pDataBuf  = (unsigned char*)&data;

    if (RtkI2CSend(pSalI2CHND) != HAL_OK) {
        return 0;
    }
    return 1;
}

int i2c_read_byte(platform_i2c_t* i2c, int last) {
    uint8_t i2cdatlocal;
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(i2c->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    /* Check if the it's the last byte or not */
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    if (!last) {
        pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;
    }

    pSalI2CHND->pRXBuf            = &i2crxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pRXBuf->DataLen   = 1;
    pSalI2CHND->pRXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pRXBuf->RegAddr   = 0;
    pSalI2CHND->pRXBuf->pDataBuf  = &i2cdatlocal;
    RtkI2CReceive(pSalI2CHND);

    return (int)i2cdatlocal;
}

static OSStatus i2c_tx_no_dma( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message )
{
    OSStatus          err = kNoErr;
    int               i;
	
    /* Send data */
    err = i2c_address_device( i2c, config, message->retries, I2C_Direction_Transmitter );
    require_noerr(err, exit);

    for ( i = 0; i < message->tx_length; i++ )
    {
        i2c_write_byte( i2c, ((uint8_t*)message->tx_buffer)[ i ] );

        require_noerr(err, exit);
    }

exit:
    return err;
}

static OSStatus i2c_rx_no_dma( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message )
{
    OSStatus          err = kNoErr;
    int               i;

    err = i2c_address_device( i2c, config, message->retries, I2C_Direction_Receiver );
    require_noerr(err, exit);

    /* Start reading bytes */
    for ( i = 0; i < message->rx_length; i++ )
    {
        /* Check if last byte has been received */
        if ( i == ( message->rx_length - 1 ) )
        {
            /* get data */
            ((uint8_t*)message->rx_buffer)[ i ] = i2c_read_byte( i2c, 1 );        
        }
        else  
        {
            ((uint8_t*)message->rx_buffer)[ i ] = i2c_read_byte( i2c, 0 );        
        }
    }

exit:
    return err;
}




