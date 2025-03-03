/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */

#include "mico_rtos.h"
//#include "misc.h"
#include "string.h" /* For memcpy */
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_logging.h"
#include "wlan_platform_common.h"
#include "chip.h"

#define DMACHN_WLAN_RX	DMAREQ_SPI1_RX
#define DMACHN_WLAN_TX	DMAREQ_SPI1_TX

#define WLAN_SPI_BUS_MHZ	48
#define WLAN_SPI_CLKCTL_BIT 10

#define USE_WLAN_SPI_SEM

// Caution: If USE_LINKED_DMA_XFER is enabled, DMA is configured to automatically
// load next desc, this enforce some hard real-time requirements to the system!
// It assumes user MUST finsih processing current desc transfer complete IRQ
// before next desc transfer completes, otherwise, you will miss one IRQ!
// Usually this is happened when the last desc has very small data size (E.g., 1 byte)
// #define USE_LINKED_DMA_XFER

// Magicoe TODO delete those tow parameter
volatile uint8_t bDMASPITXDoneFlag = false;
volatile uint8_t bDMASPIRXDoneFlag = false;

#define SEMDELAY        200

/******************************************************
*             Constants
******************************************************/

#define DMA_TIMEOUT_LOOPS      (10000000)

/**
* Transfer direction for the mico platform bus interface
*/
typedef enum
{
    /* If updating this enum, the bus_direction_mapping variable will also need to be updated */
    BUS_READ,
    BUS_WRITE
} bus_transfer_direction_t;

/******************************************************
*             Structures
******************************************************/

/******************************************************
*             Variables
******************************************************/

static mico_semaphore_t spi_transfer_finished_semaphore;
/******************************************************
*             Function declarations
******************************************************/

/* Powersave functionality */
extern void MCU_CLOCKS_NEEDED( void );
extern void MCU_CLOCKS_NOT_NEEDED( void );

extern void wlan_notify_irq( void );

/******************************************************
*             Function definitions
******************************************************/

void spi_irq_handler( void* arg )
{
    UNUSED_PARAMETER(arg);
#ifndef MICO_DISABLE_MCU_POWERSAVE
    platform_mcu_powersave_exit_notify( );
#endif
    wlan_notify_irq( );
}




typedef struct _DS_DualBufDMA
{
    DMA_CHDESC_T *pRxDescs[1], *pTxDescs[1];
    uint8_t isToRx;
    uint8_t tglBit;
    uint8_t descPnding;
    uint32_t c1Rem;		// bytes remain
    const uint8_t *pC1TxBuf;
    uint8_t *pC1RxBuf;

}DS_DualBufDMA;
volatile DS_DualBufDMA s_dbDMA;


int32_t _prvDbDescCfgNext(void)
{
    uint32_t tgl = s_dbDMA.tglBit;
    DMA_CHDESC_T *pTxDesc = s_dbDMA.pTxDescs[tgl];
    DMA_CHDESC_T *pRxDesc = s_dbDMA.pRxDescs[tgl];
    uint32_t xferLen = DMA_MAX_XFER_CNT;

    if (s_dbDMA.c1Rem == 0)
        return -1L;

    s_dbDMA.descPnding++;

    if (s_dbDMA.c1Rem <= DMA_MAX_XFER_CNT) {
        xferLen = s_dbDMA.c1Rem ;
        pRxDesc->next = pTxDesc->next = 0;
    } else {
        xferLen = DMA_MAX_XFER_CNT;
        pRxDesc->next = DMA_ADDR(s_dbDMA.pRxDescs[!tgl]);
        pTxDesc->next = DMA_ADDR(s_dbDMA.pTxDescs[!tgl]);
    }

    pTxDesc->source = DMA_ADDR(s_dbDMA.pC1TxBuf) + xferLen - 1;
    pTxDesc->xfercfg = DMA_XFERCFG_CFGVALID | DMA_XFERCFG_SETINTA |
        DMA_XFERCFG_SWTRIG | DMA_XFERCFG_WIDTH_8 | DMA_XFERCFG_SRCINC_1 |
            DMA_XFERCFG_DSTINC_0 | DMA_XFERCFG_XFERCOUNT(xferLen);

    pRxDesc->dest = DMA_ADDR(s_dbDMA.pC1RxBuf) + xferLen - 1;
    pRxDesc->xfercfg = DMA_XFERCFG_CFGVALID | DMA_XFERCFG_SETINTA |
        DMA_XFERCFG_SWTRIG | DMA_XFERCFG_WIDTH_8 | DMA_XFERCFG_DSTINC_1 |
            DMA_XFERCFG_SRCINC_0 | DMA_XFERCFG_XFERCOUNT(xferLen);

    s_dbDMA.c1Rem -= xferLen , s_dbDMA.pC1RxBuf += xferLen , s_dbDMA.pC1TxBuf += xferLen;

    return s_dbDMA.c1Rem;
}

void wlan_spi_xfer_done(uint32_t isRx)
{
    /* Clear interrupt */
    // process only if the primary channel generates the IRQ, for SPI RX.
    if (--s_dbDMA.descPnding == 0 && s_dbDMA.c1Rem == 0) {
#ifdef USE_WLAN_SPI_SEM
#ifndef NO_MICO_RTOS
        mico_rtos_set_semaphore( &spi_transfer_finished_semaphore );
#endif
#else
        bDMASPITXDoneFlag = 1;
        bDMASPIRXDoneFlag = 1;
#endif
    } else {
        // continue xfer
        // configure next desc

        _prvDbDescCfgNext();

        {
            DMA_CHDESC_T *pDesc = (DMA_CHDESC_T *) g_pDMA->SRAMBASE;
            //            pDesc[DMACHN_WLAN_RX] = s_rxDescs[0];
            //            pDesc[DMACHN_WLAN_TX] = s_txDescs[0];
            // rocky: only start DMA RX if it is for bus read
            // rocky: Toggle bit is not used if no linked DMA xfer
            if (s_dbDMA.isToRx)
                g_pDMA->DMACH[DMACHN_WLAN_RX].XFERCFG = pDesc[DMACHN_WLAN_RX].xfercfg;
            // rocky: DMA TX is ALWAYS initiated
            g_pDMA->DMACH[DMACHN_WLAN_TX].XFERCFG = pDesc[DMACHN_WLAN_TX].xfercfg;
        }

    }
}

void SpiLongXferTest(void);

OSStatus host_platform_bus_init( void )
{
    uint32_t pinCfg;
    platform_mcu_powersave_disable();
    pinCfg = IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF;
    if (WLAN_SPI_BUS_MHZ >= 24)
        pinCfg |= 1UL<<9;	// enable fast slew
    g_pIO->PIO[1][6] = IOCON_FUNC2 | pinCfg;/* SPI1_SCK */
    g_pIO->PIO[1][7] = IOCON_FUNC2 | pinCfg;	/* SPI1_MISO */
    g_pIO->PIO[1][8] = IOCON_FUNC2 | pinCfg;	/* SPI1_MOSI */
    g_pIO->PIO[wifi_spi_pins[WIFI_PIN_SPI_CS].port][wifi_spi_pins[WIFI_PIN_SPI_CS].pin_number] = IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF;	/* SPI1_SSEL0 */

    g_pGP->SET[wifi_spi_pins[WIFI_PIN_SPI_CS].port] = (uint32_t)(1ul<<(wifi_spi_pins[WIFI_PIN_SPI_CS].pin_number));// << 9;
    g_pGP->DIR[wifi_spi_pins[WIFI_PIN_SPI_CS].port] |= (uint32_t)(1ul<<(wifi_spi_pins[WIFI_PIN_SPI_CS].pin_number));// << 9;

#if defined ( MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP )
    /* Set GPIO_B[1:0] to 01 to put WLAN module into gSPI mode */
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_0], OUTPUT_PUSH_PULL );
    platform_gpio_output_high( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_0] );
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1] );
#endif

    // initialize SPI for Wlan
    {
        LPC_SPI_T *pSPI = LPC_SPI1;

        g_pASys->ASYNCAPBCLKCTRLSET = 1UL << 10;	// enable clock to SPI1

        g_pASys->ASYNCPRESETCTRLSET = 1UL << 10;
        g_pASys->ASYNCPRESETCTRLCLR = 1UL << 10;


        //			predly | postDly| fraDly | xferDly
        pSPI->DLY = 1UL<<0 | 1UL<<4 | 1UL<<8 | 1UL<<12;

        {
            uint32_t busClk = Chip_Clock_GetAsyncSyscon_ClockRate();
            uint32_t div = (busClk + WLAN_SPI_BUS_MHZ*1000000 - 1) / (WLAN_SPI_BUS_MHZ*1000000);
            pSPI->DIV = div - 1;	// proper division
        }

        pSPI->TXCTRL = (8-1)<<24;	// 8 bits per frame

        //			Enable | master | MSBfirst | mode 3 | no loopback
        pSPI->CFG = 1UL<<0 | 1UL<<2 | 0UL<<3   	| 3UL<<4 | 0UL<<7;
    }

    bDMASPITXDoneFlag = bDMASPIRXDoneFlag = false;

    /* Setup DMA for SPIX RX */
    g_pDMA->DMACOMMON[0].ENABLESET = (1UL<<DMACHN_WLAN_RX) | (1UL<<DMACHN_WLAN_TX);
    g_pDMA->DMACOMMON[0].INTENSET = (1UL<<DMACHN_WLAN_RX) | (1UL<<DMACHN_WLAN_TX);
    g_pDMA->DMACH[DMACHN_WLAN_RX].CFG = DMA_CFG_PERIPHREQEN | DMA_CFG_TRIGBURST_SNGL | DMA_CFG_CHPRIORITY(0);
    g_pDMA->DMACH[DMACHN_WLAN_TX].CFG = DMA_CFG_PERIPHREQEN | DMA_CFG_TRIGBURST_SNGL | DMA_CFG_CHPRIORITY(0);

#ifndef NO_MICO_RTOS
    mico_rtos_init_semaphore(&spi_transfer_finished_semaphore, 1);
#endif

    // setup PinINT for WLAN
    platform_gpio_init(wifi_spi_pins + WIFI_PIN_SPI_IRQ, INPUT_HIGH_IMPEDANCE);
    platform_gpio_irq_enable(wifi_spi_pins + WIFI_PIN_SPI_IRQ, IRQ_TRIGGER_RISING_EDGE, spi_irq_handler, 0);

    platform_mcu_powersave_enable();


    SpiLongXferTest();

    return kNoErr;
}

OSStatus host_platform_bus_deinit( void )
{
    platform_mcu_powersave_disable();

#ifndef NO_MICO_RTOS
    mico_rtos_deinit_semaphore(&spi_transfer_finished_semaphore);
#endif


#if defined ( MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP )
    /* Clear GPIO_B[1:0] */
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_0], INPUT_HIGH_IMPEDANCE );
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1], INPUT_HIGH_IMPEDANCE );
#endif
    platform_gpio_irq_disable(wifi_spi_pins + WIFI_PIN_SPI_IRQ);


    g_pASys->ASYNCAPBCLKCTRLCLR = 1UL<<WLAN_SPI_CLKCTL_BIT;	// shut down clock to SPI1
    g_pASys->ASYNCPRESETCTRLSET = 1UL << 10;	// put SPI1 in reset mode

    platform_mcu_powersave_enable();

    return kNoErr;
}
extern void DmaAbort(DMA_CHID_T dmaCh);
volatile uint8_t g_isWlanRx;
uint32_t g_xferCnt, g_xferDoneCnt;
OSStatus _prvWlanSPIXfer( bus_transfer_direction_t dir, uint8_t* pTxBuf, uint8_t *pRxBuf, uint16_t buffer_length )
{
    OSStatus result;
    LPC_SPI_T *pSPI = g_pSPI1;
    uint32_t retry;
    DMA_CHDESC_T *pDesc = (DMA_CHDESC_T *) g_pDMA->SRAMBASE;

    platform_mcu_powersave_disable();
    pSPI->STAT = SPI_STAT_RXOV | SPI_STAT_TXUR | SPI_STAT_SSA | SPI_STAT_SSD;

    if (buffer_length < 256 || (buffer_length < 512 && dir == BUS_WRITE))
    {
        platform_gpio_output_low( &wifi_spi_pins[WIFI_PIN_SPI_CS] ); /* CS high (to deselect) */
        NVIC_DisableIRQ(SPI1_IRQn);
        if (dir == BUS_READ)
        {
            pSPI->TXCTRL &= ~(1UL<<22);
            while (buffer_length--)
            {
                while (!(pSPI->STAT & SPI_STAT_TXRDY));
                pSPI->TXDAT = *pTxBuf++;
                while (!(pSPI->STAT & SPI_STAT_RXRDY));
                *pRxBuf++ = (uint8_t) pSPI->RXDAT;
            }
        }
        else
        {
            pSPI->TXCTRL |= (1UL<<22);
            while (buffer_length--)
            {
                while (!(pSPI->STAT & SPI_STAT_TXRDY));
                pSPI->TXDAT = *pTxBuf++;
            }
        }

        while (!(pSPI->STAT & SPI_STAT_TXRDY));
        platform_gpio_output_high( &wifi_spi_pins[WIFI_PIN_SPI_CS] ); /* CS high (to deselect) */
        platform_mcu_powersave_enable();
        return 0;
    }

    // Rocky: we use GPIO to control the SSEL level

    if ( dir == BUS_WRITE)
    {
        pSPI->TXCTRL |= 1UL<<22;	// ignore RX
    } else {
        pSPI->TXCTRL &= ~(1UL<<22);
    }

    bDMASPITXDoneFlag = false;
    bDMASPIRXDoneFlag = false;



    // init dual-buffer DMA control block
    s_dbDMA.c1Rem = buffer_length;
    s_dbDMA.isToRx = dir == BUS_READ ? 1 : 0;
    s_dbDMA.pC1TxBuf = pTxBuf , s_dbDMA.pC1RxBuf = pRxBuf;
    s_dbDMA.pRxDescs[0] = (DMA_CHDESC_T *) g_pDMA->SRAMBASE + DMACHN_WLAN_RX;
    s_dbDMA.pTxDescs[0] = (DMA_CHDESC_T *) g_pDMA->SRAMBASE + DMACHN_WLAN_TX;
    s_dbDMA.pRxDescs[0]->source = DMA_ADDR(&pSPI->RXDAT);
    s_dbDMA.pTxDescs[0]->dest = DMA_ADDR(&pSPI->TXDAT);

    s_dbDMA.tglBit = 0;
    _prvDbDescCfgNext();

    // wait until DMA channel is not active
    for (retry = 5432; retry !=0; retry--)
    {
        if (0 == (g_pDMA->DMACOMMON[0].ACTIVE & (1UL<<DMACHN_WLAN_RX | 1UL<<DMACHN_WLAN_TX)))
            break;
    }

    if (0 == retry)
    {
        return -1L;
    }

    platform_gpio_output_low( &wifi_spi_pins[WIFI_PIN_SPI_CS] ); /* CS high (to deselect) */

    {
        g_isWlanRx = dir == BUS_READ ? 1 : 0;
        // update channel config and start DMA xfer
        g_xferCnt++;
        // rocky: only start DMA RX if it is for bus read
        if (dir == BUS_READ)
            g_pDMA->DMACH[DMACHN_WLAN_RX].XFERCFG = pDesc[DMACHN_WLAN_RX].xfercfg;
        // rocky: DMA TX is ALWAYS initiated
        g_pDMA->DMACH[DMACHN_WLAN_TX].XFERCFG = pDesc[DMACHN_WLAN_TX].xfercfg;

#ifdef USE_WLAN_SPI_SEM
        result = mico_rtos_get_semaphore( &spi_transfer_finished_semaphore, 1000 );
        if (result != kNoErr)
        {

            NVIC_DisableIRQ(DMA_IRQn);
            DmaAbort(DMACHN_WLAN_RX);
            DmaAbort(DMACHN_WLAN_TX);
            g_pDMA->DMACOMMON[0].INTA = 1UL<< DMACHN_WLAN_RX | 1UL<<DMACHN_WLAN_TX;
            NVIC_EnableIRQ(DMA_IRQn);

        }
#else
        {
            while(bDMASPITXDoneFlag == false) {
                //__WFI();
                asm("wfi");
            }
            while(bDMASPIRXDoneFlag == false) {
                //__WFI();
                asm("wfi");
            }
            result = 0;
        }
#endif
        s_dbDMA.tglBit = 0;
        g_xferDoneCnt++;
        /* Clear the CS pin and the DMA status flag */
    }

    platform_gpio_output_high( &wifi_spi_pins[WIFI_PIN_SPI_CS] ); /* CS high (to deselect) */

    platform_mcu_powersave_enable();

    return result;
}

OSStatus host_platform_spi_transfer( bus_transfer_direction_t dir, uint8_t* buffer, uint16_t buffer_length )
{
    OSStatus ret = kNoErr;
    volatile uint32_t retry;
    for (retry=0; retry < 3; )
    {

        ret = _prvWlanSPIXfer(dir, buffer, buffer, buffer_length);
        if (ret != kNoErr)
            retry++;
        else
            break;
    }
    return ret;
}

#if 0
uint32_t s_buf[3072 / 4];
void SpiLongXferTest(void)
{
    uint32_t i;
    uint16_t *pC2Buf = (uint16_t*) s_buf;
    LPC_SPI_T *pSPI = LPC_SPI1;
    pSPI->CFG |= 1UL<<7;

    for (i=0; i<3072/2; i++)
        pC2Buf[i] = i;

    _prvWlanSPIXfer(BUS_READ, (uint8_t*) 0, (uint8_t*)pC2Buf, 3072);

    pSPI->CFG &= ~(1UL<<7);
}
#else
void SpiLongXferTest(void)
{
}
#endif
