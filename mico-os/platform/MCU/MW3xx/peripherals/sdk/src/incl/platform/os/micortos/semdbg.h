/*
 *  Copyright (C) 2013, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef _SEM_DBG_H_
#define _SEM_DBG_H_

#include "semphr.h"

#ifdef CONFIG_SEMAPHORE_DEBUG
typedef struct {
	xSemaphoreHandle x_queue;
	char *q_name;
	bool is_semaphore;
} sem_dbg_info_t;

#define MAX_SEM_INFO_BUF 48
extern void sem_debug_add(const xSemaphoreHandle handle, const char *name,
			  bool is_semaphore);
extern void sem_debug_delete(const xSemaphoreHandle handle);
extern int seminfo_init(void);

#else

static inline void sem_debug_add(const xSemaphoreHandle handle,
				 const char *name, bool is_semaphore) { }
static inline void sem_debug_delete(const xSemaphoreHandle handle) { }
static inline int seminfo_init(void)
{
	return 0;
}

#endif /* CONFIG_SEMAPHORE_DEBUG */

#endif /* _SEM_DBG_H_ */
