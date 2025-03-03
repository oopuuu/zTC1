/**
 ******************************************************************************
 * @file    MicoDriverPwm.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide PWM driver functions.
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
#include "platform_logging.h"
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

/******************************************************
*               Function Declarations
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/

// RockyS: Some PWM are implemented with SCT, others are implemented with CT32
// Each PWM requires one counter, we currently use SCT.L16, SCT.H16 as 2 counters
// We have 2 SCT counters, and 4 CT32 counters, So At most 2 + 4 = 6 PWM channels!

/*static*/ uint8_t s_pwmConsts[MICO_PWM_MAX];	// 0 = no const, 1 = const Low, 2 = const High
/*static*/ uint8_t s_isSCTInited;

OSStatus _prvTmrInit( const platform_pwm_t* pwm, LPC_TIMER_T *pTmr, uint32_t freq, float dc)
{
	uint32_t busClk, inClk;
	uint32_t cycNdx, pwmNdx;
	OSStatus err = kNoErr;
	Chip_TIMER_Init(pTmr);
	pTmr->TCR = 0;
	pTmr->CTCR = 0;	// timer mode
	pTmr->TC = pTmr->PC = 0;

	pwmNdx = pwm->pwmNdx;
	cycNdx = (pwmNdx + 1) % 4;
	pTmr->PWMC = 1<<pwmNdx;

	// match 3 as frequency
	pTmr->MCR = 1<<(3*cycNdx + 1);	// Reset on cycle macth channel

	if (pTmr == LPC_TIMER0 || pTmr == LPC_TIMER1)
		inClk = Chip_Clock_GetAsyncSyscon_ClockRate();
	else
		inClk = SystemCoreClock;

	// >>> config
	{
		uint32_t div8Bit;
		uint32_t resolution, pos;

		if (freq < (inClk >> 16))
		{
			div8Bit = (inClk + (freq<<16) - 1) / (freq<<16);
		} else {
			div8Bit = 1;
		}
		require_action_quiet(div8Bit < 256, exit, err = kParamErr);

		busClk = inClk / div8Bit;
		resolution = busClk / freq;
		pos = resolution - (uint32_t) ((1.0f - dc) * resolution + 0.5);

		pTmr->PR = div8Bit - 1;
		pTmr->MR[cycNdx] = resolution - 1;
		pTmr->MR[pwmNdx] = pos;
	}

exit:
	return err;
}

void _prvTmrPWMStart(LPC_TIMER_T *pTmr)
{
	pTmr->TCR = 1;
}

void _prvTmrPWMStop(LPC_TIMER_T *pTmr)
{
	pTmr->TCR = 0 | 1<<1;	// disable and hold in reset
	/*
	if (pTmr == LPC_TIMER0)
		g_pASys->ASYNCAPBCLKCTRLSET = 1<<13;
	else if (pTmr == LPC_TIMER1)
		g_pASys->ASYNCAPBCLKCTRLSET = 1<<14;
	else if (pTmr == LPC_TIMER2)
		g_pSys->PRESETCTRLSET[1] = 1<<22;
	else if (pTmr == LPC_TIMER3)
		g_pSys->PRESETCTRLSET[1] = 1<<26;
	*/
}


void _prvSCTInit( const platform_pwm_t* pwm, LPC_SCT_T *pSCT)
{
	if (!s_isSCTInited)
	{
		s_isSCTInited = 1;

		Chip_SCT_Init(pSCT);
		//			   2x16bit | busClk | Reload L, H
		pSCT->CONFIG = 0UL<<0  | 0UL<<1 | 0<<7 | 0<<8;

		pSCT->REGMODE_L = pSCT->REGMODE_H = 0;	// match mode

		pSCT->LIMIT_L = pSCT->LIMIT_H = 1<<0;	// use match 0 for limit, both L16 and H16

		pSCT->EVEN = 1<<0 | 1<<1 | 1<<2 | 1<<3;	// Use Event 0, 1, 2, 3
		// We use only state 0
		// L16 uses event 0 as Limit, event 1 as pos
		pSCT->EVENT[0].STATE = 1<<0 , pSCT->EVENT[1].STATE = 1<<0;
		pSCT->EVENT[2].STATE = 1<<0 , pSCT->EVENT[3].STATE = 1<<0;
	}
}

// SCT.L16, use event 0 as limit, event 1 as position
// SCT.H16, use event 2 as limit, event 3 as position
// RockyS: Note that SCT has a bug, if only H16 is used w/o L16, events may failed to control output pins
OSStatus _prvSCTCfg(LPC_SCT_T *pSCT, uint32_t pwmNdx, uint32_t freq, float dc)
{
	OSStatus err = kNoErr;
	uint32_t div8Bit;
	uint32_t resolution, pos;
    uint16_t ctrlBkup;
    uint32_t busClk;
    uint32_t inClk;
	__IO uint16_t *pCtrlReg;
	__IO uint16_t *pMatLmtReg, *pMatPosReg;
	__IO uint16_t *pMatRldLmtReg, *pMatRldPosReg;

	// Calculate the maximum possible resolution
    inClk = SystemCoreClock;
	if (freq < (inClk >> 16))
	{
		div8Bit = (inClk + (freq<<16) - 1) / (freq<<16);
	} else {
		div8Bit = 1;
	}
	require_action_quiet(div8Bit < 256, exit, err = kParamErr);

	busClk = inClk / div8Bit;
	resolution = busClk / freq;
	pos = resolution - (uint32_t) ((1.0f - dc) * resolution + 0.5);

	if (pwmNdx < PWMNDX_SCTH_BASE) {
		pCtrlReg = &pSCT->CTRL_L , pMatLmtReg = &pSCT->MATCH[0].L , pMatPosReg = &pSCT->MATCH[1].L;
		pMatRldLmtReg = &pSCT->MATCHREL[0].L , pMatRldPosReg = &pSCT->MATCHREL[1].L;
	}else{
		pCtrlReg = &pSCT->CTRL_H , pMatLmtReg = &pSCT->MATCH[0].H , pMatPosReg = &pSCT->MATCH[1].H;
		pMatRldLmtReg = &pSCT->MATCHREL[0].H , pMatRldPosReg = &pSCT->MATCHREL[1].H;
	}

    ctrlBkup = (*pCtrlReg & ~(0xFF<<5)) | (div8Bit - 1) << 5;
	*pCtrlReg |= 1UL<<1 | 1UL<<2;
	*pCtrlReg |= 1UL<<3;
	*pMatLmtReg = *pMatRldLmtReg = resolution - 1;
	*pMatPosReg = *pMatRldPosReg = pos;

	if (pwmNdx < PWMNDX_SCTH_BASE)
	{
		// use event 0 as limit of L16, event 1 as position of L16
        // 					  MatchSel | L/H Sel | Output | pin index sel 						| TrgByMatch
		pSCT->EVENT[0].CTRL = 0UL      | 0<<4    | 1<<5   | (pwmNdx - PWMNDX_SCTL_BASE)<<6 | 1<<12;	// SCT.PWM0 (L16) limit
		pSCT->EVENT[1].CTRL = 1UL      | 0<<4    | 1<<5   | (pwmNdx - PWMNDX_SCTL_BASE)<<6 | 1<<12;	// SCT.PWM0 (L16) position
		pSCT->OUT[pwmNdx - PWMNDX_SCTL_BASE].CLR = 1<<0;
		pSCT->OUT[pwmNdx - PWMNDX_SCTL_BASE].SET = 1<<1;
	} else {
		// use event 2 as limit of L16, event 3 as position of L16
		pSCT->EVENT[2].CTRL = 0UL      | 1<<4    | 1<<5   | (pwmNdx - PWMNDX_SCTH_BASE)<<6 | 1<<12;	// SCT.PWM1 (H16) limit
		pSCT->EVENT[3].CTRL = 1UL      | 1<<4    | 1<<5   | (pwmNdx - PWMNDX_SCTH_BASE)<<6 | 1<<12;	// SCT.PWM1 (H16) position
		pSCT->OUT[pwmNdx - PWMNDX_SCTH_BASE].CLR = 1<<2;
		pSCT->OUT[pwmNdx - PWMNDX_SCTH_BASE].SET = 1<<3;
    }
	*pCtrlReg = ctrlBkup;
exit:
	return err;
}

void _prvSCTPWMStart(LPC_SCT_T *pSCT, uint32_t pwmNdx)
{
	if (pwmNdx < PWMNDX_SCTH_BASE) {
		pSCT->CTRL_L &= ~(1UL<<1 | 1UL<<2);
	}else{
		pSCT->CTRL_H &= ~(1UL<<1 | 1UL<<2);
	}
}

void _prvSCTPWMStop(LPC_SCT_T *pSCT, uint32_t pwmNdx)
{
	if (pwmNdx < PWMNDX_SCTH_BASE) {
		pSCT->CTRL_L |= (1UL<<1 | 1UL<<2 | 1UL<<3);		// Stop, halt, and clear count
	}else{
		pSCT->CTRL_H |= (1UL<<1 | 1UL<<2 | 1UL<<3);
	}
}

OSStatus platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle )
{
	OSStatus err  = kNoErr;
	require_action_quiet(pwm != 0 && duty_cycle <= 1.0f, exit, err = kParamErr);

	if (duty_cycle >= 0.99998) {
		// if duty-cycle = 1, then use GPIO to constantly pull high
		g_pIO->PIO[pwm->portNdx][pwm->pinNdx] = 0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF;
		g_pGP->DIR[pwm->portNdx] = 1UL << pwm->pinNdx;
		g_pGP->CLR[pwm->portNdx] = 1UL << pwm->pinNdx;
	}
	else
		g_pIO->PIO[pwm->portNdx][pwm->pinNdx] = pwm->pinMux | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF;

	if (pwm->pwmNdx >= PWMNDX_SCTL_BASE) {
		_prvSCTInit(pwm, pwm->pSCT);
		_prvSCTCfg(pwm->pSCT, pwm->pwmNdx, frequency, duty_cycle);
	} else {
		_prvTmrInit(pwm, pwm->pTMR, frequency, duty_cycle);
	}

exit:
	return err;
}
/* Wecan ToDo 2015.06.24 pwm_start */
OSStatus platform_pwm_start( const platform_pwm_t* pwm )
{
	OSStatus err = kNoErr;

	require_action_quiet( pwm != NULL, exit, err = kParamErr);

	if (pwm->pwmNdx >= PWMNDX_SCTL_BASE)
		_prvSCTPWMStart(pwm->pSCT, pwm->pwmNdx);
    else
        _prvTmrPWMStart(pwm->pTMR);

exit:
	return err;
}
/* Wecan ToDo 2015.06.24 pwm_stop */
OSStatus platform_pwm_stop( const platform_pwm_t* pwm )
{
	OSStatus err = kNoErr;

	require_action_quiet( pwm != NULL, exit, err = kParamErr);

	if (pwm->pwmNdx >= PWMNDX_SCTL_BASE)
		_prvSCTPWMStop(pwm->pSCT, pwm->pwmNdx);
    else
        _prvTmrPWMStop(pwm->pTMR);

exit:
	return err;
}


