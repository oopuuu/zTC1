/****************************************************************************//**
 * @file     crc_reg.h
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

#ifndef _CRC_REG_H
#define _CRC_REG_H

struct crc_reg {
    /* 0x00: Interrupt Status Register */
    union {
        struct {
            uint32_t STATUS        :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x04: Interrupt Raw Status Register */
    union {
        struct {
            uint32_t STATUS_RAW    :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x08: Interrupt Clear Register */
    union {
        struct {
            uint32_t CLEAR         :  1;  /* [0]     w/o */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

    /* 0x0c: Interrupt Mask Register */
    union {
        struct {
            uint32_t MASK          :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x10: CRC Module Control Register */
    union {
        struct {
            uint32_t ENABLE        :  1;  /* [0]     r/w */
            uint32_t MODE          :  3;  /* [3:1]   r/w */
            uint32_t RESERVED_31_4 : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL;

    /* 0x14: Stream Length Minus 1 Register */
    union {
        struct {
            uint32_t LENGTH_M1     : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } STREAM_LEN_M1;

    /* 0x18: Stream Input Register */
    union {
        struct {
            uint32_t DATA          : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } STREAM_IN;

    /* 0x1c: CRC Calculation Result */
    union {
        struct {
            uint32_t DATA          : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } RESULT;

};

typedef volatile struct crc_reg crc_reg_t;

#ifdef CRC_IMPL
BEGIN_REG_SECTION(crc_registers)
crc_reg_t CRCREG;
END_REG_SECTION(crc_registers)
#else
extern crc_reg_t CRCREG;
#endif

#endif /* _CRC_REG_H */
