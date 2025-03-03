/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <stdlib.h>
#include <wm_os.h>
#include <wmlist.h>
#include <work-queue.h>
#include <wmlog.h>
#include <cli.h>

/*  #define CONFIG_WQ_DEBUG */
/*  #define CONFIG_WQ_FUNC_DEBUG */
/* #define WQ_JOBS_RUN_TIME_DEBUG */

#define wq_e(...)				\
	wmlog_e("wq", ##__VA_ARGS__)
#define wq_w(...)				\
	wmlog_w("wq", ##__VA_ARGS__)

#ifdef CONFIG_WQ_DEBUG
#define wq_d(...)				\
	wmlog("wq", ##__VA_ARGS__)
#else
#define wq_d(...)
#endif /* ! CONFIG_WQ_DEBUG */

#ifdef CONFIG_WQ_FUNC_DEBUG
#define wq_entry(_fmt_, ...)						\
	wmprintf("######## > %s ("_fmt_") ####\n\r", __func__, ##__VA_ARGS__)
#define wq_entry_i(...)			\
	wmlog_entry(__VA_ARGS__)
#else
#define wq_entry(...)
#define wq_entry_i(...)
#endif /* ! CONFIG_WQ_DEBUG */

/* For naming worker threads uniquely */
static int g_thread_cnt;

typedef struct {
	wq_job_t wq_job;
	uint64_t next_exec_tick;
	bool deque_req;
	os_semaphore_t sync_sem;
	int return_status;
	list_head_t node;
} wq_job_info_t;

typedef struct {
	os_thread_t worker_hnd;
	os_semaphore_t wq_sem;
	bool quit_req;
	bool deque_req_pending;
	list_head_t wq_job_list;

	/*
	 * For jobs enqueued from ISR. 'malloc' cannot be done in
	 * ISR. These pre-allocated nodes are used only when jobs are
	 * enqued from ISR.
	 */
	list_head_t wq_job_info_isr_list;
	/*
	 * Whenever a node is removed from 'wq_job_info_isr_list' this
	 * count is increased. Whenever "any" job finishes execution it is
	 * added to the 'wq_job_info_isr_list' and the 'isr_list_deficit'
	 * count is reduced.
	 */
	unsigned isr_list_deficit;

#ifdef CONFIG_WORK_QUEUE_CLI
	/* We are a node in a list of all work queues */
	struct list_head node;
#endif
} wq_t;

#ifdef CONFIG_WORK_QUEUE_CLI
/* This is a list of all init'ed work queues */
static struct list_head wq_list;
#endif

static int wq_sem_init(wq_t *wq, int cnt)
{
	char name[10];
	snprintf(name, sizeof(name), "wq-sem-%d", cnt);
	return os_semaphore_create(&wq->wq_sem, name);
}

static void wq_sem_deinit(wq_t *wq)
{
	os_semaphore_delete(&wq->wq_sem);
}

static void wq_signal(wq_t *wq)
{
	os_semaphore_put(&wq->wq_sem);
}

static void wq_wait_for_signal(wq_t *wq, unsigned long wait)
{
	os_semaphore_get(&wq->wq_sem, wait);
}

static wq_job_info_t *wq_get_preallocated_job_info(wq_t *wq)
{
	/* Get free job object from isr reserved queue */
	os_disable_all_interrupts();
	if (list_empty(&wq->wq_job_info_isr_list)) {
		os_enable_all_interrupts();
		return NULL;
	}

	/* Get the first job in the list */
	wq_job_info_t *wq_job_info = list_entry(wq->wq_job_info_isr_list.next,
						wq_job_info_t, node);
	/* Remove the object before we use it */
	list_del(&wq_job_info->node);
	wq->isr_list_deficit++;
	os_enable_all_interrupts();
	return wq_job_info;
}

static int wq_prepare_job_info(wq_t *wq, const wq_job_t *wq_job,
			     wq_job_info_t **wq_job_info, bool init_sem)
{
	*wq_job_info = NULL;

	os_semaphore_t sem;
	if (init_sem) {
		int rv = os_semaphore_create(&sem, "wq-sem");
		if (rv != WM_SUCCESS)
			return rv;

		os_semaphore_get(&sem, OS_WAIT_FOREVER);
	}

	wq_job_info_t *l_wq_job_info;
	if (is_isr_context()) {
		l_wq_job_info = wq_get_preallocated_job_info(wq);
		if (!l_wq_job_info)
			return -WM_E_NOSPC;
	} else {
		l_wq_job_info = os_mem_calloc(sizeof(wq_job_info_t));
		if (!l_wq_job_info)
			return -WM_E_NOMEM;
	}

	memcpy(&l_wq_job_info->wq_job, wq_job, sizeof(wq_job_t));

	INIT_LIST_HEAD(&l_wq_job_info->node);
	l_wq_job_info->next_exec_tick = os_total_ticks_get() +
		wq_job->initial_delay_ms;

	if (init_sem)
		l_wq_job_info->sync_sem = sem;

	*wq_job_info = l_wq_job_info;
	return WM_SUCCESS;
}

int work_enqueue_and_wait(wq_handle_t wq_handle, const wq_job_t *wq_job,
			  unsigned long timeout_ms, int *ret_status)
{
	wq_entry("%p", wq_job->job_func);
	wq_t *wq = (wq_t *) wq_handle;
	if (!wq || !wq_job || !wq_job->job_func ||
	    wq_job->owner[MAX_OWNER_NAME_LEN - 1] ||  /* Chk NULL termination */
	    is_isr_context() ||  /* ISR context not allowed */
	    wq_job->periodic_ms) /* Wait not allowed for periodic jobs */
		return -WM_E_INVAL;

	wq_job_info_t *wq_job_info;
	int rv = wq_prepare_job_info(wq, wq_job, &wq_job_info, true);
	if (rv != WM_SUCCESS)
		return rv;

	/* Add this to the work queue list */
	os_disable_all_interrupts();
	/*
	 * Add to tail so that if multiple jobs with different initial
	 * delays get active at same time, they run in enqueued sequence.
	 */
	list_add_tail(&wq_job_info->node, &wq->wq_job_list);

	os_enable_all_interrupts();

	/* Poke the worker thread */
	wq_signal(wq);

	/* Wait till job is done */
	rv = os_semaphore_get(&wq_job_info->sync_sem, timeout_ms);
	if (rv != WM_SUCCESS) {
		/*
		 * A timeout has occured. We shall now report back the
		 * same. But before that we need to delete the semaphore
		 * thus allowing the worker thread to treat this job as a
		 * normal async one and free this 'wq_job_info' node.
		 */
		os_disable_all_interrupts();
		os_semaphore_delete(&wq_job_info->sync_sem);
		wq_job_info->sync_sem = NULL;
		os_enable_all_interrupts();

		/*
		 * If the job completes in the background it will deque and
		 * free itself as 'sync_sem' is made NULL above thus
		 * categorizing it as a normal 'sync' job.
		 */
		return -WM_E_TIMEOUT;
	}

	if (ret_status)
		*ret_status = wq_job_info->return_status;

	wq->deque_req_pending = true;
	wq_job_info->deque_req = true;

	/* Poke the worker thread */
	wq_signal(wq);

	return WM_SUCCESS;
}

int work_enqueue(wq_handle_t wq_handle, const wq_job_t *wq_job,
			job_handle_t *job_handle)
{
	wq_entry("%p", wq_job->job_func);
	wq_t *wq = (wq_t *) wq_handle;
	if (!wq || !wq_job || !wq_job->job_func ||
	    wq_job->owner[MAX_OWNER_NAME_LEN - 1]) /* Check NULL termination */
		return -WM_E_INVAL;

	wq_job_info_t *wq_job_info;
	int rv = wq_prepare_job_info(wq, wq_job, &wq_job_info, false);
	if (rv != WM_SUCCESS)
		return rv;

	/* Add this to the work queue list */
	os_disable_all_interrupts();
	/*
	 * Add to tail so that if multiple jobs with different initial
	 * delays get active at same time, they run in enqueued sequence.
	 */
	list_add_tail(&wq_job_info->node, &wq->wq_job_list);

	if (job_handle)
		*job_handle = (job_handle_t) wq_job_info;
	os_enable_all_interrupts();

	/* Poke the worker thread */
	wq_signal(wq);

	return WM_SUCCESS;
}

/* Caller has to take care of locking the list */
static bool wq_is_job_present(wq_t *wq, wq_job_info_t *check_wq_job_info)
{
	wq_job_info_t *wq_job_info;
	list_for_each_entry(wq_job_info, &wq->wq_job_list, node) {
		if (wq_job_info == check_wq_job_info)
			return true;
	}

	return false;
}

int work_dequeue(wq_handle_t wq_handle, job_handle_t *job_handle)
{
	wq_entry();
	if (!wq_handle || !job_handle || !*job_handle)
		return -WM_E_INVAL;

	wq_t *wq = (wq_t *) wq_handle;
	wq_job_info_t *wq_job_info = (wq_job_info_t *) *job_handle;

	int rv;
	int save = os_enter_critical_section();
	if (wq_is_job_present(wq, wq_job_info)) {
		wq->deque_req_pending = true;
		wq_job_info->deque_req = true;

		/* Poke the worker thread */
		wq_signal(wq);

		rv = WM_SUCCESS;
	} else {
		rv = -WM_E_NOENT;
	}

	*job_handle = 0;
	os_exit_critical_section(save);

	return rv;
}

int work_dequeue_owner_all(wq_handle_t wq_handle, const char *owner)
{
	wq_entry();
	if (!wq_handle || !owner)
		return -WM_E_INVAL;

	wq_t *wq = (wq_t *) wq_handle;

	wq_job_info_t *wq_job_info;
	int save = os_enter_critical_section();

	list_for_each_entry(wq_job_info, &wq->wq_job_list, node) {
		if (!wq_job_info->wq_job.owner[0])
			continue;

		if (!strcmp(owner, wq_job_info->wq_job.owner)) {
			wq_job_info->deque_req = true;
			wq->deque_req_pending = true;
		}
	}

	os_exit_critical_section(save);

	/* Poke the worker thread */
	wq_signal(wq);

	return WM_SUCCESS;
}

static int wq_exec(wq_t *wq, wq_job_info_t *wq_job_info)
{
	wq_entry_i("%p", wq_job_info->wq_job.job_func);
	wq_job_t *wq_job = &wq_job_info->wq_job;
	/* Make local copy to avoid race condition */
	int save = os_enter_critical_section();
	job_func_t job_func = wq_job->job_func;
	void *param = wq_job->param;
	os_exit_critical_section(save);

	if (!job_func) {
		wq_w("NULL job ignored");
		return -WM_E_NOENT;
	}

	/* Run the job */
	return job_func(param);
}

static unsigned long wq_get_sleep_ticks(wq_t *wq)
{
	wq_entry_i("wq = %p", wq);
	if (list_empty(&wq->wq_job_list))
		return OS_WAIT_FOREVER;

	wq_job_info_t *wq_job_info;
	uint32_t shortest_ticks = ~0;

	uint64_t curr_tick = os_total_ticks_get();

	int save = os_enter_critical_section();
	list_for_each_entry(wq_job_info, &wq->wq_job_list, node) {
#ifdef WQ_JOBS_RUN_TIME_DEBUG
		wmprintf("Sleep Ticks: %lld: %lld\r\n",
			wq_job_info->next_exec_tick, curr_tick);
#endif /* WQ_JOBS_RUN_TIME_DEBUG */
		if (curr_tick >= wq_job_info->next_exec_tick) {
			/* The job is ready */
			shortest_ticks = 0;
			break;
		}

		uint32_t remaining_ticks =
			wq_job_info->next_exec_tick - curr_tick;
		if (shortest_ticks > remaining_ticks)
			shortest_ticks = remaining_ticks;
	}
	os_exit_critical_section(save);

	return shortest_ticks;
}

/* Doesn't care about dequeued jobs */
static wq_job_info_t *search_for_ready_job(wq_t *wq)
{
	wq_entry_i();

	if (list_empty(&wq->wq_job_list))
		return NULL;

	wq_job_info_t *wq_job_info, *ready_job = NULL;

	uint64_t curr_tick = os_total_ticks_get();

	int save = os_enter_critical_section();
	list_for_each_entry(wq_job_info, &wq->wq_job_list, node) {
		if (curr_tick >= wq_job_info->next_exec_tick) {
			/* The job is ready */
			ready_job = wq_job_info;
			break;
		}
	}
	os_exit_critical_section(save);

	return ready_job;
}

/* Caller has to take care of locking the list */
static void deque_and_free_job(wq_t *wq, wq_job_info_t *wq_job_info)
{
	if (wq_job_info->sync_sem)
		os_semaphore_delete(&wq_job_info->sync_sem);

	list_del(&wq_job_info->node);
	memset(wq_job_info, 0x00, sizeof(wq_job_info_t));
	if (wq->isr_list_deficit) {
		INIT_LIST_HEAD(&wq_job_info->node);
		list_add(&wq_job_info->node,
			 &wq->wq_job_info_isr_list);
		wq->isr_list_deficit--;
	} else {
		os_mem_free(wq_job_info);
	}
}

static void wq_purge_job_list(wq_t *wq, bool purge_isr_list)
{
	wq_entry_i();
	int save = os_enter_critical_section();
	while (!list_empty(&wq->wq_job_list)) {
		/* Get the first job in the list */
		wq_job_info_t *wq_job_info = list_entry(wq->wq_job_list.next,
						 wq_job_info_t,
						 node);
		wq_d("%s: De-queueing: %p", __func__,
		     wq_job_info->wq_job.job_func);
		deque_and_free_job(wq, wq_job_info);
	}

	if (purge_isr_list) {
		while (!list_empty(&wq->wq_job_info_isr_list)) {
			/* Get the first job in the list */
			wq_job_info_t *wq_job_info =
				list_entry(wq->wq_job_info_isr_list.next,
					   wq_job_info_t, node);
			wq_d("%s: De-queueing: %p", __func__,
			     wq_job_info->wq_job.job_func);
			deque_and_free_job(wq, wq_job_info);
		}
	}

	os_exit_critical_section(save);
}

/* Will take care of locking */
static void wq_purge_all_dequed(wq_t *wq)
{
	wq_entry_i();
	int save = os_enter_critical_section();

	wq_job_info_t *wq_job_info;

 re_run_after_node_del:
	list_for_each_entry(wq_job_info, &wq->wq_job_list, node) {
		if (wq_job_info->deque_req) {
			deque_and_free_job(wq, wq_job_info);
			goto re_run_after_node_del;
		}
	}

	/* Reset it in critical section */
	wq->deque_req_pending = false;
	os_exit_critical_section(save);
}

static void handle_ready_job(wq_t *wq, wq_job_info_t *wq_job_info)
{
	wq_entry_i();
	if (wq_job_info->deque_req) {
		/* The job is dequed. No need to run it now */
		int save = os_enter_critical_section();
		deque_and_free_job(wq, wq_job_info);
		os_exit_critical_section(save);
		return;
	}

	wq_job_t *wq_job = &wq_job_info->wq_job;
	uint64_t start_tick = os_total_ticks_get();
	int job_ret_status = wq_exec(wq, wq_job_info);

#ifdef WQ_JOBS_RUN_TIME_DEBUG
	uint64_t end_tick = os_total_ticks_get();
	uint32_t run_time = (uint32_t) (end_tick - start_tick);
	wmprintf("Job: %p(%p) ran for %u mS\r\n",
		 wq_job->job_func, wq_job->param, run_time);
#endif /* WQ_JOBS_RUN_TIME_DEBUG */
	/*
	 * Update the next run time if periodic job. Else remove the ob
	 * from list.
	 */
	if (wq_job->periodic_ms && !wq_job_info->deque_req) {
		/* This is a periodic job */
		wq_job_info->next_exec_tick = start_tick +
			os_msec_to_ticks(wq_job->periodic_ms);
	} else {
		int save = os_enter_critical_section();
		if (wq_job_info->sync_sem) {
			/*
			 * The enqueuing thread is waiting for this job to
			 * complete. Deletion of this job will be deferred
			 * till this semaphore signal is successfully
			 * delivered.
			 */
			wq_job_info->return_status = job_ret_status;

			/* Wake up the waiting user thread */
			os_semaphore_put(&wq_job_info->sync_sem);
		} else {
			deque_and_free_job(wq, wq_job_info);
		}
		os_exit_critical_section(save);
	}
}

static void wq_thread(os_thread_arg_t arg)
{
	wq_entry_i("arg = %p", arg);
	wq_t *wq = (wq_t *)arg;
	if (!wq) {
		wq_e("Invalid work queue");
		os_thread_self_complete(NULL);
	}

	while (!wq->quit_req) {
		int sleep_ticks = wq_get_sleep_ticks(wq);
		if (sleep_ticks)
			wq_wait_for_signal(wq, sleep_ticks);

		/* Check if somebody wants us to quit */
		if (wq->quit_req)
			break;

		/* Check if any jobs need to be dequed right away */
		if (wq->deque_req_pending)
			wq_purge_all_dequed(wq);

		wq_job_info_t *wq_job_info = search_for_ready_job(wq);

		if (wq_job_info && !wq->quit_req)
			handle_ready_job(wq, wq_job_info);

	}

	/* Delete all pending jobs if any */
	wq_purge_job_list(wq, true);
	wq->quit_req = false;
	/* Delete self */
	os_thread_delete(NULL);
}

static int alloc_job_info_isr_list(wq_t *wq, int count)
{
	int index;
	for (index = 0; index < count; index++) {
		wq_job_info_t *wq_job_info =
			os_mem_calloc(sizeof(wq_job_info_t));
		if (!wq_job_info)
			return -WM_E_NOMEM;

		INIT_LIST_HEAD(&wq_job_info->node);
		list_add(&wq_job_info->node, &wq->wq_job_info_isr_list);
	}

	return WM_SUCCESS;
}

int work_queue_init(const wq_cfg_t *wq_cfg, wq_handle_t *wq_handle)
{
	wq_entry();
	if (!wq_handle)
		return -WM_E_INVAL;

	int save;
#ifdef CONFIG_WORK_QUEUE_CLI
	/* Is this the first queue to be init'ed */
	save = os_enter_critical_section();
	if (wq_list.next == NULL)
		INIT_LIST_HEAD(&wq_list);
	os_exit_critical_section(save);
#endif

	int worker_priority;
	int worker_stack_bytes;
	int worker_isr_jobs_reserve;
	if (!wq_cfg) {
		worker_priority = DEFAULT_WORKER_PRIO;
		worker_stack_bytes = DEFAULT_STACK_SIZE;
		worker_isr_jobs_reserve = DEFAULT_ISR_JOBS_RESERVE;
	} else {
		worker_priority = wq_cfg->worker_priority;
		worker_stack_bytes = wq_cfg->worker_stack_bytes;
		worker_isr_jobs_reserve = wq_cfg->worker_isr_jobs_reserve;
	}

	/* Create main work queue object */
	wq_t *wq = os_mem_calloc(sizeof(wq_t));
	if (!wq)
		return -WM_E_NOMEM;

	INIT_LIST_HEAD(&wq->wq_job_list);
	INIT_LIST_HEAD(&wq->wq_job_info_isr_list);
	int rv = alloc_job_info_isr_list(wq, worker_isr_jobs_reserve);
	if (rv != WM_SUCCESS) {
		os_mem_free(wq);
		return rv;
	}

	/* Increment worker thread count */
	save = os_enter_critical_section();
	int l_thread_cnt = g_thread_cnt++;
	os_exit_critical_section(save);

	rv = wq_sem_init(wq, l_thread_cnt);
	if (rv != WM_SUCCESS) {
		os_mem_free(wq);
		return rv;
	}

	char name[configMAX_TASK_NAME_LEN];
	snprintf(name, sizeof(name), "wq/%d", l_thread_cnt);
	os_thread_stack_define(wq_stack, worker_stack_bytes);
	rv = os_thread_create(&wq->worker_hnd, name, wq_thread, wq,
			      &wq_stack, worker_priority);
	if (rv != WM_SUCCESS) {
		wq_sem_deinit(wq);
		os_mem_free(wq);
		return rv;
	}

#ifdef CONFIG_WORK_QUEUE_CLI
	save = os_enter_critical_section();
	list_add_tail(&wq->node, &wq_list);
	os_exit_critical_section(save);
#endif

	*wq_handle = (wq_handle_t) wq;
	return WM_SUCCESS;
}

void work_queue_deinit(wq_handle_t *wq_handle)
{
	wq_entry();
	if (!wq_handle) {
		wq_w("Invalid args");
		return;
	}

	wq_t *wq = (wq_t *) *wq_handle;
	if (!wq) {
		wq_w("Invalid args");
		return;
	}

	wq->quit_req = true;
	/* Poke the worker thread */
	wq_signal(wq);
	/* Will till request is acknowledged */
	int check_limit_ms = 10;
	while (wq->quit_req && check_limit_ms--)
		os_thread_sleep(os_msec_to_ticks(1));

	if (wq->quit_req) {
		wq_w("Unable to ensure work queue delete");
	}

#ifdef CONFIG_WORK_QUEUE_CLI
	list_del(&wq->node);
#endif

	wq_sem_deinit(wq);
	os_mem_free(wq);
}

#ifdef CONFIG_WORK_QUEUE_CLI
static void print_human_readable_time(uint64_t next_exec_tick)
{
	int64_t diff = next_exec_tick - (int64_t)os_total_ticks_get();
	if (diff < 0) {
		wmprintf("expired");
		return;
	}

	const int day_ms = 1000 * 60 * 60 * 24;
	const int hour_ms = 1000 * 60 * 60;
	const int minute_ms = 1000 * 60;
	int quotient;
	const int max_level = 3;
	int level = 0;

	if (diff > day_ms) {
		quotient = (int)(diff / day_ms);
		wmprintf("%d days", quotient);
		diff -= quotient * day_ms;
		level++;

		if (level == max_level)
			return;

		wmprintf(", ");
	}

	if (diff > hour_ms) {
		quotient = (int)(diff / hour_ms);
		wmprintf("%d hrs", quotient);
		diff -= quotient * hour_ms;
		level++;

		if (level == max_level)
			return;

		wmprintf(", ");
	}

	if (diff > minute_ms) {
		quotient = (int)(diff / minute_ms);
		wmprintf("%d mins", quotient);
		diff -= quotient * minute_ms;
		level++;

		if (level == max_level)
			return;

		wmprintf(", ");
	}

	if (diff > 1000) {
		quotient = (int)(diff / 1000);
		wmprintf("%d secs", quotient);
		diff -= quotient * 1000;
		level++;

		if (level == max_level)
			return;

		wmprintf(", ");
	}

	wmprintf("%d mS", (int)diff);
}

static void dump_job_info(wq_job_info_t *job_info, bool verbose)
{
	wq_job_t *wq_job = &job_info->wq_job;
	wmprintf("\t\tOwner: %s\r\n", wq_job->owner[0] ? wq_job->owner :
		 "undef");
	wmprintf("\t\tFunc: %p\r\n", wq_job->job_func);
	wmprintf("\t\tParam: %p\r\n", wq_job->param);
	wmprintf("\t\tPeriodic: %d mS\r\n", wq_job->periodic_ms);
	wmprintf("\t\tInitial delay: %d mS\r\n", wq_job->initial_delay_ms);
	wmprintf("\t\tNext exec tick: %lld (", job_info->next_exec_tick);
	print_human_readable_time(job_info->next_exec_tick);
	wmprintf(")\r\n");
	if (verbose) {
		wmprintf("\t\tDeque req: %d\r\n", job_info->deque_req);
		wmprintf("\t\tJob semaphore count: ");
		if (!job_info->sync_sem)
			wmprintf("undef\r\n");
		else
			wmprintf("%d\r\n",
				 os_semaphore_getcount(&job_info->sync_sem));

		wmprintf("\t\tReturn status: %d\r\n", job_info->return_status);
	}
}

static void dump_wq_info(wq_t *wq, bool verbose)
{
	if (verbose) {
		wmprintf("\tThread handle: %p\r\n", wq->worker_hnd);
		wmprintf("\tThread Semaphore count:");
		if (!wq->wq_sem)
			wmprintf("undef\r\n");
		else
			wmprintf("%d\r\n", os_semaphore_getcount(&wq->wq_sem));
		wmprintf("\tQuit req: %d\r\n", wq->quit_req);
		wmprintf("\tDeque req pending: %d\r\n", wq->deque_req_pending);
		wmprintf("\tISR deficit: %d\r\n", wq->isr_list_deficit);
	}

	int cnt = 0;
	wq_job_info_t *job_info;
	list_for_each_entry(job_info, &wq->wq_job_list, node) {
		wmprintf("\t[%d] Job Handle: %p\r\n", cnt++, job_info);
		dump_job_info(job_info, verbose);
	}
}

static void workq_debug_dump_cmd(int argc, char **argv)
{
	wq_t *wq;
	int cnt = 0;
	list_for_each_entry(wq, &wq_list, node) {
		wmprintf("[%d] Work queue handle: %p\r\n", cnt++, wq);
		dump_wq_info(wq, 1);
	}
}

static void workq_list_cmd(int argc, char **argv)
{
	int cnt = 0;
	wq_t *wq;
	list_for_each_entry(wq, &wq_list, node) {
		wmprintf("[%d] Work queue handle: %p\r\n", cnt++, wq);
		dump_wq_info(wq, 0);
	}
}

static void workq_deinit_cmd(int argc, char **argv)
{
	if (argc != 2) {
		wmprintf("Usage: workq-deinit <work queue handle>");
		return;
	}

	uint32_t val = strtoul(argv[1], NULL, 0);
	wq_handle_t wq_handle = (wq_handle_t) val;
	wmprintf("De-initing work queue: %p\r\n", wq_handle);
	work_queue_deinit(&wq_handle);
}

static void workq_dequeue_cmd(int argc, char **argv)
{
	if (argc != 3) {
		wmprintf("Usage: workq-dequeue <work queue handle>"
			 "<job handle>");
		return;
	}

	uint32_t val = strtoul(argv[1], NULL, 0);
	wq_handle_t wq_handle = (wq_handle_t) val;
	val = strtoul(argv[2], NULL, 0);
	job_handle_t job_handle = (job_handle_t) val;

	wmprintf("De-initing job: %p\r\n", job_handle);
	work_dequeue(wq_handle, &job_handle);
}

static void workq_dequeue_by_name_cmd(int argc, char **argv)
{
	if (argc != 3) {
		wmprintf("Usage: workq-dequeue-by-name <work queue handle>"
			 "<job owner name>");
		return;
	}

	uint32_t val = strtoul(argv[1], NULL, 0);
	wq_handle_t wq_handle = (wq_handle_t) val;
	const char *owner = argv[2];

	wmprintf("De-initing jobs with owner: %s\r\n", owner);
	work_dequeue_owner_all(wq_handle, owner);
}

struct cli_command wq_commands[] = {
	{"workq-list", NULL, workq_list_cmd},
	{"workq-deinit", "<work queue handle>", workq_deinit_cmd},
	{"workq-dequeue", "<work queue handle> <job handle>",
	 workq_dequeue_cmd},
	{"workq-dequeue-by-name", "<work queue handle> <job owner name>",
		 workq_dequeue_by_name_cmd},
	{"workq-debug-dump", NULL, workq_debug_dump_cmd},
};

int work_queue_cli_init()
{
	int i;

	for (i = 0; i < sizeof(wq_commands) / sizeof(struct cli_command); i++)
		if (cli_register_command(&wq_commands[i]))
			return -WM_FAIL;

	return WM_SUCCESS;
}
#endif
