/*
 *  Copyright (C) 2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * mdev_ssp.c: mdev driver for SSP
 */

#include <lowlevel_drivers.h>
#include <board.h>
#include <mdev_ssp.h>
#include <wmstdio.h>
#include <wm_os.h>

#define SSP_MAX_PORT 3

/* Threshold - Programmed value in register should be 1 less than desired */
#define SSP_FIFO_RX_THRESHOLD 0 /* RXFIFO = 1 */
#define SSP_FIFO_TX_THRESHOLD 0 /* TXFIFO = 1 */

#define SSP_DMA_FIFO_RX_THRESHOLD 7 /* RXFIFO = 7 */
#define SSP_DMA_FIFO_TX_THRESHOLD 8 /* TXFIFO = 7 */

#define SSP_RX_BUF_SIZE 2048
#define SSP_IRQn_BASE SSP0_IRQn
#define SSP_INT_BASE INT_SSP0
#define DEFAULT_SSP_FREQ 10000000

/* DMA tx and rx channels (should be between 0-7)*/
#define SSP_TX_CHANNEL 0
#define SSP_RX_CHANNEL 1

/* DMA transaction wait count */
/* FIXME: Wait is kept sufficent large enough to read/write data.
 * But this should removed by interrupt mechanism
*/
#define SSP_MAX_DMA_WAIT 10000000

/* Maximum dma transfer length */
#define DMA_MAX_BLK_SIZE 1023

static mdev_t mdev_ssp[SSP_MAX_PORT];
static os_mutex_t ssp_mutex[SSP_MAX_PORT];
typedef struct sspdrv_ringbuf {
	int wr_idx;		/* write pointer */
	int rd_idx;		/* read pointer */
	int buf_size;	/* number of bytes in buffer *buf_p */
	uint8_t *buf_p;		/* pointer to buffer storage */
} sspdrv_ringbuf_t;

typedef struct sspdev_priv_data {
	sspdrv_ringbuf_t rx_ringbuf;
	SSP_FrameFormat_Type device_type;
	unsigned int cs;
	uint32_t freq;
	bool slave:1; /* 0 = Master and 1 = Slave */
	bool cs_level:1; /* 0 = Active low and 1 = Active high */
	bool dma:1; /* 0 = Disable and 1 = Enable */
} sspdev_data_t;

static sspdev_data_t sspdev_data[SSP_MAX_PORT];
static void sspdrv_read_irq_handler(int port_id);
static void sspdrv_overrun_irq_handler(int port_id);

static void (*ssp_read_irq_handler[SSP_MAX_PORT])();
static void (*ssp_overrun_irq_handler[SSP_MAX_PORT])();

#define SET_RX_BUF_SIZE(ssp_data_p, size) ssp_data_p->rx_ringbuf.buf_size = size
#define GET_RX_BUF_SIZE(ssp_data_p) (ssp_data_p->rx_ringbuf.buf_size)

#define DEFINE_SSP_RD_CB(id) \
	static void \
	sspdrv ## id ## _ ## readirq_handler() {\
		sspdrv_ ## read_irq_handler(id); }

#define GET_SSP_RD_CB(id) sspdrv ## id ## _ ## readirq_handler

DEFINE_SSP_RD_CB(0)
DEFINE_SSP_RD_CB(1)
DEFINE_SSP_RD_CB(2)

#define DEFINE_SSP_OR_CB(id) \
	static void \
	sspdrv ## id ## _ ## overrunirq_handler() {\
		sspdrv_ ## overrun_irq_handler(id); }

#define GET_SSP_OR_CB(id) sspdrv ## id ## _ ## overrunirq_handler

DEFINE_SSP_OR_CB(0)
DEFINE_SSP_OR_CB(1)
DEFINE_SSP_OR_CB(2)

static void fill_cb_array()
{
	ssp_read_irq_handler[0] = GET_SSP_RD_CB(0);
	ssp_read_irq_handler[1] = GET_SSP_RD_CB(1);
	ssp_read_irq_handler[2] = GET_SSP_RD_CB(2);

	ssp_overrun_irq_handler[0] = GET_SSP_OR_CB(0);
	ssp_overrun_irq_handler[1] = GET_SSP_OR_CB(1);
	ssp_overrun_irq_handler[2] = GET_SSP_OR_CB(2);
}

/* ring buffer apis */

static uint8_t read_rx_buf(sspdev_data_t *ssp_data_p)
{
	uint8_t ret;
	int idx;
	idx = ssp_data_p->rx_ringbuf.rd_idx;
	ret = ssp_data_p->rx_ringbuf.buf_p[idx];
	ssp_data_p->rx_ringbuf.rd_idx = (ssp_data_p->rx_ringbuf.rd_idx + 1)
		% GET_RX_BUF_SIZE(ssp_data_p);
	return ret;
}

static void write_rx_buf(sspdev_data_t *ssp_data_p, uint8_t wr_char)
{
	int idx;
	idx = ssp_data_p->rx_ringbuf.wr_idx;
	ssp_data_p->rx_ringbuf.buf_p[idx] = wr_char;
	ssp_data_p->rx_ringbuf.wr_idx = (ssp_data_p->rx_ringbuf.wr_idx + 1)
		% GET_RX_BUF_SIZE(ssp_data_p);
}

static bool is_rx_buf_empty(sspdev_data_t *ssp_data_p)
{
	int rd_idx, wr_idx;
	rd_idx = ssp_data_p->rx_ringbuf.rd_idx;
	wr_idx = ssp_data_p->rx_ringbuf.wr_idx;
	if (rd_idx == wr_idx) {
		return true;
	}
	return false;
}

static bool is_rx_buf_full(sspdev_data_t *ssp_data_p)
{
	int wr_idx, rd_idx;
	rd_idx = ssp_data_p->rx_ringbuf.rd_idx;
	wr_idx = ssp_data_p->rx_ringbuf.wr_idx;
	if (((wr_idx + 1) % GET_RX_BUF_SIZE(ssp_data_p)) == rd_idx) {
		return true;
	}
	return false;
}

static int rx_buf_init(sspdev_data_t *ssp_data_p)
{
	ssp_data_p->rx_ringbuf.rd_idx = 0;
	ssp_data_p->rx_ringbuf.wr_idx = 0;
	ssp_data_p->rx_ringbuf.buf_p = os_mem_alloc
		(GET_RX_BUF_SIZE(ssp_data_p));
	if (!ssp_data_p->rx_ringbuf.buf_p)
		return -WM_FAIL;
	return WM_SUCCESS;
}

static void ssp_dma_get_rx_config(DMA_CFG_Type *dma, uint32_t addr,
		int port_id, bool dummy_data)
{
	switch (port_id) {
	case SSP0_ID:
		dma->srcDmaAddr = (uint32_t) &SSP0->SSDR.WORDVAL;
		dma->srcHwHdskInterf = DMA_HW_HS_INTER_9;
		DMA_SetHandshakingMapping(DMA_HS9_SSP0_RX);
		break;
	case SSP1_ID:
		dma->srcDmaAddr = (uint32_t) &SSP1->SSDR.WORDVAL;
		dma->srcHwHdskInterf = DMA_HW_HS_INTER_3;
		DMA_SetHandshakingMapping(DMA_HS3_SSP1_RX);
		break;
	case SSP2_ID:
		dma->srcDmaAddr = (uint32_t) &SSP2->SSDR.WORDVAL;
		dma->srcHwHdskInterf = DMA_HW_HS_INTER_4;
		DMA_SetHandshakingMapping(DMA_HS4_SSP2_RX);
		break;
	}

	/* DMA controller peripheral to memory configuration */
	dma->destDmaAddr = addr;
	dma->transfType = DMA_PER_TO_MEM;
	dma->srcBurstLength = DMA_ITEM_8;
	dma->destBurstLength = DMA_ITEM_8;
	dma->srcAddrInc = DMA_ADDR_NOCHANGE;
	if (!dummy_data)
		dma->destAddrInc = DMA_ADDR_INC;
	else
		dma->destAddrInc = DMA_ADDR_NOCHANGE;
	dma->srcTransfWidth = DMA_TRANSF_WIDTH_8;
	dma->destTransfWidth = DMA_TRANSF_WIDTH_8;
	dma->srcSwHwHdskSel = DMA_HW_HANDSHAKING;
	dma->destSwHwHdskSel = DMA_HW_HANDSHAKING;
	dma->channelPriority = DMA_CH_PRIORITY_3;
	dma->destHwHdskInterf = DMA_HW_HS_INTER_0;
	dma->fifoMode = DMA_FIFO_MODE_0;
}

static void ssp_dma_get_tx_config(DMA_CFG_Type *dma, uint32_t addr,
		int port_id, bool dummy_data)
{
	switch (port_id) {
	case SSP0_ID:
		dma->destDmaAddr = (uint32_t) &SSP0->SSDR.WORDVAL;
		dma->destHwHdskInterf = DMA_HW_HS_INTER_10;
		DMA_SetHandshakingMapping(DMA_HS10_SSP0_TX);
		break;
	case SSP1_ID:
		dma->destDmaAddr = (uint32_t) &SSP1->SSDR.WORDVAL;
		dma->destHwHdskInterf = DMA_HW_HS_INTER_7;
		DMA_SetHandshakingMapping(DMA_HS7_SSP1_TX);
		break;
	case SSP2_ID:
		dma->destDmaAddr = (uint32_t) &SSP2->SSDR.WORDVAL;
		dma->destHwHdskInterf = DMA_HW_HS_INTER_11;
		DMA_SetHandshakingMapping(DMA_HS11_SSP2_TX);
		break;
	}

	/* DMA controller memory to peripheral configuration */
	dma->srcDmaAddr = addr;
	dma->transfType = DMA_MEM_TO_PER;
	dma->srcBurstLength = DMA_ITEM_8;
	dma->destBurstLength = DMA_ITEM_8;
	if (!dummy_data)
		dma->srcAddrInc = DMA_ADDR_INC;
	else
		dma->srcAddrInc = DMA_ADDR_NOCHANGE;
	dma->destAddrInc = DMA_ADDR_NOCHANGE;
	dma->srcTransfWidth = DMA_TRANSF_WIDTH_8;
	dma->destTransfWidth = DMA_TRANSF_WIDTH_8;
	dma->srcSwHwHdskSel = DMA_HW_HANDSHAKING;
	dma->destSwHwHdskSel = DMA_HW_HANDSHAKING;
	dma->channelPriority = DMA_CH_PRIORITY_4;
	dma->srcHwHdskInterf = DMA_HW_HS_INTER_0;
	dma->fifoMode = DMA_FIFO_MODE_0;
}

/* Device name */
static const char *mdev_ssp_name[SSP_MAX_PORT] = {
	"MDEV_SSP0",
	"MDEV_SSP1",
	"MDEV_SSP2"
};

static int ssp_drv_dma_read(mdev_t *dev, uint8_t *data, int num)
{
	DMA_CFG_Type dma_tx, dma_rx;
	uint8_t dummy;
	uint16_t dma_trans = DMA_MAX_BLK_SIZE, len = num;
	uint32_t cnt;
	sspdev_data_t *ssp_data_p;

	ssp_data_p = (sspdev_data_t *) dev->private_data;
	dummy = SPI_DUMMY_WORD;
	ssp_dma_get_tx_config(&dma_tx, (uint32_t) &dummy, dev->port_id, 1);
	ssp_dma_get_rx_config(&dma_rx, (uint32_t) data, dev->port_id, 0);

	while (num > 0) {
		if (num < DMA_MAX_BLK_SIZE)
			dma_trans = num;
		if (ssp_data_p->slave == SSP_SLAVE) {

			/* FIXME: To start a dma operation, ssp needs to
			 * be disabled and enable to flush HW fifo. Ideally
			 * this shouldn't be done as we can miss some
			 * data on line, but without disabling and enabling
			 * again we will read some garbage data from HW fifo.
			 */
			SSP_Disable(dev->port_id);
			DMA_Disable();
			SSP_Enable(dev->port_id);
			DMA_ChannelInit(SSP_RX_CHANNEL, &dma_rx);
			DMA_IntClr(SSP_RX_CHANNEL, INT_CH_ALL);
			DMA_IntMask(SSP_RX_CHANNEL, INT_CH_ALL, MASK);
			DMA_SetBlkTransfSize(SSP_RX_CHANNEL, dma_trans);

			DMA_Enable();
			DMA_ChannelCmd(SSP_RX_CHANNEL, ENABLE);

			/* Wait for dma transfer to complete on given channel */
			cnt = SSP_MAX_DMA_WAIT;
			while (--cnt &&
			(DMA->RAWBLOCK.BF.RAW & (1 << SSP_RX_CHANNEL)) == 0)
				;
			if (!cnt) {
				return 0;
			}
		} else {
			/* Configure channel 0 for tx and 1 for rx */
			DMA_Disable();
			DMA_ChannelInit(SSP_TX_CHANNEL, &dma_tx);
			DMA_IntClr(SSP_TX_CHANNEL, INT_CH_ALL);
			DMA_IntMask(SSP_TX_CHANNEL, INT_CH_ALL, MASK);
			DMA_SetBlkTransfSize(SSP_TX_CHANNEL, dma_trans);

			DMA_ChannelInit(SSP_RX_CHANNEL, &dma_rx);
			DMA_IntClr(SSP_RX_CHANNEL, INT_CH_ALL);
			DMA_IntMask(SSP_RX_CHANNEL, INT_CH_ALL, MASK);
			DMA_SetBlkTransfSize(SSP_RX_CHANNEL, dma_trans);

			DMA_Enable();
			DMA_ChannelCmd(SSP_TX_CHANNEL, ENABLE);
			DMA_ChannelCmd(SSP_RX_CHANNEL, ENABLE);

			/* Wait for dma transfer to complete on given channel */
			cnt = SSP_MAX_DMA_WAIT;
			while (--cnt &&
			(DMA->RAWBLOCK.BF.RAW & (1 << SSP_TX_CHANNEL)) == 0)
				;
			if (!cnt) {
				SSP_LOG("%s: dma tx operation timed out\r\n",
					__func__);
				return 0;
			}

			/* Wait for dma transfer to complete on given channel */
			cnt = SSP_MAX_DMA_WAIT;
			while (--cnt &&
		       (DMA->RAWBLOCK.BF.RAW & (1 << SSP_RX_CHANNEL)) == 0)
				;
			if (!cnt) {
				SSP_LOG("%s: dma rx operation timed out\r\n",
					__func__);
				return 0;
			}
		}
		/* Increment destination address for rx channel */
		dma_rx.destDmaAddr += dma_trans;
		num -= dma_trans;
	}
	return len;
}

static int ssp_drv_dma_write(mdev_t *dev, const uint8_t *data, uint8_t *din,
		uint32_t num, bool flag)
{
	DMA_CFG_Type dma_tx, dma_rx;
	uint8_t dummy;
	uint16_t dma_trans = DMA_MAX_BLK_SIZE;
	uint32_t cnt;
	uint16_t len = num;
	ssp_dma_get_tx_config(&dma_tx, (uint32_t) data, dev->port_id, 0);
	if (din)
		ssp_dma_get_rx_config(&dma_rx, (uint32_t) din,
				dev->port_id, 0);
	else
		ssp_dma_get_rx_config(&dma_rx, (uint32_t) &dummy,
				dev->port_id, 1);
	while (num > 0) {
		if (num < DMA_MAX_BLK_SIZE)
			dma_trans = num;

		/* Configure channel 0 for tx and 1 for rx (optional) */
		DMA_Disable();
		DMA_ChannelInit(SSP_TX_CHANNEL, &dma_tx);
		DMA_IntClr(SSP_TX_CHANNEL, INT_CH_ALL);
		DMA_IntMask(SSP_TX_CHANNEL, INT_CH_ALL, MASK);
		DMA_SetBlkTransfSize(SSP_TX_CHANNEL, dma_trans);
		if (flag) {
			DMA_ChannelInit(SSP_RX_CHANNEL, &dma_rx);
			DMA_IntClr(SSP_RX_CHANNEL, INT_CH_ALL);
			DMA_IntMask(SSP_RX_CHANNEL, INT_CH_ALL, MASK);
			DMA_SetBlkTransfSize(SSP_RX_CHANNEL, dma_trans);
		}

		DMA_Enable();
		DMA_ChannelCmd(SSP_TX_CHANNEL, ENABLE);
		if (flag)
			DMA_ChannelCmd(SSP_RX_CHANNEL, ENABLE);

		/* Wait for dma transfer to complete on given channel */
		cnt = SSP_MAX_DMA_WAIT;
		while (--cnt &&
			(DMA->RAWBLOCK.BF.RAW & (1 << SSP_TX_CHANNEL)) == 0)
			;
		if (!cnt) {
			SSP_LOG("%s: dma tx operation timed out\r\n",
					__func__);
			return -1;
		}
		if (flag) {
			/* Wait for dma transfer to complete on channel */
			cnt = SSP_MAX_DMA_WAIT;
			while (--cnt &&
			(DMA->RAWBLOCK.BF.RAW & (1 << SSP_RX_CHANNEL)) == 0)
				;
			if (!cnt) {
				SSP_LOG("%s: dma rx operation timed out\r\n",
						__func__);
				return -1;
			}

		}
		if (din)
			/* Increment destination address for rx channel */
			dma_rx.destDmaAddr += dma_trans;
		/* Increment source address for tx channel */
		dma_tx.srcDmaAddr += dma_trans;
		num -= dma_trans;
	}
	return len;
}

static int ssp_drv_pio_write(mdev_t *dev, const uint8_t *data, uint8_t *din,
		uint32_t num, bool flag)
{
	uint32_t len = 0;
	uint32_t size = num;

	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;

	while (num > 0) {
		/* Wait if fifo is full */
		while (SSP_GetStatus(dev->port_id, SSP_STATUS_TFNF) != SET)
			os_thread_relinquish();

		SSP_SendData(dev->port_id, *data);
		data++;
		num--;
		/* Enable read RXFIFO during write operation if flag is set */
		if (flag) {

			/* read data while write operation is on to enable
			 * full duplex support in SPI protocol*/
			/*
			 * Read rxfifo and store data, provided valid din
			 * buffer is provided by user else discard it.
			 */

			if (ssp_data_p->slave == SSP_MASTER) {

				/* SSP_STATUS_BIT == SET when SSPx port is
				 * currently transmitting or receiving framed
				 * data.
				 * */
				while (SSP_GetStatus(dev->port_id,
						     SSP_STATUS_BUSY) == SET)
					os_thread_relinquish();

				/*  SSP_STATUS_RFNE == SET when RXFIFO is
				 *  non-empty
				 */
				while (SSP_GetStatus(dev->port_id,
						     SSP_STATUS_RFNE) != SET)
					os_thread_relinquish();

				if (din)
					*din++ = (uint8_t)
						SSP_RecvData(dev->port_id);
				else
					SSP_RecvData(dev->port_id);
			} else {
				/* if slave mode is enabled read from software
				 * fifo. We do not block on data just yet, but
				 * continue writing. This ensures that when
				 * master starts the clock, there is enough data
				 * to send in slave tx fifo. We come back
				 * to pending data read after write is over */
				if (is_rx_buf_empty(ssp_data_p) == true)
					continue;
				if (din)
					*din++ = (uint8_t)
						read_rx_buf(ssp_data_p);
				else
					read_rx_buf(ssp_data_p);
			}
		}
		len++;
	}

	/* Return if master as master can receive data immediately.
	 * Also return if read is not requested by slave.
	 */
	if (ssp_data_p->slave == SSP_MASTER || !flag)
		return len;

	/* Read pending data in case of slave */
	while (len < size) {
		while (is_rx_buf_empty(ssp_data_p) == true)
			os_thread_relinquish();
		if (din)
			*din++ = (uint8_t)
				read_rx_buf(ssp_data_p);
		else
			read_rx_buf(ssp_data_p);
		len++;
	}

	return len;
}

static int ssp_drv_pio_read(mdev_t *dev, uint8_t *data, uint32_t num)
{
	int len = 0;
	sspdev_data_t *ssp_data_p;

	ssp_data_p = (sspdev_data_t *) dev->private_data;

	if (ssp_data_p->slave == SSP_SLAVE) {
		while (num > 0) {
			if (!is_rx_buf_empty(ssp_data_p))
				*data = (uint8_t)read_rx_buf(ssp_data_p);
			else
				break;
			num--;
			len++;
			data++;
		}
	} else {
		/* In master mode, first send dummy data in TX line and read
		 * simultaneously. If nothing is on RX line then it can read
		 * 0x0 or 0xff. Application should check whether data is
		 * vaild or not.
		*/
		while (num > 0) {
			while (SSP_GetStatus(dev->port_id, SSP_STATUS_TFNF)
				!= SET)
				os_thread_relinquish();

			SSP_SendData(dev->port_id, SPI_DUMMY_WORD);
			while (SSP_GetStatus(dev->port_id, SSP_STATUS_RFNE)
				!= SET)
				os_thread_relinquish();
			*data = (uint8_t)SSP_RecvData(dev->port_id);
			data++;
			len++;
			num--;
		}
	}
	return len;
}

int ssp_drv_write(mdev_t *dev, const uint8_t *data, uint8_t *din, uint32_t num,
		bool flag)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;

	if (ssp_data_p->dma)
		return ssp_drv_dma_write(dev, data, din, num, flag);
	else
		return ssp_drv_pio_write(dev, data, din, num, flag);
}

int ssp_drv_read(mdev_t *dev, uint8_t *data, uint32_t num)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;

	if (ssp_data_p->dma)
		return ssp_drv_dma_read(dev, data, num);
	else
		return ssp_drv_pio_read(dev, data, num);
}

static void sspdrv_read_irq_handler(int port_num)
{
	uint32_t data;
	mdev_t *dev = &mdev_ssp[port_num];
	sspdev_data_t *ssp_data_p;

	ssp_data_p = (sspdev_data_t *) dev->private_data;
	while (SSP_GetStatus(dev->port_id, SSP_STATUS_RFNE)
			== SET) {
		/* Read data from HW-FIFO to user allocated buffer
		 * if buffer is not full */
		if (!is_rx_buf_full(ssp_data_p)) {
			data = SSP_RecvData(dev->port_id);
			write_rx_buf(ssp_data_p, (uint8_t)data);
		} else {
			break;
		}
	}

}

/* FIXME: Add mechanism to communicate error conditions to user */
static void sspdrv_overrun_irq_handler(int port_num)
{
	mdev_t *dev = &mdev_ssp[port_num];

	SSP_LOG("FIFO overrun\r\n");
	SSP_IntClr(dev->port_id , SSP_INT_RFORI);

	/* Empty entire FIFO when FIFO overrun is detected */
	while (SSP_GetStatus(dev->port_id, SSP_STATUS_RFNE)
			== SET)
		SSP_RecvData(dev->port_id);
}

void ssp_drv_cs_activate(mdev_t *dev)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;
	GPIO_WritePinOutput(ssp_data_p->cs, ssp_data_p->cs_level);
}

void ssp_drv_cs_deactivate(mdev_t *dev)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;
	GPIO_WritePinOutput(ssp_data_p->cs, !ssp_data_p->cs_level);
}

mdev_t *ssp_drv_open(SSP_ID_Type ssp_id, SSP_FrameFormat_Type format,
		SSP_MS_Type mode, SSP_DMA dma, int cs, bool level)
{
	int ret;
	SSP_CFG_Type sspCfgStruct;
	SSP_FIFO_Type sspFifoCfg;
	SPI_Param_Type spiParaStruct;
	SSP_NWK_Type sspNetworkCfg;
	PSP_Param_Type pspParaStruct;
	sspdev_data_t *ssp_data_p;
	mdev_t *mdev_p = mdev_get_handle(mdev_ssp_name[ssp_id]);
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;

	if (mdev_p == NULL) {
		SSP_LOG("Unable to open device %s\r\n",
			mdev_ssp_name[ssp_id]);
		return NULL;
	}
	ssp_data_p->slave = mode;
	ret = os_mutex_get(&ssp_mutex[mdev_p->port_id], OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		SSP_LOG("failed to get mutex\r\n");
		return NULL;
	}
	/* If ringbuffer size is not set by user then set to default size */
	if (GET_RX_BUF_SIZE(ssp_data_p) == 0) {
		SET_RX_BUF_SIZE(ssp_data_p, SSP_RX_BUF_SIZE);
	}
	/* Allocate ring buffer only for slave when dma is disabled*/
	if (mode == SSP_SLAVE && dma == DMA_DISABLE)
		if (rx_buf_init(ssp_data_p)) {
			SSP_LOG("Unable to allocate ssp software"
					" ring buffer\r\n");
			return NULL;
		}

	/* If clk is not set by user then set it to default */
	if (ssp_data_p->freq == 0) {
		ret = ssp_drv_set_clk(mdev_p->port_id, DEFAULT_SSP_FREQ);
	}

	/* Configure the pinmux for ssp pins */
	if (cs >= 0 && mode == SSP_MASTER) {
		board_ssp_pin_config(mdev_p->port_id, 0);
		/* Use user specified chip select pin */
		ssp_data_p->cs = cs;
		ssp_data_p->cs_level = level;
		GPIO_PinMuxFun(cs, PINMUX_FUNCTION_0);
		GPIO_SetPinDir(cs, GPIO_OUTPUT);
		/* Initially keep slave de-selected */
		GPIO_WritePinOutput(cs, !level);
	} else {
		board_ssp_pin_config(mdev_p->port_id, 1);
	}

	/* Configure SSP interface */
	sspCfgStruct.mode = SSP_NORMAL;
	sspCfgStruct.masterOrSlave = mode;
	sspCfgStruct.trMode = SSP_TR_MODE;
	sspCfgStruct.dataSize = SSP_DATASIZE_8;
	sspCfgStruct.sfrmPola = SSP_SAMEFRM_PSP;
	sspCfgStruct.slaveClkRunning = SSP_SLAVECLK_TRANSFER;
	sspCfgStruct.txd3StateEnable = ENABLE;
	/* RXFIFO inactivity timeout, [timeout = 100 / 26MHz] (Bus clock) */
	sspCfgStruct.timeOutVal = 100;

	switch (format) {
	case SSP_FRAME_SPI:
		sspCfgStruct.frameFormat = SSP_FRAME_SPI;
		sspCfgStruct.txd3StateType = SSP_TXD3STATE_ELSB;
		break;
	case SSP_FRAME_PSP:
		sspCfgStruct.frameFormat = SSP_FRAME_PSP;
		sspCfgStruct.txd3StateType = SSP_TXD3STATE_12SLSB;
		break;
	case SSP_FRAME_SSP:
		SSP_LOG("Frame Format not implemented.\r\n");
		return NULL;
	}

	/* Configure SSP Fifo */
	sspFifoCfg.fifoPackMode =  DISABLE;

	/* See if dma needs to be enabled */
	if (dma == DMA_ENABLE) {
		/* Enable DMA controller clock */
		CLK_ModuleClkEnable(CLK_DMAC);
		sspFifoCfg.rxFifoFullLevel = SSP_DMA_FIFO_RX_THRESHOLD;
		sspFifoCfg.txFifoEmptyLevel = SSP_DMA_FIFO_TX_THRESHOLD;
		sspFifoCfg.rxDmaService = ENABLE;
		sspFifoCfg.txDmaService = ENABLE;
		ssp_data_p->dma = 1;
		sspCfgStruct.trailByte = SSP_TRAILBYTE_DMA;
	} else {
		sspFifoCfg.rxFifoFullLevel = SSP_FIFO_RX_THRESHOLD;
		sspFifoCfg.txFifoEmptyLevel = SSP_FIFO_TX_THRESHOLD;
		sspFifoCfg.rxDmaService = DISABLE;
		sspFifoCfg.txDmaService = DISABLE;
		sspCfgStruct.trailByte = SSP_TRAILBYTE_CORE;
	}

	/* Let the settings take effect */
	SSP_Disable(mdev_p->port_id);
	SSP_Init(mdev_p->port_id, &sspCfgStruct);
	SSP_FifoConfig(mdev_p->port_id, &sspFifoCfg);

	/* Do frame format config */
	switch (format) {
	case SSP_FRAME_SPI:
		spiParaStruct.spiClkPhase = SPI_SCPHA_1;
		spiParaStruct.spiClkPolarity = SPI_SCPOL_LOW;
		SPI_Config(mdev_p->port_id, &spiParaStruct);
		break;
	case SSP_FRAME_PSP:
		pspParaStruct.pspFsrtType = 0;
		pspParaStruct.pspClkMode = PSP_DRIVFALL_SAMPRISE_IDLELOW;
		pspParaStruct.pspFrmPola = PSP_SFRMP_LOW;
		pspParaStruct.pspEndTransState = PSP_ENDTRANS_LOW;
		pspParaStruct.startDelay = 0;
		pspParaStruct.dummyStart = 0;
		pspParaStruct.dummyStop = 0;
		pspParaStruct.frmDelay = 0;
		pspParaStruct.frmLength = 8;
		PSP_Config(mdev_p->port_id, &pspParaStruct);
		sspNetworkCfg.frameRateDiv = 1;
		sspNetworkCfg.txTimeSlotActive = 3;
		sspNetworkCfg.rxTimeSlotActive = 3;
		SSP_NwkConfig(mdev_p->port_id, &sspNetworkCfg);
		break;
	case SSP_FRAME_SSP:
		SSP_LOG("Frame Format not implemented.\r\n");
		return NULL;
	}

	/* Enable read interrupts only for slave when dma is disabled*/
	if (mode == SSP_SLAVE && dma == DMA_DISABLE) {
		install_int_callback(SSP_INT_BASE + mdev_p->port_id,
				     SSP_INT_RFFI,
				     ssp_read_irq_handler[mdev_p->port_id]);
		install_int_callback(SSP_INT_BASE + mdev_p->port_id,
				     SSP_INT_RFORI,
				     ssp_overrun_irq_handler[mdev_p->port_id]);
		NVIC_EnableIRQ(SSP_IRQn_BASE + mdev_p->port_id);
		NVIC_SetPriority(SSP_IRQn_BASE + mdev_p->port_id, 0xF);
		SSP_IntMask(mdev_p->port_id, SSP_INT_RFFI, UNMASK);
	}

	SSP_Enable(mdev_p->port_id);
	return mdev_p;
}

int ssp_drv_close(mdev_t *dev)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;
	os_mem_free(ssp_data_p->rx_ringbuf.buf_p);
	SSP_Disable(dev->port_id);

	return os_mutex_put(&ssp_mutex[dev->port_id]);
}

static void ssp_drv_mdev_init(uint8_t port_num)
{
	mdev_t *mdev_p;
	sspdev_data_t *ssp_data_p;

	mdev_p = &mdev_ssp[port_num];
	mdev_p->name = mdev_ssp_name[port_num];
	mdev_p->port_id = port_num;
	mdev_p->private_data = (uint32_t)&sspdev_data[port_num];
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;
	SET_RX_BUF_SIZE(ssp_data_p, 0);
	ssp_data_p->freq = 0;
}

int ssp_drv_rxbuf_size(SSP_ID_Type ssp_id, uint32_t size)
{
	mdev_t *mdev_p;
	sspdev_data_t *ssp_data_p;

	if (!IS_SSP_PERIPH(ssp_id) || size < 0)
		return -WM_FAIL;
	/* Check ssp_id is initialized */
	mdev_p = mdev_get_handle(mdev_ssp_name[ssp_id]);
	if (!mdev_p) {
		SSP_LOG("Unable to open device %s\r\n",
			mdev_ssp_name[ssp_id]);
		return -WM_FAIL;
	}
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;
	SET_RX_BUF_SIZE(ssp_data_p, size);
	return WM_SUCCESS;
}

uint32_t ssp_drv_set_clk(SSP_ID_Type ssp_id, uint32_t freq)
{
	mdev_t *mdev_p;
	sspdev_data_t *ssp_data_p;
	uint16_t divider;

	if (!IS_SSP_PERIPH(ssp_id) || freq <= 0)
		return -WM_FAIL;
	/* Check ssp_id is initialized */
	mdev_p = mdev_get_handle(mdev_ssp_name[ssp_id]);
	if (!mdev_p) {
		SSP_LOG("Unable to open device %s\r\n",
			mdev_ssp_name[ssp_id]);
		return -WM_FAIL;
	}
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;
	/* Calculate Divider */
	divider = (uint32_t)(board_cpu_freq()/freq);
	ssp_data_p->freq = (uint32_t)((board_cpu_freq() / divider));

	if (!IS_SSP_DIVIDER(divider)) {
		SSP_LOG("Unable to set %u frequency for %s %d\r\n",
			freq, mdev_ssp_name[ssp_id], divider);
		ssp_data_p->freq = 0;
		return -WM_FAIL;
	}
	switch (ssp_id) {
	case SSP0_ID:
		CLK_ModuleClkEnable(CLK_SSP0);
		CLK_SSPClkSrc(CLK_SSP_ID_0, CLK_SYSTEM);
		CLK_ModuleClkDivider(CLK_SSP0, divider);
		break;
	case SSP1_ID:
		CLK_ModuleClkEnable(CLK_SSP1);
		CLK_SSPClkSrc(CLK_SSP_ID_1, CLK_SYSTEM);
		CLK_ModuleClkDivider(CLK_SSP1, divider);
		break;
	case SSP2_ID:
		CLK_ModuleClkEnable(CLK_SSP2);
		CLK_SSPClkSrc(CLK_SSP_ID_2, CLK_SYSTEM);
		CLK_ModuleClkDivider(CLK_SSP2, divider);
		break;
	}
	return ssp_data_p->freq;
}

int ssp_drv_init(SSP_ID_Type id)
{
	int ret;

	if (mdev_get_handle(mdev_ssp_name[id]) != NULL)
		return WM_SUCCESS;

	ret = os_mutex_create(&ssp_mutex[id], "ssp",
			      OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Initialize the SSP port's mdev structure */
	ssp_drv_mdev_init(id);

	/* Register the SSP device driver for this port */
	mdev_register(&mdev_ssp[id]);

	fill_cb_array();

	return WM_SUCCESS;
}

void ssp_drv_deinit(SSP_ID_Type id)
{
	mdev_t *dev;

	/* Return if the port ID is invalid */
	if (id < 0 || id >= SSP_MAX_PORT) {
		SSP_LOG("Port %d not enabled\r\n", id);
		return;
	}

	/* Return if the device was not registered */
	if ((dev = mdev_get_handle(mdev_ssp_name[id])) == NULL)
		return;

	/* Delete mutex and dregister the device */
	os_mutex_delete(&ssp_mutex[dev->port_id]);
	mdev_deregister(dev->name);
}
