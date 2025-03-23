#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>

#include"main.h"
#include"user_gpio.h"
#include "mqtt_server/user_mqtt_client.h"
#include"timed_task/timed_task.h"
#include"http_server/web_log.h"
#include "user_wifi.h"

int day_sec = 86400;

pTimedTask NewTask()
{
    for (int i = 0; i < MAX_TASK_NUM; i++)
    {
        pTimedTask task = &user_config->timed_tasks[i];
        if (!task->on_use)
        {
            task->on_use = true;
            return task;
        }
    }
    return NULL;
}

bool AddTaskSingle(pTimedTask task)
{
    user_config->task_count++;
    if (user_config->task_top == NULL)
    {
        task->next = NULL;
        user_config->task_top = task;
        return true;
    }

    if (task->prs_time <= user_config->task_top->prs_time)
    {
        task->next = user_config->task_top;
        user_config->task_top = task;
        return true;
    }

    pTimedTask tmp = user_config->task_top;
    while (tmp)
    {
        if (tmp->next == NULL
            || (task->prs_time >= tmp->prs_time
             && task->prs_time < tmp->next->prs_time))
        {
            task->next = tmp->next;
            tmp->next = task;
            return true;
        }
        tmp = tmp->next;
    }
    user_config->task_count--;
    return false;
}

bool AddTaskWeek(pTimedTask task)
{
    time_t now = time(NULL);
    int today_weekday = (now / day_sec + 3) % 7 + 1; //1970-01-01 星期五
    int next_day = task->weekday - today_weekday;
    bool next_day_is_today = next_day == 0 && task->prs_time % day_sec > now % day_sec;
    next_day = next_day > 0 || next_day_is_today ? next_day : next_day + 7;
    task->prs_time = (now - now % day_sec) + (next_day * day_sec) + task->prs_time % day_sec;

    return AddTaskSingle(task);
}

bool AddTask(pTimedTask task)
{
    if (task->weekday == 0 || task->weekday == 8)
        return AddTaskSingle(task);
    return AddTaskWeek(task);
}

bool DelFirstTask()
{
    if (user_config->task_top)
    {
        pTimedTask tmp = user_config->task_top;
        user_config->task_top = user_config->task_top->next;
        user_config->task_count--;
        if (tmp->weekday == 0)
        {
            tmp->on_use = false;
        }
        else if (tmp->weekday == 8) //8代表每日任务
        {
            tmp->prs_time += day_sec;
            AddTask(tmp);
        }
        else
        {
            tmp->prs_time += 7 * day_sec;
            AddTask(tmp);
        }
        return true;
    }
    return false;
}

bool DelTask(int time)
{
    if (user_config->task_top == NULL)
    {
        return false;
    }

    if (time == user_config->task_top->prs_time)
    {
        pTimedTask tmp = user_config->task_top;
        user_config->task_top = user_config->task_top->next;
        tmp->on_use = false;
        user_config->task_count--;
        return true;
    }
    else if (user_config->task_top->next == NULL)
    {
        return false;
    }

    pTimedTask pre_tsk = user_config->task_top;
    pTimedTask tmp_tsk = user_config->task_top->next;
    while (tmp_tsk)
    {
        if (time == tmp_tsk->prs_time)
        {
            pre_tsk->next = tmp_tsk->next;
            tmp_tsk->on_use = false;
            user_config->task_count--;
            return true;
        }
        tmp_tsk = tmp_tsk->next;
    }
    return false;
}

void ProcessTask()
{
    task_log("process task time[%ld] operation[%s] on[%d]",
        user_config->task_top->prs_time, get_func_name(user_config->task_top->operation), user_config->task_top->on);
    switch (user_config->task_top->operation) {
            case SWITCH_ALL_SOCKETS:
                UserRelaySetAll(user_config->task_top->on);
                mico_system_context_update(sys_config);
                for (int i = 0; i < SOCKET_NUM; i++) {
                    UserMqttSendSocketState(i);
                }
                UserMqttSendTotalSocketState();
                break;
            case SWITCH_SOCKET_1:
            case SWITCH_SOCKET_2:
            case SWITCH_SOCKET_3:
            case SWITCH_SOCKET_4:
            case SWITCH_SOCKET_5:
            case SWITCH_SOCKET_6:
                UserRelaySet(user_config->task_top->operation - 1, user_config->task_top->on);
                UserMqttSendSocketState(user_config->task_top->operation - 1);
                UserMqttSendTotalSocketState();
                mico_system_context_update(sys_config);
                break;
            case SWITCH_LED_ENABLE:

                if (RelayOut() && user_config->task_top->on) {
                    UserLedSet(1);
                } else {
                    UserLedSet(0);
                }
                UserMqttSendLedState();
                mico_system_context_update(sys_config);
                break;
            case SWITCH_CHILD_LOCK_ENABLE:
                user_config->user[0] = user_config->task_top->on;
                childLockEnabled = user_config->user[0];
                mico_system_context_update(sys_config);
                UserMqttSendChildLockState();
                break;
            case REBOOT_SYSTEM:
                MicoSystemReboot();
                break;
            case CONFIG_WIFI:

                micoWlanSuspendStation();
                ApInit(true);
                break;
            case RESET_SYSTEM:

                mico_system_context_restore(sys_config);
                mico_rtos_thread_sleep(1);
                MicoSystemReboot();
                break;
            default:
                break;
        }
    DelFirstTask();
}

char* GetTaskStr()
{
    char* str = (char*)malloc(sizeof(char)*(user_config->task_count*89+2));
    pTimedTask tmp_tsk = user_config->task_top;
    char* tmp_str = str;
    tmp_str[0] = '[';
    tmp_str[2] = 0;
    tmp_str++;
    while (tmp_tsk)
    {
        char buffer[26];
        struct tm* tm_info;
        time_t prs_time = tmp_tsk->prs_time + 28800;
        tm_info = localtime(&prs_time);
        strftime(buffer, 26, "%m-%d %H:%M", tm_info);

        sprintf(tmp_str, "{'timestamp':%ld,'prs_time':'%s','operation':%d,'on':%d,'weekday':%d},",
            tmp_tsk->prs_time, buffer, tmp_tsk->operation, tmp_tsk->on, tmp_tsk->weekday);
        tmp_str += strlen(tmp_str);
        tmp_tsk = tmp_tsk->next;
    }
    if (user_config->task_count > 0) --tmp_str;
    *tmp_str = ']';
    return str;
}
