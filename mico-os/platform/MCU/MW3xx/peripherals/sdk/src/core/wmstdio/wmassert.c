/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/** wmassert.c: Assert Functions
 */
#include <string.h>

#include <wm_os.h>
#include <wmstdio.h>
#include <wmassert.h>

void _wm_assert(const char *filename, int lineno, 
	    const char* fail_cond)
{
	ll_printf("\n\n\r*************** PANIC *************\n\r");
	ll_printf("Filename  : %s ( %d )\n\r", filename, lineno);
	ll_printf("Condition : %s\n\r", fail_cond);
	ll_printf("***********************************\n\r");
	os_enter_critical_section();
	for( ;; );
}
