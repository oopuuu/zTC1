/**
 ******************************************************************************
 * @file    micokit_ext.c
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    8-May-2015
 * @brief   micokit extension board manufacture test operations..
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
 ******************************************************************************
 */

#include "mico_platform.h"
#include "micokit_ext.h"
#include "mico_system.h"

#include "sensor/BME280/bme280_user.h"
#include "sensor/DHT11/DHT11.h"

extern mico_semaphore_t mfg_test_state_change_sem;
extern volatile int16_t mfg_test_module_number;

//---------------------------- user modules functions --------------------------

// Key1 clicked callback:  previous test module in test mode
WEAK void user_key1_clicked_callback( void )
{
    if ( NULL != mfg_test_state_change_sem )
    {
        if ( 0 < mfg_test_module_number )
        {
            mfg_test_module_number = (mfg_test_module_number - 1) % (MFG_TEST_MAX_MODULE_NUM + 1);
        }
        else
        {
            mfg_test_module_number = MFG_TEST_MAX_MODULE_NUM;
        }
        mico_rtos_set_semaphore( &mfg_test_state_change_sem );  // go back to previous module
    }
    return;
}

// Key2 clicked callback:  next test module in test mode
WEAK void user_key2_clicked_callback( void )
{
    if ( NULL != mfg_test_state_change_sem )
    {
        mfg_test_module_number = (mfg_test_module_number + 1) % (MFG_TEST_MAX_MODULE_NUM + 1);
        mico_rtos_set_semaphore( &mfg_test_state_change_sem );  // start next module
    }
    return;
}

//---------------------------- MFG TEST FOR EXT-BOARD --------------------------
#define mfg_test_oled_test_string    "abcdefghijklmnop123456789012345612345678901234561234567890123456"
#define OLED_MFG_TEST_PREFIX         "TEST:"

mico_semaphore_t mfg_test_state_change_sem = NULL;
volatile int16_t mfg_test_module_number = 0;
volatile bool scanap_done = false;
extern void mico_wlan_get_mac_address( uint8_t *mac );

static void mf_printf( char *str )
{
    OLED_Clear( );
    OLED_ShowString( 0, 0, (char*) str );
}

static void mf_printf_pos( uint8_t x, uint8_t y, char *str )
{
    //OLED_Clear();
    OLED_ShowString( x, y, (char*) str );
}

void mico_notify_WifiScanCompleteHandler( ScanResult *pApList, void * inContext )
{
    char str[32] = { '\0' };
    pApList->ApList[0].ssid[10] = '\0';  // truncate first 10 char
    sprintf( str, "SSID :%10s\r\nPOWER:%10d",
             pApList->ApList[0].ssid,
             pApList->ApList[0].ApPower );
    mf_printf_pos( 0, 4, str );
    scanap_done = true;
}

void micokit_ext_mfg_test( mico_Context_t *inContext )
{
    OSStatus err = kUnknownErr;
    char str[64] = { '\0' };
    uint8_t mac[6];

    int rgb_led_hue = 0;
    int dc_motor = 0;

    uint8_t dht11_ret = 0;
    uint8_t dht11_temp_data = 0;
    uint8_t dht11_hum_data = 0;
    int dht11_test_cnt = 0;

    int light_ret = 0;
    uint16_t light_sensor_data = 0;

    int infrared_ret = 0;
    uint16_t infrared_reflective_data = 0;

    bool bme280_on = false;
    int32_t bme280_temp = 0;
    uint32_t bme280_hum = 0;
    uint32_t bme280_press = 0;

    UNUSED_PARAMETER( inContext );

    UNUSED_PARAMETER( dht11_ret );
    UNUSED_PARAMETER( light_ret );
    UNUSED_PARAMETER( infrared_ret );

    mico_rtos_init_semaphore( &mfg_test_state_change_sem, 1 );
    err = mico_system_notify_register( mico_notify_WIFI_SCAN_COMPLETED, (void *) mico_notify_WifiScanCompleteHandler, inContext );
    require_noerr( err, exit );

    while ( 1 )
    {
        switch ( mfg_test_module_number )
        {
            case 0:  // mfg mode start
            {
                sprintf( str, "%s\r\nStart:\r\n%s\r\n%s", "TEST MODE", "  Next: KEY2", "  Prev: KEY1" );
                mf_printf( str );
                while ( kNoErr != mico_rtos_get_semaphore( &mfg_test_state_change_sem, MICO_WAIT_FOREVER ) )
                    ;
                break;
            }
            case 1:  // OUTPUT: OLED & RGB & DC_MOTOR
            {
                while ( kNoErr != mico_rtos_get_semaphore( &mfg_test_state_change_sem, 0 ) )
                {
                    // OLED display test info
                    sprintf( str, "%s OUTPUT\r\nOLED\r\nRGB_LED\r\nDC_MOTOR", OLED_MFG_TEST_PREFIX );
                    mf_printf( str );
                    mico_thread_msleep( 500 );

                    // RGB_LED
                    hsb2rgb_led_open( rgb_led_hue, 100, 50 );
                    rgb_led_hue += 120;
                    if ( rgb_led_hue >= 360 )
                    {
                        rgb_led_hue = 0;
                    }

                    // DC MOTOR
                    dc_motor = 1 - dc_motor;
                    dc_motor_set( dc_motor );

                    // OLED
                    mf_printf( mfg_test_oled_test_string );
                    mico_thread_msleep( 500 );
                }
                
                // exit, close modules
                dc_motor_set( 0 );
                hsb2rgb_led_open( 0, 0, 0 );
                OLED_Clear( );
                break;
            }
            case 2: // INPUT: infrared & light & T/H sensor
            {
                dht11_test_cnt = 0;
                bme280_on = false;
                while ( kNoErr != mico_rtos_get_semaphore( &mfg_test_state_change_sem, 0 ) )
                {
                    // infrared
                    infrared_ret = infrared_reflective_read( &infrared_reflective_data );

                    // light
                    light_ret = light_sensor_read( &light_sensor_data );

                    // display infared/light on OLED line1~3
                    sprintf( str, "%s INPUT\r\nInfrared: %6d\r\nLight: %9d",
                    OLED_MFG_TEST_PREFIX,
                             infrared_reflective_data, light_sensor_data );
                    mf_printf_pos( 0, 0, str );

                    // T/H
                    if ( dht11_test_cnt == 0 )
                    {   // DHT11 interval must >= 1s
                        // >>>DTH11
                        dht11_ret = DHT11_Read_Data( &dht11_temp_data, &dht11_hum_data );
                        // display T/H on OLED line4
                        sprintf( str, "Humiture:%2dC %2d%%", dht11_temp_data, dht11_hum_data );
                        mf_printf_pos( 0, 6, str );

                        // >>>BME280 check
                        if ( false == bme280_on )
                        {
                            err = bme280_sensor_init( );
                            if ( kNoErr == err )
                            {
                                bme280_on = true;
                            }
                        }

                        if ( bme280_on )
                        {
                            // if has BME280
                            err = bme280_data_readout( &bme280_temp, &bme280_press, &bme280_hum );
                            if ( kNoErr == err )
                            {
                                //sprintf(str, "%s BME280\r\nT: %3.1fC\r\nH: %3.1f%%\r\nP: %5.2fkPa", OLED_MFG_TEST_PREFIX,
                                //        (float)bme280_temp/100, (float)bme280_hum/1024, (float)bme280_press/1000);
                                // update T/H/P on OLED line4
                                sprintf( str, "%3.1f/%3.1f/%5.2f",
                                         (float) bme280_temp / 100,
                                         (float) bme280_hum / 1024, (float) bme280_press / 1000 );
                                mico_thread_sleep( 1 );
                                mf_printf_pos( 0, 6, "                " ); // clean line4
                                mf_printf_pos( 0, 6, str );
                            }
                        }
                    }
                    dht11_test_cnt++;
                    if ( 2 == dht11_test_cnt )
                    {
                        dht11_test_cnt = 0;
                    }

                    mico_thread_msleep( 500 );
                }
                break;
            }
            case 3: // wifi
            {
                wlan_get_mac_address( mac );
                sprintf( str, "%s Wi-Fi\r\nMAC:%02x%02x%02x%02x%02x%02x", OLED_MFG_TEST_PREFIX,
                         mac[0],
                         mac[1], mac[2], mac[3], mac[4], mac[5] );
                mf_printf( str );
                //mico_thread_msleep(500);

                scanap_done = false;
                micoWlanStartScan( );
                while ( (!scanap_done) || (kNoErr != mico_rtos_get_semaphore( &mfg_test_state_change_sem, MICO_WAIT_FOREVER )) )
                    ;
                break;
            }
            default:
                goto exit;
                // error
                //break;
        }
    }

    exit:
    mico_thread_sleep( MICO_NEVER_TIMEOUT );
}
