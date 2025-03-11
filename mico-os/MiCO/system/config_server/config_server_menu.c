/**
 ******************************************************************************
 * @file    config_server_menu.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide function for creating configuration menu that can
 *          be displayed on EasyLink APP on iOS or Android.
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
#include "platform_config.h"

#include "json_c/json.h"

OSStatus config_server_create_sector(json_object* sectors, char* const name,  json_object *menus)
{
  OSStatus err;
  json_object *object;
  err = kNoErr;

  object = json_object_new_object();
  require_action(object, exit, err = kNoMemoryErr);
  json_object_object_add(object, "N", json_object_new_string(name));      
  json_object_object_add(object, "C", menus);
  json_object_array_add(sectors, object);

exit:
  return err;
}

OSStatus config_server_create_string_cell(json_object* menus, char* const name,  char* const content, char* const privilege, json_object* secectionArray)
{
  OSStatus err;
  json_object *object;
  err = kNoErr;

  object = json_object_new_object();
  require_action(object, exit, err = kNoMemoryErr);
  json_object_object_add(object, "N", json_object_new_string(name));      
  json_object_object_add(object, "C", json_object_new_string(content));
  json_object_object_add(object, "P", json_object_new_string(privilege)); 

  if(secectionArray)
    json_object_object_add(object, "S", secectionArray); 

  json_object_array_add(menus, object);


exit:
  return err;
}

OSStatus config_server_create_number_cell(json_object* menus, char* const name,  int content, char* const privilege, json_object* secectionArray)
{
  OSStatus err;
  json_object *object;
  err = kNoErr;

  object = json_object_new_object();
  require_action(object, exit, err = kNoMemoryErr);
  json_object_object_add(object, "N", json_object_new_string(name));      

  json_object_object_add(object, "C", json_object_new_int(content));
  json_object_object_add(object, "P", json_object_new_string(privilege)); 

  if(secectionArray)
    json_object_object_add(object, "S", secectionArray); 

  json_object_array_add(menus, object);

exit:
  return err;
}

OSStatus config_server_create_float_cell(json_object* menus, char* const name,  float content, char* const privilege, json_object* secectionArray)
{
  OSStatus err;
  json_object *object;
  err = kNoErr;

  object = json_object_new_object();
  require_action(object, exit, err = kNoMemoryErr);
  json_object_object_add(object, "N", json_object_new_string(name));      

  json_object_object_add(object, "C", json_object_new_double(content));
  json_object_object_add(object, "P", json_object_new_string(privilege)); 

  if(secectionArray)
    json_object_object_add(object, "S", secectionArray); 

  json_object_array_add(menus, object);

exit:
  return err;  
}


OSStatus config_server_create_bool_cell(json_object* menus, char* const name,  boolean switcher, char* const privilege)
{
  OSStatus err;
  json_object *object;
  err = kNoErr;

  object = json_object_new_object();
  require_action(object, exit, err = kNoMemoryErr);
  json_object_object_add(object, "N", json_object_new_string(name));      
  json_object_object_add(object, "C", json_object_new_boolean(switcher));
  json_object_object_add(object, "P", json_object_new_string(privilege)); 
  json_object_array_add(menus, object);

exit:
  return err;
}

OSStatus config_server_create_sub_menu_cell(json_object* menus, char* const name, json_object* lowerSectors)
{
  OSStatus err;
  json_object *object;
  err = kNoErr;

  object = json_object_new_object();
  require_action(object, exit, err = kNoMemoryErr);
  json_object_object_add(object, "N", json_object_new_string(name));
  json_object_object_add(object, "C", lowerSectors);
  json_object_array_add(menus, object);

exit:
  return err;
}
