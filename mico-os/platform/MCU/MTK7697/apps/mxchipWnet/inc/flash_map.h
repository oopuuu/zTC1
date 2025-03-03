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

#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__


#define FLASH_LOADER_SIZE           0x8000        /*  32KB */
#define FLASH_COMM_CONF_SIZE        0x0000        /*   0KB, dummy for 97 restore default build */
#define FLASH_STA_CONF_SIZE         0x0000        /*   0KB, dummy for 97 restore default build */
#define FLASH_AP_CONF_SIZE          0x0000        /*   0KB, dummy for 97 restore default build */
#define FLASH_N9_RAM_CODE_SIZE      0x71000       /* 452KB */
#define FLASH_CM4_XIP_CODE_SIZE     0x1ED000      /* 1972KB */
#define FLASH_TMP_SIZE              0x18A000      /* 1576KB */
#define FLASH_USR_CONF_SIZE         0x10000       /*  64KB */


#define CM4_FLASH_LOADER_ADDR       0x0
#define CM4_FLASH_COMM_CONF_ADDR    (CM4_FLASH_LOADER_ADDR      + FLASH_LOADER_SIZE) /* dummy for 97 restore default build */
#define CM4_FLASH_N9_RAMCODE_ADDR   (CM4_FLASH_LOADER_ADDR    + FLASH_LOADER_SIZE)
#define CM4_FLASH_CM4_ADDR          (CM4_FLASH_N9_RAMCODE_ADDR + FLASH_N9_RAM_CODE_SIZE)
#define CM4_FLASH_TMP_ADDR          (CM4_FLASH_CM4_ADDR        + FLASH_CM4_XIP_CODE_SIZE)
#define CM4_FLASH_USR_CONF_ADDR     (CM4_FLASH_TMP_ADDR        + FLASH_TMP_SIZE)



#endif // __FLASH_MAP_H__
