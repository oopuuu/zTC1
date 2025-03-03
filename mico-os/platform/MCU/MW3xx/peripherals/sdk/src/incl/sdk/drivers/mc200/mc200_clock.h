/****************************************************************************//**
 * @file     mc200_clock.h
 * @brief    CLOCK driver module header file.
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

#ifndef __MC200_CLOCK_H__
#define __MC200_CLOCK_H__

#include "mc200_driver.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @addtogroup  Clock 
 *  @{
 */
  
/** @defgroup Clock_Public_Types Clock_Public_Types
 *  @{
 */


/** 
 *  @brief clock source option 
 */
typedef enum
{
  CLK_MAINXTAL = 0,                         /*!< XT32M Clock select */
  CLK_RC32M    = 1,                         /*!< RC32M Clock select */
  CLK_XTAL32K  = 2,                         /*!< XT32K Clock select */
  CLK_RC32K    = 3,                         /*!< RC32K Clock select */
  CLK_SFLL     = 4,                         /*!< SFLL Clock select */
  CLK_AUPLL    = 5,                         /*!< AUPLL Clock select */
  CLK_SYSTEM   = 6,                         /*!< System Clock select */
}CLK_Src_Type;

/** 
 *  @brief uart clock source option 
 */
typedef enum
{
  CLK_UART_FAST = 0,                         /*!< UART Fast Clock select */
  CLK_UART_SLOW = 1,                         /*!< UART Slow Clock select */
}CLK_UartSrc_Type;

/** 
 *  @brief crystal mode option 
 */
typedef enum
{
  CLK_OSC_INTERN = 0,                         /*!< Internal oscillator mode */
  CLK_OSC_EXTERN = 1,                         /*!< External oscillator mode */
}CLK_CrystalMode_Type;

/** 
 *  @brief aupll clock out type option 
 */
typedef enum
{
  CLK_AUPLL_USB   = 0,                      /*!< AUPLL USB clock out select */
  CLK_AUPLL_CAU   = 1,                      /*!< AUPLL CAU clock out select */
  CLK_AUPLL_AUDIO = 2,                      /*!< AUPLL audio clock out select */
}CLK_AupllClkOut_Type;

/** 
 *  @brief gpt id option 
 */
typedef enum
{
  CLK_GPT_ID_0   = 0,                       /*!< GPT0 */
  CLK_GPT_ID_1   = 1,                       /*!< GPT1 */
  CLK_GPT_ID_2   = 2,                       /*!< GPT2 */
  CLK_GPT_ID_3   = 3,                       /*!< GPT3 */
}CLK_GptID_Type;

/** 
 *  @brief uart id option 
 */
typedef enum
{
  CLK_UART_ID_0   = 0,                      /*!< UART0 */
  CLK_UART_ID_1   = 1,                      /*!< UART1 */
  CLK_UART_ID_2   = 2,                      /*!< UART2 */
  CLK_UART_ID_3   = 3,                      /*!< UART3 */
}CLK_UartID_Type;


/** 
 *  @brief ssp id option 
 */
typedef enum
{
  CLK_SSP_ID_0   = 0,                       /*!< SSP0 */
  CLK_SSP_ID_1   = 1,                       /*!< SSP1 */
  CLK_SSP_ID_2   = 2,                       /*!< SSP2 */
}CLK_SspID_Type;

/** 
 *  @brief clock module option 
 */
typedef enum
{
  CLK_QSPI0    = 0,                         /*!< QSPI0 Clock */
  CLK_RTC,                                  /*!< RTC Clock */
  CLK_GPIO,                                 /*!< GPIO Clock */
  CLK_UART0,                                /*!< UART0 Clock */
  CLK_UART1,                                /*!< UART1 Clock */
  CLK_I2C0     = 5,                         /*!< I2C0 Clock */
  CLK_SSP0,                                 /*!< SSP0 Clock */
  CLK_SSP1,                                 /*!< SSP1 Clock */
  CLK_GPT0,                                 /*!< GPT0 Clock */
  CLK_GPT1,                                 /*!< GPT1 Clock */
  CLK_UART2    = 10,                        /*!< UART2 Clock */
  CLK_UART3,                                /*!< UART3 Clock */
  CLK_SSP2,                                 /*!< SSP2 Clock */
  CLK_I2C1,                                 /*!< I2C1 Clock */
  CLK_I2C2,                                 /*!< I2C2 Clock */
  CLK_GPT2     = 15,                        /*!< GPT2 Clock */
  CLK_GPT3,                                 /*!< GPT3 Clock */
  CLK_WDT,                                  /*!< WDT Clock */
  CLK_QSPI1,                                /*!< QSPI1 Clock */
  CLK_SDIO,                                 /*!< SDIO Clock */
  CLK_DMAC    = 20,                         /*!< DMA Controller Clock */
  CLK_USBC,                                 /*!< USB Controller  Clock */
  CLK_FLASHC,                               /*!< FLASH Controller Clock */
  CLK_ACOMP,                                /*!< ACOMP Clock */
  CLK_DAC,                                  /*!< DAC Clock */
  CLK_ADC0    = 25,                         /*!< ADC1 Clock */
  CLK_ADC1,                                 /*!< ADC0 Clock */
  
  CLK_CORE,                                 /*!< HCLK/FCLK/AHB Clock */
  CLK_APB0,                                 /*!< APB0 Clock */
  CLK_APB1,                                 /*!< APB1 Clock */
  CLK_PMU     = 30,                         /*!< PMU Clock */
  
  CLK_GPT_SAMPLE,                           /*!< GPT Sample Clock */
}CLK_Module_Type;

/** 
 *  @brief calibration option(manual or Auto) 
 */
typedef enum
{
  CLK_MANUAL_CAL  = 0,                      /*!< RC32K/RC32M Manual Calibration option */
  CLK_AUTO_CAL    = 1,                      /*!< RC32K/RC32M Auto Calibration option */
}CLK_RCCal_Type;

/**  
 *  @brief clock interrupt type definition 
 */
typedef enum
{
  RC32M_CALDON        = 0,                  /*!< RC32M calibration done interrupt */
  RC32M_CKRDY         = 1,                  /*!< RC32M clock out ready interrupt */
}RC32M_INT_Type; 

/**  
 *  @brief RC32M output frequency definition 
 */
typedef enum
{
  RC32M_SELFREQ_16M        = 0,                  /*!< RC32M output frequency 16 MHz */
  RC32M_SELFREQ_32M        = 1,                  /*!< RC32M output frequency 32 MHz */
}RC32M_SELFREQ_Type; 

/**  
 *  @brief clock fraction structure definition 
 */
typedef struct
{
  uint32_t clkDividend;                     /*!< RC32M calibration done interrupt */
  uint32_t clkDivisor;                      /*!< RC32M clock out ready interrupt */
}CLK_Fraction_Type; 


/** 
 *  @brief SFLL configure structure definition 
 */
typedef struct
{
  CLK_Src_Type     refClockSrc;             /*!< only accept CLK_RC32M (default) and CLK_MAINXTAL */  
  
  uint32_t         refDiv;                  /*!< Divider for reference clock, 8-bit, range 2~255 */
                                            /*!< 0.2 MHz <= Fref (frequency of Reference clock / refDiv) <= 0.4 MHz */
  
  uint32_t         fbDiv;                   /*!< Divider for feedback clock, 11-bit */  
  
  uint32_t         kvco;                    /*!< VCO setting, 3-bit */
                                            /*!< Select KVCO per VCO target frequency */
                                            /*!< KVCO = 0, if 100 MHz < Fvco <= 110 MHz */
                                            /*!< KVCO = 1, if 110 MHz < Fvco <= 121 MHz */
                                            /*!< KVCO = 2, if 121 MHz < Fvco <= 133 MHz */
                                            /*!< KVCO = 3, if 133 MHz < Fvco <= 146 MHz */
                                            /*!< KVCO = 4, if 146 MHz < Fvco <= 160 MHz */
                                            /*!< KVCO = 5, if 160 MHz < Fvco <= 175 MHz */
                                            /*!< KVCO = 6, if 175 MHz < Fvco <= 192 MHz */
                                            /*!< KVCO = 7, if 192 MHz < Fvco <= 200 MHz */
  
  uint32_t         postDiv;                 /*!< Post divider, 2-bit */
                                            /*!< 2'b00, Fout = Fvco/1 */
                                            /*!< 2'b01, Fout = Fvco/2 */  
                                            /*!< 2'b10, Fout = Fvco/4 */  
                                            /*!< 2'b11, Fout = Fvco/8 */  
}CLK_SfllConfig_Type;


/** 
 *  @brief AUPLL configure structure definition 
 */
typedef struct
{
  uint32_t         refDiv;                  /*!< Divider for reference clock, 5-bit, range 1~31 */
                                            /*!< 2 MHz < Fref (frequency of Reference clock / refDiv) < 4 MHz */
  
  uint32_t         fbDiv;                   /*!< Divider for feedback clock, 9-bit, range 16~511 */  
  
  uint32_t         icp;                     /*!< charge pump current control, 3-bit, reserved now */
  uint32_t         updateRate;              /*!< update rate control, 1-bit */ 
    
                                            /*!< Select icp and updateRate per Fref */
                                            /*!< icp = 4, updateRate = 1, if 2 MHz < Fref <= 2.35 MHz */
                                            /*!< icp = 3, updateRate = 1, if 2.35 MHz < Fref <= 2.75 MHz */
                                            /*!< icp = 6, updateRate = 0, if 2.75 MHz < Fref <= 3.25 MHz */
                                            /*!< icp = 5, updateRate = 0, if 3.25 MHz < Fref <= 4 MHz */
  
  uint32_t         usbPostDiv;              /*!< USB clock post divider, 1-bit */
                                            /*!< 1'b0, Fout = Fvco/9 */
                                            /*!< 1'b1, Fout = Fvco/10 */  
}CLK_AupllConfig_Type;

/*@} end of group Clock_Public_Types definitions */


/** @defgroup Clock_Public_Constants
 *  @{
 */ 

/** @defgroup Clock_Command
 *  @{
 */ 

/*@} end of group Clock_Command */
  

/** @defgroup Clock_Interrupt_Flag
 *  @{
 */

/*@} end of group Clock_Interrupt_Flag */

/*@} end of group Clock_Public_Constants */

/** @defgroup Clock_Public_Macro
 *  @{
 */

/** @defgroup CLK_MAINXTAL_FREQUENCY        
 *  @{
 */
#define CLK_MAINXTAL_FREQUENCY   32000000
/*@} end of group CLK_MAINXTAL_FREQUENCY */

/** @defgroup SYS_Clock_Source        
 *  @{
 */
#define IS_SYSTEM_CLOCK_SOURCE(SOURCE)   ( ((SOURCE >= 0) && (SOURCE <= 1)) || (SOURCE == 4) )
/*@} end of group SYS_Clock_Source */

/** @defgroup SFLL_Ref_Source        
 *  @{
 */
#define IS_SFLL_REF_SOURCE(REF_CLOCK_SRC)   ( (REF_CLOCK_SRC >= 0) && (REF_CLOCK_SRC <= 1) )
/*@} end of group SFLL_Ref_Source */

/** @defgroup SFLL_Ref_Div        
 *  @{
 */
#define IS_SFLL_REF_DIV(REF_DIV)   ( (REF_DIV >= 2) && (REF_DIV <= 255) )
/*@} end of group SFLL_Ref_Div */

/** @defgroup SFLL_FB_Div        
 *  @{
 */
#define IS_SFLL_FB_DIV(FB_DIV)   ( (FB_DIV >= 1) && (FB_DIV <= 2047) )
/*@} end of group SFLL_FB_Div */

/** @defgroup SFLL_KVCO        
 *  @{
 */
#define IS_SFLL_KVCO(KVCO)   ( (KVCO >= 0) && (KVCO <= 7) )
/*@} end of group SFLL_KVCO */

/** @defgroup SFLL_POSTDIV        
 *  @{
 */
#define IS_SFLL_POSTDIV(POSTDIV)   ( (POSTDIV >= 0) && (POSTDIV <= 3) )
/*@} end of group SFLL_POSTDIV */

/** @defgroup AUPLL_Ref_Div        
 *  @{
 */
#define IS_AUPLL_REF_DIV(REF_DIV)   ( (REF_DIV >= 1) && (REF_DIV <= 31) )
/*@} end of group AUPLL_Ref_Div */

/** @defgroup AUPLL_FB_Div        
 *  @{
 */
#define IS_AUPLL_FB_DIV(FB_DIV)   ( (FB_DIV >= 16) && (FB_DIV <= 511) )
/*@} end of group AUPLL_FB_Div */

/** @defgroup AUPLL_ICP
 *  @{
 */
#define IS_AUPLL_ICP(ICP)   ( (ICP >= 3) && (ICP <= 6) )
/*@} end of group AUPLL_ICP */

/** @defgroup AUPLL_UPDATE_RATE        
 *  @{
 */
#define IS_AUPLL_UPDATE_RATE(RATE)   ( (RATE >= 0) && (RATE <= 1) )
/*@} end of group AUPLL_UPDATE_RATE */

/** @defgroup AUPLL_POSTDIV_USB        
 *  @{
 */
#define IS_AUPLL_POSTDIV_USB(POSTDIV_USB)   ( (POSTDIV_USB >= 0) && (POSTDIV_USB <= 1) )
/*@} end of group AUPLL_POSTDIV_USB */

/** @defgroup AUPLL_OUT_CLK        
 *  @{
 */
#define IS_AUPLL_OUT_CLK(OUT_CLK)   ( (OUT_CLK >= 0) && (OUT_CLK <= 2) )
/*@} end of group AUPLL_POSTDIV_USB */

/** @defgroup AUPLL_AUDIO_POSTDIV        
 *  @{
 */
#define IS_AUPLL_AUDIO_POSTDIV(POSTDIV)   ( (POSTDIV >= 2) && (POSTDIV <= 127) && ((!(POSTDIV & 0x1)) || (POSTDIV == 3)) )
/*@} end of group AUPLL_AUDIO_POSTDIV */

/** @defgroup OSC_MODE        
 *  @{
 */
#define IS_OSC_MODE(MODE)   ( (MODE >= 0) && (MODE <= 1) )
/*@} end of group OSC_MODE */

/** @defgroup RC_CALIBRATION_MODE        
 *  @{
 */
#define IS_RC_CALIBRATION_MODE(MODE)   ( (MODE >= 0) && (MODE <= 1) )
/*@} end of group RC_CALIBRATION_MODE */

/** @defgroup CLK_SRC_STATUS        
 *  @{
 */
#define IS_CLK_SRC_STATUS(CLKSRC)   ( (CLKSRC >= 0) && (CLKSRC <= 5) )
/*@} end of group CLK_SRC_STATUS  */

/** @defgroup GATED_CLK_MODULE        
 *  @{
 */
#define IS_GATED_CLK_MODULE(MODULE)   ( (MODULE >= 0) && (MODULE <= 26) )
/*@} end of group GATED_CLK_MODULE  */

/** @defgroup DIVIDED_CLK_MODULE        
 *  @{
 */
#define IS_DIVIDED_CLK_MODULE(MODULE)   ( (MODULE == 0)                        \
                                       || ((MODULE >= 5) && (MODULE <= 9))     \
                                       || ((MODULE >= 12) && (MODULE <= 16))   \
                                       || ((MODULE >= 18) && (MODULE <= 19))   \
                                       || (MODULE == 22)                       \
                                       || ((MODULE >= 27) && (MODULE <= 34)) )
/*@} end of group DIVIDED_CLK_MODULE  */

/** @defgroup APB_I2C_DIVIDER        
 *  @{
 */
#define IS_APB_I2C_DIVIDER(DIVIDER)   ( (DIVIDER >= 0) && (DIVIDER <= 3) )
/*@} end of group APB_I2C_DIVIDER  */

/** @defgroup QSPI_FLASHC_GPTSAMP_DIVIDER        
 *  @{
 */
#define IS_QSPI_FLASHC_GPTSAMP_DIVIDER(DIVIDER)   ( (DIVIDER >= 0) && (DIVIDER <= 7) )
/*@} end of group QSPI_FLASHC_GPTSAMP_DIVIDER  */

/** @defgroup SDIO_PMU_DIVIDER        
 *  @{
 */
#define IS_SDIO_PMU_DIVIDER(DIVIDER)   ( (DIVIDER >= 0) && (DIVIDER <= 15) )
/*@} end of group SDIO_PMU_DIVIDER  */

/** @defgroup SSP_DIVIDER        
 *  @{
 */
#define IS_SSP_DIVIDER(DIVIDER)   ( (DIVIDER >= 0) && (DIVIDER <= 31) )
/*@} end of group SSP_DIVIDER  */

/** @defgroup CORE_GPT_DIVIDER        
 *  @{
 */
#define IS_CORE_GPT_DIVIDER(DIVIDER)   ( (DIVIDER >= 0) && (DIVIDER <= 63) )
/*@} end of group CORE_GPT_DIVIDER  */

/** @defgroup CAU_Clock_Source        
 *  @{
 */
#define IS_CAU_Clock_Source(SOURCE)   ( ((SOURCE >= 0) && (SOURCE <= 1)) || (SOURCE == 5) )
/*@} end of group CAU_Clock_Source */

/** @defgroup GPT_Clock_Source        
 *  @{
 */
#define IS_GPT_Clock_Source(SOURCE)   ( ((SOURCE >= 0) && (SOURCE <= 3)) || (SOURCE == 6) )
/*@} end of group GPT_Clock_Source */

/** @defgroup SSP_Clock_Source        
 *  @{
 */
#define IS_SSP_Clock_Source(SOURCE)   ( (SOURCE >= 5) && (SOURCE <= 6) )
/*@} end of group SSP_Clock_Source */

/** @defgroup RTC_Clock_Source        
 *  @{
 */
#define IS_RTC_Clock_Source(SOURCE)   ( (SOURCE >= 2) && (SOURCE <= 3) )
/*@} end of group RTC_Clock_Source */

/** @defgroup UART_Clock_Source        
 *  @{
 */
#define IS_UART_Clock_Source(SOURCE)   ( (SOURCE >= 0) && (SOURCE <= 1) )
/*@} end of group UART_Clock_Source */

/** @defgroup UART_Dividend        
 *  @{
 */
#define IS_UART_Dividend(DIVIDEND)   ( (DIVIDEND >= 1) && (DIVIDEND <= 2047) )
/*@} end of group UART_Dividend */

/** @defgroup UART_Divisor        
 *  @{
 */
#define IS_UART_Divisor(DIVISOR)   ( (DIVISOR >= 1) && (DIVISOR <= 8191) )
/*@} end of group UART_Divisor */

/** @defgroup WDT_DIVIDER        
 *  @{
 */
#define IS_WDT_DIVIDER(DIVIDER)   ( (DIVIDER >= 0) && (DIVIDER <= 63) )
/*@} end of group CORE_GPT_DIVIDER  */


/*@} end of group Clock_Public_Macro */

/** @defgroup Clock_Public_FunctionDeclaration
 *  @brief Clock functions statement
 *  @{
 */
void CLK_MainXtalEnable(CLK_CrystalMode_Type oscMode);
void CLK_MainXtalDisable(void);

void CLK_Xtal32KEnable(CLK_CrystalMode_Type oscMode);
void CLK_Xtal32KDisable(void);

void CLK_RC32MEnable(void);
void CLK_RC32MDisable(void);

void CLK_RC32KEnable(void);
void CLK_RC32KDisable(void);

int32_t CLK_RC32MCalibration(CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode);
int32_t CLK_RC32KCalibration(CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode);

void CLK_SfllEnable(CLK_SfllConfig_Type * sfllConfigSet);
void CLK_SfllDisable(void);

void CLK_AupllEnable(CLK_AupllConfig_Type * aupllConfigSet);
void CLK_AupllDisable(void);
void CLK_AupllClkOutEnable(CLK_AupllClkOut_Type aupllOutClk);
void CLK_AupllClkOutDisable(CLK_AupllClkOut_Type aupllOutClk);
void CLK_SetAupllAudioFreq(int16_t freqOffset, uint8_t audioPostDiv);

FlagStatus CLK_GetClkStatus(CLK_Src_Type clockSource);

void CLK_SystemClkSrc(CLK_Src_Type clockSource);

uint32_t CLK_GetSystemClk(void);

void CLK_CAUClkSrc(CLK_Src_Type clockSource);
void CLK_GPTInternalClkSrc(CLK_GptID_Type gptIndex, CLK_Src_Type clockSource);
void CLK_SSPClkSrc(CLK_SspID_Type sspIndex, CLK_Src_Type clockSource);
void CLK_RTCClkSrc(CLK_Src_Type clockSource);

void CLK_UARTDividerSet(CLK_UartSrc_Type uartClockSource, CLK_Fraction_Type uartClkFraction);
void CLK_SetUARTClkSrc(CLK_UartID_Type uartIndex, CLK_UartSrc_Type uartClockSource);
CLK_UartSrc_Type CLK_GetUARTClkSrc(CLK_UartID_Type uartIndex);
void CLK_GetUARTDivider(CLK_UartSrc_Type uartClockSource, CLK_Fraction_Type * uartClkFraction);

void CLK_ModuleClkEnable(CLK_Module_Type clkModule);
void CLK_ModuleClkDisable(CLK_Module_Type clkModule);

void CLK_ModuleClkDivider(CLK_Module_Type clkModule, uint32_t divider);

IntStatus RC32M_GetIntStatus(RC32M_INT_Type intType);
void RC32M_SelOutput32MHz(RC32M_SELFREQ_Type freqType);
void RC32M_IntMask(RC32M_INT_Type intType, IntMask_Type intMask);
void RC32M_IntClr(RC32M_INT_Type intType);
void RC32M_IRQHandler(void);
void CLK_Init(void);

/*@} end of group Clock_Public_FunctionDeclaration */

/*@} end of group Clock */

/*@} end of group MC200_Periph_Driver */

#endif

