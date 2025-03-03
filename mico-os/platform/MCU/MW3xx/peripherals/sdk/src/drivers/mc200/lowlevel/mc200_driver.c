/****************************************************************************//**
 * @file     mc200_driver.c
 * @brief    This file provides MC200 driver common functions.
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

#include "mc200_driver.h"
#include "mc200.h"

/** @addtogroup  MC200_Periph_Driver
 *  @{
 */

/** @defgroup DRIVER_COMMON DRIVER_COMMON
 *  @brief Digger driver common functions
 *  @{
 */

/** @defgroup DRIVER_Private_Type
 *  @{
 */


/*@} end of group DRIVER_Private_Type*/

/** @defgroup DRIVER_Private_Defines
 *  @{
 */

/*@} end of group DRIVER_Private_Defines */

/** @defgroup DRIVER_Private_Variables
 *  @{
 */                               
  		      
/*@} end of group DRIVER_Private_Variables */

/** @defgroup DRIVER_Global_Variables
 *  @{
 */


intCallback_Type * extpin0IntCb[]= {NULL};

intCallback_Type * extpin1IntCb[]= {NULL};

/**  
 *  @brief RTC interrupt callback function pointer array
 */
intCallback_Type * rtcIntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL};

/**  
 *  @brief CRC interrupt callback function pointer array
 */
intCallback_Type * crcIntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL};

/**  
 *  @brief AES interrupt callback function pointer array
 */
intCallback_Type * aesIntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL};

/**  
 *  @brief I2C interrupt callback function pointer array
 */
intCallback_Type * i2c0IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * i2c1IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * i2c2IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief DMA interrupt callback function pointer array
 */
intCallback_Type * dmaCh0IntCb[]= {NULL, NULL, NULL, NULL, NULL};
intCallback_Type * dmaCh1IntCb[]= {NULL, NULL, NULL, NULL, NULL};
intCallback_Type * dmaCh2IntCb[]= {NULL, NULL, NULL, NULL, NULL};
intCallback_Type * dmaCh3IntCb[]= {NULL, NULL, NULL, NULL, NULL};
intCallback_Type * dmaCh4IntCb[]= {NULL, NULL, NULL, NULL, NULL};
intCallback_Type * dmaCh5IntCb[]= {NULL, NULL, NULL, NULL, NULL};
intCallback_Type * dmaCh6IntCb[]= {NULL, NULL, NULL, NULL, NULL};
intCallback_Type * dmaCh7IntCb[]= {NULL, NULL, NULL, NULL, NULL};


/**  
 *  @brief SSP interrupt callback function pointer array
 */
intCallback_Type * ssp0IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * ssp1IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * ssp2IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief QSPI interrupt callback function pointer array
 */
intCallback_Type * qspi0IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * qspi1IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief GPT interrupt callback function pointer array
 */
intCallback_Type * gpt0IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * gpt1IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * gpt2IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * gpt3IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief UART interrupt callback function pointer array
 */
intCallback_Type * uart0IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * uart1IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * uart2IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * uart3IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief WDT interrupt callback function pointer array
 */
intCallback_Type * wdtIntCb[]= {NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief ADC interrupt callback function pointer array
 */
intCallback_Type * adc0IntCb[]= {NULL, NULL, NULL, NULL, NULL};
intCallback_Type * adc1IntCb[]= {NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief DAC interrupt callback function pointer array
 */
intCallback_Type * dacIntCb[]= {NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief COMP interrupt callback function pointer array
 */
intCallback_Type * acompIntCb[]= {NULL, NULL, NULL, NULL};



/**  
 *  @brief SDIO interrupt callback function pointer array
 */
intCallback_Type * sdioIntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief USB interrupt callback function pointer array
 */
intCallback_Type * usbIntCb[]= {NULL, NULL, NULL};

/**  
 *  @brief RC32M interrupt callback function pointer array
 */
intCallback_Type * rc32mIntCb[]= {NULL, NULL};

/**  
 *  @brief MPU interrupt callback function pointer array
 */
intCallback_Type * mpuIntCb[]= {NULL, NULL, NULL, NULL};

/**  
 *  @brief Digger interrupt callback function pointer array
 */
intCallback_Type ** intCbfArra[] = {extpin0IntCb, extpin1IntCb,
rtcIntCb, crcIntCb, aesIntCb, i2c0IntCb, i2c1IntCb, i2c2IntCb, 
                                    dmaCh0IntCb, dmaCh1IntCb, dmaCh2IntCb, dmaCh3IntCb, dmaCh4IntCb, dmaCh5IntCb, dmaCh6IntCb, dmaCh7IntCb, 
                                   NULL,ssp0IntCb, ssp1IntCb, ssp2IntCb, qspi0IntCb, qspi1IntCb,
                                    gpt0IntCb, gpt1IntCb, gpt2IntCb, gpt3IntCb, uart0IntCb, uart1IntCb, uart2IntCb, uart3IntCb, 
                                    wdtIntCb, adc0IntCb, adc1IntCb, dacIntCb, 
                                    acompIntCb, sdioIntCb, usbIntCb, rc32mIntCb, mpuIntCb};  

/*@} end of group DRIVER_Global_Variables */

/** @defgroup DRIVER_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group DRIVER_Private_FunctionDeclaration */

/** @defgroup DRIVER_Private_Functions
 *  @{
 */

/*@} end of group DRIVER_Private_Functions */

/** @defgroup DRIVER_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Install interrupt callback function for given interrupt type and peripheral 
 *
 * @param[in]  intPeriph:  Select the peripheral, such as INT_UART1,INT_AES
 * @param[in]  intType: Specifies the interrupt type. must be select interrupt enum type 
 *             defined as XXXX_INT_Type(XXXX here is peripher name) in mc200_xxx.h. 
               There is a exception for GPIO,that interrupt type is the GPIO_NO_Type.
 * @param[in]  cbFun:  Pointer to interrupt callback function. The type should be 
 *             void (*fn)(void).
 *
 * @return none
 *
 *******************************************************************************/
void install_int_callback(INT_Peripher_Type intPeriph, uint32_t intType, intCallback_Type * cbFun)
{
  /* Check the parameters */
  CHECK_PARAM(IS_INT_PERIPH(intPeriph));

  intCbfArra[intPeriph][intType] = cbFun;
}

#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.

* @param[in]	file: Pointer to the source file name
* @param[in]    line: assert_param error line source number

* @return	none
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif /* DEBUG */

/*@} end of group DRIVER_Public_Functions */

/*@} end of group DRIVER_COMMON */

/*@} end of group MC200_Periph_Driver */

