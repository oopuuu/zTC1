/*
    FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#include "FreeRTOSConfig.h"
#include "hal_platform.h"

#if configUSE_TICKLESS_IDLE == 2

#include "FreeRTOS.h"
#include "task.h"
#include "port_tickless.h"
#include "hal_gpt.h"
#include "gpt.h"
#include "timer.h"
#include "hal_log.h"
#include "type_def.h"
#include "hal_lp.h"
#include "core_cm4.h"
#include "top.h"
#include "connsys_driver.h"
#include "hal_sleep_driver.h"
#include <string.h>
#include "hal.h"

#ifndef configSYSTICK_CLOCK_HZ
#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
/* Ensure the SysTick is clocked at the same frequency as the core. */
#define portNVIC_SYSTICK_CLK_BIT    ( 1UL << 2UL )
#else
/* The way the SysTick is clocked is not modified in case it is not the same
as the core. */
#define portNVIC_SYSTICK_CLK_BIT    ( 0 )
#endif

/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define portMISSED_COUNTS_FACTOR                        ( 45UL )


static hal_gpt_port_t wakeup_gpt_port = HAL_GPT_0;

static uint32_t ulTimerCountsForOneTick = 0;
static uint32_t ulStoppedTimerCompensation = 0;
static uint32_t xMaximumPossibleSuppressedTicks = 0;

uint8_t tickless_handle_index = 0xFF;

unsigned char AST_TimeOut_flag = 1;
unsigned int TMR0_ReloadCnt, CTP;

#define AST_CLOCK 32768 //32.768 kHz
#define xMaximumPossibleASTTicks (0xFFFFFFFF / (AST_CLOCK/configTICK_RATE_HZ))

#define TICKLESS_DEBUG 0

#if TICKLESS_DEBUG == 1
uint32_t bAbort = 0;
uint32_t workaround = 0;
static uint32_t ticklessDebug = 0;
#endif

extern int ticklessMode;
extern uint32_t ticklessCount;

void AST_vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements, ulAST_Reload_ms, ulAST_CurrentCount, ulAST_Current_ms = 0;
    TickType_t xModifiableIdleTime;

    //need ues AST
    /* Calculate the reload value required to wait xExpectedIdleTime
    tick periods.  -1 is used because this code will execute part way
    through one of the tick periods. */
    ulReloadValue = SysTick->VAL;
    if (ulReloadValue > ulStoppedTimerCompensation) {
        ulReloadValue -= ulStoppedTimerCompensation;
    }

    //Calculate total idle time to ms
    ulAST_Reload_ms = ulReloadValue / (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ);
    ulAST_Reload_ms = ulAST_Reload_ms + ((xExpectedIdleTime - 1) / (1000 / configTICK_RATE_HZ));

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    method as that will mask interrupts that should exit sleep mode. */
    __asm volatile("cpsid i");

    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
#if TICKLESS_DEBUG == 1
        bAbort = 1;
#endif
        /* Restart from whatever is left in the count register to complete
        this tick period. */
        SysTick->LOAD = SysTick->VAL;

        /* Restart SysTick. */
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        /* Reset the reload register to the value required for normal tick
        periods. */
        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;

        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile("cpsie i");

        return;
    } else {
        //Setup TMR0 and start timer
        hal_gpt_start_timer_ms(wakeup_gpt_port, ulAST_Reload_ms, HAL_GPT_TIMER_TYPE_ONE_SHOT);
        TMR0_ReloadCnt = ulAST_Reload_ms;
    }

    sleepdrv_run_suspend_cbs();

    /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
    set its parameter to 0 to indicate that its implementation contains
    its own wait for interrupt or wait for event instruction, and so wfi
    should not be executed again.  However, the original expected idle
    time variable must remain unmodified, so a copy is taken. */
    xModifiableIdleTime = xExpectedIdleTime;
    configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
    if (xModifiableIdleTime > 0) {
        ticklessCount++;
        /* Enable FW_OWN_BACK_INT interrupt */
        hal_lp_connsys_get_own_enable_int();

        /* Give connsys ownership to N9 */
        hal_lp_connsys_give_n9_own();

        /* Switch flash clock to XTAL */
        cmnSerialFlashClkConfToXtal();

        /* Switch MCU clock to XTAL */
        cmnCpuClkConfigureToXtal();

        /* Unclaim PLL usage */
        cmnPLL1OFF_PLL2OFF();

#ifdef HAL_WDT_PROTECTION_ENABLED
        hal_wdt_feed(HAL_WDT_FEED_MAGIC);
#endif

        if (ticklessMode == 1) {
            __asm volatile("dsb");
            __asm volatile("wfi");
            __asm volatile("isb");
        } else {
            hal_lp_legacy_sleep();
        }

#ifdef HAL_WDT_PROTECTION_ENABLED
        hal_wdt_feed(HAL_WDT_FEED_MAGIC);
#endif

        /* After wakeup from legacy sleep */
        /* Enable MCU clock to 192MHz */
        cmnCpuClkConfigureTo192M();

        /* Enable flash clock to 64MHz */
        cmnSerialFlashClkConfTo64M();

        /* re-init connsys for handling inband-cmd response */
        if (FALSE == connsys_get_ownership())
            log_hal_info("connsys_get_ownership fail\n");
    }
    configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

    sleepdrv_run_resume_cbs();

    /* Re-enable interrupts - see comments above the cpsid instruction()
    above. */
    __asm volatile("cpsie i");

    if (AST_TimeOut_flag != 0) {
        ulAST_CurrentCount = GPT_return_current_count(wakeup_gpt_port);
        hal_gpt_stop_timer(wakeup_gpt_port);

        /* The tick interrupt has already executed, and the SysTick
        count reloaded with ulReloadValue.	Reset the
        portNVIC_SYSTICK_LOAD_REG with whatever remains of this tick
        period. */

        //ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL ) - ( ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE_REG );

        /* Don't allow a tiny value, or values that have somehow
        underflowed because the post sleep hook did something
        that took too long. */
        /*if( ( ulCalculatedLoadValue < ulStoppedTimerCompensation ) || ( ulCalculatedLoadValue > ulTimerCountsForOneTick ) )
        {
        	ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL );
        }*/

        //portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;
        SysTick->LOAD = ulTimerCountsForOneTick;

        /* The tick interrupt handler will already have pended the tick
        processing in the kernel.  As the pending tick will be
        processed as soon as this function exits, the tick value
        maintained by the tick is stepped forward by one less than the
        time spent waiting. */
        ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
    } else {
        /* Something other than the tick interrupt ended the sleep.
        Work out how long the sleep lasted rounded to complete tick
        periods (not the ulReload value which accounted for part
        ticks). */
        ulAST_CurrentCount = GPT_return_current_count(wakeup_gpt_port);
        hal_gpt_stop_timer(wakeup_gpt_port);

        /* Calculate TRM0 count to ms */
        ulAST_Current_ms = ((ulAST_CurrentCount * 1000) / AST_CLOCK);
        ulCompletedSysTickDecrements = ulAST_Reload_ms - ulAST_Current_ms;

        /* How many complete tick periods passed while the processor
        was waiting? */
        ulCompleteTickPeriods = ulCompletedSysTickDecrements / ((1000 / configTICK_RATE_HZ));

        /* The reload value is set to whatever fraction of a single tick
        period remains. */
        SysTick->LOAD = ulTimerCountsForOneTick;
    }

    //workaround
    if (ulCompleteTickPeriods > ulAST_Reload_ms) {
#if TICKLESS_DEBUG == 1
        workaround = 1;
#endif
        ulCompleteTickPeriods = ulAST_Reload_ms;
    }

    /* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
    again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
    value.	The critical section is used to ensure the tick interrupt
    can only execute once in the case that the reload register is near
    zero. */
    CTP = ulCompleteTickPeriods;
    SysTick->VAL = 0UL;
    portENTER_CRITICAL();
    {
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        vTaskStepTick(ulCompleteTickPeriods);
        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;
    }
    portEXIT_CRITICAL();

#if TICKLESS_DEBUG == 1
    printf("xExpectedIdleTime = %d ms\n", xExpectedIdleTime);
    printf("bAbort = %d\n", bAbort);
    printf("workaround = %d\n", workaround);
    printf("AST_TimeOut_flag = %d\n", AST_TimeOut_flag);
    printf("ulAST_Current_ms = %u ms\n", ulAST_Current_ms);
    printf("ulAST_Reload_ms = %d ms\n", ulAST_Reload_ms);
    printf("ulCompleteTickPeriods = %d ms\n", ulCompleteTickPeriods);
    printf("ulReloadValue = %u\n\n", ulReloadValue);
    bAbort = 0;
    workaround = 0;
#endif

    AST_TimeOut_flag = 0;
}

void Tickless_GPT_CB(void* data)
{
    AST_TimeOut_flag = 1;
    hal_sleep_manager_unlock_sleep(tickless_handle_index);
}

void tickless_init()
{
    hal_gpt_init(wakeup_gpt_port);
    hal_gpt_register_callback(wakeup_gpt_port, Tickless_GPT_CB, NULL);

    if (tickless_handle_index == 0xFF)
        tickless_handle_index = hal_sleep_manager_set_sleep_handle("tickless");
}

void tickless_handler(TickType_t xExpectedIdleTime)
{
    uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements, ulSysTickCTRL;
    TickType_t xModifiableIdleTime;

    /* Stop the SysTick momentarily.  The time the SysTick is stopped for
    is accounted for as best it can be, but using the tickless mode will
    inevitably result in some tiny drift of the time maintained by the
    kernel with respect to calendar time. */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

#ifdef HAL_WDT_PROTECTION_ENABLED
    if (xExpectedIdleTime >= ((HAL_WDT_TIMEOUT_VALUE-1)*1000))
    {
        xExpectedIdleTime = (HAL_WDT_TIMEOUT_VALUE-1)*1000;
    }
#endif

    /* Make sure the SysTick reload value does not overflow the counter. */
    if ((xExpectedIdleTime > xMaximumPossibleSuppressedTicks) && (!hal_sleep_manager_is_sleep_locked())) {
        //xExpectedIdleTime = xMaximumPossibleSuppressedTicks;

        /* Make sure the AST reload value does not overflow the counter. */
        if (xExpectedIdleTime > xMaximumPossibleASTTicks) {
            xExpectedIdleTime = xMaximumPossibleASTTicks;
        }

        AST_vPortSuppressTicksAndSleep(xExpectedIdleTime);

        return;
    }

    if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks) {
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
    }

    /* Calculate the reload value required to wait xExpectedIdleTime
    tick periods.  -1 is used because this code will execute part way
    through one of the tick periods. */
    ulReloadValue = SysTick->VAL + (ulTimerCountsForOneTick * (xExpectedIdleTime - 1UL));
    if (ulReloadValue > ulStoppedTimerCompensation) {
        ulReloadValue -= ulStoppedTimerCompensation;
    }

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    method as that will mask interrupts that should exit sleep mode. */
    __asm volatile("cpsid i");

    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        /* Restart from whatever is left in the count register to complete
        this tick period. */
        SysTick->LOAD = SysTick->VAL;

        /* Restart SysTick. */
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        /* Reset the reload register to the value required for normal tick
        periods. */
        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;

        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile("cpsie i");
    } else {
        /* Set the new reload value. */
        SysTick->LOAD = ulReloadValue;

        /* Clear the SysTick count flag and set the count value back to
        zero. */
        SysTick->VAL = 0UL;

        /* Restart SysTick. */
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
        set its parameter to 0 to indicate that its implementation contains
        its own wait for interrupt or wait for event instruction, and so wfi
        should not be executed again.  However, the original expected idle
        time variable must remain unmodified, so a copy is taken. */
        xModifiableIdleTime = xExpectedIdleTime;
        configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
        if (xModifiableIdleTime > 0) {
            __asm volatile("dsb");
            __asm volatile("wfi");
            __asm volatile("isb");
        }
        configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

        /* Stop SysTick.  Again, the time the SysTick is stopped for is
        accounted for as best it can be, but using the tickless mode will
        inevitably result in some tiny drift of the time maintained by the
        kernel with respect to calendar time. */
        ulSysTickCTRL = SysTick->CTRL;
        SysTick->CTRL = (ulSysTickCTRL & ~SysTick_CTRL_ENABLE_Msk);

        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile("cpsie i");

        if ((ulSysTickCTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0) {
            uint32_t ulCalculatedLoadValue;

            /* The tick interrupt has already executed, and the SysTick
            count reloaded with ulReloadValue.  Reset the
            portNVIC_SYSTICK_LOAD_REG with whatever remains of this tick
            period. */
            ulCalculatedLoadValue = (ulTimerCountsForOneTick - 1UL) - (ulReloadValue - SysTick->VAL);

            /* Don't allow a tiny value, or values that have somehow
            underflowed because the post sleep hook did something
            that took too long. */
            if ((ulCalculatedLoadValue < ulStoppedTimerCompensation) || (ulCalculatedLoadValue > ulTimerCountsForOneTick)) {
                ulCalculatedLoadValue = (ulTimerCountsForOneTick - 1UL);
            }

            SysTick->LOAD = ulCalculatedLoadValue;

            /* The tick interrupt handler will already have pended the tick
            processing in the kernel.  As the pending tick will be
            processed as soon as this function exits, the tick value
            maintained by the tick is stepped forward by one less than the
            time spent waiting. */
            ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
        } else {
            /* Something other than the tick interrupt ended the sleep.
            Work out how long the sleep lasted rounded to complete tick
            periods (not the ulReload value which accounted for part
            ticks). */
            ulCompletedSysTickDecrements = (xExpectedIdleTime * ulTimerCountsForOneTick) - SysTick->VAL;

            /* How many complete tick periods passed while the processor
            was waiting? */
            ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

            /* The reload value is set to whatever fraction of a single tick
            period remains. */
            SysTick->LOAD = ((ulCompleteTickPeriods + 1) * ulTimerCountsForOneTick) - ulCompletedSysTickDecrements;
        }

        /* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
        again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
        value.  The critical section is used to ensure the tick interrupt
        can only execute once in the case that the reload register is near
        zero. */
        SysTick->VAL = 0UL;
        portENTER_CRITICAL();
        {
            SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
            vTaskStepTick(ulCompleteTickPeriods);
            SysTick->LOAD = ulTimerCountsForOneTick - 1UL;
        }
        portEXIT_CRITICAL();
    }
}

void vPortSetupTimerInterrupt(void)
{
    /* Calculate the constants required to configure the tick interrupt. */
#if configUSE_TICKLESS_IDLE == 2
    {
        ulTimerCountsForOneTick = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ);
        xMaximumPossibleSuppressedTicks = SysTick_LOAD_RELOAD_Msk / ulTimerCountsForOneTick;
        ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / (configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ);
    }
#endif /* configUSE_TICKLESS_IDLE */

    /* Configure SysTick to interrupt at the requested rate. */
    SysTick->LOAD = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

#endif /* configUSE_TICKLESS_IDLE */
