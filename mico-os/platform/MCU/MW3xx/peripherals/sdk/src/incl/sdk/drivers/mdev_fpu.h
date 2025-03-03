/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */


/*! \file mdev_fpu.h
 *  \brief Cortex M4 Floating Point Unit
 */


/**  Enable Full Access to FPU
 *
 *  The function enables the CP10 and CP11 for full access to FPU
 *  @note This API should be used only with Mw300 based platform
 */
#define fpu_init() FPU_Init()

/** Disable Access to FPU
 *
 *  The function disables the CP10 and CP11 for any access to FPU.
 *  Any attempted access generates a NOCP UsageFault.
 *  @note This API should be used only with Mw300 based platform
 */
#define fpu_deinit() FPU_Deinit()

