/* 
 * Script for GNU linker.
 * Describes layout of sections, location of stack.
 *
 * In this case vectors are at location 0 (reset @ 0x08)
 *
 *
 *
 * +------------+  0x0040000
 * Vect redirect   32
 * +------------+  
 *
 * +------------+  0x00400020
 * data         |
 * end
 * |(heap)      |
 * .            .
 * .            .
 * |(heap limit)|
 *
 * |- - - - - - |
 * stack bottom    256k
 * +------------+  
 * 
 * 
 * +------------+  0x0000000
 * |Bootloader  |
 * |            |  64k 
 * +------------+  0x0010000
 * |   Para 1   |   4k
 * +------------+ 
 * |   Para 2   |   4k
 * +------------+  0x0012000
 * |vectors     |
 * |            |
 * |------------+
 * |text        |  
 * |data        |  632k
 * |            |  
 * +------------+ 
 *
 *
 * +------------+  0x00B0000
 * |            |
 * |            |
 * |  OTA TEMP  |
 * |            |  
 * |            |
 * |            |  320k
 * +------------+
 */


/* Split memory into area for vectors and ram */
MEMORY
{
	flash  (rx) : ORIGIN = 0x0012000, LENGTH = 546936
	ram   (rwx): ORIGIN = 0x00400020, LENGTH = 256k - 32
}

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_vector_start);
_vector_start = 0x0012000;

SECTIONS
{
/* vectors go to vectors region */
	. = 0x0012000;
	.vectors : 
	{ 
      KEEP(*(*.vectors))
	} > flash

/* instructions go to the text region*/

	. = ALIGN(0x8);
/* code, instructions.for example: i=i+1; */
	.text : 
	{
	    *(.text*)
	    KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        
        *(.rodata*)
        KEEP(*(.eh_frame*))
        
	} > flash	
	
	.ARM.extab ALIGN(8) :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > flash

	.ARM.exidx ALIGN(8):
	{
	    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > flash
	
	_begin_data = .;
	.data : AT ( _begin_data )
	{
	    __data_start__ = .;
	    _sdata = .;
		*(.data*)
    	
    	. = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
    	SORT(CONSTRUCTORS)
    	
        /* All data end */
        __data_end__ = .;
        _edata = .;
        
        KEEP(*(.dummydata))
	} >ram
	
	/* Loader will copy data from _flash_begin to _ram_begin..ram_end */
	_data_flash_begin = LOADADDR(.data);
	_data_ram_begin = ADDR(.data);
	_data_ram_end = .;

/* uninitialized data section - global   int i; */
	.bss ALIGN(8):
	{
	    . = ALIGN(4);
		_bss_start = .;
		__bss_start__ = .;
		_sbss = .;
		*(.bss*)
	   *(COMMON)
	   . = ALIGN(4);
		_bss_end = .;
		__bss_end__ = .;
		_ebss = .;
	} > ram						/* in RAM */

	. = ALIGN (8);
	_empty_ram = .;

/* This symbol defines end of code/data sections. Heap starts here. */
	PROVIDE(end    	  = .);
	PROVIDE(_heap_start    	  = .);
	
	.heap (COPY):
    {
        __end__ = .;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    } > ram
    
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > ram
	
/* _stack symbol defines initial stack bottom addres. Stack grows to lower addresses.
   Typically you set this to be top of your RAM. Note: code never checks, if stack
   grows into heap area!
*/
	PROVIDE(_stack_unused      = 0x440000 - 0x3F0 - 0x7F0 - 0xFF0 - 0x3F0 - 0x10); /* 0x10*/
	PROVIDE(_stack_svc         = 0x440000 - 0x3F0 - 0x7F0 - 0xFF0 - 0x3F0); /* 0x3F0*/
	PROVIDE(_stack_irq         = 0x440000 - 0x3F0 - 0x7F0 - 0xFF0); /* 0xFF0*/
	PROVIDE(_stack_fiq         = 0x440000 - 0x3F0 - 0x7F0); /* 0x7F0*/
	PROVIDE(_stack_sys         = 0x440000 - 0x3F0); /* 0x3F0*/

	PROVIDE(_heap_end      = _stack_unused);
	PROVIDE(_heap_len      = _heap_end -  _heap_start);

	ASSERT ((_heap_len > 0x19000 - 1 ), "Error: No room left for the heap") /*heap must bigger than 100k*/
	
	__StackTop = ORIGIN(ram) + LENGTH(ram);
    _estack = __StackTop;
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    
}
 GROUP(
   libstdc++.a
   libsupc++.a
   libgcc.a
   libg.a
   libc.a
   libm.a
   libnosys.a
 )