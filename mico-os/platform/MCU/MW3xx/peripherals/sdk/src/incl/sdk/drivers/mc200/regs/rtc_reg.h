/****************************************************************************//**
 * @file     rtc_reg.h
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

#ifndef _RTC_REG_H
#define _RTC_REG_H

struct rtc_reg {
    /* 0x00: Counter Enable Register */
    union {
        struct {
            uint32_t CNT_START      :  1;  /* [0]     w/o */
            uint32_t CNT_STOP       :  1;  /* [1]     w/o */
            uint32_t CNT_RESET      :  1;  /* [2]     w/o */
            uint32_t RESERVED_15_3  : 13;  /* [15:3]  rsvd */
            uint32_t CNT_RUN        :  1;  /* [16]    r/o */
            uint32_t CNT_RST_DONE   :  1;  /* [17]    r/o */
            uint32_t STS_RESETN     :  1;  /* [18]    r/o */
            uint32_t RESERVED_31_19 : 13;  /* [31:19] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNT_EN;

    uint8_t zReserved0x04[28];  /* pad 0x04 - 0x1f */

    /* 0x20: Interrupt Raw Register */
    union {
        struct {
            uint32_t RESERVED_15_0  : 16;  /* [15:0]  rsvd */
            uint32_t CNT_UPP_INT    :  1;  /* [16]    r/w1clr */
            uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_RAW;

    /* 0x24: Interrupt Register */
    union {
        struct {
            uint32_t RESERVED_15_0  : 16;  /* [15:0]  rsvd */
            uint32_t CNT_UPP_INTR   :  1;  /* [16]    r/o */
            uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT;

    /* 0x28: Interrupt Mask Register */
    union {
        struct {
            uint32_t RESERVED_15_0  : 16;  /* [15:0]  rsvd */
            uint32_t CNT_UPP_MSK    :  1;  /* [16]    r/w */
            uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_MSK;

    uint8_t zReserved0x2c[20];  /* pad 0x2c - 0x3f */

    /* 0x40: Counter Control Register */
    union {
        struct {
            uint32_t RESERVED_3_0   :  4;  /* [3:0]   rsvd */
            uint32_t CNT_DBG_ACT    :  1;  /* [4]     r/w */
            uint32_t RESERVED_7_5   :  3;  /* [7:5]   rsvd */
            uint32_t CNT_UPDT_MOD   :  2;  /* [9:8]   r/w */
            uint32_t RESERVED_31_10 : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNT_CNTL;

    uint8_t zReserved0x44[12];  /* pad 0x44 - 0x4f */

    /* 0x50: Counter Value Register */
    union {
        struct {
            uint32_t CNT_VAL        : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } CNT_VAL;

    uint8_t zReserved0x54[12];  /* pad 0x54 - 0x5f */

    /* 0x60: Counter Upper Value Register */
    union {
        struct {
            uint32_t UPP_VAL        : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } CNT_UPP_VAL;

    uint8_t zReserved0x64[28];  /* pad 0x64 - 0x7f */

    /* 0x80: Clock control register */
    union {
        struct {
            uint32_t RESERVED_7_0   :  8;  /* [7:0]   rsvd */
            uint32_t CLK_DIV        :  4;  /* [11:8]  r/w */
            uint32_t RESERVED_31_12 : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_CNTL;

};

typedef volatile struct rtc_reg rtc_reg_t;

#ifdef RTC_IMPL
BEGIN_REG_SECTION(rtc_registers)
rtc_reg_t RTCREG;
END_REG_SECTION(rtc_registers)
#else
extern rtc_reg_t RTCREG;
#endif

#endif /* _RTC_REG_H */
