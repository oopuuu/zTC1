/**
 ******************************************************************************
 * @file    qc_test_cli.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Dec-2016
 * @brief   This file provide the command line function in QC test interface.
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */


#include "mico.h"
#include "platform.h"

/******************************************************
*               Variables Definitions
******************************************************/

static char cmd_str[64];

/******************************************************
*               Function Definitions
******************************************************/

void mf_printf(char *str)
{
  MicoUartSend( MFG_TEST, str, strlen(str));
}

void mf_putc(char ch)
{
  MicoUartSend( MFG_TEST, &ch, 1);
}

int mf_get_line( char** p_cmd_str )
{
#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
#define CR         0x0D
#define LF         0x0A

  char *p = cmd_str;
  *p_cmd_str = cmd_str;
  int i = 0;
  char c;

  memset(cmd_str, 0, sizeof(cmd_str));
  while(1) {
    if( MicoUartRecv( MFG_TEST, p, 1, 100) != kNoErr)
      continue;

    mf_putc(*p);
    if (*p == BACKSPACE  ||  *p == DEL)  {
      if(i>0) {
        c = 0x20;
        mf_putc(c);
        mf_putc(*p);
        p--;
        i--;
      }
      continue;
    }
    if(*p == CR || *p == LF) {
      *p = 0;
      return i;
    }

    p++;
    i++;
    if (i>sizeof(cmd_str))
      break;
  }

  return 0;
}














