#include <wm_os.h>
#include <stdlib.h>
#include <mdev_gpio.h>
#include <mdev_pinmux.h>
#include <board.h>

#include <work-queue.h>

typedef struct {
	int led_no;
	bool led_state;
} led_t;

led_t led[4];

static os_mutex_t wq_test_mutex;
static int test_2_cnt;
static mdev_t *pinmux_dev, *gpio_dev;

#define GPIO_LED_FN  PINMUX_FUNCTION_0

static int toggle_led(void *param)
{
	/* wmprintf("%s\r\n", __func__); */
	led_t *led = (led_t *)param;

	if (led->led_state) {
		gpio_drv_write(gpio_dev, led->led_no, GPIO_IO_LOW);
		led->led_state = false;
	} else {
		gpio_drv_write(gpio_dev, led->led_no, GPIO_IO_HIGH);
		led->led_state = true;
	}

	return WM_SUCCESS;
}

static int inc_data(void *param)
{
	os_mutex_get(&wq_test_mutex, OS_WAIT_FOREVER);
	test_2_cnt++;
	os_mutex_put(&wq_test_mutex);

	return WM_SUCCESS;
}

static void wqtest_configure_gpio_led(int gpio_no)
{
	pinmux_drv_setfunc(pinmux_dev, gpio_no, GPIO_LED_FN);
	gpio_drv_setdir(gpio_dev, gpio_no, GPIO_OUTPUT);
	gpio_drv_write(gpio_dev, gpio_no, GPIO_IO_HIGH);
}

/* Single work queue handling 4 tasks */
static void wq_test_1()
{
	wmprintf("Work queue test 1. Heap: %d bytes\r\n",
		 os_get_free_size());
	wq_handle_t wq_handle;
	int rv = work_queue_init(NULL, &wq_handle);
	if (rv != WM_SUCCESS) {
		wmprintf("Failed to init work queue");
		return;
	}

	/* Initialize  pinmux driver */
	pinmux_drv_init();

	/* Open pinmux driver */
	pinmux_dev = pinmux_drv_open("MDEV_PINMUX");

	/* Initialize GPIO driver */
	gpio_drv_init();

	/* Open GPIO driver */
	gpio_dev = gpio_drv_open("MDEV_GPIO");

	wqtest_configure_gpio_led(board_led_1().gpio);
	wqtest_configure_gpio_led(board_led_2().gpio);
	wqtest_configure_gpio_led(board_led_3().gpio);
	wqtest_configure_gpio_led(board_led_4().gpio);

	led[0].led_no = board_led_1().gpio;
	led[0].led_state = false;
	led[1].led_no = board_led_2().gpio;
	led[1].led_state = false;
	led[2].led_no = board_led_3().gpio;
	led[2].led_state = false;
	led[3].led_no = board_led_4().gpio;
	led[3].led_state = false;

	wq_job_t job = {
		.job_func = toggle_led,
		.param = &led[0],
		.periodic_ms = 500,
		.initial_delay_ms = 5000,
	};

	job_handle_t job_handle[4];
	work_enqueue(wq_handle, &job, &job_handle[0]);

	job.periodic_ms = 500;
	job.initial_delay_ms = 5100;
	job.param = &led[1];
	work_enqueue(wq_handle, &job, &job_handle[1]);

	job.periodic_ms = 500;
	job.initial_delay_ms = 5200;
	job.param = &led[2];
	work_enqueue(wq_handle, &job, &job_handle[2]);

	job.periodic_ms = 500;
	job.initial_delay_ms = 5300;
	job.param = &led[3];
	work_enqueue(wq_handle, &job, &job_handle[3]);
	/* Wait for work to happen */
	os_thread_sleep(15000);

	work_dequeue(wq_handle, &job_handle[0]);
	work_dequeue(wq_handle, &job_handle[1]);
	work_dequeue(wq_handle, &job_handle[2]);
	work_dequeue(wq_handle, &job_handle[3]);
	work_queue_deinit(&wq_handle);

	/* Off LEDs */
	gpio_drv_write(gpio_dev, board_led_1().gpio, GPIO_IO_HIGH);
	gpio_drv_write(gpio_dev, board_led_2().gpio, GPIO_IO_HIGH);
	gpio_drv_write(gpio_dev, board_led_3().gpio, GPIO_IO_HIGH);
	gpio_drv_write(gpio_dev, board_led_4().gpio, GPIO_IO_HIGH);

	gpio_drv_close(gpio_dev);
	pinmux_drv_close(pinmux_dev);

	wmprintf("Test 1 success. Heap: %d\r\n", os_get_free_size());
}

/* Create and then destroy 'n' work queues and do a simple work */
static void wq_test_2()
{
	wmprintf("Work queue test 2. Heap: %d bytes\r\n",
		 os_get_free_size());
	const int queue_max_cnt = 200;
	wq_handle_t wq_handle[queue_max_cnt];

	test_2_cnt = 0;

	int queue_cnt, rv;
	for (queue_cnt = 0; queue_cnt < queue_max_cnt;
	     queue_cnt++) {
		rv = work_queue_init(NULL, &wq_handle[queue_cnt]);
		if (rv != WM_SUCCESS) {
			wmprintf("Note: Failed to init work queue %d\r\n",
				 queue_cnt);
			break;
		}
	}

	int i;
	wmprintf("%d work queues were created\r\n", queue_cnt);

	wq_job_t job = {
		.job_func = inc_data,
		.param = NULL,
		.periodic_ms = 0,
		.initial_delay_ms = 0,
	};

	for (i = 0; i < queue_cnt; i++) {
		if (work_enqueue(wq_handle[i], &job, NULL) < 0) {
			wmprintf("work enque failed (%d)\r\n", i);
			break;
		}
	}

	/* Wait for some time */
	os_thread_sleep(os_msec_to_ticks(50));
	for (i = 0; i < queue_cnt; i++)
		work_queue_deinit(&wq_handle[i]);

	if (test_2_cnt != queue_cnt) {
		wmprintf("Counter 2 success: Expected: %d Actual: %d\r\n",
			 queue_cnt, test_2_cnt);
		wmprintf("Test 2 failed\r\n");
	} else
		wmprintf("Test 2 success. Heap: %d\r\n", os_get_free_size());

	/* os_thread_sleep(1000); */
}

/* Create and then destroy 'n' work queues and do a simple work but this
 * time randomly enque and deque. The 'success' is that there is no hard
 * fault and any other system error. Which could occur if not dequeued
 * properly.
*/
static void wq_test_3()
{
	wmprintf("Work queue test 3. Heap: %d bytes\r\n",
		 os_get_free_size());
	const int queue_max_cnt = 200;
	wq_handle_t wq_handle[queue_max_cnt];

	wq_job_t job = {
		.job_func = inc_data,
		.param = NULL,
		.periodic_ms = 0,
		.initial_delay_ms = 0,
	};

	int queue_cnt, rv;
	for (queue_cnt = 0; queue_cnt < queue_max_cnt;
	     queue_cnt++) {
		rv = work_queue_init(NULL, &wq_handle[queue_cnt]);
		if (rv != WM_SUCCESS) {
			wmprintf("Note: Failed to init work queue %d\r\n",
				 queue_cnt);
			break;
		}

		job.initial_delay_ms = rand() % 1000;

		if (work_enqueue(wq_handle[queue_cnt], &job, NULL) < 0) {
			wmprintf("work enque failed: %d\r\n", queue_cnt);
			break;
		}
	}

	int i;
	wmprintf("%d work queues were created\r\n", queue_cnt);

	for (i = 0; i < queue_cnt; i++) {
		if (rand() % 2) {
			work_queue_deinit(&wq_handle[i]);
			wq_handle[i] = 0;
		}
	}

	/* Wait for some time to ensure all jobs are finished */
	os_thread_sleep(os_msec_to_ticks(1000));

	/* de-init the remaining */
	for (i = 0; i < queue_cnt; i++) {
		if (wq_handle[i]) {
			work_queue_deinit(&wq_handle[i]);
			wq_handle[i] = 0;
		}
	}

	wmprintf("Test 3 success Heap: %d bytes\r\n",
		 os_get_free_size());
}


const char *owner_1 = "ow1", *owner_2 = "ow2";
int ow1_cnt, ow2_cnt;

static int counter_callback(void *param)
{
	char *owner = (char *)param;

	if (!strcmp(owner_1, owner))
		ow1_cnt++;
	else if (!strcmp(owner_2, owner))
		ow2_cnt++;

	return WM_SUCCESS;
}

/* Test work_dequeue_owner_all() */
void wq_test_4()
{
	wmprintf("Work queue test 4. Heap: %d bytes\r\n",
		 os_get_free_size());

	wq_handle_t wq_handle;
	int rv = work_queue_init(NULL, &wq_handle);
	if (rv != WM_SUCCESS) {
		wmprintf("%s: wq init failed\r\n", __func__);
		return;
	}

	/* Job 1, Owner 1 */
	wq_job_t job = {
		.job_func = counter_callback,
		.param = (void *)owner_1,
		.periodic_ms = 0,
		.initial_delay_ms = 1000 + (rand() % 1000),
	};
	strncpy(job.owner, owner_1, MAX_OWNER_NAME_LEN);
	work_enqueue(wq_handle, &job, NULL);

	/* Job 2 Owner_2*/
	strncpy(job.owner, owner_2, MAX_OWNER_NAME_LEN);
	job.param = (void *) owner_2;
	job.initial_delay_ms = 2000 + (rand() % 1000);
	work_enqueue(wq_handle, &job, NULL);

	/* Job 3 Owner_1 */
	strncpy(job.owner, owner_1, MAX_OWNER_NAME_LEN);
	job.param = (void *) owner_1;
	job.initial_delay_ms = 1000 + (rand() % 1000);
	work_enqueue(wq_handle, &job, NULL);

	/* Job 4 Owner_2 */
	strncpy(job.owner, owner_2, MAX_OWNER_NAME_LEN);
	job.param = (void *) owner_2;
	job.initial_delay_ms = 2000 + (rand() % 1000);
	work_enqueue(wq_handle, &job, NULL);

	/* Wait for all jobs of owner_1 to complete */
	os_thread_sleep(os_msec_to_ticks(1500));

	work_dequeue_owner_all(wq_handle, owner_2);

	/* Sleep so that work queue get time to dequeue */
	os_thread_sleep(2000);

	work_queue_deinit(&wq_handle);

	if (ow1_cnt != 2 || ow2_cnt != 0) {
		wmprintf("Test 4 fail\r\n");
	} else {
		wmprintf("Test 4 success. Heap: %d bytes\r\n",
			 os_get_free_size());
	}
}

#define T5_NO_OF_JOBS DEFAULT_ISR_JOBS_RESERVE
static int counter_test_5;
static wq_handle_t test_5_wq_handle;
static int tick_cb_count;

static int test_5_callback(void *param)
{
	/* wmprintf("%s: %d\r\n", __func__, counter_test_5); */
	counter_test_5--;
	return WM_SUCCESS;
}

static void test_5_tick_function_cb()
{
	/*
	 * Enque job one more than T5_NO_OF_JOBS. Expected result is the
	 * last attempt should fail as all reserve jobs are over.
	 */
	if (tick_cb_count == -1) {
		return;
	}

	tick_cb_count--;
	/* Job 1, Owner 1 */
	wq_job_t job = {
		.job_func = test_5_callback,
		.periodic_ms = 0,
		.initial_delay_ms = 1000,
	};
	job.owner[0] = 0;
	work_enqueue(test_5_wq_handle, &job, NULL);
}

/* Check ISR job enque */
void wq_test_5()
{
	wmprintf("Work queue test 5. Heap: %d bytes\r\n",
		 os_get_free_size());

	int rv = work_queue_init(NULL, &test_5_wq_handle);
	if (rv != WM_SUCCESS) {
		wmprintf("%s: wq init failed\r\n", __func__);
		return;
	}

	/* Set to number */
	tick_cb_count = T5_NO_OF_JOBS;
	counter_test_5 = T5_NO_OF_JOBS + 1;

	/* Register so that we get callback from ISR context */
	os_setup_tick_function(test_5_tick_function_cb);
	os_thread_sleep(os_msec_to_ticks(2000));

	/*
	 * Reset to zero so that the tick callback enques one more
	 * job. This tests that the reserved jobs are returned back to
	 * list and this job should be scheduled successfully.
	 */
	tick_cb_count = 0;
	os_thread_sleep(os_msec_to_ticks(1500));

	os_remove_tick_function(test_5_tick_function_cb);

	work_queue_deinit(&test_5_wq_handle);

	if (counter_test_5 != 0) {
		wmprintf("Test 5 fail\r\n");
	} else {
		wmprintf("Test 5 success. Heap: %d bytes\r\n",
			 os_get_free_size());
	}
}

#define STANDARD_RET_VALUE -100
#define TEST_6_DELAY 500
static int test_6_callback(void *param)
{
	int sleep_ms = (int) param;
	/* wmprintf("%s: %d\r\n", __func__, counter_test_5); */
	os_thread_sleep(sleep_ms);
	return STANDARD_RET_VALUE;
}

/* Check work_enqueue_and_wait() */
void wq_test_6()
{
	wmprintf("Work queue test 6. Heap: %d bytes\r\n",
		 os_get_free_size());

	wq_handle_t test_6_wq_handle;
	int rv = work_queue_init(NULL, &test_6_wq_handle);
	if (rv != WM_SUCCESS) {
		wmprintf("%s: wq init failed\r\n", __func__);
		return;
	}

	/* Job 1, Owner 1 */
	wq_job_t job;
	memset(&job, 0x00, sizeof(wq_job_t));
	job.job_func = test_6_callback;
	job.param = (void *)TEST_6_DELAY;

	/* Check infinite wait */
	int start_tick = os_ticks_get();
	int ret_status;
	rv = work_enqueue_and_wait(test_6_wq_handle, &job, OS_WAIT_FOREVER,
				   &ret_status);
	if (rv != WM_SUCCESS) {
		wmprintf("%s: wq enqueue and wait failed\r\n", __func__);
		return;
	}

	if (ret_status != STANDARD_RET_VALUE) {
		wmprintf("%s: Return value not correct\r\n", __func__);
		goto test_6_fail;
	}

	if ((os_ticks_get() - start_tick) < TEST_6_DELAY) {
		wmprintf("%s: wait did not work\r\n", __func__);
		goto test_6_fail;
	}

	/* Check timeout */
	rv = work_enqueue_and_wait(test_6_wq_handle, &job, TEST_6_DELAY / 2,
				   &ret_status);
	if (rv != -WM_E_TIMEOUT) {
		wmprintf("%s: wq enq and wait failed (TO)\r\n", __func__);
		return;
	}

	/* Wait till job is complete before de-init */
	os_thread_sleep(TEST_6_DELAY);
	work_queue_deinit(&test_6_wq_handle);
	wmprintf("Test 6 success. Heap: %d bytes\r\n",
			 os_get_free_size());
	return;

 test_6_fail:
	work_queue_deinit(&test_6_wq_handle);
	wmprintf("Test 6 fail\r\n");
}

int work_queue_tests_init()
{

	int rv = os_mutex_create(&wq_test_mutex, "wq-test-m", OS_MUTEX_INHERIT);
	if (rv != WM_SUCCESS)
		return rv;

	wmprintf("===========================\r\n");
	wq_test_1();
	wmprintf("===========================\r\n");
	wq_test_2();
	int i = 15;
	while (i--) {
		wmprintf("Work queue test 3: %d\r\n", i);
		wq_test_3();
	}
	wmprintf("===========================\r\n");
	wq_test_4();
	wmprintf("===========================\r\n");
	wq_test_5();
	wmprintf("===========================\r\n");
	wq_test_6();
	wmprintf("===========================\r\n");

	os_mutex_delete(&wq_test_mutex);
	wmprintf("All tests done !\r\n");
	return 0;
}
