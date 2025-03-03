/****************************************************************************//**
 * @file     dac_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.0.0
 * @date     06-Feb-2013
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

#ifndef _DAC_REG_H
#define _DAC_REG_H

struct dac_reg {
    /* 0x00: DAC Control Register */
    union {
        struct {
            uint32_t REF_SEL           :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1     : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL;

    /* 0x04: DAC Status Register */
    union {
        struct {
            uint32_t A_DV              :  1;  /* [0]     r/o */
            uint32_t B_DV              :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2     : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS;

    /* 0x08: Channel A Control Register */
    union {
        struct {
            uint32_t A_EN              :  1;  /* [0]     r/w */
            uint32_t A_IO_EN           :  1;  /* [1]     r/w */
            uint32_t A_TRIG_EN         :  1;  /* [2]     r/w */
            uint32_t A_TRIG_SEL        :  2;  /* [4:3]   r/w */
            uint32_t A_TRIG_TYP        :  2;  /* [6:5]   r/w */
            uint32_t A_DEN             :  1;  /* [7]     r/w */
            uint32_t A_MODE            :  1;  /* [8]     r/w */
            uint32_t A_TRIA_HALF       :  1;  /* [9]     r/w */
            uint32_t A_TRIA_MAMP_SEL   :  4;  /* [13:10] r/w */
            uint32_t A_TRIA_STEP_SEL   :  2;  /* [15:14] r/w */
            uint32_t A_WAVE            :  2;  /* [17:16] r/w */
            uint32_t A_RANGE           :  2;  /* [19:18] r/w */
            uint32_t RESERVED_31_20    : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ACTRL;

    /* 0x0c: Channel B Control Register */
    union {
        struct {
            uint32_t B_EN              :  1;  /* [0]     r/w */
            uint32_t B_IO_EN           :  1;  /* [1]     r/w */
            uint32_t B_TRIG_EN         :  1;  /* [2]     r/w */
            uint32_t B_TRIG_SEL        :  2;  /* [4:3]   r/w */
            uint32_t B_TRIG_TYP        :  2;  /* [6:5]   r/w */
            uint32_t B_DEN             :  1;  /* [7]     r/w */
            uint32_t B_MODE            :  1;  /* [8]     r/w */
            uint32_t B_WAVE            :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11    : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } BCTRL;

    /* 0x10: Channel A Data Register */
    union {
        struct {
            uint32_t A_DATA            : 10;  /* [9:0]   r/w */
            uint32_t RESERVED_31_10    : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ADATA;

    /* 0x14: Channel B Data Register */
    union {
        struct {
            uint32_t B_DATA            : 10;  /* [9:0]   r/w */
            uint32_t RESERVED_31_10    : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } BDATA;

    /* 0x18: Interrupt Status Register */
    union {
        struct {
            uint32_t A_RDY_INT         :  1;  /* [0]     r/o */
            uint32_t B_RDY_INT         :  1;  /* [1]     r/o */
            uint32_t A_TO_INT          :  1;  /* [2]     r/o */
            uint32_t B_TO_INT          :  1;  /* [3]     r/o */
            uint32_t TRIA_OVFL_INT     :  1;  /* [4]     r/o */
            uint32_t RESERVED_31_5     : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x1c: Interrupt Mask Register */
    union {
        struct {
            uint32_t A_RDY_INT_MSK     :  1;  /* [0]     r/w */
            uint32_t B_RDY_INT_MSK     :  1;  /* [1]     r/w */
            uint32_t A_TO_INT_MSK      :  1;  /* [2]     r/w */
            uint32_t B_TO_INT_MSK      :  1;  /* [3]     r/w */
            uint32_t TRIA_OVFL_INT_MSK :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5     : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x20: Interrupt Raw Status Register */
    union {
        struct {
            uint32_t A_RDY_INT_RAW     :  1;  /* [0]     r/o */
            uint32_t B_RDY_INT_RAW     :  1;  /* [1]     r/o */
            uint32_t A_TO_INT_RAW      :  1;  /* [2]     r/o */
            uint32_t B_TO_INT_RAW      :  1;  /* [3]     r/o */
            uint32_t TRIA_OVFL_INT_RAW :  1;  /* [4]     r/o */
            uint32_t RESERVED_31_5     : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x24: Interrupt Clear Register */
    union {
        struct {
            uint32_t A_RDY_INT_CLR     :  1;  /* [0]     w/o */
            uint32_t B_RDY_INT_CLR     :  1;  /* [1]     w/o */
            uint32_t A_TO_INT_CLR      :  1;  /* [2]     w/o */
            uint32_t B_TO_INT_CLR      :  1;  /* [3]     w/o */
            uint32_t TRIA_OVFL_INT_CLR :  1;  /* [4]     w/o */
            uint32_t RESERVED_31_5     : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

    /* 0x28: Clock Register */
    union {
        struct {
            uint32_t FORCE_CLK_ON      :  1;  /* [0]     r/w */
            uint32_t CLK_CTRL          :  2;  /* [2:1]   r/w */
            uint32_t CLK_INV_SEL       :  1;  /* [3]     r/w */
            uint32_t SOFT_CLK_RST      :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5     : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK;

    /* 0x2c: Soft Reset Register */
    union {
        struct {
            uint32_t A_SOFT_RST        :  1;  /* [0]     r/w */
            uint32_t B_SOFT_RST        :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2     : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RST;

};

typedef volatile struct dac_reg dac_reg_t;

#ifdef DAC_IMPL
BEGIN_REG_SECTION(dac_registers)
dac_reg_t DACREG;
END_REG_SECTION(dac_registers)
#else
extern dac_reg_t DACREG;
#endif

#endif /* _DAC_REG_H */
