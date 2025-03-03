/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef _CACHE_PROFILE_H_
#define _CACHE_PROFILE_H_

#ifdef CONFIG_CPU_MW300

#define NO_FLUSH	0
#define FLUSH_CACHE	1

int begin_cacheprof_reading(int flag);
int cacheprof_end_reading(void);
void cacheprof_show_readings(void);
int cacheprof_cli_init(void);

#endif /* CONFIG_CPU_MW300 */

#endif /* _CACHE_PROFILE_H_ */
