#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include"http_server/web_log.h"

char* LOG_TMP;
time_t LOG_NOW;

LogRecord log_record = { 1,{ 0 } };
char log_record_str[LOG_NUM*LOG_LEN] = { 0 };

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

void WebLog(const char *M, ...)
{
    char* buff = (char*)malloc(sizeof(char)*LOG_LEN);

    time_t now = time(NULL) + 28800; //东8区
    strftime(buff, TIME_LEN, "[%Y-%m-%d %H:%M:%S]", localtime(&now));
    buff[TIME_LEN - 1] = ' ';

    va_list ap;
    va_start(ap, M);
    vsnprintf(buff + TIME_LEN, LOG_LEN - TIME_LEN, M, ap);
    va_end(ap);

    SetLogRecord(&log_record, buff);
}

