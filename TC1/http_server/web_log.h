#include <time.h>

#ifndef WEB_LOG_H
#define WEB_LOG_H

#define LOG_NUM 100
#define LOG_LEN 128
#define TIM_LEN 32

typedef struct
{
    int idx;
    char* logs[LOG_NUM];
} LogRecord;

extern LogRecord log_record;
extern char* LOG_TMP;
extern time_t now;
extern char time_buf[];

void SetLogRecord(LogRecord* lr, char* log);
char* GetLogRecord();

#define web_log(format, ...)                           \
    LOG_TMP = (char*)malloc(sizeof(char)*LOG_LEN);     \
    now = time(NULL); \
    now += 28800; \
    strftime(time_buf, TIM_LEN, "[%Y-%m-%d %H:%M:%S]", localtime(&now)); \
    snprintf(LOG_TMP, LOG_LEN, "%s"format, time_buf, ##__VA_ARGS__); \
    SetLogRecord(&log_record, LOG_TMP);                \

#endif // !WEB_LOG_H
