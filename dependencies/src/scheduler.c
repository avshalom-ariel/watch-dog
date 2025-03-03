/*
Name: Avshalom Ariel
Reveiwer: Itay
date: 	05/06/2024
status: Approved
*/

#include <stdlib.h> 	/*malloc, free*/
#include <stddef.h> 	/*size_t*/
#include <assert.h>		/*assert */
#include<unistd.h>		/*sleep */


#include "scheduler.h" 	/*scheduler_t typedefs and functions*/
#include "uid.h" 		/*ilrd_uid_t*/
#include "task.h" 		/*task_t and all Taskfunctions*/
#include "pqueue.h" 	/*pqueue_t, all PQfunctions*/

#include <sys/types.h>		/*need to be removed*/
       #include <unistd.h>	/*need to be removed*/


enum scheduler_status_t 
{
	OFF, 
	ON
};

enum action_func_status 
{
	ACTION_FUNC_SUCCESS, 
	ACTION_FUNC_FAILURE, 
	ACTION_FUNC_REMOVE_ME
};


static int CmpFunc(const void *data1, const void *data2);
static int UIDMachFunc(const void *task, const void *uid);


struct scheduler 
{
    pqueue_t *list;
    enum scheduler_status_t is_scheduler_on;
};


scheduler_t *SchedulerCreate(void)
{
	scheduler_t *scheduler = (scheduler_t *)malloc(sizeof(scheduler_t));
	if (NULL == scheduler)
	{
		return scheduler;
	}

	scheduler -> list = PQCreate(CmpFunc);
	if (NULL == scheduler -> list)
	{
		return NULL;
	}
	
	return scheduler;
}


void SchedulerDestroy(scheduler_t *scheduler)
{
	assert(NULL != scheduler);
	
	SchedulerClear(scheduler);
	PQDestroy(scheduler -> list);
	free(scheduler);
}


ilrd_uid_t SchedulerAddTask(scheduler_t *scheduler,
                            scheduler_action_func_t action_func,
                            scheduler_clean_func_t clean_func,
                            void *action_param,
                            size_t time_interval)
{
	task_t *task = NULL;
	
	assert(NULL != scheduler);
	
	task = TaskCreate(action_func, clean_func, action_param, time_interval);
	if (NULL == task)
	{
		return UIDGetBad();
	}
	
	if(0 != PQEnqueue(scheduler -> list, task))
	{
		TaskClean(task);
		return UIDGetBad();
	}
	
	return TaskUID(task);
}


int SchedulerRemove(scheduler_t *scheduler, ilrd_uid_t uid)
{
	task_t *task_to_remove = NULL;
	
	assert(NULL != scheduler);
	assert(!UIDIsEqual(uid, UIDGetBad()));
	
	task_to_remove = PQErase(scheduler -> list, UIDMachFunc, &uid);
	
	if(NULL == task_to_remove)
	{
		return 1;
	}
	
	TaskClean(task_to_remove);
	
	return 0;
}


int SchedulerRun(scheduler_t *scheduler)
{
	task_t *task_runner = NULL; 
	size_t time_to_sleep = 0;
	int task_command = 0;
	scheduler->is_scheduler_on = ON;
	
	assert(NULL != scheduler);
/*	printf("first running scheduler %d\n\n", getpid());*/
	while (!SchedulerIsEmpty(scheduler) && ON == scheduler->is_scheduler_on)
	{	
/*		printf("second running scheduler %d\n\n", getpid());*/
		
		task_runner = PQDequeue(scheduler -> list);
/*		printf("third running scheduler %d\n\n", getpid());*/
		time_to_sleep = TaskSleepTime(task_runner);
		if((size_t)-1 == time_to_sleep)
		{
/*			printf("why?!?!?!?! sleep failed\n\n");*/
			return -1;
		}
/*		printf("fourth running scheduler %d\n\n", getpid());*/
		while(0 < time_to_sleep)
		{
/*			printf("sleep time scheduler, %d, %d\n\n", getpid(), time_to_sleep);*/
			sleep(time_to_sleep);
			time_to_sleep = TaskSleepTime(task_runner);
			if((size_t)-1 == time_to_sleep)
			{
/*				printf("why?!?!?!?! sleep failed second time\n\n");*/
				return -1;
			}
		}
/*		printf("fifth running scheduler %d\n\n", getpid());*/
		task_command = TaskExecute(task_runner);
/*		printf("sixth running scheduler %d\n\n", getpid());*/
		if(task_command == ACTION_FUNC_REMOVE_ME)
		{
			TaskClean(task_runner);
		}
		else
		{
			TaskTimeUpdate(task_runner);
			if(PQEnqueue(scheduler -> list, task_runner))
			{
/*				printf("why?!?!?!?! didnt enqueeu\n\n");*/
				return -1;
			}
		}
/*		printf("seventh running scheduler %d\n\n", getpid());*/
	}
	
/*	printf("why?!?!?!?! is on?: %d\n\n", scheduler->is_scheduler_on);*/
	
	return (!SchedulerIsEmpty(scheduler));
}


size_t SchedulerSize(const scheduler_t *scheduler)
{
	assert(NULL != scheduler);

	return PQSize(scheduler -> list);
}


int SchedulerIsEmpty(const scheduler_t *scheduler)
{
	assert(NULL != scheduler);
	
	return PQIsEmpty(scheduler -> list);
}


void SchedulerStop(scheduler_t *scheduler)
{
	assert(NULL != scheduler);

	scheduler->is_scheduler_on = OFF;
}


void SchedulerClear(scheduler_t *scheduler)
{
	while (!SchedulerIsEmpty(scheduler))
	{
		TaskClean((task_t *)PQDequeue(scheduler -> list));
	}
}


int CmpFunc(const void *data1, const void *data2)
{	
	return TaskTime((task_t *)data1) - TaskTime((task_t *)data2);
}


int UIDMachFunc(const void *task, const void *uid)
{
	return UIDIsEqual(TaskUID((task_t *)task), *(ilrd_uid_t *)uid);
}


