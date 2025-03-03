/*
Name: Avshalom Ariel
Reveiwer: Or
Date: 01/07/2024
status: Approved
*/

#include <stdlib.h> 			/*malloc, free*/
#include <stddef.h> 			/*size_t*/
#include <assert.h>				/* assert */
#include <unistd.h>				/* sleep */

#include "heap_pq_scheduler.h" 	/*heap_pq_scheduler_t typedefs and functions*/
#include "uid.h" 				/*ilrd_uid_t*/
#include "task.h" 				/*task_t and all Taskfunctions*/
#include "heap_pq.h" 			/*pqueue_t, all PQfunctions*/

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
    heap_pq_t *heap;
    enum scheduler_status_t is_scheduler_on;
};


/********************************************/
/*			   API Functions				*/
/********************************************/


heap_pq_scheduler_t *Heap_PQ_SchedulerCreate(void)
{
	heap_pq_scheduler_t *scheduler = (heap_pq_scheduler_t *)malloc(sizeof(heap_pq_scheduler_t));
	if (!scheduler)
	{
		return NULL;
	}

	scheduler->heap = HeapPQCreate(CmpFunc);
	if (!scheduler->heap)
	{
		return NULL;
	}
	
	return scheduler;
}

void Heap_PQ_SchedulerDestroy(heap_pq_scheduler_t *scheduler)
{
	assert(scheduler);
	
	Heap_PQ_SchedulerClear(scheduler);
	HeapPQDestroy(scheduler -> heap);
	free(scheduler);
}

ilrd_uid_t Heap_PQ_SchedulerAddTask(heap_pq_scheduler_t *scheduler,
                            scheduler_action_func_t action_func,
                            scheduler_clean_func_t clean_func,
                            void *action_param,
                            size_t time_interval)
{
	task_t *task = NULL;
	
	assert(scheduler);
	
	task = TaskCreate(action_func, clean_func, action_param, time_interval);
	if (!task)
	{
		return UIDGetBad();
	}
	
	if(0 != HeapPQEnqueue(scheduler -> heap, task))
	{
		TaskClean(task);
		return UIDGetBad();
	}
	
	return TaskUID(task);
}

int Heap_PQ_SchedulerRemove(heap_pq_scheduler_t *scheduler, ilrd_uid_t uid)
{
	task_t *task_to_remove = NULL;
	
	assert(scheduler);
	assert(!UIDIsEqual(uid, UIDGetBad()));
	
	task_to_remove = HeapPQErase(scheduler -> heap, UIDMachFunc, &uid);
	if(!task_to_remove)
	{
		return 1;
	}
	
	TaskClean(task_to_remove);
	
	return 0;
}

int Heap_PQ_SchedulerRun(heap_pq_scheduler_t *scheduler)
{
	task_t *task_runner = NULL; 
	size_t time_to_sleep = 0;
	int task_command = 0;
	scheduler->is_scheduler_on = ON;
	
	assert(scheduler);
		
	while (!Heap_PQ_SchedulerIsEmpty(scheduler) && ON == scheduler->is_scheduler_on)
	{	
		task_runner = HeapPQDequeue(scheduler -> heap);
		
		time_to_sleep = TaskSleepTime(task_runner);
		if((size_t)-1 == time_to_sleep)
		{
			return -1;
		}
		
		while(0 < time_to_sleep)
		{
			sleep(time_to_sleep);
			time_to_sleep = TaskSleepTime(task_runner);
			if((size_t)-1 == time_to_sleep)
			{
				return -1;
			}
		}
		
		task_command = TaskExecute(task_runner);
		
		if(task_command == ACTION_FUNC_REMOVE_ME)
		{
			TaskClean(task_runner);
		}
		else
		{
			TaskTimeUpdate(task_runner);
			if(HeapPQEnqueue(scheduler -> heap, task_runner))
			{
				return -1;
			}
		}
	}
	
	return (!Heap_PQ_SchedulerIsEmpty(scheduler));
}


size_t Heap_PQ_SchedulerSize(const heap_pq_scheduler_t *scheduler)
{
	assert(scheduler);

	return HeapPQSize(scheduler -> heap);
}


int Heap_PQ_SchedulerIsEmpty(const heap_pq_scheduler_t *scheduler)
{
	assert(scheduler);
	
	return HeapPQIsEmpty(scheduler -> heap);
}


void Heap_PQ_SchedulerStop(heap_pq_scheduler_t *scheduler)
{
	assert(scheduler);

	scheduler->is_scheduler_on = OFF;
}


void Heap_PQ_SchedulerClear(heap_pq_scheduler_t *scheduler)
{
	while (!Heap_PQ_SchedulerIsEmpty(scheduler))
	{
		TaskClean((task_t *)HeapPQDequeue(scheduler -> heap));
	}
}


int CmpFunc(const void *data1, const void *data2)
{	
	return TaskTime((task_t *)data2) - TaskTime((task_t *)data1);
}


/*int UIDMachFunc(const void *uid, const void *task)*/
/*{*/
/*	return UIDIsEqual(TaskUID((task_t *)task), *(ilrd_uid_t *)uid);*/
/*}*/




int UIDMachFunc(const void *uid, const void *task)
{
	return UIDIsEqual(TaskUID((task_t *)task), *(ilrd_uid_t *)uid);
}


