
#include <time.h> /*time*/
#include <stddef.h> /*size_t*/
#include <stdlib.h> /*malloc free*/

#include "task.h" /*task_t*/
#include "scheduler.h" /*typedefs*/
#include "uid.h" /*ilrd_uid_t*/

struct task
{
    task_action_func_t task_action_func;
    task_clean_func_t task_clean_func;
    void *action_param;
    time_t time_to_run;
    size_t interval;
    ilrd_uid_t uid;
};


task_t *TaskCreate(task_action_func_t action_func,
                    task_clean_func_t clean_func,
                    void *action_param,
                    size_t time_interval)
{
	task_t *task = (task_t *)malloc(sizeof(task_t));
	if (NULL == task)
	{
		return NULL;
	}
	
	task -> uid = UIDCreate();
	if (UIDIsEqual(UIDGetBad(), TaskUID(task)))
	{
		return NULL;
	}
	
	task -> time_to_run = time(NULL);
	if ((time_t)-1 == (task -> time_to_run))
	{
		return NULL;
	}
	
	task -> time_to_run += time_interval;
	task -> task_action_func = action_func;
	task -> task_clean_func = clean_func;
	task -> action_param = action_param;
	task -> interval = time_interval;
	
	return task;
}


double TaskSleepTime(task_t *task)
{
	double time_to_sleep = 0;
	time_t now = time(NULL);
	if((time_t)-1 == now)
	{
		return -1;
	}
	
	time_to_sleep = difftime(task -> time_to_run, now);
	
	if(time_to_sleep < 0)
	{
		time_to_sleep = 0;
	}
	
	return time_to_sleep;
}


time_t TaskTime(task_t *task)
{
	return task -> time_to_run;
}


void TaskTimeUpdate(task_t *task)
{
	task -> time_to_run = time(NULL) + task -> interval;
}


ilrd_uid_t TaskUID(const task_t *task)
{
	return task -> uid;
}


void TaskClean(task_t *task)
{
	(task -> task_clean_func)(task -> action_param);
	
	free(task);
}


int TaskExecute(task_t *task)
{
	return (task -> task_action_func)(task -> action_param);	
}


