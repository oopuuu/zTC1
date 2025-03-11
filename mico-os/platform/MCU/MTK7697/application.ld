/* Entry Point */
ENTRY(Reset_Handler)

/* Specify the memory areas */
MEMORY
{
	ROM_CODE        (rx) : ORIGIN = 0x0,           LENGTH = 0x00005000	/* 20k  */
	TCMRAM          (rwx) : ORIGIN = 0x00100000,	LENGTH = 0x00010000	/* 64k  */
	XIP_CODE        (rx) : ORIGIN = 0x100CE000,    LENGTH = 0x00100000	/* 1024k  */
	SYSRAM          (rwx) : ORIGIN = 0x20000000,    LENGTH = 0x00040000	/* 256k */
}
__FLASH_segment_start__ = ORIGIN( XIP_CODE );
__FLASH_segment_end__ = __FLASH_segment_start__ + LENGTH( XIP_CODE );

__SRAM_segment_start__ = ORIGIN( SYSRAM );
__SRAM_segment_end__ = __SRAM_segment_start__ + LENGTH( SYSRAM );


/* Define output sections */
SECTIONS
{
    . = ORIGIN(XIP_CODE);
    .text :
    {
        _text_start = .;
        KEEP(*(.init))
        KEEP(*(.fini))

        *(.text*)
        *(.text)
        *(.rodata*)
        _text_end = .;
        . = ALIGN(4);
    } > XIP_CODE

    . = ALIGN(4);
    __etext = .;
    . = ALIGN(4);

    . = ALIGN(4);
    .data :
    {
        __data_start__ = .;
        _data_start = .;
	    KEEP(*(.isr_vector))
        . = ALIGN(4);
        *(.data)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);

        _data_end = .;
       __data_end__ = .;

    } > SYSRAM AT> XIP_CODE

    . = ALIGN(4);
    .bssRandom (NOLOAD) :
    {
        __bssrandom_start__ = .;
        *(.bssRandom)
        __bssrandom_end__   = .;

    } > TCMRAM

    . = ALIGN(4);
    __ramtext_load__ = LOADADDR(.ramTEXT);

    . = ALIGN(4);
    .ramTEXT :
    {
        __ramtext_start__ = .;
        KEEP(*(.ramTEXT))
        __ramtext_end__ = .;

    } > TCMRAM AT> XIP_CODE

    . = ALIGN(4);
    .tcmBSS (NOLOAD) :
    {
        __tcmbss_start__ = .;
        *(.tcmBSS)
        __tcmbss_end__   = .;

    } > TCMRAM

    . = ALIGN(4);
    .bss :
    {
        __bss_start__ = .;
        *(.bss)
        *(COMMON)
        *(.bss*)
        . = ALIGN(4);
        __bss_end__ = .;
    } > SYSRAM
    . = ALIGN(4);

    .heap :
    {
        __end__ = .;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    } > SYSRAM

/*
   .tcmcode :
    {
        *(.tcmsection.text .tcmsection.*)
        . = ALIGN(4);
    } > TCMRAM
    . = ALIGN(4);
*/

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */

    .stack_dummy :
    {
        *(.stack)
    } > SYSRAM

    __StackTop = ORIGIN(SYSRAM) + LENGTH(SYSRAM);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

}

/* Declare libc Heap to start at end of allocated RAM */

PROVIDE( _heap = __end__ );

/* End of the heap is top of RAM, aligned 8 byte */

PROVIDE( _eheap = ALIGN( __HeapLimit - 8, 8 ) );
