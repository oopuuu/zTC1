/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */


#include <string.h>
#include "platform_init.h"
#include "platform_cmsis.h"
#include "platform_toolchain.h"

#define SCB_AIRCR_VECTKEY        ( (unsigned long)( 0x5FA << SCB_AIRCR_VECTKEY_Pos ))

extern void * link_global_data_initial_values;
extern void * link_global_data_start;
extern void * link_global_data_end;
#define link_global_data_size   ((unsigned long)&link_global_data_end - (unsigned long)&link_global_data_start)

extern void * link_run_from_ram_code_flash_location;
extern void * link_run_from_ram_code_ram_location;
extern void * link_run_from_ram_code_ram_end;
#define link_run_from_ram_code_size   ((unsigned long)&link_run_from_ram_code_ram_end - (unsigned long)&link_run_from_ram_code_ram_location)

extern void * link_interrupt_vectors_location;

extern void * link_bss_location;
extern void * link_bss_end;
#define link_bss_size   ((unsigned long)&link_bss_end  -  (unsigned long)&link_bss_location )

#ifdef DEBUG
extern void * link_stack_location;
extern void * link_stack_end;
#define link_stack_size   ((unsigned long)&link_stack_end  -  (unsigned long)&link_stack_location )
#endif /* ifdef DEBUG */

typedef void  (*constructor_ptr_t)( void );
extern constructor_ptr_t link_constructors_location[];
extern constructor_ptr_t link_constructors_end;

/* Must be naked to avoid overwriting a program which is in RAM during function prologue when stack pointer is invalid */
void _start( void ) __attribute__ (( naked ));
void _start_init( void ) __attribute__ (( naked ));
void _exit( int status );


#define link_constructors_size   ((unsigned long)&link_constructors_end  -  (unsigned long)&link_constructors_location )


__attribute__((section(".copy_ramcode"))) void _start(void)
{
    /* Copy from flash any code to be run from RAM. */
    uint32_t *dest = (uint32_t *)&link_run_from_ram_code_ram_location;
    uint32_t *src = (uint32_t *)&link_run_from_ram_code_flash_location;
    uint32_t i = 0;

    /* Do not use memcpy, memcpy may be linked in ram */
    if ( (&link_run_from_ram_code_ram_location != &link_run_from_ram_code_flash_location) && ( link_run_from_ram_code_size != 0) )
    {
        for ( i = 0; i < link_run_from_ram_code_size; i++ )
        {
            dest[i] = src[i];
        }
    }

    __asm volatile(
        "       ldr   r0, =_start_init \n\t"
        "       bx    r0        \n\t");
}

void _start_init( void )
{
    unsigned long ctor_num;

    /* Stack pointer is usually set up by boot process, but if program was loaded via jtag in RAM, that might not have happened */
    __asm__( "   ldr r1, =link_stack_end\n"
             "   mov sp, r1\n"
           );

    /* Setup the interrupt vectors address */
    SCB->VTOR = (unsigned long) &link_interrupt_vectors_location;

    /* Enable CPU Cycle counting */
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	
    /* Initialise clocks and memory. init_clocks() and init_memory() must NOT depend on globals as global data and bss sections aren't initialised yet */
    init_clocks();
    init_memory();

    /* Copy initial values for global variables into RAM  */
    if ( ( &link_global_data_start != &link_global_data_initial_values ) && ( link_global_data_size != 0 ) )
    {
        memcpy( &link_global_data_start, &link_global_data_initial_values, (size_t) link_global_data_size );
    }

    /* BSS segment is for zero initialised elements, so memset it to zero */
    memset( &link_bss_location, 0, (size_t) link_bss_size );

#if 0 /* was ifdef DEBUG */
    /* This is not a valid way to fill the stack, since it is currently in use - causes a problem in release with debug on - optimisation of active stack overwriting causes hardfault */
    memset( &link_stack_location, 0xA5, link_stack_size ); /* Fill stack with pattern to allow checking of stack usage */
#endif /* if 0 */

    /*
     * Run global C++ constructors if any
     */

    /* TODO: make this an unconditional goto?, so that return address stuff doesn't get put on the stack. (what happens if main returns in this case?) */
    init_architecture();
    init_platform();

    for ( ctor_num = 0; ctor_num < link_constructors_size/sizeof(constructor_ptr_t); ctor_num++ )
    {
        link_constructors_location[ctor_num]();
    }

    main( );

    /* the main loop has returned - there is now nothing to do - reboot. */

    /* Reset request */
    SCB->AIRCR = SCB_AIRCR_SYSRESETREQ_Msk | SCB_AIRCR_VECTKEY;

}

void _exit( int status )
{
    /* the main loop has returned - there is now nothing to do - reboot. */

    /* Allow some time for any printf calls to complete */
    volatile unsigned int i; /* try to make this not get optimized out by declaring the variable as volatile */
    volatile unsigned int j; /* try to make this not get optimized out by declaring the variable as volatile */

    (void) status; /* unused parameter */

    for ( i = 0; i < (unsigned int) 1000; i++ )
    {
        for ( j = 0; j < (unsigned int) 10000; j++ )
        {
            __asm("NOP");
        }
    }

    /* Reset request */
    SCB->AIRCR = SCB_AIRCR_SYSRESETREQ_Msk | SCB_AIRCR_VECTKEY;

    /* Should never get here, but this will get rid of error: "noreturn function does return" */
    while ( 1 == 1 )
    {
        /* do nothing */
    }
}

void _memory_init(void)
{
	unsigned long ctor_num;
	/* Copy from flash any code to be run from RAM. Setup .fastcode section first in case init_clocks() needs it */
	if ( ( &link_run_from_ram_code_ram_location != &link_run_from_ram_code_flash_location ) && ( link_run_from_ram_code_size != 0 ) )
	{
		memcpy( &link_run_from_ram_code_ram_location, &link_run_from_ram_code_flash_location, (size_t) link_run_from_ram_code_size );
	}

	/* Copy initial values for global variables into RAM  */
	if ( ( &link_global_data_start != &link_global_data_initial_values ) && ( link_global_data_size != 0 ) )
	{
		memcpy( &link_global_data_start, &link_global_data_initial_values, (size_t) link_global_data_size );
	}

	/* BSS segment is for zero initialised elements, so memset it to zero */
	memset( &link_bss_location, 0, (size_t) link_bss_size );


	/*
	 * Run global C++ constructors if any
	 */


	for ( ctor_num = 0; ctor_num < link_constructors_size/sizeof(constructor_ptr_t); ctor_num++ )
	{
		link_constructors_location[ctor_num]();
	}

}
