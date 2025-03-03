/*
 * hal_sleep_common.h
 *
 *  Created on: 2015?12?5?
 *      Author: MTK10267
 */

#ifndef GVA_DRIVER_CHIP_MT2523_INC_HAL_SLEEP_MANAGER_PLATFORM_H_
#define GVA_DRIVER_CHIP_MT2523_INC_HAL_SLEEP_MANAGER_PLATFORM_H_
#include "hal_platform.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

typedef enum {
    HAL_SLEEP_MANAGER_STATE_ACTIVE,                                                /**< sleep_manager is in active state*/
    HAL_SLEEP_MANAGER_STATE_IDLE,                                                  /**< sleep_manager is in idle state. */
    HAL_SLEEP_MANAGER_STATE_SLEEP,                                                 /**< sleep_manager is in sleep state. */
    HAL_SLEEP_MANAGER_STATE_DEEP_SLEEP                                             /**< sleep_manager is in deep sleep state. */
} hal_sleep_manager_sleep_mode_enum_t;

#endif /* HAL_SLEEP_MANAGER_ENABLED */
#endif /* GVA_DRIVER_CHIP_MT2523_INC_HAL_SLEEP_MANAGER_PLATFORM_H_ */
