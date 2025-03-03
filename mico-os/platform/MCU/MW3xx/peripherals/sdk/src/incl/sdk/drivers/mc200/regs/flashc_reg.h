/****************************************************************************//**
 * @file     flashc_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.2.0
 * @date     29-May-2013
 * @author   CE Application Team
 *
 * @note
 * Copyright (C) 2012 Marvell Technology Group Ltd. All rights reserved.
 *
 * @par
 * Marvell is supplying this software which provides customers with programming
 * information regarding the products. Marvell has no responsibility or 
 * liability for the use of the software. Marvell not guarantee the correctness 
 * of this software. Marvell reserves the right to make changes in the software 
 * without notification. 
 * 
 *******************************************************************************/

#ifndef _FLASHC_REG_H
#define _FLASHC_REG_H

struct flashc_reg {
    /* 0x0: Flash Controller Configuration Register */
    union {
        struct {
            uint32_t RESERVED_30_0 : 31;  /* [30:0]  rsvd */
            uint32_t FLASHC_PAD_EN :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } FCCR;

};

typedef volatile struct flashc_reg flashc_reg_t;

#ifdef FLASHC_IMPL
BEGIN_REG_SECTION(flashc_registers)
flashc_reg_t FLASHCREG;
END_REG_SECTION(flashc_registers)
#else
extern flashc_reg_t FLASHCREG;
#endif

#endif /* _FLASHC_REG_H */

