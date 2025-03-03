/****************************************************************************//**
 * @file     mc200_dma.h
 * @brief    DMA driver module header file.
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

#ifndef __MC200_DMA_H__
#define __MC200_DMA_H__

#include "mc200.h"
#include "mc200_driver.h"

/** @addtogroup MC200_Periph_Driver
 *  @{
 */

/** @addtogroup DMA 
 *  @{
 */
  
/** @defgroup DMA_Public_Types DMA_Public_Types 
 *  @{
 */

/**  
 *  @brief DMA Channel number type definition 
 */ 
typedef enum
{
  CHANNEL_0 = 0,                           /*!< Channel 0 define */
  CHANNEL_1,                               /*!< Channel 1 define */
  CHANNEL_2,                               /*!< Channel 2 define */
  CHANNEL_3,                               /*!< Channel 3 define */
  CHANNEL_4,                               /*!< Channel 4 define */
  CHANNEL_5,                               /*!< Channel 5 define */ 
  CHANNEL_6,                               /*!< Channel 6 define */ 
  CHANNEL_7,                               /*!< Channel 7 define */ 
}DMA_Channel_Type;

/**  
 *  @brief DMA handshaking mapping type definition 
 */ 
typedef enum
{
  DMA_HS0_AES_CRC_IN = 0,                  /*!< HS0 AES CRC IN mapping */
  DMA_HS0_UART0_RX,                        /*!< HS0 UART0 RX mapping */
  DMA_HS1_AES_CRC_OUT = 0x10,              /*!< HS1 AES CRC OUT mapping */
  DMA_HS1_UART1_RX,                        /*!< HS1 UART1 RX mapping */
  DMA_HS2_UART2_RX = 0x20,                 /*!< HS2 UART2 RX mapping */
  DMA_HS2_QSPI0_TX,                        /*!< HS2 QSPI0 TX mapping */
  DMA_HS3_GPT0_0 = 0x30,                   /*!< HS3 GPT0 CH0 mapping */
  DMA_HS3_UART3_RX,                        /*!< HS3 UART3 RX mapping */
  DMA_HS3_SSP1_RX,                         /*!< HS3 SSP1 RX mapping */
  DMA_HS4_UART0_TX = 0x40,                 /*!< HS4 UART0 TX mapping */
  DMA_HS4_SSP2_RX,                         /*!< HS4 SSP2 RX mapping */
  DMA_HS5_GPT0_1 = 0x50,                   /*!< HS5 GPT0 CH1 mapping */
  DMA_HS5_UART1_TX,                        /*!< HS5 UART1 TX mapping */
  DMA_HS5_QSPI0_RX,                        /*!< HS5 QSPI0 RX mapping */
  DMA_HS6_DAC_CH_A = 0x60,                 /*!< HS6 DAC CH A mapping */
  DMA_HS6_UART2_TX,                        /*!< HS6 UART2 TX mapping */
  DMA_HS6_I2C1_RX,                         /*!< HS6 I2C1 RX mapping */
  DMA_HS7_DAC_CH_B = 0x70,                 /*!< HS7 DAC CH B mapping */
  DMA_HS7_UART3_TX,                        /*!< HS7 UART3 TX mapping */
  DMA_HS7_SSP1_TX,                         /*!< HS7 SSP1 TX mapping */
  DMA_HS8_ADC0 = 0x80,                     /*!< HS8 ADC0 mapping */
  DMA_HS8_I2C2_RX,                         /*!< HS8 I2C2 RX mapping */
  DMA_HS9_ADC1 = 0x90,                     /*!< HS9 ADC1 mapping */
  DMA_HS9_SSP0_RX,                         /*!< HS9 SSP0 RX mapping */
  DMA_HS10_GPT1_0 = 0xA0,                  /*!< HS10 GPT1 CH0 mapping */
  DMA_HS10_SSP0_TX,                        /*!< HS10 SSP0 TX mapping */
  DMA_HS11_GPT1_1 = 0xB0,                  /*!< HS11 GPT1 CH1 mapping */
  DMA_HS11_SSP2_TX,                        /*!< HS11 SSP2 TX mapping */
  DMA_HS12_GPT2_0 = 0xC0,                  /*!< HS12 GPT2 CH0 mapping */
  DMA_HS12_I2C1_TX,                        /*!< HS12 I2C1 TX mapping */
  DMA_HS12_UART0_RX,                       /*!< HS12 UART0 RX mapping */
  DMA_HS12_QSPI1_RX,                       /*!< HS12 QSPI1 RX mapping */
  DMA_HS13_GPT2_1 = 0xD0,                  /*!< HS13 GPT2 CH1 mapping */
  DMA_HS13_I2C2_TX,                        /*!< HS13 I2C2 TX mapping */
  DMA_HS13_UART1_RX,                       /*!< HS13 UART1 RX mapping */
  DMA_HS13_QSPI1_TX,                       /*!< HS13 QSPI1 TX mapping */
  DMA_HS14_GPT3_0 = 0xE0,                  /*!< HS14 GPT3 CH0 mapping */
  DMA_HS14_I2C0_RX,                        /*!< HS14 I2C0 RX mapping */
  DMA_HS15_GPT3_1 = 0xF0,                  /*!< HS15 GPT3 CH1 mapping */
  DMA_HS15_I2C0_TX,                        /*!< HS15 I2C0 TX mapping */
}DMA_HsMapping_Type;

/**  
 *  @brief DMA transfer type definition 
 */ 
typedef enum
{
  DMA_MEM_TO_MEM = 0,                      /*!< Memory to Memory */
  DMA_MEM_TO_PER,                          /*!< Memory to peripheral */
  DMA_PER_TO_MEM,                          /*!< Peripheral to memory */
  DMA_PER_TO_PER,                          /*!< Peripheral to peripheral */ 
}DMA_TransfType_Type;

/**  
 *  @brief DMA transfer burst length definition 
 */ 
typedef enum
{
  DMA_ITEM_1 = 0,                          /*!< 1 item */
  DMA_ITEM_4,                              /*!< 4 items */
  DMA_ITEM_8,                              /*!< 8 items */
  DMA_ITEM_16,                             /*!< 16 items */ 
}DMA_BurstLength_Type;

/**  
 *  @brief DMA transfer address increment definition 
 */ 
typedef enum
{
  DMA_ADDR_INC = 0,                        /*!< Address increment */
  DMA_ADDR_DEC,                            /*!< Address decrement */
  DMA_ADDR_NOCHANGE,                       /*!< Address nochange */
}DMA_AddrInc_Type;

/**  
 *  @brief DMA transfer width definition 
 */ 
typedef enum
{
  DMA_TRANSF_WIDTH_8 = 0,                  /*!< 8 bits */
  DMA_TRANSF_WIDTH_16,                     /*!< 16 bits */
  DMA_TRANSF_WIDTH_32,                     /*!< 32 bits */
}DMA_TransfWidth_Type;

/**  
 *  @brief DMA transfer handshaking type definition 
 */ 
typedef enum
{
  DMA_HW_HANDSHAKING,                      /*!< Hardware handshaking interface */
  DMA_SW_HANDSHAKING,                      /*!< Software handshaking interface */
}DMA_Handshaking_Type;

/**  
 *  @brief DMA Channel priority type definition 
 */ 
typedef enum
{
  DMA_CH_PRIORITY_0,                       /*!< Channel priority level 0 */
  DMA_CH_PRIORITY_1,                       /*!< Channel priority level 1 */
  DMA_CH_PRIORITY_2,                       /*!< Channel priority level 2 */
  DMA_CH_PRIORITY_3,                       /*!< Channel priority level 3 */
  DMA_CH_PRIORITY_4,                       /*!< Channel priority level 4 */
  DMA_CH_PRIORITY_5,                       /*!< Channel priority level 5 */
  DMA_CH_PRIORITY_6,                       /*!< Channel priority level 6 */
  DMA_CH_PRIORITY_7,                       /*!< Channel priority level 7 */
}DMA_ChPriority_Type;

/**  
 *  @brief DMA hardware handshaking interface definition 
 */ 
typedef enum
{
  DMA_HW_HS_INTER_0,                       /*!< hardware handshaking interface 0 */
  DMA_HW_HS_INTER_1,                       /*!< hardware handshaking interface 1 */
  DMA_HW_HS_INTER_2,                       /*!< hardware handshaking interface 2 */
  DMA_HW_HS_INTER_3,                       /*!< hardware handshaking interface 3 */
  DMA_HW_HS_INTER_4,                       /*!< hardware handshaking interface 4 */
  DMA_HW_HS_INTER_5,                       /*!< hardware handshaking interface 5 */
  DMA_HW_HS_INTER_6,                       /*!< hardware handshaking interface 6 */
  DMA_HW_HS_INTER_7,                       /*!< hardware handshaking interface 7 */
  DMA_HW_HS_INTER_8,                       /*!< hardware handshaking interface 8 */
  DMA_HW_HS_INTER_9,                       /*!< hardware handshaking interface 9 */
  DMA_HW_HS_INTER_10,                      /*!< hardware handshaking interface 10 */
  DMA_HW_HS_INTER_11,                      /*!< hardware handshaking interface 11 */
  DMA_HW_HS_INTER_12,                      /*!< hardware handshaking interface 12 */
  DMA_HW_HS_INTER_13,                      /*!< hardware handshaking interface 13 */
  DMA_HW_HS_INTER_14,                      /*!< hardware handshaking interface 14 */
  DMA_HW_HS_INTER_15,                      /*!< hardware handshaking interface 15 */
}DMA_HwHsInter_Type;

/**  
 *  @brief DMA FIFO	mode definition 
 */ 
typedef enum
{
  DMA_FIFO_MODE_0,                         /*!< Space/data available for single AHB transfer of the specified transfer width */
  DMA_FIFO_MODE_1,                         /*!< Data available is greater than or equal to half the FIFO depth for destination
                                                transfers and space available is greater than half the FIFO depth for source 
                                                transfer. The execptions are at the end of a burst transaction request or at the 
                                                end of a block transfer */
}DMA_FIFOMode_Type;

/**  
 *  @brief DMA Configuration Structure type definition 
 */  
typedef struct
{
  uint32_t srcDmaAddr;                     /*!< Source address of DMA transfer */

  uint32_t destDmaAddr;                    /*!< Destination address of DMA transfer */

  DMA_TransfType_Type transfType;          /*!< Transfer type and flow control
                                                0: Memory to Memory
                                                1: Memory to peripheral
                                                2: Peripheral to memory
                                                3: Peripheral to peripheral */
                                            
  DMA_BurstLength_Type srcBurstLength;     /*!< Number of data items for source burst transaction length.
                                                Each item width is as same as source tansfer width.  
                                                0: 1 item
                                                1: 4 items
                                                2: 8 items
                                                3: 16 items */
                                          
  DMA_BurstLength_Type destBurstLength;    /*!< Number of data items for destination burst transaction length.
                                                Each item width is as same as destination tansfer width.  
                                                0: 1 item
                                                1: 4 items
                                                2: 8 items
                                                3: 16 items */

  DMA_AddrInc_Type srcAddrInc;             /*!< Source address increment 
                                                0: Increment
                                                1: Decrement
                                                2: No change
                                                3: No change */
                                         
  DMA_AddrInc_Type destAddrInc;            /*!< Destination address increment 
                                                0: Increment
                                                1: Decrement
                                                2: No change
                                                3: No change */

  DMA_TransfWidth_Type srcTransfWidth;     /*!< Source transfer width 
                                                0: 8   bits
                                                1: 16  bits
                                                2: 32  bits
                                                3: 64  bits
                                                4: 128 bits
                                                >=5: 256 bits */
                                         
  DMA_TransfWidth_Type destTransfWidth;    /*!< Destination transfer width 
                                                0: 8   bits
                                                1: 16  bits
                                                2: 32  bits
                                                3: 64  bits
                                                4: 128 bits
                                                >=5: 256 bits */   
                                         
  DMA_Handshaking_Type srcSwHwHdskSel;     /*!< Source software or hardware handshaking select
                                                0: Hardware handshaking interface
                                                1: Software handshaking interface
                                                If the source peripheral is memory, then this member is ignored */

  DMA_Handshaking_Type destSwHwHdskSel;    /*!< Destination software or hardware handshaking select
                                                0: Hardware handshaking interface
                                                1: Software handshaking interface
                                                If the Destination peripheral is memory, then this member is ignored */                                        

  DMA_ChPriority_Type channelPriority;     /*!< Channel priority
                                                A priority of 7 is the highest priority, and 0 is the lowest
                                                The value must be programed within the following range: 0:7 */
                                        
  DMA_HwHsInter_Type srcHwHdskInterf;      /*!< Assigns a hardware handshaking interface(0-15) to
                                                the source of channelx if source hardware handshaking select */

  DMA_HwHsInter_Type destHwHdskInterf;     /*!< Assigns a hardware handshaking interface(0-15) to
                                                the destination of channelx if destination hardware handshaking select.*/
                                            
  DMA_FIFOMode_Type fifoMode;              /*!< Determine how much space or data need to be available in the FIFO before a 
                                                burst transaction reques is serviced.
                                                0: Space/data available for single AHB transfer of the specified transfer width
                                                1: Data available is greater than or equal to half the FIFO depth for destination
                                                   transfers and space available is greater than half the FIFO depth for source 
                                                   transfer. The execptions are at the end of a burst transaction request or at the 
                                                   end of a block transfer */ 
}DMA_CFG_Type;

/**  
 *  @brief DMA source/destination type definition 
 */
typedef enum
{
  DMA_SOURCE,	                           /*!< Dma source transaction direction */
  DMA_DESTINATION,                         /*!< Dma destination transaction direction */
}DMA_TransDir_Type;

/**  
 *  @brief DMA transaction mode type definition 
 */
typedef enum
{
  DMA_SINGLE,	                           /*!< Single transaction */
  DMA_BURST,	                           /*!< Burst transaction */
}DMA_TransMode_Type;

/**  
 *  @brief DMA interrupt type definition 
 */
typedef enum
{
  INT_DMA_TRANS_COMPLETE,                  /*!< Dma transfer complete interrupt */
  INT_BLK_TRANS_COMPLETE,                  /*!< Block transfer complete interrupt */
  INT_SRC_TRANS_COMPLETE,                  /*!< Source transfer complete interrupt */
  INT_DEST_TRANS_COMPLETE,                 /*!< Destination transfer complete interrupt */
  INT_ERROR,                               /*!< Error interrupt */
  INT_CH_ALL,                              /*!< All interrupts for channelx */                 
}DMA_Int_Type; 

/*@} end of group DMA_Public_Types */

/** @defgroup DMA_Public_Constants
 *  @{
 */ 
                                      
/** @defgroup DMA_Channel        
 *  @{
 */
#define IS_DMA_CHANNEL(CHANNEL)        ((CHANNEL) <= 7)

/*@} end of group DMA_Channel */

/** @defgroup DMA_Tansfer_Direction       
 *  @{
 */
#define IS_DMA_TRANS_DIR(DIR)          (((DIR) == DMA_SOURCE) || ((DIR) == DMA_DESTINATION))
                                           
/*@} end of group DMA_Tansfer_Direction */

/** @defgroup DMA_Tansfer_Mode       
 *  @{
 */
#define IS_DMA_TRANS_MODE(MODE)        (((MODE) == DMA_SINGLE) || ((MODE) == DMA_BURST))
                                           
/*@} end of group DMA_Tansfer_Mode */

/** @defgroup DMA_Tansfer_Block_Size       
 *  @{
 */
#define IS_DMA_BLK_SIZE(BLKSIZE)        ((BLKSIZE) < 1024)

/*@} end of group DMA_Tansfer_Block_Size */

/** @defgroup DMA_Int_Type       
 *  @{
 */
#define IS_DMA_INT(INT)                (((INT) == INT_DMA_TRANS_COMPLETE)  || \
                                        ((INT) == INT_BLK_TRANS_COMPLETE)  || \
                                        ((INT) == INT_SRC_TRANS_COMPLETE)  || \
                                        ((INT) == INT_DEST_TRANS_COMPLETE) || \
                                        ((INT) == INT_ERROR)               || \
                                        ((INT) == INT_CH_ALL))                                           
/*@} end of group DMA_Int_Type */

/*@} end of group DMA_Public_Macro */

/** @defgroup DMA_Public_FunctionDeclaration
 *  @brief DMA functions declaration
 *  @{
 */
void DMA_ChannelInit(DMA_Channel_Type channelx, DMA_CFG_Type * dmaCfgStruct);
void DMA_SetHandshakingMapping(DMA_HsMapping_Type hsMapping);

void DMA_Enable(void);
void DMA_Disable(void);
void DMA_ChannelCmd(DMA_Channel_Type channelx, FunctionalState newState);
void DMA_SetTransfDataLength(DMA_Channel_Type channelx, uint32_t length);
void DMA_SetBlkTransfSize(DMA_Channel_Type channelx, uint32_t blkSize);
void DMA_SoftwareTransReq(DMA_Channel_Type channelx, DMA_TransDir_Type dir, DMA_TransMode_Type transMode);
void DMA_ChannelSuspend(DMA_Channel_Type channelx, FunctionalState newState);

void DMA_ChannelIntConfig(DMA_Channel_Type channelx, FunctionalState newState);
void DMA_IntMask(DMA_Channel_Type channelx, DMA_Int_Type intType, IntMask_Type intMask);
FlagStatus DMA_GetChannelIntStatus(DMA_Channel_Type channelx, DMA_Int_Type intType);
FlagStatus DMA_GetIntStatus(DMA_Int_Type intType);
void DMA_IntClr(DMA_Channel_Type channelx, DMA_Int_Type intType);
void DMA_IRQHandler(void);
void DMA_SetSourceAddr(DMA_Channel_Type channelx, uint32_t srcDmaAddr);
void DMA_SetDestAddr(DMA_Channel_Type channelx,uint32_t destDmaAddr);
INT_Peripher_Type DMA_Channel_Int_Type(DMA_Channel_Type channelx);


/*@} end of group DMA_Public_FunctionDeclaration */

/*@} end of group DMA */

/*@} end of group MC200_Periph_Driver */
#endif /* __MC200_DMA_H__ */
