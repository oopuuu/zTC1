/**************************************************
 *
 * Global data initialization for use with ilink.
 * New style, where each init table routine defines
 * its own format.
 *
 * Copyright 2008 IAR Systems. All rights reserved.
 *
 * $Revision: 36645 $
 *
 **************************************************/

#include "data_init3.h"

#pragma language = extended
#pragma build_attribute vfpcc_compatible
#pragma build_attribute arm_thumb_compatible
#pragma build_attribute ropi_compatible
#pragma build_attribute rwpi_compatible

#pragma section = "Region$$Table" __pcrel const

void
__iar_data_init3(void);
void
__iar_data_init3(void)
{
  char const * p = __section_begin("Region$$Table");
  uint32_t const * pe = __section_end("Region$$Table");
  uint32_t const * pi = (uint32_t const *)(p);
  while (pi != pe)
  {
    init_fun_t * fun = (init_fun_t *)((char *)pi + *(int32_t *)pi);
    pi++;
    pi = fun(pi);
  }
}
