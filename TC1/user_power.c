#include "http_server/web_log.h"
#include "TimeUtils.h"

#include "mico.h"
#include "main.h"
#include "mqtt_server/user_mqtt_client.h"
#include "user_power.h"

uint32_t p_count = 0;
PowerRecord power_record = { 1,{ 0 } };
char power_record_str[1101] = { 0 };

void SetPowerRecord(PowerRecord* pr, uint32_t pw)
{
    pr->powers[(++pr->idx)% PW_NUM] = pw;
}

char* GetPowerRecord(int idx)
{
    if (idx > power_record.idx) return "";
    idx = idx <= power_record.idx - PW_NUM ? 0 : idx;

    int i = idx > 0 ? idx : (power_record.idx - PW_NUM + 1);
    i = i < 0 ? 0 : i;
    char* tmp = power_record_str;
    for (; i <= power_record.idx; i++)
    {
        sprintf(tmp, "%lu,", power_record.powers[i%PW_NUM]);
        tmp += strlen(tmp);
    }
    *(--tmp) = 0;
    return power_record_str;
}

uint64_t NS = 1000000000;
float n_1s = 0;       //在当前这一秒功率中断次数
uint64_t past_ns = 0; //系统运行的纳秒数
uint64_t irq_old = 0; //上次中断的时间(纳秒)

static void PowerIrqHandler(void* arg)
{
    p_count++;

    //mico_time_get_time(&past_ns); //系统运行毫秒数
    past_ns = mico_nanosecond_clock_value(); //系统运行纳秒数
    uint64_t spend_ns = past_ns - irq_old;

    if (irq_old % NS + spend_ns <= NS)
    {
        n_1s += 1;
        irq_old = past_ns;
        return;
    }

    int n = (spend_ns - past_ns % NS) / NS;
    n_1s += (float)(NS - irq_old % NS) / spend_ns;
    float power2 = 17.1 * n_1s;
    SetPowerRecord(&power_record, (int)power2);

    int i = 0;
    for (; i < n; i++)
    {
        power2 = 17.1 * NS / spend_ns;
        SetPowerRecord(&power_record, (int)power2);
    }
    irq_old = past_ns;
    n_1s = (float)(past_ns % NS) / spend_ns;
}

void PowerInit(void)
{
    ota_log("user_power_init");
    MicoGpioInitialize(POWER, INPUT_PULL_UP);
    MicoGpioEnableIRQ(POWER, IRQ_TRIGGER_FALLING_EDGE, PowerIrqHandler, NULL);
}

