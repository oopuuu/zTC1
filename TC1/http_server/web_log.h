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

extern LogRecord log_record;
extern char* LOG_TMP;
extern time_t LOG_NOW;
#define web_log(N, M, ...) do {                      \
    char* _wl_tmp = (char*)malloc(sizeof(char)*LOG_LEN); \
    if (_wl_tmp) {                                    \
        time_t _wl_now = time(NULL) + 28800;          \
        strftime(_wl_tmp, TIME_LEN, "[%Y-%m-%d %H:%M:%S]", localtime(&_wl_now)); \
        _wl_tmp[TIME_LEN - 1] = ' ';                 \
        snprintf(_wl_tmp + TIME_LEN, LOG_LEN - TIME_LEN, "[" N " %s:%d] " M, SHORT_FILE, __LINE__, ##__VA_ARGS__); \
        SetLogRecord(&log_record, _wl_tmp);           \
    }                                                 \
} while(0)

#define web_log0(N, M, ...) WebLog("["N" %s:%d] "M, SHORT_FILE, __LINE__, ##__VA_ARGS__)

#endif // !WEB_LOG_H
