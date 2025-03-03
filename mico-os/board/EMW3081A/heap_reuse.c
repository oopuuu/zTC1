
#include "stdint.h"
#include "mico.h"
#include "heap_reuse.h"

extern void insert_heap(void *pv, int len);
#pragma section=".bootup_reuse"
#pragma section=".ram.bss"
#pragma section=".elink_reuse"
#pragma section=".softap_reuse"

void free_memory_reuse(void)
{
	uint32_t len;
	void *start;
	
	start = __section_end(".bootup_reuse");
	len = (uint32_t)__section_begin(".ram.bss") - (uint32_t)start;

	printf("free heap reuse: %p %d\r\n", start, len);
	insert_heap((void *)start, (int)len);
}

void bootup_region_reuse(void)
{
	printf("bootup region %p-%d \r\n",
		(void *)__section_begin(".bootup_reuse"), (int)__section_size(".bootup_reuse"));
	insert_heap((void *)__section_begin(".bootup_reuse"), (int)__section_size(".bootup_reuse"));
}

void elink_region_reuse(void)
{
	static int elink_done = 0;
	if (elink_done == 1)
		return;
	insert_heap((void *)__section_begin(".elink_reuse"), (int)__section_size(".elink_reuse"));
	elink_done = 1;
}

void softap_region_reuse(void)
{	
	static int alink_done = 0;
	if (alink_done == 1)
		return;
	insert_heap((void *)__section_begin(".softap_reuse"), (int)__section_size(".softap_reuse"));
	alink_done = 1;
}



