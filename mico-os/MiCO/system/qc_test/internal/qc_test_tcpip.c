/**
 ******************************************************************************
 * @file    qc_test_tcpip.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Dec-2016
 * @brief   This file provide the TCPIP test functions, connect to an unsecured
 *          wlan according to user input and start tcp/udp test.
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
#include "qc_test_internal.h"

/******************************************************
*               Function Declarations
******************************************************/

extern int mfg_connect (char *ssid);  //TODO: Get it from MiCO core libraries
extern void mfg_option (int use_udp, uint32_t remoteaddr);  //TODO: Get it from MiCO core libraries

/******************************************************
*               Function Definitions
******************************************************/

MAY_BE_UNUSED static char * ssid_get(void)
{
  char *cmd;
  uint32_t remote_addr = 0xFFFFFFFF;

  while (1)  {                                 /* loop forever                */
    mf_printf ("\r\nMXCHIP_MFMODE> ");
    mf_get_line( &cmd );
    if (strncmp(cmd, "tcp ", 4) == 0) {
      mf_printf ("\r\n");
      remote_addr = inet_addr(cmd+4);
      if (remote_addr == 0)
        remote_addr = 0xffffffff;
      sprintf(cmd, "Use TCP send packet to 0x%X\r\n", (unsigned int)remote_addr);
      mf_printf (cmd);
    } else if (strncmp(cmd, "udp ", 4) == 0) {
      mf_printf ("\r\n");
      remote_addr = inet_addr(cmd+4);
      if (remote_addr == 0)
        remote_addr = 0xffffffff;
      sprintf(cmd, "Use UDP send packet to 0x%X\r\n", (unsigned int)remote_addr);
      mf_printf (cmd);
    }  else if (strncmp(cmd, "ssid ", 5) == 0) {
      mf_printf ("\r\n");
      return cmd+5;
    } else {
      mf_printf ("Please input as \"ssid <ssid_string>\"");
      continue;
    }
  }
}

void qc_test_tcpip(void)
{
    //mfg_connect (ssid_get());
}













