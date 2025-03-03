/*! \file overlays.h
 * \brief Overlay Manager
 *
 * This provides support for loading various overlays from flash to RAM.  This
 * is very useful in scenarios where two or more sections are mutually exclusive
 * to each other and have to be used only one at a time. The overlay manager can
 * be instructed to load the required section in ram as desired.
 *
 * An overlay can contain multiple ranges. Each range within an overlay can be
 * loaded at the same ram location. Each range may contain text, data and
 * bss. The overlay_load() function can be used to load a given range into RAM.
 *
 * Application developers may optionally use the ov_utl utility
 * (sdk/tools/bin) to create and manage overlays.
 */
/*  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */
#ifndef _OVERLAYS_H_
#define _OVERLAYS_H_

#include <wm_os.h>
#include <wmlog.h>

#define overlays_e(...)                              \
		wmlog_e("overlays", ##__VA_ARGS__)
#define overlays_w(...)                              \
		wmlog_w("overlays", ##__VA_ARGS__)


/** Overlay Range */
struct overlay_ranges {
	/** The start address in flash of one range */
	char *or_flash_start;
	/** The end address in flash of one range */
	char *or_flash_end;
	/** The start address of the bss section that should be zeroed out */
	char *o_bss_start;
	/** The end address of the bss section, before which the zeroing will
	 * stop */
	char *o_bss_end;
};

/** The overlay structure */
struct overlay {
	/** A user-friendly name for the overlay */
	char         *o_name;
	/** The start address of the overlay in RAM */
	char         *o_ram_start;
	/** The number of parts that can be overlaid on top of each other */
	unsigned char o_no_parts;
	/** A pointer to an array that contains the flash parts */
	struct overlay_ranges *o_range;
};

/** Load Overlay
 *
 * The overlay load function loads an overlay from the flash into ram. In the
 * process it overwrites any contents already in the flash.
 *
 * \note This function assumes that the flash address are relative to the start
 * of the firmware binary before any headers are appended to the firmware
 * image. This eases the linker script since this care need not be taken in the
 * script.
 *
 * \note This function also works in conjunction with the active passive
 * upgrades. It checks the boot2 setup to determine which partition have we
 * loaded from, and picks the overlay contents from the appropriate location.
 *
 * \note Along with loading the text and data sections from flash it also zeroes
 * out the bss section.
 *
 * \param ov The overlay structure that describes the overlay
 * \param index The range that should be loaded from this overlay
 *
 * \return WM_SUCCESS on success, error otherwise
 */
int overlay_load(struct overlay *ov, unsigned char index);

#endif /* ! _OVERLAYS_H_ */
