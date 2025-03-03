/**
 ******************************************************************************
 * @file    paltform_uart.c
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


#include "mico_rtos.h"
#include "mico_platform.h"

#include "platform.h"
#include "platform_peripheral.h"
#include "debug.h"

#include "pinmap.h"

#define mico_log(M, ...) custom_log("MICO", M, ##__VA_ARGS__)
#define mico_log_trace() custom_log_trace("MICO")


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
static const PinMap PinMap_UART_TX[] = {
    {PC_3,  RTL_PIN_PERI(UART0, 0, S0), RTL_PIN_FUNC(UART0, S0)},
    {PE_0,  RTL_PIN_PERI(UART0, 0, S1), RTL_PIN_FUNC(UART0, S1)},
    {PA_7,  RTL_PIN_PERI(UART0, 0, S2), RTL_PIN_FUNC(UART0, S2)},
    {PD_3,  RTL_PIN_PERI(UART1, 1, S0), RTL_PIN_FUNC(UART1, S0)},
    {PE_4,  RTL_PIN_PERI(UART1, 1, S1), RTL_PIN_FUNC(UART1, S1)},
    {PB_5,  RTL_PIN_PERI(UART1, 1, S2), RTL_PIN_FUNC(UART1, S2)},
    {PA_4,  RTL_PIN_PERI(UART2, 2, S0), RTL_PIN_FUNC(UART2, S0)},
    {PC_9,  RTL_PIN_PERI(UART2, 2, S1), RTL_PIN_FUNC(UART2, S1)},
    {PD_7,  RTL_PIN_PERI(UART2, 2, S2), RTL_PIN_FUNC(UART2, S2)},
    {NC,    NC,     0}
};

static const PinMap PinMap_UART_RX[] = {
    {PC_0,  RTL_PIN_PERI(UART0, 0, S0), RTL_PIN_FUNC(UART0, S0)},
    {PE_3,  RTL_PIN_PERI(UART0, 0, S1), RTL_PIN_FUNC(UART0, S1)},
    {PA_6,  RTL_PIN_PERI(UART0, 0, S2), RTL_PIN_FUNC(UART0, S2)},
    {PD_0,  RTL_PIN_PERI(UART1, 1, S0), RTL_PIN_FUNC(UART1, S0)},
    {PE_7,  RTL_PIN_PERI(UART1, 1, S1), RTL_PIN_FUNC(UART1, S1)},
    {PB_4,  RTL_PIN_PERI(UART1, 1, S2), RTL_PIN_FUNC(UART1, S2)},
    {PA_0,  RTL_PIN_PERI(UART2, 2, S0), RTL_PIN_FUNC(UART2, S0)},
    {PC_6,  RTL_PIN_PERI(UART2, 2, S1), RTL_PIN_FUNC(UART2, S1)},
    {PD_4,  RTL_PIN_PERI(UART2, 2, S2), RTL_PIN_FUNC(UART2, S2)},
    {NC,    NC,     0}
};

static uint8_t tmp_data_out;

#define UART_NUM (3)
#define SERIAL_TX_IRQ_EN        0x01
#define SERIAL_RX_IRQ_EN        0x02
#define SERIAL_TX_DMA_EN        0x01
#define SERIAL_RX_DMA_EN        0x02

//static uint32_t serial_irq_ids[UART_NUM] = {0, 0, 0};

//static uart_irq_handler irq_handler[UART_NUM];
//static uint32_t serial_irq_en[UART_NUM]={0, 0, 0};

#ifdef CONFIG_GDMA_EN
static uint32_t serial_dma_en[UART_NUM] = {0, 0, 0};
static HAL_GDMA_OP UartGdmaOp;
#endif

extern u32 ConfigDebugErr;
extern u32 ConfigDebuginfo;

/******************************************************
*        Static Function Declarations
******************************************************/
/* Interrupt service functions - called from interrupt vector table */
#ifndef NO_MICO_RTOS
static void thread_wakeup(void *arg);
static void RX_PIN_WAKEUP_handler(void *arg);
#endif

/******************************************************
*               Function Definitions
******************************************************/
OSStatus rtk_uart_init( platform_uart_driver_t* driver, platform_uart_t* peripheral, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer )
{
    OSStatus          err = kNoErr;
    uint32_t uart_tx, uart_rx;
    uint32_t uart_sel;
    uint8_t uart_idx;
    PHAL_RUART_OP      pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter;
#ifdef CONFIG_GDMA_EN
    PUART_DMA_CONFIG   pHalRuartDmaCfg;
    PHAL_GDMA_OP pHalGdmaOp=&UartGdmaOp;
#endif

    ConfigDebugErr &= (~_DBG_UART_);
    ConfigDebugInfo &= (~_DBG_UART_);


	//rtk_uart_deinit(driver);

    // Determine the UART to use (UART0, UART1, or UART3)
    uart_tx = pinmap_peripheral(peripheral->tx, PinMap_UART_TX);
    uart_rx = pinmap_peripheral(peripheral->rx, PinMap_UART_RX);

    uart_sel = pinmap_merge(uart_tx, uart_rx);
    uart_idx = RTL_GET_PERI_IDX(uart_sel);
    if (unlikely(uart_idx == (uint8_t)NC)) {
        DBG_UART_ERR("%s: Cannot find matched UART\n", __FUNCTION__);
        return kParamErr;
    }

    pHalRuartOp = &(peripheral->hal_uart_op);
    pHalRuartAdapter = &(peripheral->hal_uart_adp);

    if ((NULL == pHalRuartOp) || (NULL == pHalRuartAdapter)) {
        DBG_UART_ERR("%s: Allocate Adapter Failed\n", __FUNCTION__);
        return kParamErr;
    }
    
    HalRuartOpInit((VOID*)pHalRuartOp);

#ifdef CONFIG_GDMA_EN
    HalGdmaOpInit((VOID*)pHalGdmaOp);
    pHalRuartDmaCfg = &peripheral->uart_gdma_cfg;
    pHalRuartDmaCfg->pHalGdmaOp = pHalGdmaOp;
    pHalRuartDmaCfg->pTxHalGdmaAdapter = &peripheral->uart_gdma_adp_tx;
    pHalRuartDmaCfg->pRxHalGdmaAdapter = &peripheral->uart_gdma_adp_rx;
    _memset((void*)(pHalRuartDmaCfg->pTxHalGdmaAdapter), 0, sizeof(HAL_GDMA_ADAPTER));
    _memset((void*)(pHalRuartDmaCfg->pRxHalGdmaAdapter), 0, sizeof(HAL_GDMA_ADAPTER));	
#endif

    pHalRuartOp->HalRuartAdapterLoadDef(pHalRuartAdapter, uart_idx);
    pHalRuartAdapter->PinmuxSelect = RTL_GET_PERI_SEL(uart_sel);
    //pHalRuartAdapter->BaudRate = 9600;
    pHalRuartAdapter->IrqHandle.Priority = 6;    
    

    pHalRuartAdapter->BaudRate = config->baud_rate;
//  HalRuartInit(pHalRuartAdapter);
    //HalRuartSetBaudRate((VOID*)pHalRuartAdapter);	

  switch ( config->data_width)
  {
    case DATA_WIDTH_7BIT  :
      pHalRuartAdapter->WordLen = RUART_WLS_7BITS;
      break;

    case DATA_WIDTH_8BIT:
      pHalRuartAdapter->WordLen = RUART_WLS_8BITS;
      break;

    default:
      err = kParamErr;
      goto exit;
  }

  switch ( config->parity )
  {
    case NO_PARITY:
      pHalRuartAdapter->Parity = RUART_PARITY_DISABLE;
      break;

    case EVEN_PARITY:
      pHalRuartAdapter->Parity = RUART_PARITY_ENABLE;
      pHalRuartAdapter->ParityType = RUART_EVEN_PARITY;
      break;

    case ODD_PARITY:
      pHalRuartAdapter->Parity = RUART_PARITY_ENABLE;
      pHalRuartAdapter->ParityType = RUART_ODD_PARITY;
      break;

    default:
      err = kParamErr;
      goto exit;
  }

  switch ( config->stop_bits )
  {
    case STOP_BITS_1:
      pHalRuartAdapter->StopBit = RUART_STOP_BIT_1;
      break;

    case STOP_BITS_2:
      pHalRuartAdapter->StopBit = RUART_STOP_BIT_2;
      break;

    default:
      err = kParamErr;
      goto exit;
  } 

  switch ( config->flow_control )
  {
    case FLOW_CONTROL_DISABLED:
    case FLOW_CONTROL_RTS:		
      pHalRuartAdapter->FlowControl = AUTOFLOW_DISABLE;
      break;

    case FLOW_CONTROL_CTS:
    case FLOW_CONTROL_CTS_RTS:
      pHalRuartAdapter->FlowControl = AUTOFLOW_ENABLE;
      break;

    default:
      err = kParamErr;
      goto exit;
  }

    //pHalRuartOp->HalRuartInit(pHalRuartAdapter);
    //pHalRuartOp->HalRuartRegIrq(pHalRuartAdapter);    
    //pHalRuartOp->HalRuartIntEnable(pHalRuartAdapter);	
    HalRuartInit(pHalRuartAdapter);
	HalRuartSetBaudRateRtl8195a(pHalRuartAdapter);     
	
    pHalRuartAdapter->TxCompCallback = (void(*)(void*))platform_uart_tx_dma_irq;
    pHalRuartAdapter->TxCompCbPara = (void*)driver;	

    if(!optional_ring_buffer){
      pHalRuartAdapter->RxCompCallback = (void(*)(void*))platform_uart_rx_dma_irq;
      pHalRuartAdapter->RxCompCbPara = (void*)driver;
    }else{  
      pHalRuartAdapter->RxDRCallback = (void(*)(void*))platform_uart_irq;
      pHalRuartAdapter->RxDRCbPara = (void*)driver;    
      pHalRuartAdapter->Interrupts |= RUART_IER_ERBI | RUART_IER_ELSI;
      HalRuartSetIMRRtl8195a (pHalRuartAdapter);  	  
    }

    //pHalRuartOp->HalRuartRegIrq(pHalRuartAdapter);    
    //pHalRuartOp->HalRuartIntEnable(pHalRuartAdapter);	
    pHalRuartOp->HalRuartRegIrq(pHalRuartAdapter);    
    pHalRuartOp->HalRuartIntEnable(pHalRuartAdapter);    
	
exit:
    return err;
}

OSStatus rtk_uart_deinit( platform_uart_driver_t* driver)
{
    OSStatus          err = kNoErr;
    PHAL_RUART_ADAPTER pHalRuartAdapter;
#ifdef CONFIG_GDMA_EN
    u8  uart_idx;
    PUART_DMA_CONFIG   pHalRuartDmaCfg;
#endif

    pHalRuartAdapter = &(driver->peripheral->hal_uart_adp);

    HalRuartDeInit(pHalRuartAdapter);

#ifdef CONFIG_GDMA_EN
    uart_idx = pHalRuartAdapter->UartIndex;
    pHalRuartDmaCfg = &driver->peripheral->uart_gdma_cfg;
    if (serial_dma_en[uart_idx] & SERIAL_RX_DMA_EN) {
        HalRuartRxGdmaDeInit(pHalRuartDmaCfg);
        serial_dma_en[uart_idx] &= ~SERIAL_RX_DMA_EN;
    }

    if (serial_dma_en[uart_idx] & SERIAL_TX_DMA_EN) {
        HalRuartTxGdmaDeInit(pHalRuartDmaCfg);
        serial_dma_en[uart_idx] &= ~SERIAL_TX_DMA_EN;
    }    
#endif	
exit:
    return err;
}

// return the byte count received before timeout, or error(<0)
OSStatus rtk_uart_recv_stream_timeout (platform_uart_driver_t* driver, char *prxbuf, uint32_t len, uint32_t timeout_ms, void *force_cs)
{
    PHAL_RUART_OP      pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(driver->peripheral->hal_uart_adp);
    uint32_t TimeoutCount=0, StartCount;
    int ret;
    void (*task_yield)(void);

    task_yield = NULL;
    pHalRuartOp = &(driver->peripheral->hal_uart_op);
    ret = pHalRuartOp->HalRuartIntRecv(pHalRuartAdapter, (u8*)prxbuf, len);
    if ((ret == HAL_OK) && (timeout_ms > 0))  {
        TimeoutCount = (timeout_ms*1000/TIMER_TICK_US);
        StartCount = HalTimerOp.HalTimerReadCount(1);
        task_yield = (void(*)(void))force_cs;
        while (pHalRuartAdapter->State & HAL_UART_STATE_BUSY_RX) {
            if (HAL_TIMEOUT == RuartIsTimeout(StartCount, TimeoutCount)) {
                ret = pHalRuartOp->HalRuartStopRecv((VOID*)pHalRuartAdapter);
                ret = pHalRuartOp->HalRuartResetRxFifo((VOID*)pHalRuartAdapter);
                pHalRuartAdapter->Status = HAL_UART_STATUS_TIMEOUT;
                return kTimeoutErr;
            }
            if (NULL != task_yield) {
               task_yield();
            }
        }
        return kNoErr;
    }else {
        return kGeneralErr;
    }
}

int rtk_uart_readable(platform_uart_driver_t* driver) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(driver->peripheral->hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;

    if ((HAL_RUART_READ32(uart_idx, RUART_LINE_STATUS_REG_OFF)) & RUART_LINE_STATUS_REG_DR) {
        return 1;
    }
    else {
        return 0;
    }
}

int rtk_uart_getc(platform_uart_driver_t* driver) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(driver->peripheral->hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;

    while (!rtk_uart_readable(driver));
    return (int)((HAL_RUART_READ32(uart_idx, RUART_REV_BUF_REG_OFF)) & 0xFF);
}

OSStatus platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer )
{
  OSStatus          err = kNoErr;

  platform_mcu_powersave_disable();

  require_action_quiet( ( driver != NULL ) && ( peripheral != NULL ) && ( config != NULL ), exit, err = kParamErr);
  require_action_quiet( (optional_ring_buffer == NULL) || ((optional_ring_buffer->buffer != NULL ) && (optional_ring_buffer->size != 0)), exit, err = kParamErr);
    
  driver->rx_size              = 0;
  driver->tx_size              = 0;
  driver->last_transmit_result = kNoErr;
  driver->last_receive_result  = kNoErr;
  driver->peripheral           = (platform_uart_t*)peripheral;
#ifndef NO_MICO_RTOS
  mico_rtos_init_semaphore( &driver->tx_complete, 1 );
  mico_rtos_init_semaphore( &driver->rx_complete, 1 );
  mico_rtos_init_semaphore( &driver->sem_wakeup,  1 );
  mico_rtos_init_mutex    ( &driver->tx_mutex );
#else
  driver->tx_complete = false;
  driver->rx_complete = false;
#endif
    
  err = rtk_uart_init(driver, (platform_uart_t*)peripheral, config, optional_ring_buffer);

    if (optional_ring_buffer != NULL){
      /* Note that the ring_buffer should've been initialised first */
      driver->rx_buffer = optional_ring_buffer;
      driver->rx_size   = 0;
      //platform_uart_receive_bytes( uart, optional_rx_buffer->buffer, optional_rx_buffer->size, 0 );
    }

exit:
  MicoMcuPowerSaveConfig(true);
  return err;
}

OSStatus platform_uart_deinit( platform_uart_driver_t* driver )
{
  uint8_t          uart_number;
  OSStatus          err = kNoErr;

  platform_mcu_powersave_disable();

  require_action_quiet( ( driver != NULL ), exit, err = kParamErr);

  err = rtk_uart_deinit(driver);
  
#ifndef NO_MICO_RTOS
  mico_rtos_deinit_semaphore( &driver->rx_complete );
  mico_rtos_deinit_semaphore( &driver->tx_complete );
  mico_rtos_deinit_mutex( &driver->tx_mutex );
#else
  driver->rx_complete = false;
  driver->tx_complete = false;
#endif
  driver->rx_size              = 0;
  driver->tx_size              = 0;
  driver->last_transmit_result = kNoErr;
  driver->last_receive_result  = kNoErr;

exit:
    platform_mcu_powersave_enable();
    return err;
}

OSStatus platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
  OSStatus err = kNoErr;
#ifndef NO_MICO_RTOS
  mico_rtos_lock_mutex(&driver->tx_mutex);
#endif
  if( size == 1 )
  {
     tmp_data_out = *data_out;
     data_out = &tmp_data_out;
  }

  platform_mcu_powersave_disable();
  
    PHAL_RUART_OP      pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(driver->peripheral->hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;
    int32_t ret;

    pHalRuartOp = &(driver->peripheral->hal_uart_op);

    if ((serial_dma_en[uart_idx] & SERIAL_TX_DMA_EN)==0) {
        PUART_DMA_CONFIG   pHalRuartDmaCfg;
        
        pHalRuartDmaCfg = &driver->peripheral->uart_gdma_cfg;
        if (HAL_OK == HalRuartTxGdmaInit(pHalRuartOp, pHalRuartAdapter, pHalRuartDmaCfg)) {
            serial_dma_en[uart_idx] |= SERIAL_TX_DMA_EN;
        }
        else {
            return kGeneralErr;
        }
    }    

    HalRuartEnterCritical(pHalRuartAdapter);	
    ret = pHalRuartOp->HalRuartDmaSend(pHalRuartAdapter, (u8*)data_out, size);
    HalRuartExitCritical(pHalRuartAdapter);	
    if(ret != HAL_OK)
        return kGeneralErr;
  
/* Wait for transmission complete */
#ifndef NO_MICO_RTOS
  mico_rtos_get_semaphore( &driver->tx_complete, MICO_NEVER_TIMEOUT );
#else 
  while( driver->tx_complete == false );
  driver->tx_complete = false;
#endif

  /* Disable DMA and clean up */
  driver->tx_size = 0;
  err = driver->last_transmit_result;

exit:  
  platform_mcu_powersave_enable();
#ifndef NO_MICO_RTOS
  mico_rtos_unlock_mutex(&driver->tx_mutex);
#endif
  return err;
}

OSStatus platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t expected_data_size, uint32_t timeout_ms )
{
  OSStatus err = kNoErr;

  //platform_mcu_powersave_disable();

  require_action_quiet( ( driver != NULL ) && ( data_in != NULL ) && ( expected_data_size != 0 ), exit, err = kParamErr);

  if ( driver->rx_buffer != NULL )
  {
      /* Check if ring buffer already contains the required amount of data. */
      if ( expected_data_size > ring_buffer_used_space( driver->rx_buffer ) )
      {
        /* Set rx_size and wait in rx_complete semaphore until data reaches rx_size or timeout occurs */
        driver->rx_size = expected_data_size;
        if(expected_data_size <= ring_buffer_used_space( driver->rx_buffer ))
        {
          goto end;
        }
        
#ifndef NO_MICO_RTOS
        if ( mico_rtos_get_semaphore( &driver->rx_complete, timeout_ms) != kNoErr )
        {
          driver->rx_size = 0;
          return kTimeoutErr;
        }
#else
        driver->rx_complete = false;
        if(expected_data_size <= ring_buffer_used_space( driver->rx_buffer ))
        {
          goto end;
        }
        int delay_start = mico_get_time_no_os();
        while(driver->rx_complete == false){
          if(mico_get_time_no_os() >= delay_start + timeout_ms && timeout_ms != MICO_NEVER_TIMEOUT){
            driver->rx_size = 0;
            return kTimeoutErr;
          }
        }
#endif
      end:        
        /* Reset rx_size to prevent semaphore being set while nothing waits for the data */
        driver->rx_size = 0;
      }
      
      // Grab data from the buffer
      uint32_t read_size = 0;
      ring_buffer_read(driver->rx_buffer, data_in, expected_data_size, &read_size);
  }
  else
  {
      err = rtk_uart_recv_stream_timeout( driver, data_in, expected_data_size, timeout_ms, NULL );
  }
  
exit:
  //platform_mcu_powersave_enable();
  return err;
}


uint32_t platform_uart_get_length_in_buffer( platform_uart_driver_t* driver )
{  
  return ring_buffer_used_space( driver->rx_buffer );
}


#if 0
uint8_t platform_uart_get_port_number( USART_TypeDef* uart )
{
    if ( uart == USART1 )
    {
        return 0;
    }
    else if ( uart == USART2 )
    {
        return 1;
    }
    else if ( uart == USART3 )
    {
        return 2;
    }
    else if ( uart == UART4 )
    {
        return 3;
    }
    else if ( uart == UART5 )
    {
        return 4;
    }
    else if ( uart == USART6 )
    {
        return 5;
    }
    else
    {
        return 0xff;
    }
}
#endif

#ifndef NO_MICO_RTOS
static void thread_wakeup(void *arg)
{
#if 0
  platform_uart_driver_t* driver = arg;
  
  while(1){
    if( mico_rtos_get_semaphore( driver->sem_wakeup, 1000) != kNoErr )
    {
      platform_gpio_irq_enable( driver->peripheral->pin_rx, IRQ_TRIGGER_FALLING_EDGE, RX_PIN_WAKEUP_handler, driver );
      platform_mcu_powersave_enable( );
    }
  }
#endif  
}
#endif

/******************************************************
*            Interrupt Service Routines
******************************************************/
#ifndef NO_MICO_RTOS
void RX_PIN_WAKEUP_handler(void *arg)
{
#if 0
  (void)arg;
  platform_uart_driver_t* driver = arg;
  uint32_t uart_number;
  
  platform_gpio_enable_clock( driver->peripheral->pin_rx );

  uart_number = platform_uart_get_port_number( driver->peripheral->port );

  uart_peripheral_clock_functions[ uart_number ]( uart_peripheral_clocks[ uart_number ], ENABLE );

  /* Enable DMA peripheral clock */
  if ( driver->peripheral->tx_dma_config.controller == DMA1 )
  {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA1;
  }
  else
  {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA2;
  }

  platform_gpio_irq_disable( driver->peripheral->pin_rx );
  platform_mcu_powersave_disable( );
  mico_rtos_set_semaphore( &driver->sem_wakeup );
#endif
}
#endif


void platform_uart_irq( platform_uart_driver_t* driver )
{
  int status;
  uint8_t rxData;
  //platform_uart_port_t* uart = (platform_uart_port_t*) driver->peripheral->port;

    rxData = rtk_uart_getc(driver);	
    ring_buffer_write( driver->rx_buffer, &rxData,1 );

    status++;

    //printf("\r\nnuart irq %d\r\n", status);	 

    // Notify thread if sufficient data are available
    if ( ( driver->rx_size > 0 ) &&
        ( ring_buffer_used_space( driver->rx_buffer ) >= driver->rx_size ) )
    {
  #ifndef NO_MICO_RTOS
      mico_rtos_set_semaphore( &driver->rx_complete );
  #else
      driver->rx_complete = true;
  #endif
      driver->rx_size = 0;
    }

}

void platform_uart_tx_dma_irq( platform_uart_driver_t* driver )
{
        #ifndef NO_MICO_RTOS
        /* Set semaphore regardless of result to prevent waiting thread from locking up */
        mico_rtos_set_semaphore( &driver->tx_complete );
        #else
        driver->tx_complete = true;
        #endif
}

void platform_uart_rx_dma_irq( platform_uart_driver_t* driver )
{
    mico_log("uart irq");	 

        /* Set semaphore regardless of result to prevent waiting thread from locking up */
        #ifndef NO_MICO_RTOS
        mico_rtos_set_semaphore( &driver->rx_complete );
        #else
        driver->rx_complete = true;
        #endif

}


