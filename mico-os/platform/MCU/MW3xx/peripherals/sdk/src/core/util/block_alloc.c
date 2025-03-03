/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <wm_os.h>
#include <wm_utils.h>
#include <block_alloc.h>
#include <wmlog.h>

#define ba_e(...)\
	wmlog_e("ba", ##__VA_ARGS__)
#define ba_w(...)\
	wmlog_w("ba", ##__VA_ARGS__)

#ifdef CONFIG_BLOCK_ALLOCATOR_DEBUG
#define ba_d(...)\
	wmlog("ba", ##__VA_ARGS__)
#else
#define ba_d(...)
#endif /* ! BLOCK_ALLOCATOR_DEBUG */

#define ffs __builtin_ffs

#define mul_32(num) (num << 5)
#define div_32(num) (num >> 5)
#define div_8(num) (num >> 3)
#define modulo_32(num) (num & 31)
#define bit_sizeof(num) (num << 3)

#define get_word(bitmap, block_num) (bitmap + div_32(block_num))
#define get_bit(block_num) ((uint32_t)0x01 << modulo_32(block_num))

static void ba_bitmap_set_bit(struct pool_info *b, int block_num)
{
	/* Set the specified bit to 1 indicating that now it is allocated */
	*(get_word(b->bitmap, block_num)) |= get_bit(block_num);
}

static void ba_bitmap_reset_bit(struct pool_info *b, int block_num)
{
	/* Set the specified bit to 0 indicating that now it is free */
	*(get_word(b->bitmap, block_num)) &= ~(get_bit(block_num));
}

static int ba_bitmap_get_bit(struct pool_info *b, int block_num)
{
	/* return bit value in bitmap at location block_num */
	return *(get_word(b->bitmap, block_num)) & (get_bit(block_num));
}

/* This function returns the free block number if successful.
 * It returns -1 on error */
static int ba_get_free_block(const struct pool_info *b)
{
	int i, block_num = 0;
	for (i = 0; i < b->max_block_count; i +=
		     bit_sizeof(sizeof(*b->bitmap))) {
		block_num = ffs(~(*get_word(b->bitmap, i)));
		if (block_num != 0) {
			/* ffs returns bit position in a word. Hence,
			 *  adding 'i' to return position of bit in Bitmap
			 */
			return block_num - 1 + i;
		}
	}
	return -WM_FAIL;
}

static bool is_bitmap_empty(const struct pool_info *b)
{
	uint32_t i, *bitmap = b->bitmap;
	for (i = 0; i < b->max_block_count;  i +=
		     bit_sizeof(sizeof(*b->bitmap))) {
		if (*bitmap != 0)
			return false;
		bitmap++;
	}
	return true;
}

/* This function creates the pool for a particular block size */
struct pool_info *ba_pool_create(unsigned long block_size, void *buf,
				 unsigned long pool_size)
{
	int ret, alloc_bytes;
	struct pool_info *b;
	if (buf == NULL) {
		ba_d("Input pointer is NULL.");
		return NULL;
	}

	/* Ensure that the buffer given by user is 4 byte aligned */
	if ((unsigned)buf & 0x3) {
		ba_d("Input pointer needs to be word aligned.");
		return NULL;
	}

	if (pool_size % block_size) {
		ba_d("Pool size(%d) should be a multiple of block_size(%d)",
		     pool_size, block_size);
		return NULL;
	}

	b = os_mem_calloc(sizeof(struct pool_info));
	if (!b) {
		ba_d("Memory allocation for pool info failed");
		return NULL;
	}

	b->head = (char *) buf;
	ba_d("Address of pool info is: %p.", b->head);

	ret = os_mutex_create(&b->mutex, "ba", OS_MUTEX_INHERIT);
	if (ret) {
		goto err;
	}

	b->block_size = block_size;
	b->max_block_count = (pool_size / block_size);
	if (b->max_block_count == 0) {
		ba_d("Block count is 0");
		goto err;
	}

	alloc_bytes = ALIGN(b->max_block_count, bit_sizeof(sizeof(*b->bitmap)));
	b->bitmap = os_mem_calloc(div_8(alloc_bytes));
	if (!b->bitmap) {
		ba_d("Memory allocation for bitmap failed");
		goto err;
	}
	return b;
err:
	os_mem_free(b);
	return NULL;
}

/* This function destroys the pool (if created) */
int ba_pool_delete(struct pool_info *b)
{
	int ret;
	if (b == NULL) {
		ba_d("Input pointer is NULL.");
		return -WM_FAIL;
	}

	/* Delete the pool only if all its blocks are unallocated or free */
	if (is_bitmap_empty(b)) {
		ret = os_mutex_get(&b->mutex, OS_WAIT_FOREVER);
		if (ret) {
			ba_d("Failed to get the lock for block allocator.");
			return -WM_FAIL;
		}
		ret = os_mutex_delete(&b->mutex);
		if (!ret) {
			ba_d("Deleting the mutex for block allocator.");
		}
		os_mem_free(b->bitmap);
		b->bitmap = NULL;
		os_mem_free(b);
		return WM_SUCCESS;
	} else {
		ba_d("Trying to free a pool which has blocks allocated.");
		ba_d("Free all the blocks and then try to delete the pool.");
		return -WM_FAIL;

	}
}

/* This function is used to allocate a free block */
void *ba_block_allocate(struct pool_info *b)
{
	int block_num, ret;
	void *ptr;
	if (b == NULL) {
		ba_d("Input pointer is NULL or not initialized.");
		return NULL;
	}

	ret = os_mutex_get(&b->mutex, OS_WAIT_FOREVER);
	if (ret) {
		return NULL;
	}

	/* Get the position of the first free block */
	block_num = ba_get_free_block(b);
	if (block_num == -WM_FAIL || block_num >= b->max_block_count) {
		ba_d("No free block. Returning.");
		os_mutex_put(&b->mutex);
		return NULL;
	}
	ba_d("Block number to be allocated is: %d.", block_num);
	ptr = b->head + (b->block_size * block_num);

	/* Update the bitmap by setting the corresponding bit to 1 (indicating
	 * allocated) */
	ba_bitmap_set_bit(b, block_num);
	os_mutex_put(&b->mutex);
	return ptr;
}

/* This function is used to free an allocated block */
int ba_block_release(struct pool_info *b, void *alloc)
{
	int ret;
	unsigned int diff;

	if (b == NULL || alloc == NULL || alloc < b->head) {
		ba_d("Input pointer is NULL or not initialized.");
		return -WM_FAIL;
	}

	diff = (alloc - (void *)b->head);

	if (diff % b->block_size) {
		ba_d("Invalid address. Cannot free the memory.");
		return -WM_FAIL;
	} else {
		diff /= b->block_size;
	}

	ret = os_mutex_get(&b->mutex, OS_WAIT_FOREVER);
	if (ret) {
		ba_d("Failed to get the lock for block allocator.");
		return -WM_FAIL;
	}

	/* Check if the specified bit is allocated (1) else return error */
	if (!ba_bitmap_get_bit(b, diff)) {
		ba_d("Trying to free a location that is already free.");
		os_mutex_put(&b->mutex);
		return -WM_FAIL;
	}

	ba_bitmap_reset_bit(b, diff);
	os_mutex_put(&b->mutex);

	ba_d("Freed the requested block successfully.");
	return WM_SUCCESS;
}
