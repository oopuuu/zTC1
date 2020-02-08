#include "http_server/web_log.h"
#define os_log(format, ...) do { custom_log("SNTP", format, ##__VA_ARGS__); web_log(format, ##__VA_ARGS__) } while(0)

#include "main.h"
//#include "user_gpio.h"
#include "time_server/user_sntp.h"
#include "sntp.h"

/* Callback function when MiCO UTC time in sync to NTP server */
static void sntp_time_call_back(void)
{
    struct tm *     currentTime;
//  iso8601_time_t  iso8601_time;
    mico_utc_time_t utc_time;
    mico_rtc_time_t rtc_time;

//  mico_time_get_iso8601_time(&iso8601_time);
//  os_log("sntp_time_synced: %.26s", (char*)&iso8601_time);

    mico_time_get_utc_time(&utc_time);
    utc_time+=28800; //+8:00
    currentTime = localtime((const time_t *)&utc_time);
    rtc_time.sec = currentTime->tm_sec;
    rtc_time.min = currentTime->tm_min;
    rtc_time.hr = currentTime->tm_hour;

    rtc_time.date = currentTime->tm_mday;
    rtc_time.weekday = currentTime->tm_wday;
    rtc_time.month = currentTime->tm_mon + 1;
    rtc_time.year = (currentTime->tm_year + 1900) % 100;

    MicoRtcSetTime(&rtc_time);


    MicoRtcGetTime(&rtc_time);
    os_log("SNTP time2:20%d/%d/%d %d %d:%d:%d",rtc_time.year,rtc_time.month,rtc_time.date,rtc_time.weekday,rtc_time.hr,rtc_time.min,rtc_time.sec);

}


void sntp_init(void)
{
    struct in_addr ipp;
    ipp.s_addr=0xd248912c;
    sntp_set_server_ip_address (0,ipp);
    sntp_start_auto_time_sync (15000,  sntp_time_call_back);        //每小时校准一次

    mico_rtc_time_t rtc_time;
    MicoRtcGetTime(&rtc_time);
    os_log("time3:20%d/%d/%d %d %d:%d:%d",rtc_time.year,rtc_time.month,rtc_time.date,rtc_time.weekday,rtc_time.hr,rtc_time.min,rtc_time.sec);

}
