#include "chip.h"
#include "power_control.h"
// #include "kernel_res_mgr.h"
// #include "CW_DEFINE.h"
// #include "CywWDT_RTOS.h"
// #include "DriverAPI.h"

#define DELAY_ENTER_SLEEP_MODE      (5000)

uint32_t osTimeStampGet(void);

PWRD_API_T const *g_pPWRD;
uint32_t g_CurrentFreq = 0;
Freq_CTRL_T g_freqCtrl = {0, IDLE_TASK};
/*
   Calling this function will cause the MCU enter low power mode specified by powerMode.
   Note: After entering low power mode, MCU need to be waked up by RTC interupt, So user should
        configure RTC properly before calling this function.
 */


#define WKTMROSC	 SYSCON_PDRUNCFG_PD_32K_OSC //(g_isRTCWorking ? SYSCON_PDRUNCFG_PD_32K_OSC : SYSCON_PDRUNCFG_PD_WDT_OSC)


#define USE_LP_REG

#define taskENTER_CRITICAL()
#define taskEXIT_CRITICAL()

#define LEAVE_LP_REG_MODE() g_pSys->PDRUNCFGCLR = SYSCON_PDRUNCFG_LP_VD1 | SYSCON_PDRUNCFG_LP_VD2 | SYSCON_PDRUNCFG_LP_VD3 | SYSCON_PDRUNCFG_LP_VD8
#define ENTER_LP_REG_MODE() g_pSys->PDRUNCFGSET = SYSCON_PDRUNCFG_LP_VD1 | SYSCON_PDRUNCFG_LP_VD2 | SYSCON_PDRUNCFG_LP_VD3 | SYSCON_PDRUNCFG_LP_VD8

typedef struct _DS_PwrPrm {
    uint32_t vd1;
    uint32_t vd8;
    uint32_t pllMult;
    SYSCON_FLASHTIM_T waitSts;
} DS_PwrPrm;

DS_PwrPrm g_pwrPrm;

// rocky: anti-PMU-bug, if multiple pins toggle at high frequency (E.g., SPI), actual voltage can be lower than specified voltage
#define HIGH_FREQ_PIN_TGL


uint32_t g_irqDisCnt;
void disable_interrupts(void)
{
    __disable_irq();
    g_irqDisCnt++;
}

void enable_interrupts(void)
{
    if (g_irqDisCnt == 1) {
        g_irqDisCnt = 0;
        __enable_irq();
    }
    else {
        g_irqDisCnt--;
    }

}

void PwrCtlStateReset(void)
{
    g_pPWRD = 0;
    g_CurrentFreq = 0;
    g_freqCtrl.currentFreq = 0 , g_freqCtrl.currentTask = IDLE_TASK;
}

int32_t _prvGetPwrPrm(uint32_t mult, DS_PwrPrm *pPrm)
{
    if ((mult >= 10) || (0 == mult)) {
        return -1L;
    }
    pPrm->pllMult = mult;
    switch (mult) {
    case 2:
    case 3:
        pPrm->waitSts = SYSCON_FLASH_2CYCLE;
        pPrm->vd1 = POWER_V0900;
        break;

    case 4:
    case 5:
    case 6:
        pPrm->waitSts = SYSCON_FLASH_4CYCLE;
		#ifndef HIGH_FREQ_PIN_TGL
        	pPrm->vd1 = POWER_V1000;
		#else
        	pPrm->vd1 = POWER_V1250;
		#endif
        break;

    case 7:
        pPrm->waitSts = SYSCON_FLASH_5CYCLE;
        #ifndef HIGH_FREQ_PIN_TGL
        	pPrm->vd1 = POWER_V1000;
        #else
        	pPrm->vd1 = POWER_V1300;
        #endif
        break;

    case 8:
        pPrm->waitSts = SYSCON_FLASH_5CYCLE;
		#ifndef HIGH_FREQ_PIN_TGL
        	pPrm->vd1 = POWER_V1100;
		#else
        	pPrm->vd1 = POWER_V1350;
		#endif

        break;

    default:
        return -1L;
    }
    pPrm->vd8 = pPrm->vd1;
    return 0;
}


unsigned int MainClkSelIRC()
{
    if (Chip_Clock_GetMainClockSource() != SYSCON_MAINCLKSRC_PLLIN) {
        Chip_Clock_SetMain_B_ClockSource(SYSCON_MAIN_B_CLKSRC_SYSPLLIN);
        Chip_Clock_SetSysClockDiv(1);
        if (Chip_Clock_GetSystemClockRate() == MCU_FREQ_UNIT) {
            // Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_IRC);
            /* trun off the PLL */
            Chip_SYSCON_PowerDown(SYSCON_PDRUNCFG_PD_SYS_PLL);
            Chip_FMC_SetFLASHAccess(FLASHTIM_20MHZ_CPU);
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    return TRUE;
}

uint32_t g_ioconSaves[40];
uint32_t g_dirSaves[2];
void ReleasePins(void)
{
    uint32_t i, j, ndx;
    // save IOCONs
    for (i = 0, j = 0, ndx = 0; ndx < 40; ndx++) {
        g_ioconSaves[ndx] = g_pIO->PIO[i][j];
        if (++j >= 32) {
            j = 0, i++;
        }
    }
    g_dirSaves[0] = g_pGP->DIR[0];
    g_dirSaves[1] = g_pGP->DIR[1];

    // release Red, Grn, Blue leds
    g_pIO->PIO[1][0] = 0x180;
    g_pIO->PIO[1][1] = 0x180;
    g_pIO->PIO[1][2] = 0x180;
    g_pGP->DIR[1] &= ~(1UL << 0 | 1UL << 1 | 1UL << 2);

    g_pGP->CLR[1] = 1UL << 0 | 1UL << 1 | 1UL << 2;
}

void RestorePins(void)
{
    uint32_t i, j, ndx;
    for (i = 0, j = 0, ndx = 0; ndx < 40; ndx++) {
        g_pIO->PIO[i][j] = g_ioconSaves[ndx];
        if (++j >= 32) {
            j = 0, i++;
        }
    }
    g_pGP->DIR[0] = g_dirSaves[0];
    g_pGP->DIR[1] = g_dirSaves[1];
}

extern volatile const uint8_t g_isI2CMUnderGo;
// Power Mode definitaton
/*
                typedef enum  power_mode_config {
                        PMU_SLEEP                  = 0,
                        PMU_DEEP_SLEEP             = 1,
                        PMU_POWERDOWN              = 2,
                        PMU_POWERDOWN_FLASH_RETAIN = 3,
                        PMU_DEEP_POWERDOWN         = 4
                  } power_mode_cfg_t;
 */
void PwrCtlLowPowerMode(unsigned int powerMode)
{

    /* check if user enabled voice trigger on Android side. If yes then go to partial power down mode */
    //--DbgPinCtl(1, 4, 0);
    //--DbgPinCtl(1, 4, 1);
    // makes I2C0,I2C1,I2C2 can wake up MCU from power down mode

    // if I2CM is ongoing, DMA may be used for transfer, so we only sleep, no power down

    g_pSys->STARTERSET[0] = 1UL<<21 | 1UL<<22 | 1UL<<23;

    {
        uint32_t ahbClkCtl = g_pSys->AHBCLKCTRL[0];
//      uint32_t p1_5iocon = LPC_IOCON->PIO[1][5];
//      uint32_t gpio0pin = LPC_GPIO->PIN[0];

//      if (osTimeStampGet()*1000 < 10000000 /*|| isPdmStart*/) {
//          powerMode = PMU_SLEEP;
//      }


        g_pSys->AHBCLKCTRLCLR[0] = 1UL << SYSCON_CLOCK_FLASH | 1UL << SYSCON_CLOCK_IOCON | 1UL <<
                                          SYSCON_CLOCK_GPIO0 | 1UL << SYSCON_CLOCK_GPIO1 | 1UL<<SYSCON_CLOCK_INPUTMUX;

        TaskProcNotify(MICO_TASK, 0);
        g_pSys->PDRUNCFGSET = 1UL << 10;        // shut down ADC

        if (powerMode == POWER_SLEEP) {
            // LPC_PWRD_API->power_mode_configure(powerMode, WKTMROSC, 1);
            __WFI();
        }
        else {
            // dbgk1 ReleasePins();
		    #ifdef USE_LP_REG
		            LEAVE_LP_REG_MODE();
					g_pPWRD->set_lpvd_level(POWER_LP_V0700,
					       POWER_LP_V1200,
					       POWER_LP_V1200,
					       POWER_LP_V1200,
					       POWER_FINE_LP_V_P100,
					       0,
					       0,
					       0,
					       0);
		    #endif

		    #ifdef EXTERNAL_RTC_CLOCK
		    g_pPWRD->power_mode_configure(
		        powerMode,
		        /*WKTMROSC |*/ SYSCON_PDRUNCFG_PD_SRAM0A | SYSCON_PDRUNCFG_PD_SRAM0B | SYSCON_PDRUNCFG_PD_SRAM1 |
		        SYSCON_PDRUNCFG_PD_SRAM2, 1);
		    #else
		    g_pPWRD->power_mode_configure(
		        powerMode,
		        WKTMROSC | SYSCON_PDRUNCFG_PD_SRAM0A | SYSCON_PDRUNCFG_PD_SRAM0B | SYSCON_PDRUNCFG_PD_SRAM1 |
		        SYSCON_PDRUNCFG_PD_SRAM2, 1);
		    #endif

		   #ifdef USE_LP_REG
				g_pPWRD->set_lpvd_level(POWER_LP_V1200,
				       POWER_LP_V1200,
				       POWER_LP_V1200,
				       POWER_LP_V1200,
				       POWER_FINE_LP_V_P100,
				       0,
				       0,
				       0,
				       0);
				ENTER_LP_REG_MODE();
		    #endif
            // dbgk1 RestorePins();
        }

        g_pSys->AHBCLKCTRL[0] = ahbClkCtl;
//      LPC_IOCON->PIO[1][5] = p1_5iocon;
//      if (gpio0pin & (1UL << 5)) {
//          LPC_GPIO->SET[0] = 1UL << 5;
//      }
//      else {
//          LPC_GPIO->CLR[0] = 1UL << 5;
//      }

    }
}

unsigned int TaskProcNotify(TASK_T task, uint32_t isOn)
{
    uint32_t freqReq;
    disable_interrupts();

    if (isOn) {
        g_freqCtrl.currentTask |= task;
    }
    else {
        g_freqCtrl.currentTask &= ~(task);
    }

    // determine max required frequency multiplier
    if (g_freqCtrl.currentTask & MICO_TASK) {
        freqReq = 8;    /* 8*12 = 96MHz */
        //g_pSys->SYSAHBCLKDIV = 1;    //6MHz
    }
    else {
        freqReq = 1;    /* 12 = 12MHz */
        //g_pSys->SYSAHBCLKDIV = 2;    //6MHz
    }

    if (freqReq != g_freqCtrl.currentFreq) {
        g_freqCtrl.currentFreq = freqReq;

/*
        if (freqReq != 1) {
            DbgPinCtl(0,12,1);
        }else{
            DbgPinCtl(0,12,0);
        }
        DbgPinCtl(1,3,1);
 */
        PwrCtlSetMcuFrequency(freqReq);
        //DbgPinCtl(1,3,0);
    }

    enable_interrupts();
    return 1;
}

/*
   parameter "freq" is in the unit of 12MHz, should be in the range of 1~8.
 */

// /////////////////////////////////////////////////////////////////////////////////////////
//
//  Note that this is a very cut down version of the PLL150 driver
//
//  It supports basic IRC integer multiplication
//
//  The MSEL value is limited to maximum M=16 and N is hardcoded to be a div by 2
// Also minimum M needs to be 2
//
//  This means that effectively means that we support only 5MHz input or above due to:
//     Fcco needing to be at least 75MHz and with N=2 and M=16 we get a multiply factor of 16 (M*2/N)
//     So minimal freq is 75MHz/16 =~ 4.7MHz
// //////////////////////////////////////////////////////////////////////////////////////////

void xSWinitSysPll(uint32_t multiply_by, uint32_t input_freq, bool poll_for_lock) {

    uint32_t cco_freq = input_freq * multiply_by;
    uint32_t pdec = 1;
    uint32_t mdec, ndec, selr, seli, selp;

    uint32_t directo = 1;

    while (cco_freq < 75000000) {
        multiply_by = multiply_by << 1; // double value in each iteration
        pdec = pdec << 1;                   // correspondingly double pdec to cancel effect of double msel
        cco_freq = input_freq * multiply_by;
    }

    selr = 0;
    seli = (multiply_by & 0x3c) + 4;
    selp = (multiply_by >> 1) + 1;

    if (pdec > 1) {
        directo = 0;// use post divider
        pdec = pdec / 2;// account for minus 1 encoding
        // /  Translate P value
        pdec             = (pdec == 1)  ? 0x62 :        // 1  * 2
                           (pdec == 2)  ? 0x42 :        // 2  * 2
                           (pdec == 4)  ? 0x02 :        // 4  * 2
                           (pdec == 8)  ? 0x0b :        // 8  * 2
                           (pdec == 16) ? 0x11 :        // 16 * 2
                           (pdec == 32) ? 0x08 : 0x08;  // 32 * 2
    }

    mdec = 0x7fff >> (16 - (multiply_by - 1));  // we only support values of 2 to 16 (to keep driver simple)
    ndec = 0x202;   // pre divide by 2 (hardcoded)

    g_pSys->SYSPLLCTRL        =  SYS_PLL_BANDSEL(1) | SYS_PLL_DIRECTI(0) |  SYS_PLL_DIRECTO(directo) |
                                    SYS_PLL_INSELR(selr) | SYS_PLL_INSELI(seli) | SYS_PLL_INSELP(selp);                                                                         // tbd
    g_pSys->SYSPLLPDEC        = pdec    | (1 << 7); // set Pdec value and assert preq
    g_pSys->SYSPLLNDEC        = ndec    | (1 << 10);// set Pdec value and assert preq
    g_pSys->SYSPLLSSCTRL[0] = (1 << 18) | (1 << 17) | mdec;   // select non sscg MDEC value, assert mreq and select mdec value
    g_pSys->PDRUNCFGCLR       = SYSCON_PDRUNCFG_PD_SYS_PLL;   // turn on PLL

    if (poll_for_lock) {
        while (g_pSys->SYSPLLSTAT != 1) {}  // sample lock
    }

}

unsigned int PwrCtlSetMcuFrequency(unsigned int freq)
{
    volatile uint32_t i = 0;
    // disable_interrupts();
    taskENTER_CRITICAL();
    // /*//rocky
    if (0 == g_pPWRD) {
        g_pPWRD = LPC_PWRD_API;
        /* set voltage levels properly for I2C slave event to wake the part */
        /* vd1     VD2          VD3    VD8        FINE LEVEL FOR 0.7 CHOICES   */
        g_pPWRD->set_lpvd_level(POWER_LP_V1200,
                                       POWER_LP_V1200,
                                       POWER_LP_V0700,
                                       POWER_LP_V1200,
                                       POWER_FINE_LP_V_P100,
                                       0,
                                       0,
                                       0,
                                       0);
        Chip_Clock_SetSysClockDiv(1);
    }
    // */

    /* check if we are already running at same or higher frequency than resquested.
       If yes, then don't change the frequency. If not update clock tree.
     */
    //    if ((freq != 1) && (g_CurrentFreq >= freq) )
    //      return TRUE;
    MainClkSelIRC();
    if (freq == 1) {
        Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_IRC);
        /* trun off the PLL */
        Chip_SYSCON_PowerDown(SYSCON_PDRUNCFG_PD_SYS_PLL);
        /* set flash wait states */
        LPC_SYSCON->FLASHCFG = 0x1A;
        g_pPWRD->set_vd_level(POWER_VD1, POWER_V0800 /*V0900*/, POWER_FINE_V_NONE);
        g_pPWRD->set_vd_level(POWER_VD8, POWER_V0900 /*V0900*/, POWER_FINE_V_NONE);
#if defined(USE_LP_REG)

        g_pPWRD->set_lpvd_level(POWER_LP_V1200,
                                       POWER_LP_V1200,
                                       POWER_LP_V1200,
                                       POWER_LP_V1200,
                                       POWER_FINE_LP_V_P100,
                                       0,
                                       0,
                                       0,
                                       0);
        ENTER_LP_REG_MODE();
#endif
    }
    else {
        // if (g_CurrentFreq >= freq)// TODO: if the frequency is not lowered to 1, frequency will keep in lvl8.
        // return 0;
        _prvGetPwrPrm(freq, &g_pwrPrm);
        #if defined(USE_LP_REG)
        LEAVE_LP_REG_MODE();
        #endif

        #if 0
        LPC_PMU_T *pPMU = LPC_PMU;
        pPMU->VDCTRL[0] = 9;
        pPMU->VDCTRL[1] = 11;
        pPMU->VDCTRL[2] = 11;
        pPMU->VDCTRL[3] = 9;
        #else
        g_pPWRD->set_vd_level(POWER_VD1, g_pwrPrm.vd1, POWER_FINE_V_NONE);
        g_pPWRD->set_vd_level(POWER_VD8, g_pwrPrm.vd8, POWER_FINE_V_NONE);
        #endif

#if 0
        Chip_Clock_SetupSystemPLL(freq, SYSCON_IRC_FREQ);

        /* Turn on the PLL by clearing the power down bit */
        Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_SYS_PLL);

        /* Wait for PLL to lock */

#else
        Chip_Clock_SetSystemPLLSource(SYSCON_PLLCLKSRC_IRC);

        /* Power down PLL to change the PLL divider ratio */
        Chip_SYSCON_PowerDown(SYSCON_PDRUNCFG_PD_SYS_PLL);

        /* VD2_ANA needs to be powered up too. According to the design team, it's powered up at chip reset. */
        if ( Is_Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_VD2_ANA) ) {
            Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_VD2_ANA);
        }
        xSWinitSysPll(16, 12000000, false);
        xSWinitSysPll(freq, 12000000, false);
        for (i = 0; i <= 100; i++) {// wait for lock to be relevant for new settings
        }
#endif
        while (!Chip_Clock_IsSystemPLLLocked()) {}

        Chip_Clock_SetSysClockDiv(1);
        /* set flash wait states */
        //      LPC_SYSCON->FLASHCFG = (g_pwrPrm.waitSts << 12) | (1 << 5) | 0x1A;
        Chip_FMC_SetFLASHAccess(g_pwrPrm.waitSts);

        Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_PLLOUT);
    }

    g_CurrentFreq = freq;
    SystemCoreClockUpdate();
    // enable_interrupts();
    taskEXIT_CRITICAL();
    return TRUE;
}
#if 0
void RTC_IRQHandler(void)
{
    uint32_t rtcStatus;

    // Board_LED_Toggle(1);

    /* Get RTC status register */
    rtcStatus = Chip_RTC_GetStatus(LPC_RTC);

    /* Check RTC 1KHz match interrupt */
    if (rtcStatus & RTC_CTRL_WAKE1KHZ) {
        /* RTC high resultiuon wakeup interrupt */
        //  Chip_RTC_SetWake(LPC_RTC, 500);
        //  DEBUGOUT("wake interrupt in\n");
        //  CwSetMcuFrequency(3);
        //   Chip_RTC_Disable(LPC_RTC);
        //WakeUpTaskFromRTC();
    }

    /* Check RTC 1hz match interrupt */
    if (rtcStatus & RTC_CTRL_ALARM1HZ) {
        /* Alarm */
        //  Chip_RTC_SetCount(LPC_RTC, 0);
        //  DEBUGOUT("RTC Alarm interrupt False Wakeup\n");
    }

    /* Clear only latched RTC status */
    Chip_RTC_ClearStatus(LPC_RTC, (rtcStatus & (RTC_CTRL_WAKE1KHZ | RTC_CTRL_ALARM1HZ)));
}

void RTCSetUp(void)
{
    /* Turn on the RTC 32K Oscillator by clearing the power down bit */
    if ( !Is_Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_32K_OSC) ) {
        Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_32K_OSC);
    }

    /* Enable the RTC oscillator, oscillator rate can be determined by
       calling Chip_Clock_GetRTCOscRate()   */
    Chip_Clock_EnableRTCOsc();

    // For X2 Pro external RTC clock enable, Powerdown 32K osillator
#ifdef EXTERNAL_RTC_CLOCK
    g_pSys->PDRUNCFGSET |= SYSCON_PDRUNCFG_PD_32K_OSC;
    g_pSys->PDSLEEPCFG |= SYSCON_PDRUNCFG_PD_32K_OSC;
#endif
    /* Initialize RTC driver (enables RTC clocking) */
    Chip_RTC_Init(LPC_RTC);

    /* Enable RTC as a peripheral wakeup event */
    Chip_SYSCON_EnableWakeup(SYSCON_STARTER_RTC);

    /* RTC reset */
    Chip_RTC_Reset(LPC_RTC);

    /* Start RTC at a count of 0 when RTC is disabled. If the RTC is enabled, you
       need to disable it before setting the initial RTC count. */
    Chip_RTC_Disable(LPC_RTC);
    Chip_RTC_SetCount(LPC_RTC, 0);

    /* Set a long alarm time so the interrupt won't trigger */
    Chip_RTC_SetAlarm(LPC_RTC, 2000);

    /* Enable RTC and high resolution timer - this can be done in a single
       call with Chip_RTC_EnableOptions(LPC_RTC, (RTC_CTRL_RTC1KHZ_EN | RTC_CTRL_RTC_EN)); */
    Chip_RTC_Enable1KHZ(LPC_RTC);
    Chip_RTC_Enable(LPC_RTC);

    /* Clear latched RTC interrupt statuses */
    Chip_RTC_ClearStatus(LPC_RTC, (RTC_CTRL_OFD | RTC_CTRL_ALARM1HZ | RTC_CTRL_WAKE1KHZ));

    /* Enable RTC interrupt */
    NVIC_EnableIRQ(RTC_IRQn);

    /* Enable RTC alarm interrupt */
    Chip_RTC_EnableWakeup(LPC_RTC, (RTC_CTRL_ALARMDPD_EN | RTC_CTRL_WAKEDPD_EN));

    //  // add by EricDing
    //  Chip_RTC_SetWake(LPC_RTC, 0xFFFF);
}


//#define prvSleep() PwrCtlLowPowerMode(PMU_SLEEP)
#define prvSleep() PwrCtlLowPowerMode(PMU_POWERDOWN)
/* Constants required to manipulate the core.  Registers first... */
#define portNVIC_SYSTICK_CTRL_REG           ( *( (volatile unsigned long *) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG           ( *( (volatile unsigned long *) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG  ( *( (volatile unsigned long *) 0xe000e018 ) )
#define portNVIC_SYSPRI2_REG                ( *( (volatile unsigned long *) 0xe000ed20 ) )
/* ...then bits in the registers. */
#define portNVIC_SYSTICK_CLK_BIT            ( 1UL << 2UL )
#define portNVIC_SYSTICK_INT_BIT            ( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT         ( 1UL << 0UL )
#define portNVIC_SYSTICK_COUNT_FLAG_BIT     ( 1UL << 16UL )
#define portNVIC_PENDSVCLEAR_BIT            ( 1UL << 27UL )
#define portNVIC_PEND_SYSTICK_CLEAR_BIT     ( 1UL << 25UL )

extern void __NXPWFI(void);

unsigned short ulGetExternalTime(void)
{
    return 0;
}

void prvStopTickInterruptTimer(void)
{
    portNVIC_SYSTICK_CTRL_REG =  portNVIC_SYSTICK_INT_BIT;
}

void prvStartTickInterruptTimer(void)
{
    portNVIC_SYSTICK_CTRL_REG =  portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT;
}

void vSetWakeTimeInterrupt(unsigned short xExpectedIdleTime)
{
    if (g_isRTCWorking) {
        Chip_RTC_SetWake(LPC_RTC, xExpectedIdleTime);
        Chip_RTC_Enable(LPC_RTC);
    }
    else {
        PwrCtlWdtSetTime(xExpectedIdleTime);
    }
}


volatile uint32_t sleepCnter = 0;

extern const uint8_t i2cslv_status;
void vApplicationSleep(unsigned short xExpectedIdleTime)
{
    unsigned long ulLowPowerTimeBeforeSleep, ulLowPowerTimeAfterSleep;
    eSleepModeStatus eSleepStatus;
    extern volatile bool isPdmStart;

    /* Let MCU can program the code when enable the low power mode */
    if(xTaskGetTickCount() < DELAY_ENTER_SLEEP_MODE)
        return;

    /* When system find the I2C Slave was accessed, it will return the low power process */
    // rocky: not sure if we are safe to enter low power mode when I2C master is stil working
    // but if we skip power down, pipeline mode current get much higher
    disable_interrupts();
    if(i2cslv_status/* || g_isI2CMUnderGo*/)
    {
        if (g_freqCtrl.currentTask != IDLE_TASK)
            TaskProcNotify(FUSION_TASK|VOICE_TASK,0);
        __WFI();
        enable_interrupts();
        return;
    }
    enable_interrupts();

    // make I2C0 and I2C1 can wake up from power down mode
    g_pSys->STARTERSET[0] = 1UL<<21 | 1UL<<22;

    // dbgk1_n

    /* Read the current time from a time source that will remain operational
       while the microcontroller is in a low power state. */
    ulLowPowerTimeBeforeSleep = xExpectedIdleTime;

    /* Stop the timer that is generating the tick interrupt. */
    prvStopTickInterruptTimer();

    /* Enter a critical section that will not effect interrupts bringing the MCU
       out of sleep mode. */
    disable_interrupts();

    /* Ensure it is still ok to enter the sleep mode. */
    eSleepStatus = eTaskConfirmSleepModeStatus();

    if ( eSleepStatus == eAbortSleep ) {
        /* A task has been moved out of the Blocked state since this macro was
           executed, or a context siwth is being held pending.  Do not enter a
           sleep state.  Restart the tick and exit the critical section. */

        prvStartTickInterruptTimer();
        enable_interrupts();
    }
    else {
        if ( eSleepStatus == eNoTasksWaitingTimeout ) {
            /* It is not necessary to configure an interrupt to bring the
               microcontroller out of its low power state at a fixed time in the
               future. */
            prvSleep();
        }
        else {
            /* Configure an interrupt to bring the microcontroller out of its low
               power state at the time the kernel next needs to execute.  The
               interrupt must be generated from a source that remains operational
               when the microcontroller is in a low power state. */
            vSetWakeTimeInterrupt(xExpectedIdleTime);

            /* Enter the low power state. */
            //g_pSys->SYSAHBCLKDIV = 1;    // 12MHz
            prvSleep();

            /* Determine how long the microcontroller was actually in a low power
               state for, which will be less than xExpectedIdleTime if the
               microcontroller was brought out of low power mode by an interrupt
               other than that configured by the vSetWakeTimeInterrupt() call.
               Note that the scheduler is suspended before
               portSUPPRESS_TICKS_AND_SLEEP() is called, and resumed when
               portSUPPRESS_TICKS_AND_SLEEP() returns.  Therefore no other tasks will
               execute until this function completes. */
            //   DEBUGOUT("wake up\n");

            if (g_isRTCWorking) {
                ulLowPowerTimeAfterSleep = Chip_RTC_GetWake(LPC_RTC);
            }
            else {
                ulLowPowerTimeAfterSleep = PwrCtlWdtGetExternalTime();
            }


            /* Correct the kernels tick count to account for the time the
               microcontroller spent in its low power state. */
            // >>> rocky 20150513: FreeRTOS 7.5.3 has a bug for tickless support, that, it stalls a tick then resumes delayed tasks
            // workaround is to call xTaskIncrementTick() manually and compensate the ulLowPowerTimeAfterSleep (substract by 1)
            xTaskIncrementTick();
            if(ulLowPowerTimeAfterSleep > 0)
                ulLowPowerTimeAfterSleep--;
            vTaskStepTick(ulLowPowerTimeBeforeSleep - ulLowPowerTimeAfterSleep);

            sleepCnter++;
        }

        /* Exit the critical section - it might be possible to do this immediately
           after the prvSleep() calls. */
        enable_interrupts();

        /* Restart the timer that is generating the tick interrupt. */
        prvStartTickInterruptTimer();
        //g_pSys->SYSAHBCLKDIV = 2;    //6MHz
    }
}

#endif


