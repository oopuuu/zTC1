/****************************************************************************//**
 * @file     mw300_mpu.c
 * @brief    This file provides MPU functions.
 * @version  V1.3.0
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

#include "mw300.h"
#include "mw300_mpu.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup MPU MPU
 *  @brief MPU driver modules
 *  @{
 */

/** @defgroup MPU_Private_Type
 *  @{
 */

/*@} end of group MPU_Private_Type*/

/** @defgroup MPU_Private_Defines
 *  @{
 */

/*@} end of group MPU_Private_Defines */


/** @defgroup MPU_Private_Variables
 *  @{
 */


/*@} end of group MPU_Private_Variables */

/** @defgroup MPU_Global_Variables
 *  @{
 */


/*@} end of group MPU_Global_Variables */


/** @defgroup MPU_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group MPU_Private_FunctionDeclaration */

/** @defgroup MPU_Private_Functions
 *  @{
 */


/*@} end of group MPU_Private_Functions */

/** @defgroup MPU_Public_Functions
 *  @{
 */

 
/****************************************************************************//**
 * @brief      Enable the MPU 
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_Enable(void)
{
  MPU->CTRL |= 0x01;
}

/****************************************************************************//**
 * @brief      Disable MPU 
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_Disable(void)
{
  MPU->CTRL &= 0xfffffffe;
}
/****************************************************************************//**
 * @brief      Enable the MPU region
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_RegionEnable(MPU_RegionNo_Type regionNo)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR = regionNo; 
  
  MPU->RASR |= 0x01;
}

/****************************************************************************//**
 * @brief      Disable the MPU region
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_RegionDisable(MPU_RegionNo_Type regionNo)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR = regionNo; 
  
  MPU->RASR &= (~0x01);
}

/****************************************************************************//**
 * @brief      Disable the MPU subregion
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_SubregionDisable(MPU_RegionNo_Type regionNo, MPU_SubregionNo_Type subregionNo)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR = regionNo; 
  
  MPU->RASR |= (subregionNo << 8);
}


/****************************************************************************//**
 * @brief      Enable the MPU subregion
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_SubregionEnable(MPU_RegionNo_Type regionNo, MPU_SubregionNo_Type subregionNo)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR |= regionNo; 
  
  MPU->RASR &= (~(subregionNo << 8));
}
/****************************************************************************//**
 * @brief      Initialize the MPU 
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_Init(MPU_Config_Type* mpuConfig)
{
  if(ENABLE == mpuConfig->hardFaultNMIEnable)
  {
    MPU->CTRL |= 0x02;
  }
  if(DISABLE == mpuConfig->hardFaultNMIEnable)
  {
    MPU->CTRL &= 0xfffffffd;
  }
  
  if(ENABLE == mpuConfig->privDefMemEnable)
  {
    MPU->CTRL |= 0x04;
  }
  if(DISABLE == mpuConfig->privDefMemEnable)
  {
    MPU->CTRL &= 0xfffffffb;
  }
  
  
}



/****************************************************************************//**
 * @brief      Initialize the MPU 
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_RegionConfig(MPU_RegionNo_Type regionNo, MPU_RegionConfig_Type* regionConfig)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR = regionNo; 
  
  /* set base address */
  MPU->RBAR &= 0x1f;
  regionConfig->baseAddress &= (~0x1f);  
  MPU->RBAR |= regionConfig->baseAddress;
  
  /* set region size */
  MPU->RASR &= (~0x3e);
  MPU->RASR |= ((regionConfig->size) << 1);
  
  /* set data access permission */
  MPU->RASR &= (~(7 << 24));
  MPU->RASR |= ((regionConfig->accessPermission) << 24);
  
  /* instruction access */
  if(ENABLE == regionConfig->instructionAccess)
  {
    MPU->RASR &= (~(1 <<28));
  }
  
  if(DISABLE == regionConfig->instructionAccess)
  {
    MPU->RASR |= (1 <<28);
  }
}

/****************************************************************************//**
 * @brief      Enable the MPU interrupt
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_IntEnable(void)
{
  SCB->SHCSR |= 0x00010000;
}

/****************************************************************************//**
 * @brief      Disable the MPU interrupt
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_IntDisable(void)
{
  SCB->SHCSR &= (~(0x00010000));
}

/****************************************************************************//**
 * @brief       MPU interrupt handler
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MemManageException(void)
{
  uint32_t intStatus;  
  
  /* keep a copy of current interrupt status */
  intStatus = SCB->CFSR;
  intStatus &= 0xff;
  
  /* clear the generated interrupts */
  SCB->CFSR = intStatus;

  /* stacking error interrupt */
  if((intStatus & (0x00000010))!= 0x00)
  {
    if(intCbfArra[INT_MPU][MPU_INT_STACKING_ERR] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_MPU][MPU_INT_STACKING_ERR]();
    }
    else
    {
      SCB->SHCSR &= (~(1 << 16));
    }
  }
 
  /* unstacking interrupt */
  if((intStatus & (0x00000008))!= 0x00)
  {
    if(intCbfArra[INT_MPU][MPU_INT_UNSTACKING_ERR] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_MPU][MPU_INT_UNSTACKING_ERR]();
    }
    else
    {
      SCB->SHCSR &= (~(1 << 16));
    }    
  }  

  /* data access violation interrupt */
  if((intStatus & (0x00000002))!= 0x00)
  {
    if(intCbfArra[INT_MPU][MPU_INT_DATA_ACCESS_VIOL] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_MPU][MPU_INT_DATA_ACCESS_VIOL]();
    }
    else
    {
      SCB->SHCSR &= (~(1 << 16));
    }    
  }  
  
  /* instruction access violation interrupt */
  if((intStatus & (0x00000001))!= 0x00)
  {
    if(intCbfArra[INT_MPU][MPU_INT_INSTRUCION_ACCESS_VIOL] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_MPU][MPU_INT_INSTRUCION_ACCESS_VIOL]();
    }  
    else
    {
      SCB->SHCSR &= (~(1 << 16));
    }    
  } 
}
/*@} end of group MPU_Public_Functions */

/*@} end of group MPU  */

/*@} end of group MW300_Periph_Driver */

