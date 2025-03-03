/****************************************************************************//**
 * @file     sys_ctrl_reg.h
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

#ifndef _SYS_CTRL_REG_H
#define _SYS_CTRL_REG_H

struct sys_ctrl_reg {
    /* 0x00: Chip Revision Register */
    union {
        struct {
            uint32_t REV_ID             : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } REV_ID;

    /* 0x04: Memory Space Configuration Register */
    union {
        struct {
            uint32_t CFG                :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8      : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MEM;

    uint8_t zReserved0x08[40];  /* pad 0x08 - 0x2f */

    /* 0x30: DMA Handshaking Mapping Register */
    union {
        struct {
            uint32_t MAPPING_0          :  1;  /* [0]     r/w */
            uint32_t MAPPING_1          :  1;  /* [1]     r/w */
            uint32_t MAPPING_2          :  1;  /* [2]     r/w */
            uint32_t MAPPING_3          :  2;  /* [4:3]   r/w */
            uint32_t MAPPING_4          :  1;  /* [5]     r/w */
            uint32_t MAPPING_5          :  2;  /* [7:6]   r/w */
            uint32_t MAPPING_6          :  2;  /* [9:8]   r/w */
            uint32_t MAPPING_7          :  2;  /* [11:10] r/w */
            uint32_t MAPPING_8          :  1;  /* [12]    r/w */
            uint32_t MAPPING_9          :  1;  /* [13]    r/w */
            uint32_t MAPPING_10         :  1;  /* [14]    r/w */
            uint32_t MAPPING_11         :  1;  /* [15]    r/w */
            uint32_t MAPPING_12         :  2;  /* [17:16] r/w */
            uint32_t MAPPING_13         :  2;  /* [19:18] r/w */
            uint32_t MAPPING_14         :  1;  /* [20]    r/w */
            uint32_t MAPPING_15         :  1;  /* [21]    r/w */
            uint32_t RESERVED_31_22     : 10;  /* [31:22] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMA_HS;

    uint8_t zReserved0x34[8];  /* pad 0x34 - 0x3b */

    /* 0x3c: Peripheral SW reset */
    union {
        struct {
            uint32_t WDT_RSTN_EN        :  1;  /* [0]     r/w */
            uint32_t USB_RSTN_EN        :  1;  /* [1]     r/w */
            uint32_t RESERVED_2         :  1;  /* [2]     rsvd */
            uint32_t SDIO_RSTN_EN       :  1;  /* [3]     r/w */
            uint32_t GPT3_RSTN_EN       :  1;  /* [4]     r/w */
            uint32_t GPT2_RSTN_EN       :  1;  /* [5]     r/w */
            uint32_t GPT1_RSTN_EN       :  1;  /* [6]     r/w */
            uint32_t GPT0_RSTN_EN       :  1;  /* [7]     r/w */
            uint32_t SSP2_RSTN_EN       :  1;  /* [8]     r/w */
            uint32_t SSP1_RSTN_EN       :  1;  /* [9]     r/w */
            uint32_t SSP0_RSTN_EN       :  1;  /* [10]    r/w */
            uint32_t I2C2_RSTN_EN       :  1;  /* [11]    r/w */
            uint32_t I2C1_RSTN_EN       :  1;  /* [12]    r/w */
            uint32_t I2C0_RSTN_EN       :  1;  /* [13]    r/w */
            uint32_t UART3_RSTN_EN      :  1;  /* [14]    r/w */
            uint32_t UART2_RSTN_EN      :  1;  /* [15]    r/w */
            uint32_t UART1_RSTN_EN      :  1;  /* [16]    r/w */
            uint32_t UART0_RSTN_EN      :  1;  /* [17]    r/w */
            uint32_t FLASH_QSPI_RSTN_EN :  1;  /* [18]    r/w */
            uint32_t QSPI1_RSTN_EN      :  1;  /* [19]    r/w */
            uint32_t QSPI0_RSTN_EN      :  1;  /* [20]    r/w */
            uint32_t RESERVED_31_21     : 11;  /* [31:21] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI_SW_RST;

    /* 0x40: USB Control Register */
    union {
        struct {
            uint32_t PLL_LOCK_BYPASS    :  1;  /* [0]     r/w */
            uint32_t FSDRV_EN           :  4;  /* [4:1]   r/w */
            uint32_t EXT_FS_RCAL        :  4;  /* [8:5]   r/w */
            uint32_t RX_BUF_RTC         :  2;  /* [10:9]  r/w */
            uint32_t RX_BUF_WTC         :  2;  /* [12:11] r/w */
            uint32_t TX_BUF_RTC         :  2;  /* [14:13] r/w */
            uint32_t TX_BUF_WTC         :  2;  /* [16:15] r/w */
            uint32_t USBBUF_PDWN        :  1;  /* [17]    r/w */
            uint32_t USBBUF_PDWN_EN     :  1;  /* [18]    r/w */
            uint32_t DISABLE_EL16       :  1;  /* [19]    r/w */
            uint32_t RESERVED_31_20     : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USB_CTRL;

    /* 0x44: USB phy Control Register */
    union {
        struct {
            uint32_t RESERVED_12_0      : 13;  /* [12:0]  rsvd */
            uint32_t TX_LS              :  4;  /* [16:13] r/w */
            uint32_t LS_EN              :  4;  /* [20:17] r/w */
            uint32_t REG_PU_USB         :  1;  /* [21]    r/w */
            uint32_t RESERVED_31_22     : 10;  /* [31:22] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USB_PHY_CTRL;

    /* 0x44: USB Test Control Register1 */
    union {
        struct {
            uint32_t REG_TEST_PATTERN    :  8;  /* [7:0]   r/w */
            uint32_t REG_TEST_MODE       :  1;  /* [8]     r/w */
            uint32_t REG_TEST_EN         :  1;  /* [9]     r/w */
            uint32_t REG_TEST_SEL        :  1;  /* [10]    r/w */
            uint32_t REG_TEST_FAIL       :  1;  /* [11]    r/o */
            uint32_t REG_TEST_DONE       :  1;  /* [12]    r/o */
            uint32_t REG_RESERVE         :  8;  /* [20:13] r/w */
            uint32_t REG_PU_USB          :  1;  /* [21]    r/w */
            uint32_t REG_TESTMON         :  4;  /* [25:22] r/w */
            uint32_t RESERVED_31_26      :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USB_TEST_1;

    /* 0x48: */
    union {
        struct {
            uint32_t REG_DPDM00_SEL      :  1;  /* [0]     r/w */
            uint32_t REG_TEST_RESET      :  1;  /* [1]     r/w */
            uint32_t REG_TEST_DMPULLDOWN :  1;  /* [2]     r/w */
            uint32_t REG_TEST_DPPULLDOWN :  1;  /* [3]     r/w */
            uint32_t REG_TEST_OPMODE     :  1;  /* [4]     r/w */
            uint32_t REG_TEST_TERMSELECT :  1;  /* [5]     r/w */
            uint32_t REG_TEST_XCVRSELECT :  1;  /* [6]     r/w */
            uint32_t REG_TEST_LENGTH     : 10;  /* [16:7]  r/w */
            uint32_t RESERVED_31_17      : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USB_TEST_2;

};

typedef volatile struct sys_ctrl_reg sys_ctrl_reg_t;

#ifdef SYS_CTRL_IMPL
BEGIN_REG_SECTION(sys_ctrl_registers)
sys_ctrl_reg_t SYS_CTRLREG;
END_REG_SECTION(sys_ctrl_registers)
#else
extern sys_ctrl_reg_t SYS_CTRLREG;
#endif

#endif /* _SYS_CTRL_REG_H */
