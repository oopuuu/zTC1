/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <wm_os.h>
#include <wmstdio.h>

#include <flash.h>
#include <boot_flags.h>
#include <firmware_structure.h>
#include <partition.h>
#include <overlays.h>
#define COPY_BUF_SIZE    128

#define overlays_d(...) wmlog("ovrl", ##__VA_ARGS__);

/* The job of this function is to determine the partition that we have booted
 * from. Then find out the start of this partition. Read the image headers, so
 * that we know exactly what should be the offset that should be added to the
 * linker script address to get the real flash address.
 */
static int overlay_get_flash_offset(mdev_t **fl_dev, uint32_t *offset)
{
	uint8_t boot_part = boot_get_partition_no();
	short index = 0;
	int err;
	struct partition_entry *pe;
	flash_desc_t fd;
	struct img_hdr ih;

	*offset = 0;
	*fl_dev = NULL;

	part_init();
	pe = part_get_layout_by_id(FC_COMP_FW, &index);
	if (!pe) {
		overlays_e("No firmware partition present");
		return -WM_FAIL;
	}

	if (boot_part) {
		/* If boot_part is 0, then we already have the pe for that. But
		 * if boot_part is 1, then we need to read the next pe.
		 */
		pe = part_get_layout_by_id(FC_COMP_FW, &index);
		if (!pe) {
			overlays_e("No second firmware partition present");
			return -WM_FAIL;
		}
	}
	part_to_flash_desc(pe, &fd);

	*fl_dev = flash_drv_open(fd.fl_dev);
	if (*fl_dev == NULL) {
		overlays_e("Failed to open flash");
		return -WM_FAIL;
	}

	err = flash_drv_read(*fl_dev, (uint8_t *)&ih, sizeof(ih),
			     fd.fl_start);
	if (err != WM_SUCCESS) {
		overlays_e("Error reading flash\r\n");
		return -WM_FAIL;
	}
	overlays_d("boot_part = %d, flash_start = 0x%0x seg_cnt = %d",
		 boot_part, fd.fl_start, SEG_CNT);
	*offset = fd.fl_start + sizeof(ih) + (SEG_CNT *
					      sizeof(struct seg_hdr));
	return WM_SUCCESS;
}

/* Load the contents from flash_start to flash_end after ram_start */
int overlay_load(struct overlay *ov, unsigned char index)
{
	mdev_t *fl_dev = NULL;
	char *flash_start = ov->o_range[index].or_flash_start;
	char *flash_end = ov->o_range[index].or_flash_end;
	char *ram_start = ov->o_ram_start;
	char *buf = NULL;
	uint32_t flash_offset;
	int err = WM_SUCCESS, pending_read, rd_size;

	if (ov->o_name) {
		overlays_d("Load index %d of overlay %s", index, ov->o_name);
	}

	buf = os_mem_alloc(COPY_BUF_SIZE);
	if (buf == NULL) {
		overlays_e("Not enough memory");
		err = -WM_E_NOMEM;
		goto err_ret;
	}
	pending_read = flash_end - flash_start;

	err = overlay_get_flash_offset(&fl_dev, &flash_offset);
	if (err != WM_SUCCESS) {
		overlays_e("Couldn't get flash_offset");
		goto err_ret;
	}

	overlays_d("Flash offset = 0x%0x", flash_offset);
	flash_start += flash_offset;
	overlays_d("load: flash_start = 0x%0x flash_end = 0x%0x size = 0x%0x",
		   flash_start, flash_end, pending_read);
	overlays_d("bss_start = 0x%0x bss_end = 0x%0x ram_start = 0x%0x",
		   ov->o_range[index].o_bss_start, ov->o_range[index].o_bss_end,
		   ram_start);
	while (pending_read > 0) {
		/* Read in our buffer */
		rd_size = (pending_read < COPY_BUF_SIZE) ?
			pending_read : COPY_BUF_SIZE;

		err = flash_drv_read(fl_dev, (uint8_t *)buf, rd_size,
				     (uint32_t)flash_start);
		if (err != WM_SUCCESS) {
			overlays_e("Error reading flash");
			goto err_ret;
		}
		flash_start += rd_size;
		pending_read -= rd_size;

		/* Copy to RAM */
		memcpy(ram_start, buf, rd_size);
		ram_start += rd_size;
	}

	/* Clear bss to the byte before o_bss_end */
	memset(ov->o_range[index].o_bss_start, 0,
	       (ov->o_range[index].o_bss_end - ov->o_range[index].o_bss_start));
#ifdef CONFIG_DEBUG_BUILD
	wmprintf("[ovrl] First 8 bytes of the overlay:");
	ram_start = ov->o_ram_start;
	int i;
	for (i = 0; i < 8; i++)
		wmprintf("%0x ", ram_start[i]);
	wmprintf("\r\n");
#endif

err_ret:
	if (buf)
		os_mem_free(buf);
	flash_drv_close(fl_dev);
	return err;
}
