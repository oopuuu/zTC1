/**
 ******************************************************************************
 * @file    MICOBonjour.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   Zero-configuration protocol compatible with Bonjour from Apple
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mico.h"
#include "system_internal.h"

#include "platform_config.h"
#include "StringUtils.h"

OSStatus system_discovery_init( system_context_t * const inContext )
{
  char *temp_txt= NULL;
  char *temp_txt2;
  OSStatus err;
  net_para_st para;
  mdns_init_t init;

  temp_txt = malloc(500);
  require_action(temp_txt, exit, err = kNoMemoryErr);

  memset(&init, 0x0, sizeof(mdns_init_t));

  micoWlanGetIPStatus(&para, Station);

  init.service_name = "_easylink._tcp.local.";

  /*   name#xxxxxx.local.  */
  snprintf( temp_txt, 100, "%s.local.", inContext->flashContentInRam.micoSystemConfig.name);
  init.host_name = (char*)__strdup(temp_txt);

  /*   name#xxxxxx.   */
  snprintf( temp_txt, 100, "%s",        inContext->flashContentInRam.micoSystemConfig.name);
  init.instance_name = (char*)__strdup(temp_txt);

#ifndef MICO_LOCAL_SERVER_PORT
  init.service_port = MICO_CONFIG_SERVER_PORT;
#else
  init.service_port = MICO_LOCAL_SERVER_PORT;
#endif

  temp_txt2 = __strdup_trans_dot(inContext->micoStatus.mac);
  sprintf(temp_txt, "MAC=%s.", temp_txt2);
  free(temp_txt2);

  temp_txt2 = __strdup_trans_dot(FIRMWARE_REVISION);
  sprintf(temp_txt, "%sFirmware Rev=%s.", temp_txt, temp_txt2);
  free(temp_txt2);
  
  temp_txt2 = __strdup_trans_dot(HARDWARE_REVISION);
  sprintf(temp_txt, "%sHardware Rev=%s.", temp_txt, temp_txt2);
  free(temp_txt2);

  temp_txt2 = __strdup_trans_dot(MicoGetVer());
  sprintf(temp_txt, "%sMICO OS Rev=%s.", temp_txt, temp_txt2);
  free(temp_txt2);

  temp_txt2 = __strdup_trans_dot(MODEL);
  sprintf(temp_txt, "%sModel=%s.", temp_txt, temp_txt2);
  free(temp_txt2);

  temp_txt2 = __strdup_trans_dot(PROTOCOL);
  sprintf(temp_txt, "%sProtocol=%s.", temp_txt, temp_txt2);
  free(temp_txt2);

  temp_txt2 = __strdup_trans_dot(MANUFACTURER);
  sprintf(temp_txt, "%sManufacturer=%s.", temp_txt, temp_txt2);
  free(temp_txt2);
  
  sprintf(temp_txt, "%sSeed=%lu.", temp_txt, inContext->flashContentInRam.micoSystemConfig.seed);
  init.txt_record = (char*)__strdup(temp_txt);

  mdns_add_record( init, Station, 1500);

  free(init.host_name);
  free(init.instance_name);
  free(init.txt_record);
  
exit:
  if(temp_txt) free(temp_txt);
  return err;
}
