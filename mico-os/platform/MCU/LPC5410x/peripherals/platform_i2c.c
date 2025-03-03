/**
 ******************************************************************************
 * @file    MicoDriverI2C.c
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


#include "mico_platform.h"
#include "mico_rtos.h"

#include "platform.h"
#include "platform_peripheral.h"
#include "chip.h"
#include "platform_logging.h"

/******************************************************
*                    Constants
******************************************************/

#define I2C_FLAG_CHECK_TIMEOUT      ( 1000 )
#define I2C_FLAG_CHECK_LONG_TIMEOUT ( 1000 )

#define  I2C_Direction_Transmitter      ((uint8_t)0x00) // Wecan Modify 2015.06.05
#define  I2C_Direction_Receiver         ((uint8_t)0x01) // Wecan Modify 2015.06.05

//#define I2C_USE_DMA

#define DMA_FLAG_TC(stream_id) dma_flag_tc(stream_id)
#define DMA_TIMEOUT_LOOPS      (10000000)

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

platform_i2c_cb_t s_i2cCtxs[MICO_I2C_MAX];


/******************************************************
*               Function Declarations
******************************************************/
//// Wecan ToDo 2015.06.05
//static OSStatus i2c_address_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries, uint8_t direction );
//static OSStatus i2c_wait_for_event( LPC_I2C_T *pI2C,  uint32_t number_of_waits );
//////////////////////////////////////////////////////////////////////////////////////////////


void _prvI2CHwCfg(const platform_i2c_t *i2c, const platform_i2c_config_t *config)
{
	uint32_t pinSCL = 0, pinSDA = 0, pinCfg = 0, busHz = 0;
	LPC_I2C_T *pI2C = i2c->port;
	if (config->speed_mode == I2C_LOW_SPEED_MODE)
		busHz = 10 * 1000;
	else if (config->speed_mode == I2C_STANDARD_SPEED_MODE)
		busHz = 100 * 1000;
	else if (config->speed_mode == I2C_HIGH_SPEED_MODE)
		busHz = 400 * 1000;
	//else if (config->speed_mode == I2C_HISPD_PLUS_MODE)
	//	busHz = 1000 * 1000;

	pinCfg = IOCON_FUNC1 | 1UL<<7 | 1UL<<8;
	if (busHz > 400 * 1000)
		pinCfg |= 1UL<<9 | 1UL<<10;	// High driver, no filter

	if (pI2C == LPC_I2C0)
		pinSCL = 23 , pinSDA = 24;
	else if (pI2C == LPC_I2C1)
		pinSCL = 25 , pinSDA = 26;
	else if (pI2C == LPC_I2C2)
		pinSCL = 27 , pinSDA = 28;

	g_pIO->PIO[0][pinSCL] = pinCfg;
	g_pIO->PIO[0][pinSDA] = pinCfg;
	//////////////////////////////////////////////////////////////////////////////////////////////////


    /* Setup clock rate for I2C */
    {
        uint32_t t1;
        // 12 times of wanted I2C bus clock
        t1 = Chip_Clock_GetAsyncSyscon_ClockRate();
        t1 /= 12 * busHz;
        if (0 == t1)
        	t1 = 1;
        if (t1 > 65536)
        	t1 = 65536;
		pI2C->CLKDIV = t1 - 1;
        pI2C->MSTTIME = (6-2) | ((6-2) << 4);
    }

}

/******************************************************
*               Function Definitions
******************************************************/

OSStatus platform_i2c_init(const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
	OSStatus err = kNoErr;
	LPC_I2C_T *pI2C;
	#ifndef BOOTLOADER
	platform_i2c_cb_t *pCtx = i2c->pCtx;
#endif
	// platform_mcu_powersave_disable( );
	require_action_quiet( (i2c != NULL && i2c->hwNdx < MICO_I2C_MAX && config != NULL && config->address_width == I2C_ADDRESS_WIDTH_7BIT), exit, err = kParamErr);

	pI2C = i2c->port;

	#ifndef BOOTLOADER
    if (NULL == pCtx->semXfer)
        mico_rtos_init_semaphore(&pCtx->semXfer, 1);
	if (NULL == pCtx->mtxXfer)
        mico_rtos_init_mutex(&pCtx->mtxXfer);
	#endif

	// Init & Reset I2C clocks Wecan modify 2015.06.03
	Chip_I2C_Init(i2c->port);//(LPC_I2C_PORT);

	_prvI2CHwCfg(i2c, config);

    Chip_I2CM_Enable(pI2C);
    NVIC_EnableIRQ((IRQn_Type)i2c->irqNdx);

exit:
	// platform_mcu_powersave_enable( );
	return err;
}


OSStatus platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries )
{
	OSStatus err = kNoErr;

	require_action_quiet( ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ), exit, err = kParamErr);

	memset(message, 0x00, sizeof(mico_i2c_message_t));
	message->rx_buffer = 0;
	message->tx_buffer = tx_buffer;
	message->retries = retries;
	message->tx_length= tx_buffer_length;
	message->rx_length= 0;
	message->combined = 0;


exit:
	return err;
}

OSStatus platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries )
{
	OSStatus err = kNoErr;

	require_action_quiet( ( message != NULL ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ), exit, err = kParamErr);

	memset(message, 0x00, sizeof(mico_i2c_message_t));
	message->rx_buffer = rx_buffer;
	message->tx_buffer = 0;
	message->retries = retries;
	message->tx_length= 0;
	message->rx_length= rx_buffer_length;
	message->combined = 0;



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
	message->tx_length= tx_buffer_length;
	message->rx_length= rx_buffer_length;
	message->combined = 1;

exit:
	return err;
}


/* Master transfer state change handler handler */
uint32_t _prvI2CMXferFSM(LPC_I2C_T *pI2C, platform_i2c_cb_t *xfer)
{
	uint32_t status = pI2C->STAT;

	if (status & I2C_STAT_MSTRARBLOSS) {
		/* Master Lost Arbitration */
		/* Set transfer status as Arbitration Lost */
		xfer->status = I2CM_STATUS_ARBLOST;
		/* Clear Status Flags */
		pI2C->STAT = I2C_STAT_MSTRARBLOSS;
		/* Master continue */
		if (status & I2C_STAT_MSTPENDING) {
			pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
		}
	}
	else if (status & I2C_STAT_MSTSTSTPERR) {
		/* Master Start Stop Error */
		/* Set transfer status as Bus Error */
		xfer->status = I2CM_STATUS_BUS_ERROR;
		/* Clear Status Flags */
		pI2C->STAT = I2C_STAT_MSTSTSTPERR;

		/* Master continue */
		if (status & I2C_STAT_MSTPENDING) {
			pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
		}
	}
	else if (status & I2C_STAT_MSTPENDING) {
		/* Master is Pending */
		/* Branch based on Master State Code */
		switch ((pI2C->STAT & I2C_STAT_MSTSTATE) >> 1) {
		case I2C_STAT_MSTCODE_IDLE: /* Master idle */
			/* Can transition to idle between transmit and receive states */
			if (xfer->txSz || xfer->isProbe) {
				/* Start transmit state */
				pI2C->MSTDAT = xfer->slaveAddr << 1;
				pI2C->MSTCTL = I2C_MSTCTL_MSTSTART;
			}
			else if (xfer->rxSz) {
				/* Start receive state with start ot repeat start */
				pI2C->MSTDAT = (xfer->slaveAddr << 1) | 0x1;
				pI2C->MSTCTL = I2C_MSTCTL_MSTSTART;
			}
			else {
				/* No data to send, done */
				xfer->status = I2CM_STATUS_OK;
			}
			break;

		case I2C_STAT_MSTCODE_RXREADY:  /* Receive data is available */
			/* Read Data up until the buffer size */
			if (xfer->rxSz) {
				*xfer->rxBuff = pI2C->MSTDAT;
				xfer->rxBuff++;
				xfer->rxSz--;
			}

			if (xfer->rxSz) {
				pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
			}
			else {
				/* Last byte to receive, send stop after byte received */
				pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE | I2C_MSTCTL_MSTSTOP;
			}
			break;

		case I2C_STAT_MSTCODE_TXREADY:  /* Master Transmit available */
			if (xfer->txSz) {
				/* If Tx data available transmit data and continue */
				pI2C->MSTDAT = (uint32_t) *xfer->txBuff;
				pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
				xfer->txBuff++;
				xfer->txSz--;
			}
			else if (xfer->rxSz == 0) {
				pI2C->MSTCTL = I2C_MSTCTL_MSTSTOP;
			}
			else {
				/* Start receive state with start ot repeat start */
				pI2C->MSTDAT = (xfer->slaveAddr << 1) | 0x1;

#if 0 //#ifdef I2CM_DMA_SUPPORT
				if (xfer->rxSz >= I2C_SENSOR_BUS_DMABYTELIM)
				{
#ifdef I2CM_DMA_RXALL
					setupI2CDMAXfer((void *) xfer->rxBuff, xfer->rxSz, 0 /*isTx*/);
#else
					setupI2CDMAXfer((void *) xfer->rxBuff, xfer->rxSz - 1, 0 /*isTx*/);
#endif
					LPC_I2C_PORT->INTENCLR = I2C_INTENSET_MSTPENDING;
					// g_pDMA->DMACOMMON[0].SETVALID = (1 << I2C_SENSOR_BUS_DMAID);
					pI2C->MSTCTL = I2C_MSTCTL_MSTSTART | I2C_MSTCTL_MSTDMA;
				}
				else
				{
					pI2C->MSTCTL = I2C_MSTCTL_MSTSTART;
				}
#else
				pI2C->MSTCTL = I2C_MSTCTL_MSTSTART;
#endif
			}
			break;

		case I2C_STAT_MSTCODE_NACKADR:  /* Slave address was NACK'ed */
			/* Set transfer status as NACK on address */
			xfer->status = I2CM_STATUS_NAK_ADR;
			pI2C->MSTCTL = I2C_MSTCTL_MSTSTOP;
			break;

		case I2C_STAT_MSTCODE_NACKDAT:  /* Slave data was NACK'ed */
			/* Set transfer status as NACK on data */
			xfer->status = I2CM_STATUS_NAK_DAT;
			pI2C->MSTCTL = I2C_MSTCTL_MSTSTOP;
			break;

		default:
			/* Illegal I2C master state machine case. This should never happen.
			Try to advance state machine by continuing. */
			xfer->status = I2CM_STATUS_ERROR;
			pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
			break;
		}
	}
	else {
		/* Unsupported operation. This may be a call to the master handler
		for a wrong interrupt type. This handler should only be called when a
		master arbitration loss, master start/stop error, or master pending status
		occurs. */
		xfer->status = I2CM_STATUS_ERROR;
	}

	#ifndef BOOTLOADER
	if (xfer->status != I2CM_STATUS_BUSY)
	{
		platform_i2c_t *pDev = (platform_i2c_t*) xfer->pDev;
        pDev->pCtx->isProbe = 0;
		mico_rtos_set_semaphore(&pDev->pCtx->semXfer);
	}
	#endif

	return xfer->status != I2CM_STATUS_BUSY;
}


void MyI2CM_Xfer(const platform_i2c_t *pDev, platform_i2c_message_t *pMsg)
{
	LPC_I2C_T *pI2C = pDev->port;
	platform_i2c_cb_t *pCtx = pDev->pCtx;
	/* set the transfer status as busy */

	pCtx->rxBuff = pMsg->rx_buffer;
	pCtx->txBuff = pMsg->tx_buffer;
	pCtx->rxSz = pMsg->rx_length;
	pCtx->txSz = pMsg->tx_length;
	pCtx->retries = pMsg->retries;

	pCtx->status = I2CM_STATUS_BUSY;

	/* Reset master state machine */
	Chip_I2CM_Disable(pI2C);
	Chip_I2CM_Enable(pI2C);

	/* Clear controller state. */
	Chip_I2CM_ClearStatus(pI2C, I2C_STAT_MSTRARBLOSS | I2C_STAT_MSTSTSTPERR);

	/* Handle transfer via initial call to handler */
	_prvI2CMXferFSM(pI2C, pCtx);
}

OSStatus WaitForI2cXferComplete(const platform_i2c_t* i2c, platform_i2c_message_t *xferRecPtr)
{
	OSStatus err = kNoErr;

	#ifndef BOOTLOADER
		//do {
			err = mico_rtos_get_semaphore(&i2c->pCtx->semXfer, 300);
		//} while (err != kNoErr);

	#else
	    int i2c_timeout = 3000;

	    while (i2c->pCtx->status == I2CM_STATUS_BUSY && i2c_timeout-- > 0) {
	        /* Sleep until next interrupt */
	        __WFI();
	    }
	    if (0 == i2c_timeout)
	    	err = kTimeoutErr;
	#endif

	if (err != kNoErr)
		err = kTimeoutErr;
	else
	{
		if (i2c->pCtx->status != I2CM_STATUS_OK)
			err = kResponseErr;
	}

    return err;
}

OSStatus platform_i2c_transfer(const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages )
{
	OSStatus err = kNoErr;
	int      i   = 0, retryCnt;
	platform_i2c_cb_t *pCtx = i2c->pCtx;
	// platform_mcu_powersave_disable();

	require_action_quiet( i2c != NULL, exit, err = kParamErr);

	#ifndef BOOTLOADER
		mico_rtos_lock_mutex(&pCtx->mtxXfer);
	#endif

    pCtx->slaveAddr = config->address;
    pCtx->pDev = i2c;

	for( i=0; i < number_of_messages; i++ )
	{
		_prvI2CHwCfg(i2c, config);
		for (retryCnt = 0; retryCnt < messages[i].retries; retryCnt++)
		{
			MyI2CM_Xfer(i2c, messages + i);
            /* Enable Master Interrupts */
            Chip_I2C_EnableInt(i2c->port, I2C_INTENSET_MSTPENDING | I2C_INTENSET_MSTRARBLOSS | I2C_INTENSET_MSTSTSTPERR);
			/* Wait for transfer completion */
			err = WaitForI2cXferComplete(i2c, messages + i);

			if (err == kNoErr)
				break;
		}
		if (err != kNoErr)
			break;
	}
exit:
	#ifndef BOOTLOADER
		mico_rtos_unlock_mutex(&pCtx->mtxXfer);
	#endif

	// platform_mcu_powersave_enable();
	return err;
}

bool platform_i2c_probe_device(const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries )
{
	OSStatus err = kNoErr;
	platform_i2c_message_t msg;
	memset(&msg, 0, sizeof(msg));
    msg.retries = retries;
	i2c->pCtx->isProbe = 1;
	err = platform_i2c_transfer(i2c, config, &msg, 1);
    return err;
}


OSStatus platform_i2c_deinit(const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
	UNUSED_PARAMETER( config );
	OSStatus err = kNoErr;
#ifndef BOOTLOADER
	platform_i2c_cb_t *pCtx = i2c->pCtx;
#endif


	require_action_quiet( i2c != NULL, exit, err = kParamErr);

	// Wecan Todo 2015.06.04
	Chip_I2C_DeInit( i2c->port );         //Disable I2C peripheral clockss
#ifndef BOOTLOADER
    if (NULL != pCtx->semXfer)
        mico_rtos_deinit_semaphore(&pCtx->semXfer);
	if (NULL != pCtx->mtxXfer)
        mico_rtos_deinit_mutex(&pCtx->mtxXfer);
    pCtx->semXfer = NULL;
    pCtx->mtxXfer = NULL;
#endif

exit:
	return err;
}


void OnNonBlockI2CMDone(LPC_I2C_T *pI2C)
{
	Chip_I2C_DisableInt(pI2C, I2C_INTENSET_MSTPENDING | I2C_INTENSET_MSTRARBLOSS | I2C_INTENSET_MSTSTSTPERR);
}



void I2CMIrqHandler(platform_i2c_t *pDev)
{
	LPC_I2C_T *pI2C = pDev->port;

	uint32_t state = Chip_I2C_GetPendingInt(pI2C);
	/* Error handling */
	if (state & (I2C_INTENSET_MSTRARBLOSS | I2C_INTENSET_MSTSTSTPERR)) {
		Chip_I2CM_ClearStatus(pI2C, I2C_STAT_MSTRARBLOSS | I2C_STAT_MSTSTSTPERR);
	}

	/* Call I2CM ISR function with the I2C device and transfer rec */
	if (state & I2C_INTENSET_MSTPENDING) {
		_prvI2CMXferFSM(pI2C, pDev->pCtx);
		if (pDev->pCtx->status != I2CM_STATUS_BUSY)
			OnNonBlockI2CMDone(pI2C);
	}
}

