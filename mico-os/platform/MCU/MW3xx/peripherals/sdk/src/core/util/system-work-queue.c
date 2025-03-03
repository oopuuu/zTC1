
/*
 * Copyright 2015, Marvell International Ltd.
 * All Rights Reserved.
 */

/* System Work queue is a common work queue which can be used by any
 * module, instead of having to define its own queue or thread for
 * performing simple tasks.
 * Please refer the documentation of work queues (work-queue.h) for
 * more details.
 */
#include <work-queue.h>
#include <wm_os.h>

static wq_handle_t sys_work_queue_handle;
static bool sys_work_queue_init_flag;

int sys_work_queue_init()
{
	int rv = WM_SUCCESS;
	/* If the queue has already been initialized, just return SUCCESS */
	if (!sys_work_queue_init_flag) {
		wq_cfg_t cfg;
		memset(&cfg, 0, sizeof(cfg));
		cfg.worker_stack_bytes = CONFIG_SYS_WQ_STACK;
		cfg.worker_priority = DEFAULT_WORKER_PRIO;
		cfg.worker_isr_jobs_reserve = DEFAULT_ISR_JOBS_RESERVE;
		rv = work_queue_init(&cfg, &sys_work_queue_handle);
		if (rv == WM_SUCCESS)
			sys_work_queue_init_flag = true;

	}
	return rv;
}

wq_handle_t sys_work_queue_get_handle()
{
	return sys_work_queue_handle;
}
