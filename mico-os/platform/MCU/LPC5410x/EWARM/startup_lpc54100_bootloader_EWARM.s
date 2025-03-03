;/*****************************************************************************
; * @file:    startup_LPC5410x.s
; * @purpose: CMSIS Cortex-M4/M0+ Core Device Startup File
; *           for the NXP LPC5410x Device Series (manually edited)
; * @version: V1.00
; * @date:    19. October 2009
; *----------------------------------------------------------------------------
; *
; * Copyright (C) 2009 ARM Limited. All rights reserved.
; *
; * ARM Limited (ARM) is supplying this software for use with Cortex-Mx
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  hard_fault_handler_c
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler             ; Reset Handler
        DCD     Default_Handler         ; NMI Handler
        DCD     Default_Handler         ; Hard Fault Handler
        DCD     Default_Handler         ; MPU Fault Handler
        DCD     Default_Handler         ; Bus Fault Handler
        DCD     Default_Handler         ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     Default_Handler         ; SVCall Handler
        DCD     Default_Handler         ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     Default_Handler         ; PendSV Handler
        DCD     Default_Handler         ; SysTick Handler

        ; External Interrupts
        DCD     WDT_IRQHandler              ; Watchdog
        DCD     BOD_IRQHandler              ; Brown Out Detect
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     DMA_IRQHandler              ; DMA Controller
        DCD     GINT0_IRQHandler            ; GPIO Group0 Interrupt
        DCD     PIN_INT0_IRQHandler         ; PIO INT0
        DCD     PIN_INT1_IRQHandler         ; PIO INT1
        DCD     PIN_INT2_IRQHandler         ; PIO INT2
        DCD     PIN_INT3_IRQHandler         ; PIO INT3
        DCD     UTICK_IRQHandler            ; UTICK timer
        DCD     MRT_IRQHandler              ; Multi-Rate Timer
        DCD     CT32B0_IRQHandler           ; CT32B0
        DCD     CT32B1_IRQHandler           ; CT32B1
        DCD     CT32B2_IRQHandler           ; CT32B2
        DCD     CT32B3_IRQHandler           ; CT32B3
        DCD     CT32B4_IRQHandler           ; CT32B4
        DCD     SCT0_IRQHandler             ; Smart Counter Timer
        DCD     UART0_IRQHandler            ; UART0
        DCD     UART1_IRQHandler            ; UART1
        DCD     UART2_IRQHandler            ; UART2
        DCD     UART3_IRQHandler            ; UART3
        DCD     I2C0_IRQHandler             ; I2C0 controller
        DCD     I2C1_IRQHandler             ; I2C1 controller
        DCD     I2C2_IRQHandler             ; I2C2 controller
        DCD     SPI0_IRQHandler             ; SPI0 controller
        DCD     SPI1_IRQHandler             ; SPI1 controller
        DCD     ADC_SEQA_IRQHandler         ; ADC0 A sequence (A/D Converter) interrupt
        DCD     ADC_SEQB_IRQHandler         ; ADC0 B sequence (A/D Converter) interrupt
        DCD     ADC_THCMP_IRQHandler        ; ADC THCMP and OVERRUN ORed
        DCD     RTC_IRQHandler              ; RTC Timer
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     MAILBOX_IRQHandler          ; Mailbox
        DCD     GINT1_IRQHandler            ; GPIO Group1 Interrupt
        DCD     PIN_INT4_IRQHandler         ; PIO INT4
        DCD     PIN_INT5_IRQHandler         ; PIO INT5
        DCD     PIN_INT6_IRQHandler         ; PIO INT6
        DCD     PIN_INT7_IRQHandler         ; PIO INT7
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     RIT_IRQHandler              ; RITimer
        DCD     Reserved41_IRQHandler       ; Reserved
        DCD     Reserved42_IRQHandler       ; Reserved
        DCD     Reserved43_IRQHandler       ; Reserved
        DCD     Reserved44_IRQHandler       ; Reserved

        SECTION .intvec_RAM:CODE:ROOT(2)

        EXTERN  __iar_program_start
        PUBLIC  __vector_table_RAM

        DATA
__vector_table_RAM
        DCD     sfe(CSTACK)
        DCD     Reset_Handler             ; Reset Handler

        DCD     NMI_Handler
        DCD     HardFault_Handler
        DCD     MemManage_Handler
        DCD     BusFault_Handler
        DCD     UsageFault_Handler
        DCD     0							; Checksum of the first 7 words
        DCD     0
        DCD     0							; Enhanced image marker, set to 0x0 for legacy boot
        DCD     0							; Pointer to enhanced boot block, set to 0x0 for legacy boot
        DCD     SVC_Handler
        DCD     DebugMon_Handler
        DCD     0
        DCD     PendSV_Handler
        DCD     SysTick_Handler

        ; External Interrupts
        DCD     WDT_IRQHandler              ; Watchdog
        DCD     BOD_IRQHandler              ; Brown Out Detect
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     DMA_IRQHandler              ; DMA Controller
        DCD     GINT0_IRQHandler            ; GPIO Group0 Interrupt
        DCD     PIN_INT0_IRQHandler         ; PIO INT0
        DCD     PIN_INT1_IRQHandler         ; PIO INT1
        DCD     PIN_INT2_IRQHandler         ; PIO INT2
        DCD     PIN_INT3_IRQHandler         ; PIO INT3
        DCD     UTICK_IRQHandler            ; UTICK timer
        DCD     MRT_IRQHandler              ; Multi-Rate Timer
        DCD     CT32B0_IRQHandler           ; CT32B0
        DCD     CT32B1_IRQHandler           ; CT32B1
        DCD     CT32B2_IRQHandler           ; CT32B2
        DCD     CT32B3_IRQHandler           ; CT32B3
        DCD     CT32B4_IRQHandler           ; CT32B4
        DCD     SCT0_IRQHandler             ; Smart Counter Timer
        DCD     UART0_IRQHandler            ; UART0
        DCD     UART1_IRQHandler            ; UART1
        DCD     UART2_IRQHandler            ; UART2
        DCD     UART3_IRQHandler            ; UART3
        DCD     I2C0_IRQHandler             ; I2C0 controller
        DCD     I2C1_IRQHandler             ; I2C1 controller
        DCD     I2C2_IRQHandler             ; I2C2 controller
        DCD     SPI0_IRQHandler             ; SPI0 controller
        DCD     SPI1_IRQHandler             ; SPI1 controller
        DCD     ADC_SEQA_IRQHandler         ; ADC0 A sequence (A/D Converter) interrupt
        DCD     ADC_SEQB_IRQHandler         ; ADC0 B sequence (A/D Converter) interrupt
        DCD     ADC_THCMP_IRQHandler        ; ADC THCMP and OVERRUN ORed
        DCD     RTC_IRQHandler              ; RTC Timer
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     MAILBOX_IRQHandler          ; Mailbox
        DCD     GINT1_IRQHandler            ; GPIO Group1 Interrupt
        DCD     PIN_INT4_IRQHandler         ; PIO INT4
        DCD     PIN_INT5_IRQHandler         ; PIO INT5
        DCD     PIN_INT6_IRQHandler         ; PIO INT6
        DCD     PIN_INT7_IRQHandler         ; PIO INT7
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     Reserved_IRQHandler         ; Reserved
        DCD     RIT_IRQHandler              ; RITimer
        DCD     Reserved41_IRQHandler       ; Reserved
        DCD     Reserved42_IRQHandler       ; Reserved
        DCD     Reserved43_IRQHandler       ; Reserved
        DCD     Reserved44_IRQHandler       ; Reserved

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;

        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
; Reset Handler - shared for both cores
Reset_Handler
        LDR		r0, =SystemInit
        BLX		r0
        LDR		r0, =__iar_program_start
        BX		r0

; For cores with SystemInit() or __iar_program_start(), the code will sleep the MCU

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        PUBWEAK SystemInit
        SECTION .text:CODE:REORDER(1)
SystemInit
        BX		LR


        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
	 TST LR, #4
	 ITE EQ
	 MRSEQ R0, MSP
	 MRSNE R0, PSP
	 B hard_fault_handler_c

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
         B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

	PUBWEAK Reserved_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Reserved_IRQHandler
        B .

		PUBWEAK WDT_IRQHandler              ; Watchdog
		PUBWEAK BOD_IRQHandler              ; Brown Out Detect
		PUBWEAK DMA_IRQHandler              ; DMA Controller
		PUBWEAK GINT0_IRQHandler            ; GPIO Group0 Interrupt
		PUBWEAK PIN_INT0_IRQHandler         ; PIO INT0
		PUBWEAK PIN_INT1_IRQHandler         ; PIO INT1
		PUBWEAK PIN_INT2_IRQHandler         ; PIO INT2
		PUBWEAK PIN_INT3_IRQHandler         ; PIO INT3
		PUBWEAK UTICK_IRQHandler            ; UTICK timer
		PUBWEAK MRT_IRQHandler              ; Multi-Rate Timer
		PUBWEAK CT32B0_IRQHandler           ; CT32B0
		PUBWEAK CT32B1_IRQHandler           ; CT32B1
		PUBWEAK CT32B2_IRQHandler           ; CT32B2
		PUBWEAK CT32B3_IRQHandler           ; CT32B3
		PUBWEAK CT32B4_IRQHandler           ; CT32B4
		PUBWEAK UART0_IRQHandler            ; UART0
		PUBWEAK SCT0_IRQHandler             ; Smart Counter Timer
		PUBWEAK UART1_IRQHandler            ; UART1
		PUBWEAK UART2_IRQHandler            ; UART2
		PUBWEAK UART3_IRQHandler            ; UART3
		PUBWEAK I2C0_IRQHandler             ; I2C0 controller
		PUBWEAK I2C1_IRQHandler             ; I2C1 controller
		PUBWEAK I2C2_IRQHandler             ; I2C2 controller
		PUBWEAK SPI0_IRQHandler             ; SPI0 controller
		PUBWEAK SPI1_IRQHandler             ; SPI1 controller
		PUBWEAK ADC_SEQA_IRQHandler         ; ADC0 A sequence (A/D Converter) interrupt
		PUBWEAK ADC_SEQB_IRQHandler         ; ADC0 B sequence (A/D Converter) interrupt
		PUBWEAK ADC_THCMP_IRQHandler        ; ADC THCMP and OVERRUN ORed
		PUBWEAK RTC_IRQHandler              ; RTC Timer
		PUBWEAK MAILBOX_IRQHandler          ; Mailbox
		PUBWEAK GINT1_IRQHandler            ; GPIO Group1 Interrupt
		PUBWEAK PIN_INT4_IRQHandler         ; PIO INT4
		PUBWEAK PIN_INT5_IRQHandler         ; PIO INT5
		PUBWEAK PIN_INT6_IRQHandler         ; PIO INT6
		PUBWEAK PIN_INT7_IRQHandler         ; PIO INT7
		PUBWEAK RIT_IRQHandler              ; RITimer
		PUBWEAK Reserved41_IRQHandler       ; Reserved
		PUBWEAK Reserved42_IRQHandler       ; Reserved
		PUBWEAK Reserved43_IRQHandler       ; Reserved
		PUBWEAK Reserved44_IRQHandler       ; Reserved

WDT_IRQHandler              ; Watchdog
BOD_IRQHandler              ; Brown Out Detect
DMA_IRQHandler              ; DMA Controller
GINT0_IRQHandler            ; GPIO Group0 Interrupt
PIN_INT0_IRQHandler         ; PIO INT0
PIN_INT1_IRQHandler         ; PIO INT1
PIN_INT2_IRQHandler         ; PIO INT2
PIN_INT3_IRQHandler         ; PIO INT3
UTICK_IRQHandler            ; UTICK timer
MRT_IRQHandler              ; Multi-Rate Timer
CT32B0_IRQHandler           ; CT32B0
CT32B1_IRQHandler           ; CT32B1
CT32B2_IRQHandler           ; CT32B2
CT32B3_IRQHandler           ; CT32B3
CT32B4_IRQHandler           ; CT32B4
UART0_IRQHandler            ; UART0
SCT0_IRQHandler             ; Smart Counter Timer
UART1_IRQHandler            ; UART1
UART2_IRQHandler            ; UART2
UART3_IRQHandler            ; UART3
I2C0_IRQHandler             ; I2C0 controller
I2C1_IRQHandler             ; I2C1 controller
I2C2_IRQHandler             ; I2C2 controller
SPI0_IRQHandler             ; SPI0 controller
SPI1_IRQHandler             ; SPI1 controller
ADC_SEQA_IRQHandler         ; ADC0 A sequence (A/D Converter) interrupt
ADC_SEQB_IRQHandler         ; ADC0 B sequence (A/D Converter) interrupt
ADC_THCMP_IRQHandler        ; ADC THCMP and OVERRUN ORed
RTC_IRQHandler              ; RTC Timer
MAILBOX_IRQHandler          ; Mailbox
GINT1_IRQHandler            ; GPIO Group1 Interrupt
PIN_INT4_IRQHandler         ; PIO INT4
PIN_INT5_IRQHandler         ; PIO INT5
PIN_INT6_IRQHandler         ; PIO INT6
PIN_INT7_IRQHandler         ; PIO INT7
RIT_IRQHandler              ; RITimer
Reserved41_IRQHandler       ; Reserved
Reserved42_IRQHandler       ; Reserved
Reserved43_IRQHandler       ; Reserved
Reserved44_IRQHandler       ; Reserved

Default_Handler:
        B .
        END
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
