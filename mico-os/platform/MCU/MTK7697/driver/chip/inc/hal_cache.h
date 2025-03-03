/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_CACHE_H__
#define __HAL_CACHE_H__
#include "hal_platform.h"

#ifdef HAL_CACHE_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup CACHE
 * @{
 * This section describes the programming interfaces of the CACHE controller designed by MediaTek.
 * The CACHE controller is a unified CACHE (I-CACHE and D-CACHE use the same CACHE). This CACHE driver allows the user to set the CACHE size, configure the cacheable region(s), enable/disable the CACHE and to perform CACHE maintenance operations, such as invalidate and flush. These maintenance operations may be performed on the entire CACHE or based on given address and length.
 *
 * @section HAL_CACHE_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the CACHE driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b CACHE                      | A collection of data duplicating original values stored in external memory, to accelerate the data access and enhance the MCU performance.|
 * |\b CACHE line                 | The smallest unit of memory that can be transferred between the main memory and the CACHE. Unless otherwise indicated, the CACHE line size is 8 words (64 Bytes).|
 * |\b flush                	  | Data in invalid CACHE line with dirty bit set is evicted to write-back buffer, then dirty bits are cleared. If the entry is invalid or the dirty flag is not set, leave as it is. The action does not clear the valid bit(s).|
 * |\b invalidate              	  | Unconditionally clear valid and dirty bits of corresponding CACHE line(s).|
 * |\b TCM              	      | Tightly Coupled Memory (TCM), memory which resides directly in the processor of a computer.|
 * |\b AHB              	      | Advanced High-performance Bus (AHB) is a bus protocol introduced in <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ihi0011a/index.html">Advanced Microcontroller Bus Architecture version 2</a> published by ARM Ltd company.|
 *
 * @section HAL_CACHE_Features_Chapter Supported features
 * Features supported by this module are listed below:
 *
 * - \b Configure \b the \b overall \b CACHE \b size.\n
 *   To reach the optimum system performance, the CACHE size can be set with flexibility. If the CACHE size is not set to the maximum (32KB), then the memory of the unused CACHE is reused by TCM.
 *   User can call the function hal_cache_set_size(), to set the CACHE size. The parameter of this function can be any value of type #hal_cache_size_t.
 *   Apply different parameters for function hal_cache_set_size() to set the cache size to one of the following 4 configurations:
 *  .
 *
 * |Parameter             | CACHE size     |
 * |----------------------|----------------|
 * | #HAL_CACHE_SIZE_32KB | 32KB 		   |
 * | #HAL_CACHE_SIZE_16KB | 16KB 		   |
 * | #HAL_CACHE_SIZE_8KB  | 8KB 		   |
 * | #HAL_CACHE_SIZE_0KB  | No CACHE 	   |
 *
 * - \b Using \b CACHE \b region \b to \b set \b cacheable \b attribute \b for \b a \b memory \b block.\n
 *   The CACHE controller has 16 regions for cacheable attribute setting. Note that each region can be continuous or non-continuous to each other. For those address ranges not covered by any region in the CACHE, the cacheable settings are set as uncacheable automatically. There is one restriction: different regions must not overlap.
 *   By calling the function hal_cache_region_config(), user can set cacheable memory blocks for each region. The parameter should be #hal_cache_region_config_t, and the field in the #hal_cache_region_config_t includes cache_region_address(must be 4KB aligned), cache_region_size(must be 4KB aligned) and cache_region_cacheable(indicates whether the corresponding region is cacheable or not). \n
 *
 * - \b CACHE \b flush/invalidate. \n
 *   The CACHE maintenance operation includes flush and invalidate. The two operations may be performed on the entire CACHE or on address-length basis. The CACHE driver API names distinguish one from the other by the word "all". For example, to invalidate all CACHE lines, call the API hal_cache_invalidate_all_cache_lines(). To invalidate the CACHE lines for a particular memory block, simply call hal_cache_invalidate_multiple_cache_lines(). Note that the parameters "address" and "length" must be supplied to specify the physical address range you want to perform the CACHE maintenance operation on. The usage of flush operation is the same as the invalidate operation described above.
 *
 *
 * @section HAL_CACHE_Driver_Usage_Chapter How to use this driver
 *
 * - \b Use \b CACHE \b driver \b to \b enable/disable \b CACHE.
 *  - hal_cache_region_enable()/ hal_cache_region_disable() is used to enable/disable cacheable attribute for a specified region.
 *  - hal_cache_enable()/ hal_cache_disable() is used to enable/disable the CACHE controller. If disabled, MCU memory accesses are all non-cacheable. If enabled, the settings in CACHE controller will take effect.
 *
 * - \b Use \b CACHE \b driver \b to \b set \b a \b cacheable \b region.
 *  - Step1: Call hal_cache_set_size() to set the total size to be used for CACHE.
 *  - Step2: Call hal_cache_region_config() to set a CACHE region with specified address and length(should call more than once if there are multiple regions to be configured as cacheable).
 *  - Step3: Call hal_cache_region_enable() to enable the CACHE function of the corresponding region configured in step2.
 *  - Step4: Call hal_cache_enable() to enable the cacheable/non-cacheable settings.
 *  - Sample code:
 *  @code
 *
 *  hal_cache_region_t region = HAL_CACHE_REGION_0;
 *	hal_cache_region_config_t region_config;
 *
 *	hal_cache_set_size(HAL_CACHE_SIZE_32K);//Set the total CACHE size.
 *
 *	region_config.cache_region_address = 0x10000000;//The start address of the region that is cacheable, that must be 4KB aligned.
 *	region_config.cache_region_size = 0x400000;//The size of the cacheable region.
 *
 *	hal_cache_region_config(region, &region_config);
 *	hal_cache_region_enable(region);
 *
 *	hal_cache_enable();
 *  @endcode
 *
 * - \b Use \b CACHE \b driver \b to \b invalidate/flush \b a \b cacheable \b region.\n
 *   - Step1: Call hal_cache_flush_multiple_cache_lines() to flush a cacheable memory specified by address and length.
 *  \n Call hal_cache_flush_one_cache_line() instead if just one line needs to be flushed.
 *   - Step2: Call hal_cache_invalidate_multiple_cache_lines() to invalidate a cacheable memory specified by address and length.
 *  \n Call hal_cache_invalidate_one_cache_line() instead if just one line needs to be invalidated.
 *   - Sample code:
 *   @code
 *
 *  hal_cache_region_t region = HAL_CACHE_REGION_0;
 *	hal_cache_region_config_t region_config;
 *
 *	hal_cache_set_size(HAL_CACHE_SIZE_32K);//Set the total CACHE size.
 *
 *	region_config.cache_region_address = 0x10000000;//The start address of the region that is cacheable, that must be 4KB aligned.
 *	region_config.cache_region_size = 0x400000;//The size of the cacheable region.
 *
 *	hal_cache_region_config(region, &region_config);
 *	hal_cache_region_enable(region);
 *
 *	hal_cache_enable();
 *
 *	hal_cache_flush_multiple_cache_lines(region_config.cache_region_address, region_config.cache_size);
 *	hal_cache_invalidate_multiple_cache_lines(region_config.cache_region_address, region_config.cache_size);
 *	@endcode
 *
 * - \b Use \b CACHE \b driver \b to \b invalidate/flush \b all \b cacheable \b regions. \n
 *   - Step1: Call hal_cache_flush_all_cache_lines().
 *   - Step2: Call hal_cache_invalidate_all_cache_lines().
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_cache_enum Enum
 *  @{
 */

/** @brief This enum defines the CACHE API return status*/
typedef enum {
    HAL_CACHE_STATUS_INVALID_PARAMETER = -7,    /**< Invalid parameter */
    HAL_CACHE_STATUS_ERROR_BUSY = -6,           /**< CACHE is busy */
    HAL_CACHE_STATUS_ERROR_CACHE_SIZE = -5,     /**< CACHE size is invalid, total CACHE size is not a value of type #hal_cache_size_t*/
    HAL_CACHE_STATUS_ERROR_REGION = -4,         /**< CACHE region error, CACHE region is not a value of type #hal_cache_region_t */
    HAL_CACHE_STATUS_ERROR_REGION_ADDRESS = -3, /**< CACHE region address error, CACHE region address is not 4KB aligned */
    HAL_CACHE_STATUS_ERROR_REGION_SIZE = -2,    /**< CACHE region size error, CACHE region size is not a multiple of 4KB */
    HAL_CACHE_STATUS_ERROR = -1,                /**< CACHE error , imprecise error*/
    HAL_CACHE_STATUS_OK = 0                     /**< CACHE ok */
} hal_cache_status_t;


/** @brief CACHE size*/
typedef enum {
    HAL_CACHE_SIZE_0KB = 0,                     /**< No CACHE */
    HAL_CACHE_SIZE_8KB = 1,                     /**< CACHE size is 8KB */
    HAL_CACHE_SIZE_16KB = 2,                    /**< CACHE size is 16KB */
    HAL_CACHE_SIZE_32KB = 3,                    /**< CACHE size is 32KB */
    HAL_CACHE_SIZE_MAX                          /**< MAX CACHE size (invalid) */
} hal_cache_size_t;


/** @brief CACHE region number*/
typedef enum {
    HAL_CACHE_REGION_0 = 0,                     /**< CACHE region 0 */
    HAL_CACHE_REGION_1 = 1,                     /**< CACHE region 1 */
    HAL_CACHE_REGION_2 = 2,                     /**< CACHE region 2 */
    HAL_CACHE_REGION_3 = 3,                     /**< CACHE region 3 */
    HAL_CACHE_REGION_4 = 4,                     /**< CACHE region 4 */
    HAL_CACHE_REGION_5 = 5,                     /**< CACHE region 5 */
    HAL_CACHE_REGION_6 = 6,                     /**< CACHE region 6 */
    HAL_CACHE_REGION_7 = 7,                     /**< CACHE region 7 */
    HAL_CACHE_REGION_8 = 8,                     /**< CACHE region 8 */
    HAL_CACHE_REGION_9 = 9,                     /**< CACHE region 9 */
    HAL_CACHE_REGION_10 = 10,                   /**< CACHE region 10 */
    HAL_CACHE_REGION_11 = 11,                   /**< CACHE region 11 */
    HAL_CACHE_REGION_12 = 12,                   /**< CACHE region 12 */
    HAL_CACHE_REGION_13 = 13,                   /**< CACHE region 13 */
    HAL_CACHE_REGION_14 = 14,                   /**< CACHE region 14 */
    HAL_CACHE_REGION_15 = 15,                   /**< CACHE region 15 */
    HAL_CACHE_REGION_MAX                        /**< Max CACHE region number (invalid) */
} hal_cache_region_t;

/**
 * @}
 */

/*****************************************************************************
 * Structures
 *****************************************************************************/

/** @defgroup hal_cache_struct Struct
 *  @{
 */

/** @brief CACHE region config structure, that contains the start address of the region (must be 4KB aligned), the size of the region, and whether the region is cacheable or not.*/
typedef struct {
    uint32_t cache_region_address;              /**< CACHE region start address */
    uint32_t cache_region_size;                 /**< CACHE region size */
} hal_cache_region_config_t;

/**
 * @}
 */

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief 	CACHE initialization function, set the default cacheable attribute for the project memory layout.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE initialization is successful. \n
 * #HAL_CACHE_STATUS_ERROR_BUSY, CACHE is busy.
 */
hal_cache_status_t hal_cache_init(void);


/**
 * @brief  CACHE deinitialization function. Make the CACHE module to its default state.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE deinitialization is successful.
 */
hal_cache_status_t hal_cache_deinit(void);


/**
 * @brief 	CACHE enable function. Enable the CACHE settings during a memory access. @sa hal_cache_region_disable().
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE enable is successful. \n
 * #HAL_CACHE_STATUS_ERROR_CACHE_SIZE, CACHE can not be enabled when CACHE size is 0KB.
 */
hal_cache_status_t hal_cache_enable(void);


/**
 * @brief CACHE disable function. Disable the CACHE settings. @sa hal_cache_region_enable().
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE disable is successful.
 */
hal_cache_status_t hal_cache_disable(void);


/**
 * @brief CACHE region enable function. Once hal_cache_enable() is called, the settings of the specified region take effect. @sa hal_cache_enable().
 * @param[in] region is the enabled region, this parameter can only be a value of type #hal_cache_region_t.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE region enable is successful. \n
 * #HAL_CACHE_STATUS_ERROR_REGION, the region is invalid. \n
 * #HAL_CACHE_STATUS_ERROR, the region is not configured before enable.
 */
hal_cache_status_t hal_cache_region_enable(hal_cache_region_t region);


/**
 * @brief CACHE region disable function. When this function is called, the CACHE settings of corresponding region are disabled, even if the hal_cache_enable() function is called. @sa hal_cache_disable().
 * @param[in] region is the disabled region, this parameter is any value of type #hal_cache_region_t.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE region disable is successful. \n
 * #HAL_CACHE_STATUS_ERROR_REGION, the region is invalid.
 */
hal_cache_status_t hal_cache_region_disable(hal_cache_region_t region);


/**
 * @brief Set the total size of the CACHE.
 * @param[in] cache_size should only be any value of type #hal_cache_size_t.
 * @return
 * #HAL_CACHE_STATUS_OK, the CACHE size setting is successful. \n
 * #HAL_CACHE_STATUS_ERROR_CACHE_SIZE, the size of CACHE is invalid.
 */
hal_cache_status_t hal_cache_set_size(hal_cache_size_t cache_size);

/**
 * @brief 	CACHE config function.
 * @param[in] region is the region that is configured.
 * @param[in] region_config is the configuration information of the region.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE region configuration is successful. \n
 * #HAL_CACHE_STATUS_INVALID_PARAMETER, region_config is NULL. \n
 * #HAL_CACHE_STATUS_ERROR_REGION, the region is invalid. \n
 * #HAL_CACHE_STATUS_ERROR_REGION_SIZE, the region size is invalid. \n
 * #HAL_CACHE_STATUS_ERROR_REGION_ADDRESS, the region address is invalid.
 */
hal_cache_status_t hal_cache_region_config(hal_cache_region_t region, const hal_cache_region_config_t *region_config);

/**
 * @brief  Invalidate one CACHE line by address.
 * @param[in] address is the start address of the memory that is invalidated.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE invalidate is successful. \n
 * #HAL_CACHE_STATUS_INVALID_PARAMETER, address is not CACHE line size aligned.
 */
hal_cache_status_t hal_cache_invalidate_one_cache_line(uint32_t address);

/**
 * @brief  Invalidate CACHE lines by address and length.
 * @param[in] address is the start address of the memory that is invalidated.
 * @param[in] length is the length of memory that to be invalidated.The unit of the memory length is in bytes and both address and length must be CACHE line size aligned when the API is called.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE invalidate is successful. \n
 * #HAL_CACHE_STATUS_INVALID_PARAMETER, either address or length is not CACHE line size aligned.
 */
hal_cache_status_t hal_cache_invalidate_multiple_cache_lines(uint32_t address, uint32_t length);


/**
 * @brief 	Invalidate all CACHE lines.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE invalidate is successful.
 */
hal_cache_status_t hal_cache_invalidate_all_cache_lines(void);

/**
 * @brief  Flush one CACHE line by address.
 * @param[in] address is the start address of the memory that is flushed.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE flush is successful. \n
 * #HAL_CACHE_STATUS_INVALID_PARAMETER, address is not CACHE line size aligned.
 */
hal_cache_status_t hal_cache_flush_one_cache_line(uint32_t address);

/**
 * @brief  Flush CACHE lines by address and length.
 * @param[in] address is the start address of the memory that is flushed.
 * @param[in] length is the length of the memory that to be flushed. The unit of the memory length is in bytes and both address and length must be CACHE line size aligned when the API is called.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE flush is successful. \n
 * #HAL_CACHE_STATUS_INVALID_PARAMETER, either address or length is not CACHE line size aligned.
 */
hal_cache_status_t hal_cache_flush_multiple_cache_lines(uint32_t address, uint32_t length);


/**
 * @brief 	Flush all CACHE lines.
 * @return
 * #HAL_CACHE_STATUS_OK, CACHE flush is successful.
 * @note Flush all CACHE lines.
 */
hal_cache_status_t hal_cache_flush_all_cache_lines(void);


#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
*/
#endif /*HAL_CACHE_MODULE_ENABLED*/
#endif /* __HAL_CACHE_H__ */

