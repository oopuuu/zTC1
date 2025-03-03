/****************************************************************************//**
 * @file     mc200_gpio.h
 * @brief    GPIO driver module header file.
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

#ifndef __MC200_GPIO_H__
#define __MC200_GPIO_H__

#include "mc200.h"
#include "mc200_driver.h"

/** @addtogroup MC200_Periph_Driver
 *  @{
 */

/** @addtogroup GPIO 
 *  @{
 */
  
/** @defgroup GPIO_Public_Types GPIO_Public_Types 
 *  @{
 */

/**  
 *  @brief GPIO No. type definition 
 */
typedef enum
{
  GPIO_0,                           /*!< GPIO0  Pin define */
  GPIO_1,                           /*!< GPIO1  Pin define */
  GPIO_2,                           /*!< GPIO2  Pin define */
  GPIO_3,                           /*!< GPIO3  Pin define */
  GPIO_4,                           /*!< GPIO4  Pin define */
  GPIO_5,                           /*!< GPIO5  Pin define */
  GPIO_6,                           /*!< GPIO6  Pin define */
  GPIO_7,                           /*!< GPIO7  Pin define */
  GPIO_8,                           /*!< GPIO8  Pin define */
  GPIO_9,                           /*!< GPIO9  Pin define */
  GPIO_10,                          /*!< GPIO10 Pin define */
  GPIO_11,                          /*!< GPIO11 Pin define */
  GPIO_12,                          /*!< GPIO12 Pin define */
  GPIO_13,                          /*!< GPIO13 Pin define */
  GPIO_14,                          /*!< GPIO14 Pin define */
  GPIO_15,                          /*!< GPIO15 Pin define */
  GPIO_16,                          /*!< GPIO16 Pin define */
  GPIO_17,                          /*!< GPIO17 Pin define */
  GPIO_18,                          /*!< GPIO18 Pin define */
  GPIO_19,                          /*!< GPIO19 Pin define */
  GPIO_20,                          /*!< GPIO20 Pin define */
  GPIO_21,                          /*!< GPIO21 Pin define */
  GPIO_22,                          /*!< GPIO22 Pin define */
  GPIO_23,                          /*!< GPIO23 Pin define */
  GPIO_24,                          /*!< GPIO24 Pin define */
  GPIO_25,                          /*!< GPIO25 Pin define */
  GPIO_26,                          /*!< GPIO26 Pin define */
  GPIO_27,                          /*!< GPIO27 Pin define */
  GPIO_28,                          /*!< GPIO28 Pin define */
  GPIO_29,                          /*!< GPIO29 Pin define */
  GPIO_30,                          /*!< GPIO30 Pin define */
  GPIO_31,                          /*!< GPIO31 Pin define */ 
  GPIO_32,                          /*!< GPIO32 Pin define */
  GPIO_33,                          /*!< GPIO33 Pin define */
  GPIO_34,                          /*!< GPIO34 Pin define */
  GPIO_35,                          /*!< GPIO35 Pin define */
  GPIO_36,                          /*!< GPIO36 Pin define */
  GPIO_37,                          /*!< GPIO37 Pin define */
  GPIO_38,                          /*!< GPIO38 Pin define */
  GPIO_39,                          /*!< GPIO39 Pin define */
  GPIO_40,                          /*!< GPIO40 Pin define */
  GPIO_41,                          /*!< GPIO41 Pin define */
  GPIO_42,                          /*!< GPIO42 Pin define */
  GPIO_43,                          /*!< GPIO43 Pin define */
  GPIO_44,                          /*!< GPIO44 Pin define */
  GPIO_45,                          /*!< GPIO45 Pin define */
  GPIO_46,                          /*!< GPIO46 Pin define */
  GPIO_47,                          /*!< GPIO47 Pin define */
  GPIO_48,                          /*!< GPIO48 Pin define */
  GPIO_49,                          /*!< GPIO49 Pin define */
  GPIO_50,                          /*!< GPIO50 Pin define */
  GPIO_51,                          /*!< GPIO51 Pin define */
  GPIO_52,                          /*!< GPIO52 Pin define */
  GPIO_53,                          /*!< GPIO53 Pin define */
  GPIO_54,                          /*!< GPIO54 Pin define */
  GPIO_55,                          /*!< GPIO55 Pin define */
  GPIO_56,                          /*!< GPIO56 Pin define */
  GPIO_57,                          /*!< GPIO57 Pin define */
  GPIO_58,                          /*!< GPIO58 Pin define */
  GPIO_59,                          /*!< GPIO59 Pin define */
  GPIO_60,                          /*!< GPIO60 Pin define */
  GPIO_61,                          /*!< GPIO61 Pin define */
  GPIO_62,                          /*!< GPIO62 Pin define */
  GPIO_63,                          /*!< GPIO63 Pin define */
  GPIO_64,                          /*!< GPIO64 Pin define */
  GPIO_65,                          /*!< GPIO65 Pin define */
  GPIO_66,                          /*!< GPIO66 Pin define */
  GPIO_67,                          /*!< GPIO67 Pin define */
  GPIO_68,                          /*!< GPIO68 Pin define */
  GPIO_69,                          /*!< GPIO69 Pin define */
  GPIO_70,                          /*!< GPIO70 Pin define */
  GPIO_71,                          /*!< GPIO71 Pin define */
  GPIO_72,                          /*!< GPIO72 Pin define */
  GPIO_73,                          /*!< GPIO73 Pin define */
  GPIO_74,                          /*!< GPIO74 Pin define */
  GPIO_75,                          /*!< GPIO75 Pin define */
  GPIO_76,                          /*!< GPIO76 Pin define */
  GPIO_77,                          /*!< GPIO77 Pin define */
  GPIO_78,                          /*!< GPIO78 Pin define */
  GPIO_79,                          /*!< GPIO79 Pin define */
}GPIO_NO_Type;

/**  
 *  @brief GPIO data direction type definition 
 */
typedef enum
{
  GPIO_INPUT = 0,                             /*!< Set GPIO port data direction as input */  
  GPIO_OUTPUT,	                              /*!< Set GPIO port data direction as output */
}GPIO_Dir_Type;

/**  
 *  @brief GPIO data Input/Output type definition 
 */
typedef enum
{
  GPIO_IO_LOW = 0,                            /*!< Set GPIO port data Input/Output value as low  */  
  GPIO_IO_HIGH,	                              /*!< Set GPIO port data Input/Output value as high */
}GPIO_IO_Type;

/**  
 *  @brief GPIO interrut level type definition 
 */
typedef enum
{
  GPIO_INT_RISING_EDGE = 0,                    /*!< Interrupt type: Rising edge */                                       
  GPIO_INT_FALLING_EDGE,                       /*!< Interrupt type: Falling edge */
  GPIO_INT_BOTH_EDGES,                         /*!< Interrupt type: Rising edge and Falling edge */
  GPIO_INT_DISABLE,                            /*!< Disable interrupt */
}GPIO_Int_Type;
 
/*@} end of group GPIO_Public_Types */

/** @defgroup GPIO_Public_Constants
 *  @{
 */ 
#define GPIO_MinNo        GPIO_0
#define GPIO_MaxNo        GPIO_79

/** @defgroup GPIO_Num        
 *  @{
 */
#define IS_GPIO_NO(GPIONO)                     ((GPIONO) <= GPIO_MaxNo)

/*@} end of group GPIO_Num */

/** @defgroup GPIO_DataDir       
 *  @{
 */
#define IS_GPIO_DDR(GPIODDR)                   (((GPIODDR) == GPIO_INPUT) || ((GPIODDR) == GPIO_OUTPUT))
                                                                                     
/*@} end of group GPIO_DataDir */

/** @defgroup GPIO_IO      
 *  @{
 */
#define IS_GPIO_IO(GPIODDRIO)                  (((GPIODDRIO) == GPIO_IO_LOW) || ((GPIODDRIO) == GPIO_IO_HIGH))

/*@} end of group GPIO_IO */

/** @defgroup GPIO_INT_TYPE      
 *  @{
 */
#define IS_GPIO_INTTYPE(INTTYPE)               (((INTTYPE) == GPIO_INT_RISING_EDGE) || ((INTTYPE) == GPIO_INT_FALLING_EDGE) || ((INTTYPE) == GPIO_INT_BOTH_EDGES) || ((INTTYPE) == GPIO_INT_DISABLE))

/*@} end of group GPIO_INT_TYPE */
  
/*@} end of group GPIO_Public_Constants */

/** @defgroup GPIO_Public_Macro
 *  @{
 */

/*@} end of group GPIO_Public_Macro */

/** @defgroup GPIO_Public_FunctionDeclaration
 *  @brief GPIO functions declaration
 *  @{
 */
void GPIO_SetPinDir(GPIO_NO_Type gpioNo, GPIO_Dir_Type dir);
void GPIO_WritePinOutput(GPIO_NO_Type gpioNo, GPIO_IO_Type bitVal);
GPIO_IO_Type GPIO_ReadPinLevel(GPIO_NO_Type gpioNo);

void GPIO_IntConfig(GPIO_NO_Type gpioNo, GPIO_Int_Type intType);
void GPIO_IntMask(GPIO_NO_Type gpioNo, IntMask_Type intMask);
FlagStatus GPIO_GetIntStatus(GPIO_NO_Type gpioNo);
void GPIO_IntClr(GPIO_NO_Type gpioNo);

/*@} end of group GPIO_Public_FunctionDeclaration */

/*@} end of group GPIO */

/*@} end of group MC200_Periph_Driver */
#endif /* __MC200_GPIO_H__ */