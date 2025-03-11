/**
 ******************************************************************************
 * @file    micokit_STmems.c
 * @author  William Xu
 * @version V1.0.0
 * @date    8-May-2015
 * @brief   micokit st mems extension board peripherals operations..
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mico_platform.h"
#include "MiCOKit_STmems.h"

#define micokit_STmems_log(M, ...) custom_log("MICOKIT_STMEMS", M, ##__VA_ARGS__)
#define micokit_STmems_log_trace() custom_log_trace("MICOKIT_STMEMS")

WEAK void micokit_STmems_key1_clicked_callback(void)
{

}

WEAK void micokit_STmems_key2_clicked_callback(void)
{
  
}

//------------------------------------- API ------------------------------------
OSStatus micokit_STmems_init(void)
{
  OSStatus err = kUnknownErr;
#if defined(MICOKIT_STMEMS_KEY1)||defined(MICOKIT_STMEMS_KEY2)
  button_init_t init;
#endif

  //init RGB LED(P9813)
  rgb_led_init();
  rgb_led_close();  // off
  
  dc_motor_init();
  dc_motor_set(0);   // off
  
  // init OLED
  OLED_Init();

  OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_1,  MODEL);
  OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_2, "MiCO            ");
  OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_3, "   Starting... ");
  OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, "                ");

#ifdef MICOKIT_STMEMS_KEY1
  init.gpio = MICOKIT_STMEMS_KEY1;
  init.pressed_func = micokit_STmems_key1_clicked_callback;
  init.long_pressed_func = NULL;
  init.long_pressed_timeout = 5000;
  button_init( IOBUTTON_USER_1, init);
#endif

#ifdef MICOKIT_STMEMS_KEY2
  init.gpio = MICOKIT_STMEMS_KEY2;
  init.pressed_func = micokit_STmems_key2_clicked_callback;
  init.long_pressed_func = NULL;
  init.long_pressed_timeout = 5000;
  button_init( IOBUTTON_USER_2, init);
#endif
    
  /*init HTS221 */
  err = hts221_sensor_init();
  require_noerr_string( err, exit, "ERROR: Unable to Init HTS221" );
  
  /*init UVIS25 */
  err = uvis25_sensor_init();
  require_noerr_string( err, exit, "ERROR: Unable to Init UVIS25" );
  
  /*init LSM9DS1_ACC_GYR */
  err = lsm9ds1_acc_gyr_sensor_init();
  require_noerr_string( err, exit, "ERROR: Unable to Init LSM9DS1_ACC_GYR" );
  
  err = lsm9ds1_mag_sensor_init();
  require_noerr_string( err, exit, "ERROR: Unable to Init LSM9DS1_MAG" );
  
  /*init LPS25HB */
  err = lps25hb_sensor_init();
  require_noerr_string( err, exit, "ERROR: Unable to Init LPS25HB" );
  
  light_sensor_init();
   
exit:
  return err;
}
