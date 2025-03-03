/**
 ******************************************************************************
 * @file    paltform_spi.c
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


#include "mico_rtos.h"
#include "mico_platform.h"

#include "platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "debug.h"

#include "pinmap.h"
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
*               Static Function Declarations
******************************************************/

static uint16_t spi_transfer       ( const platform_spi_t* spi, uint16_t data );
static OSStatus spi_dma_transfer   ( const platform_spi_t* spi, const platform_spi_config_t* config );
static void     spi_dma_config     ( const platform_spi_t* spi, const platform_spi_message_segment_t* message );

/******************************************************
*               Variables Definitions
******************************************************/
extern const DW_SSI_DEFAULT_SETTING SpiDefaultSetting;

static const PinMap PinMap_SSI_MOSI[] = {
    {PE_2,  RTL_PIN_PERI(SPI0, 0, S0), RTL_PIN_FUNC(SPI0, S0)},
    {PC_2,  RTL_PIN_PERI(SPI0, 0, S1), RTL_PIN_FUNC(SPI0, S1)},
    {PA_1,  RTL_PIN_PERI(SPI1, 1, S0), RTL_PIN_FUNC(SPI1, S0)},
    {PB_6,  RTL_PIN_PERI(SPI1, 1, S1), RTL_PIN_FUNC(SPI1, S1)},
    {PD_6,  RTL_PIN_PERI(SPI1, 1, S2), RTL_PIN_FUNC(SPI1, S2)},
    {PG_2,  RTL_PIN_PERI(SPI2, 2, S0), RTL_PIN_FUNC(SPI2, S0)},
    {PE_6,  RTL_PIN_PERI(SPI2, 2, S1), RTL_PIN_FUNC(SPI2, S1)},
    {PD_2,  RTL_PIN_PERI(SPI2, 2, S2), RTL_PIN_FUNC(SPI2, S2)},
    {NC,    NC,     0}
};

static const PinMap PinMap_SSI_MISO[] = {
    {PE_3,  RTL_PIN_PERI(SPI0, 0, S0), RTL_PIN_FUNC(SPI0, S0)},
    {PC_3,  RTL_PIN_PERI(SPI0, 0, S1), RTL_PIN_FUNC(SPI0, S1)},
    {PA_0,  RTL_PIN_PERI(SPI1, 1, S0), RTL_PIN_FUNC(SPI1, S0)},
    {PB_7,  RTL_PIN_PERI(SPI1, 1, S1), RTL_PIN_FUNC(SPI1, S1)},
    {PD_7,  RTL_PIN_PERI(SPI1, 1, S2), RTL_PIN_FUNC(SPI1, S2)},
    {PG_3,  RTL_PIN_PERI(SPI2, 2, S0), RTL_PIN_FUNC(SPI2, S0)},
    {PE_7,  RTL_PIN_PERI(SPI2, 2, S1), RTL_PIN_FUNC(SPI2, S1)},
    {PD_3,  RTL_PIN_PERI(SPI2, 2, S2), RTL_PIN_FUNC(SPI2, S2)},
    {NC,    NC,     0}
};

/******************************************************
*               Function Definitions
******************************************************/
#if 0
uint8_t platform_spi_get_port_number( platform_spi_port_t* spi )
{
  if ( spi == SPI1 )
  {
    return 0;
  }
  else if ( spi == SPI2 )
  {
    return 1;
  }
  else if ( spi == SPI3 )
  {
    return 2;
  }
  else
  {
    return 0xFF;
  }
}
#endif

OSStatus rtk_spi_init( platform_spi_driver_t* driver, platform_spi_t* peripheral, const platform_spi_config_t* config )
{
  uint32_t ssi_mosi, ssi_miso, ssi_peri;
  uint8_t  ssi_idx, ssi_pinmux;
  PHAL_SSI_ADAPTOR pHalSsiAdaptor;
  PHAL_SSI_OP pHalSsiOp;

  pHalSsiAdaptor = &peripheral->spi_adp;
  pHalSsiOp = &peripheral->spi_op;

  OSStatus          err = kNoErr;
 
    peripheral->state = 0;
    uint32_t SystemClock = SystemGetCpuClk();
    uint32_t MaxSsiFreq  = (SystemClock >> 2) >> 1;

    /* SsiClockDivider doesn't support odd number */

    DBG_SSI_INFO("SystemClock: %d\n", SystemClock);
    DBG_SSI_INFO("MaxSsiFreq : %d\n", MaxSsiFreq);

    ssi_mosi = pinmap_peripheral(peripheral->mosi, PinMap_SSI_MOSI);
    ssi_miso = pinmap_peripheral(peripheral->miso, PinMap_SSI_MISO);
    //DBG_SSI_INFO("ssi_mosi: %d, ssi_miso: %d\n", ssi_mosi, ssi_miso);

    ssi_peri = pinmap_merge(ssi_mosi, ssi_miso);
    if (unlikely(ssi_peri == NC)) {
        DBG_SSI_ERR("spi_init(): Cannot find matched SSI index.\n");
        return kParamErr;
    }

    ssi_idx = RTL_GET_PERI_IDX(ssi_peri);
    ssi_pinmux = RTL_GET_PERI_SEL(ssi_peri);
    DBG_SSI_INFO("ssi_peri: %d, ssi_idx: %d, ssi_pinmux: %d\n", ssi_peri, ssi_idx, ssi_pinmux);

    pHalSsiAdaptor = &peripheral->spi_adp;
    pHalSsiOp = &peripheral->spi_op;

    pHalSsiAdaptor->Index = ssi_idx;
    pHalSsiAdaptor->PinmuxSelect = ssi_pinmux;

    //TODO: Implement default setting structure.
    pHalSsiOp->HalSsiLoadSetting(pHalSsiAdaptor, (void*)&SpiDefaultSetting);
    pHalSsiAdaptor->DefaultRxThresholdLevel = SpiDefaultSetting.RxThresholdLevel;
      
  /* Configure data-width */
  if ( config->bits == 8){
    pHalSsiAdaptor->DataFrameSize = 7;
  }else if(config->bits == 16 ){
    pHalSsiAdaptor->DataFrameSize = 15;
  }else{
    err = kUnsupportedErr;
    goto exit;
  }
  
  /* Configure mode CPHA and CPOL */
  if ( config->mode & SPI_CLOCK_IDLE_HIGH )
  {
    pHalSsiAdaptor->SclkPolarity = SCPOL_INACTIVE_IS_HIGH;
  }
  else
  {
    pHalSsiAdaptor->SclkPolarity = SCPOL_INACTIVE_IS_LOW;
  }
  
  if ( config->mode & SPI_CLOCK_RISING_EDGE )
  {
    pHalSsiAdaptor->SclkPhase = ( config->mode & SPI_CLOCK_IDLE_HIGH ) ? SCPH_TOGGLES_AT_START : SCPH_TOGGLES_IN_MIDDLE;
  }
  else
  {
    pHalSsiAdaptor->SclkPhase = ( config->mode & SPI_CLOCK_IDLE_HIGH ) ? SCPH_TOGGLES_IN_MIDDLE : SCPH_TOGGLES_AT_START;
  }

  pHalSsiAdaptor->Role = SSI_MASTER;

  HalSsiSetSclk(pHalSsiAdaptor, (u32)(config->speed));
  

  driver->peripheral = (platform_spi_t *)peripheral;
    
  HalSsiOpInit((VOID*)pHalSsiOp);

  pHalSsiOp->HalSsiSetDeviceRole(pHalSsiAdaptor, pHalSsiAdaptor->Role);

  /* Pinmux workaround */
  if ((ssi_idx == 0) && (ssi_pinmux == SSI0_MUX_TO_GPIOC)) {
      EEPROM_PIN_CTRL(OFF);
  }

  if ((ssi_idx == 0) && (ssi_pinmux == SSI0_MUX_TO_GPIOE)) {
      DBG_SSI_WARN(ANSI_COLOR_MAGENTA"SPI0 Pin may conflict with JTAG\r\n"ANSI_COLOR_RESET);        
  }

  HalSsiInit(pHalSsiAdaptor);

  HalSsiEnable((VOID*)pHalSsiAdaptor);  

exit:
  return err;
}

OSStatus platform_spi_init( platform_spi_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_config_t* config )
{
  OSStatus          err = kNoErr;
  
  platform_mcu_powersave_disable();
  
  require_action_quiet( ( driver != NULL ) && ( peripheral != NULL ) && ( config != NULL ), exit, err = kParamErr);
  require_action( !(config->mode & SPI_USE_DMA), exit, err = kUnsupportedErr);

  require_noerr(err, exit);

  err = rtk_spi_init(driver, (platform_spi_t*)peripheral, config);
  
exit:
  platform_mcu_powersave_enable();
  return err;
}



OSStatus platform_spi_deinit( platform_spi_driver_t* driver )
{
  UNUSED_PARAMETER( driver );
  /* TODO: unimplemented */
  return kUnsupportedErr;
}


OSStatus platform_spi_transfer( platform_spi_driver_t* driver, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
  OSStatus err    = kNoErr;
  uint32_t count  = 0;
  uint16_t i;
  
  platform_mcu_powersave_disable();
  
  require_action_quiet( ( driver != NULL ) && ( config != NULL ) && ( segments != NULL ) && ( number_of_segments != 0 ), exit, err = kParamErr);
  
  /* Activate chip select */
  
  for ( i = 0; i < number_of_segments; i++ )
  {
    {
      count = segments[i].length;
      
      /* in interrupt-less mode */
      if ( config->bits == 8 )
      {
        const uint8_t* send_ptr = ( const uint8_t* )segments[i].tx_buffer;
        uint8_t*       rcv_ptr  = ( uint8_t* )segments[i].rx_buffer;
        
        while ( count-- )
        {
          uint16_t data = 0xFF;
          
          if ( send_ptr != NULL )
          {
            data = *send_ptr++;
          }
          
          data = spi_transfer( driver->peripheral, data );
          
          if ( rcv_ptr != NULL )
          {
            *rcv_ptr++ = (uint8_t)data;
          }
        }
      }
      else if ( config->bits == 16 )
      {
        const uint16_t* send_ptr = (const uint16_t *) segments[i].tx_buffer;
        uint16_t*       rcv_ptr  = (uint16_t *) segments[i].rx_buffer;
        
        /* Check that the message length is a multiple of 2 */
        
        require_action_quiet( ( count % 2 ) == 0, cleanup_transfer, err = kSizeErr);
        
        /* Transmit/receive data stream, 16-bit at time */
        while ( count != 0 )
        {
          uint16_t data = 0xFFFF;
          
          if ( send_ptr != NULL )
          {
            data = *send_ptr++;
          }
          
          data = spi_transfer( driver->peripheral, data );
          
          if ( rcv_ptr != NULL )
          {
            *rcv_ptr++ = data;
          }
          
          count -= 2;
        }
      }
    }
  }
  
cleanup_transfer:
  ///* Deassert chip select */
  //platform_gpio_output_high( config->chip_select );
  
exit:
  platform_mcu_powersave_enable( );
  return err;
}


static inline void ssi_write (platform_spi_t* spi, int value)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &spi->spi_adp;
    pHalSsiOp = &spi->spi_op;

    while (!pHalSsiOp->HalSsiWriteable(pHalSsiAdaptor));
    pHalSsiOp->HalSsiWrite((VOID*)pHalSsiAdaptor, value);
}

static inline int ssi_read(platform_spi_t* spi)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &spi->spi_adp;
    pHalSsiOp = &spi->spi_op;

    while (!pHalSsiOp->HalSsiReadable(pHalSsiAdaptor));
    return (int)pHalSsiOp->HalSsiRead(pHalSsiAdaptor);
}

static uint16_t spi_transfer( const platform_spi_t* spi, uint16_t data )
{
    ssi_write((platform_spi_t*)spi, (int)data);
    return (uint16_t)ssi_read((platform_spi_t*)spi);
}














