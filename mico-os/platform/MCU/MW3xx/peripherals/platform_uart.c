/*
 * Copyright 2016, MXCHIP.
 * All Rights Reserved.
 */

/*
 * uart.c: mxchip driver for UART
 */

#include <board.h>
#include <wmtypes.h>
#include <wmstdio.h>
#include <mdev_uart.h>
//#include <wm_os.h>
#include <pwrmgr.h>
#include <mdev_dma.h>
#include <wmassert.h>
#include <system-work-queue.h>
#include "common.h"
#include "mico_platform.h"

#define DISABLE_DMA
#ifdef DISABLE_DMA
#define NUM_UART_PORTS 3
#define UART_IRQn_BASE UART0_IRQn
#define UART_INT_BASE	INT_UART0

#ifdef NO_MICO_RTOS
#define os_thread_relinquish()
#endif


static void (*uart_8bit_cb_p[NUM_UART_PORTS]) ();
static void (*uart_9bit_cb_p[NUM_UART_PORTS]) ();

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

extern platform_uart_t platform_uart_peripherals[];
extern platform_uart_driver_t platform_uart_drivers[];

/* UART callback functions */
static void uart_8bit_cb(int port_num)
{
	uint8_t recv_byte, i;
	platform_uart_driver_t* driver = NULL;

    for ( i = 0; i < MICO_UART_MAX; i++ ) {
        if ( platform_uart_peripherals[i].port_id == port_num ) {
            driver = &platform_uart_drivers[i];
            break;
        }
    }

    if ( driver == NULL ) return;
	/* Receive new data */
	while (UART_GetLineStatus(port_num, UART_LINESTATUS_DR) == SET) {
		if (ring_buffer_is_full(driver->rx_ring_buffer) == 0) {
			recv_byte = UART_ReceiveData(port_num);
    		ring_buffer_write( driver->rx_ring_buffer, &recv_byte,1 );
			if ( ( driver->rx_size > 0 ) &&
					( ring_buffer_used_space( driver->rx_ring_buffer) >= driver->rx_size ) ) {
				mico_rtos_set_semaphore( &driver->rx_complete );

				driver->rx_size = 0;

			}
		} else {
			/* Rx Buffer is full, discard received data */
			if (driver->flow_control != FLOW_CONTROL_DISABLED_DRV) {
				/* We need to mask the interrupt till the data
				   is read; otherwise we keep getting
				   data-ready interrupt infinitely */
				UART_IntMask(port_num, UART_INT_RDA, MASK);
				return;
			} else {
				UART_ReceiveData(port_num);
			}
		}
	}
}

static void uart_9bit_cb(int port_num)
{
	uint16_t data;
	platform_uart_driver_t* driver = &platform_uart_drivers[port_num];
	uint8_t recv_byte;
	/* Receive new data */
	while (UART_GetLineStatus(port_num, UART_LINESTATUS_DR) == SET) {
		if (ring_buffer_free_space(driver->rx_ring_buffer) > 1) {
			data = UART_Receive9bits(port_num);
			recv_byte = (uint8_t) ((data >> 8) & 0x01);
    		ring_buffer_write( driver->rx_ring_buffer, &recv_byte,1 );
			recv_byte = (uint8_t) (data & 0xFF);
    		ring_buffer_write( driver->rx_ring_buffer, &recv_byte,1 );
			
			if ( ( driver->rx_size > 0 ) &&
					( ring_buffer_used_space( driver->rx_ring_buffer ) >= driver->rx_size ) ) {
				mico_rtos_set_semaphore( &driver->rx_complete );
				driver->rx_size = 0;
			}
		} else {
			/* Rx Buffer is full, discard received data */
			if (driver->flow_control != FLOW_CONTROL_DISABLED_DRV) {
				/* We need to mask the interrupt till the data
				   is read; otherwise we keep getting
				   data-ready interrupt infinitely */
				UART_IntMask(port_num, UART_INT_RDA, MASK);
				return;
			} else {
				UART_Receive9bits(port_num);
			}
		}
	}
}

DEFINE_UART_CB(0, 8)
DEFINE_UART_CB(1, 8)
DEFINE_UART_CB(0, 9)
DEFINE_UART_CB(1, 9)
#if (NUM_UART_PORTS > 2)
DEFINE_UART_CB(2, 8)
DEFINE_UART_CB(2, 9)
#if (NUM_UART_PORTS > 3)
DEFINE_UART_CB(3, 8)
DEFINE_UART_CB(3, 9)
#endif
#endif


static void fill_cb_array()
{
	uart_8bit_cb_p[0] = GET_UART_CB(0, 8);
	uart_8bit_cb_p[1] = GET_UART_CB(1, 8);
	uart_9bit_cb_p[0] = GET_UART_CB(0, 9);
	uart_9bit_cb_p[1] = GET_UART_CB(1, 9);
#if (NUM_UART_PORTS > 2)
	uart_8bit_cb_p[2] = GET_UART_CB(2, 8);
	uart_9bit_cb_p[2] = GET_UART_CB(2, 9);
#if (NUM_UART_PORTS > 3)
	uart_8bit_cb_p[3] = GET_UART_CB(3, 8);
	uart_9bit_cb_p[3] = GET_UART_CB(3, 9);
#endif
#endif
}

OSStatus platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer )
//int platform_uart_init(int port_id, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer)
{
	UART_CFG_Type uartcfg;
	UART_FifoCfg_Type fifocfg;
	uint32_t port_id = peripheral->port_id;
	//platform_uart_driver_t* driver;

	//sys_work_queue_init();
	if (port_id < 0 || port_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", port_id);
		return -1;
	}
	fill_cb_array();
	
	//driver = &uart_drivers[port_id];
	driver->id = port_id;
	
	switch (driver->id) {
	case UART0_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_0, CLK_UART_FAST);
		CLK_ModuleClkEnable(CLK_UART0);
		break;
	case UART1_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_1, CLK_UART_SLOW);
		CLK_ModuleClkEnable(CLK_UART1);
		break;
	case UART2_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_2, CLK_UART_SLOW);
		CLK_ModuleClkEnable(CLK_UART2);
		break;
	}
	if (optional_ring_buffer != NULL){
      /* Note that the ring_buffer should've been initialised first */
      driver->rx_ring_buffer = optional_ring_buffer;
     
    } else {
      uint8_t *pbuf;
	  pbuf = malloc(512);
	  ring_buffer_init(driver->rx_ring_buffer, pbuf, 512);
	}
	
	/* Initialize the UART port's mdev structure */
	driver->rx_size              = 0;
    driver->tx_size              = 0;
    driver->last_transmit_result = kNoErr;
    driver->last_receive_result  = kNoErr;

	uartcfg.baudRate = config->baud_rate;
	uartcfg.stickyParity = DISABLE;
	fifocfg.fifoEnable = ENABLE;

	switch (config->data_width) {
	case DATA_WIDTH_5BIT:
		uartcfg.dataBits = UART_DATABITS_5;
		break;
	case DATA_WIDTH_6BIT:
		uartcfg.dataBits = UART_DATABITS_6;
		break;
	case DATA_WIDTH_7BIT:
		uartcfg.dataBits = UART_DATABITS_7;
		break;
	case DATA_WIDTH_8BIT:
		uartcfg.dataBits = UART_DATABITS_8;
		break;
	case DATA_WIDTH_9BIT:
		uartcfg.dataBits = UART_DATABITS_8;
		uartcfg.parity = UART_PARITY_EVEN;
		uartcfg.stickyParity = DISABLE;
		fifocfg.fifoEnable = DISABLE;
		break;
	default:
        uartcfg.dataBits = UART_DATABITS_8;
		break;
	}
	switch ( config->parity )
    {
      case NO_PARITY:
        uartcfg.parity = UART_PARITY_NONE;
        break;
  
      case EVEN_PARITY:
        uartcfg.parity = UART_PARITY_EVEN;
        break;
  
      case ODD_PARITY:
        uartcfg.parity = UART_PARITY_ODD;
        break;
  
      default:
        uartcfg.parity = UART_PARITY_NONE;
		break;
    }
	if (config->flow_control != FLOW_CONTROL_DISABLED) {
		fifocfg.autoFlowControl = ENABLE;
	} else {
		fifocfg.autoFlowControl = DISABLE;
	}

	if ( config->stop_bits == STOP_BITS_1 )
		UART_SetStopBits(port_id, UART_STOPBITS_1);
	else
		UART_SetStopBits(port_id, UART_STOPBITS_2);
	
	fifocfg.rxFifoLevel = UART_RXFIFO_BYTE_1;
	fifocfg.txFifoLevel = UART_TXFIFO_HALF_EMPTY;
	fifocfg.peripheralBusType = UART_PERIPHERAL_BITS_8;
	fifocfg.fifoDmaEnable = DISABLE;

	UART_Disable(driver->id);
	UART_Enable(driver->id);
	UART_Init(driver->id, &uartcfg);
	UART_FifoConfig(driver->id, &fifocfg);
	
	install_int_callback(UART_INT_BASE + driver->id,
				     UART_INT_RDA,
				     (config->data_width == DATA_WIDTH_9BIT) ?
				     uart_9bit_cb_p[driver->id] :
				     uart_8bit_cb_p[driver->id]);

	NVIC_EnableIRQ(UART_IRQn_BASE + driver->id);
	NVIC_SetPriority(UART_IRQn_BASE + driver->id, 0xf);
	UART_IntMask(driver->id, UART_INT_RDA, UNMASK);

	/* Configure the pinmux for uart pins */
	board_uart_pin_config(driver->id);

	mico_rtos_init_semaphore( &driver->tx_complete, 1 );
    mico_rtos_init_semaphore( &driver->rx_complete, 1 );
    mico_rtos_init_semaphore( &driver->sem_wakeup,  1 );
    mico_rtos_init_mutex    ( &driver->tx_mutex );
	return WM_SUCCESS;
}

//uint32_t mico_get_time_no_os(void)
//{
//	return os_total_ticks_get();
//}

OSStatus platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t expected_data_size, uint32_t timeout_ms )
{
  OSStatus err = kNoErr;

   if (( data_in == NULL ) || ( expected_data_size == 0 )) {
   		err = kParamErr;
		goto exit;
   }


    while (expected_data_size != 0)
    {
      uint32_t transfer_size = MIN( driver->rx_ring_buffer->size/2, expected_data_size );
      
      /* Check if ring buffer already contains the required amount of data. */
      if ( transfer_size > ring_buffer_used_space( driver->rx_ring_buffer ) )
      {
        /* Set rx_size and wait in rx_complete semaphore until data reaches rx_size or timeout occurs */
        driver->rx_size = transfer_size;
        
        if ( mico_rtos_get_semaphore( &driver->rx_complete, timeout_ms) != kNoErr )
        {
          driver->rx_size = 0;
          return kTimeoutErr;
        }
        
        /* Reset rx_size to prevent semaphore being set while nothing waits for the data */
        driver->rx_size = 0;
      }
      
      expected_data_size -= transfer_size;
      
      // Grab data from the buffer
      do
      {
        uint8_t* available_data;
        uint32_t bytes_available;
        
        ring_buffer_get_data( driver->rx_ring_buffer, &available_data, &bytes_available );
        bytes_available = MIN( bytes_available, transfer_size );
        memcpy( data_in, available_data, bytes_available );
        transfer_size -= bytes_available;
        data_in = ( (uint8_t*) data_in + bytes_available );
        ring_buffer_consume( driver->rx_ring_buffer, bytes_available );
      } while ( transfer_size != 0 );
    }
    
    if ( expected_data_size != 0 )
    {
      return kGeneralErr;
    }
    else
    {
      return kNoErr;
    }
  
  
exit:
  //platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
//OSStatus platform_uart_transmit_bytes( int port_id, const uint8_t* data_out, uint32_t size )
{
  int i;
  
  if (driver->tx_mutex)
  	mico_rtos_lock_mutex( &driver->tx_mutex );
  for(i=0; i<size; i++) {
		while (UART_GetLineStatus(driver->id,
					  UART_LINESTATUS_TDRQ) != SET) {
			os_thread_relinquish();
		}
		
		UART_SendData(driver->id, data_out[i]);
	}
  if (driver->tx_mutex)
  	mico_rtos_unlock_mutex( &driver->tx_mutex );
  return kNoErr;
}


uint32_t platform_uart_get_length_in_buffer( platform_uart_driver_t* driver )
{
  return ring_buffer_used_space( driver->rx_ring_buffer );
}


OSStatus platform_uart_deinit( platform_uart_driver_t* driver )
{
  
  mico_rtos_deinit_semaphore( &driver->rx_complete );
  mico_rtos_deinit_semaphore( &driver->tx_complete );
  mico_rtos_deinit_semaphore( &driver->sem_wakeup );
  mico_rtos_deinit_mutex( &driver->tx_mutex );
  driver->rx_size              = 0;
  driver->tx_size              = 0;
  driver->last_transmit_result = kNoErr;
  driver->last_receive_result  = kNoErr;

  while (UART_GetLineStatus(driver->id, UART_LINESTATUS_TEMT)!= SET)
	  ;
		
	UART_Disable(driver->id);

	/* de-register interrupt callbacks */
	NVIC_DisableIRQ(UART_IRQn_BASE + driver->id);
	install_int_callback(UART_INT_BASE + driver->id, UART_INT_RDA, 0);

	
    return 0;
}
#else
#define NUM_UART_PORTS 3
#define UART_IRQn_BASE UART0_IRQn
#define UART_INT_BASE	INT_UART0

static platform_uart_driver_t uart_drivers[NUM_UART_PORTS];
static void (*uart_8bit_cb_p[NUM_UART_PORTS]) ();
static void (*uart_9bit_cb_p[NUM_UART_PORTS]) ();

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

/* UART callback functions */
static void uart_8bit_cb(int port_num)
{
	uint8_t recv_byte;
	platform_uart_driver_t* driver = &uart_drivers[port_num];

	
	/* Receive new data */
	while (UART_GetLineStatus(port_num, UART_LINESTATUS_DR) == SET) {
		if (ring_buffer_is_full(driver->rx_ring_buffer) == 0) {
			recv_byte = UART_ReceiveData(port_num);
    		ring_buffer_write( driver->rx_ring_buffer, &recv_byte,1 );
			if ( ( driver->rx_size > 0 ) &&
					( ring_buffer_used_space( driver->rx_ring_buffer) >= driver->rx_size ) ) {
				mico_rtos_set_semaphore( &driver->rx_complete );
				driver->rx_size = 0;

			}
		} else {
			/* Rx Buffer is full, discard received data */
			if (driver->flow_control != FLOW_CONTROL_DISABLED) {
				/* We need to mask the interrupt till the data
				   is read; otherwise we keep getting
				   data-ready interrupt infinitely */
				UART_IntMask(port_num, UART_INT_RDA, MASK);
				return;
			} else {
				UART_ReceiveData(port_num);
			}
		}
	}
}

static void uart_9bit_cb(int port_num)
{
	uint16_t data;
	platform_uart_driver_t* driver = &uart_drivers[port_num];
	uint8_t recv_byte;
	/* Receive new data */
	while (UART_GetLineStatus(port_num, UART_LINESTATUS_DR) == SET) {
		if (ring_buffer_free_space(driver->rx_ring_buffer) > 1) {
			data = UART_Receive9bits(port_num);
			recv_byte = (uint8_t) ((data >> 8) & 0x01);
    		ring_buffer_write( driver->rx_ring_buffer, &recv_byte,1 );
			recv_byte = (uint8_t) (data & 0xFF);
    		ring_buffer_write( driver->rx_ring_buffer, &recv_byte,1 );
			
			if ( ( driver->rx_size > 0 ) &&
					( ring_buffer_used_space( driver->rx_ring_buffer ) >= driver->rx_size ) ) {
				mico_rtos_set_semaphore( &driver->rx_complete );
				driver->rx_size = 0;
			}
		} else {
			/* Rx Buffer is full, discard received data */
			if (driver->flow_control != FLOW_CONTROL_DISABLED) {
				/* We need to mask the interrupt till the data
				   is read; otherwise we keep getting
				   data-ready interrupt infinitely */
				UART_IntMask(port_num, UART_INT_RDA, MASK);
				return;
			} else {
				UART_Receive9bits(port_num);
			}
		}
	}
}

DEFINE_UART_CB(0, 8)
DEFINE_UART_CB(1, 8)
DEFINE_UART_CB(0, 9)
DEFINE_UART_CB(1, 9)
#if (NUM_UART_PORTS > 2)
DEFINE_UART_CB(2, 8)
DEFINE_UART_CB(2, 9)
#if (NUM_UART_PORTS > 3)
DEFINE_UART_CB(3, 8)
DEFINE_UART_CB(3, 9)
#endif
#endif


static void fill_cb_array()
{
	uart_8bit_cb_p[0] = GET_UART_CB(0, 8);
	uart_8bit_cb_p[1] = GET_UART_CB(1, 8);
	uart_9bit_cb_p[0] = GET_UART_CB(0, 9);
	uart_9bit_cb_p[1] = GET_UART_CB(1, 9);
#if (NUM_UART_PORTS > 2)
	uart_8bit_cb_p[2] = GET_UART_CB(2, 8);
	uart_9bit_cb_p[2] = GET_UART_CB(2, 9);
#if (NUM_UART_PORTS > 3)
	uart_8bit_cb_p[3] = GET_UART_CB(3, 8);
	uart_9bit_cb_p[3] = GET_UART_CB(3, 9);
#endif
#endif
}


int platform_uart_init(int port_id, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer)
{
	UART_CFG_Type uartcfg;
	UART_FifoCfg_Type fifocfg;
	platform_uart_driver_t* driver;
	int ret;
	
	//sys_work_queue_init();
	if (port_id < 0 || port_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", port_id);
		return -1;
	}
	fill_cb_array();
	
	driver = &uart_drivers[port_id];
	driver->id = port_id;
	
	switch (driver->id) {
	case UART0_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_0, CLK_UART_FAST);
		CLK_ModuleClkEnable(CLK_UART0);
		break;
	case UART1_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_1, CLK_UART_SLOW);
		CLK_ModuleClkEnable(CLK_UART1);
		break;
	case UART2_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_2, CLK_UART_SLOW);
		CLK_ModuleClkEnable(CLK_UART2);
		break;
	}
	/* Initialize the UART port's mdev structure */
	driver->rx_size              = 0;
    driver->tx_size              = 0;
    driver->last_transmit_result = kNoErr;
    driver->last_receive_result  = kNoErr;

	uartcfg.baudRate = config->baud_rate;
	uartcfg.stickyParity = DISABLE;
	fifocfg.fifoEnable = ENABLE;

	switch (config->data_width) {
	case DATA_WIDTH_5BIT:
		uartcfg.dataBits = UART_DATABITS_5;
		break;
	case DATA_WIDTH_6BIT:
		uartcfg.dataBits = UART_DATABITS_6;
		break;
	case DATA_WIDTH_7BIT:
		uartcfg.dataBits = UART_DATABITS_7;
		break;
	case DATA_WIDTH_8BIT:
		uartcfg.dataBits = UART_DATABITS_8;
		break;
	case DATA_WIDTH_9BIT:
		uartcfg.dataBits = UART_DATABITS_8;
		uartcfg.parity = UART_PARITY_EVEN;
		uartcfg.stickyParity = DISABLE;
		fifocfg.fifoEnable = DISABLE;
		break;
	default:
        uartcfg.dataBits = UART_DATABITS_8;
		break;
	}
	switch ( config->parity )
    {
      case NO_PARITY:
        uartcfg.parity = UART_PARITY_NONE;
        break;
  
      case EVEN_PARITY:
        uartcfg.parity = UART_PARITY_EVEN;
        break;
  
      case ODD_PARITY:
        uartcfg.parity = UART_PARITY_ODD;
        break;
  
      default:
        uartcfg.parity = UART_PARITY_NONE;
		break;
    }
	if (config->flow_control != FLOW_CONTROL_DISABLED) {
		fifocfg.autoFlowControl = ENABLE;
	} else {
		fifocfg.autoFlowControl = DISABLE;
	}

	if ( config->stop_bits == STOP_BITS_1 )
		UART_SetStopBits(port_id, UART_STOPBITS_1);
	else
		UART_SetStopBits(port_id, UART_STOPBITS_2);
	
	fifocfg.rxFifoLevel = UART_RXFIFO_BYTE_1;
	fifocfg.txFifoLevel = UART_TXFIFO_HALF_EMPTY;
	fifocfg.peripheralBusType = UART_PERIPHERAL_BITS_8;
	fifocfg.fifoDmaEnable = DISABLE;

	UART_Disable(driver->id);
	UART_Enable(driver->id);
	UART_Init(driver->id, &uartcfg);
	UART_FifoConfig(driver->id, &fifocfg);
	
	/* initialize dma driver */
	ret = dma_drv_init();
	if (ret != WM_SUCCESS) {
		UART_LOG("Failed to initialize dma driver\r\n");
		return NULL;
	}
	ret = sys_work_queue_init();
	if (ret != WM_SUCCESS) {
		UART_LOG("Failed to initialize system workqueue\r\n");
		return NULL;
	}
	if (optional_ring_buffer != NULL){
      /* Note that the ring_buffer should've been initialised first */
      driver->rx_ring_buffer = optional_ring_buffer;
     
    } else {
      uint8_t *pbuf;
	  pbuf = os_mem_alloc(512);
	  ring_buffer_init(driver->rx_ring_buffer, pbuf, 512);
	}
	driver->rx_size   = 0;
	
	/* Enable UART to send dma request to DMAC */
	UART_DmaCmd(port_id, ENABLE);
	uart_dma_read_start(port_id,
			(uint8_t *)driver->rx_ring_buffer->buffer,
			driver->rx_ring_buffer->size);
	UART_IntMask(driver->id, UART_INT_RDA, UNMASK);

	/* Configure the pinmux for uart pins */
	board_uart_pin_config(driver->id);
	
	
	return WM_SUCCESS;
}

uint32_t mico_get_time_no_os(void)
{
	return os_total_ticks_get();
}

OSStatus platform_uart_receive_bytes( int port_id, uint8_t* data_in, uint32_t expected_data_size, uint32_t timeout_ms )
{
  OSStatus err = kNoErr;
	platform_uart_driver_t* driver;

	if (port_id < 0 || port_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", port_id);
		return -1;
	}

	
	driver = &uart_drivers[port_id];


   if (( data_in == NULL ) || ( expected_data_size == 0 )) {
   		err = kParamErr;
		goto exit;
   }


    while (expected_data_size != 0)
    {
      uint32_t transfer_size = MIN( driver->rx_ring_buffer->size/2, expected_data_size );
      
      /* Check if ring buffer already contains the required amount of data. */
      if ( transfer_size > ring_buffer_used_space( driver->rx_ring_buffer ) )
      {
        /* Set rx_size and wait in rx_complete semaphore until data reaches rx_size or timeout occurs */
        driver->rx_size = transfer_size;
        
        if ( mico_rtos_get_semaphore( &driver->rx_complete, timeout_ms) != kNoErr )
        {
          driver->rx_size = 0;
          return kTimeoutErr;
        }
        
        /* Reset rx_size to prevent semaphore being set while nothing waits for the data */
        driver->rx_size = 0;
      }
      
      expected_data_size -= transfer_size;
      
      // Grab data from the buffer
      do
      {
        uint8_t* available_data;
        uint32_t bytes_available;
        
        ring_buffer_get_data( driver->rx_ring_buffer, &available_data, &bytes_available );
        bytes_available = MIN( bytes_available, transfer_size );
        memcpy( data_in, available_data, bytes_available );
        transfer_size -= bytes_available;
        data_in = ( (uint8_t*) data_in + bytes_available );
        ring_buffer_consume( driver->rx_ring_buffer, bytes_available );
      } while ( transfer_size != 0 );
    }
    
    if ( expected_data_size != 0 )
    {
      return kGeneralErr;
    }
    else
    {
      return kNoErr;
    }
  
  
exit:
  //platform_mcu_powersave_enable();
  return err;
}


OSStatus platform_uart_transmit_bytes( int port_id, const uint8_t* data_out, uint32_t size )
{
  int i;
  
  if (port_id < 0 || port_id >= NUM_UART_PORTS) {
  	UART_LOG("Port %d not enabled\r\n", port_id);
  	return kGeneralErr;
  }
	
  for(i=0; i<size; i++) {
		while (UART_GetLineStatus(port_id,
					  UART_LINESTATUS_TDRQ) != SET) {
			os_thread_relinquish();
		}
		
		UART_SendData(port_id, data_out[i]);
	}
  return kNoErr;
}



OSStatus platform_uart_get_length_in_buffer( int port_id )
{  
  platform_uart_driver_t* driver;

	if (port_id < 0 || port_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", port_id);
		return 0;
	}

	
	driver = &uart_drivers[port_id];
	
  return ring_buffer_used_space( driver->rx_ring_buffer );
}


OSStatus platform_uart_deinit( int port_id )
{
  platform_uart_driver_t* driver;

	if (port_id < 0 || port_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", port_id);
		return 0;
	}

	
	driver = &uart_drivers[port_id];
  
  mico_rtos_deinit_semaphore( &driver->rx_complete );
  mico_rtos_deinit_semaphore( &driver->tx_complete );
  mico_rtos_deinit_semaphore( &driver->sem_wakeup );
  mico_rtos_deinit_mutex( &driver->tx_mutex );
  driver->rx_size              = 0;
  driver->tx_size              = 0;
  driver->last_transmit_result = kNoErr;
  driver->last_receive_result  = kNoErr;

  while (UART_GetLineStatus(port_id, UART_LINESTATUS_TEMT)!= SET)
	  ;
		
	UART_Disable(port_id);

	/* de-register interrupt callbacks */
	NVIC_DisableIRQ(UART_IRQn_BASE + port_id);
	install_int_callback(UART_INT_BASE + port_id, UART_INT_RDA, 0);

	
    return 0;
}

#endif
