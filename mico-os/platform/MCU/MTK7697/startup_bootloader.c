/*
 * startup_bl.c
 *
 *  Created on: Jul 28, 2016
 *      Author: swyang
 */

#include "hal.h"

#define CACHE_CTRL_BASE     ((volatile unsigned long *)0x01530000)
#define CACHE_CHANNEL_EN    ((volatile unsigned long *)0x0153002C)
#define CACHE_ENTRY_START   ((volatile unsigned long *)0x01540000)
#define CACHE_ENTRY_END     ((volatile unsigned long *)0x01540040)

#define CACHE_DISABLE       (0x00000000)
#define CACHE_ENABLE        (0x0000030D)
#define CACHE_START_ADDR    (0x10000100)
#define CACHE_END_ADDR      (0x11000000)
/*
 * Symbols defined in linker script.
 */
extern unsigned long _text_start;
extern unsigned long _text_end;
extern unsigned long _text_lma_start;
extern unsigned long _data_start;
extern unsigned long _data_end;
extern unsigned long _data_lma_start;
extern unsigned long _bss_start;
extern unsigned long _bss_end;
extern unsigned long _bss_lma_start;

static unsigned long KernelStack[2048];

void ResetISR(void);
void NMIISR(void);
void FaultISR(void);
void ExpDefHandler(void);
extern void SysTick_Handler(void);

extern hal_nvic_status_t isrC_main(void);


/*
 * ARM CM4 Exception Vectors
 */
__attribute__ ((section(".except_vectors"))) unsigned long __isr_vector[47] =
{
    ((unsigned long)(KernelStack)) + sizeof(KernelStack),                 // Master stack pointer(MSP)
    (unsigned long) &_text_lma_start + sizeof(__isr_vector) + 1 ,         // 1, Reset, in XIP, +1 is for thumb mode
    (unsigned long) NMIISR,                                               // 2, NMI
    (unsigned long) FaultISR,                                             // 3, Hard fault
    (unsigned long) ExpDefHandler,                                        // 4, MPU fault
    (unsigned long) ExpDefHandler,                                        // 5, Bus fault
    (unsigned long) ExpDefHandler,                                        // 6, Usage fault
    (unsigned long) 0,                                                    // 7, Reserved
    (unsigned long) 0,                                                    // 8, Reserved
    (unsigned long) 0,                                                    // 9, Reserved
    (unsigned long) 0,                                                    // 10, Reserved
    (unsigned long) ExpDefHandler,                                        // 11, SVCall
    (unsigned long) ExpDefHandler,                                        // 12, Debug monitor
    (unsigned long) 0,                                                    // 13, Reserved
    (unsigned long) ExpDefHandler,                                        // 14, PendSV
    (unsigned long) SysTick_Handler,                                      // 15, Systick

    // External Interrupts
    (unsigned long) isrC_main,        // 16: UART1
    (unsigned long) isrC_main,        // 17: DMA
    (unsigned long) isrC_main,        // 18: HIF
    (unsigned long) isrC_main,        // 19: I2C1
    (unsigned long) isrC_main,        // 20: I2C2
    (unsigned long) isrC_main,        // 21: UART2
    (unsigned long) isrC_main,        // 22: MTK_CRYPTO
    (unsigned long) isrC_main,        // 23: SF
    (unsigned long) isrC_main,        // 24: EINT
    (unsigned long) isrC_main,        // 25: BTIF
    (unsigned long) isrC_main,        // 26: WDT
    (unsigned long) isrC_main,        // 27: reserved
    (unsigned long) isrC_main,        // 28: SPI_SLAVE
    (unsigned long) isrC_main,        // 29: WDT_N9
    (unsigned long) isrC_main,        // 30: ADC
    (unsigned long) isrC_main,        // 31: IRDA_TX
    (unsigned long) isrC_main,        // 32: IRDA_RX
    (unsigned long) isrC_main,        // 33: USB_VBUS_ON
    (unsigned long) isrC_main,        // 34: USB_VBUS_OFF
    (unsigned long) isrC_main,        // 35: timer_hit
    (unsigned long) isrC_main,        // 36: GPT3
    (unsigned long) isrC_main,        // 37: alarm_hit
    (unsigned long) isrC_main,        // 38: AUDIO
    (unsigned long) isrC_main,        // 39: n9_cm4_sw_irq
    (unsigned long) isrC_main,        // 40: GPT4
    (unsigned long) isrC_main,        // 41: adc_comp_irq
    (unsigned long) isrC_main,        // 42: reserved
    (unsigned long) isrC_main,        // 43: SPIM
    (unsigned long) isrC_main,        // 44: USB
    (unsigned long) isrC_main,        // 45: UDMA
    (unsigned long) isrC_main,        // 46: TRNG
};

/*
 * Using specific section(.reset_isr) to mark this routine in ld-script to arrange its location
 * (to behind the vector table)
 */
__attribute__ ((section(".reset_isr"))) void ResetISR(void)
{
    unsigned long current_pc;
    /*
     *  The local variables in ResetISR() are cleared to zero.
     */
    unsigned long *src = 0;
    unsigned long *dst = 0;
    unsigned long *end = 0;

    *CACHE_CTRL_BASE    = CACHE_DISABLE;
    *CACHE_ENTRY_START  = CACHE_START_ADDR;
    *CACHE_ENTRY_END    = CACHE_END_ADDR;
    *CACHE_CHANNEL_EN   = 1;
    *CACHE_CTRL_BASE    = CACHE_ENABLE;

    /*
     * Loader code: Copy both text and data sections from flash to SYSRAM.
     */
    /*
     * This routine(ResetISR()) is running on Flash(XIP), although its VMA is resident in SYSRAM.
     *
     * The reason that it can run safely is we assume GCC generates PIC code for following code.
     *
     * For example, the following "ldr" instruction are "PC related" based addressing, after GCC compiles the
     * C statement.

            unsigned long text_sect_len = (unsigned long)&_text_end - (unsigned long)&_text_start;

            2000040e:       4a22            ldr     r2, [pc, #136]  ; (20000498 <done+0xa>)
            20000410:       4b22            ldr     r3, [pc, #136]  ; (2000049c <done+0xe>)
            20000412:       1ad3            subs    r3, r2, r3
            20000414:       607b            str     r3, [r7, #4]

     * Another way to run it safely on XIP is using pure assembly instead of C code.
     *
         */

    unsigned long text_sect_len = (unsigned long)&_text_end - (unsigned long)&_text_start;
    unsigned long data_sect_len = (unsigned long)&_data_end - (unsigned long)&_data_start;

    //*((volatile unsigned int *)(0x83050030)) = 0x3;

    /* Get current program counter to judge whether if we are resident in SYSRAM or others */
    __asm volatile(
        /* update MSP */
        "       bl    getpc            \n\t"
        "getpc:                        \n\t"
        "       mov    %0, r14         \n\t"
                : "=r" (current_pc) ::
        );
    if(current_pc > (unsigned long)&_text_start && current_pc < ((unsigned long)&_text_start+text_sect_len)){
        /* do nothing, we may be resident in SYSRAM(VMA) already. */
    }else{
        /* resident in flash, do self-relocation */
        src = (unsigned long *)&_text_lma_start;
        dst = (unsigned long *)&_text_start;
        end = (unsigned long *)((unsigned char *)(&_text_lma_start) + text_sect_len);

        for(;src < end; src++, dst++)
            *dst = *src;

        src = (unsigned long *)&_data_lma_start;
        dst = (unsigned long *)&_data_start;
        end = (unsigned long *)((unsigned char *)(&_data_lma_start) + data_sect_len);

        for(; src < end; src++, dst++)
            *dst = *src;
    }

    /*
     * Assume the relocation is performed.
         * Trigger CM4 CPU invalidate internal buffer/cache
     */
    __asm volatile( "dsb" );
    __asm volatile( "isb" );

    /* BSS/ZI section */
    /*
     * Can't write in C to clear BSS section because the local stack variables of
     * this function are also resident in BSS section(kernel stack) actually, and the clear
     * action would clear these local variables to zero unexpectedly, and lead
     * to infinite loop.
     *
     * So using cpu registers operation to complete it.
     */
    __asm volatile(
        "    ldr    r0, =_bss_start \n\t"
        "    ldr    r1, =_bss_end   \n\t"
        "loop:                      \n\t"
        "    cmp    r0, r1          \n\t"
        "    bcc    clear           \n\t"
        "    b      done            \n\t"
        "clear:                     \n\t"
        "    movs   r2, #0          \n\t"
        "    str    r2, [r0]        \n\t"
        "    adds   r0, #4          \n\t"
        "    b      loop            \n\t"
        "done:                      \n\t");

    /*
     * Branch to main() which is on SYSRAM. but we can't just call main() directly.
     *
     * That is because compiler will generate PIC code (mentioned as above comment) for main() call statment,
     * and it will jump to main() in flash(XIP).
     *
     * So we force a long jump by assembly here.
     */
        __asm volatile(
        "       ldr   r0, =main \n\t"
        "       bx    r0        \n\t");

}

void NMIISR(void)
{
    while(1);
}

void FaultISR(void)
{
    while(1);
}

void MPUFaultFhandler(unsigned long add)
{
}

void ExpDefHandler(void)
{
    while(1);
}


void SerialFlashISR(void)
{
    // should not happened
    while(1);
}
