#pragma once
#include <time.h>

struct TimedTask;
typedef struct TimedTask* pTimedTask;
struct TimedTask
{
    bool on_use;     //正在使用
    time_t prs_time; //被执行的格林尼治时间戳
    int operation;  //要进行的操作
    int on;          //开或者关，或者其他操作
    int weekday;     //星期重复 0代表不重复 8代表每日重复
    pTimedTask next; //下一个任务(按之间排序)
};

pTimedTask NewTask();
bool AddTask(pTimedTask task);
bool DelTask(int time);
bool DelFirstTask();
void ProcessTask();
char* GetTaskStr();
