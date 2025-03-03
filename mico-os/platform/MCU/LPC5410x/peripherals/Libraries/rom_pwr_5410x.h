/*
 * @brief LPC5410X Power ROM API declarations and functions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __ROM_PWR_5410X_H_
#define __ROM_PWR_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup PWRD_5410X CHIP: LPC5410X Power ROM API declarations and functions
 * @ingroup ROMAPI_5410X
 * @{
 */

/* 'mode' input values to set_voltage ROM function */
typedef enum {
	POWER_LOW_POWER_MODE = 0,
	POWER_BALANCED_MODE,
	POWER_HIGH_PERFORMANCE
} PERF_MODE_T;

/* 'mode' input values to power_mode_configure ROM function */
typedef enum {
	POWER_SLEEP = 0,
	POWER_DEEP_SLEEP,
	POWER_POWER_DOWN,
	POWER_DEEP_POWER_DOWN
} POWER_MODE_T;

/** @brief Power ROM indirect function structure
 * Do not use these functions as direct calls to ROM. Instead, use the
 * wrapper functions provided by the Power library (power_lib_5410x.h)
 */
typedef struct {
	void (*set_pll)(unsigned int cmd[], unsigned int resp[]);
	void (*set_power)(unsigned int cmd[], unsigned int resp[]);
	void (*power_mode_configure)(unsigned int power_mode, unsigned int peripheral_ctrl, unsigned int gint_flag);
	void (*clear_flash_data_latch)(void);
	void (*set_vd_level)(uint32_t domain, uint32_t level, uint32_t flevel);
	void (*set_lpvd_level)(uint32_t vd1LpLevel, uint32_t vd2LpLevel, uint32_t vd3LpLevel, uint32_t vd8LpLevel,
		  uint32_t flevel, uint32_t vd1_lp_full_poweren, uint32_t vd2_lp_full_poweren, uint32_t vd3_lp_full_poweren , uint32_t vd8_lp_full_poweren);
	void (*set_clamp_level)(uint32_t domain, uint32_t low_level, uint32_t high_level, uint32_t enable_low_clamp, uint32_t enable_high_clamp);
	void (*enter_RBB)( void );
	void (*enter_NBB)( void );
	void (*enter_FBB)( void );
	void (*set_aclkgate)(unsigned aclkgate);
	unsigned (*get_aclkgate)(void);
} PWRD_API_T;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ROM_PWR_5410X_H_ */
