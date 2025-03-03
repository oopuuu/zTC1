/****************************************************************************//**
 * @file     mc200_pinmux.c
 * @brief    This file provides PINMUX functions.
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

#include "mc200.h"
#include "mc200_driver.h"
#include "mc200_gpio.h"
#include "mc200_pinmux.h"

/** @addtogroup MC200_Periph_Driver
 *  @{
 */

/** @defgroup PINMUX PINMUX
 *  @brief GPIO pinmux driver modules
 *  @{
 */

/** @defgroup PINMUX_Private_Type
 *  @{
 */

/*@} end of group PINMUX_Private_Type */

/** @defgroup PINMUX_Private_Defines
 *  @{
 */

/*@} end of group PINMUX_Private_Defines */

/** @defgroup PINMUX_Private_Variables
 *  @{
 */

#ifdef  DEBUG

#ifdef PACKAGE_88_PIN
/**  
 *  @brief GPIO pinumux function mask value definition for 88 pin package
 */
static const uint8_t PinMuxFunMask[80]={
  0x45,    /*!< GPIO0  pinmux function check value: 01000101b */
  0x45,    /*!< GPIO1  pinmux function check value: 01000101b */
  0x45,    /*!< GPIO2  pinmux function check value: 01000101b */
  0x45,    /*!< GPIO3  pinmux function check value: 01000101b */
  0x5D,    /*!< GPIO4  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO5  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO6  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO7  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO8  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO9  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO10 pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO11 pinmux function check value: 01011101b */
  0x00,    /*!< GPIO12 reserved */
  0x00,    /*!< GPIO13 reserved */
  0x00,    /*!< GPIO14 reserved */
  0x00,    /*!< GPIO15 reserved */
  0x1D,    /*!< GPIO16 pinmux function check value: 00011101b */
  0x1F,    /*!< GPIO17 pinmux function check value: 00011111b */
  0x5D,    /*!< GPIO18 pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO19 pinmux function check value: 01011101b */
  0x03,    /*!< GPIO20 pinmux function check value: 00000011b */
  0x03,    /*!< GPIO21 pinmux function check value: 00000011b */
  0x03,    /*!< GPIO22 pinmux function check value: 00000011b */
  0x03,    /*!< GPIO23 pinmux function check value: 00000011b */
  0x03,    /*!< GPIO24 pinmux function check value: 00000011b */
  0x1F,    /*!< GPIO25 pinmux function check value: 00011111b */
  0x5F,    /*!< GPIO26 pinmux function check value: 01011111b */
  0x77,    /*!< GPIO27 pinmux function check value: 01110111b */
  0x37,    /*!< GPIO28 pinmux function check value: 00110111b */
  0x3F,    /*!< GPIO29 pinmux function check value: 00111111b */
  0x3F,    /*!< GPIO30 pinmux function check value: 00111111b */
  0x00,    /*!< GPIO31 reserved */
  0x1F,    /*!< GPIO32 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO33 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO34 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO35 pinmux function check value: 00011111b */
  0x00,    /*!< GPIO36 reserved */
  0x00,    /*!< GPIO37 reserved */
  0x00,    /*!< GPIO38 reserved */
  0x00,    /*!< GPIO39 reserved */
  0x0F,    /*!< GPIO40 pinmux function check value: 00001111b */
  0x0F,    /*!< GPIO41 pinmux function check value: 00001111b */
  0x0F,    /*!< GPIO42 pinmux function check value: 00001111b */
  0x0F,    /*!< GPIO43 pinmux function check value: 00001111b */
  0x3F,    /*!< GPIO44 pinmux function check value: 00111111b */
  0x3F,    /*!< GPIO45 pinmux function check value: 00111111b */
  0x00,    /*!< GPIO46 reserved */
  0x00,    /*!< GPIO47 reserved */
  0x00,    /*!< GPIO48 reserved */
  0x00,    /*!< GPIO49 reserved */
  0x07,    /*!< GPIO50 pinmux function check value: 00000111b */
  0x1F,    /*!< GPIO51 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO52 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO53 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO54 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO55 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO56 pinmux function check value: 00011111b */
  0x17,    /*!< GPIO57 pinmux function check value: 00010111b */
  0x17,    /*!< GPIO58 pinmux function check value: 00010111b */
  0x1B,    /*!< GPIO59 pinmux function check value: 00011011b */
  0x1B,    /*!< GPIO60 pinmux function check value: 00011011b */
  0x1B,    /*!< GPIO61 pinmux function check value: 00011011b */
  0x1B,    /*!< GPIO62 pinmux function check value: 00011011b */
  0x1F,    /*!< GPIO63 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO64 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO65 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO66 pinmux function check value: 00011111b */
  0x00,    /*!< GPIO67 reserved */
  0x07,    /*!< GPIO68 pinmux function check value: 00000111b */
  0x00,    /*!< GPIO69 reserved */
  0x00,    /*!< GPIO70 reserved */
  0x00,    /*!< GPIO71 reserved */
  0x1F,    /*!< GPIO72 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO73 pinmux function check value: 00011111b */
  0x0F,    /*!< GPIO74 pinmux function check value: 00001111b */
  0x0B,    /*!< GPIO75 pinmux function check value: 00001011b */
  0x1F,    /*!< GPIO76 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO77 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO78 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO79 pinmux function check value: 00011111b */
};
#elif  defined(PACKAGE_68_PIN)
/**  
 *  @brief GPIO pinumux function mask value definition for 68 pin package
 */
static const uint8_t PinMuxFunMask[80]={
  0x00,    /*!< GPIO0  reserved */
  0x00,    /*!< GPIO1  reserved */
  0x00,    /*!< GPIO2  reserved */
  0x00,    /*!< GPIO3  reserved */
  0x5D,    /*!< GPIO4  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO5  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO6  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO7  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO8  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO9  pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO10 pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO11 pinmux function check value: 01011101b */
  0x00,    /*!< GPIO12 reserved */
  0x00,    /*!< GPIO13 reserved */
  0x00,    /*!< GPIO14 reserved */
  0x00,    /*!< GPIO15 reserved */
  0x00,    /*!< GPIO16 reserved */
  0x1F,    /*!< GPIO17 pinmux function check value: 00011111b */
  0x5D,    /*!< GPIO18 pinmux function check value: 01011101b */
  0x5D,    /*!< GPIO19 pinmux function check value: 01011101b */
  0x03,    /*!< GPIO20 pinmux function check value: 00000011b */
  0x03,    /*!< GPIO21 pinmux function check value: 00000011b */
  0x03,    /*!< GPIO22 pinmux function check value: 00000011b */
  0x03,    /*!< GPIO23 pinmux function check value: 00000011b */
  0x03,    /*!< GPIO24 pinmux function check value: 00000011b */
  0x1F,    /*!< GPIO25 pinmux function check value: 00011111b */
  0x5F,    /*!< GPIO26 pinmux function check value: 01011111b */
  0x77,    /*!< GPIO27 pinmux function check value: 01110111b */
  0x3F,    /*!< GPIO28 pinmux function check value: 00111111b */
  0x3F,    /*!< GPIO29 pinmux function check value: 00111111b */
  0x3F,    /*!< GPIO30 pinmux function check value: 00111111b */
  0x00,    /*!< GPIO31 reserved */
  0x00,    /*!< GPIO32 reserved */
  0x00,    /*!< GPIO33 reserved */
  0x00,    /*!< GPIO34 reserved */
  0x00,    /*!< GPIO35 reserved */
  0x00,    /*!< GPIO36 reserved */
  0x00,    /*!< GPIO37 reserved */
  0x00,    /*!< GPIO38 reserved */
  0x00,    /*!< GPIO39 reserved */
  0x00,    /*!< GPIO40 reserved */
  0x00,    /*!< GPIO41 reserved */
  0x00,    /*!< GPIO42 reserved */
  0x00,    /*!< GPIO43 reserved */
  0x3F,    /*!< GPIO44 pinmux function check value: 00111111b */
  0x3F,    /*!< GPIO45 pinmux function check value: 00111111b */
  0x00,    /*!< GPIO46 reserved */
  0x00,    /*!< GPIO47 reserved */
  0x00,    /*!< GPIO48 reserved */
  0x00,    /*!< GPIO49 reserved */
  0x00,    /*!< GPIO50 reserved */
  0x1F,    /*!< GPIO51 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO52 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO53 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO54 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO55 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO56 pinmux function check value: 00011111b */
  0x17,    /*!< GPIO57 pinmux function check value: 00010111b */
  0x1F,    /*!< GPIO58 pinmux function check value: 00011111b */
  0x00,    /*!< GPIO59 reserved */
  0x00,    /*!< GPIO60 reserved */
  0x00,    /*!< GPIO61 reserved */
  0x00,    /*!< GPIO62 reserved */
  0x1F,    /*!< GPIO63 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO64 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO65 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO66 pinmux function check value: 00011111b */
  0x00,    /*!< GPIO67 reserved */
  0x07,    /*!< GPIO68 pinmux function check value: 00000111b */
  0x00,    /*!< GPIO69 reserved */
  0x00,    /*!< GPIO70 reserved */
  0x00,    /*!< GPIO71 reserved */
  0x1F,    /*!< GPIO72 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO73 pinmux function check value: 00011111b */
  0x0F,    /*!< GPIO74 pinmux function check value: 00001111b */
  0x0B,    /*!< GPIO75 pinmux function check value: 00001011b */
  0x1F,    /*!< GPIO76 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO77 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO78 pinmux function check value: 00011111b */
  0x1F,    /*!< GPIO79 pinmux function check value: 00011111b */
};

#endif

/** @defgroup GPIO_PINMUX_FUNCTION_MASK     
 *  @{
 */
#define IS_GPIO_PINMUXFUNMASK(GPIONO, PINMUXFUNMASK)           ((PinMuxFunMask[GPIONO]) && (1 << PINMUXFUNMASK))

#else
/** @defgroup GPIO_PINMUX_FUNCTION_MASK
 *  @{
 */
#define IS_GPIO_PINMUXFUNMASK(GPIONO, PINMUXFUNMASK)           1

#endif

/*@} end of group GPIO_PINMUX_FUNCTION_MASK */

/*@} end of group PINMUX_Private_Variables */

/** @defgroup PINMUX_Global_Variables
 *  @{
 */

/*@} end of group PINMUX_Global_Variables */

/** @defgroup PINMUX_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group PINMUX_Private_FunctionDeclaration */

/** @defgroup PINMUX_Private_Functions
 *  @{
 */

/*@} end of group PINMUX_Private_Functions */

/** @defgroup PINMUX_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      GPIO pinmux function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  PinMuxFun:  GPIO pin function, should be GPIO_PinMuxFunc_Type
 *
 * @return Status: DERROR or DSUCCESS
 *
 *******************************************************************************/
void GPIO_PinMuxFun(GPIO_NO_Type gpioNo, GPIO_PinMuxFunc_Type PinMuxFun)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_PINMUXFUN(PinMuxFun));
  CHECK_PARAM(IS_GPIO_PINMUXFUNMASK(gpioNo, PinMuxFun));
  
  /* When function to be set is not GPIO function, clear PIO_PULL_SEL_R to make the pin to default mode */
  /* For GPIO20~26 and GPIO57/58, function 1 is GPIO function, for other GPIOs, function 0 is GPIO function */
  if(((gpioNo>=GPIO_20)&&(gpioNo<=GPIO_26))||(gpioNo==GPIO_57)||(gpioNo==GPIO_58))
  {
    if(PinMuxFun==PINMUX_FUNCTION_1)
    {
      PINMUX->GPIO_PINMUX[gpioNo].BF.FSEL_XR = (PinMuxFun & 0x07);
    }
    else
    {
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.FSEL_XR = (PinMuxFun & 0x07); 
    }
  }
  else
  {
    if(PinMuxFun==PINMUX_FUNCTION_0)
    {
      PINMUX->GPIO_PINMUX[gpioNo].BF.FSEL_XR = (PinMuxFun & 0x07);
    }
    else
    {
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.FSEL_XR = (PinMuxFun & 0x07); 
    }
  }
}

/****************************************************************************//**
 * @brief      GPIO pin mode function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  gpioPinMode:  GPIO pin mode, should be PINMODE_DEFAULT, PINMODE_PULLUP, 
 *                           PINMODE_PULLDOWN, PINMODE_NOPULL or PINMODE_TRISTATE.
 *                           when this pin is not configured as GPIO function, 
 *                           or the data transfer direction is not input,
 *                           PINMODE_PULLUP, PINMODE_PULLDOWN or PINMODE_TRISTATE has no use.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_PinModeConfig(GPIO_NO_Type gpioNo, GPIO_PINMODE_Type gpioPinMode)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_PINMODE(gpioPinMode));
 
  switch(gpioPinMode)
  {
    case PINMODE_DEFAULT:
      /* Default */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 0;
      break;
      
    case PINMODE_PULLUP:
      /* Pullup */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLUP_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLDN_R = 0;	  
      break;

    case PINMODE_PULLDOWN:
      /* Pulldown */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLUP_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLDN_R = 1;	
      break;

	 case PINMODE_NOPULL:
      /* Nopull */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLUP_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLDN_R = 0;	
      break;

    case PINMODE_TRISTATE:
      /* Tristate */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 0;                                                                        
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLUP_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLDN_R = 0;	
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      GPIO Serial Flash HOLDn Configuration define
 *
 * @param[in]  sflashMode: should be MODE_SHUTDOWN or MODE_DEFAULT
 *
 * @return none
 *
 *******************************************************************************/
void SFLASH_HOLDnConfig(SFLASH_MODE_Type sflashMode)
{
  CHECK_PARAM(IS_SFLASH_MODE(sflashMode));

  switch(sflashMode)
  {
    case MODE_SHUTDOWN:
      /* Shutdown */
      PINMUX->SFLASH_HOLDn.BF.DI_EN = 0;
      break;

    case MODE_DEFAULT:
      /* Default */
      PINMUX->SFLASH_HOLDn.BF.DI_EN = 1;
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      GPIO Serial Flash DIO Configuration define
 *
 * @param[in]  sflashMode: should be MODE_SHUTDOWN or MODE_DEFAULT
 *
 * @return none
 *
 *******************************************************************************/
void SFLASH_DIOConfig(SFLASH_MODE_Type sflashMode)
{
  CHECK_PARAM(IS_SFLASH_MODE(sflashMode));

  switch(sflashMode)
  {
    case MODE_SHUTDOWN:
      /* Shutdown */
      PINMUX->SFLASH_DIO.BF.DI_EN = 0;
      break;

    case MODE_DEFAULT:
      /* Default */
      PINMUX->SFLASH_DIO.BF.DI_EN = 1;
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      GPIO Serial Flash Write Protect Configuration define
 *
 * @param[in]  sflashMode: should be MODE_SHUTDOWN or MODE_DEFAULT
 *
 * @return none
 *
 *******************************************************************************/
void SFLASH_WPConfig(SFLASH_MODE_Type sflashMode)
{
  switch(sflashMode)
  {
    case MODE_SHUTDOWN:
      /* Shutdown */
      PINMUX->SFLASH_WP.BF.DI_EN = 0;
      break;

    case MODE_DEFAULT:
      /* Default */
      PINMUX->SFLASH_WP.BF.DI_EN = 1;
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      GPIO Serial Flash DO Configuration define
 *
 * @param[in]  sflashMode: should be MODE_SHUTDOWN or MODE_DEFAULT
 *
 * @return none
 *
 *******************************************************************************/
void SFLASH_DOConfig(SFLASH_MODE_Type sflashMode)
{
  switch(sflashMode)
  {
    case MODE_SHUTDOWN:
      /* Shutdown */
      PINMUX->SFLASH_DO.BF.DI_EN = 0;
      break;

    case MODE_DEFAULT:
      /* Default */
      PINMUX->SFLASH_DO.BF.DI_EN = 1;
      break;

    default:
      break;
  }
}
/*@} end of group PINMUX_Public_Functions */

/*@} end of group PINMUX_definitions */

/*@} end of group MC200_Periph_Driver */
