/**
 ******************************************************************************
 * @file    CheckSumUtils.c
 * @author  William Xu
 * @version V1.0.0
 * @date    30-July-2015
 * @brief   This file contains function that aid in checksum calculations.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include "CheckSumUtils.h"



uint8_t UpdateCRC8(uint8_t crcIn, uint8_t byte)
{
  uint8_t crc = crcIn;
  uint8_t i;
 
  crc^= byte;
     
  for(i=0;i<8;i++)
  {
      if(crc&0x01)
      {
          crc = (crc>>1)^0x8C;                          
      }
      else
         crc>>=1;                  
  } 
  return crc; 
}


void CRC8_Init( CRC8_Context *inContext )
{
  inContext->crc = 0;
}


void CRC8_Update( CRC8_Context *inContext, const void *inSrc, size_t inLen )
{
  const uint8_t * src = (const uint8_t *) inSrc;
  const uint8_t * srcEnd = src + inLen;
  while( src < srcEnd )
    inContext->crc = UpdateCRC8(inContext->crc, *src++); 
}


void CRC8_Final( CRC8_Context *inContext, uint8_t *outResult )
{
    //inContext->crc = UpdateCRC8(inContext->crc, 0);
    *outResult = inContext->crc&0xffu;
}
 

/*******************************************************************************/

uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
  uint32_t crc = crcIn;
  uint32_t in = byte | 0x100;
  
  do
  {
    crc <<= 1;
    in <<= 1;  
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }while(!(in & 0x10000));
  return crc & 0xffffu;
}

void CRC16_Init( CRC16_Context *inContext )
{
  inContext->crc = 0;
}


void CRC16_Update( CRC16_Context *inContext, const void *inSrc, size_t inLen )
{
  const uint8_t * src = (const uint8_t *) inSrc;
  const uint8_t * srcEnd = src + inLen;
  while( src < srcEnd )
    inContext->crc = UpdateCRC16(inContext->crc, *src++);
}


void CRC16_Final( CRC16_Context *inContext, uint16_t *outResult )
{
  inContext->crc = UpdateCRC16(inContext->crc, 0);
  inContext->crc = UpdateCRC16(inContext->crc, 0);
  *outResult = inContext->crc&0xffffu;
}

