#include <time.h>

#ifndef WEB_LOG_H
#define WEB_LOG_H

#define LOG_NUM 100
#define LOG_LEN 128
#define TIME_LEN 22

typedef struct
{
    int idx;
    char* logs[LOG_NUM];
} LogRecord;

void SetLogRecord(LogRecord* lr, char* log);
char* GetLogRecord();
void WebLog(const char *M, ...);

#define web_log(N, M, ...) WebLog("["N" %s:%d] "M, SHORT_FILE, __LINE__, ##__VA_ARGS__)

#endif // !WEB_LOG_H
