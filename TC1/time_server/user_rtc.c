#include "http_server/web_log.h"
#define os_log(format, ...) do { custom_log("RTC", format, ##__VA_ARGS__); web_log(format, ##__VA_ARGS__) } while(0)

#include "main.h"
#include "user_gpio.h"
#include "sntp.h"
#include "c_json/c_json.h"
#include "mqtt_server/user_mqtt_client.h"
#include "mqtt_server/user_function.h"

void RtcThread(mico_thread_arg_t arg);

OSStatus UserSntpGetTime()
{
    OSStatus err = kNoErr;
    ntp_timestamp_t current_time;

    struct hostent * hostent_content = NULL;
    char ** pptr = NULL;
    struct in_addr ipp;

    ipp.s_addr = 0xd248912c;
    err = sntp_get_time(&ipp, &current_time);

    if (err != kNoErr)
    {
        int ntp_count = 5;
        char* ntp_hosts[5] = {
            "pool.ntp.org",
            "cn.ntp.org.cn",
            "cn.pool.ntp.org",
            "s1a.time.edu.cn",
            "ntp.sjtu.edu.cn",
        };

        int i = 0;
        for (; i < ntp_count; i++)
        {
            hostent_content = gethostbyname(ntp_hosts[i]);
            if (hostent_content == NULL)
            {
                os_log("gethostbyname(%s)", ntp_hosts[i]);
                continue;
            }
            pptr = hostent_content->h_addr_list;
            ipp.s_addr = *(uint32_t *)(*pptr);
            err = sntp_get_time(&ipp, &current_time);
            if (err == kNoErr)
            {
                break;
            }
        }
    }

    if (err != kNoErr)
    {
        os_log("sntp_get_time4 err[%d]", err);
        return err;
    }

    mico_utc_time_ms_t utc_time_ms = (uint64_t)current_time.seconds * (uint64_t)1000
        + (current_time.microseconds / 1000);
    mico_time_set_utc_time_ms(&utc_time_ms);
    return kNoErr;
}

OSStatus UserRtcInit(void)
{
    OSStatus err = kNoErr;

    /* start rtc client */
    err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "rtc",
                                   (mico_thread_function_t) RtcThread,
                                   0x1000, 0);
    require_noerr_string(err, exit, "ERROR: Unable to start the rtc thread.");

    if (kNoErr != err) os_log("ERROR1, app thread exit err: %d kNoErr[%d]", err, kNoErr);

    exit:
    return err;
}

void RtcThread(mico_thread_arg_t arg)
{
    OSStatus err = kUnknownErr;
    LinkStatusTypeDef LinkStatus;
    mico_rtc_time_t rtc_time;

    mico_utc_time_t utc_time;
    mico_utc_time_t utc_time_last = 0;
    while (1)
    {   //上电后连接了wifi才开始走时否则等待连接
        micoWlanGetLinkStatus(&LinkStatus);
        if (LinkStatus.is_connected == 1)
        {
            err = UserSntpGetTime();
            if (err == kNoErr)
            {
                os_log("sntp success!");
                rtc_init = 1;
                break;
            }
        }
        mico_rtos_thread_sleep(3);
    }

    while (1)
    {
        mico_time_get_utc_time(&utc_time);
        utc_time += 28800;

        if (utc_time_last != utc_time)
        {
            utc_time_last = utc_time;
            total_time++;
        }

        struct tm * currentTime = localtime((const time_t *) &utc_time);
        rtc_time.sec = currentTime->tm_sec;
        rtc_time.min = currentTime->tm_min;
        rtc_time.hr = currentTime->tm_hour;

        rtc_time.date = currentTime->tm_mday;
        rtc_time.weekday = currentTime->tm_wday;
        rtc_time.month = currentTime->tm_mon + 1;
        rtc_time.year = (currentTime->tm_year + 1900) % 100;

        // MicoRtcSetTime(&rtc_time);      //MicoRtc不自动走时!

        //SNTP服务 开机及每小时校准一次
        if (rtc_init != 1 || (rtc_time.sec == 0 && rtc_time.min == 0))
        {
            micoWlanGetLinkStatus(&LinkStatus);
            if (LinkStatus.is_connected == 1)
            {
                err = UserSntpGetTime();
                if (err == kNoErr)
                    rtc_init = 1;
                else
                    rtc_init = 2;
            }
        }

        mico_rtos_thread_msleep(900);
    }

//  exit:
    os_log("EXIT: rtc exit with err = %d.", err);
    mico_rtos_delete_thread(NULL);
}

