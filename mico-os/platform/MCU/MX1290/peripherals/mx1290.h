/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2017 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */

#ifndef _HAL_MX1290_H_
#define _HAL_MX1290_H_


#include "mx1290_vector.h"


#define __CM3_REV                      0x0200    /**< Core revision r0p0 */
#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               4         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         1         /**< Vendor specific implementation of SysTickConfig is defined *///see vPortSetupTimerInterrupt

#if !defined  (__FPU_PRESENT) 
#define __FPU_PRESENT             1       /*!< FPU present                                   */
#define __VFP_FP__	1
#endif /* __FPU_PRESENT */   


#endif //_HAL_8710B_H_
