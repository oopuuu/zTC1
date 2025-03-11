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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM4F port.
 *----------------------------------------------------------*/

#include "../../../../Source/portable/GCC/mt2523/port_tickless.h"

#include "FreeRTOSConfig.h"

#if configUSE_TICKLESS_IDLE == 2
#include "hal_cm4_topsm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "memory_attribute.h"
#include "hal_sleep_manager.h"
#include "hal_ostd.h"
#include "core_cm4.h"
#include "hal_gpt.h"

//#define TICKLESS_DEEBUG_ENABLE
#ifdef  TICKLESS_DEEBUG_ENABLE
#define log_debug(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define log_debug(_message,...)
#endif

#ifndef configSYSTICK_CLOCK_HZ
#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
/* Ensure the SysTick is clocked at the same frequency as the core. */
#define portNVIC_SYSTICK_CLK_BIT	( 1UL << 2UL )
#else
/* The way the SysTick is clocked is not modified in case it is not the same
as the core. */
#define portNVIC_SYSTICK_CLK_BIT	( 0 )
#endif

/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define portMISSED_COUNTS_FACTOR			( 45UL )

/*
 * The number of SysTick increments that make up one tick period.
 */
static uint32_t ulTimerCountsForOneTick = 0;

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
static uint32_t xMaximumPossibleSuppressedTicks = 0;

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
static uint32_t ulStoppedTimerCompensation = 0;
#define DEEP_SLEEP_HW_WAKEUP_TIME 2
#define DEEP_SLEEP_SW_BACKUP_RESTORE_TIME 3

unsigned int log_f32k_time(int mode, int index);

void AST_vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    TickType_t xModifiableIdleTime;
    volatile static unsigned int ulAST_Reload_ms;
    uint32_t ulCompleteTickPeriods, ulCompletedSysTickDecrements;

    log_f32k_time(0, 0);

    //Calculate total idle time to ms
    ulAST_Reload_ms = (xExpectedIdleTime - 1) / (1000 / configTICK_RATE_HZ);
    ulAST_Reload_ms = ulAST_Reload_ms - DEEP_SLEEP_SW_BACKUP_RESTORE_TIME - DEEP_SLEEP_HW_WAKEUP_TIME;

    __asm volatile("cpsid i");

    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {

        SysTick->LOAD = SysTick->VAL;

        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;

        __asm volatile("cpsie i");

        return;
    } else {
        xModifiableIdleTime = xExpectedIdleTime;

        configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
        if (xModifiableIdleTime > 0) {
            hal_sleep_manager_set_sleep_time(ulAST_Reload_ms);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_DEEP_SLEEP);
        }
        configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

        log_f32k_time(0, 1);
        ulCompletedSysTickDecrements = log_f32k_time(1, 1);
        ulCompletedSysTickDecrements = (unsigned int)(((float)ulCompletedSysTickDecrements) / 32.768f); //unit : ms
        ulCompleteTickPeriods = ulCompletedSysTickDecrements / ((1000 / configTICK_RATE_HZ));

        //Limit OS Tick Compensation Value
        if (ulCompleteTickPeriods > (xExpectedIdleTime - 1)) {
            ulCompleteTickPeriods = xExpectedIdleTime - 1;
        }

        vTaskStepTick(ulCompleteTickPeriods);
        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

        __asm volatile("cpsie i");

        log_debug("\r\nEIT=%u\r\n"  , xExpectedIdleTime);
        log_debug("CSD=%d\r\n"      , ulCompletedSysTickDecrements);
        log_debug("RL=%u\r\n"       , ulAST_Reload_ms);
    }
}

#define MaximumIdleTime 20  //ms
void tickless_handler(uint32_t xExpectedIdleTime)
{
    uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements, ulSysTickCTRL;
    uint32_t xModifiableIdleTime;
    static uint32_t last_cpu_clock = 0, systick_scale;

    /* Stop the SysTick momentarily.  The time the SysTick is stopped for
    is accounted for as best it can be, but using the tickless mode will
    inevitably result in some tiny drift of the time maintained by the
    kernel with respect to calendar time. */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    if (configCPU_CLOCK_HZ != last_cpu_clock) {
        ulTimerCountsForOneTick = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ);
        xMaximumPossibleSuppressedTicks = SysTick_LOAD_RELOAD_Msk / ulTimerCountsForOneTick;
        ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / (configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ);

        if (configCPU_CLOCK_HZ == 26000000) {
            systick_scale = 16;
        } else {
            systick_scale = 64;
        }
        last_cpu_clock = configCPU_CLOCK_HZ;
    }

    if ((xExpectedIdleTime > (MaximumIdleTime * (1000 / configTICK_RATE_HZ))) && (hal_sleep_manager_is_sleep_locked() == 0)) {
        AST_vPortSuppressTicksAndSleep(xExpectedIdleTime);
        return;
    }

    /* Make sure the SysTick reload value does not overflow the counter. */
    if (xExpectedIdleTime > (xMaximumPossibleSuppressedTicks * systick_scale)) {
        xExpectedIdleTime = (xMaximumPossibleSuppressedTicks * systick_scale);
    }

    /* Calculate the reload value required to wait xExpectedIdleTime
    tick periods.  -1 is used because this code will execute part way
    through one of the tick periods. */
    //ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE_REG + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );
    ulReloadValue = (SysTick->VAL / systick_scale) + ((ulTimerCountsForOneTick / systick_scale) * (xExpectedIdleTime - 1UL));
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
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_IDLE);
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
            ulCompletedSysTickDecrements = (xExpectedIdleTime * ulTimerCountsForOneTick) - (SysTick->VAL * systick_scale);

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
            SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
        }
        portEXIT_CRITICAL();

        log_debug("\r\nST_CPT=%u\r\n"   , ulCompleteTickPeriods);
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

unsigned int log_f32k_time(int mode, int index)
{
#define LOG_F32K_TIME_SIZE 2
    volatile static unsigned int f32k[LOG_F32K_TIME_SIZE], i, result;

    //log F32K Count
    if (mode == 0) {
        f32k[index] = *CM4_TOPSM_F32K_CNT;
        return (0);
    }
    //diff Count
    if (mode == 1) {
        if (f32k[index] >= f32k[index - 1]) {
            result = f32k[index] - f32k[index - 1];
        } else {
            result = f32k[index] + (0xFFFFFFFF - f32k[index - 1]);
        }
        return (result);
    }

    if (mode == 255) {
        printf("\r\n log_f32k_time\r\n");
        for (i = 0; i < LOG_F32K_TIME_SIZE; i++) {
            log_debug("log_f32k_time[%d]: %d \r\n", i, f32k[i]);
        }
        for (i = 1; i < LOG_F32K_TIME_SIZE; i++) {
            log_debug("diff log_f32k_time[%d]: %d \r\n", i, (f32k[i] - f32k[i - 1]));
        }
    }
    return (0);
}

#endif

