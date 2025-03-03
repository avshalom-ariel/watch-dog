#include <signal.h>				/*struct sigaction*/
#include <stdio.h>				/*perror*/
#include <stdlib.h>				/*getenv*/
#include <sys/stat.h>  			/*for mode constants in sem_open (SEM_NAME,)*/
#include <fcntl.h>      		/*for O_* constants in fopen (O_CREAT)*/


#include <pthread.h>			/*pthread typedef and functions*/
#include <semaphore.h>			/*sem_t, semaphore functions*/
#include <unistd.h>				/*getpid*/
#include <sys/types.h>			/*pid_t*/

#include "watch_dog.h"
#include "watch_dog_util.h"

/*static function declarations*/
static int SetSignalMasking(int blocking_status);
static void *WDThreadCheckRoutine(void *arg);
static int RecoverSigUser(void);

/*global variables*/
pthread_t g_communication_thread;
static struct sigaction g_old_sig1;
static struct sigaction g_old_sig2;


/********************************************/
/*			   API Functions				*/
/********************************************/


int MMI(size_t interval_in_seconds, size_t repitition, char **argv)
{
	sem_t *watch_dog_sem = sem_open(WATCH_DOG_SEM_NAME, O_CREAT, SEM_PERMISSIONS, 0);
	sem_t *thread_sem = sem_open(THREAD_SEM_NAME, O_CREAT, SEM_PERMISSIONS, 0);

	if (SEM_FAILED == thread_sem || SEM_FAILED == watch_dog_sem)
	{
		perror("Failed creating semaphores (MMI)\n");
		
		return -1;
	}

	/*set relevent data into a external struct*/
	SetWatchDogStruct(argv, interval_in_seconds, repitition);

	/*set watch dog execute arguments into g_communication_arg_vector (external)*/
	SetWatchDogArgv();

	/*check if watch dog needs to be executed*/
	if (NULL == getenv(ENV_VAR_NAME))
	{
		/*execute watch dog*/
		ProcessExec(g_communication_arg_vector);
	}
	else
	{
		/*if alive, get watch dog pid*/
		g_communication_pid = atoi(getenv(ENV_VAR_NAME));

		/*dont wait for watch dog setup*/
		sem_post(watch_dog_sem);
	}

	/*mask SIGUSR1 & SIGUSR2*/
	SetSignalMasking(SIG_BLOCK);

	/*build scheduler with alll relevent tasks*/
	BuildCommunicationScheduler();

	if (0 != pthread_create(&g_communication_thread, NULL, 
				  			  WDThreadCheckRoutine, NULL))
	{
		perror("Thread creation failed!\n");
		
		return -1;
	}

	/*wait for watch dog and communication thread*/
	sem_wait(watch_dog_sem);
	sem_wait(thread_sem);

	/*close watch dog and communication thread semaphores*/
	sem_close(watch_dog_sem);
	sem_close(thread_sem);
	
	return 0;
}

void DNR(void)
{
    sem_t *dnr_sem = sem_open(DNR_SEM_NAME, 0);
	
	/*make sure DNR signal dont sent during revival*/
	sem_wait(dnr_sem);
	
    /*send communication thread and watch dog signal to stop*/
    kill(g_communication_pid, SIGUSR2);
    kill(getpid(), SIGUSR2);

    /*wait for communication thread to to terminate*/
	pthread_join(g_communication_thread, NULL);
	
	/*recover SIGUSR1 & SIGUSR2 old handlers*/
	RecoverSigUser();

	sem_close(dnr_sem);

	/*unlink named semaphores*/
	sem_unlink(REVIVAL_SEM_NAME);
	sem_unlink(THREAD_SEM_NAME);
	sem_unlink(DNR_SEM_NAME);
}


/********************************************/
/*			   Static Functions				*/
/********************************************/


void *WDThreadCheckRoutine(void *arg)
{
	sem_t *thread_sem = sem_open(THREAD_SEM_NAME, 0);
	struct sigaction act_sig1 = {0};
	struct sigaction act_sig2 = {0};

	/*unmask SIGUSR1 & SIGUSR2 in communication thread */
	SetSignalMasking(SIG_UNBLOCK);

	/*set signal handlers for communication thread*/
	if (-1 == SignalHandle(&act_sig1, &act_sig2, &g_old_sig1, &g_old_sig2))
	{
		perror("SignalHandle in communication thread failed!\n");
		
		return NULL;
	}
	
	/*tell mmi stop waiting for thread*/
	sem_post(thread_sem);

	/*running communication scheduler*/
	RunCommunicationScheduler();
	
	/*close semaphore*/
	sem_close(thread_sem);
	
	UNUSED(arg);
	
	return NULL;
}

int SetSignalMasking(int blocking_status)
{
	int status = 0;
	sigset_t set = {0};

	/*mask/unmask SIGUSR1 & SIGUSR2 in main thread*/
	status = sigemptyset(&set);
	status = sigaddset(&set, SIGUSR1);
	status = sigaddset(&set, SIGUSR2);
	status = pthread_sigmask(blocking_status, &set, NULL);
	
	return status;
}

int RecoverSigUser(void)
{
	/*restore SIGUSR1 handler*/
	if (-1 == sigaction(SIGUSR1, &g_old_sig1, NULL))
	{
		perror("error restoring SIGUSR1 handler\n");
		
		return 1;
	}
	
	/*restore SIGUSR2 handler*/
	if (-1 == sigaction(SIGUSR2, &g_old_sig2, NULL))
	{
		perror("error restoring SIGUSR2 handler\n");
		
		return 1;
	}
	
	return 0;
}



