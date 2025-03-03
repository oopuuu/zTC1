/****************************************************************************//**
 * @file     pinmux_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.0.0
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

#ifndef _PINMUX_REG_H                                         
#define _PINMUX_REG_H                                         
                                                               
struct pinmux_reg {                                           
    /* 0x000-0x13c: GPIO_PINMUX Configuration register */
    union {                                                    
        struct {                                               
            uint32_t FSEL_XR          :  3;  /* [2:0]   r/w */ 
            uint32_t DI_EN            :  1;  /* [3]     r/w */
            uint32_t RESERVED_12_4    :  9;  /* [12:4]  rsvd */
            uint32_t PIO_PULLDN_R     :  1;  /* [13]    r/w */ 
            uint32_t PIO_PULLUP_R     :  1;  /* [14]    r/w */ 
            uint32_t PIO_PULL_SEL_R   :  1;  /* [15]    r/w */ 
            uint32_t RESERVED_31_16   : 16;  /* [31:16] rsvd */ 
        } BF;                                                  
        uint32_t WORDVAL;                                      
    } GPIO_PINMUX[80];   
                                           
};                                                             
                                                               
typedef volatile struct pinmux_reg pinmux_reg_t;             

#ifdef PINMUX_IMPL
BEGIN_REG_SECTION(pinmux_registers)
pinmux_reg_t PINMUXREG;
END_REG_SECTION(pinmux_registers)
#else
extern pinmux_reg_t PINMUXREG;
#endif

#endif /* _PINMUX_REG_H */

