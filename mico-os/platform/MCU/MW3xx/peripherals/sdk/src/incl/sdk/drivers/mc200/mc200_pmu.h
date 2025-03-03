/****************************************************************************//**
 * @file     mc200_pmu.h
 * @brief    PMU driver module header file.
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

#ifndef __MC200_PMU_H__
#define __MC200_PMU_H__

#include "mc200_driver.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @addtogroup  PMU
 *  @{
 */

/** @defgroup PMU_Public_Types PMU_Public_Types
 *  @{
 */

/** 
 *  @brief PMU enable or disable definition 
 */

/** 
 *  @brief sleep mode definition 
 */
typedef enum
{
  PMU_PM1 = 0,                            /*!< Power Mode 1 */
  PMU_PM2,                                /*!< Power Mode 2 */
  PMU_PM3,                                /*!< Power Mode 3 */
  PMU_PM4,                                /*!< Power Mode 4 */
}PMU_SleepMode_Type;

/**
 *  @brief wakeup source list
 */
typedef enum
{
  PMU_GPIO25_INT,                        /*!< Wakeup Int PIN GPIO25 */
  PMU_GPIO26_INT,                        /*!< Wakeup Int PIN GPIO26 */
}PMU_WakeupPinSrc_Type;

/**
 *  @brief wakeup pin trigger mode list
 */
typedef enum
{
  PMU_WAKEUP_LEVEL_LOW = 0,              /*!< Wakeup at low level */
  PMU_WAKEUP_LEVEL_HIGH,                 /*!< Wakeup at high level */
}PMU_WakeupTriggerMode_Type; 

/** 
 *  @brief wakeup pad pull up/down mode list 
 */
typedef enum
{
  PMU_WAKEUP_PAD_PULLUP = 0,              /*!< Wakeup pad pull up */ 
  PMU_WAKEUP_PAD_PULLDOWN,                /*!< Wakeup pad pull down */ 
}PMU_WakeupPadMode_Type;

/** 
 *  @brief last reset cause list 
 */
typedef enum
{
  PMU_BROWNOUT_VBAT   = 0x01,             /*!< Vbat brown-out reset */
  PMU_CM3_SYSRESETREQ = 0x08,             /*!< CM3 SYSRESETREQ reset */
  PMU_CM3_LOCKUP      = 0x10,             /*!< CM3 lockup reset */
  PMU_WDTOUT          = 0x20,             /*!< WDT-out reset */
  PMU_RESETSRC_ALL    = 0x7F,             /*!< All reset source */
}PMU_LastResetCause_Type;

/** 
 *  @brief power saving pad list 
 */
typedef enum
{
  PMU_PAD_XTAL32K_IN,                    /*!< XTAL32K_IN pad */
  PMU_PAD_XTAL32K_OUT,                   /*!< XTAL32K_OUT pad */
  PMU_PAD_TDO,                           /*!< TDO pad */
  PMU_PAD_ALLTHREE,                      /*!< All three pad */
}PMU_PowerSavePadSrc_Type;

/** 
 *  @brief power saving pad mode list 
 */
typedef enum
{
  PMU_PAD_MODE_NORMAL = 0,                /*!< normal mode */
  PMU_PAD_MODE_POWER_SAVING,              /*!< power saving mode */
}PMU_PowerSavePadMode_Type;

/** 
 *  @brief X32K output pad list 
 */
typedef enum
{
  PMU_PAD_GPIO27,                    /*!< GPIO27 pad */
  PMU_PAD_GPIO25,                         /*!< GPIO25 pad */
  PMU_PAD_GPIO26,                         /*!< GPIO26 pad */
}PMU_X32KOutputPadSrc_Type;

/** 
 *  @brief X32K output pad mode list 
 */
typedef enum
{
  PMU_PAD_MODE_PINMUX = 0,                /*!< X32K output pad normal pin muxing mode */   
  PMU_PAD_MODE_X32K_OUTPUT,               /*!< X32K output pad x32k output mode */
}PMU_X32KOutputPadMode_Type;

/** 
 *  @brief vddio domain list 
 */
typedef enum
{
  PMU_VDDIO_D0,                            /*!< VDDIO domain 0 */
                                           /*!< GPIO_0/1/2/3/4/5/6/7/8/9/10/11/12/13/14/15/16/17 */
                                           
  PMU_VDDIO_AON,                           /*!< VDDIO domain AON */
                                           /*!< OSC32K_IN/OSC32K_OUT/TDO/TCK/TMS/TDI/TRST/WAKEUP0/WAKEUP1/GPIO_27 */
                                           
  PMU_VDDIO_D1,                           /*!< VDDIO domain 1 */
                                           /*!< GPIO_28/29/30/31/32/33/34/35/36/37/38/39 */  
                                           /*!< GPIO_40/41/42/43/44/45/46/47/48/49/50 */  
                                           
  PMU_VDDIO_SDIO,                          /*!< VDDIO domain SDIO */
                                           /*!< GPIO_51/52/53/54/55/56 */  
                                           
  PMU_VDDIO_D2,                            /*!< VDDIO domain 2 */
                                           /*!< GPIO_59/60/61/62/63/64/65/66/67/68/69/70/71/72/73/74/75/76/77/78/79 */
                                           
  PMU_VDDIO_FL,                            /*!< VDDIO domain FLASH */
                                           
}PMU_VDDIODOMAIN_Type;

/** 
 *  @brief vddio level list 
 */
typedef enum
{
  PMU_VDDIO_LEVEL_3P3V = 0,                 /*!< VDDIO 3.3V */
  PMU_VDDIO_LEVEL_1P8V = 1,                 /*!< VDDIO 1.8V */                                           
}PMU_VDDIOLEVEL_Type;

/** 
 *  @brief extra interrupt list 
 */
typedef enum
{
  PMU_INT34_GPIO_0,                         /*!< INT_34 from GPIO_0 */
  PMU_INT34_GPIO_1,                         /*!< INT_34 from GPIO_1 */
  PMU_INT34_GPIO_2,                         /*!< INT_34 from GPIO_2 */
  
  PMU_INT35_GPIO_3,                         /*!< INT_35 from GPIO_3 */
  PMU_INT35_GPIO_4,                         /*!< INT_35 from GPIO_4 */
  PMU_INT35_GPIO_5,                         /*!< INT_35 from GPIO_5 */
  
  PMU_INT36_GPIO_6,                         /*!< INT_36 from GPIO_6 */
  PMU_INT36_GPIO_7,                         /*!< INT_36 from GPIO_7 */
  PMU_INT36_GPIO_8,                         /*!< INT_36 from GPIO_8 */
  
  PMU_INT37_GPIO_9,                         /*!< INT_37 from GPIO_9 */
  PMU_INT37_GPIO_10,                        /*!< INT_37 from GPIO_10 */
  PMU_INT37_GPIO_11,                        /*!< INT_37 from GPIO_11 */
                                       
  PMU_INT38_GPIO_12,                        /*!< INT_38 from GPIO_12 */
  PMU_INT38_GPIO_13,                        /*!< INT_38 from GPIO_13 */
  PMU_INT38_GPIO_14,                        /*!< INT_38 from GPIO_14 */
  
  PMU_INT39_GPIO_15,                        /*!< INT_39 from GPIO_15 */
  PMU_INT39_GPIO_16,                        /*!< INT_39 from GPIO_16 */
  PMU_INT39_GPIO_17,                        /*!< INT_39 from GPIO_17 */ 
  
  PMU_INT40_GPIO_18,                        /*!< INT_40 from GPIO_18 */
  PMU_INT40_GPIO_19,                        /*!< INT_40 from GPIO_19 */
  PMU_INT40_GPIO_20,                        /*!< INT_40 from GPIO_20 */
  
  PMU_INT41_GPIO_21,                        /*!< INT_41 from GPIO_21 */
  PMU_INT41_GPIO_22,                        /*!< INT_41 from GPIO_22 */
  PMU_INT41_GPIO_23,                        /*!< INT_41 from GPIO_23 */
  
  PMU_INT42_GPIO_24,                        /*!< INT_42 from GPIO_24 */
  PMU_INT42_GPIO_28,                        /*!< INT_42 from GPIO_28 */
  PMU_INT42_GPIO_29,                        /*!< INT_42 from GPIO_29 */
  
  PMU_INT43_GPIO_30,                        /*!< INT_43 from GPIO_30 */
  PMU_INT43_GPIO_31,                        /*!< INT_43 from GPIO_31 */
  PMU_INT43_GPIO_32,                        /*!< INT_43 from GPIO_32 */
  
  PMU_INT44_GPIO_33,                        /*!< INT_44 from GPIO_33 */
  PMU_INT44_GPIO_34,                        /*!< INT_44 from GPIO_34 */
  PMU_INT44_GPIO_35,                        /*!< INT_44 from GPIO_35 */
  
  PMU_INT45_GPIO_36,                        /*!< INT_45 from GPIO_36 */
  PMU_INT45_GPIO_37,                        /*!< INT_45 from GPIO_37 */
  PMU_INT45_GPIO_38,                        /*!< INT_45 from GPIO_38 */    
  
  PMU_INT46_GPIO_39,                        /*!< INT_46 from GPIO_39 */
  PMU_INT46_GPIO_40,                        /*!< INT_46 from GPIO_40 */
  PMU_INT46_GPIO_41,                        /*!< INT_46 from GPIO_41 */
  
  PMU_INT47_GPIO_42,                        /*!< INT_47 from GPIO_42 */
  PMU_INT47_GPIO_43,                        /*!< INT_47 from GPIO_43 */
  PMU_INT47_GPIO_44,                        /*!< INT_47 from GPIO_44 */ 
  
  PMU_INT48_GPIO_45,                        /*!< INT_48 from GPIO_45 */
  PMU_INT48_GPIO_46,                        /*!< INT_48 from GPIO_46 */
  PMU_INT48_GPIO_47,                        /*!< INT_48 from GPIO_47 */
  
  PMU_INT49_GPIO_48,                        /*!< INT_49 from GPIO_48 */
  PMU_INT49_GPIO_49,                        /*!< INT_49 from GPIO_49 */
  PMU_INT49_GPIO_50,                        /*!< INT_49 from GPIO_50 */     
  
  PMU_INT50_GPIO_51,                        /*!< INT_50 from GPIO_51 */
  PMU_INT50_GPIO_52,                        /*!< INT_50 from GPIO_52 */
  PMU_INT50_GPIO_53,                        /*!< INT_50 from GPIO_53 */     
  
  PMU_INT51_GPIO_54,                        /*!< INT_51 from GPIO_54 */
  PMU_INT51_GPIO_55,                        /*!< INT_51 from GPIO_55 */
  PMU_INT51_GPIO_56,                        /*!< INT_51 from GPIO_56 */  
  
  PMU_INT52_GPIO_57,                        /*!< INT_52 from GPIO_57 */
  PMU_INT52_GPIO_58,                        /*!< INT_52 from GPIO_58 */
  PMU_INT52_GPIO_59,                        /*!< INT_52 from GPIO_59 */  
  
  PMU_INT53_GPIO_60,                        /*!< INT_53 from GPIO_60 */
  PMU_INT53_GPIO_61,                        /*!< INT_53 from GPIO_61 */
  PMU_INT53_GPIO_62,                        /*!< INT_53 from GPIO_62 */  
  
  PMU_INT54_GPIO_63,                        /*!< INT_54 from GPIO_63 */
  PMU_INT54_GPIO_64,                        /*!< INT_54 from GPIO_64 */
  PMU_INT54_GPIO_65,                        /*!< INT_54 from GPIO_65 */ 
  
  PMU_INT55_GPIO_66,                        /*!< INT_55 from GPIO_66 */
  PMU_INT55_GPIO_67,                        /*!< INT_55 from GPIO_67 */
  PMU_INT55_GPIO_68,                        /*!< INT_55 from GPIO_68 */  
  
  PMU_INT56_GPIO_69,                        /*!< INT_56 from GPIO_69 */
  PMU_INT56_GPIO_70,                        /*!< INT_56 from GPIO_70 */
  PMU_INT56_GPIO_71,                        /*!< INT_56 from GPIO_71 */ 
  
  PMU_INT57_GPIO_72,                        /*!< INT_57 from GPIO_72 */
  PMU_INT57_GPIO_73,                        /*!< INT_57 from GPIO_73 */
  PMU_INT57_GPIO_74,                        /*!< INT_57 from GPIO_74 */  
  
  PMU_INT58_GPIO_75,                        /*!< INT_58 from GPIO_75 */
  PMU_INT58_GPIO_76,                        /*!< INT_58 from GPIO_76 */
  PMU_INT58_GPIO_77,                        /*!< INT_58 from GPIO_77 */  
  
  PMU_INT59_GPIO_78,                        /*!< INT_59 from GPIO_78 */
  PMU_INT59_GPIO_79,                        /*!< INT_59 from GPIO_79 */
}PMU_EXTRAINT_Type;

/**
 *  @brief ulpcomp mode list
 */
typedef enum
{
  PMU_ULPCOMP_MODE_SINGLE = 0x0,            /*!< Ulpcomp single-ended mode */
  PMU_ULPCOMP_MODE_DIFFERENTIAL,            /*!< Ulpcomp differential mode */
}PMU_UlpcompMode_Type;

/**
 *  @brief ulpcomp reference voltage list
 */
typedef enum
{
  PMU_ULPCOMP_REFVOLT_0 = 0x0,              /*!< Ulpcomp reference voltage level 0 */
  PMU_ULPCOMP_REFVOLT_1,                    /*!< Ulpcomp reference voltage level 1 */
  PMU_ULPCOMP_REFVOLT_2,                    /*!< Ulpcomp reference voltage level 2 */
  PMU_ULPCOMP_REFVOLT_3,                    /*!< Ulpcomp reference voltage level 3 */
  PMU_ULPCOMP_REFVOLT_4,                    /*!< Ulpcomp reference voltage level 4 */
  PMU_ULPCOMP_REFVOLT_5,                    /*!< Ulpcomp reference voltage level 5 */
  PMU_ULPCOMP_REFVOLT_6,                    /*!< Ulpcomp reference voltage level 6 */
  PMU_ULPCOMP_REFVOLT_7,                    /*!< Ulpcomp reference voltage level 7 */
}PMU_UlpcompRef_Type;

/**
 *  @brief ulpcomp hysteresis list
 */
typedef enum
{
  PMU_ULPCOMP_HYST_0 = 0x0,                 /*!< Ulpcomp hysteresis level 0 */
  PMU_ULPCOMP_HYST_1,                       /*!< Ulpcomp hysteresis level 1 */
  PMU_ULPCOMP_HYST_2,                       /*!< Ulpcomp hysteresis level 2 */
  PMU_ULPCOMP_HYST_3,                       /*!< Ulpcomp hysteresis level 3 */
}PMU_UlpcompHyst_Type;

/**
 *  @brief vbat brownout detect trigger voltage list
 */
typedef enum
{
  PMU_VBAT_BRNDET_TRIGVOLT_0 = 0x0,         /*!< Vbat brndet trigger voltage level 0 */
  PMU_VBAT_BRNDET_TRIGVOLT_1,               /*!< Vbat brndet trigger voltage level 1 */
  PMU_VBAT_BRNDET_TRIGVOLT_2,               /*!< Vbat brndet trigger voltage level 2 */
  PMU_VBAT_BRNDET_TRIGVOLT_3,               /*!< Vbat brndet trigger voltage level 3 */
  PMU_VBAT_BRNDET_TRIGVOLT_4,               /*!< Vbat brndet trigger voltage level 4 */
  PMU_VBAT_BRNDET_TRIGVOLT_5,               /*!< Vbat brndet trigger voltage level 5 */
  PMU_VBAT_BRNDET_TRIGVOLT_6,               /*!< Vbat brndet trigger voltage level 6 */
  PMU_VBAT_BRNDET_TRIGVOLT_7,               /*!< Vbat brndet trigger voltage level 7 */
}PMU_VbatBrndetTrig_Type;

/**
 *  @brief vbat brownout detect hysteresis list
 */
typedef enum
{
  PMU_VBAT_BRNDET_HYST_0 = 0x0,             /*!< Vbat brndet hysteresis level 0 */
  PMU_VBAT_BRNDET_HYST_1,                   /*!< Vbat brndet hysteresis level 1 */
  PMU_VBAT_BRNDET_HYST_2,                   /*!< Vbat brndet hysteresis level 2 */
  PMU_VBAT_BRNDET_HYST_3,                   /*!< Vbat brndet hysteresis level 3 */
}PMU_VbatBrndetHyst_Type;

/**
 *  @brief vbat brownout detect filter list
 */
typedef enum
{
  PMU_VBAT_BRNDET_FILT_0 = 0x0,             /*!< Vbat brndet filter level 0 */
  PMU_VBAT_BRNDET_FILT_1,                   /*!< Vbat brndet filter level 1 */
  PMU_VBAT_BRNDET_FILT_2,                   /*!< Vbat brndet filter level 2 */
  PMU_VBAT_BRNDET_FILT_3,                   /*!< Vbat brndet filter level 3 */
}PMU_VbatBrndetFilt_Type;

/**
 *  @brief LDO V18 ramp rate list
 */
typedef enum
{
  PMU_V18_RAMP_RATE_SLOW = 0x3,             /*!< LDO V18 slow ramp up */
  PMU_V18_RAMP_RATE_FAST = 0x2,             /*!< LDO V18 fast ramp up */
}PMU_V18RampRate_Type;

/**
 *  @brief LDO V12 ramp rate list
 */
typedef enum
{
  PMU_V12_RAMP_RATE_SLOW = 0x3,             /*!< LDO V12 slow ramp up */
  PMU_V12_RAMP_RATE_FAST = 0x2,             /*!< LDO V12 slow ramp up */
}PMU_V12RampRate_Type;

/**
 *  @brief vfl power ready signal delay time list
 */
typedef enum
{
  PMU_VFL_READY_DELAY_LONG = 0x3,          /*!< vfl power ready signal long delay */
  PMU_VFL_READY_DELAY_SHORT = 0x1,         /*!< vfl power ready signal short delay */
}PMU_VflReadyDelay_Type;

/**
 *  @brief v18 power ready signal delay time list
 */
typedef enum
{
  PMU_V18_READY_DELAY_LONG = 0x3,         /*!< v18 power ready signal long delay */
  PMU_V18_READY_DELAY_SHORT = 0x0,        /*!< v18 power ready signal short delay */
}PMU_V18ReadyDelay_Type;

/**
 *  @brief v12 power ready signal delay time list
 */
typedef enum
{
  PMU_V12_READY_DELAY_LONG = 0x3,        /*!< v12 power ready signal long delay */
  PMU_V12_READY_DELAY_SHORT = 0x0,       /*!< v12 power ready signal short delay */
}PMU_V12ReadyDelay_Type;

/**
 *  @brief vbat brownout detect configure type
 */
typedef struct
{
  PMU_VbatBrndetTrig_Type brnTrigVolt;      /*!< Configure vbat brndet trigger voltage level 
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_0 (0): level 0
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_1 (1): level 1
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_2 (2): level 2
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_3 (3): level 3
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_4 (4): level 4
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_5 (5): level 5
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_6 (6): level 6
                                                                                            PMU_VBAT_BRNDET_TRIGVOLT_7 (7): level 7 */
  PMU_VbatBrndetHyst_Type brnHyst;          /*!< Configure vbat brndet hysteresis level
                                                                                            PMU_VBAT_BRNDET_HYST_0 (0): level 0
                                                                                            PMU_VBAT_BRNDET_HYST_1 (1): level 1
                                                                                            PMU_VBAT_BRNDET_HYST_2 (2): level 2
                                                                                            PMU_VBAT_BRNDET_HYST_3 (3): level 3 */
  PMU_VbatBrndetFilt_Type brnFilter;        /*!< Configure vbat brndet filter level
                                                                                            PMU_VBAT_BRNDET_FILT_0 (0): level 0
                                                                                            PMU_VBAT_BRNDET_FILT_1 (1): level 1
                                                                                            PMU_VBAT_BRNDET_FILT_2 (2): level 2
                                                                                            PMU_VBAT_BRNDET_FILT_3 (3): level 3 */
  
}PMU_VbatBrndetConfig_Type;

/**
 *  @brief power ramp rate configure type
 */
typedef struct
{
  PMU_V18RampRate_Type v18ramp;             /*!< Configure v18 ramp rate level   
                                                                                            PMU_V18_RAMP_RATE_SLOW (3): slow ramp
                                                                                            PMU_V18_RAMP_RATE_FAST (2): fast ramp */
  PMU_V12RampRate_Type v12ramp;             /*!< Configure v12 ramp rate level   
                                                                                            PMU_V12_RAMP_RATE_SLOW (3): slow ramp
                                                                                            PMU_V12_RAMP_RATE_FAST (2): fast ramp */
}PMU_PowerRampRateConfig_Type;

/**
 *  @brief power ready signal delay time configure type
 */
typedef struct
{
  PMU_VflReadyDelay_Type vflDelay;         /*!< Configure vfl delay time level 
                                                                                            PMU_VFL_READY_DELAY_LONG (3): long delay 
                                                                                            PMU_VFL_READY_DELAY_SHORT (1): short delay */
  PMU_V18ReadyDelay_Type v18Delay;         /*!< Configure v18 delay time level 
                                                                                            PMU_V18_READY_DELAY_LONG (3): long delay 
                                                                                            PMU_V18_READY_DELAY_SHORT (0): short delay */
  PMU_V12ReadyDelay_Type v12Delay;         /*!< Configure v12 delay time level 
                                                                                            PMU_V12_READY_DELAY_LONG (3): long delay 
                                                                                            PMU_V12_READY_DELAY_SHORT (0): short delay */
}PMU_PowerReadyDelayConfig_Type;

/** @defgroup PMU_Public_Constants
 *  @{
 */

/** @defgroup PMU_Command
 *  @{
 */

/*@} end of group PMU_Command */

/** @defgroup PMU_Interrupt_Flag
 *  @{
 */

/*@} end of group PMU_Interrupt_Flag */

/*@} end of group PMUPublic_Constants */

/** @defgroup PMU_Public_Macro
 *  @{
 */

/*@} end of group PMU_Public_Macro */

/** @defgroup PMU_Public_FunctionDeclaration
 *  @brief PMU functions statement
 *  @{
 */
void PMU_SetSleepMode(PMU_SleepMode_Type pmuMode);
uint32_t PMU_GetSleepMode(void);

void PMU_ConfigWakeupPin(PMU_WakeupPinSrc_Type wakeupPin, PMU_WakeupTriggerMode_Type trigmode);
void PMU_ClearWakeupSrc(PMU_WakeupPinSrc_Type wakeupPin);
void PMU_ConfigWakeupPadMode(PMU_WakeupPinSrc_Type wakeupPin, PMU_WakeupPadMode_Type padmode);

uint32_t PMU_GetLastResetCause(void);
void PMU_ClrLastResetCause(PMU_LastResetCause_Type resetCause);

void PMU_ConfigPowerSavePadMode(PMU_PowerSavePadSrc_Type pad, PMU_PowerSavePadMode_Type padmode);

void PMU_ConfigX32KOutputPadMode(PMU_X32KOutputPadSrc_Type pad, PMU_X32KOutputPadMode_Type padmode);

void PMU_PowerOnVDDIO(PMU_VDDIODOMAIN_Type domain);
void PMU_PowerOffVDDIO(PMU_VDDIODOMAIN_Type domain);
void PMU_PowerDeepOffVDDIO(PMU_VDDIODOMAIN_Type domain);
void PMU_ConfigVDDIOLevel(PMU_VDDIODOMAIN_Type domain, PMU_VDDIOLEVEL_Type level);

void PMU_ConfigExtraInterrupt(PMU_EXTRAINT_Type gpioPin);

void PMU_UlpcompModeSelect(PMU_UlpcompMode_Type ulpcompMode);
void PMU_UlpcompRefVoltageSel(PMU_UlpcompRef_Type refVolType);
void PMU_UlpcompHysteresisSel(PMU_UlpcompHyst_Type hystType);
void PMU_UlpcompCmd(FunctionalState state);
FlagStatus PMU_GetUlpcompStatus(void);
uint32_t PMU_GetUlpcompOutValue(void);

void PMU_ConfigVbatBrndet(PMU_VbatBrndetConfig_Type* brndetConfig);
void PMU_VbatBrndetCmd(FunctionalState state);
void PMU_VbatBrndetRstCmd(FunctionalState state);
FlagStatus PMU_GetVbatBrndetStatus(void);
uint32_t PMU_GetVbatBrndetOutValue(void);
void PMU_VbatBrndetIntCmd(FunctionalState state);

void PMU_ConfigPowerReadyDelayTime(PMU_PowerReadyDelayConfig_Type* delayConfig);
void PMU_ConfigPowerRampUpTime(PMU_PowerRampRateConfig_Type* rampConfig);

/*@} end of group PMU_Public_FunctionDeclaration */

/*@} end of group PMU */

/*@} end of group MC200_Periph_Driver */
#endif

