#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"http_server/web_log.h"

LogRecord log_record = { 1,{ 0 } };
char log_record_str[LOG_NUM*LOG_LEN] = { 0 };
char* LOG_TMP;
char log[LOG_LEN];
time_t now;
char time_buf[TIM_LEN];

void SetLogRecord(LogRecord* lr, char* log)
{
    if (strlen(log) > LOG_LEN)
    {
        log[LOG_LEN-1] = 0;
    }
    char** p_log = &lr->logs[(++lr->idx)% LOG_NUM];
    if (*p_log)
    {
        free(*p_log);
    }
    *p_log = log;
}

char* GetLogRecord()
{
    int i = log_record.idx - LOG_NUM + 1;
    i = i < 0 ? 0 : i;
    char* tmp = log_record_str;
    sprintf(tmp, "%d\n", log_record.idx);
    for (; i <= log_record.idx; i++)
    {
        tmp += strlen(tmp);
        if (!log_record.logs[i%LOG_NUM]) continue;
        sprintf(tmp, "%s\n", log_record.logs[i%LOG_NUM]);
    }
    return log_record_str;
}

void web_log(const char *N, const char *M, ...)
{
    va_list ap;
    va_start(ap, M);
    int ret = vsnprintf(log, sizeof(log), M, ap);
    va_end(ap);

    LOG_TMP = (char*)malloc(sizeof(char)*LOG_LEN);
    now = time(NULL);
    now += 28800;
    strftime(time_buf, TIM_LEN, "%Y-%m-%d %H:%M:%S", localtime(&now));
    snprintf(LOG_TMP, LOG_LEN, "[%s][%s %s:%d] %s", time_buf, N, SHORT_FILE, __LINE__, log);
    SetLogRecord(&log_record, LOG_TMP);
}

