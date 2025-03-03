/**************************************************
 *
 * Segment initialization that must be
 * performed before main is called, ilink version.
 * New style, where each init table routine defines
 * its own format.
 *
 * Copyright 2008 IAR Systems. All rights reserved.
 *
 * $Revision: 36645 $
 *
 **************************************************/

#include <stdint.h>

typedef uint32_t const * init_fun_t(uint32_t const *);
