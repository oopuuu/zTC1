/**
 ******************************************************************************
 * @file    MicoDriverUart.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide UART driver functions.
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
#include "debug.h"

extern void DmaAbort(DMA_CHID_T dmaCh);

/******************************************************
*                    Constants
******************************************************/
/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/
//#define UART_BUFFERMODE 1

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Variables Definitions
******************************************************/
/* UART alternate functions */


/* UART peripheral clock functions */


/* UART peripheral clocks */


/******************************************************
*        Static Function Declarations
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/

uint32_t BaudrateCfg(LPC_USART_T *pUART, uint32_t baud, uint32_t isRtcClkMode)
{
	uint32_t uartClk;
	uint32_t mul = g_pASys->FRGCTRL >> 8;
	uint64_t errRatex256 = 0;

	pUART->CFG = UART_CFG_ENABLE | UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1 |
		0UL<<8 /*NoLIN*/ | 0UL<<9 /*NoFlowCtl*/ | 0UL<<11 /*Async*/ | 0UL<<15 /*NoLoopback*/ | 0UL<<16 /*NoIrDA*/;

	pUART->CTRL = 0UL<<1 /* ~TxBrkEn */ | 0UL<<2 /* ~AddrDetect*/ | 0UL<<6 /* ~TxDis */ | 0UL<<16 /* ~AutoBaud*/;

	if (!isRtcClkMode)
	{
		uartClk = (uint32_t)((uint64_t) Chip_Clock_GetAsyncSyscon_ClockRate() * 256 / (256 + mul));
		pUART->OSR = 0x0F;	// set oversampling as 16
		pUART->BRG = uartClk / 16 / baud;
		errRatex256 = uartClk % (16 * baud);
		errRatex256 = errRatex256 * 256 / uartClk;
	}
	else
	{
		pUART->CFG |= UART_CFG_MODE32K;
		pUART->BRG = 0;
	}
	return (uint32_t) errRatex256;
}

#define UART_RX_FIFO

void _prvInitDataStructure(platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config)
{
	driver->rx_size 			 = 0;
	driver->tx_size 			 = 0;
	driver->last_transmit_result = kNoErr;
	driver->last_receive_result  = kNoErr;
	driver->peripheral			 = (platform_uart_t*)peripheral;

//	#ifndef NO_MICO_RTOS
		mico_rtos_init_semaphore( &driver->tx_complete, 1 );
		mico_rtos_init_semaphore( &driver->rx_complete, 1 );
		mico_rtos_init_semaphore( &driver->sem_wakeup,	1 );
		mico_rtos_init_mutex	( &driver->tx_mutex );
//	#else
//		driver->tx_complete = false;
//		driver->rx_complete = false;
//	#endif

}



void _prvFifoFlush(uint32_t usartIndex)
{
	uint32_t flushMask = (LPC_PERIPFIFO_INT_RXFLUSH | LPC_PERIPFIFO_INT_TXFLUSH);
	g_pFIFO->usart[usartIndex].CTLSET = flushMask;
	g_pFIFO->usart[usartIndex].CTLCLR = flushMask;

}

void _prv_prvFifoCfg(uint32_t usartIndex, uint32_t rxThsld)
{
	LPC_FIFO_USART_T *pThis = g_pFIFO->usart + usartIndex;
	// set FIFO size
	g_pFIFO->common.FIFOCFGUSART[usartIndex] = 16UL<<0 | 0UL<<8;
	// update size, this must be done for all USARTs together
	g_pFIFO->common.FIFOUPDATEUSART = 0xF | 0xF<<16;

	//			 RstToutOnEmpty | ToutBase | ToutVal | RxThsld
	pThis->CFG = 0UL<<5         | 0xF<<8   | 15<<12  | rxThsld<<16;

	// clear IRQ flags
	pThis->STAT = LPC_PERIPFIFO_STATCLR_BUSERR | LPC_PERIPFIFO_STATCLR_RXTIMEOUT;

	// enable IRQ
	pThis->CTLSET = LPC_PERIPFIFO_INT_RXTH | LPC_PERIPFIFO_INT_RXTIMEOUT;
}

void _prvFifoRecfg(uint32_t usartIndex)
{

	// pause RX FIFO
	g_pFIFO->common.FIFOCTLUSART |= 1UL<<0;
	while (!(g_pFIFO->common.FIFOCTLUSART & 1UL<<1)) {}

	_prvFifoFlush(usartIndex);

	_prv_prvFifoCfg(usartIndex, 7);

	// unpause RXFIFO
	g_pFIFO->common.FIFOCTLUSART &= ~(1UL<<0);
	while ((g_pFIFO->common.FIFOCTLUSART & 1UL<<1)) {}
}

// return : overrun count
uint32_t _prvReadFifo(uint32_t usartIndex, uint8_t *pBuf, uint8_t *pBufEnd, uint32_t *pCopiedCnt)
{
	LPC_FIFO_USART_T *pThis = g_pFIFO->usart + usartIndex;
	__IO uint32_t *pRxDat = &pThis->RXDAT;
	uint32_t ovrnCnt = 0, cbToCopy;

	uint32_t cbPnd = (pThis->STAT >> 16) & 0xFF;
	uint32_t bufCap;

	bufCap = (uint32_t)pBufEnd - (uint32_t)pBuf;

	if (cbPnd < bufCap)
		cbToCopy = cbPnd , ovrnCnt = 0;
	else
		cbToCopy = bufCap , ovrnCnt = cbPnd - bufCap;

	if (pCopiedCnt)
		*pCopiedCnt = cbToCopy;

	while(cbToCopy--)
	{
		if (pBuf != pBufEnd)
			*pBuf++ = (uint8_t) *pRxDat;
		else
			*pRxDat , ovrnCnt++;
	}

	pThis->STAT = LPC_PERIPFIFO_STATCLR_BUSERR | LPC_PERIPFIFO_STATCLR_RXTIMEOUT;

	return ovrnCnt;
}


uint32_t _prvReadFifoWithEnoughBuf(uint32_t usartIndex, uint8_t *pBuf)
{
	LPC_FIFO_USART_T *pThis = g_pFIFO->usart + usartIndex;
	__IO uint32_t *pRxDat = &pThis->RXDAT;
	uint32_t cbPnd = (pThis->STAT >> 16) & 0xFF;
	uint32_t ret = cbPnd;
	while(cbPnd--)
	{
		*pBuf++ = (uint8_t) *pRxDat;
	}

	pThis->STAT = LPC_PERIPFIFO_STATCLR_BUSERR | LPC_PERIPFIFO_STATCLR_RXTIMEOUT;

	return ret;
}

extern platform_uart_driver_t platform_uart_drivers[];

void OnUartIRQ(LPC_USART_T *p)
{
	uint32_t stat = p->STAT, fifoStat = LPC_PERIPFIFO_STAT_RXEMPTY;
	uint32_t usartIndex = 0, cbRx = 0, cbCopied = 0;
	uint32_t isRxDone;
	ring_buffer_t *pRxRing;
	uint8_t fifoBuf[16];
	platform_uart_driver_t *pDrv = NULL;
	LPC_FIFO_USART_T *pThisFifo = 0;
	if (p == LPC_USART0)
		pDrv = platform_uart_drivers + 0 , usartIndex = 0;
        else if (p == LPC_USART1)
		pDrv = platform_uart_drivers + 1 , usartIndex = 1;
        else if (p == LPC_USART2)
		pDrv = platform_uart_drivers + 1 , usartIndex = 2;
	else if (p == LPC_USART3)
		pDrv = platform_uart_drivers + 1 , usartIndex = 3;

	pRxRing = pDrv->pRxRing;
	isRxDone = 0;

	if (pDrv->peripheral->isRxFifoEn)
	{
		pThisFifo = &g_pFIFO->usart[usartIndex];
		fifoStat = pThisFifo->STAT;
	}

	if (stat & 1UL<<0 || (!(fifoStat & LPC_PERIPFIFO_STAT_RXEMPTY)))
	{
		// some data arrived, receive
		if (pRxRing != 0)
		{
			// RX data ready
			if (pDrv->peripheral->isRxFifoEn)
				cbRx = _prvReadFifoWithEnoughBuf(usartIndex, fifoBuf);
			else {
				fifoBuf[0] = (uint8_t) p->RXDATA;
				cbRx = 1;
			}
			cbCopied = ring_buffer_write(pRxRing, fifoBuf, cbRx);
			if (cbCopied < cbRx)
				pDrv->last_receive_result = kOverrunErr;

			if (ring_buffer_used_space(pRxRing) >= pDrv->bufFillThsld) {
				isRxDone = 1;
				pDrv->bufFillThsld = 0;	// auto-clear so we won't set more than one time
			}
		}
		else
		{
			// no ring buffer, use the previous linear buffer given from caller to receive
			// USART IRQ is disabled if there is no ring buffer
			uint32_t ovrnCnt;
			ovrnCnt = _prvReadFifo(usartIndex, pDrv->pBuf, pDrv->pBufEnd, &cbCopied);
			pDrv->pBuf += cbCopied;
			if (ovrnCnt != 0)
			{
				// overrun happen
				pDrv->last_receive_result = kOverrunErr;
			} else {
				pDrv->last_receive_result = kNoErr;
			}

			if (pDrv->pBuf == pDrv->pBufEnd)
				isRxDone = 1;
		}

		if (isRxDone)
		{

//			#ifndef NO_MICO_RTOS
				mico_rtos_set_semaphore(&pDrv->rx_complete);
//			#else
//				pDrv->rx_complete = 1;
//			#endif
		}

	}


	if (stat & (UART_STAT_OVERRUNINT | UART_STAT_FRM_ERRINT | UART_STAT_RXNOISEINT))
	{
		// optional todo: handle USART communication error
	}

	p->STAT = UART_STAT_RXRDY | UART_STAT_OVERRUNINT | UART_STAT_FRM_ERRINT | UART_STAT_RXNOISEINT;
	if (pThisFifo)
		pThisFifo->STAT = LPC_PERIPFIFO_STATCLR_BUSERR | LPC_PERIPFIFO_STATCLR_RXTIMEOUT;

}

uint8_t s_isFifoInited;
void _prvFifoInit(uint32_t usartIndex)
{

	// enable FIFO RX
	g_pSys->FIFOCTRL |= 1UL<<(8 + usartIndex);

	if (0 == s_isFifoInited) {
		Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_FIFO);
		Chip_SYSCON_PeriphReset(RESET_FIFO);
	}
	s_isFifoInited = 1;
}

OSStatus _prvInitUartHw(platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config)
{
	uint32_t config_value = 0;
	uint32_t usartIndex;
    IRQn_Type irqNdx;
    uint32_t pinMux;
	OSStatus err = kNoErr;

	usartIndex = peripheral->hwNdx;
    irqNdx = (IRQn_Type)peripheral->irqNdx;

	NVIC_DisableIRQ(irqNdx);

	if (peripheral->isRxFifoEn)
	{
		_prvFifoInit(usartIndex);
		_prvFifoRecfg(usartIndex);
	}
	// >>> prepare FIFO for USART
	// <<<


	if (peripheral->port == LPC_USART0 || peripheral->port == LPC_USART1)
		pinMux = 1;
	else
		pinMux = 2;
	Chip_IOCON_PinMuxSet(LPC_IOCON, peripheral->pin_tx->port, peripheral->pin_tx->pin_number, (pinMux | IOCON_MODE_INACT | IOCON_DIGITAL_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, peripheral->pin_rx->port, peripheral->pin_rx->pin_number, (pinMux | IOCON_MODE_INACT | IOCON_DIGITAL_EN));


	if(config->data_width == DATA_WIDTH_9BIT) {
		config_value |= UART_CFG_DATALEN_9;
	}
	else if(config->data_width == DATA_WIDTH_8BIT) {
		config_value |= UART_CFG_DATALEN_8;
	}
	else if(config->data_width == DATA_WIDTH_7BIT) {
		config_value |= UART_CFG_DATALEN_7;
	}
	else {
		err = kParamErr;
		goto exit;
	}

	if(config->stop_bits == STOP_BITS_1) {
		config_value |= UART_CFG_STOPLEN_1;
	}
	else {
		config_value |= UART_CFG_STOPLEN_2;
	}

	switch ( config->parity )
	{
	case NO_PARITY:
		//      uart_init_structure.USART_Parity = USART_Parity_No;
		config_value |= UART_CFG_PARITY_NONE;
		break;
	case EVEN_PARITY:
		//      uart_init_structure.USART_Parity = USART_Parity_Even;
		config_value |= UART_CFG_PARITY_EVEN;
		break;
	case ODD_PARITY:
		//      uart_init_structure.USART_Parity = USART_Parity_Odd;
		config_value |= UART_CFG_PARITY_ODD;
		break;
	default:
		err = kParamErr;
		goto exit;
	}

	// Magicoe TODO
	switch ( config->flow_control )
	{
	case FLOW_CONTROL_DISABLED:
		//      uart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		break;
	case FLOW_CONTROL_CTS:
		//      uart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;
		config_value |= UART_CFG_CTSEN;
		break;
	case FLOW_CONTROL_RTS:
		//      uart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS;
		break;
	case FLOW_CONTROL_CTS_RTS:
		//      uart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
		break;
	default:
		err = kParamErr;
		goto exit;
	}

	// enable clock to fraction baudrage generator, shared by all USARTs
	g_pASys->ASYNCAPBCLKCTRLSET = 1UL<<15;

	/* Initialise USART peripheral */
	Chip_UART_DeInit(peripheral->port);
	Chip_UART_Init(peripheral->port);

	if ( Chip_Clock_GetMainClockSource() == SYSCON_MAINCLKSRC_RTC ) {
		BaudrateCfg(peripheral->port, config->baud_rate, 1);
	}
	else  {
		BaudrateCfg(peripheral->port, config->baud_rate, 0);
	}

	Chip_UART_ClearStatus(peripheral->port, UART_STAT_OVERRUNINT|UART_STAT_DELTARXBRK|UART_STAT_FRM_ERRINT|UART_STAT_PAR_ERRINT|UART_STAT_RXNOISEINT);

	Chip_UART_IntEnable(peripheral->port, UART_INTEN_RXRDY | UART_INTEN_OVERRUN | UART_INTEN_FRAMERR);
	Chip_UART_IntDisable(peripheral->port, UART_INTEN_TXRDY);	/* May not be needed */

	if (driver->pRxRing)
		NVIC_EnableIRQ(irqNdx);


exit:
	return err;
}

uint32_t platform_uart_get_length_in_buffer( platform_uart_driver_t* driver )
{
	return ring_buffer_used_space( driver->pRxRing );
}


OSStatus platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer )
{
	OSStatus          err = kNoErr;

	platform_mcu_powersave_disable();

	require_action_quiet( ( driver != NULL ) && ( peripheral != NULL ) && ( config != NULL ), exit, err = kParamErr);
	require_action_quiet( (optional_ring_buffer == NULL) || ((optional_ring_buffer->buffer != NULL ) && (optional_ring_buffer->size != 0)), exit, err = kParamErr);

	_prvInitDataStructure(driver, peripheral, config);

	/* Setup ring buffer */
	if ( optional_ring_buffer != NULL )
	{
		/* Note that the ring_buffer should've been initialised first */
		driver->pRxRing = optional_ring_buffer;
	}

	err = _prvInitUartHw(driver, peripheral, config);

	if (err != kNoErr)
		goto exit;


exit:
	MicoMcuPowerSaveConfig(true);
	return err;
}

OSStatus platform_uart_deinit( platform_uart_driver_t* driver )
{
	OSStatus          err = kNoErr;
	LPC_USART_T *pUSART;
	uint32_t bvUSART = 0;

	platform_mcu_powersave_disable();

	require_action_quiet( ( driver != NULL ), exit, err = kParamErr);

	// >>> put USART in reset mode
	pUSART = driver->peripheral->port;
	if (pUSART == LPC_USART0) {
		bvUSART = 1UL<<1;
	} else if (pUSART == LPC_USART1) {
		bvUSART = 1UL<<2;
	} else if (pUSART == LPC_USART2) {
		bvUSART = 1UL<<3;
	} else if (pUSART == LPC_USART3) {
		bvUSART = 1UL<<4;
	}

	g_pASys->ASYNCAPBCLKCTRLCLR = bvUSART;
	g_pASys->ASYNCPRESETCTRLSET = bvUSART;
	// <<<

	// >>> release s/w resources
//	#ifndef NO_MICO_RTOS
	  mico_rtos_deinit_semaphore( &driver->rx_complete );
	  mico_rtos_deinit_semaphore( &driver->tx_complete );
	  mico_rtos_deinit_mutex( &driver->tx_mutex );
//	#else
//	  driver->rx_complete = false;
//	  driver->tx_complete = false;
//	#endif
	  driver->rx_size              = 0;
	  driver->tx_size              = 0;
	  driver->last_transmit_result = kNoErr;
	  driver->last_receive_result  = kNoErr;
	// <<<

exit:
	platform_mcu_powersave_enable();
	return err;
}

OSStatus platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
	OSStatus err = kNoErr;

	platform_mcu_powersave_disable();

#ifndef NO_MICO_RTOS
	mico_rtos_lock_mutex( &driver->tx_mutex );
#endif

	require_action_quiet( ( driver != NULL ) && ( data_out != NULL ) && ( size != 0 ), exit, err = kParamErr);

	/* Init DMA parameters and variables */
	driver->last_transmit_result                    = kGeneralErr;
	driver->tx_size                                 = size;

	if (driver->peripheral->hwNdx == 0) {
		Chip_UART_ClearStatus(LPC_USART0, UART_STAT_OVERRUNINT|UART_STAT_DELTARXBRK|UART_STAT_FRM_ERRINT|UART_STAT_PAR_ERRINT|UART_STAT_RXNOISEINT);
		Chip_DMA_InitChannel( DMAREQ_UART0_TX, DMA_ADDR(data_out), DMA_XFERCFG_SRCINC_1,
			DMA_ADDR(&LPC_USART0->TXDATA), DMA_XFERCFG_DSTINC_0, WIDTH_8_BITS,
			size, (DMA_CFG_PERIPHREQEN | DMA_CFG_BURSTPOWER_1 | DMA_CFG_CHPRIORITY(3)));
		Chip_DMA_EnableIntChannel( LPC_DMA, DMAREQ_UART0_TX );
		Chip_DMA_StartTransfer(DMAREQ_UART0_TX, DMA_XFERCFG_SRCINC_1, DMA_XFERCFG_DSTINC_0, DMA_XFERCFG_WIDTH_8, size);
	}
	else if (driver->peripheral->hwNdx == 1)
	{
		Chip_UART_ClearStatus(LPC_USART1, UART_STAT_OVERRUNINT|UART_STAT_DELTARXBRK|UART_STAT_FRM_ERRINT|UART_STAT_PAR_ERRINT|UART_STAT_RXNOISEINT);
		Chip_DMA_InitChannel( DMAREQ_UART1_TX, DMA_ADDR(data_out), DMA_XFERCFG_SRCINC_1,
			DMA_ADDR(&LPC_USART1->TXDATA), DMA_XFERCFG_DSTINC_0, WIDTH_8_BITS,
						size, (DMA_CFG_PERIPHREQEN | DMA_CFG_BURSTPOWER_1 | DMA_CFG_CHPRIORITY(3)));
		Chip_DMA_StartTransfer(DMAREQ_UART1_TX, DMA_XFERCFG_SRCINC_1, DMA_XFERCFG_DSTINC_0, DMA_XFERCFG_WIDTH_8, size);
	}
	else if (driver->peripheral->hwNdx == 2)
	{
		Chip_UART_ClearStatus(LPC_USART2, UART_STAT_OVERRUNINT|UART_STAT_DELTARXBRK|UART_STAT_FRM_ERRINT|UART_STAT_PAR_ERRINT|UART_STAT_RXNOISEINT);
		Chip_DMA_InitChannel( DMAREQ_UART2_TX, DMA_ADDR(data_out), DMA_XFERCFG_SRCINC_1,
			DMA_ADDR(&LPC_USART2->TXDATA), DMA_XFERCFG_DSTINC_0, WIDTH_8_BITS,
			size, (DMA_CFG_PERIPHREQEN | DMA_CFG_BURSTPOWER_1 | DMA_CFG_CHPRIORITY(3)));
		Chip_DMA_StartTransfer(DMAREQ_UART2_TX, DMA_XFERCFG_SRCINC_1, DMA_XFERCFG_DSTINC_0, DMA_XFERCFG_WIDTH_8, size);
	}
	else if (driver->peripheral->hwNdx == 3)
	{
		Chip_UART_ClearStatus(LPC_USART3, UART_STAT_OVERRUNINT|UART_STAT_DELTARXBRK|UART_STAT_FRM_ERRINT|UART_STAT_PAR_ERRINT|UART_STAT_RXNOISEINT);
		Chip_DMA_InitChannel( DMAREQ_UART3_TX, DMA_ADDR(data_out), DMA_XFERCFG_SRCINC_1,
			DMA_ADDR(&LPC_USART3->TXDATA), DMA_XFERCFG_DSTINC_0, WIDTH_8_BITS,
			size, (DMA_CFG_PERIPHREQEN | DMA_CFG_BURSTPOWER_1 | DMA_CFG_CHPRIORITY(3)));
		Chip_DMA_StartTransfer(DMAREQ_UART3_TX, DMA_XFERCFG_SRCINC_1, DMA_XFERCFG_DSTINC_0, DMA_XFERCFG_WIDTH_8, size);
	}
	/* Wait for transmission complete */
//#ifndef NO_MICO_RTOS
	mico_rtos_get_semaphore( &driver->tx_complete, MICO_NEVER_TIMEOUT );
//#else
//	while( driver->tx_complete == false )
//		__WFI();
//	driver->tx_complete = false;
//#endif

	driver->tx_size = 0;
	err = driver->last_transmit_result;

exit:
//#ifndef NO_MICO_RTOS
	mico_rtos_unlock_mutex( &driver->tx_mutex );
//#endif
	platform_mcu_powersave_enable();
	return err;
}

void _prvRingBufReadAndConsume(ring_buffer_t *pRing, uint8_t *pDataIn, uint32_t transfer_size)
{
	// this function assumes transfer_size <= buffer filled count
	do
	{
		uint8_t* available_data;
		uint32_t bytes_available;

		ring_buffer_get_data(pRing, &available_data, &bytes_available );
		bytes_available = MIN( bytes_available, transfer_size );

		memcpy( pDataIn, available_data, bytes_available );
		transfer_size -= bytes_available;

		pDataIn += bytes_available;

		disable_interrupts();
		ring_buffer_consume(pRing, bytes_available );
		enable_interrupts();

	} while ( transfer_size != 0 );

}

// reserve some margin for incoming data while processing previous receive complete
#define BUFFER_MARGIN	32

OSStatus platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* pRxBuf, uint32_t c1ToRx, uint32_t timeout_ms )
{
	OSStatus err = kNoErr;
	ring_buffer_t *pRing = driver->pRxRing;
	//platform_mcu_powersave_disable();

	require_action_quiet( ( driver != NULL ) && ( pRxBuf != NULL ) && ( c1ToRx != 0 ), exit, err = kParamErr);
	require_action_quiet( (c1ToRx <= driver->pRxRing->size - BUFFER_MARGIN), exit, err = kSizeErr);

	if (pRing != NULL)
	{
		uint32_t bufFillCnt;

		disable_interrupts();
		bufFillCnt = ring_buffer_used_space(pRing);
		if (bufFillCnt >= c1ToRx)
		{
			enable_interrupts();
			_prvRingBufReadAndConsume(pRing, pRxBuf, c1ToRx);
			err = kNoErr;
		}
		else
		{
			// buffer contains less bytes than wanted, drain the buffer at first
			driver->bufFillThsld = c1ToRx;
			enable_interrupts();

//			#ifndef NO_MICO_RTOS
				err = mico_rtos_get_semaphore(&driver->rx_complete, timeout_ms);
//			#else
//				driver->rx_complete = false;
//                                int delay_start = mico_get_time_no_os();
//                                while(driver->rx_complete == false){
//                                  if(mico_get_time_no_os() >= delay_start + timeout_ms && timeout_ms != MICO_NEVER_TIMEOUT){
//                                    driver->rx_size = 0;
//                                    err = kTimeoutErr;
//                                    goto exit;
//                                  }
//                                }
//                                driver->rx_size = 0;
//			#endif

			// we assume asked size is <= ring buffer size
			if (err == kNoErr) {
				_prvRingBufReadAndConsume(pRing, pRxBuf, c1ToRx);
			}else{
				// we return no data if there are error

			}

		}

	}
	else
	{
		IRQn_Type irqNdx = (IRQn_Type) driver->peripheral->irqNdx;
		NVIC_DisableIRQ(irqNdx);
		driver->pBuf = pRxBuf;
		driver->pBufEnd = pRxBuf + c1ToRx;
		NVIC_EnableIRQ(irqNdx);

//		#ifndef NO_MICO_RTOS
			err = mico_rtos_get_semaphore(driver->rx_complete, timeout_ms);
//		#else
//				while (!driver->rx_complete)
//					__WFI();
//		#endif

		NVIC_DisableIRQ(irqNdx);
		driver->pBuf = driver->pBufEnd = 0;

	}
exit:
	//platform_mcu_powersave_enable();
	return err;
}

/******************************************************
*            Interrupt Service Routines
******************************************************/

void platform_uart_tx_dma_irq( platform_uart_driver_t* driver )
{
	// bool tx_complete = false;
	driver->last_transmit_result = kNoErr;

	if ( driver->tx_size > 0 ) {
//#ifndef NO_MICO_RTOS
		/* Set semaphore regardless of result to prevent waiting thread from locking up */
		mico_rtos_set_semaphore( &driver->tx_complete);
//#else
//		driver->tx_complete = true;
//#endif
	}
}

// end file --- MG.Niu ---
