#ifndef __TASK_H__
#define __TASK_H__

#include "uid.h" /*ilrd_uid_t*/


typedef struct task task_t;


typedef int (*task_action_func_t)(void *);
typedef void (*task_clean_func_t)();


task_t *TaskCreate(task_action_func_t action_func,
                    task_clean_func_t clean_func,
                    void *action_param,
                    size_t time_interval);
       
double TaskSleepTime(task_t *task);
time_t TaskTime(task_t *task);
void TaskTimeUpdate(task_t *task);
void TaskClean(task_t *task);
ilrd_uid_t TaskUID(const task_t *task);
int TaskExecute(task_t *task);























#endif /*__TASK_H__*/                  
