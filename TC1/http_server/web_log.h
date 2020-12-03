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
void WebLog(const char *M, ...);

#define web_log0(N, M, ...)                           \
    LOG_TMP = (char*)malloc(sizeof(char)*LOG_LEN);     \
    now = time(NULL); \
    now += 28800; \
    strftime(time_buf, TIM_LEN, "%Y-%m-%d %H:%M:%S", localtime(&now)); \
    snprintf(LOG_TMP, LOG_LEN, "[%s][%s %s:%d] "M, time_buf, N, SHORT_FILE, __LINE__, ##__VA_ARGS__); \
    SetLogRecord(&log_record, LOG_TMP);                \

#define web_log(N, M, ...) WebLog("["N" %s:%d] "M, SHORT_FILE, __LINE__, ##__VA_ARGS__)

#endif // !WEB_LOG_H
