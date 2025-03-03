/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include <board.h>
#include <mdev_i2c.h>
#include <wm_os.h>
#include <limits.h>
#include <lowlevel_drivers.h>
#include <wmlog.h>

#ifdef CONFIG_DRV_I2C_DEBUG_DUMP
#define i2c_dump(__fmt__, ...)			\
	wmprintf(__fmt__"\n\r", ##__VA_ARGS__)
#else
#define i2c_dump(...)
#endif /* CONFIG_DRV_I2C_DEBUG_DUMP */

#define i2c_e(...)				\
	wmlog_e("i2c", ##__VA_ARGS__)
#define i2c_w(...)				\
	wmlog_w("i2c", ##__VA_ARGS__)

#ifdef CONFIG_I2C_DEBUG
#define i2c_d(...)				\
	wmlog("i2c", ##__VA_ARGS__)
#else
#define i2c_d(...)
#endif				/* ! CONFIG_I2C_DEBUG */

/* #define CONFIG_I2C_FUNC_DEBUG */

#ifdef CONFIG_I2C_FUNC_DEBUG
#define i2c_entry wmlog_entry
#define i2c_exit wmlog_exit
#else
#define i2c_entry(...)
#define i2c_exit(...)
#endif				/* ! CONFIG_I2C_DEBUG */


#define DMA_MAX_BLK_SIZE 1023

/* Fixme: dynamic DMA channel allocation */
#define I2C_RX_CHANNEL CHANNEL_0
#define I2C_TX_CHANNEL CHANNEL_1

#define NUM_MC200_I2C_PORTS 3
#define I2C_INT_BASE INT_I2C0
#define I2C_IRQn_BASE I2C0_IRQn
#define I2C_RX_BUF_SIZE 128
#define SEMAPHORE_NAME_LEN 30

/* HIGH_TIME & LOW_TIME values are set using
 * MC200 datasheet- I2C_CLK frequency configuration
 */
#define MIN_SCL_HIGH_TIME_STANDARD 4000	/* 4000ns for 100KHz */
#define MIN_SCL_LOW_TIME_STANDARD  4700	/* 4700ns for 100KHz */
#define MIN_SCL_HIGH_TIME_FAST 600	/* 600ns for 400KHz  */
#define MIN_SCL_LOW_TIME_FAST 1300	/* 1300ns for 400KHz */

/* I2C Module Clock has to be less than 100Mhz. However set to 66Mhz
 * as there is at least one sensor that does not work with 100MHz.
 */
#define I2C_MOD_CLK_DIV 3
#define i2c_mod_freq_MHz() (board_cpu_freq()/1000000/I2C_MOD_CLK_DIV)
#define os_get_timestamp_msec() (os_get_timestamp()/1000)
/* SLAVE_WRITE_TIMEOUT: Timeout for the thread to wait on RD_REQ in slave mode
 */
#define SLAVE_WRITE_TIMEOUT 100

/* HCNT & LCNT initialized to min required count */
/* values for standard speed 100KHz*/
static int STANDARD_LCNT = 8;
static int STANDARD_HCNT = 6;
/* values for fast speed 400KHz*/
static int FAST_LCNT = 8;
static int FAST_HCNT = 6;
/* values for High speed 3.4MHz*/
/* Note: High speed is currently not supported */
static int HS_LCNT = 8;
static int HS_HCNT = 6;

static bool write_cmd_enabled;
static bool is_clkcnt_set = RESET;
#define SLAVE_WRITE_TIMEOUT 100
/* The device objects */
static mdev_t mdev_i2c[NUM_MC200_I2C_PORTS];

#define RELEASE_SCL true
#define HOLD_SCL false

#define MAX_FIFO_EMPTYING_TIME 1000

I2C_DmaReqLevel_Type i2cDmaCfg = { 0, 0 };

/* Device name */
static const char *mdev_i2c_name[NUM_MC200_I2C_PORTS] = {
	"MDEV_I2C0",
	"MDEV_I2C1",
	"MDEV_I2C2"
};

typedef struct i2cdrv_ringbuf {
	int wr_idx;		/* write pointer */
	int rd_idx;		/* read pointer */
	int buf_size;		/* number of bytes in buffer *buf_p */
	uint8_t *buf_p;		/* pointer to buffer storage */
} i2c_ringbuf_t;

typedef struct i2cdrv_clkcnt {
	int high_time;
	int low_time;
} i2c_drv_clkcnt;

typedef struct mdev_i2c_priv_data_ {
	i2c_ringbuf_t rx_ringbuf;	/* receive buffer structure */
	I2C_CFG_Type i2c_cfg;
	i2c_drv_clkcnt i2c_clkcnt;

	/** Event callback to application */
	i2c_int_cb event_cb;

	/** user's private pointer that is passed as
	 * a parameter to the callback function
	 */
	void *data_cb;

	int dma:1;
	/*
	 * Below two bits are used for sync between thread and ISR
	 * context.
	 */
	int read_req_pending:1;
	int tx_empty_event:1;
	int rx_done_event:1;
	int tx_abort_event:1;
	uint32_t tx_abort_cause;
	os_mutex_t i2c_mutex;
	os_semaphore_t write_slave_sem;
	/* Transmit timeout */
	uint32_t tx_tout;
	/* Receive timeout */
	uint32_t rx_tout;
	/*
	 * Used for blocking the calling thread till DMA transmit (form RAM
	 * to I2C peripheral) is complete.
	 */
	os_semaphore_t i2c_dma_tx_sem;
	/*
	 * Used for blocking the calling thread till DMA receive (from I2C
	 * peripheral to RAM) is complete.
	 */
	os_semaphore_t i2c_dma_rx_sem;
} mdev_i2c_priv_data_t;

I2C_FIFO_Type i2cFifoCfg = { 0,	/* i2c receive request level */
	0			/* i2c transmit request level */
};

/* Device private data */
static mdev_i2c_priv_data_t i2cdev_priv_data_[NUM_MC200_I2C_PORTS];

static void i2cdrv_tx_cb(int port_num);
static void i2cdrv_rx_cb(int port_num);

static void (*i2c_dma_tx_cb[NUM_MC200_I2C_PORTS]) ();
static void (*i2c_dma_rx_cb[NUM_MC200_I2C_PORTS]) ();

#define SET_RX_BUF_SIZE(i2c_data_p, size) i2c_data_p->rx_ringbuf.buf_size = size
#define GET_RX_BUF_SIZE(i2c_data_p) (i2c_data_p->rx_ringbuf.buf_size)

#define DEFINE_I2C_TX_CB(id) \
				static void \
				i2cdrv_tx ## id ## _cb() {\
				i2cdrv_tx_cb(id); }
#define GET_I2C_TX_CB(id) i2cdrv_tx ## id ## _cb

#define DEFINE_I2C_RX_CB(id) \
				static void \
				i2cdrv_rx ## id ## _cb() {\
				i2cdrv_rx_cb(id); }
#define GET_I2C_RX_CB(id) i2cdrv_rx ## id ## _cb

DEFINE_I2C_TX_CB(0)
DEFINE_I2C_TX_CB(1)
DEFINE_I2C_TX_CB(2)

DEFINE_I2C_RX_CB(0)
DEFINE_I2C_RX_CB(1)
DEFINE_I2C_RX_CB(2)

static void fill_cb_array()
{
	/* Interrupt handler: DMA_TX complete */
	i2c_dma_tx_cb[0] = GET_I2C_TX_CB(0);
	i2c_dma_tx_cb[1] = GET_I2C_TX_CB(1);
	i2c_dma_tx_cb[2] = GET_I2C_TX_CB(2);

	/* Interrupt handler: DMA_RX complete */
	i2c_dma_rx_cb[0] = GET_I2C_RX_CB(0);
	i2c_dma_rx_cb[1] = GET_I2C_RX_CB(1);
	i2c_dma_rx_cb[2] = GET_I2C_RX_CB(2);

}

/* ring buffer apis */
static int get_free_rx_buf_size(mdev_t *dev)
{
	int wr_ptr, rd_ptr, rx_ringbuf_size, free_space;
	mdev_i2c_priv_data_t *i2c_data_p;

	i2c_data_p = (mdev_i2c_priv_data_t *) dev->private_data;
	wr_ptr = i2c_data_p->rx_ringbuf.wr_idx;
	rd_ptr = i2c_data_p->rx_ringbuf.rd_idx;
	rx_ringbuf_size = GET_RX_BUF_SIZE(i2c_data_p);

	/* free space = { rd_ptr + (buf.size - wr_ptr } % rx_ringbuf_size
	 *  = (rd_ptr - wr_ptr) % rx_ringbuf_size
	 */
	free_space = (rd_ptr - wr_ptr) % rx_ringbuf_size;
	return free_space;
}

static uint8_t read_rx_buf(mdev_i2c_priv_data_t *i2c_data_p)
{
	uint8_t ret;
	int idx;
	/* RX FIFO is written in ISR context. If the interrupt
	 * RX_FULL occurs while the application thread is running,
	 * corresponding ISR gets invoked.
	 * Data is pushed into rx-fifo; the number of data bytes
	 * available in the rx-fifo is greater than or equal to the value
	 * returned by the ring buffer APIs. Thus critical section can be
	 * omitted.
	 */
	idx = i2c_data_p->rx_ringbuf.rd_idx;
	ret = i2c_data_p->rx_ringbuf.buf_p[idx];
	i2c_data_p->rx_ringbuf.rd_idx = (i2c_data_p->rx_ringbuf.rd_idx + 1)
	    % GET_RX_BUF_SIZE(i2c_data_p);
	return ret;
}

static void write_rx_buf(mdev_i2c_priv_data_t *i2c_data_p, uint8_t wr_char)
{
	int idx;
	idx = i2c_data_p->rx_ringbuf.wr_idx;
	i2c_data_p->rx_ringbuf.buf_p[idx] = wr_char;
	i2c_data_p->rx_ringbuf.wr_idx = (i2c_data_p->rx_ringbuf.wr_idx + 1)
	    % GET_RX_BUF_SIZE(i2c_data_p);
}

static int rx_buf_init(mdev_i2c_priv_data_t *i2c_data_p)
{
	i2c_data_p->rx_ringbuf.rd_idx = 0;
	i2c_data_p->rx_ringbuf.wr_idx = 0;
	i2c_data_p->rx_ringbuf.buf_p = os_mem_alloc
	    (GET_RX_BUF_SIZE(i2c_data_p));
	if (!i2c_data_p->rx_ringbuf.buf_p)
		return -WM_FAIL;
	return WM_SUCCESS;
}

int i2c_drv_enable(mdev_t *dev)
{
	if (mdev_get_handle(mdev_i2c_name[dev->port_id]) == NULL) {
		i2c_w("trying to enable unregistered device");
		return -WM_FAIL;
	}
	I2C_Enable(dev->port_id);

	return WM_SUCCESS;
}

int i2c_drv_disable(mdev_t *dev)
{
	if (mdev_get_handle(mdev_i2c_name[dev->port_id]) == NULL) {
		i2c_w("trying to disable unregistered device");
		return -WM_FAIL;
	}

	I2C_Disable(dev->port_id);

	return WM_SUCCESS;
}

static void i2cdrv_rx_cb(int port_id)
{
	mdev_t *mdev_p = mdev_get_handle(mdev_i2c_name[port_id]);
	mdev_i2c_priv_data_t *priv_data_p =
		(mdev_i2c_priv_data_t *) mdev_p->private_data;
	if (!mdev_p) {
		ll_printf("Unable to find device %s", mdev_i2c_name[port_id]);
		return;
	}

	os_semaphore_put(&priv_data_p->i2c_dma_rx_sem);
}

static void i2cdrv_tx_cb(int port_id)
{
	mdev_t *mdev_p = mdev_get_handle(mdev_i2c_name[port_id]);
	mdev_i2c_priv_data_t *priv_data_p =
		(mdev_i2c_priv_data_t *) mdev_p->private_data;
	if (!mdev_p) {
		ll_printf("Unable to find device %s", mdev_i2c_name[port_id]);
		return;
	}

	os_semaphore_put(&priv_data_p->i2c_dma_tx_sem);
}

static void dma_error_cb()
{
	/* FIXME: Use a common debug macro */
	ll_printf("error in I2C DMA transfer\r\n");
}

static void i2c_dma_rx_config(mdev_t *dev, uint32_t addr)
{
	DMA_CFG_Type dma;
	switch (dev->port_id) {
	case I2C0_PORT:
		dma.srcDmaAddr = (uint32_t) &I2C0->DATA_CMD.WORDVAL;
		dma.srcHwHdskInterf = DMA_HW_HS_INTER_14;
		/* Destination hardware interface not
		 * required as destination is memory */
		DMA_SetHandshakingMapping(DMA_HS14_I2C0_RX);
		break;
	case I2C1_PORT:
		dma.srcDmaAddr = (uint32_t) &I2C1->DATA_CMD.WORDVAL;
		dma.srcHwHdskInterf = DMA_HW_HS_INTER_6;
		/* Destination hardware interface not
		 * required as destination is memory */
		DMA_SetHandshakingMapping(DMA_HS6_I2C1_RX);
		break;
	case I2C2_PORT:
		dma.srcDmaAddr = (uint32_t) &I2C2->DATA_CMD.WORDVAL;
		dma.srcHwHdskInterf = DMA_HW_HS_INTER_8;
		/* Destination hardware interface not
		 * required as destination is memory */
		DMA_SetHandshakingMapping(DMA_HS8_I2C2_RX);
		break;
	}

	/* destination buffer address */
	dma.destDmaAddr = addr;

	/* Peripheral to memory */
	dma.transfType = DMA_PER_TO_MEM;

	/* Source burst length: 1 item */
	dma.srcBurstLength = DMA_ITEM_1;
	 /* destination burst length:1 item */
	dma.destBurstLength = DMA_ITEM_1;

	/* Src address no change (I2C peripheral) */
	dma.srcAddrInc = DMA_ADDR_NOCHANGE;

	/* Destination address(RAM/App buffer address) increment */
	dma.destAddrInc = DMA_ADDR_INC;

	/* Hardware handshaking interface*/
	dma.srcSwHwHdskSel = DMA_HW_HANDSHAKING;

	/* Channel priority level 0 */
	dma.channelPriority = DMA_CH_PRIORITY_0;

	/* FIFO mode 0: Immediate data transfer from DMA FIFO */
	dma.fifoMode = DMA_FIFO_MODE_0;

	dma.srcTransfWidth   = DMA_TRANSF_WIDTH_8;
	dma.destTransfWidth  = DMA_TRANSF_WIDTH_8;

	/* Enable(unmask) INT_BLK_TRANS_COMPLETE interrupt */
	DMA_IntMask(I2C_RX_CHANNEL, INT_BLK_TRANS_COMPLETE, UNMASK);
	/* Enable(unmask) INT_ERROR interrupt */
	DMA_IntMask(I2C_RX_CHANNEL, INT_ERROR, UNMASK);

	/* Register callbacks for unmasked DMA interrupts */
	install_int_callback(INT_DMA_CH0, INT_BLK_TRANS_COMPLETE,
			     i2c_dma_rx_cb[dev->port_id]);
	install_int_callback(INT_DMA_CH0, INT_ERROR, dma_error_cb);

	/* Enable DMA interrupts at NVIC level */
	NVIC_EnableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, 0xf);

	/* I2C FIFO configuration to trigger DMA transfer */
	I2C_DmaConfig((I2C_ID_Type) dev->port_id, &i2cDmaCfg);

	/* Enable DMA for I2C TX & RX mode */
	I2C_DmaCmd((I2C_ID_Type)dev->port_id, ENABLE, ENABLE);

	/* Disable channel before configuring it*/
	DMA_ChannelCmd(I2C_RX_CHANNEL, DISABLE);
	/* Fixme: set block transfer size to max */
	/* Setup DMA channel configuration in DMA registers */
	DMA_ChannelInit(I2C_RX_CHANNEL, &dma);
}

static int i2c_drv_slave_dma_read(mdev_t *dev, uint8_t *buf, uint32_t nbytes)
{
	/* Fixme: Take mutex */
	DMA_CFG_Type dma_rx;
	uint16_t dma_trans = DMA_MAX_BLK_SIZE;
	int num = nbytes;
	uint32_t cnt;

	mdev_i2c_priv_data_t *priv_data_p =
		(mdev_i2c_priv_data_t *) dev->private_data;
	unsigned long sem_timeout = (priv_data_p->rx_tout) ?
		priv_data_p->rx_tout : OS_WAIT_FOREVER;

	i2c_dma_rx_config(dev, (uint32_t)buf);
	while (num > 0) {

		if (num < DMA_MAX_BLK_SIZE)
			dma_trans = num;

		DMA_ChannelCmd(I2C_RX_CHANNEL, DISABLE);
		DMA_ChannelInit(I2C_RX_CHANNEL, &dma_rx);
		DMA_IntClr(I2C_RX_CHANNEL, INT_CH_ALL);
		DMA_IntMask(I2C_RX_CHANNEL, INT_BLK_TRANS_COMPLETE, UNMASK);
		DMA_SetBlkTransfSize(I2C_RX_CHANNEL, dma_trans);
		DMA_Enable();

		/* Wait till data is received in the RX FIFO */
		while (I2C_GetStatus(dev->port_id, I2C_STATUS_RFNE) != SET)
			;

		DMA_ChannelCmd(I2C_RX_CHANNEL, ENABLE);

		/* Wait on semaphore i2c_dma_rx_sem i.e till requested
		 * number of bytes are transferred to memory form I2C
		 * peripheral.
		 */
		int sem_status = os_semaphore_get(&priv_data_p->i2c_dma_rx_sem,
						  sem_timeout);

		if (sem_status != WM_SUCCESS)
			return -WM_FAIL;
		dma_rx.destDmaAddr += dma_trans;
		num = num - dma_trans;
		cnt++;
	}
	return nbytes;
}

static void i2c_dma_tx_config(mdev_t *dev, uint32_t addr)
{
	DMA_CFG_Type dma;
	switch (dev->port_id) {
	case I2C0_PORT:
		dma.destDmaAddr = (uint32_t) &I2C0->DATA_CMD.WORDVAL;
		dma.destHwHdskInterf = DMA_HW_HS_INTER_15;
		/* Source hardware interface not
		 * required as source is memory */
		DMA_SetHandshakingMapping(DMA_HS15_I2C0_TX);
		break;
	case I2C1_PORT:
		dma.destDmaAddr = (uint32_t) &I2C1->DATA_CMD.WORDVAL;
		dma.destHwHdskInterf = DMA_HW_HS_INTER_12;
		/* Source hardware interface not
		 * required as source is memory */
		DMA_SetHandshakingMapping(DMA_HS12_I2C1_TX);
		break;
	case I2C2_PORT:
		dma.destDmaAddr = (uint32_t) &I2C2->DATA_CMD.WORDVAL;
		dma.destHwHdskInterf = DMA_HW_HS_INTER_13;
		/* Source hardware interface not
		 * required as source is memory */
		DMA_SetHandshakingMapping(DMA_HS13_I2C2_TX);
		break;
	}
		/* DMA controller peripheral to memory configuration */
		/* Source buffer address */
		dma.srcDmaAddr = addr;

		/* Memory to Peripheral */
		dma.transfType = DMA_MEM_TO_PER;

		/* Source burst length: 1 item */
		dma.srcBurstLength = DMA_ITEM_1;
		/* destination burst length:1 item */
		dma.destBurstLength = DMA_ITEM_1;

		/* Source address(RAM/App buffer address) increment */
		dma.srcAddrInc = DMA_ADDR_INC;

		/* Destination address no change (I2C peripheral) */
		dma.destAddrInc = DMA_ADDR_NOCHANGE;

		/* Hardware handshaking interface*/
		dma.srcSwHwHdskSel = DMA_HW_HANDSHAKING;

		/* Fixme: check if this is really required */
		dma.destSwHwHdskSel = DMA_HW_HANDSHAKING;
		dma.srcHwHdskInterf = DMA_HW_HS_INTER_0;

		/* Channel priority level 0 */
		dma.channelPriority = DMA_CH_PRIORITY_0;

		/* FIFO mode 0: Immediate data transfer from DMA FIFO */
		dma.fifoMode = DMA_FIFO_MODE_0;

		if (write_cmd_enabled) {
			/* Src & Destination transfer width 16 bits required
			 * to send RD_REQ to slave when in master mode */
			dma.srcTransfWidth   = DMA_TRANSF_WIDTH_16;
			dma.destTransfWidth  = DMA_TRANSF_WIDTH_16;
		} else {
			/* Src & Destination transfer width 8 bits */
			dma.srcTransfWidth = DMA_TRANSF_WIDTH_8;
			dma.destTransfWidth = DMA_TRANSF_WIDTH_8;
		}

		/* Clear all DMA interrupts */
		DMA_IntClr(I2C_TX_CHANNEL, INT_CH_ALL);

		/* Enable(unmask) INT_BLK_TRANS_COMPLETE interrupt */
		DMA_IntMask(I2C_TX_CHANNEL, INT_BLK_TRANS_COMPLETE, UNMASK);

		/* Enable(unmask) INT_ERROR interrupt */
		DMA_IntMask(I2C_TX_CHANNEL, INT_ERROR, UNMASK);

		/* Register callbacks for unmasked DMA interrupts */
		install_int_callback(INT_DMA_CH1, INT_BLK_TRANS_COMPLETE,
				i2c_dma_tx_cb[dev->port_id]);

		install_int_callback(INT_DMA_CH0, INT_ERROR, dma_error_cb);

		/* Enable DMA interrupts at NVIC level */
		NVIC_EnableIRQ(DMA_IRQn);
		NVIC_SetPriority(DMA_IRQn, 0xf);

		/* I2C FIFO configuration to trigger DMA transfer */
		I2C_DmaConfig((I2C_ID_Type)dev->port_id, &i2cDmaCfg);

		/* Enable DMA for I2C TX & RX mode */
		I2C_DmaCmd((I2C_ID_Type)dev->port_id, ENABLE, ENABLE);

		/* Disable channel before channel Init */
		DMA_ChannelCmd(I2C_TX_CHANNEL, DISABLE);
		DMA_SetBlkTransfSize(I2C_TX_CHANNEL, DMA_MAX_BLK_SIZE);
		DMA_ChannelInit(I2C_TX_CHANNEL, &dma);

}

int i2c_drv_dma_write(mdev_t *dev, const uint8_t *buf, uint32_t nbytes)
{
	/* Fixme: Take mutex */
	uint16_t dma_trans = DMA_MAX_BLK_SIZE;
	int num = nbytes;
	i2c_dma_tx_config(dev, (uint32_t) buf);

	I2C_CFG_Type *i2c_cfg;
	mdev_i2c_priv_data_t *priv_data_p;
	priv_data_p = (mdev_i2c_priv_data_t *) dev->private_data;
	i2c_cfg = &priv_data_p->i2c_cfg;

	unsigned long sem_timeout = (priv_data_p->tx_tout) ?
		priv_data_p->tx_tout : OS_WAIT_FOREVER;

	while (num > 0) {
		DMA_IntClr(I2C_TX_CHANNEL, INT_CH_ALL);
		if (num < DMA_MAX_BLK_SIZE) {
			dma_trans = num;
			DMA_SetBlkTransfSize(I2C_TX_CHANNEL, dma_trans);
		}
		/* for slave mode wait on semaphore write_slave_sem
		   for write request  from master */
		if (i2c_cfg->masterSlaveMode == I2C_SLAVE) {
			os_semaphore_get(&priv_data_p->write_slave_sem,
					 sem_timeout);
		}

		DMA_ChannelCmd(I2C_TX_CHANNEL, ENABLE);

		/* Wait on semaphore i2c_dma_tx_sem for DMA trasfer to
		   complete */
		int sem_status;
		sem_status = os_semaphore_get(&priv_data_p->i2c_dma_tx_sem,
					      sem_timeout);

		if (sem_status != WM_SUCCESS)
			return -WM_FAIL;

		buf += dma_trans;
		num -= dma_trans;
		if (num > 0)
			DMA_SetSourceAddr(I2C_TX_CHANNEL, (uint32_t)buf);
	}
	return nbytes;
}

static int i2c_drv_master_dma_read(mdev_t *dev, void *buf, uint32_t nbytes)
{
	/* Fixme: Take mutex */

	DMA_CFG_Type dma_rx;
	uint16_t dma_trans = DMA_MAX_BLK_SIZE;
	int num = nbytes;
	uint32_t cnt = 0;
	int i;
	mdev_i2c_priv_data_t *priv_data_p =
		(mdev_i2c_priv_data_t *) dev->private_data;

	/* check if timeout is set */
	unsigned long sem_timeout = (priv_data_p->tx_tout) ?
		priv_data_p->tx_tout : OS_WAIT_FOREVER;

	/* Set DMA RX channel configuration */
	i2c_dma_rx_config(dev, (uint32_t)buf);

	/* cmd_buf: transmit buffer to generate read request */
	/* Fixme: allocate from heap and deallocate at the end
	 Allocate only DMA_MAX_BLK_SIZE bytes and reuse it later */
	uint16_t cmd_buf[nbytes];
	for (i = 0; i < nbytes+1; i++)
		cmd_buf[i] = 0x100;

	while (num > 0) {
		DMA_ChannelCmd(I2C_RX_CHANNEL, DISABLE);
		if (num < DMA_MAX_BLK_SIZE)
			dma_trans = num;
		DMA_IntClr(I2C_RX_CHANNEL, INT_CH_ALL);
		DMA_SetBlkTransfSize(I2C_RX_CHANNEL, dma_trans);
		DMA_ChannelCmd(I2C_RX_CHANNEL, ENABLE);
		write_cmd_enabled = 1; /* enable 16 bit transfer */
		/* send RD_REQ to slave */
		i2c_drv_dma_write(dev, (uint8_t *) cmd_buf, dma_trans);

		write_cmd_enabled = 0;

		/* Wait on semaphore i2c_dma_rx_sem i.e till requested
		 * number of bytes are transferred to memory form I2C
		 * peripheral.
		 */
		int sem_status = os_semaphore_get(&priv_data_p->i2c_dma_rx_sem,
						  sem_timeout);
			if (sem_status != WM_SUCCESS)
					return -WM_FAIL;
			/* Fixme: use DMADestSrcAddr() */
			dma_rx.destDmaAddr += dma_trans;
			num = num - dma_trans;
			cnt++;
	}
	return nbytes;

}

static int i2c_drv_pio_read(mdev_t *dev, uint8_t *buf, uint32_t nbytes)
{
	int len = 0;
	uint32_t recvlength = 0, txlength = 0;
	I2C_CFG_Type *i2c_cfg;
	mdev_i2c_priv_data_t *priv_data_p;
	priv_data_p = (mdev_i2c_priv_data_t *) dev->private_data;
	i2c_cfg = &priv_data_p->i2c_cfg;
	uint32_t t1 = os_get_timestamp(), t2, tout =
		priv_data_p->rx_tout * 1000;

	if (i2c_cfg->masterSlaveMode == I2C_MASTER) {
		os_mutex_get(&priv_data_p->i2c_mutex, OS_WAIT_FOREVER);
		/*
		 * Note: I2C generates stop condition as soon as its tx fifo
		 * becomes empty (pops out last element from tx fifo), and
		 * hence for multi-byte read make sure to keep tx fifo full
		 * for amount of bytes to read (acknowledge) from slave
		 * device.
		 */

		while (recvlength < nbytes) {
			if ((SET ==
			     I2C_GetStatus(dev->port_id, I2C_STATUS_TFNF))
			    && txlength < nbytes) {
				I2C_MasterReadCmd(dev->port_id);
				txlength++;
			}
			while ((SET ==
				I2C_GetStatus(dev->port_id, I2C_STATUS_RFNE))
			       && recvlength < nbytes) {
				buf[recvlength] =
				    (uint8_t) I2C_ReceiveByte(dev->port_id);
				recvlength++;
				t1 = os_get_timestamp();
			}
			if (tout) {
				t2 = os_get_timestamp();
				if ((((t2 > t1) && ((t2 - t1) > tout)) ||
					((t1 > t2) &&
					(UINT_MAX - t1 + t2) > tout))) {
					recvlength = -WM_E_AGAIN;
					i2c_d("rx timeout");
					break;
				}
			}
		}
		os_mutex_put(&priv_data_p->i2c_mutex);
		return recvlength;
	} else {
		while (nbytes > 0) {
			/* Read data if rx buf is non empty */
			if (get_free_rx_buf_size(dev) != 0) {
				*buf = (uint8_t) read_rx_buf(priv_data_p);
			} else if (I2C_GetStatus(dev->port_id, I2C_STATUS_RFNE)
				   == SET) {
				*buf = (uint8_t) I2C_ReceiveByte(dev->port_id);
			} else
				break;
			nbytes--;
			len++;
			buf++;
		}
		return len;
	}
}

int i2c_drv_read(mdev_t *dev, void *buf, uint32_t nbytes)
{
	i2c_entry("buf: %p cnt: %d", buf, nbytes);
	mdev_i2c_priv_data_t *p_data = (mdev_i2c_priv_data_t *)
	    dev->private_data;

	/* check if dma is enabled */
	if (p_data->dma == I2C_DMA_ENABLE) {

		/* check I2C device mode */
		I2C_CFG_Type *i2c_cfg;
		i2c_cfg = &p_data->i2c_cfg;
		int slv_mode = i2c_cfg->masterSlaveMode;
		if (slv_mode)
			return i2c_drv_slave_dma_read(dev, buf, nbytes);
		else
			return i2c_drv_master_dma_read(dev, buf, nbytes);
	}

	else
		return i2c_drv_pio_read(dev, buf, nbytes);

}

static int _i2c_drv_pio_slave_write(mdev_t *dev,
				    mdev_i2c_priv_data_t *priv_data_p,
				    const uint8_t *buf, uint32_t nbytes)
{
	int test_clr_rd_req_cnt = 0;
	int ret;
	uint32_t sentlength = 0;
	unsigned long sem_timeout = (priv_data_p->tx_tout) ?
		priv_data_p->tx_tout : OS_WAIT_FOREVER;

	ret = os_semaphore_get(&priv_data_p->write_slave_sem, sem_timeout);
	if (ret != WM_SUCCESS)
		return 0; /* sent bytes = 0 */

	/* Start writing */
	while (sentlength < nbytes) {
		if (SET == I2C_GetStatus(dev->port_id, I2C_STATUS_TFNF)) {
			/* Keep writing till transmit FIFO is full */
			I2C_SendByte(dev->port_id, buf[sentlength]);
			if (priv_data_p->read_req_pending) {
				priv_data_p->read_req_pending = 0;
				/* Dummy read to clear bit */
				I2C_IntClr(dev->port_id, I2C_INT_RD_REQ);
				I2C_IntMask(dev->port_id,
					    I2C_INT_RD_REQ, UNMASK);
				test_clr_rd_req_cnt++;
			}

			sentlength++;
		} else {
			/*
			 * Transmit FIFO is full. Lets sleep till lower
			 * watermark is reached. i2cdrv_tx_empty_handler()
			 * will wake us up.
			 */
			i2c_dump("Tx-FIFO Full");
			if (priv_data_p->rx_done_event) {
				/* The master does not want more data */
				i2c_dump("Got RX_DONE event");
				priv_data_p->rx_done_event = false;
				break;
			}
			/* Enable the FIFO lower threshold breach ISR */
			I2C_IntMask(dev->port_id, I2C_INT_TX_EMPTY, UNMASK);
			ret = os_semaphore_get(&priv_data_p->write_slave_sem,
					      MAX_FIFO_EMPTYING_TIME);
			if (ret != WM_SUCCESS) {
				/* Something bad has happened on the I2C bus */
				/* fixme: I2C slave reset ? */
				goto slave_write_exit;
			}

			priv_data_p->tx_empty_event = 0;
			/*
			 * Continue filling the FIFO now that lower
			 * watermark is reached
			 */
		}
	}

	i2c_dump("Wrote %d bytes. Clr req: %d",
		 sentlength, test_clr_rd_req_cnt);
 slave_write_exit:
	return sentlength;
}

static void reset_priv_data(mdev_i2c_priv_data_t *priv_data_p)
{
	/* Reset tx_abort_event */
	priv_data_p->tx_abort_event = 0;

	/* Reset tx_abort_cause */
	priv_data_p->tx_abort_cause = 0;
}

static int i2c_drv_pio_write(mdev_t *dev, const uint8_t *buf, uint32_t nbytes)
{
	uint32_t sentlength = 0;
	I2C_CFG_Type *i2c_cfg;
	mdev_i2c_priv_data_t *priv_data_p;
	priv_data_p = (mdev_i2c_priv_data_t *) dev->private_data;
	i2c_cfg = &priv_data_p->i2c_cfg;
	uint32_t t1 = os_get_timestamp(), t2,
		tout = priv_data_p->tx_tout * 1000;
	reset_priv_data(priv_data_p);
	if (i2c_cfg->masterSlaveMode == I2C_MASTER) {
		os_mutex_get(&priv_data_p->i2c_mutex, OS_WAIT_FOREVER);
		while (sentlength < nbytes) {
			if (SET == I2C_GetStatus(dev->port_id,
						I2C_STATUS_TFNF)) {
				I2C_SendByte(dev->port_id, buf[sentlength]);
				/* Check if transfer is not aborted */
				if (priv_data_p->tx_abort_event) {
					priv_data_p->tx_abort_event = 0;
					os_mutex_put(&priv_data_p->i2c_mutex);
					return -WM_FAIL;
				}
				sentlength++;
				t1 = os_get_timestamp();
			} else if (tout) {
				t2 = os_get_timestamp();
				if ((((t2 > t1) && ((t2 - t1) > tout)) ||
					((t1 > t2) &&
					(UINT_MAX - t1 + t2) > tout))) {
					sentlength = -WM_E_AGAIN;
					i2c_d("tx timeout");
					break;
				}
			}
		}
		os_mutex_put(&priv_data_p->i2c_mutex);
	} else {
		sentlength = _i2c_drv_pio_slave_write(dev, priv_data_p,
						      buf, nbytes);
	}
	return sentlength;
}

int i2c_drv_write(mdev_t *dev, const void *buf, uint32_t nbytes)
{
	i2c_entry("buf = %x cnt = %d", buf, nbytes);
	mdev_i2c_priv_data_t *p_data = (mdev_i2c_priv_data_t *)
	    dev->private_data;

	/* check if dma is enabled */
	if (p_data->dma == I2C_DMA_ENABLE)
		return i2c_drv_dma_write(dev, buf, nbytes);

	else
		return i2c_drv_pio_write(dev, buf, nbytes);
}

int i2c_drv_get_status_bitmap(mdev_t *mdev_p, uint32_t *status)
{
	if (status == NULL)
		return -WM_E_INVAL;

	if (mdev_p == NULL)
		return -WM_E_INVAL;

	uint32_t port_no = mdev_p->port_id;

	/* check if I2C port is configured as a master or slave */
	mdev_i2c_priv_data_t *p_data;

	p_data = (mdev_i2c_priv_data_t *) mdev_p->private_data;

	I2C_CFG_Type *i2c_cfg;

	i2c_cfg = &p_data->i2c_cfg;

	int mode = i2c_cfg->masterSlaveMode;

	*status = 0;

	if (p_data->tx_abort_event) {
		i2c_e("TX_ABORT cause %d", p_data->tx_abort_cause);
		*status = I2C_ERROR;
		p_data->tx_abort_event = 0;
		return WM_SUCCESS;
	}

	/* if i2c slave mode check for slave active bit */
	if (mode) {
		if (I2C_GetStatus(port_no, I2C_STATUS_SLV_ACTIVITY))
			*status = I2C_ACTIVE;
		else
			*status = I2C_INACTIVE;

		/* if i2c master mode check for master active bit */
	} else if (I2C_GetStatus(port_no, I2C_STATUS_MST_ACTIVITY)) {
		*status = I2C_ACTIVE;
	}
	/* if none of the conditions checked above is true set I2C_INACTIVE */
	else
		*status = I2C_INACTIVE;

	return WM_SUCCESS;
}

/* Exposed APIs and helper functions */

int i2c_drv_xfer_mode(I2C_ID_Type i2c_id, I2C_DMA_MODE dma_mode)
{
	mdev_t *mdev_p;
	mdev_i2c_priv_data_t *p_data;

	if (i2c_id < 0 || i2c_id >= NUM_MC200_I2C_PORTS) {
		i2c_e("Port %d not present", i2c_id);
		return -WM_FAIL;
	}

	if (!(dma_mode == I2C_DMA_ENABLE || dma_mode == I2C_DMA_DISABLE)) {
		return -WM_FAIL;
	}

	mdev_p = mdev_get_handle(mdev_i2c_name[i2c_id]);

	if (!mdev_p) {
		i2c_e("Unable to open device %s", mdev_i2c_name[i2c_id]);
		return -WM_FAIL;
	}
	p_data = (mdev_i2c_priv_data_t *) mdev_p->private_data;
	p_data->dma = dma_mode;
	return WM_SUCCESS;

}


mdev_t *i2c_drv_open(I2C_ID_Type i2c_id, uint32_t flags)
{
	i2c_entry("flags: %p", flags);
	int mode;
	I2C_CFG_Type *i2c_cfg;
	mdev_i2c_priv_data_t *p_data;

	mdev_t *mdev_p = mdev_get_handle(mdev_i2c_name[i2c_id]);

	if (mdev_p == NULL) {
		i2c_w("driver open called without registering device"
		      " (%s)", mdev_i2c_name[i2c_id]);
		return NULL;
	}

	p_data = (mdev_i2c_priv_data_t *) mdev_p->private_data;
	i2c_cfg = &p_data->i2c_cfg;

	int dma_enabled = p_data->dma;
	/* Configure pinmux for i2c pins */
	board_i2c_pin_config(mdev_p->port_id);

	/* Disable i2c */
	I2C_Disable(mdev_p->port_id);

	if (flags & I2C_DEVICE_SLAVE) {
		/* config for slave device */
		mode = I2C_SLAVE;
		i2c_cfg->masterSlaveMode = I2C_SLAVE;
		i2c_cfg->ownSlaveAddr = flags & 0x0ff;

		/* Set the default bit mode to 7 bit mode */
		i2c_cfg->slaveAddrBitMode = I2C_ADDR_BITS_7;

		if (flags & I2C_10_BIT_ADDR)
			i2c_cfg->slaveAddrBitMode = I2C_ADDR_BITS_10;

	} else {

		/* config for master device */
		mode = I2C_MASTER;
		i2c_cfg->masterSlaveMode = I2C_MASTER;

		/* Set the default bit mode to 7 bit mode */
		i2c_cfg->masterAddrBitMode = I2C_ADDR_BITS_7;
		I2C_SetSlaveAddr(mdev_p->port_id, flags & 0x0ff);

		/* for 10 bit addressing mode (if enabled) */
		if (flags & I2C_10_BIT_ADDR) {
			i2c_cfg->masterAddrBitMode = I2C_ADDR_BITS_10;
			I2C_SetSlaveAddr(mdev_p->port_id, flags & 0x3ff);
		}

		I2C_IntMask(mdev_p->port_id, I2C_INT_ALL, MASK);
		I2C_IntMask(mdev_p->port_id, I2C_INT_TX_ABORT, UNMASK);

		NVIC_EnableIRQ(I2C_IRQn_BASE + mdev_p->port_id);
		NVIC_SetPriority(I2C_IRQn_BASE + mdev_p->port_id, 0xf);

	}

	i2c_cfg->restartEnable = ENABLE;

	/* Configure High Count and Low Count registers
	 * HCNT =  i2c_mod_freq_MHz * HIGH_TIME(corresponding to I2C speed)
	 * LCNT =  i2c_mod_freq_MHz * LOW_TIME(corresponding to I2C speed)
	 */
	int scl_high, scl_low;
	if (flags & I2C_CLK_400KHZ) {

		i2c_cfg->speedMode = I2C_SPEED_FAST;	/* 400KHz */

		/* check if user has tuned I2C clkcnt */
		if (is_clkcnt_set == SET) {
			scl_high = p_data->i2c_clkcnt.high_time;
			scl_low =  p_data->i2c_clkcnt.low_time;
		} else {
			scl_high = MIN_SCL_HIGH_TIME_FAST;
			scl_low = MIN_SCL_LOW_TIME_FAST;
		}
		if ((i2c_mod_freq_MHz() * scl_high) / 1000 >
		    FAST_HCNT)
			FAST_HCNT = i2c_mod_freq_MHz() * scl_high / 1000;

		if ((i2c_mod_freq_MHz() * scl_low / 1000) > FAST_LCNT)
			FAST_LCNT = i2c_mod_freq_MHz() * scl_low / 1000;

	} else {

		i2c_cfg->speedMode = I2C_SPEED_STANDARD; /* 100KHz Default */
		scl_high = p_data->i2c_clkcnt.high_time;
		if ((i2c_mod_freq_MHz() * scl_high / 1000) >
		    STANDARD_HCNT)
			STANDARD_HCNT = i2c_mod_freq_MHz() * scl_high / 1000;

		scl_low = p_data->i2c_clkcnt.low_time;
		if ((i2c_mod_freq_MHz() * scl_low / 1000) >
		    STANDARD_LCNT)
			STANDARD_LCNT = i2c_mod_freq_MHz() * scl_low / 1000;
	}

	/* configure registers required to set I2C_CLK speed before I2C_Init */

	I2C_SpeedClkCnt_Type clnt = { STANDARD_HCNT, STANDARD_LCNT,
		FAST_HCNT, FAST_LCNT, HS_HCNT, HS_LCNT
	};

	I2C_SetClkCount(mdev_p->port_id, &clnt);

	/* I2C init */
	I2C_Init(mdev_p->port_id, i2c_cfg);
	/* Fifo config */
	I2C_FIFOConfig(mdev_p->port_id, &i2cFifoCfg);

	if (mode == I2C_SLAVE) {
		/* in slave mode, create a semaphore to wait on read
		 * request from master device */
		char sem_name[SEMAPHORE_NAME_LEN];
		snprintf(sem_name, sizeof(sem_name), "i2c-write-sem%u",
			 (unsigned int)(mdev_p->port_id));
		int ret = os_semaphore_create(&p_data->write_slave_sem,
					      sem_name);
		if (ret == -WM_FAIL) {
			i2c_e("Failed to create semaphore");
			return NULL;
		}
		os_semaphore_get(&p_data->write_slave_sem, OS_WAIT_FOREVER);

		/* if rx_buffer is not set by the user set it to default size */
		if (GET_RX_BUF_SIZE(p_data) == 0) {
			SET_RX_BUF_SIZE(p_data, I2C_RX_BUF_SIZE);
		}
		/* init ring buffer */
		if (rx_buf_init(p_data) != WM_SUCCESS) {
			i2c_e("Unable to allocate rx buffer");
			return NULL;
		}

		/* Mask all interrupts */
		I2C_IntMask(mdev_p->port_id, I2C_INT_ALL, MASK);

		/* Unmask I2C_INT_RD_REQ interrupt */

		I2C_IntMask(mdev_p->port_id, I2C_INT_RX_UNDER, UNMASK);
		I2C_IntMask(mdev_p->port_id, I2C_INT_RX_OVER, UNMASK);
		I2C_IntMask(mdev_p->port_id, I2C_INT_RX_FULL, UNMASK);
		I2C_IntMask(mdev_p->port_id, I2C_INT_TX_OVER, UNMASK);
		/*
		 * We are not unmasking I2C_INT_TX_EMPTY because it
		 * triggers whenever the FIFO is empty. It will trigger
		 * even if there is no request pending from master.
		 */
		I2C_IntMask(mdev_p->port_id, I2C_INT_RD_REQ, UNMASK);
		I2C_IntMask(mdev_p->port_id, I2C_INT_TX_ABORT, UNMASK);
		I2C_IntMask(mdev_p->port_id, I2C_INT_RX_DONE, UNMASK);
		/* I2C_IntMask(mdev_p->port_id, I2C_INT_ACTIVITY, UNMASK); */
		I2C_IntMask(mdev_p->port_id, I2C_INT_STOP_DET, UNMASK);
		I2C_IntMask(mdev_p->port_id, I2C_INT_START_DET, UNMASK);
		I2C_IntMask(mdev_p->port_id, I2C_INT_GEN_CALL, UNMASK);

		/* For PIO mode (non-DMA mode) in slave device
		 * install callback for RX_FULL */
		if (!dma_enabled) {
			/* Unmask I2C_INT_RD_REQ interrupt */
			I2C_IntMask(mdev_p->port_id, I2C_INT_RX_FULL, UNMASK);
		}
		/* Enable i2c module interrupt */
		NVIC_EnableIRQ(I2C_IRQn_BASE + mdev_p->port_id);
		NVIC_SetPriority(I2C_IRQn_BASE + mdev_p->port_id, 0xf);
	}

	if (dma_enabled) {
		char i2c_tx_sem_name[SEMAPHORE_NAME_LEN];
		snprintf(i2c_tx_sem_name, sizeof(i2c_tx_sem_name),
			 "i2c-dma-tx-sem%u", (unsigned int)(mdev_p->port_id));
		int ret = os_semaphore_create(&p_data->i2c_dma_tx_sem,
					      i2c_tx_sem_name);

		if (ret) {
			i2c_e("Failed to initialize semaphore");
			return NULL;
		}
		os_semaphore_get(&p_data->i2c_dma_tx_sem, OS_WAIT_FOREVER);

		char i2c_rx_sem_name[SEMAPHORE_NAME_LEN];
		snprintf(i2c_rx_sem_name,
			 sizeof(i2c_rx_sem_name),
			 "i2c-dma-rx-sem%u",
			 (unsigned int)mdev_p->port_id);
		ret = os_semaphore_create(&p_data->i2c_dma_rx_sem,
					  i2c_rx_sem_name);

		if (ret) {
			i2c_e("Failed to initialize semaphore");
			return NULL;
		}

		os_semaphore_get(&p_data->i2c_dma_rx_sem,
				 OS_WAIT_FOREVER);
		DMA_Enable();

	}

	/* Enable i2c */
	I2C_Enable(mdev_p->port_id);

	return mdev_p;
}

int i2c_drv_set_callback(mdev_t *dev, i2c_int_cb event_cb, void *data)
{
	if (!dev)
		return -WM_E_INVAL;

	mdev_i2c_priv_data_t *p_data =
		(mdev_i2c_priv_data_t *) dev->private_data;
	p_data->event_cb = event_cb;
	p_data->data_cb = data;
	return WM_SUCCESS;
}


int i2c_drv_set_address(mdev_t *dev, uint32_t flags)
{
	if (!dev)
		return -WM_E_INVAL;

	mdev_i2c_priv_data_t *p_data =
		(mdev_i2c_priv_data_t *) dev->private_data;

	if (flags & I2C_DEVICE_SLAVE ||
			p_data->i2c_cfg.masterSlaveMode == I2C_SLAVE) {
		i2c_e("%s API supported for master mode only", __func__);
		return -WM_FAIL;
	} else {
		/* wait till i2c is inactive */
		while (RESET == I2C_GetStatus(dev->port_id, I2C_STATUS_TFE) ||
			SET == I2C_GetStatus(dev->port_id,
					I2C_STATUS_MST_ACTIVITY))
			os_thread_relinquish();

		/* Set the default bit mode to 7 bit mode */
		p_data->i2c_cfg.masterAddrBitMode = I2C_ADDR_BITS_7;
		I2C_SetSlaveAddr(dev->port_id, flags & 0x0ff);

		/* for 10 bit addressing mode (if enabled) */
		if (flags & I2C_10_BIT_ADDR) {
			p_data->i2c_cfg.masterAddrBitMode = I2C_ADDR_BITS_10;
			I2C_SetSlaveAddr(dev->port_id, flags & 0x3ff);
		}
	}

	return WM_SUCCESS;
}

int i2c_drv_close(mdev_t *dev)
{
	i2c_entry();
	uint32_t status;
	mdev_i2c_priv_data_t *i2c_data_p =
	    (mdev_i2c_priv_data_t *) dev->private_data;

	if (mdev_get_handle(mdev_i2c_name[dev->port_id]) == NULL) {
		i2c_w("trying to close unregistered device");
		return -WM_FAIL;
	}

	while (1) {
		i2c_drv_get_status_bitmap(dev, &status);
		if (status != I2C_ACTIVE)
			break;
		os_thread_relinquish();
	}

	I2C_Disable(dev->port_id);
	NVIC_DisableIRQ(I2C_IRQn_BASE + dev->port_id);

	if (i2c_data_p->rx_ringbuf.buf_p) {
		os_mem_free(i2c_data_p->rx_ringbuf.buf_p);
		i2c_data_p->rx_ringbuf.buf_p = NULL;
		i2c_data_p->rx_ringbuf.buf_size = 0;
	}
	I2C_CFG_Type *i2c_cfg = &i2c_data_p->i2c_cfg;
	/* Delete semaphore if device is in slave mode */
	if (i2c_cfg->masterSlaveMode == I2C_SLAVE) {
		os_semaphore_delete(&i2c_data_p->write_slave_sem);
	}

	if (i2c_data_p->i2c_dma_rx_sem)
		os_semaphore_delete(&i2c_data_p->i2c_dma_rx_sem);

	if (i2c_data_p->i2c_dma_tx_sem)
		os_semaphore_delete(&i2c_data_p->i2c_dma_tx_sem);

	return os_mutex_put(&i2c_data_p->i2c_mutex);
}

void i2c_drv_deinit(I2C_ID_Type id)
{
	mdev_t *dev;
	/* Return if the port ID is invalid */
	if (id < 0 || id >= NUM_MC200_I2C_PORTS) {
		i2c_e("Port %d not enabled", id);
		return;
	}
	/* Return if the device was not registered */
	if ((dev = mdev_get_handle(mdev_i2c_name[id])) == NULL)
		return;

	mdev_i2c_priv_data_t *i2c_data_p =
		(mdev_i2c_priv_data_t *) dev->private_data;

	/* Delete mutex and de-register the device */
	os_mutex_delete(&i2c_data_p->i2c_mutex);
	/* Deregister mdev handle */
	mdev_deregister(dev->name);
}

static int i2c_drv_mdev_init(int port_num)
{
	mdev_t *mdev_p;
	mdev_i2c_priv_data_t *priv_data_p;

	mdev_p = &mdev_i2c[port_num];

	mdev_p->name = mdev_i2c_name[port_num];
	mdev_p->port_id = port_num;
	mdev_p->private_data = (uint32_t) &i2cdev_priv_data_[port_num];
	priv_data_p = (mdev_i2c_priv_data_t *) &i2cdev_priv_data_[port_num];
	priv_data_p->rx_ringbuf.buf_size = 0;
	/* clock configuration for I2C port */
	priv_data_p->i2c_clkcnt.high_time = MIN_SCL_HIGH_TIME_STANDARD;
	priv_data_p->i2c_clkcnt.low_time = MIN_SCL_LOW_TIME_STANDARD;

	int ret = os_mutex_create(&priv_data_p->i2c_mutex,
				  mdev_i2c_name[port_num],
				  OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Set default tx/rx timeout */
	priv_data_p->tx_tout = 0;
	priv_data_p->rx_tout = 0;

	return WM_SUCCESS;
}

void i2c_drv_set_clkcnt(I2C_ID_Type i2c_id, int hightime, int lowtime)
{
	mdev_i2c_priv_data_t *priv_data_p;
	priv_data_p = (mdev_i2c_priv_data_t *) &i2cdev_priv_data_[i2c_id];

	priv_data_p->i2c_clkcnt.high_time = hightime;
	priv_data_p->i2c_clkcnt.low_time = lowtime;
	is_clkcnt_set = SET;
}

int i2c_drv_rxbuf_size(I2C_ID_Type i2c_id, uint32_t size)
{

	mdev_t *mdev_p;
	mdev_i2c_priv_data_t *i2c_data_p;

	if (size < 0)
		return -WM_FAIL;

	/*check i2c_id is initialized */
	mdev_p = mdev_get_handle(mdev_i2c_name[i2c_id]);
	if (!mdev_p) {
		i2c_e("Unable to open device %s", mdev_i2c_name[i2c_id]);
		return -WM_FAIL;
	}
	i2c_data_p = (mdev_i2c_priv_data_t *) mdev_p->private_data;
	SET_RX_BUF_SIZE(i2c_data_p, size);
	return WM_SUCCESS;
}

void i2c_drv_timeout(I2C_ID_Type id, uint32_t txtout, uint32_t rxtout)
{
	mdev_t *mdev_p = mdev_get_handle(mdev_i2c_name[id]);
	mdev_i2c_priv_data_t *priv_data_p =
		(mdev_i2c_priv_data_t *) mdev_p->private_data;

	if (!mdev_p) {
		i2c_e("Unable to open device %s", mdev_i2c_name[id]);
		return;
	}

	priv_data_p->tx_tout = txtout;
	priv_data_p->rx_tout = rxtout;
}

int i2c_drv_init(I2C_ID_Type id)
{
	if (mdev_get_handle(mdev_i2c_name[id]) != NULL)
		return WM_SUCCESS;

	/* Initialize the i2c port's mdev structure */
	int ret = i2c_drv_mdev_init(id);
	if (ret != WM_SUCCESS)
		return ret;


	/* Enable I2Cx Clock */
	switch (id) {
	case 0:
		CLK_ModuleClkEnable(CLK_I2C0);
		CLK_ModuleClkDivider(CLK_I2C0, I2C_MOD_CLK_DIV);
		break;
	case 1:
		CLK_ModuleClkEnable(CLK_I2C1);
		CLK_ModuleClkDivider(CLK_I2C1, I2C_MOD_CLK_DIV);
		break;
	case 2:
		CLK_ModuleClkEnable(CLK_I2C2);
		CLK_ModuleClkDivider(CLK_I2C2, I2C_MOD_CLK_DIV);
		break;
	}

	/* Register the I2C device driver for this port */
	mdev_register(&mdev_i2c[id]);

	fill_cb_array();

	return WM_SUCCESS;
}

static void i2cdrv_rxdone_irq_handler(int port_num)
{
	mdev_t *dev = &mdev_i2c[port_num];
	mdev_i2c_priv_data_t *i2c_data_p =
		(mdev_i2c_priv_data_t *) dev->private_data;

	i2c_data_p->rx_done_event = true;

	if (i2c_data_p->event_cb)
		i2c_data_p->event_cb(I2C_INT_RX_DONE, i2c_data_p->data_cb);
}

static void i2cdrv_stop_irq_handler(int port_num)
{
	mdev_t *dev = &mdev_i2c[port_num];
	mdev_i2c_priv_data_t *i2c_data_p =
		(mdev_i2c_priv_data_t *) dev->private_data;

	if (i2c_data_p->event_cb)
		i2c_data_p->event_cb(I2C_INT_STOP_DET, i2c_data_p->data_cb);
}

static void i2cdrv_start_irq_handler(int port_num)
{
	/* Stub */
}

/*
 * This can be invoked multiple times in a single transaction if master
 * needs more data.
 */
static void i2cdrv_rx_full_handler(int port_num)
{
	uint32_t data;
	mdev_t *dev = &mdev_i2c[port_num];

	mdev_i2c_priv_data_t *i2c_data_p;
	i2c_data_p = (mdev_i2c_priv_data_t *) dev->private_data;

	if (i2c_data_p->event_cb)
		i2c_data_p->event_cb(I2C_INT_RX_FULL, i2c_data_p->data_cb);

	while (I2C_GetStatus(dev->port_id, I2C_STATUS_RFNE) == SET) {
		data = I2C_ReceiveByte(dev->port_id);
		/* Write rx-buf only if it is not full */
		if (get_free_rx_buf_size(dev) != 1)
			write_rx_buf(i2c_data_p, (uint8_t) data);
	}
}

static void i2cdrv_tx_empty_handler(int port_num)
{
	mdev_t *mdev_p = &mdev_i2c[port_num];
	mdev_i2c_priv_data_t *priv_data_p;
	priv_data_p = (mdev_i2c_priv_data_t *) mdev_p->private_data;

	/*
	 * This notification need NOT go to application. If the thread
	 * context has data it will be put in FIFO. If it does not and the
	 * master still needs more data a read request will be generated by
	 * hardware again to notify the application.
	 */
	/* Set for the benefit of the application */
	priv_data_p->tx_empty_event = 1;
	/* The application is waiting. Wake it up */
	os_semaphore_put(&priv_data_p->write_slave_sem);
}

static bool i2cdrv_read_req_handler(int port_num)
{
	mdev_t *mdev_p = &mdev_i2c[port_num];
	mdev_i2c_priv_data_t *priv_data_p;
	priv_data_p = (mdev_i2c_priv_data_t *) mdev_p->private_data;

	if (priv_data_p->event_cb) {
		priv_data_p->event_cb(I2C_INT_RD_REQ, priv_data_p->data_cb);
	}

	int dma_enabled = priv_data_p->dma;


	if (dma_enabled) {
		/* DMA sends out the data on RD_REQ */
		os_semaphore_put(&priv_data_p->write_slave_sem);
		return RELEASE_SCL;
	}

	/* Set for the benefit of the application */
	priv_data_p->read_req_pending = 1;
	/* Notify the thread context */
	os_semaphore_put(&priv_data_p->write_slave_sem);

	/*
	 * We need to hold the SCL line low till the thread context writes
	 * data to the FIFO. We have already given notification to the
	 * application. Till then lets hold the SCL line low and continue
	 * clock stretching.
	 */
	return HOLD_SCL;
}

static void i2c_drv_tx_abort_handler(int port_num)
{
	mdev_t *mdev_p = &mdev_i2c[port_num];
	mdev_i2c_priv_data_t *priv_data_p;
	priv_data_p = (mdev_i2c_priv_data_t *) mdev_p->private_data;

	/* tx_abort_event indicates, tx_abort
	 * interrupt has occurred */
	priv_data_p->tx_abort_event = 1;
	/* Get reason */
	I2C_GetTxAbortCause(port_num , &priv_data_p->tx_abort_cause);
}

static void i2c_drv_common_isr_handler(I2C_ID_Type i2c_no)
{
	uint32_t int_status;

	/* Read the interrupt status */
	int_status = I2C_GetFullIntStatus(i2c_no);

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_RX_UNDER)) {
		/* Stub */
		/* Clear the interrupt bit */
		I2C_IntClr(i2c_no, I2C_INT_RX_UNDER);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_RX_OVER)) {
		/* Stub */
		/* Clear the interrupt bit */
		I2C_IntClr(i2c_no, I2C_INT_RX_OVER);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_RX_FULL)) {
		i2cdrv_rx_full_handler(i2c_no);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_TX_OVER)) {
		/* Stub */
		/* Clear the interrupt bit */
		I2C_IntClr(i2c_no, I2C_INT_TX_OVER);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_RD_REQ)) {
		bool clr_int = i2cdrv_read_req_handler(i2c_no);
		if (clr_int) {
			/*
			 * Clear the interrupt bit. After this bit is
			 * cleared the SCL line will be released by the
			 * slave hardware. The master (not us) will give
			 * out clock signals and our OUT FIFO needs to have
			 * data at this point.
			 */
			I2C_IntClr(i2c_no, I2C_INT_RD_REQ);
		} else
			I2C_IntMask(i2c_no, I2C_INT_RD_REQ, MASK);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_TX_EMPTY)) {
		/* Stub */
		i2cdrv_tx_empty_handler(i2c_no);
		/*
		 * Mask the interrupt till the thread context specifically
		 * unmasks it again.
		 */
		I2C_IntMask(i2c_no, I2C_INT_TX_EMPTY, MASK);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_TX_ABORT)) {
		i2c_drv_tx_abort_handler(i2c_no);
		/* Clear the interrupt bit */
		I2C_IntClr(i2c_no, I2C_INT_TX_ABORT);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_RX_DONE)) {
		i2cdrv_rxdone_irq_handler(i2c_no);
		/* Clear the interrupt bit */
		I2C_IntClr(i2c_no, I2C_INT_RX_DONE);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_ACTIVITY)) {
		/* Stub */
		/* Clear the interrupt bit */
		I2C_IntClr(i2c_no, I2C_INT_ACTIVITY);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_STOP_DET)) {
		i2cdrv_stop_irq_handler(i2c_no);
		/* Clear the interrupt bit */
		I2C_IntClr(i2c_no, I2C_INT_STOP_DET);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_START_DET)) {
		i2cdrv_start_irq_handler(i2c_no);
		/* Clear the interrupt bit */
		I2C_IntClr(i2c_no, I2C_INT_START_DET);
	}

	if (IS_I2C_INTSTAT_SET(int_status, I2C_INT_GEN_CALL)) {
		/* Stub */
		/* Clear the interrupt bit */
		I2C_IntClr(i2c_no, I2C_INT_GEN_CALL);
	}
}

void I2C0_IRQHandler()
{
	i2c_drv_common_isr_handler(I2C0_PORT);
}

void I2C1_IRQHandler()
{
	i2c_drv_common_isr_handler(I2C1_PORT);
}

void I2C2_IRQHandler()
{
	i2c_drv_common_isr_handler(I2C2_PORT);
}

int i2c_drv_wait_till_inactivity(mdev_t *dev, uint32_t timeout_ms)
{
	if (!dev)
		return -WM_E_INVAL;

	bool inactive = false;
	uint64_t end_cnt = os_total_ticks_get() + os_msec_to_ticks(timeout_ms);

	while (1) {
		if (I2C_GetStatus(dev->port_id, I2C_STATUS_ACTIVITY) == RESET) {
			inactive = true;
			break;
		}

		if (timeout_ms == 0) {
			/* Caller has requested us to return immediately */
			break;
		}

		if (end_cnt <= os_total_ticks_get()) {
			/* Timeout is done. The controller is still active :( */
			break;
		}
	}

	if (inactive)
		return WM_SUCCESS;
	else
		return -WM_FAIL;
}
