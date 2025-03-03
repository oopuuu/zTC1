/****************************************************************************//**
 * @file     dma_reg.h
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

#ifndef _DMA_REG_H
#define _DMA_REG_H

struct dma_reg {
    struct {
        /* 0x000: Channel 0 source address */
        union {
            struct {
                uint64_t SAR            : 32;  /* [31:0]  r/w */
                uint64_t RESERVED_63_32 : 32;  /* [63:32] rsvd */
            } BF;
            uint64_t WORDVAL;
        } SAR;

        /* 0x008: Channel 0 destination address */
        union {
            struct {
                uint64_t DAR            : 32;  /* [31:0]  r/w */
                uint64_t RESERVED_63_32 : 32;  /* [63:32] rsvd */
            } BF;
            uint64_t WORDVAL;
        } DAR;

        uint8_t zReserved0x010[8];  /* pad 0x010 - 0x017 */

        /* 0x018: Channel 0 control */
        union {
            struct {
                uint64_t INT_EN         :  1;  /* [0]     r/w */
                uint64_t DST_TR_WIDTH   :  3;  /* [3:1]   r/w */
                uint64_t SRC_TR_WIDTH   :  3;  /* [6:4]   r/w */
                uint64_t DINC           :  2;  /* [8:7]   r/w */
                uint64_t SINC           :  2;  /* [10:9]  r/w */
                uint64_t DEST_MSIZE     :  2;  /* [12:11] r/w */
                uint64_t RESERVED_13    :  1;  /* [13]    rsvd */
                uint64_t SRC_MSIZE      :  2;  /* [15:14] r/w */
                uint64_t RESERVED_19_16 :  4;  /* [19:16] rsvd */
                uint64_t TT_FC          :  2;  /* [21:20] r/w */
                uint64_t RESERVED_31_22 : 10;  /* [31:22] rsvd */
                uint64_t BLOCK_TS       : 10;  /* [41:32] r/w */
                uint64_t RESERVED_63_42 : 22;  /* [63:42] rsvd */
            } BF;
            uint64_t WORDVAL;
        } CTL;

        uint8_t zReserved0x020[32];  /* pad 0x020 - 0x03f */

        /* 0x040: Channel 0 configuration */
        union {
            struct {
                uint64_t RESERVED_4_0   :  5;  /* [4:0]   rsvd */
                uint64_t CH_PRIOR       :  3;  /* [7:5]   r/w */
                uint64_t CH_SUSP        :  1;  /* [8]     r/w */
                uint64_t FIFO_EMPTY     :  1;  /* [9]     r/o */
                uint64_t HS_SEL_DST     :  1;  /* [10]    r/w */
                uint64_t HS_SEL_SRC     :  1;  /* [11]    r/w */
                uint64_t RESERVED_31_12 : 20;  /* [31:12] rsvd */
                uint64_t FCMODE         :  1;  /* [32]    r/w */
                uint64_t FIFO_MODE      :  1;  /* [33]    r/w */
                uint64_t RESERVED_38_34 :  5;  /* [38:34] rsvd */
                uint64_t SRC_PER        :  4;  /* [42:39] r/w */
                uint64_t DEST_PER       :  4;  /* [46:43] r/w */
                uint64_t RESERVED_63_47 : 17;  /* [63:47] rsvd */
            } BF;
            uint64_t WORDVAL;
        } CFG;

        uint8_t zReserved0x048[16];  /* pad 0x048 - 0x057 */
    } CHANNEL[8];

    /* 0x2c0: Raw Status for IntTfr Interrupt */
    union {
        struct {
            uint64_t RAW            :  8;  /* [7:0]   r/w */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } RAWTFR;

    /* 0x2c8: Raw Status for IntBlock Interrupt */
    union {
        struct {
            uint64_t RAW            :  8;  /* [7:0]   r/w */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } RAWBLOCK;

    /* 0x2d0: Raw Status for IntSrcTran Interrupt */
    union {
        struct {
            uint64_t RAW            :  8;  /* [7:0]   r/w */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } RAWSRCTRAN;

    /* 0x2d8: Raw Status for IntDstTran Interrupt */
    union {
        struct {
            uint64_t RAW            :  8;  /* [7:0]   r/w */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } RAWDSTTRAN;

    /* 0x2e0: Raw Status for IntErr Interrupt */
    union {
        struct {
            uint64_t RAW            :  8;  /* [7:0]   r/w */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } RAWERR;

    /* 0x2e8: Status for IntTfr Interrupt */
    union {
        struct {
            uint64_t STATUS         :  8;  /* [7:0]   r/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } STATUSTFR;

    /* 0x2f0: Status for IntBlock Interrupt */
    union {
        struct {
            uint64_t STATUS         :  8;  /* [7:0]   r/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } STATUSBLOCK;

    /* 0x2f8: Status for IntSrcTran Interrupt */
    union {
        struct {
            uint64_t STATUS         :  8;  /* [7:0]   r/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } STATUSSRCTRAN;

    /* 0x300: Status for IntDstTran Interrupt */
    union {
        struct {
            uint64_t STATUS         :  8;  /* [7:0]   r/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } STATUSDSTTRAN;

    /* 0x308: Status for IntErr Interrupt */
    union {
        struct {
            uint64_t STATUS         :  8;  /* [7:0]   r/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } STATUSERR;

    /* 0x310: Mask for IntTfr Interrupt */
    union {
        struct {
            uint64_t INT_MASK       :  8;  /* [7:0]   r/w */
            uint64_t INT_MASK_WE    :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } MASKTFR;

    /* 0x318: Mask for IntBlock Interrupt */
    union {
        struct {
            uint64_t INT_MASK       :  8;  /* [7:0]   r/w */
            uint64_t INT_MASK_WE    :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } MASKBLOCK;

    /* 0x320: Mask for IntSrcTran Interrupt */
    union {
        struct {
            uint64_t INT_MASK       :  8;  /* [7:0]   r/w */
            uint64_t INT_MASK_WE    :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } MASKSRCTRAN;

    /* 0x328: Mask for IntDstTran Interrupt */
    union {
        struct {
            uint64_t INT_MASK       :  8;  /* [7:0]   r/w */
            uint64_t INT_MASK_WE    :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } MASKDSTTRAN;

    /* 0x330: Mask for IntErr Interrupt */
    union {
        struct {
            uint64_t INT_MASK       :  8;  /* [7:0]   r/w */
            uint64_t INT_MASK_WE    :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } MASKERR;

    /* 0x338: Clear for IntTfr Interrupt */
    union {
        struct {
            uint64_t CLEAR          :  8;  /* [7:0]   w/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } CLEARTFR;

    /* 0x340: Clear for IntBlock Interrupt */
    union {
        struct {
            uint64_t CLEAR          :  8;  /* [7:0]   w/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } CLEARBLOCK;

    /* 0x348: Clear for IntSrcTran Interrupt */
    union {
        struct {
            uint64_t CLEAR          :  8;  /* [7:0]   w/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } CLEARSRCTRAN;

    /* 0x350: Clear for IntDstTran Interrupt */
    union {
        struct {
            uint64_t CLEAR          :  8;  /* [7:0]   w/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } CLEARDSTTRAN;

    /* 0x358: Clear for IntErr Interrupt */
    union {
        struct {
            uint64_t CLEAR          :  8;  /* [7:0]   w/o */
            uint64_t RESERVED_63_8  : 56;  /* [63:8]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } CLEARERR;

    /* 0x360: Status for each Interrupt type */
    union {
        struct {
            uint64_t TFR            :  1;  /* [0]     r/o */
            uint64_t BLOCK          :  1;  /* [1]     r/o */
            uint64_t SRCT           :  1;  /* [2]     r/o */
            uint64_t DSTT           :  1;  /* [3]     r/o */
            uint64_t ERR            :  1;  /* [4]     r/o */
            uint64_t RESERVED_63_5  : 59;  /* [63:5]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } STATUSINT;

    /* 0x368: Source Software Transaction Request register */
    union {
        struct {
            uint64_t SRC_REQ        :  8;  /* [7:0]   r/w */
            uint64_t SRC_REQ_WE     :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } REQSRCREG;

    /* 0x370: Destination Software Transaction Request register */
    union {
        struct {
            uint64_t DST_REQ        :  8;  /* [7:0]   r/w */
            uint64_t DST_REQ_WE     :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } REQDSTREG;

    /* 0x378: Source Single Transaction Request register */
    union {
        struct {
            uint64_t SRC_SGLREQ     :  8;  /* [7:0]   r/w */
            uint64_t SRC_SGLREQ_WE  :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } SGLREQSRCREG;

    /* 0x380: Destination Single Transaction Request register */
    union {
        struct {
            uint64_t DST_SGLREQ     :  8;  /* [7:0]   r/w */
            uint64_t DST_SGLREQ_WE  :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } SGLREQDSTREG;

    uint8_t zReserved0x388[16];  /* pad 0x388 - 0x397 */

    /* 0x398: DMA Configuration Register */
    union {
        struct {
            uint64_t DMA_EN         :  1;  /* [0]     r/w */
            uint64_t RESERVED_63_1  : 63;  /* [63:1]  rsvd */
        } BF;
        uint64_t WORDVAL;
    } DMACFGREG;

    /* 0x3a0: Channel enable register */
    union {
        struct {
            uint64_t CH_EN          :  8;  /* [7:0]   r/w */
            uint64_t CH_EN_WE       :  8;  /* [15:8]  r/w */
            uint64_t RESERVED_63_16 : 48;  /* [63:16] rsvd */
        } BF;
        uint64_t WORDVAL;
    } CHENREG;

};

typedef volatile struct dma_reg dma_reg_t;

#ifdef DMA_IMPL
BEGIN_REG_SECTION(dma_registers)
dma_reg_t DMAREG;
END_REG_SECTION(dma_registers)
#else
extern dma_reg_t DMAREG;
#endif

#endif /* _DMA_REG_H */
