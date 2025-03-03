/*
 * Copyright 2014, Marvell International Ltd.
 * All Rights Reserved.
 */

/* Test Program for block_alloc */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cli.h>
#include <cli_utils.h>
#include <wm_os.h>
#include <block_alloc.h>
#include <wmtime.h>

#define BA_DEBUG_AND_ERR_LOG_ENABLE
#define MAX_BLOCK_COUNT 512

#ifdef BA_DEBUG_AND_ERR_LOG_ENABLE
#define ba_test_e(...)\
	wmprintf("[ba_test] " __VA_ARGS__)
#define ba_test_d(...)\
	wmprintf("[ba_test] " __VA_ARGS__)
#else
#define ba_test_e(...) (void)0
#define ba_test_d(...) (void)0
#endif

#define mod_512(num) (((num) & 511) + 1)
#define mod_32(num) (((num) & 31) + 1)

struct pool_info *pool;

static void print_pool_bitmap(uint32_t *bitmap, unsigned long max_block_count)
{
	int i;
	ba_test_d("Bitmap is : ");
#ifdef BA_DEBUG_AND_ERR_LOG_ENABLE
	for (i = 0; i <= max_block_count/32; i++) {
		wmprintf(" %x", *(bitmap + i));
	}
	wmprintf("\r\n");
#endif

}


static void print_pool_info(struct pool_info *b)
{
	ba_test_d("Pool info:\r\n");
	ba_test_d("Pool address : %p\r\n", b->head);
	ba_test_d("Pool size (block_count * block_size): (%d * %d ) %d\r\n",
		  b->max_block_count, b->block_size,
		  b->block_size * b->max_block_count);
	print_pool_bitmap(b->bitmap, b->max_block_count);
}

static int ba_test_case1()
{
	int ret;
	char *buf;
	int block_size, block_count;
	void *alloc;
	block_size = mod_32(rand());
	block_count = mod_512(rand());
	buf = os_mem_alloc(block_size * block_count);
	if (!buf) {
		ba_test_e("Buffer memory allocation failed."
			  " Not enough memory\r\n");
		return -WM_E_NOMEM;
	}
	pool = ba_pool_create(block_size, buf, block_size * block_count);
	if (!pool) {
		ba_test_e("Memory pool allocation failed."
			  " Not enough memory\r\n");
		goto err;
	}

	print_pool_info(pool);

	alloc = ba_block_allocate(pool);
	if (!alloc) {
		ba_test_e("Block allocation failed."
			  " No free space remaining in memory pool\r\n");
		goto err;
	}
	print_pool_bitmap(pool->bitmap, pool->max_block_count);

	ret = ba_block_release(pool, alloc);
	if (ret != WM_SUCCESS) {
		ba_test_e("Failed to release block.\r\n");
		goto err;
	}
	print_pool_bitmap(pool->bitmap, pool->max_block_count);

	ret = ba_pool_delete(pool);
	if (ret != WM_SUCCESS) {
		ba_test_e("Failed to delete pool.");
		goto err;
	}
	print_pool_bitmap(pool->bitmap, pool->max_block_count);

	os_mem_free(buf);
	return WM_SUCCESS;
err:
	os_mem_free(buf);
	return -WM_FAIL;
}

static int ba_test_case2()
{
	int i = 0;
	char *buf;
	int block_size, block_count;
	void *alloc[MAX_BLOCK_COUNT];
	block_size = mod_32(rand());
	block_count = mod_512(rand());
	buf = os_mem_alloc(block_size * block_count);
	if (!buf) {
		ba_test_e("Buffer memory allocation failed."
			  " Not enough memory\r\n");
		return -WM_E_NOMEM;
	}
	pool = ba_pool_create(block_size, buf, block_size * block_count);
	if (!pool) {
		ba_test_e("Memory pool allocation failed."
			  " Not enough memory\r\n");
		goto err;
	}

	print_pool_info(pool);

	while (1) {
		alloc[i] = ba_block_allocate(pool);
		if (!alloc[i]) {
			if (i == pool->max_block_count) {
				print_pool_bitmap(pool->bitmap,
						  pool->max_block_count);
				break;
			} else {
				ba_test_d("Failed block allocation at "
					  "index %d\r\n", i);
				goto err;
			}
		}
		i++;
	}

	os_mem_free(buf);
	return WM_SUCCESS;
err:
	os_mem_free(buf);
	return -WM_FAIL;
}


static int ba_test_case3()
{
	int ret, i = 0, j;
	char *buf;
	int block_size, block_count, rand_block;
	void *alloc[MAX_BLOCK_COUNT];
	block_size = mod_32(rand());
	block_count = mod_512(rand());
	buf = os_mem_alloc(block_size * block_count);
	if (!buf) {
		ba_test_e("Buffer memory allocation failed."
			  " Not enough memory\r\n");
		return -WM_E_NOMEM;
	}
	pool = ba_pool_create(block_size, buf, block_size * block_count);
	if (!pool) {
		ba_test_e("Memory pool allocation failed."
			  " Not enough memory\r\n");
		goto err;
	}

	print_pool_info(pool);

	while (1) {
		alloc[i] = ba_block_allocate(pool);
		if (!alloc[i]) {
			if (i == pool->max_block_count) {
				print_pool_bitmap(pool->bitmap,
						  pool->max_block_count);
				break;
			} else {
				ba_test_d("Failed block allocation at "
					  "index %d\r\n", i);
				goto err;
			}
		}
		i++;
	}

	rand_block = mod_32(rand());

	ba_test_d("Deleting blocks at index multiple of %d\r\n", rand_block);
	for (i = 0; i < rand_block; i++) {
		for (j = i; j < pool->max_block_count; j += rand_block) {
			ret = ba_block_release(pool, alloc[j]);
			if (ret != WM_SUCCESS) {
				ba_test_d("Failed to delete block at index "
					  "%d\r\n", j);
				goto err;
			}
		}
		print_pool_bitmap(pool->bitmap, pool->max_block_count);
	}

	os_mem_free(buf);
	return WM_SUCCESS;

err:
	os_mem_free(buf);
	return -WM_FAIL;

}

static int ba_test_case_4_1()
{
	char *buf;
	int block_size, block_count;
	block_size = mod_32(rand());
	block_count = mod_512(rand());
	buf = os_mem_alloc(block_size * block_count);
	if (!buf) {
		ba_test_e("Buffer memory allocation failed."
			  " Not enough memory\r\n");
		return -WM_E_NOMEM;
	}
	pool = ba_pool_create(block_size, buf, block_size * block_count + 1);
	if (!pool) {
		ba_test_e("Validation handled: Incorrect memory pool size\r\n");
		return WM_SUCCESS;
	}

	print_pool_info(pool);
	os_mem_free(buf);
	return -WM_FAIL;
}


static int ba_test_case4_2()
{
	int ret, i = 0;
	char *buf;
	int block_size, block_count, rand_block;
	void *alloc[MAX_BLOCK_COUNT];
	block_size = mod_32(rand());
	block_count = mod_512(rand());
	buf = os_mem_alloc(block_size * block_count);
	if (!buf) {
		ba_test_e("Buffer memory allocation failed."
			  " Not enough memory\r\n");
		return -WM_E_NOMEM;
	}
	pool = ba_pool_create(block_size, buf, block_size * block_count);
	if (!pool) {
		ba_test_e("Memory pool allocation failed."
			  " Not enough memory\r\n");
		goto err;
	}

	print_pool_info(pool);

	rand_block = rand() % block_count + 1;

	for (i = 0; i < rand_block; i++) {
		alloc[i] = ba_block_allocate(pool);
		if (!alloc[i]) {
			ba_test_d("Failed block allocation at "
				  "index %d\r\n", i);
			goto err;
		}
	}

	rand_block = rand() % rand_block;
	ret = ba_block_release(pool, alloc[rand_block]);
	if (ret != WM_SUCCESS) {
		ba_test_e("Failed to release block at the index %d\r\n",
			  rand_block);
		goto err;
	}

	ret = ba_block_release(pool, alloc[rand_block]);
	if (ret != WM_SUCCESS) {
		ba_test_e("Validation handled: Try to release block which is "
			  "already free\r\n");
	}
	os_mem_free(buf);
	return WM_SUCCESS;

err:
	os_mem_free(buf);
	return -WM_FAIL;

}

static int ba_test_case4_3()
{
	int ret, i = 0;
	char *buf;
	int block_size, block_count, rand_block;
	void *alloc[MAX_BLOCK_COUNT];
	block_size = mod_32(rand());
	block_count = mod_512(rand());
	buf = os_mem_alloc(block_size * block_count);
	if (!buf) {
		ba_test_e("Buffer memory allocation failed."
			  " Not enough memory\r\n");
		return -WM_E_NOMEM;
	}
	pool = ba_pool_create(block_size, buf, block_size * block_count);
	if (!pool) {
		ba_test_e("Memory pool allocation failed."
			  " Not enough memory\r\n");
		goto err;
	}

	for (i = 0; i < pool->max_block_count; i++) {
		alloc[i] = ba_block_allocate(pool);
		if (!alloc[i]) {
			ba_test_d("Failed block allocation at "
				  "index %d\r\n", i);
			goto err;
		}
	}

	rand_block = rand() % pool->max_block_count;
	ret = ba_block_release(pool, alloc[rand_block]);
	if (ret != WM_SUCCESS) {
		ba_test_e("Failed to release block at the index %d\r\n",
			  rand_block);
		goto err;
	}

	print_pool_info(pool);

	ret = (int) ba_block_allocate(pool);
	if (ret) {
		ba_test_e("Validation handled: allocate freed block\r\n");
	} else {
		ba_test_e("Validation failed\r\n");
		goto err;
	}
	os_mem_free(buf);
	return WM_SUCCESS;

err:
	os_mem_free(buf);
	return -WM_FAIL;

}


static int ba_test_case4()
{
	int ret;
	ba_test_d("Validation 1: Creating memory pool of size not multiple of "
		  "block size\r\n");
	ret = ba_test_case_4_1();

	ba_test_d("Validation 2: Allocating blocks more than max block "
		  "count\r\n");
	ret = ba_test_case2();

	ba_test_d("Validation 3: Releasing block which is already "
		  "released\r\n");
	ret = ba_test_case4_2();

	ba_test_d("Validation 4: Allocating all blocks. Randomly deleting one "
		  "and allocating again\r\n");
	ret = ba_test_case4_3();

	return ret;
}

void ba_test(int argc, char **argv)
{
	int ret;

	srand(wmtime_time_get_posix());

	wmprintf("[ba_test] Test case 1: Basic Operations\r\n");
	wmprintf("[ba_test] APIs tested are : "
		 "ba_pool_create, ba_pool_delete, ba_block_alloc, "
		 "ba_block_release\r\n");
	ret = ba_test_case1();
	if (ret == WM_SUCCESS)
		wmprintf("[ba_test] Test case 1: Success\r\n");
	else
		goto done;

	wmprintf("[ba_test] Test case 2: Block Allocation\r\n[ba_test] To check"
		 " if maximum number of blocks allocated are as per"
		 " expectation\r\n");
	ret = ba_test_case2();
	if (ret == WM_SUCCESS)
		wmprintf("[ba_test] Test case 2: Success\r\n");
	else
		goto done;

	wmprintf("[ba_test] Test case 3: Block Allocation\r\n[ba_test] To check"
		 " if block allocation and release is working properly\r\n");
	ret = ba_test_case3();
	if (ret == WM_SUCCESS)
		wmprintf("[ba_test] Test case 3: Success\r\n");
	else
		goto done;

	wmprintf("[ba_test] Test case 4: Validations and Error handling\r\n");
	ret = ba_test_case4();
	if (ret == WM_SUCCESS)
		wmprintf("[ba_test] Test case 4: Success\r\n");
	else
		goto done;
done:
	return;
}


struct cli_command ba_command = {
	"slab-alloc-test", "Runs Block Alloc Tests", ba_test
};

int ba_cli_init(void)
{
	if (cli_register_command(&ba_command))
		return 1;
	return 0;
}

