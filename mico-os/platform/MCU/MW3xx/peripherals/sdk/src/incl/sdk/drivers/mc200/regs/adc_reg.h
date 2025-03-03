/****************************************************************************//**
 * @file     adc_reg.h
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

#ifndef _ADC_REG_H
#define _ADC_REG_H

struct adc_reg {
    /* 0x00: Power Enable Register */
    union {
        struct {
            uint32_t GLOBAL_EN      :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1  : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR;

    /* 0x04: Clock Control Register */
    union {
        struct {
            uint32_t SOFT_RST       :  1;  /* [0]     r/w */
            uint32_t SOFT_CLK_RST   :  1;  /* [1]     r/w */
            uint32_t RESERVED_4_2   :  3;  /* [4:2]   rsvd */
            uint32_t INT_CLK_DIV    :  5;  /* [9:5]   r/w */
            uint32_t RESERVED_31_10 : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLKRST;

    /* 0x08: Command Register */
    union {
        struct {
            uint32_t CONV_START     :  1;  /* [0]     r/w */
            uint32_t TRIGGER_EN     :  1;  /* [1]     r/w */
            uint32_t TRIGGER_SEL    :  2;  /* [3:2]   r/w */
            uint32_t PWR_MODE       :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5  : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CMD;

    /* 0x0c: Time Interval Register */
    union {
        struct {
            uint32_t WARMUP_TIME    :  5;  /* [4:0]   r/w */
            uint32_t RESERVED_31_5  : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } INTERVAL;

    /* 0x10: Analog Configuration Register */
    union {
        struct {
            uint32_t RESERVED_0     :  1;  /* [0]     rsvd */
            uint32_t EXT_SEL        :  1;  /* [1]     r/w */
            uint32_t TS_EN          :  1;  /* [2]     r/w */
            uint32_t CAL            :  1;  /* [3]     r/w */
            uint32_t RESERVED_5_4   :  2;  /* [5:4]   rsvd */
            uint32_t VREF_SEL       :  2;  /* [7:6]   r/w */
            uint32_t AMUX_SEL       :  4;  /* [11:8]  r/w */
            uint32_t SINGLEDIFF     :  1;  /* [12]    r/w */
            uint32_t OSR            :  2;  /* [14:13] r/w */
            uint32_t BIAS_SEL       :  1;  /* [15]    r/w */
            uint32_t PGA            :  2;  /* [17:16] r/w */
            uint32_t VREF_BFSEL     :  1;  /* [18]    r/w */
            uint32_t IN_BFSEL       :  1;  /* [19]    r/w */
            uint32_t RESERVED_31_20 : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ANA;

    /* 0x14: DMA Control Register */
    union {
        struct {
            uint32_t DMA_EN         :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1  : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMAR;

    uint8_t zReserved0x18[4];  /* pad 0x18 - 0x1b */

    /* 0x1c: Status Register */
    union {
        struct {
            uint32_t ACT            :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1  : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS;

    /* 0x20: Interrupt Status Register */
    union {
        struct {
            uint32_t RDY            :  1;  /* [0]     r/o */
            uint32_t GAINSAT        :  1;  /* [1]     r/o */
            uint32_t OFFSAT         :  1;  /* [2]     r/o */
            uint32_t DMA_ERR        :  1;  /* [3]     r/o */
            uint32_t FILTERSAT      :  1;  /* [4]     r/o */
            uint32_t RESERVED_31_5  : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x24: Interrupt Mask Register */
    union {
        struct {
            uint32_t RDY_MASK       :  1;  /* [0]     r/w */
            uint32_t GAINSAT_MASK   :  1;  /* [1]     r/w */
            uint32_t OFFSAT_MASK    :  1;  /* [2]     r/w */
            uint32_t DMA_ERR_MASK   :  1;  /* [3]     r/w */
            uint32_t FILTERSAT_MASK :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5  : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x28: Interrupt Raw Status Register */
    union {
        struct {
            uint32_t RDY_RAW        :  1;  /* [0]     r/o */
            uint32_t GAINSAT_RAW    :  1;  /* [1]     r/o */
            uint32_t OFFSAT_RAW     :  1;  /* [2]     r/o */
            uint32_t DMA_ERR_RAW    :  1;  /* [3]     r/o */
            uint32_t FILTERSAT_RAW  :  1;  /* [4]     r/o */
            uint32_t RESERVED_31_5  : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x2c: Interrup Clear Register */
    union {
        struct {
            uint32_t RDY_CLR        :  1;  /* [0]     w/o */
            uint32_t GAINSAT_CLR    :  1;  /* [1]     w/o */
            uint32_t OFFSAT_CLR     :  1;  /* [2]     w/o */
            uint32_t DMA_ERR_CLR    :  1;  /* [3]     w/o */
            uint32_t FILTERSAT_CLR  :  1;  /* [4]     w/o */
            uint32_t RESERVED_31_5  : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

    /* 0x30: Final Data Register */
    union {
        struct {
            uint32_t DATA           : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESULT;

    uint8_t zReserved0x34[4];  /* pad 0x34 - 0x37 */

    /* 0x38: Offset Calibration Data Register */
    union {
        struct {
            uint32_t SELF_CAL       : 16;  /* [15:0]  r/o */
            uint32_t SYS_CAL        : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } OFF_CAL;

    /* 0x3c: Gain Calibration Data Register */
    union {
        struct {
            uint32_t GAIN_CAL       : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GAIN_CAL;

};

typedef volatile struct adc_reg adc_reg_t;

#ifdef ADC_IMPL
BEGIN_REG_SECTION(adc_registers)
adc_reg_t ADCREG;
END_REG_SECTION(adc_registers)
#else
extern adc_reg_t ADCREG;
#endif

#endif /* _ADC_REG_H */
