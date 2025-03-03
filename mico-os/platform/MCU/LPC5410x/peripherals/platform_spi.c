/**
 ******************************************************************************
 * @file    MicoDriverSpi.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide SPI driver functions.
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
#include "platform_config.h"
#include "platform_peripheral.h"
#include "debug.h"

/******************************************************
*                    Constants
******************************************************/
#define MAX_NUM_SPI_PRESCALERS     (8)

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
*               Static Function Declarations
******************************************************/

/******************************************************
*               Variables Definitions
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/
uint8_t platform_spi_get_port_number( platform_spi_port_t* spi )
{
  if ( spi == LPC_SPI0 ) {
    return 0;
  }
  else if ( spi == LPC_SPI1 ) {
    return 1;
  }
//  else if ( spi == LPC_SPI2 ) {
//    return 2;
//  }
  else if ( spi == NULL ) {
    return 3;
  }
  else {
    return 0xFF;
  }
}

//
#define SPI_FLASH_CS_LOW         Chip_GPIO_SetPinState(LPC_GPIO, 0, 14, 0)
#define SPI_FLASH_CS_HIGH        Chip_GPIO_SetPinState(LPC_GPIO, 0, 14, 1)
//
#define SPI_FLASH_MISO_LOW
#define SPI_FLASH_MISO_HIGH

#define SPI_FLASH_MISO_GET      Chip_GPIO_GetPinState(LPC_GPIO, 0, 13)

#define SPI_FLASH_MOSI_LOW       Chip_GPIO_SetPinState(LPC_GPIO, 0, 12, 0)
#define SPI_FLASH_MOSI_HIGH      Chip_GPIO_SetPinState(LPC_GPIO, 0, 12, 1)

#define SPI_FLASH_SCK_LOW       Chip_GPIO_SetPinState(LPC_GPIO, 0, 11, 0)
#define SPI_FLASH_SCK_HIGH      Chip_GPIO_SetPinState(LPC_GPIO, 0, 11, 1)

// Magicoe OSStatus platform_spi_init( const platform_spi_t* spi, const platform_spi_config_t* config )
OSStatus platform_spi_init( platform_spi_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_config_t* config )
{
//  SPI_InitTypeDef   spi_init;
  OSStatus          err = kNoErr;
  platform_mcu_powersave_disable();
	uint32_t t1;	//general var
  require_action_quiet( ( peripheral != NULL ) && ( config != NULL ), exit, err = kParamErr);

  require_action_quiet( ( peripheral->port == LPC_SPI0 ), exit, err = kParamErr);


	driver->isRxDone = driver->isTxDone = 0;
	#ifndef NO_MICO_RTOS
	if (driver->sem_xfer_done == 0)
		mico_rtos_init_semaphore(&driver->sem_xfer_done, 1);
	#endif

   // >>> Init Pin mux
  	t1 = IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF;

	if (peripheral->port == LPC_SPI0)
	{
	  	if (config->speed >= 24000000)
	  		t1 |= 1UL<<9;	// enable fast slew
		g_pIO->PIO[0][11] = IOCON_FUNC1 | t1;/* SPI0_SCK */
		g_pIO->PIO[0][12] = IOCON_FUNC1 | t1;	/* SPI0_MOSI */
		g_pIO->PIO[0][13] = IOCON_FUNC1 | t1;	/* SPI0_MISO */
		// config CS pin as GPIO
		g_pIO->PIO[config->chip_select->port][config->chip_select->pin_number] = IOCON_FUNC0  | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF;
		platform_gpio_output_high(config->chip_select);
		platform_gpio_init(config->chip_select, OUTPUT_PUSH_PULL);
	}
	// <<<

	// >>>
	// initialize SPI peripheral
	{
		uint32_t bv;
		LPC_SPI_T *pSPI = peripheral->port;

		// >>>
		bv = pSPI == LPC_SPI0 ? 1UL << 9 : 1UL<<10;

		g_pASys->ASYNCAPBCLKCTRLSET = bv;	// enable clock to SPI

		g_pASys->ASYNCPRESETCTRLSET = bv;
		g_pASys->ASYNCPRESETCTRLCLR = bv;
		// <<<

		//			predly | postDly| fraDly | xferDly
		pSPI->DLY = 1UL<<0 | 1UL<<4 | 1UL<<8 | 1UL<<12;
        pSPI->DLY = 0;

		t1 = Chip_Clock_GetAsyncSyscon_ClockRate();
		t1 = (t1 + config->speed - 1) / config->speed;
		pSPI->DIV = t1 - 1;	// proper division

		pSPI->TXCTRL = (config->bits - 1) << 24;	// 8 bits per frame

		//   Enable | master | no loopback
		t1 = 1UL<<0 | 1UL<<2 | 0UL<<7;
		// determine SPI mode
		if (config->mode & SPI_CLOCK_IDLE_HIGH) {
			t1 |= 1UL<<5;	// CPOL = 1
			if (t1 & SPI_CLOCK_RISING_EDGE)
				t1 |= 1UL<<4;	// CPHA = 1
		} else {
			if (!(t1 & SPI_CLOCK_RISING_EDGE))
				t1 |= 1UL<<4;
		}
		pSPI->CFG = t1;
	}
	// <<<

	g_pDMA->DMACOMMON[0].ENABLESET = (1UL<<peripheral->dmaRxChnNdx) | (1UL<<peripheral->dmaTxChnNdx);
	g_pDMA->DMACOMMON[0].INTENSET = (1UL<<peripheral->dmaRxChnNdx) | (1UL<<peripheral->dmaTxChnNdx);

	g_pDMA->DMACH[peripheral->dmaRxChnNdx].CFG = DMA_CFG_PERIPHREQEN | DMA_CFG_TRIGBURST_SNGL | DMA_CFG_CHPRIORITY(1);
	g_pDMA->DMACH[peripheral->dmaTxChnNdx].CFG = DMA_CFG_PERIPHREQEN | DMA_CFG_TRIGBURST_SNGL | DMA_CFG_CHPRIORITY(1);

exit:
  platform_mcu_powersave_enable();
  return err;
}



// Magicoe OSStatus platform_spi_deinit( const platform_spi_t* spi )
OSStatus platform_spi_deinit( platform_spi_driver_t* driver )
{
	uint32_t bv;
	OSStatus          err = kNoErr;
	platform_spi_t const *peripheral;;
	LPC_SPI_T *pSPI;

	require_action_quiet( ( driver != NULL ) && ( driver->peripheral != NULL ), exit, err = kParamErr);

	peripheral = driver->peripheral , pSPI = peripheral->port;

	bv = pSPI == LPC_SPI0 ? 1UL << 9 : 1UL<<10;

	g_pASys->ASYNCAPBCLKCTRLCLR = bv;	// disable clock to SPI
	g_pASys->ASYNCPRESETCTRLSET = bv;	// put SPI into reset state

	// disable DMA channels for SPI
	g_pDMA->DMACOMMON[0].ENABLECLR = (1UL<<peripheral->dmaRxChnNdx) | (1UL<<peripheral->dmaTxChnNdx);
	g_pDMA->DMACOMMON[0].INTENCLR = (1UL<<peripheral->dmaRxChnNdx) | (1UL<<peripheral->dmaTxChnNdx);

	#ifndef NO_MICO_RTOS
		if (driver->sem_xfer_done != 0)
			mico_rtos_deinit_semaphore(&driver->sem_xfer_done);
		driver->sem_xfer_done = 0;
	#endif
exit:
  return err;
}

/*static*/ platform_spi_driver_t *s_pSPIDrvs[1];
void OnSPIDmaXferDone(uint32_t spiNdx, uint32_t isRx, uint32_t isErr)
{
	platform_spi_driver_t *pDrv = s_pSPIDrvs[spiNdx];
	if (isRx) {
		pDrv->isRxDone = 1;
		if (isErr)
			pDrv->xferErr |= 1UL<<1;
	} else {
		pDrv->isTxDone = 1;
		if (isErr)
			pDrv->xferErr |= 1UL<<0;
	}
	#ifndef NO_MICO_RTOS
	if (pDrv->isTxDone) {
		if (!pDrv->isRx || pDrv->isRxDone)
			mico_rtos_set_semaphore(&pDrv->sem_xfer_done);
	}
	#endif
}

// Magicoe OSStatus platform_spi_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
OSStatus platform_spi_transfer( platform_spi_driver_t* driver, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
	OSStatus err    = kNoErr;
	uint32_t count  = 0;
	uint32_t i;
	const platform_spi_message_segment_t *pSeg;
	uint32_t dmaXferLen;
	DMA_CHDESC_T *pTxDesc, *pRxDesc;
	LPC_SPI_T *pSPI;
	uint32_t dmaRxChnNdx, dmaTxChnNdx;

	const uint8_t *pcTx;
	uint8_t *pRx;

	require_action_quiet( ( driver != NULL ) && ( config != NULL ) && ( segments != NULL ) && ( number_of_segments != 0 ), exit, err = kParamErr);

	// save the driver pointer so that in DMA IRQ callback we can access its members


	platform_mcu_powersave_disable();

	pSeg = segments;
	pTxDesc = (DMA_CHDESC_T *) g_pDMA->SRAMBASE + driver->peripheral->dmaTxChnNdx;
	pRxDesc = (DMA_CHDESC_T *) g_pDMA->SRAMBASE + driver->peripheral->dmaRxChnNdx;
	pSPI = driver->peripheral->port;
	if (pSPI == LPC_SPI0)
	s_pSPIDrvs[0] = driver;
	dmaRxChnNdx = driver->peripheral->dmaRxChnNdx , dmaTxChnNdx = driver->peripheral->dmaTxChnNdx;
	driver->xferErr = 0;
	/* Activate chip select */
	platform_gpio_output_low( config->chip_select );
	for ( i = 0; i < number_of_segments; i++, pSeg++ )
	{
		// transfer one seg
		count = pSeg->length;
		if (0 == count)
			continue;

		pcTx = pSeg->tx_buffer , pRx = pSeg->rx_buffer;

		do
		{
			dmaXferLen = count > DMA_MAX_XFER_CNT ? DMA_MAX_XFER_CNT : count;
			count -= dmaXferLen;
            driver->isRxDone = driver->isTxDone = 0;
			#if 0
			{
				if (pRx != 0)
				{
					pSPI->TXCTRL &= ~(1UL<<22);
                    if (pSPI->STAT & SPI_STAT_RXRDY)
                        pSPI->RXDAT;
					while (dmaXferLen--)
					{
						while (!(pSPI->STAT & SPI_STAT_TXRDY));
						pSPI->TXDAT = *pcTx++;
						while (!(pSPI->STAT & SPI_STAT_RXRDY));
						*pRx++ = (uint8_t) pSPI->RXDAT;
					}
				}
				else
				{
					pSPI->TXCTRL |= (1UL<<22);
					while (dmaXferLen--)
					{
						while (!(pSPI->STAT & SPI_STAT_TXRDY));
						pSPI->TXDAT = *pcTx++;
					}
				}

                while (!(pSPI->STAT & SPI_STAT_TXRDY));
			}
	        #else
				pTxDesc->next = 0;
				pTxDesc->dest = DMA_ADDR(&pSPI->TXDAT);
				pTxDesc->source = DMA_ADDR(pcTx) + dmaXferLen - 1;
				pTxDesc->xfercfg = DMA_XFERCFG_CFGVALID | DMA_XFERCFG_SETINTA |
					DMA_XFERCFG_SWTRIG | DMA_XFERCFG_WIDTH_8 | DMA_XFERCFG_SRCINC_1 |
					DMA_XFERCFG_DSTINC_0 | DMA_XFERCFG_XFERCOUNT(dmaXferLen);

				if (pRx != 0)
				{
                    pSPI->TXCTRL &= ~(1UL<<22);
					driver->isRx = 1;
					pRxDesc->next = 0;
					pRxDesc->source = DMA_ADDR(&pSPI->RXDAT);
					pRxDesc->dest = DMA_ADDR(pRx) + dmaXferLen - 1;
					pRxDesc->xfercfg = DMA_XFERCFG_CFGVALID | DMA_XFERCFG_SETINTA |
						DMA_XFERCFG_SWTRIG | DMA_XFERCFG_WIDTH_8 | DMA_XFERCFG_DSTINC_1 |
						DMA_XFERCFG_SRCINC_0 | DMA_XFERCFG_XFERCOUNT(dmaXferLen);

					// start RX DMA
					g_pDMA->DMACH[dmaRxChnNdx].XFERCFG = pRxDesc->xfercfg;
				} else {
					driver->isRx = 0;
                    pSPI->TXCTRL |= (1UL<<22);
				}

				// start TX DMA
				g_pDMA->DMACH[dmaTxChnNdx].XFERCFG = pTxDesc->xfercfg;



				#ifndef NO_MICO_RTOS
				mico_rtos_get_semaphore(&driver->sem_xfer_done, MICO_WAIT_FOREVER);
				#else
				while(1)
				{
					if (driver->isTxDone)
					{
						if (!driver->isRx || driver->isRxDone)
							break;
					}
//					__WFI();
			        mico_rtos_delay_milliseconds(1);
				}

				#endif
			#endif
			if (driver->xferErr)
			{
				err = kGeneralErr;
				break;
			}
            // >>> update read and/or write pointers
			pcTx += dmaXferLen;
			if (pRx != 0)
				pRx += dmaXferLen;
            // <<<
		} while (count);
	}
	platform_gpio_output_high( config->chip_select );
exit:


  platform_mcu_powersave_enable( );
  return err;
}


uint8_t SPI_SendData(uint8_t data)
{
  uint32_t i = 0, j = 0;
  uint8_t ret = 0;

  for(i=0; i<8; i++) {
    ret = ((ret<<1)&0xFE);
    SPI_FLASH_SCK_LOW;
    for(j=0; j<10; j++);
    if(data&0x80) {
      SPI_FLASH_MOSI_HIGH;
    }
    else{
      SPI_FLASH_MOSI_LOW;
    }
    for(j=0; j<10; j++);
    SPI_FLASH_SCK_HIGH;
    for(j=0; j<10; j++);
    data = (data<<1);
    if((SPI_FLASH_MISO_GET&0x01) == 1) {
      ret = ret|0x01;
    }
    else {
      ret &= 0xFE;
    }
  }
  return ret;
}
//
//static uint16_t spi_transfer( const platform_spi_t* spi, uint16_t data )
//{
//  uint16_t ret = 0;
//  ret = SPI_SendData(data);
////  printf("SPI RW %x %x\r\n", tmp, ret);
//  return ret;
//}

platform_spi_message_segment_t s_segs[3];
#if 0
#pragma data_alignment=16
uint8_t s_spiTestbuf[3200];

extern const mico_spi_device_t mico_spi_flash;
extern platform_spi_driver_t platform_spi_drivers[];
extern const platform_gpio_t platform_gpio_pins[];

void SPIDrvTest(void)
{
	platform_spi_config_t cfg;
	s_segs[0].length = 1024;
	s_segs[0].tx_buffer = (const void*) 0;
	s_segs[0].rx_buffer = s_spiTestbuf;

	s_segs[1].length = 1025;
	s_segs[1].tx_buffer = (const void*) 0x1000;
	s_segs[1].rx_buffer = s_spiTestbuf;

	s_segs[2].length = sizeof(s_spiTestbuf);
	s_segs[2].tx_buffer = (const void*) 0x4000;
	s_segs[2].rx_buffer = s_spiTestbuf;

	cfg.speed = mico_spi_flash.speed;
	cfg.chip_select = platform_gpio_pins + mico_spi_flash.chip_select;
	cfg.mode = mico_spi_flash.mode;
	cfg.bits = mico_spi_flash.bits;

	// >>>
	{

		platform_spi_driver_t *pDrv = &platform_spi_drivers[MICO_SPI_0];
		platform_spi_t const *pHw = pDrv->peripheral;
		platform_spi_init(pDrv, pHw, &cfg);
		platform_spi_deinit(pDrv);
		platform_spi_init(pDrv, pHw, &cfg);
		g_pSPI0->CFG |= 1UL << 7;
		platform_spi_transfer(pDrv, &cfg, s_segs + 0, 3);
	}
}
#else
void SPIDrvTest(void) {}
#endif

// end file --- MG.Niu ---
