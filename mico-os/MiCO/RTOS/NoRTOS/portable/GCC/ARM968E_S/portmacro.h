/**
 ******************************************************************************
 * @file    portmacro.c
 * @author  William Xu
 * @version V1.0.0
 * @date    06-Jun-2017
 * @brief   This file provide the NoRTOS portable MACROS
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2017 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Type definitions. */
#define portCHAR			char
#define portFLOAT			float
#define portDOUBLE			double
#define portLONG			long
#define portSHORT			short
#define portSTACK_TYPE		uint32_t
#define portBASE_TYPE		long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;


#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif
/*-----------------------------------------------------------*/

/* Hardware specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			8
#define portYIELD()					__asm ( "SWI 0" )
#define portNOP()                   __asm ( "NOP" )

/*-----------------------------------------------------------*/
/* Critical section handling. */
void vPortEnterCritical( void );
void vPortExitCritical( void );

#define portENTER_CRITICAL()	    vPortEnterCritical()
#define portEXIT_CRITICAL()			vPortExitCritical()

/*
 * Enable Interrupts
 */	
#define portENABLE_IRQ()					\
	({							              \
		unsigned long temp;				\
		__asm volatile(					\
		"mrs	%0, cpsr		@ local_irq_enable\n"	\
	       "bic	%0, %0, #0x80\n"					\
	       "msr	cpsr_c, %0"					       \
		: "=r" (temp)						       \
		:							              \
		: "memory");						       \
	})
#define portENABLE_FIQ()					\
	({							              \
		unsigned long temp;				\
		__asm volatile(					\
		"mrs	%0, cpsr		@ local_irq_enable\n"	\
	       "bic	%0, %0, #0x40\n"					\
	       "msr	cpsr_c, %0"					       \
		: "=r" (temp)						       \
		:							              \
		: "memory");						       \
	})

extern uint8_t platform_is_in_interrupt_context( void );
extern uint32_t platform_is_in_fiq_context( void );
	
#define portENABLE_INTERRUPTS()			do{		\
			if(!platform_is_in_interrupt_context())\
										    	portENABLE_IRQ();\
			if(!platform_is_in_fiq_context())\
										    	portENABLE_FIQ();\
										    }while(0)
										    
/*
 * Disable Interrupts
 */
static inline  int portDISABLE_FIQ(void)
{						                     
	unsigned long temp;				       
	unsigned long mask;		
	
	__asm volatile(					
	"mrs	%1, cpsr		@ local_irq_disable\n"	
	"orr	%0, %1, #0x40\n"					
	"msr	cpsr_c, %0"					       
	: "=r" (temp),"=r" (mask)						       
	:							              
	: "memory");		

	return (!!(mask & 0x40));
}

static inline  int portDISABLE_IRQ(void)
{						                     
	unsigned long temp;				       
	unsigned long mask;		
	
	__asm volatile(					
	"mrs	%1, cpsr		@ local_irq_disable\n"	
	"orr	%0, %1, #0x80\n"					
	"msr	cpsr_c, %0"					       
	: "=r" (temp),"=r" (mask)						       
	:							              
	: "memory");		

	return (!!(mask & 0x80));
}
	
#define portDISABLE_INTERRUPTS()		do{		\
										    	portDISABLE_FIQ();\
										    	portDISABLE_IRQ();\
										    }while(0)

#define DISABLE_INTERRUPTS() portDISABLE_INTERRUPTS()
#define ENABLE_INTERRUPTS()  portENABLE_INTERRUPTS()

	
/*-----------------------------------------------------------*/
/* Task utilities. */
#define portEND_SWITCHING_ISR( xSwitchRequired ) 	\
{													\
extern void vTaskSwitchContext( void ); 			\
													\
	if( xSwitchRequired )							\
	{												\
		vTaskSwitchContext();						\
	}												\
}

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void * pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void * pvParameters )

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
// eof
