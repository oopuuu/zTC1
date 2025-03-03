/****************************************************************************//**
 * @file     pmu_reg.h
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

#ifndef _PMU_REG_H
#define _PMU_REG_H

struct pmu_reg {
    /* 0x00: Power mode control register */
    union {
        struct {
            uint32_t PWR_MODE                    :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2               : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR_MODE;

    /* 0x04: BOOT_JTAG register */
    union {
        struct {
            uint32_t JTAG_EN                     :  1;  /* [0]     r/w */
            uint32_t BOOT_MODE_REG               :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2               : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } BOOT_JTAG;

    /* 0x08: Last reset cause  register */
    union {
        struct {
            uint32_t BROWNOUT_VBAT               :  1;  /* [0]     r/o */
            uint32_t BROWNOUT_V12                :  1;  /* [1]     r/o */
            uint32_t BROWNOUT_AV18               :  1;  /* [2]     r/o */
            uint32_t CM3_SYSRESETREQ             :  1;  /* [3]     r/o */
            uint32_t CM3_LOCKUP                  :  1;  /* [4]     r/o */
            uint32_t WDT_RST                     :  1;  /* [5]     r/o */
            uint32_t BROWNOUT_VFL                :  1;  /* [6]     r/o */
            uint32_t RESERVED_31_7               : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LAST_RST_CAUSE;

    /* 0x0c: Last reset cause clear register */
    union {
        struct {
            uint32_t BROWNOUT_VBAT_CLR           :  1;  /* [0]     r/w */
            uint32_t BROWNOUT_V12_CLR            :  1;  /* [1]     r/w */
            uint32_t BROWNOUT_AV18_CLR           :  1;  /* [2]     r/w */
            uint32_t CM3_SYSRESETREQ_CLR         :  1;  /* [3]     r/w */
            uint32_t CM3_LOCKUP_CLR              :  1;  /* [4]     r/w */
            uint32_t WDT_RST_CLR                 :  1;  /* [5]     r/w */
            uint32_t BROWNOUT_VFL_CLR            :  1;  /* [6]     r/w */
            uint32_t RESERVED_31_7               : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LAST_RST_CLR;

    /* 0x10: Wake up source clear register */
    union {
        struct {
            uint32_t CLR_PIN_INT0                :  1;  /* [0]     r/w */
            uint32_t CLR_PIN_INT1                :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2               : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } WAKE_SRC_CLR;

    uint8_t zReserved0x14[4];  /* pad 0x14 - 0x17 */

    /* 0x18: Clock source selection register */
    union {
        struct {
            uint32_t SYS_CLK_SEL                 :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2               : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_SRC;

    uint8_t zReserved0x1c[4];  /* pad 0x1c - 0x1f */
    
    /* 0x20: Pmip brndet interrupt selection register */
    union {
        struct {
            uint32_t PMIP_BRN_INT_SEL            :  1;  /* [0]    r/w */
            uint32_t RESERVED_31_1               : 31;  /* [31:1]  rsvd*/
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRN_INT_SEL;

    uint8_t zReserved0x24[4];  /* pad 0x24 - 0x27 */

    /* 0x28: Clock ready  register */
    union {
        struct {
            uint32_t PLL_CLK_RDY                 :  1;  /* [0]     r/o */
            uint32_t RESERVED_1                  :  1;  /* [1]     rsvd */
            uint32_t RC32M_RDY                   :  1;  /* [2]     r/o */
            uint32_t X32K_RDY                    :  1;  /* [3]     r/o */
            uint32_t RESERVED_5_4                :  2;  /* [5:4]  rsvd */
            uint32_t XTAL32M_CLK_RDY             :  1;  /* [6]     r/o */
            uint32_t RESERVED_31_7               : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_RDY;

    /* 0x2c: RC 32M control */
    union {
        struct {
            uint32_t CAL_IN_PROGRESS             :  1;  /* [0]     r/o */
            uint32_t CAL_ALLOW                   :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2               : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32M_CTRL;

    uint8_t zReserved0x30[4];  /* pad 0x30 - 0x33 */

    /* 0x34: SFLL control register 1 */
    union {
        struct {
            uint32_t RESERVED_0                  :  1;  /* [0]     rsvd */
            uint32_t SFLL_FBDIV                  : 11;  /* [11:1]  r/w */
            uint32_t SFLL_REFDIV                 :  9;  /* [20:12] r/w */
            uint32_t RESERVED_31_21              : 11;  /* [31:21] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SFLL_CTRL1;

    /* 0x38: Xtal32M clock request register */
    union {
        struct {
            uint32_t RESERVED_0                  :  1;  /* [0]     rsvd */
            uint32_t PU_XTAL                     :  1;  /* [1]     r/w */
            uint32_t PU                          :  1;  /* [2]     r/w */
            uint32_t SFLL_READY_DET_HIGH         : 11;  /* [13:3]  r/w */
            uint32_t SFLL_READY_DET_LOW          : 11;  /* [24:14] r/w */
            uint32_t RESERVED_31_25              :  7;  /* [31:25] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ANA_GRP_CTRL0;

    /* 0x3c: SFLL control register 2 */
    union {
        struct {
            uint32_t SFLL_PU                     :  1;  /* [0]     r/w */
            uint32_t RESERVED_12_1               : 12;  /* [12:1]  rsvd */
            uint32_t SFLL_DIV_SEL                :  2;  /* [14:13] r/w */
            uint32_t RESERVED_19_15              :  5;  /* [19:15] rsvd */
            uint32_t SFLL_KVCO                   :  3;  /* [22:20] r/w */
            uint32_t RESERVED_24_23              :  2;  /* [24:23] rsvd */
            uint32_t SFLL_REFCLK_SEL             :  1;  /* [25]    r/w */
            uint32_t SFLL_LOCK                   :  1;  /* [26]    r/o */
            uint32_t RESERVED_31_27              :  5;  /* [31:27] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SFLL_CTRL0;

    uint8_t zReserved0x40[4];  /* pad 0x40 - 0x43 */

    /* 0x44: Power status register */
    union {
        struct {
            uint32_t VDD_VFL_RDY                 :  1;  /* [0]     r/o */
            uint32_t VDD_MEM_RDY                 :  2;  /* [2:1]   r/o */
            uint32_t VDD_CAU_RDY                 :  2;  /* [4:3]   r/o */
            uint32_t VDD_MCU_RDY                 :  2;  /* [6:5]   r/o */
            uint32_t AV18_RDY                    :  1;  /* [7]     r/o */
            uint32_t RESERVED_31_8               : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR_STAT;

    /* 0x48: Pad Ctrl0 register */
    union {
        struct {
            uint32_t RESERVED_0                  :  1;   /* [0]    rsvd */
            uint32_t XTAL32K_IN_CTRL             :  1;   /* [1]    r/w */
            uint32_t XTAL32K_OUT_CTRL            :  1;   /* [2]    r/w */
            uint32_t TDO_CTRL                    :  1;   /* [3]    r/w */
            uint32_t RESERVED_31_4               : 28;   /* [31:4] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PAD_CTRL0_REG;
    
    /* 0x4C: Pad Ctrl1 register */
    union {
        struct {
            uint32_t GPIO_27_CTRL                :  1;   /* [0]    r/w */
            uint32_t RESERVED_1                  :  1;   /* [1]    rsvd */
            uint32_t WAKEUP0_CTRL                :  1;   /* [2]    r/w */
            uint32_t WAKEUP1_CTRL                :  1;   /* [3]    r/w */
            uint32_t RESERVED_31_4               : 28;   /* [31:4] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PAD_CTRL1_REG;
          
    uint8_t zReserved0x50[4];  /* pad 0x50 - 0x53 */


    /* 0x54: Brownout config register */
    union {
        struct {
            uint32_t BRNDET_V12_RST_EN           :  1;  /* [0]     r/w */
            uint32_t BRNDET_VBAT_RST_EN          :  1;  /* [1]     r/w */
            uint32_t BRNDET_VFL_RST_EN           :  1;  /* [2]     r/w */
            uint32_t BRNDET_AV18_RST_EN          :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4               : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRN_CFG;
    
    /* 0x58: PMU MISC register */
    union {
        struct {
            uint32_t RESERVED_0                  :  1;  /* [0]     rsvd */
            uint32_t CAU_CLOCK_GATE              :  1;  /* [1]     r/w */
            uint32_t AUPLL_LOCK_STATUS           :  1;  /* [2]     r/o */
            uint32_t RESERVED_OUT                : 29;  /* [31:3]  r/w */
        } BF;
        uint32_t WORDVAL;
    } RSVD;

    /* 0x5c: BG control register */
    union {
        struct {
            uint32_t RESERVED_9_0                : 10;  /* [9:0]   rsvd */
            uint32_t BYPASS                      :  1;  /* [10]    r/w */
            uint32_t RESERVED_31_11              : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ANA_GRP_CTRL1;

    /* 0x60: Power Configure register */
    union {
        struct {
            uint32_t RESERVED_1_0                :  2;  /* [1:0]   rsvd */
            uint32_t AV18_EXT                    :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3               : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_PWR_CONFIG;

    /* 0x64: Chargepump control register0 */
    union {
        struct {
            uint32_t RESERVED_13_0               : 14;  /* [13:0]  rsvd */
            uint32_t DEL_V12_SEL                 :  2;  /* [15:14] r/w */
            uint32_t RESERVED_31_16              : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_CHP_CTRL0;

    /* 0x68: Chargepump control register1 */
    union {
        struct {
            uint32_t RESERVED_3_0                :  4;  /* [3:0]   rsvd */
            uint32_t CHP_SPREADSP                :  2;  /* [5:4]   r/w */
            uint32_t RESERVED_31_6               : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_CHP_CTRL1;

    uint8_t zReserved0x6c[12];  /* pad 0x6c - 0x77 */

    /* 0x78: USB and audio pll control register */
    union {
        struct {
            uint32_t FBDIV                       :  9;  /* [8:0]   r/w */
            uint32_t REFDIV                      :  5;  /* [13:9]  r/w */
            uint32_t PU                          :  1;  /* [14]    r/w */
            uint32_t RESERVED_31_15              : 17;  /* [31:15] rsvd */
        } BF;
        uint32_t WORDVAL;
    } AUPLL_CTRL0;

    /* 0x7c: Peripheral clock gating register */
    union {
        struct {
            uint32_t RESERVED_0                  :  1;  /* [0]     rsvd */
            uint32_t QSPI0_CLK_EN                :  1;  /* [1]     r/w */
            uint32_t RESERVED_2                  :  1;  /* [2]     rsvd */
            uint32_t RTC_CLK_EN                  :  1;  /* [3]     r/w */
            uint32_t GPIO_CLK_EN                 :  1;  /* [4]     r/w */
            uint32_t UART0_CLK_EN                :  1;  /* [5]     r/w */
            uint32_t UART1_CLK_EN                :  1;  /* [6]     r/w */
            uint32_t I2C0_CLK_EN                 :  1;  /* [7]     r/w */
            uint32_t SSP0_CLK_EN                 :  1;  /* [8]     r/w */
            uint32_t SSP1_CLK_EN                 :  1;  /* [9]     r/w */
            uint32_t GPT0_CLK_EN                 :  1;  /* [10]    r/w */
            uint32_t GPT1_CLK_EN                 :  1;  /* [11]    r/w */
            uint32_t RESERVED_14_12              :  3;  /* [14:12] rsvd */
            uint32_t UART2_CLK_EN                :  1;  /* [15]    r/w */
            uint32_t UART3_CLK_EN                :  1;  /* [16]    r/w */
            uint32_t SSP2_CLK_EN                 :  1;  /* [17]    r/w */
            uint32_t RESERVED_18                 :  1;  /* [18]    rsvd */
            uint32_t I2C1_CLK_EN                 :  1;  /* [19]    r/w */
            uint32_t I2C2_CLK_EN                 :  1;  /* [20]    r/w */
            uint32_t GPT2_CLK_EN                 :  1;  /* [21]    r/w */
            uint32_t GPT3_CLK_EN                 :  1;  /* [22]    r/w */
            uint32_t WDT_CLK_EN                  :  1;  /* [23]    r/w */
            uint32_t QSPI1_CLK_EN                :  1;  /* [24]    r/w */
            uint32_t SDIO_CLK_EN                 :  1;  /* [25]    r/w */
            uint32_t RESERVED_26                 :  1;  /* [26]    rsvd */
            uint32_t USBC_CLK_EN                 :  1;  /* [27]    r/w */
            uint32_t FLASH_QSPI_CLK_EN           :  1;  /* [28]    r/w */
            uint32_t RESERVED_31_29              :  3;  /* [31:29] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI_CLK_EN;

    /* 0x80: UART fast clock div register */
    union {
        struct {
            uint32_t DENOMINATOR                 : 11;  /* [10:0]  r/w */
            uint32_t NOMINATOR                   : 13;  /* [23:11] r/w */
            uint32_t RESERVED_31_24              :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } UART_FAST_CLK_DIV;

    /* 0x84: UART slow clock div register */
    union {
        struct {
            uint32_t DENOMINATOR                 : 11;  /* [10:0]  r/w */
            uint32_t NOMINATOR                   : 13;  /* [23:11] r/w */
            uint32_t RESERVED_31_24              :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } UART_SLOW_CLK_DIV;

    /* 0x88: UART clock select register */
    union {
        struct {
            uint32_t UART0_CLK_SEL               :  1;  /* [0]     r/w */
            uint32_t UART1_CLK_SEL               :  1;  /* [1]     r/w */
            uint32_t UART2_CLK_SEL               :  1;  /* [2]     r/w */
            uint32_t UART3_CLK_SEL               :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4               : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } UART_CLK_SEL;

    /* 0x8c: MCU CORE clock divider ratio register */
    union {
        struct {
            uint32_t FCLK_DIV                    :  6;  /* [5:0]   r/w */
            uint32_t RESERVED_31_6               : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MCU_CORE_CLK_DIV;

    /* 0x90: Peripheral0 clock divider ratio register */
    union {
        struct {
            uint32_t SSP0_CLK_DIV                :  5;  /* [4:0]   r/w */
            uint32_t SSP1_CLK_DIV                :  5;  /* [9:5]   r/w */
            uint32_t SSP2_CLK_DIV                :  5;  /* [14:10] r/w */
            uint32_t RESERVED_15                 :  1;  /* [15]    rsvd */
            uint32_t SDIO_CLK_DIV                :  4;  /* [19:16] r/w */
            uint32_t RESERVED_31_20              : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI0_CLK_DIV;

    /* 0x94: Peripheral1 clock divider ratio register */
    union {
        struct {
            uint32_t PMU_CLK_DIV                 :  4;  /* [3:0]   r/w */
            uint32_t FLASH_CLK_DIV               :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_7                  :  1;  /* [7]     rsvd */
            uint32_t QSPI0_CLK_DIV               :  3;  /* [10:8]  r/w */
            uint32_t RESERVED_11                 :  1;  /* [11]    rsvd */
            uint32_t QSPI1_CLK_DIV               :  3;  /* [14:12] r/w */
            uint32_t RESERVED_15                 :  1;  /* [15]    rsvd */
            uint32_t APB0_CLK_TO_AHB_CLK_RATIO   :  2;  /* [17:16] r/w */
            uint32_t APB1_CLK_TO_AHB_CLK_RATIO   :  2;  /* [19:18] r/w */
            uint32_t RESERVED_31_20              : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI1_CLK_DIV;

    /* 0x98: Peripheral2 clock divider ratio register */
    union {
        struct {
            uint32_t GPT_SAMPLE_CLK_DIV          :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_3                  :  1;  /* [3]     rsvd */
            uint32_t WDT_CLK_DIV_5_3             :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_7                  :  1;  /* [7]     rsvd */
            uint32_t GPT3_CLK_DIV_2_0            :  3;  /* [10:8]  r/w */
            uint32_t RESERVED_11                 :  1;  /* [11]    rsvd */
            uint32_t GPT3_CLK_DIV_5_3            :  3;  /* [14:12] r/w */
            uint32_t RESERVED_19_15              :  5;  /* [19:15] rsvd */
            uint32_t I2C_CLK_DIV                 :  2;  /* [21:20] r/w */
            uint32_t RESERVED_23_22              :  2;  /* [23:22] rsvd */
            uint32_t WDT_CLK_DIV_1_0             :  2;  /* [25:24] r/w */
            uint32_t RESERVED_27_26              :  2;  /* [27:26] rsvd */
            uint32_t WDT_CLK_DIV_2_2             :  1;  /* [28]    r/w */
            uint32_t RESERVED_31_29              :  3;  /* [31:29] rsvd */         
        } BF;
        uint32_t WORDVAL;
    } PERI2_CLK_DIV;

    /* 0x9c: select signal for cau mclk */
    union {
        struct {
            uint32_t CAU_CLK_SEL                 :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2               : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CAU_CLK_SEL;

    /* 0xa0: Wakeup pull up pull down ctrl resigter */
    union {
        struct {
            uint32_t RESERVED_0                  :  1;   /* [0]    rsvd */
            uint32_t WAKEUP0_PUPD_CTRL           :  1;   /* [1]    r/w */
            uint32_t WAKEUP1_PUPD_CTRL           :  1;   /* [2]    r/w */
            uint32_t RESERVED_31_3               : 29;   /* [31:3] rsvd */
        } BF;
        uint32_t WORDVAL;
    } WAKEUP_PUPD_CTRL;

    /* 0xa4: Power Config Resigter */
    union {
        struct {
            uint32_t VDD_IO9_REG_PDB_CORE        :  1;  /* [0]     r/w */
            uint32_t RESERVED_1                  :  1;  /* [1]     rsvd */
            uint32_t V18EN_LVL_GPIO2_V18EN_CORE  :  1;  /* [2]     r/w */
            uint32_t POR_LVL_GPIO2_LOW_VDDB_CORE :  1;  /* [3]     r/w */
            uint32_t VDD_IO7_REG_PDB_CORE        :  1;  /* [4]     r/w */
            uint32_t RESERVED_5                  :  1;  /* [5]     rsvd */
            uint32_t V18EN_LVL_SDIO_V18EN_CORE   :  1;  /* [6]     r/w */
            uint32_t POR_LVL_SDIO_LOW_VDDB_CORE  :  1;  /* [7]     r/w */
            uint32_t VDD_IO6_REG_PDB_CORE        :  1;  /* [8]     r/w */
            uint32_t RESERVED_9                  :  1;  /* [9]     rsvd */
            uint32_t VDD_IO4_REG_PDB_CORE        :  1;  /* [10]    r/w */
            uint32_t RESERVED_11                 :  1;  /* [11]    rsvd */
            uint32_t V18EN_LVL_GPIO1_V18EN_CORE  :  1;  /* [12]    r/w */
            uint32_t POR_LVL_GPIO1_LOW_VDDB_CORE :  1;  /* [13]    r/w */
            uint32_t VDD_IO2_REG_PDB_CORE        :  1;  /* [14]    r/w */
            uint32_t RESERVED_15                 :  1;  /* [15]    rsvd */
            uint32_t V18EN_LVL_AON_V18EN_CORE    :  1;  /* [16]    r/w */
            uint32_t POR_LVL_AON_LOW_VDDB_CORE   :  1;  /* [17]    r/w */
            uint32_t VDD_IO1_REG_PDB_CORE        :  1;  /* [18]    r/w */
            uint32_t RESERVED_19                 :  1;  /* [19]    rsvd */
            uint32_t V18EN_LVL_GPIO0_V18EN_CORE  :  1;  /* [20]    r/w */
            uint32_t POR_LVL_GPIO0_LOW_VDDB_CORE :  1;  /* [21]    r/w */
            uint32_t VDDO_FL_REG_PDB_CORE        :  1;  /* [22]    r/w */
            uint32_t RESERVED_24_23              :  2;  /* [24:23] rsvd */
            uint32_t POR_LVL_FL_LOW_VDDB_CORE    :  1;  /* [25]    r/w */
            uint32_t RESERVED_31_26              :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } IO_PAD_PWR_CFG;

    /* 0xa8: extra interrupt select register0 */
    union {
        struct {
            uint32_t SEL_34                      :  2;  /* [1:0]   r/w */
            uint32_t SEL_35                      :  2;  /* [3:2]   r/w */
            uint32_t SEL_36                      :  2;  /* [5:4]   r/w */
            uint32_t SEL_37                      :  2;  /* [7:6]   r/w */
            uint32_t SEL_38                      :  2;  /* [9:8]   r/w */
            uint32_t SEL_39                      :  2;  /* [11:10] r/w */
            uint32_t SEL_40                      :  2;  /* [13:12] r/w */
            uint32_t SEL_41                      :  2;  /* [15:14] r/w */
            uint32_t SEL_42                      :  2;  /* [17:16] r/w */
            uint32_t SEL_43                      :  2;  /* [19:18] r/w */
            uint32_t SEL_44                      :  2;  /* [21:20] r/w */
            uint32_t SEL_45                      :  2;  /* [23:22] r/w */
            uint32_t SEL_46                      :  2;  /* [25:24] r/w */
            uint32_t SEL_47                      :  2;  /* [27:26] r/w */
            uint32_t SEL_48                      :  2;  /* [29:28] r/w */
            uint32_t SEL_49                      :  2;  /* [31:30] r/w */
        } BF;
        uint32_t WORDVAL;
    } EXT_SEL_REG0;

    /* 0xac: extra interrupt select register1 */
    union {
        struct {
            uint32_t SEL_50                      :  2;  /* [1:0]   r/w */
            uint32_t SEL_51                      :  2;  /* [3:2]   r/w */
            uint32_t SEL_52                      :  2;  /* [5:4]   r/w */
            uint32_t SEL_53                      :  2;  /* [7:6]   r/w */
            uint32_t SEL_54                      :  2;  /* [9:8]   r/w */
            uint32_t SEL_55                      :  2;  /* [11:10] r/w */
            uint32_t SEL_56                      :  2;  /* [13:12] r/w */
            uint32_t SEL_57                      :  2;  /* [15:14] r/w */
            uint32_t SEL_58                      :  2;  /* [17:16] r/w */
            uint32_t SEL_59                      :  2;  /* [19:18] r/w */
            uint32_t RESERVED_31_20              : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } EXT_SEL_REG1;

    /* 0xb0: USB and audio pll control register */
    union {
        struct {
            uint32_t FREQ_OFFSET                 : 17;  /* [16:0]  r/w */
            uint32_t RESET_INTP_EXT              :  1;  /* [17]    r/w */
            uint32_t CLK_DET_EN                  :  1;  /* [18]    r/w */
            uint32_t RESERVED_22_19              :  4;  /* [22:19] rsvd */
            uint32_t UPDATE_SEL                  :  1;  /* [23]    r/w */
            uint32_t RESERVED_28_24              :  5;  /* [28:24] rsvd */
            uint32_t PI_EN                       :  1;  /* [29]    r/w */
            uint32_t RESERVED_31_30              :  2;  /* [31:30] rsvd */
        } BF;
        uint32_t WORDVAL;
    } AUPLL_CTRL1;

    /* 0xb4: USB and audio pll control register */
    union {
        struct {
            uint32_t RESERVED_13_0               : 14;  /* [13:0]  rsvd */
            uint32_t POSTDIV_AUDIO               :  7;  /* [20:14] r/w */
            uint32_t POSTDIV_AUDIO_EN            :  1;  /* [21]    r/w */
            uint32_t POSTDIV_USB_EN              :  1;  /* [22]    r/w */
            uint32_t POSTDIV_USB                 :  1;  /* [23]    r/w */
            uint32_t RESET_OFFSET_EXT            :  1;  /* [24]    r/w */
            uint32_t FREQ_OFFSET_VALID           :  1;  /* [25]    r/w */
            uint32_t CLKOUT_30M_EN               :  1;  /* [26]    r/w */
            uint32_t RESERVED_31_27              :  5;  /* [31:27] rsvd */
        } BF;
        uint32_t WORDVAL;
    } AUPLL_CTRL2;

    /* 0xb8: CAU Clock Control Register */
    union {
        struct {
            uint32_t CAU_ACOMP_MCLK_EN           :  1;  /* [0]     r/w */
            uint32_t CAU_GPDAC_MCLK_EN           :  1;  /* [1]     r/w */
            uint32_t CAU_GPADC1_MCLK_EN          :  1;  /* [2]     r/w */
            uint32_t CAU_GPADC0_MCLK_EN          :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4               : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CAU_CTRL;

    /* 0xbc: RC32K Control Register */
    union {
        struct {
            uint32_t RC32K_CAL_DONE              :  1;  /* [0]     r/o */
            uint32_t RC32K_RDY                   :  1;  /* [1]     r/o */
            uint32_t RESERVED_2                  :  1;  /* [2]     rsvd */
            uint32_t RC32K_CAL_INPROGRESS        :  1;  /* [3]     r/o */
            uint32_t RC32K_CODE_FR_CAL           :  7;  /* [10:4]  r/o */
            uint32_t RC32K_ALLOW_CAL             :  1;  /* [11]    r/w */
            uint32_t RC32K_CAL_DIV               :  2;  /* [13:12] r/w */
            uint32_t RESERVED_14                 :  1;  /* [14]    rsvd */
            uint32_t RC32K_EXT_CODE_EN           :  1;  /* [15]    r/w */
            uint32_t RC32K_CODE_FR_EXT           :  7;  /* [22:16] r/w */
            uint32_t RC32K_CAL_EN                :  1;  /* [23]    r/w */
            uint32_t RC32K_PD                    :  1;  /* [24]    r/w */
            uint32_t RESERVED_31_25              :  7;  /* [31:25] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32K_CTRL;

    /* 0xc0: XTAL32K Control Register */
    union {
        struct {
            uint32_t X32K_RDY                    :  1;  /* [0]     r/o */
            uint32_t RESERVED_10_1               : 10;  /* [10:1]  rsvd */
            uint32_t X32K_EXT_OSC_EN             :  1;  /* [11]    r/w */
            uint32_t X32K_EN                     :  1;  /* [12]    r/w */
            uint32_t RESERVED_31_13              : 19;  /* [31:13] rsvd */
        } BF;
        uint32_t WORDVAL;
    } XTAL32K_CTRL;

    /* 0xc4: Comparator control register */
    union {
        struct {
            uint32_t COMP_OUT                    :  1;  /* [0]     r/o */
            uint32_t COMP_RDY                    :  1;  /* [1]     r/o */
            uint32_t COMP_REF_SEL                :  3;  /* [4:2]   r/w */
            uint32_t COMP_DIFF_EN                :  1;  /* [5]     r/w */
            uint32_t COMP_EN                     :  1;  /* [6]     r/w */
            uint32_t COMP_HYST                   :  2;  /* [8:7]   r/w */
            uint32_t CAU_REF_EN                  :  1;  /* [9]     r/w */
            uint32_t RESERVED_31_10              : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_CMP_CTRL;

    /* 0xc8: AV18 brownout detector control register */
    union {
        struct {
            uint32_t RESERVED_1_0                :  2;  /* [1:0]   r/w */
            uint32_t LDO_AV18_RAMP_RATE          :  2;  /* [3:2]   r/w */
            uint32_t RESERVED_7_2                :  4;  /* [7:4]   rsvd */
            uint32_t BRNDET_AV18_OUT             :  1;  /* [8]     r/o */
            uint32_t BRNDET_AV18_RDY             :  1;  /* [9]     r/o */
            uint32_t BRNDET_AV18_FILT            :  2;  /* [11:10] r/w */
            uint32_t BRNHYST_AV18_CNTL           :  2;  /* [13:12] r/w */
            uint32_t BRNTRIG_AV18_CNTL           :  3;  /* [16:14] r/w */
            uint32_t BRNDET_AV18_EN              :  1;  /* [17]    r/w */
            uint32_t DEL_AV18_SEL                :  2;  /* [19:18] r/w */
            uint32_t RESERVED_31_20              : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRNDET_AV18;

    /* 0xcc: VFL brownout detector control register */
    union {
        struct {
            uint32_t RESERVED_1_0                :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_7_2                :  6;  /* [7:2]   rsvd */
            uint32_t BRNDET_VFL_OUT              :  1;  /* [8]     r/o */
            uint32_t BRNDET_VFL_RDY              :  1;  /* [9]     r/o */
            uint32_t BRNDET_VFL_FILT             :  2;  /* [11:10] r/w */
            uint32_t BRNHYST_VFL_CNTL            :  2;  /* [13:12] r/w */
            uint32_t BRNTRIG_VFL_CNTL            :  3;  /* [16:14] r/w */
            uint32_t BRNDET_VFL_EN               :  1;  /* [17]    r/w */
            uint32_t DEL_VFL_SEL                 :  2;  /* [19:18] r/w */
            uint32_t RESERVED_31_20              : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRNDET_VFL;

    /* 0xd0: Vbat brownout detector control register */
    union {
        struct {
            uint32_t RESERVED_9_0                : 10;  /* [9:0]   rsvd */
            uint32_t BRNDET_VBAT_OUT             :  1;  /* [10]    r/o */
            uint32_t BRNDET_VBAT_RDY             :  1;  /* [11]    r/o */
            uint32_t BRNDET_VBAT_FILT            :  2;  /* [13:12] r/w */
            uint32_t BRNHYST_VBAT_CNTL           :  2;  /* [15:14] r/w */
            uint32_t BRNTRIG_VBAT_CNTL           :  3;  /* [18:16] r/w */
            uint32_t BRNDET_VBAT_EN              :  1;  /* [19]    r/w */
            uint32_t RESERVED_31_20              : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRNDET_VBAT;

    /* 0xd4: V12 brownout detector control register */
    union {
        struct {
            uint32_t LDO_V12_RAMP_RATE           :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_4_2                :  3;  /* [4:2]   rsvd */
            uint32_t BRNDET_V12_OUT              :  1;  /* [5]     r/o */
            uint32_t BRNDET_V12_RDY              :  1;  /* [6]     r/o */
            uint32_t BRNDET_V12_FILT             :  2;  /* [8:7]   r/w */
            uint32_t BRNHYST_V12_CNTL            :  2;  /* [10:9]  r/w */
            uint32_t BRNTRIG_V12_CNTL            :  3;  /* [13:11] r/w */
            uint32_t BRNDET_V12_EN               :  1;  /* [14]    r/w */
            uint32_t RESERVED_31_15              : 17;  /* [31:15] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRNDET_V12;

    /* 0xd8: LDO control register */
    union {
        struct {
            uint32_t RESERVED_4_0                :  5;  /* [4:0]   rsvd */
            uint32_t LDO_V12_EN                  :  1;  /* [5]     r/w */
            uint32_t LDO_AV18_PWRSW_EN           :  1;  /* [6]     r/w */
            uint32_t RESERVED_10_7               :  4;  /* [10:7]  rsvd */
            uint32_t LDO_AV18_EN                 :  1;  /* [11]    r/w */
            uint32_t RESERVED_31_12              : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_LDO_CTRL;

    /* 0xdc: Peripheral Clock Source Register */
    union {
        struct {
            uint32_t SSP0_AUDIO_SEL              :  1;  /* [0]     r/w */
            uint32_t SSP1_AUDIO_SEL              :  1;  /* [1]     r/w */
            uint32_t SSP2_AUDIO_SEL              :  1;  /* [2]     r/w */
            uint32_t RTC_INT_SEL                 :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4               : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI_CLK_SRC;

    uint8_t zReserved0xe0[4];  /* pad 0xe0 - 0xe3 */

    /* 0xe4: GPT0 Clock Control Register */
    union {
        struct {
            uint32_t GPT0_CLK_DIV                :  6;  /* [5:0]   r/w */
            uint32_t RESERVED_6                  :  1;  /* [6]     rsvd */
            uint32_t GPT0_CLK_SEL1               :  2;  /* [8:7]   r/w */
            uint32_t GPT0_CLK_SEL0               :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11              : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPT0_CTRL;

    /* 0xe8: GPT1 Clock Control Register */
    union {
        struct {
            uint32_t GPT1_CLK_DIV                :  6;  /* [5:0]   r/w */
            uint32_t RESERVED_6                  :  1;  /* [6]     rsvd */
            uint32_t GPT1_CLK_SEL1               :  2;  /* [8:7]   r/w */
            uint32_t GPT1_CLK_SEL0               :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11              : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPT1_CTRL;

    /* 0xec: GPT2 Clock Control Register */
    union {
        struct {
            uint32_t GPT2_CLK_DIV                :  6;  /* [5:0]   r/w */
            uint32_t RESERVED_6                  :  1;  /* [6]     rsvd */
            uint32_t GPT2_CLK_SEL1               :  2;  /* [8:7]   r/w */
            uint32_t GPT2_CLK_SEL0               :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11              : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPT2_CTRL;

    /* 0xf0: GPT3 Clock Control Register */
    union {
        struct {
            uint32_t RESERVED_6_0                :  7;  /* [6:0]   rsvd */
            uint32_t GPT3_CLK_SEL1               :  2;  /* [8:7]   r/w */
            uint32_t GPT3_CLK_SEL0               :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11              : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPT3_CTRL;
    
    /* 0xf4: wakeup edge detect register */
    union {
        struct {
            uint32_t WAKEUP0                     :  1;  /* [0]     r/w */
            uint32_t WAKEUP1                     :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2               : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } WAKEUP_EDGE_DETECT;

};

typedef volatile struct pmu_reg pmu_reg_t;

#ifdef PMU_IMPL
BEGIN_REG_SECTION(pmu_registers)
pmu_reg_t PMUREG;
END_REG_SECTION(pmu_registers)
#else
extern pmu_reg_t PMUREG;
#endif

#endif /* _PMU_REG_H */

