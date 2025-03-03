/****************************************************************************//**
 * @file     rc32m_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.3.0
 * @date     12-Aug-2013
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

#ifndef _RC32M_REG_H
#define _RC32M_REG_H

struct rc32m_reg {
    /* 0x00: Control Register */
    union {
        struct {
            uint32_t EN             :  1;  /* [0]     r/w */
            uint32_t CAL_EN         :  1;  /* [1]     r/w */
            uint32_t EXT_CODE_EN    :  1;  /* [2]     r/w */
            uint32_t PD             :  1;  /* [3]     r/w */
            uint32_t CODE_FR_EXT    :  8;  /* [11:4]  r/w */
            uint32_t CAL_DIV        :  2;  /* [13:12] r/w */
            uint32_t RESERVED_31_14 : 18;  /* [31:14] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL;

    /* 0x04: Status Register */
    union {
        struct {
            uint32_t CLK_RDY        :  1;  /* [0]     r/o */
            uint32_t CAL_DONE       :  1;  /* [1]     r/o */
            uint32_t CODE_FR_CAL    :  8;  /* [9:2]   r/o */
            uint32_t RESERVED_31_10 : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS;

    /* 0x08: Interrupt Status Register */
    union {
        struct {
            uint32_t CALDON_INT     :  1;  /* [0]     r/o */
            uint32_t CKRDY_INT      :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x0c: Interrupt Mask Register */
    union {
        struct {
            uint32_t CALDON_INT_MSK :  1;  /* [0]     r/w */
            uint32_t CKRDY_INT_MSK  :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x10: Interrupt Raw Status Register */
    union {
        struct {
            uint32_t CALDON_INT_RAW :  1;  /* [0]     r/o */
            uint32_t CKRDY_INT_RAW  :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x14: Interrupt Clear Register */
    union {
        struct {
            uint32_t CALDON_INT_CLR :  1;  /* [0]     r/w */
            uint32_t CKRDY_INT_CLR  :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

    /* 0x18: Clock Register */
    union {
        struct {
            uint32_t CLK_INV_SEL    :  1;  /* [0]     r/w */
            uint32_t FORCE_CLK_ON   :  1;  /* [1]     r/w */
            uint32_t REF_SEL        :  1;  /* [2]     r/w */
            uint32_t SOFT_CLK_RST   :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4  : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK;

    /* 0x1c: Soft Reset Register */
    union {
        struct {
            uint32_t SOFT_RST       :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1  : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RST;

    /* 0x20: Test Register */
    union {
        struct {
            uint32_t TEST_EN        :  1;  /* [0]     r/w */
            uint32_t TEST_SEL       :  2;  /* [2:1]   r/w */
            uint32_t RESERVED_31_3  : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TEST;

    /* 0x24: Reserved Register */
    union {
        struct {
            uint32_t UNUSED_15_0    : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RSVD;

};

typedef volatile struct rc32m_reg rc32m_reg_t;

#ifdef RC32M_IMPL
BEGIN_REG_SECTION(rc32m_registers)
rc32m_reg_t RC32MREG;
END_REG_SECTION(rc32m_registers)
#else
extern rc32m_reg_t RC32MREG;
#endif

#endif /* _RC32M_REG_H */

