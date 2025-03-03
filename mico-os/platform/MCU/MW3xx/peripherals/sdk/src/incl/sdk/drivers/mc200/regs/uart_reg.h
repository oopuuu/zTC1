/****************************************************************************//**
 * @file     uart_reg.h
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

#ifndef _UART_REG_H
#define _UART_REG_H

struct uart_reg {
    /* 0x00: Receive Buffer Register */
    union {
        struct {
            uint32_t RBR                     :  8;  /* [7:0]   r/o */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF_RBR;
        struct {
            uint32_t DLL                     :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8          : 24;  /* [31:8]  r/o */
        } BF_DLL;
        struct {
            uint32_t THR                     :  8;  /* [7:0]   w/o */
            uint32_t RESERVED_31_8          : 24;  /* [31:8]  r/o */
        } BF_THR;
        uint32_t WORDVAL;
    } RBR_DLL_THR;

    /* 0x04: Interrupt Enable Register */
    union {
        struct {
            uint32_t ERBFI                   :  1;  /* [0]     r/w */
            uint32_t ETBEI                   :  1;  /* [1]     r/w */
            uint32_t ELSI                    :  1;  /* [2]     r/w */
            uint32_t EDSSI                   :  1;  /* [3]     r/w */
            uint32_t RESERVED_6_4            :  3;  /* [6:4]   rsvd */
            uint32_t PTIME                   :  1;  /* [7]     r/w */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF_IER;
        struct {
            uint32_t DLH                     :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8          : 24;  /* [31:8]  r/o */
        } BF_DLH;
        uint32_t WORDVAL;
    } IER_DLH;

    /* 0x08: Interrupt Identification Register */
    union {
        struct {
            uint32_t IID                     :  4;  /* [3:0]   r/o */
            uint32_t RSVD_5_4           :  2;  /* [5:4]   r/o */
            uint32_t FIFOSE                  :  2;  /* [7:6]   r/o */
            uint32_t RSVD_31_8          : 24;  /* [31:8]  r/o */
        } BF_IIR;
        struct {
            uint32_t FIFOE                   :  1;  /* [0]     w/o */
            uint32_t RFIFOR                  :  1;  /* [1]     w/o */
            uint32_t XFIFOR                  :  1;  /* [2]     w/o */
            uint32_t DMAM                    :  1;  /* [3]     w/o */
            uint32_t TET                     :  2;  /* [5:4]   w/o */
            uint32_t RT                      :  2;  /* [7:6]   w/o */
            uint32_t RSVD_31_8          : 24;  /* [31:8]  r/o */
        } BF_FCR;
        uint32_t WORDVAL;
    } IIR_FCR;

    /* 0x0c: Line Control Register */
    union {
        struct {
            uint32_t DLS                     :  2;  /* [1:0]   r/w */
            uint32_t STOP                    :  1;  /* [2]     r/w */
            uint32_t PEN                     :  1;  /* [3]     r/w */
            uint32_t EPS                     :  1;  /* [4]     r/w */
            uint32_t RESERVED_5              :  1;  /* [5]     rsvd */
            uint32_t BREAK                   :  1;  /* [6]     r/w */
            uint32_t DLAB                    :  1;  /* [7]     r/w */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LCR;

    /* 0x10: Modem Control Register */
    union {
        struct {
            uint32_t DTR                     :  1;  /* [0]     r/w */
            uint32_t RTS                     :  1;  /* [1]     r/w */
            uint32_t OUT1                    :  1;  /* [2]     r/w */
            uint32_t OUT2                    :  1;  /* [3]     r/w */
            uint32_t LOOPBACK                :  1;  /* [4]     r/w */
            uint32_t AFCE                    :  1;  /* [5]     r/w */
            uint32_t SIRE                    :  1;  /* [6]     r/w */
            uint32_t RESERVED_31_7           : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MCR;

    /* 0x14: Line Status Register */
    union {
        struct {
            uint32_t DR                      :  1;  /* [0]     r/o */
            uint32_t OE                      :  1;  /* [1]     r/o */
            uint32_t PE                      :  1;  /* [2]     r/o */
            uint32_t FE                      :  1;  /* [3]     r/o */
            uint32_t BI                      :  1;  /* [4]     r/o */
            uint32_t THRE                    :  1;  /* [5]     r/o */
            uint32_t TEMT                    :  1;  /* [6]     r/o */
            uint32_t RFE                     :  1;  /* [7]     r/o */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LSR;

    /* 0x18: Modem Status Register */
    union {
        struct {
            uint32_t DCTS                    :  1;  /* [0]     r/o */
            uint32_t DDSR                    :  1;  /* [1]     r/o */
            uint32_t TERI                    :  1;  /* [2]     r/o */
            uint32_t DDCD                    :  1;  /* [3]     r/o */
            uint32_t CTS                     :  1;  /* [4]     r/o */
            uint32_t DSR                     :  1;  /* [5]     r/o */
            uint32_t RI                      :  1;  /* [6]     r/o */
            uint32_t DCD                     :  1;  /* [7]     r/o */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MSR;

    /* 0x1c: Scratchpad Register */
    union {
        struct {
            uint32_t SCR                     :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SCR;

    /* 0x20: Low Power Divisor Latch Low */
    union {
        struct {
            uint32_t LPDLL                   :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LPDLL;

    /* 0x24: Low Power Divisor Latch High */
    union {
        struct {
            uint32_t LPDLH                   :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LPDLH;

    uint8_t zReserved0x28[8];  /* pad 0x28 - 0x2f */

    /* 0x30-0x6c: Shadow Receive Buffer Register */
    union {
        struct {
            uint32_t SRBR0                   :  8;  /* [7:0]   r/o */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF_SRBR0;
        struct {
            uint32_t STHR0                   :  8;  /* [7:0]   w/o */
            uint32_t RESERVED_31_8        : 24;  /* [31:8]  r/o */
        } BF_STHR0;
        uint32_t WORDVAL;
    } SRBR_STHR[16];

    /* 0x70: FIFO Access Register */
    union {
        struct {
            uint32_t FAR                     :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1           : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } FAR;

    /* 0x74: Transmit FIFO Read */
    union {
        struct {
            uint32_t TFR                     :  8;  /* [7:0]   r/o */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TFR;

    /* 0x78: Receive FIFO Write */
    union {
        struct {
            uint32_t RFWD                    :  8;  /* [7:0]   w/o */
            uint32_t RFPE                    :  1;  /* [8]     w/o */
            uint32_t RFFE                    :  1;  /* [9]     w/o */
            uint32_t RESERVED_31_10          : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RFW;

    /* 0x7c: UART Status register */
    union {
        struct {
            uint32_t BUSY                    :  1;  /* [0]     r/o */
            uint32_t TFNF                    :  1;  /* [1]     r/o */
            uint32_t TFE                     :  1;  /* [2]     r/o */
            uint32_t RFNE                    :  1;  /* [3]     r/o */
            uint32_t RFF                     :  1;  /* [4]     r/o */
            uint32_t RESERVED_31_5           : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } USR;

    /* 0x80: */
    union {
        struct {
            uint32_t TFL                     :  5;  /* [4:0]   r/o */
            uint32_t RESERVED_31_5           : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TFL;

    /* 0x84: Receive FIFO Level */
    union {
        struct {
            uint32_t RFL                     :  5;  /* [4:0]   r/o */
            uint32_t RESERVED_31_5           : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RFL;

    /* 0x88: Software Reset Register */
    union {
        struct {
            uint32_t UR                      :  1;  /* [0]     w/o */
            uint32_t RFR                     :  1;  /* [1]     w/o */
            uint32_t XFR                     :  1;  /* [2]     w/o */
            uint32_t RESERVED_31_3           : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SRR;

    /* 0x8c: Shadow Request to Send */
    union {
        struct {
            uint32_t SRTS                    :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1           : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SRTS;

    /* 0x90: Shadow Break Control Register */
    union {
        struct {
            uint32_t SBCB                    :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1           : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SBCR;

    /* 0x94: Shadow DMA Mode */
    union {
        struct {
            uint32_t SDMAM                   :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1           : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SDMAM;

    /* 0x98: Shadow FIFO Enable */
    union {
        struct {
            uint32_t SFE                     :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1           : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SFE;

    /* 0x9c: Shadow RCVR Trigger */
    union {
        struct {
            uint32_t SRT                     :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2           : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SRT;

    /* 0xa0: Shadow TX Empty Trigger */
    union {
        struct {
            uint32_t STET                    :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2           : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STET;

    /* 0xa4: Halt TX */
    union {
        struct {
            uint32_t HTX                     :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1           : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } HTX;

    /* 0xa8: DMA Software Acknowledge */
    union {
        struct {
            uint32_t DMASA                   :  1;  /* [0]     w/o */
            uint32_t RESERVED_31_1           : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMASA;

    uint8_t zReserved0xac[72];  /* pad 0xac - 0xf3 */

    /* 0xf4: Component Parameter Register */
    union {
        struct {
            uint32_t APB_DATA_WIDTH          :  2;  /* [1:0]   r/o */
            uint32_t RESERVED_3_2            :  2;  /* [3:2]   rsvd */
            uint32_t AFCE_MODE               :  1;  /* [4]     r/o */
            uint32_t THRE_MODE               :  1;  /* [5]     r/o */
            uint32_t SIR_MODE                :  1;  /* [6]     r/o */
            uint32_t SIR_LP_MODE             :  1;  /* [7]     r/o */
            uint32_t ADDITIONAL_FEAT         :  1;  /* [8]     r/o */
            uint32_t FIFO_ACCESS             :  1;  /* [9]     r/o */
            uint32_t FIFO_STAT               :  1;  /* [10]    r/o */
            uint32_t SHADOW                  :  1;  /* [11]    r/o */
            uint32_t UART_ADD_ENCODED_PARAMS :  1;  /* [12]    r/o */
            uint32_t DMA_EXTRA               :  1;  /* [13]    r/o */
            uint32_t RESERVED_15_14          :  2;  /* [15:14] rsvd */
            uint32_t FIFO_MODE               :  8;  /* [23:16] r/o */
            uint32_t RESERVED_31_24          :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CPR;

    /* 0xf8: Component Version */
    union {
        struct {
            uint32_t UART_COMPONENT_VERSION  : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } UCV;

    /* 0xfc: Component Type Register */
    union {
        struct {
            uint32_t PERIPHERAL_ID           : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } CTR;

};

typedef volatile struct uart_reg uart_reg_t;

#ifdef UART_IMPL
BEGIN_REG_SECTION(uart_registers)
uart_reg_t UARTREG;
END_REG_SECTION(uart_registers)
#else
extern uart_reg_t UARTREG;
#endif

#endif /* _UART_REG_H */

