/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

/*
 * mdev_uart.c: mdev driver for UART
 */
#include <mc200_gpio.h>
#include <mc200_clock.h>
#include <mc200_dma.h>
#include <mc200_pinmux.h>
#include <board.h>

#include <wmtypes.h>
#include <wmstdio.h>
#include <mdev_uart.h>
#include <wm_os.h>
#include <pwrmgr.h>
#define NUM_UART_PORTS 4
#define UART_IRQn_BASE UART0_IRQn
#define UART_INT_BASE	INT_UART0
#define UART_FIFO_DEPTH 16

#define dmaChannel_rx CHANNEL_0
#define dmaChannel_tx CHANNEL_1

#define DMA_MAX_BLK_SIZE 1023
#define SEMAPHORE_NAME_LEN 25

/* Device name */
static const char *mdev_uart_name[NUM_UART_PORTS] = {
	"MDEV_UART0",
	"MDEV_UART1",
#if (NUM_UART_PORTS > 2)
	 "MDEV_UART2",
#if (NUM_UART_PORTS > 3)
	"MDEV_UART3",
#endif
#endif
};

/* The device objects */
static mdev_t mdev_uart[NUM_UART_PORTS];

typedef struct uartdrv_ringbuf {
	int wr_idx;		/* write pointer */
	int rd_idx;		/* read pointer */
	int buf_size;		/* number of bytes in buffer *buf_p */
	int dma_block_size;     /* dma block size */
	char *buf_p;		/* pointer to buffer storage */
} uartdrv_ringbuf_t;

typedef struct uartdev_priv_data {
	uartdrv_ringbuf_t rx_ringbuf;
	bool mode;
	bool dma;
	bool is_read_blocking;
	uint32_t baudrate;
	uint32_t parity;
	uint32_t stopbits;
	flow_control_t flowctl;
} uartdev_data_t;

DMA_CFG_Type read_dma[NUM_UART_PORTS];

/* Device private data */
static uartdev_data_t uartdev_data[NUM_UART_PORTS];
static os_mutex_t uart_mutex[NUM_UART_PORTS];
static void (*uart_8bit_cb_p[NUM_UART_PORTS]) ();
static void (*uart_9bit_cb_p[NUM_UART_PORTS]) ();
static void (*uart_dma_rd_cb_p[NUM_UART_PORTS]) ();
static void (*uart_dma_wr_cb_p[NUM_UART_PORTS]) ();
static os_semaphore_t uart_block_read_sem[NUM_UART_PORTS];
static os_semaphore_t uart_block_dma_wr_sem[NUM_UART_PORTS];

#define SET_RX_BUF_SIZE(uart_data_p, size)	\
	 uart_data_p->rx_ringbuf.buf_size = size
#define GET_RX_BUF_SIZE(uart_data_p) (uart_data_p->rx_ringbuf.buf_size)

#define SET_DMA_BLK_SIZE(uart_data_p, size)	\
	 uart_data_p->rx_ringbuf.dma_block_size = size
#define GET_DMA_BLK_SIZE(uart_data_p) (uart_data_p->rx_ringbuf.dma_block_size)

#define DEFINE_UART_CB(id, mode)	\
					static void \
					uart ## id ## _ ## mode ## bit_cb() {\
					uart_ ## mode ## bit_cb(id); }
#define GET_UART_CB(id, mode)		uart ## id ## _ ## mode ## bit_cb

#define DEFINE_UART_DMA_RD_CB(id)	\
					static void \
					uart ## id ## dma_rd_cb() {\
					uart_dma_rd_cb(id); }
#define GET_UART_DMA_RD_CB(id)		uart ## id ## dma_rd_cb

#define DEFINE_UART_DMA_WR_CB(id)	\
					static void \
					uart ## id ## dma_wr_cb() {\
					uart_dma_wr_cb(id); }
#define GET_UART_DMA_WR_CB(id)		uart ## id ## dma_wr_cb

void uart_ps_cb(power_save_event_t event, void *data)
{
	mdev_t *mdev_p = (mdev_t *) data;
	unsigned int uart_no = mdev_p->port_id;
	uartdev_data_t *uart_data_p;
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;

	switch (event) {
	case ACTION_ENTER_PM2:

	case ACTION_ENTER_PM3: {
		while (UART_GetLineStatus(uart_no, UART_LINESTATUS_TEMT)
			!= SET)
				;
		break;
			}



	case ACTION_EXIT_PM3:{
			UART_CFG_Type uartcfg;
			UART_FIFO_Type fifocfg;
			/* Initialize the hardware */
			uartcfg.baudRate = uart_data_p->baudrate;
			uartcfg.dataBits = UART_DATABITS_8;
			uartcfg.stopBits = uart_data_p->stopbits;
			uartcfg.parity = uart_data_p->parity;
			fifocfg.FIFO_Function = ENABLE;
			fifocfg.FIFO_ResetRx = 0;
			fifocfg.FIFO_ResetTx = 0;
			/* Initializing dummy value as
			   we are not using tx interrupts */
			fifocfg.FIFO_TxEmptyTrigger = 3;
			fifocfg.FIFO_RcvrTrigger = 0;

			if (uart_data_p->mode == UART_9BIT) {
				uartcfg.parity = UART_PARITY_EVEN;
				fifocfg.FIFO_Function = DISABLE;
			}
			uartcfg.autoFlowControl = DISABLE;

			board_uart_pin_config(uart_no);

			switch (uart_no) {
			case UART0_ID:
				CLK_SetUARTClkSrc(CLK_UART_ID_0, CLK_UART_FAST);
				CLK_ModuleClkEnable(CLK_UART0);
				break;
			case UART1_ID:
				CLK_SetUARTClkSrc(CLK_UART_ID_1, CLK_UART_SLOW);
				CLK_ModuleClkEnable(CLK_UART1);
				break;
			case UART2_ID:
			case UART3_ID:
				/* Not implemented */
				break;
			}
			UART_Reset(uart_no);
			UART_Init(uart_no, &uartcfg);
			UART_FIFOConfig(uart_no, &fifocfg);
			UART_IntMask(uart_no, UART_INT_ERBFI, UNMASK);
		}
		break;
	default:
		break;
	}
}

/* ring buffer apis */
static int get_buf_rx_size(mdev_t *dev)
{
	uartdev_data_t *uart_data_p;
	uart_data_p = (uartdev_data_t *) dev->private_data;
	return uart_data_p->rx_ringbuf.buf_size;
}

/* return value 0 => buffer is empty
 * return value 1 => buffer is full
 * return value x => x bytes free in rx buffer
 */
static int get_free_rx_buf_size(mdev_t *dev)
{
	int wr_ptr, rd_ptr, rx_ringbuf_size, free_space;
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) dev->private_data;
	wr_ptr = uart_data_p->rx_ringbuf.wr_idx;
	rd_ptr = uart_data_p->rx_ringbuf.rd_idx;
	rx_ringbuf_size = get_buf_rx_size(dev);
	free_space = ((rd_ptr - wr_ptr) + rx_ringbuf_size) % rx_ringbuf_size;
	return free_space;
}

static void deassert_cts(mdev_t *dev)
{
	uint8_t xoff = XOFF;

	if (get_free_rx_buf_size(dev) == UARTDRV_THRESHOLD_RX_BUF) {
		uart_drv_write(dev, (uint8_t *)(&xoff) , 1);
	}
}

static void assert_cts(mdev_t *dev)
{
	uint8_t xon = XON;

	if (get_free_rx_buf_size(dev) ==
	    (get_buf_rx_size(dev) - UARTDRV_THRESHOLD_RX_BUF)) {
		uart_drv_write(dev, (uint8_t *)(&xon), 1);
	}
}

static char read_rx_buf(mdev_t *dev)
{
	char ret;
	int idx;
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) dev->private_data;
	if (uart_data_p->flowctl == FLOW_CONTROL_SW) {
		assert_cts(dev);
	}
	idx = uart_data_p->rx_ringbuf.rd_idx;
	ret = uart_data_p->rx_ringbuf.buf_p[idx];
	uart_data_p->rx_ringbuf.rd_idx = (uart_data_p->rx_ringbuf.rd_idx + 1)
	    % get_buf_rx_size(dev);
	return ret;
}

static void write_rx_buf(mdev_t *dev, char wr_char)
{
	int idx;
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) dev->private_data;
	idx = uart_data_p->rx_ringbuf.wr_idx;
	uart_data_p->rx_ringbuf.buf_p[idx] = wr_char;
	uart_data_p->rx_ringbuf.wr_idx = (uart_data_p->rx_ringbuf.wr_idx + 1)
	    % get_buf_rx_size(dev);
	if (uart_data_p->flowctl == FLOW_CONTROL_SW) {
		deassert_cts(dev);
	}
}

static void incr_rx_buf_ptr(mdev_t *dev, uint32_t size)
{
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) dev->private_data;
	uart_data_p->rx_ringbuf.wr_idx = (uart_data_p->rx_ringbuf.wr_idx + size)
	    % get_buf_rx_size(dev);
}

int uart_drv_rx_buf_reset(mdev_t *dev)
{
	if (dev ==  NULL) {
		UART_LOG("Unable to open device\r\n");
		return -WM_FAIL;
	}
	uartdev_data_t *uart_data_p;
	uart_data_p = (uartdev_data_t *) dev->private_data;
	uart_data_p->rx_ringbuf.rd_idx = 0;
	uart_data_p->rx_ringbuf.wr_idx = 0;
	return WM_SUCCESS;
}

/* UART callback functions */
static void uart_8bit_cb(int port_num)
{
	char recv_byte;
	mdev_t *dev = &mdev_uart[port_num];
	uartdev_data_t *uart_data_p;

	long xHigherPriorityTaskWoken = pdFALSE;

	uart_data_p = (uartdev_data_t *) dev->private_data;

	if (uart_data_p->is_read_blocking == true)
		if (get_free_rx_buf_size(dev) == 0) {
			xSemaphoreGiveFromISR(uart_block_read_sem[port_num],
					      &xHigherPriorityTaskWoken);
		}

	/* Receive new data */
	while (UART_GetLineStatus(port_num, UART_LINESTATUS_DR) == SET) {
		if (get_free_rx_buf_size(dev) != 1) {
			recv_byte = UART_ReceiveByte(port_num);
			write_rx_buf(dev, recv_byte);
		} else {
			/* Rx Buffer is full, discard received data */
			if (uart_data_p->flowctl == FLOW_CONTROL_HW) {
				/* We need to mask the interrupt till the data
				   is read; otherwise we keep getting
				   data-ready interrupt infinitely */
				UART_IntMask(port_num, UART_INT_ERBFI, MASK);
				return;
			} else {
				UART_ReceiveByte(port_num);
			}

		}
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

static void uart_9bit_cb(int port_num)
{
	uint16_t data;
	mdev_t *dev = &mdev_uart[port_num];
	/* Receive new data */
	if (get_free_rx_buf_size(dev) != 1) {
		data = UART_Receive9bits(port_num);
		write_rx_buf(dev, (uint8_t) ((data >> 8) & 0x01));
		write_rx_buf(dev, (uint8_t) (data & 0xFF));
	} else {
		/* Rx Buffer is full, discard received data */
		UART_Receive9bits(port_num);
	}
}

static void uart_dma_start(int port_id, uint8_t *buf, uint32_t num)
{
	read_dma[port_id].destDmaAddr = (uint32_t)buf;
	DMA_ChannelInit(dmaChannel_rx, &read_dma[port_id]);
	DMA_SetBlkTransfSize(dmaChannel_rx, num);
	DMA_Enable();
	DMA_ChannelCmd(dmaChannel_rx, 1);
}

static void uart_dma_rd_cb(int port_id)
{
	mdev_t *dev = &mdev_uart[port_id];
	uartdev_data_t *uart_data_p;
	long xHigherPriorityTaskWoken = pdFALSE;

	uart_data_p = (uartdev_data_t *) dev->private_data;

	if (uart_data_p->is_read_blocking == true &&
	    get_free_rx_buf_size(dev) == 0)
		xSemaphoreGiveFromISR(uart_block_read_sem[port_id],
				      &xHigherPriorityTaskWoken);

	incr_rx_buf_ptr(dev, GET_DMA_BLK_SIZE(uart_data_p));

	/* TODO: Add flag to check ringbuffer over error*/
	uart_dma_start(port_id, (uint8_t *)(uart_data_p->rx_ringbuf.buf_p +
					    uart_data_p->rx_ringbuf.wr_idx),
		       GET_DMA_BLK_SIZE(uart_data_p));
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

static void uart_dma_wr_cb(int port_id)
{
	os_semaphore_put(&uart_block_dma_wr_sem[port_id]);
}

DEFINE_UART_CB(0, 8)
DEFINE_UART_CB(1, 8)
DEFINE_UART_CB(0, 9)
DEFINE_UART_CB(1, 9)
DEFINE_UART_DMA_RD_CB(0)
DEFINE_UART_DMA_RD_CB(1)
DEFINE_UART_DMA_WR_CB(0)
DEFINE_UART_DMA_WR_CB(1)
#if (NUM_UART_PORTS > 2)
DEFINE_UART_CB(2, 8)
DEFINE_UART_CB(2, 9)
DEFINE_UART_DMA_RD_CB(2)
DEFINE_UART_DMA_WR_CB(2)
#if (NUM_UART_PORTS > 3)
DEFINE_UART_CB(3, 8)
DEFINE_UART_CB(3, 9)
DEFINE_UART_DMA_RD_CB(3)
DEFINE_UART_DMA_WR_CB(3)
#endif
#endif

static void uart_dma_get_rx_config(DMA_CFG_Type * dma, uint32_t addr,
				   int port_id)
{
	switch (port_id) {
	case UART0_ID:
		dma->srcDmaAddr = UART0_BASE;
		dma->srcHwHdskInterf = DMA_HW_HS_INTER_0;
		DMA_SetHandshakingMapping(DMA_HS0_UART0_RX);
		break;
	case UART1_ID:
		dma->srcDmaAddr = UART1_BASE;
		dma->srcHwHdskInterf = DMA_HW_HS_INTER_1;
		DMA_SetHandshakingMapping(DMA_HS1_UART1_RX);
		break;
#if (NUM_UART_PORTS > 2)
	case UART2_ID:
		dma->srcDmaAddr = UART2_BASE;
		dma->srcHwHdskInterf = DMA_HW_HS_INTER_2;
		DMA_SetHandshakingMapping(DMA_HS2_UART2_RX);
		break;
#if (NUM_UART_PORTS > 3)
	case UART3_ID:
		dma->srcDmaAddr = UART3_BASE;
		dma->srcHwHdskInterf = DMA_HW_HS_INTER_3;
		DMA_SetHandshakingMapping(DMA_HS3_UART3_RX);
		break;
#endif
#endif
	}

	/* DMA controller peripheral to memory configuration */
	dma->destDmaAddr = addr;
	dma->transfType = DMA_PER_TO_MEM;
	dma->srcBurstLength = DMA_ITEM_4;
	dma->destBurstLength = DMA_ITEM_4;
	dma->srcAddrInc = DMA_ADDR_NOCHANGE;
	dma->destAddrInc = DMA_ADDR_INC;
	dma->srcTransfWidth = DMA_TRANSF_WIDTH_8;
	dma->destTransfWidth = DMA_TRANSF_WIDTH_8;
	dma->srcSwHwHdskSel = DMA_HW_HANDSHAKING;
	dma->destSwHwHdskSel = DMA_HW_HANDSHAKING;
	dma->channelPriority = DMA_CH_PRIORITY_5;
	dma->destHwHdskInterf = DMA_HW_HS_INTER_1;
	dma->fifoMode = DMA_FIFO_MODE_0;
}

static void uart_dma_get_tx_config(DMA_CFG_Type *dma, uint32_t addr,
				   int port_id)
{
	switch (port_id) {
	case UART0_ID:
		dma->destDmaAddr = UART0_BASE;
		dma->destHwHdskInterf = DMA_HW_HS_INTER_4;
		DMA_SetHandshakingMapping(DMA_HS4_UART0_TX);
		break;
	case UART1_ID:
		dma->destDmaAddr = UART1_BASE;
		dma->destHwHdskInterf = DMA_HW_HS_INTER_5;
		DMA_SetHandshakingMapping(DMA_HS5_UART1_TX);
		break;
#if (NUM_UART_PORTS > 2)
	case UART2_ID:
		dma->destDmaAddr = UART2_BASE;
		dma->destHwHdskInterf = DMA_HW_HS_INTER_6;
		DMA_SetHandshakingMapping(DMA_HS6_UART2_TX);
		break;
#if (NUM_UART_PORTS > 3)
	case UART3_ID:
		dma->destDmaAddr = UART3_BASE;
		dma->destHwHdskInterf = DMA_HW_HS_INTER_7;
		DMA_SetHandshakingMapping(DMA_HS7_UART3_TX);
		break;
#endif
#endif
	}

	/* DMA controller peripheral to memory configuration */
	dma->srcDmaAddr = addr;
	dma->transfType = DMA_MEM_TO_PER;
	dma->srcBurstLength = DMA_ITEM_8;
	dma->destBurstLength = DMA_ITEM_8;
	dma->srcAddrInc = DMA_ADDR_INC;
	dma->destAddrInc = DMA_ADDR_NOCHANGE;
	dma->srcTransfWidth = DMA_TRANSF_WIDTH_8;
	dma->destTransfWidth = DMA_TRANSF_WIDTH_8;
	dma->srcSwHwHdskSel = DMA_HW_HANDSHAKING;
	dma->destSwHwHdskSel = DMA_HW_HANDSHAKING;
	dma->channelPriority = DMA_CH_PRIORITY_4;
	dma->srcHwHdskInterf = DMA_HW_HS_INTER_0;
	dma->fifoMode = DMA_FIFO_MODE_0;
}

/* Exposed APIs and helper functions */
int uart_drv_xfer_mode(UART_ID_Type port_id, UART_DMA_MODE dma_mode)
{
	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (port_id < 0 || port_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", port_id);
		return -WM_FAIL;
	}

	if (!(dma_mode == UART_DMA_ENABLE || dma_mode == UART_DMA_DISABLE)) {
		return -WM_FAIL;
	}
	mdev_p = mdev_get_handle(mdev_uart_name[port_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s\r\n",
			mdev_uart_name[port_id]);
		return -WM_FAIL;
	}
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;
	uart_data_p->dma = dma_mode;
	return WM_SUCCESS;
}

mdev_t *uart_drv_open(UART_ID_Type port_id, uint32_t baud)
{
	int ret;
	UART_CFG_Type uartcfg;
	UART_FIFO_Type fifocfg;
	uartdev_data_t *uart_data_p;
	mdev_t *mdev_p;
	uint32_t buf_size;

	if (port_id < 0 || port_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", port_id);
		return NULL;
	}

	mdev_p = mdev_get_handle(mdev_uart_name[port_id]);

	if (mdev_p == NULL) {
		UART_LOG("Unable to open device %s\r\n",
			 mdev_uart_name[port_id]);
		return NULL;
	}

	ret = os_mutex_get(&uart_mutex[mdev_p->port_id], OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		UART_LOG("failed to get mutex\r\n");
		return NULL;
	}

	uart_data_p = (uartdev_data_t *) mdev_p->private_data;

	if (uart_data_p->mode == UART_9BIT &&
	    uart_data_p->dma == UART_DMA_ENABLE) {
		UART_LOG("Can't initialize dma with 9bit uart\r\n");
		return NULL;
	}

	/* Setup RX ring buffer */
	/* if rx_buffer is not set by the user set it to default size*/
	if (GET_RX_BUF_SIZE(uart_data_p) == 0) {
		SET_RX_BUF_SIZE(uart_data_p, UARTDRV_SIZE_RX_BUF);
	}
	/* init ring buffer */
	if (uart_data_p->dma == true) {
		/*
		 * If DMA block size is greater than ringbuffer block
		 * size, then set DMA block size to ringbuffer size.
		 * Else make ringbuffer size equal to multiple of DMA
		 * block size.
		 */
		if (GET_RX_BUF_SIZE(uart_data_p) <
		    GET_DMA_BLK_SIZE(uart_data_p)) {
			SET_DMA_BLK_SIZE(uart_data_p,
					 GET_RX_BUF_SIZE(uart_data_p));
			buf_size = GET_RX_BUF_SIZE(uart_data_p);
		} else {
			buf_size = GET_RX_BUF_SIZE(uart_data_p) -
				(GET_RX_BUF_SIZE(uart_data_p) %
				 GET_DMA_BLK_SIZE(uart_data_p));
		}
	} else {
		buf_size = GET_RX_BUF_SIZE(uart_data_p);
	}

	uart_data_p->rx_ringbuf.buf_p =	os_mem_alloc(buf_size);
	if (!uart_data_p->rx_ringbuf.buf_p) {
		UART_LOG("Could not allocate fifo\r\n");
		os_mutex_put(&uart_mutex[mdev_p->port_id]);
		return NULL;
	}
	uart_data_p->rx_ringbuf.buf_size = GET_RX_BUF_SIZE(uart_data_p);
	uart_data_p->rx_ringbuf.rd_idx = 0;
	uart_data_p->rx_ringbuf.wr_idx = 0;

	/* Initialize the hardware */
	if (baud) {
		uartcfg.baudRate = baud;
		uart_data_p->baudrate = baud;
	} else {
		uartcfg.baudRate = UARTDRV_DEFAULT_BAUDRATE;
		uart_data_p->baudrate = UARTDRV_DEFAULT_BAUDRATE;
	}
	uartcfg.dataBits = UART_DATABITS_8;
	uartcfg.stopBits = uart_data_p->stopbits;
	if (uart_data_p->mode == UART_9BIT) {
		uartcfg.parity = UART_PARITY_EVEN;
		fifocfg.FIFO_Function = DISABLE;
	} else {
		uartcfg.parity = uart_data_p->parity;
		fifocfg.FIFO_Function = ENABLE;
	}

	if (uart_data_p->flowctl == FLOW_CONTROL_HW) {
		uartcfg.autoFlowControl = ENABLE;
	} else {
		uartcfg.autoFlowControl = DISABLE;
	}

	if (uart_data_p->dma == UART_DMA_DISABLE) {
		fifocfg.FIFO_ResetRx = 0;
		fifocfg.FIFO_ResetTx = 0;
		/* Initializing dummy value as we are not using tx interrupts */
		fifocfg.FIFO_TxEmptyTrigger = 3;
		/* XXX: Change this if you wish to use Hardware fifo */
		fifocfg.FIFO_RcvrTrigger = 0;
	} else {
		fifocfg.FIFO_ResetRx = ENABLE;
		fifocfg.FIFO_ResetTx = ENABLE;
		/* TX fifo watermark at 1/4 of tx fifo size */
		fifocfg.FIFO_TxEmptyTrigger = UART_FIFO_TET_QUATER_FULL;
		/* RX fifo watermark at 1/4 of rx fifo size */
		fifocfg.FIFO_RcvrTrigger = UART_FIFO_RT_QUATER_FULL;
	}
	/* Configure the pinmux for uart pins */
	board_uart_pin_config(mdev_p->port_id);

	UART_Reset(mdev_p->port_id);
	UART_Init(mdev_p->port_id, &uartcfg);
	UART_FIFOConfig(mdev_p->port_id, &fifocfg);

	if (uart_data_p->flowctl == FLOW_CONTROL_HW) {
		/* As per datasheet, RTS bit should be asserted and SIR should
		   be disabled for Hardware auto flow control */
		UART_RTS_Assert(port_id);
		UART_SIR_Disable(port_id);
	}

	if (uart_data_p->is_read_blocking == true) {
		char  sem_name[SEMAPHORE_NAME_LEN];
		snprintf(sem_name, sizeof(sem_name), "uart-block-read-sem%d",
			port_id);
		ret = os_semaphore_create(&uart_block_read_sem[mdev_p->port_id],
					  sem_name);
		if (ret) {
			UART_LOG("Failed to initialize semaphore for blocking"
				 " read\n\r");
			return NULL;
		}
	}

	if (uart_data_p->dma == UART_DMA_DISABLE) {
		install_int_callback(UART_INT_BASE + mdev_p->port_id,
				     UART_INTSTATUS_RBFI,
				     (uart_data_p->mode == UART_9BIT) ?
				     uart_9bit_cb_p[mdev_p->port_id] :
				     uart_8bit_cb_p[mdev_p->port_id]);

		NVIC_EnableIRQ(UART_IRQn_BASE + mdev_p->port_id);
		NVIC_SetPriority(UART_IRQn_BASE + mdev_p->port_id, 0xf);
	} else {
		char  sem_name[SEMAPHORE_NAME_LEN];

		snprintf(sem_name, sizeof(sem_name), "uart-block-dma-wr-sem%d",
			port_id);
		ret = os_semaphore_create(
			&uart_block_dma_wr_sem[mdev_p->port_id],
					  sem_name);
		if (ret) {
			UART_LOG("Failed to initialize semaphore for dma"
				 " write\n\r");
			return NULL;
		}
		os_semaphore_get(&uart_block_dma_wr_sem[mdev_p->port_id],
					 OS_WAIT_FOREVER);

		install_int_callback(DMA_Channel_Int_Type(dmaChannel_rx),
				     INT_BLK_TRANS_COMPLETE,
				     uart_dma_rd_cb_p[mdev_p->port_id]);

		install_int_callback(DMA_Channel_Int_Type(dmaChannel_tx),
				     INT_DMA_TRANS_COMPLETE,
				     uart_dma_wr_cb_p[mdev_p->port_id]);

		NVIC_EnableIRQ(DMA_IRQn);
		NVIC_SetPriority(DMA_IRQn, 0xf);

		uart_dma_get_rx_config(&read_dma[mdev_p->port_id],
				       (uint32_t) NULL,
				       mdev_p->port_id);

		DMA_IntClr(dmaChannel_rx, INT_CH_ALL);
		DMA_IntMask(dmaChannel_rx, INT_BLK_TRANS_COMPLETE, UNMASK);
		uart_dma_start(mdev_p->port_id,
			       (uint8_t *)uart_data_p->rx_ringbuf.buf_p,
			       GET_DMA_BLK_SIZE(uart_data_p));
	}
	UART_IntMask(mdev_p->port_id, UART_INT_ERBFI, UNMASK);
	pm_register_cb(ACTION_EXIT_PM3|ACTION_ENTER_PM2|ACTION_ENTER_PM3,
			uart_ps_cb, mdev_p);

	return mdev_p;
}

void uart_drv_tx_flush(mdev_t *dev)
{
	/* There is no need to flush fifo if its disabled */
	if (UART_GetCompParam(dev->port_id, UART_COMP_FIFO_STAT) == SET)
		while (UART_GetLineStatus(dev->port_id, UART_LINESTATUS_TEMT)
		       != SET)
			os_thread_relinquish();
}

uint32_t uart_drv_get_portid(mdev_t *dev)
{
	return dev->port_id;
}

static uint32_t uart_9bit_read(mdev_t *dev, uint8_t *buf, uint32_t num)
{
	uint32_t len = 0;
	uint16_t data, *buf1 = (uint16_t *) buf;
	/* Get the desired bytes from the RX ring buffer */
	while (num > 0) {
		if (get_free_rx_buf_size(dev)) {
			data = read_rx_buf(dev);
			*buf1 = (data << 8) | read_rx_buf(dev);
			--num;
			++len;
			++buf1;
		} else
			break;
	}
	return len;
}

uint32_t uart_drv_read(mdev_t *dev, uint8_t *buf, uint32_t num)
{
	uint32_t len = 0;
	uartdev_data_t *uart_data_p = (uartdev_data_t *) dev->private_data;
	int port_num = dev->port_id;

	if (uart_data_p->mode == UART_9BIT)
		return uart_9bit_read(dev, buf, num);

	if (uart_data_p->is_read_blocking == true) {
		while (get_free_rx_buf_size(dev) == 0)
			os_semaphore_get(&uart_block_read_sem[port_num],
					 OS_WAIT_FOREVER);
	}

	/* Get the desired bytes from the RX ring buffer */
	while (num > 0) {
		if (get_free_rx_buf_size(dev)) {
			*buf = read_rx_buf(dev);
			--num;
			++len;
			++buf;
		} else
			break;
	}

	if (uart_data_p->flowctl == FLOW_CONTROL_HW)
		UART_IntMask(port_num, UART_INT_ERBFI, UNMASK);

	return len;
}

static uint32_t uart_9bit_write(mdev_t *dev, const uint8_t *buf, uint32_t num)
{
	uint32_t len = 0;
	const uint16_t *buf1 = (const uint16_t *) buf;
	while (num > 0) {
		while (UART_GetLineStatus(dev->port_id,
					  UART_LINESTATUS_THRE) != SET)
			os_thread_relinquish();
		UART_Send9bits(dev->port_id, *buf1);
		buf1++;
		num--;
		len++;
	}

	/* Return number of bytes written */
	return len;
}

static uint32_t uart_drv_dma_write(mdev_t *dev, const uint8_t *buf,
				   uint32_t num)
{
	uint32_t len = num;
	uint16_t dma_trans = DMA_MAX_BLK_SIZE;
	DMA_CFG_Type dma;

	uart_dma_get_tx_config(&dma, (uint32_t) buf, dev->port_id);

	while (num > 0) {
		if (num < DMA_MAX_BLK_SIZE)
			dma_trans = num;
		DMA_ChannelInit(dmaChannel_tx, &dma);
		DMA_IntClr(dmaChannel_tx, INT_CH_ALL);
		DMA_IntMask(dmaChannel_tx, INT_DMA_TRANS_COMPLETE, UNMASK);

		DMA_SetBlkTransfSize(dmaChannel_tx, dma_trans);
		DMA_Enable();
		DMA_ChannelCmd(dmaChannel_tx, 1);

		os_semaphore_get(&uart_block_dma_wr_sem[dev->port_id],
					 OS_WAIT_FOREVER);

		dma.srcDmaAddr += dma_trans;
		num -= dma_trans;
	}
	return len;
}

static uint32_t uart_drv_pio_write(mdev_t *dev, const uint8_t *buf,
				   uint32_t num)
{
	uint32_t len = 0, i;
	uartdev_data_t *uart_data_p = (uartdev_data_t *) dev->private_data;

	if (uart_data_p->mode == UART_9BIT)
		return uart_9bit_write(dev, buf, num);

	while (num > 0) {
		while (UART_GetLineStatus(dev->port_id,
					  UART_LINESTATUS_THRE) != SET)
			os_thread_relinquish();

		for (i = 0; i < UART_FIFO_DEPTH && num > 0; i++) {
			if ((*buf == XON || *buf == XOFF || *buf == ESC)
			    && uart_data_p->flowctl == FLOW_CONTROL_SW) {
				UART_SendByte(dev->port_id, ESC);
			}
			UART_SendByte(dev->port_id, *buf);
			buf++;
			num--;
			len++;
		}
	}

	/* Return number of bytes written */
	return len;
}

uint32_t uart_drv_write(mdev_t *dev, const uint8_t *buf, uint32_t num)
{
	uartdev_data_t *uart_data_p = (uartdev_data_t *) dev->private_data;

	if (uart_data_p->dma == UART_DMA_ENABLE)
		return uart_drv_dma_write(dev, buf, num);
	else
		return uart_drv_pio_write(dev, buf, num);
}

int uart_drv_close(mdev_t *mdev_p)
{
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) mdev_p->private_data;
	uart_drv_tx_flush(mdev_p);
	UART_Reset(mdev_p->port_id);

	/* de-register interrupt callbacks */
	NVIC_DisableIRQ(UART_IRQn_BASE + mdev_p->port_id);
	if (uart_data_p->dma == UART_DMA_DISABLE)
		install_int_callback(UART_INT_BASE + mdev_p->port_id,
				     UART_INTSTATUS_RBFI, 0);
	else {
		NVIC_DisableIRQ(DMA_IRQn);
		install_int_callback(DMA_Channel_Int_Type(dmaChannel_rx),
				INT_BLK_TRANS_COMPLETE, 0);
		install_int_callback(DMA_Channel_Int_Type(dmaChannel_tx),
				INT_DMA_TRANS_COMPLETE, 0);
	}

	if (uart_data_p->rx_ringbuf.buf_p) {
		os_mem_free(uart_data_p->rx_ringbuf.buf_p);
		uart_data_p->rx_ringbuf.buf_p = NULL;
		uart_data_p->rx_ringbuf.buf_size = 0;
	}
	if (uart_data_p->is_read_blocking == true)
		os_semaphore_delete(&uart_block_read_sem[mdev_p->port_id]);
	if (uart_data_p->dma == UART_DMA_ENABLE)
		os_semaphore_delete(&uart_block_dma_wr_sem[mdev_p->port_id]);

	return os_mutex_put(&uart_mutex[mdev_p->port_id]);
}

static void fill_cb_array()
{
	uart_8bit_cb_p[0] = GET_UART_CB(0, 8);
	uart_8bit_cb_p[1] = GET_UART_CB(1, 8);
	uart_9bit_cb_p[0] = GET_UART_CB(0, 9);
	uart_9bit_cb_p[1] = GET_UART_CB(1, 9);
	uart_dma_rd_cb_p[0] = GET_UART_DMA_RD_CB(0);
	uart_dma_rd_cb_p[1] = GET_UART_DMA_RD_CB(1);
	uart_dma_wr_cb_p[0] = GET_UART_DMA_WR_CB(0);
	uart_dma_wr_cb_p[1] = GET_UART_DMA_WR_CB(1);
#if (NUM_UART_PORTS > 2)
	uart_8bit_cb_p[2] = GET_UART_CB(2, 8);
	uart_9bit_cb_p[2] = GET_UART_CB(2, 9);
	uart_dma_rd_cb_p[2] = GET_UART_DMA_RD_CB(2);
	uart_dma_wr_cb_p[2] = GET_UART_DMA_WR_CB(2);
#if (NUM_UART_PORTS > 3)
	uart_8bit_cb_p[3] = GET_UART_CB(3, 8);
	uart_9bit_cb_p[3] = GET_UART_CB(3, 9);
	uart_dma_rd_cb_p[3] = GET_UART_DMA_RD_CB(3);
	uart_dma_wr_cb_p[3] = GET_UART_DMA_WR_CB(3);
#endif
#endif
}

static void uartdrv_mdev_init(int port_num, int mode)
{
	uartdev_data_t *uart_data_p;
	mdev_t *mdev_p;

	mdev_p = &mdev_uart[port_num];
	mdev_p->name = mdev_uart_name[port_num];
	mdev_p->port_id = port_num;
	mdev_p->private_data = (uint32_t) &uartdev_data[port_num];
	uart_data_p = &uartdev_data[port_num];
	uart_data_p->mode = mode;
	SET_RX_BUF_SIZE(uart_data_p, 0);
	uart_data_p->parity = UART_PARITY_NONE;
	uart_data_p->stopbits = UART_STOPBITS_1;
	/* DMA is default disabled */
	uart_data_p->dma = UART_DMA_DISABLE;
	uart_data_p->is_read_blocking = false;
	SET_DMA_BLK_SIZE(uart_data_p, 128);
	fill_cb_array();
}

int uart_drv_init(UART_ID_Type id, int mode)
{
	int ret;

	if (id < 0 || id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", id);
		return -WM_FAIL;
	}

	if (mdev_get_handle(mdev_uart_name[id]) != NULL)
		return WM_SUCCESS;

	ret = os_mutex_create(&uart_mutex[id], "uart", OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Initialize the UART port's mdev structure */
	uartdrv_mdev_init(id, mode);

	switch (id) {
	case UART0_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_0, CLK_UART_SLOW);
		CLK_ModuleClkEnable(CLK_UART0);
		break;
	case UART1_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_1, CLK_UART_SLOW);
		CLK_ModuleClkEnable(CLK_UART1);
		break;
	case UART2_ID:
	case UART3_ID:
		/* Not implemented */
		break;
	}

	/* Register the UART device driver for this port */
	mdev_register(&mdev_uart[id]);

	return WM_SUCCESS;
}

int uart_drv_rxbuf_size(UART_ID_Type uart_id, uint32_t size)
{
	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (uart_id < 0 || uart_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	if (size < 0)
		return -WM_FAIL;

	/*check uart_id is initialized*/
	mdev_p = mdev_get_handle(mdev_uart_name[uart_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s", mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;

	/* threshold limit = 2 x UARTDRV_THRESHOLD_RX_BUFFER */
	if ((uart_data_p->flowctl == FLOW_CONTROL_SW) &&
	    (size <= (2 * UARTDRV_THRESHOLD_RX_BUF))) {
		UART_LOG("Port %d rx buffer size should be greater than"
			 " threshold limit when software flow control is"
			 " enabled\r\n");
		return -WM_FAIL;
	}

	SET_RX_BUF_SIZE(uart_data_p, size);
	return WM_SUCCESS;
}

int uart_drv_dma_rd_blk_size(UART_ID_Type uart_id, uint32_t size)
{
	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (uart_id < 0 || uart_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	if (size < 0)
		return -WM_FAIL;

	/*check uart_id is initialized*/
	mdev_p = mdev_get_handle(mdev_uart_name[uart_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s", mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}

	uart_data_p = (uartdev_data_t *) mdev_p->private_data;

	if (uart_data_p->dma != true) {
		UART_LOG("Can't set block size without enabling dma for %s",
			 mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}
	if (!IS_DMA_BLK_SIZE(size)) {
		UART_LOG("Block size should be less than 1024");
		return -WM_FAIL;
	}
	SET_DMA_BLK_SIZE(uart_data_p, size);
	return WM_SUCCESS;
}

int uart_drv_set_opts(UART_ID_Type uart_id, uint32_t parity, uint32_t stopbits,
		      flow_control_t flow_control)
{
	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (uart_id < 0 || uart_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	if (!IS_UART_PARITY(parity) || !IS_UART_STOPBITS(stopbits)) {
		UART_LOG("Port %d parameter error\r\n", uart_id);
		return -WM_FAIL;
	}

	/*check uart_id is initialized*/
	mdev_p = mdev_get_handle(mdev_uart_name[uart_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s", mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;
	uart_data_p->parity = parity;
	uart_data_p->stopbits = stopbits;

	if (flow_control == FLOW_CONTROL_SW &&
	    uart_data_p->dma == UART_DMA_ENABLE) {
		UART_LOG("Port %d software flow control cannot be enabled"
			 " when DMA is enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	uart_data_p->flowctl = flow_control;

	return WM_SUCCESS;
}

int uart_drv_blocking_read(UART_ID_Type uart_id, bool is_blocking)
{
	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (uart_id < 0 || uart_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	/*check uart_id is initialized*/
	mdev_p = mdev_get_handle(mdev_uart_name[uart_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s", mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;
	if (uart_data_p->dma) {
		UART_LOG("No support for blocking read in DMA mode for %s",
			 mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}
	uart_data_p->is_read_blocking = is_blocking;
	return WM_SUCCESS;
}

void uart_drv_deinit(UART_ID_Type id)
{
	mdev_t *dev;
	/* Return if the port ID is invalid */
	if (id < 0 || id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", id);
		return;
	}

	/* Return if the device was not registered */
	if ((dev = mdev_get_handle(mdev_uart_name[id])) == NULL)
		return;

	/* Delete mutex and dregister the device */
	os_mutex_delete(&uart_mutex[dev->port_id]);
	mdev_deregister(dev->name);
}
