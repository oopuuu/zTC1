/**
 ******************************************************************************
 * @file    MicoDriverAdc.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide ADC driver functions.
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
#include "chip.h" // Magicoe "stm32f2xx.h"
#include "platform_logging.h"
#include "Packing.h"

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/** @brief	ADC SEQ Options */
#define TRIG_SOFT      (0UL << 12)		/*!< Software Trigger */
#define TRIG_PININT0   (0UL << 12)		/*!< Hardware Trigger on PININT0 */
#define TRIG_PININT1   (1UL << 12)		/*!< Hardware Trigger on PININT1 */
#define TRIG_ARMTXEV   (5UL << 12)		/*!< Hardware Trigger on ARM_TXEV */
#define TRIG_POL_NEG   0				/*!< Trigger polarity is negative */
#define TRIG_POL_POS   (1UL << 18)		/*!< Trigger polarity is positive */
#define BYPASS_SYNC    (1UL << 19)		/*!< Bypass Synchronization Filp-Flop */
#define MODE_BURST     (1UL << 27)		/*!< Enable Burst mode */
#define MODE_SINGLE    (1UL << 28)		/*!< Enable Single mode */
#define SEQA_PIRO_HIGH (1UL << 29)		/*!< Set SEQA as HIGH Priority */
#define MODE_EOC       (0UL << 30)		/*!< Event after end of Conversion */
#define MODE_EOS       (1UL << 30)		/*!< Event after end of sequence */
#define ENABLE_CH(ch)  (1UL << (ch))	/*!< Enable the channel number */

/* Gets the ADC trigger from configuration */
#define ADC_TRIGGER(cfg) (((cfg) & 0x3F000) >> 12)

/** @brief	ADC CTRL Options */
#define MODE_SYNC      (0UL << 8)		/*!< Set ADC to synchoronous mode */
#define MODE_ASYNC     (1UL << 8)		/*!< Set ADC to asynchoronous mode */
#define RESOL_6BIT     (0UL << 9)		/*!< Set ADC Resolution to 6 bits */
#define RESOL_8BIT     (1UL << 9)		/*!< Set ADC Resolution to 8 bits */
#define RESOL_10BIT    (2UL << 9)		/*!< Set ADC Resolution to 10 bits */
#define RESOL_12BIT    (3UL << 9)		/*!< Set ADC Resolution to 12 bits */
#define BYPASS_CALIB   (1UL << 11)		/*!< Bypass calibration data */
#define SAMPLE_TIME(x) (((x) & 7) << 12)	/*!< Set the Sample Time to @a x */
#define ENABLE_OVR     (1UL << 24)		/*!< Enable Overflow interrupt */

/* Internal defines */
#define SEQ_A_MASK     0x7C0C5FFF
#define SEQ_B_MASK     0x5C0C5FFF
#define CTRL_MASK      0x00007F00

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/
uint32_t        valSeq[2];		/* Stored SEQ A/B Values */

/******************************************************
 *               Function Declarations
 ******************************************************/
static ErrorCode_t ADC_Handler_SeqPoll_test( const platform_adc_t* adc, ADC_SEQ_IDX_T seqIndex, uint16_t* output);
static ErrorCode_t ADC_Handler_Seq_test( const platform_adc_t* adc, ADC_SEQ_IDX_T seqIndex, uint16_t* output);
static ErrorCode_t ADC_ReadData_test(const platform_adc_t* adc, ADC_SEQ_IDX_T seqIndex, uint16_t* output);
static ErrorCode_t ADC_GetData_test(uint32_t data, uint16_t* output);
static ErrorCode_t ADC_Calibrate_test(LPC_ADC_T *pREGS, uint32_t sysclk_freq);

/******************************************************
 *               Function Definitions
 ******************************************************/


OSStatus platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle )
{
    OSStatus    err = kNoErr;

    platform_mcu_powersave_disable();

    require_action_quiet( adc != NULL, exit, err = kParamErr);

    /* Initialize the associated GPIO */
    Chip_IOCON_PinMuxSet(LPC_IOCON, adc->pin->port, adc->pin->pin_number, IOCON_MODE_INACT | IOCON_FUNC1 | IOCON_ANALOG_EN);

    /* Initialize the ADC CLK */
    Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_ADC0 | SYSCON_PDRUNCFG_PD_VDDA_ENA | SYSCON_PDRUNCFG_PD_VREFP);
    Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_ADC0);
    Chip_Clock_SetADCClockSource(SYSCON_ADCCLKSELSRC_MAINCLK);
    Chip_Clock_SetADCClockDiv(0x1);

    /* To be safe stop the ADC in case it is not stopped */
    adc->port->SEQ_CTRL[0] = 0x0;
    adc->port->SEQ_CTRL[1] = 0x0;
    adc->port->INTEN = 1 << adc->seqIndex;

    /* Configure the ADC */
    valSeq[adc->seqIndex] = ADC_SEQ_CTRL_SEQ_ENA | ADC_SEQ_CTRL_START | ((TRIG_SOFT | TRIG_POL_POS | MODE_EOC | ENABLE_CH(adc->channel)) & SEQ_A_MASK );
    adc->port->CTRL = ( MODE_SYNC | RESOL_12BIT | SAMPLE_TIME(sample_cycle) ) & CTRL_MASK;
    Chip_ADC_SetClockRate( adc->port , adc->adc_peripheral_clock );

    /* Calibrate the ADC */
    if (ADC_Calibrate_test( adc->port , Chip_Clock_GetSystemClockRate()) != LPC_OK) {
          printf("ERROR: Calibrating ADC0_%d\r\n",adc->channel);
          err = kInProgressErr;
          goto exit;
    }
    //printf("ADC0_%d Initialized and Calibrated successfully!\r\n",adc->channel);

exit:
    platform_mcu_powersave_enable();
    return err;
}

OSStatus platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output )
{
    OSStatus    err = kNoErr;

    platform_mcu_powersave_disable();

    require_action_quiet( adc != NULL, exit, err = kParamErr);

    /* Start analog to digital conversion on selected sequence */
    adc->port->SEQ_CTRL[adc->seqIndex] = valSeq[adc->seqIndex] & ~(ADC_SEQ_CTRL_SEQ_ENA | ADC_SEQ_CTRL_START);
    adc->port->SEQ_CTRL[adc->seqIndex] = valSeq[adc->seqIndex];

    /* Wait required number of samples */
    if (ADC_Handler_SeqPoll_test( adc, adc->seqIndex , output ) != LPC_OK) {
      printf("ERROR: required number of samples \r\n");
      err = kInProgressErr;
      goto exit;
    }

exit:
    platform_mcu_powersave_enable();
    return err;
}

OSStatus platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length )
{
    UNUSED_PARAMETER(adc);
    UNUSED_PARAMETER(buffer);
    UNUSED_PARAMETER(buffer_length);
    platform_log("unimplemented");
    return kNotPreparedErr;
}

OSStatus platform_adc_deinit( const platform_adc_t* adc )
{
    UNUSED_PARAMETER(adc);
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable();

  require_action_quiet( adc != NULL, exit, err = kParamErr);

  Chip_ADC_DeInit( adc->port );

exit:
  platform_mcu_powersave_enable();
  return err;
}
/* EXPORTED API: Calibrate the ADC */
static ErrorCode_t ADC_Calibrate_test(LPC_ADC_T *pREGS, uint32_t sysclk_freq)
{
	volatile uint32_t i;

	pREGS->STARTUP = ADC_STARTUP_ENABLE;
	for ( i = 0; i < 0x10; i++ ) {}
	if ( !(pREGS->STARTUP & ADC_STARTUP_ENABLE) ) {
		return ERR_ADC_NO_POWER;
	}

	/* If not in by-pass mode do the calibration */
	if ( (pREGS->CALIBR & ADC_CALREQD) && !(pREGS->CTRL & ADC_CR_BYPASS) ) {
		uint32_t ctrl = pREGS->CTRL & (CTRL_MASK | 0xFF);
		uint32_t tmp = ctrl;

		/* Set ADC to SYNC mode */
		tmp &= ~ADC_CR_ASYNC_MODE;

		/* To be safe run calibration at 1MHz UM permits upto 30MHz */
		if (sysclk_freq > 1000000UL) {
			pREGS->CTRL = tmp | (((sysclk_freq / 1000000UL) - 1) & 0xFF);
		}

		/* Calibration is needed, do it now. */
		pREGS->CALIBR = ADC_CALIB;
		i = 0xF0000;
		while ( (pREGS->CALIBR & ADC_CALIB) && --i ) {}
		pREGS->CTRL = ctrl;
		return i ? LPC_OK : ERR_TIME_OUT;
	}

	/* A dummy conversion cycle will be performed. */
	pREGS->STARTUP = (pREGS->STARTUP | ADC_STARTUP_INIT) & 0x03;
	i = 0x7FFFF;
	while ( (pREGS->STARTUP & ADC_STARTUP_INIT) && --i ) {}
	return i ? LPC_OK : ERR_TIME_OUT;
}
/* PRIVATE: ADC sequence handler polling mode */
static ErrorCode_t ADC_Handler_SeqPoll_test( const platform_adc_t* adc, ADC_SEQ_IDX_T seqIndex, uint16_t* output)
{
	ErrorCode_t ret = LPC_OK;
	/* Poll as long as the sequence is enabled */
	while (adc->port->SEQ_CTRL[seqIndex] & ADC_SEQ_CTRL_SEQ_ENA) {

		if (!(adc->port->FLAGS & ADC_FLAGS_SEQN_INT_MASK(seqIndex))) {
			continue;
		}

		ret = ADC_Handler_Seq_test(adc, seqIndex, output);
		if (ret != LPC_OK) {
			break;
		}
	}
	return ret;
}

/* PRIVATE: ADC Sequence event handler function */
static ErrorCode_t ADC_Handler_Seq_test( const platform_adc_t* adc, ADC_SEQ_IDX_T seqIndex, uint16_t* output)
{
	uint32_t flag = adc->port->FLAGS;
	uint32_t tmp;
        *output = 0xffff;

	/* Check if overrun is enabled and got an overrun */
	tmp = flag & ADC_FLAGS_SEQN_OVRRUN_MASK(seqIndex);

	if (!(flag & ADC_FLAGS_SEQN_INT_MASK(seqIndex))) {
		return ERR_ADC_INVALID_SEQUENCE;
	}

	if (ADC_ReadData_test(adc, seqIndex, output) != LPC_OK) {
		return ERR_FAILED;
	}

	/* Handle the overflow */
	if (tmp) {
		printf("ADC Handle the overflow!\r\n");
	}

	/* Clear the interrupt if it is for EOS and not EOC */
	if (valSeq[seqIndex] & ADC_SEQ_CTRL_MODE_EOS) {
		adc->port->FLAGS = ADC_FLAGS_SEQN_INT_MASK(seqIndex);
	}

        if ( *output != 0xffff ){

          /* Disable interrupts */
          adc->port->INTEN &= ~(1 << seqIndex);

          /* Stop and disable the sequence */
          adc->port->SEQ_CTRL[seqIndex] = valSeq[seqIndex] &
                                              ~(ADC_SEQ_CTRL_SEQ_ENA | ADC_SEQ_CTRL_BURST | ADC_SEQ_CTRL_START);
          return LPC_OK;
        }

	/* If we are not in burst mode we must trigger next sample */
	if (!((valSeq[seqIndex] >> 12) & 0x1F) && !(valSeq[seqIndex] & ADC_SEQ_CTRL_BURST)) {
		adc->port->SEQ_CTRL[seqIndex] = valSeq[seqIndex];
}

	return LPC_OK;
}
/* PRIVATE: Reads data from the GDAT or DAT register based on mode of operation */
static ErrorCode_t ADC_ReadData_test(const platform_adc_t* adc, ADC_SEQ_IDX_T seqIndex, uint16_t* output)
{
	int i;
	/* Check if this is End-of-Seq or End-of-SingleConversion */
	if (!(valSeq[seqIndex] & ADC_SEQ_CTRL_MODE_EOS)) {
		return ADC_GetData_test(adc->port->SEQ_GDAT[seqIndex], output);
	}
	/* Read channels having conversion data */
	for (i = 0; i < sizeof(adc->port->DAT) / sizeof(adc->port->DAT[0]); i++) {
		if (valSeq[seqIndex] & ADC_SEQ_CTRL_CHANSEL(i)) {
			if (ADC_GetData_test(adc->port->DAT[i], output) != LPC_OK) {
				return ERR_FAILED;
			}
		}
	}
	return LPC_OK;
}
/* PRIVATE: Extract, format data and store into user buffer  */
static ErrorCode_t ADC_GetData_test(uint32_t data, uint16_t* output)
{
	/* If data is not vaild something is wrong! */
	if (!(data & ADC_SEQ_GDAT_DATAVALID)) {
		return ERR_FAILED;
	}

        /* Read ADC conversion result */
        *output = (uint16_t) ADC_DR_RESULT(data);
	return LPC_OK;
}
