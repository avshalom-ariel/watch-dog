#define _POSIX_C_SOURCE 200112L /*setenv*/
#include <signal.h>				/*signal functions, signal macros*/
#include <stdatomic.h>			/*atomic typedef, atomic functions*/
#include <semaphore.h>			/*semahore typedef, semaphore function*/
#include <sys/stat.h>  			/*for mode constants in sem_open (SEM_NAME,)*/
#include <fcntl.h>      		/*for O_* constants in fopen (O_CREAT)*/
#include <unistd.h>				/*fork, execvp, getpid*/
#include <sys/types.h>			/*fork, getpid,*/
#include <assert.h>				/*assert*/
#include <string.h>				/*memcpy*/
#include <stdlib.h>				/*getenv, setenv, exit*/
#include <stdio.h>				/*sprintf, perror, printf*/

#include "heap_pq_scheduler.h"
#include "watch_dog_util.h"
#include "uid.h"

/*static function declarations*/
static void Handler2(int signal, siginfo_t *info, void *arg);
static void Handler1(int signal, siginfo_t *info, void *arg);
static int CounterCheck(void *repitition);
static int SendSignal(void *pid);
static int DNRCheck(void *arg);
static void DummyClean();

/*static volatile atomic global variables*/
static volatile atomic_int g_counter = ATOMIC_VAR_INIT(0);
static volatile atomic_int g_dnr = ATOMIC_VAR_INIT(0);	

/*static global variabls*/
static watch_dog_arg_t g_watch_dog_arg_struct;
static heap_pq_scheduler_t *g_scheduler;
static char g_str_repetitions[16];
static char g_str_interval[16];

/*external global variabls*/
char *g_communication_arg_vector[NUM_OF_ARG];
pid_t g_communication_pid;


/********************************************/
/*			   API Functions				*/
/********************************************/


int ProcessExec(char **argv)
{
	pid_t pid = {0};
	
	assert(argv);
	
	pid = fork();
	
	if (0 > pid)		/*error*/
	{
		perror("Failed forking\n");
		
		exit(EXIT_FAILURE);
	}
	else if (0 == pid)	/*child*/
	{
		if (-1 == execvp(argv[0], argv))
		{
			perror("Failed to execute\n");
			
			exit(EXIT_FAILURE);
		}
	}
	else 				/*parent*/
	{
		g_communication_pid = pid;
	}
	
	return 0;
}

int SignalHandle(struct sigaction *act_sig1, struct sigaction *act_sig2,
		 struct sigaction *old_sig_act1, struct sigaction *old_sig_act2)
{
	/*set signal handlers for SIGUSR1 & SIGUSR2*/
	act_sig1->sa_sigaction = Handler1;
	act_sig1->sa_flags = SA_SIGINFO;
	
	assert(act_sig1);
	assert(act_sig2);
	
	if (-1 == sigaction(SIGUSR1, act_sig1, old_sig_act1))
	{
		perror("Failed setting sigaction (SIGUSR1)!\n");
		
		exit(EXIT_FAILURE);
	}
	
	act_sig2->sa_sigaction = Handler2;
	act_sig2->sa_flags = SA_SIGINFO;
	
	if (-1 == sigaction(SIGUSR2, act_sig2, old_sig_act2))
	{
		perror("Failed setting sigaction (SIGUSR2)!\n");
		
		exit(EXIT_FAILURE);
	}
	
	return 0;
}

int BuildCommunicationScheduler()
{
	ilrd_uid_t uid_counter = {0};
	ilrd_uid_t uid_signal = {0};
	ilrd_uid_t uid_dnr = {0};
	
	/*create scheduler*/
	g_scheduler = Heap_PQ_SchedulerCreate();
	if (!g_scheduler)
	{
		perror("Failed creating scheduler!\n");
		
		exit(EXIT_FAILURE);
	}
	
	/*add signal sending task*/
	uid_signal = Heap_PQ_SchedulerAddTask(g_scheduler, SendSignal, 
	DummyClean, NULL, g_watch_dog_arg_struct.interval_in_seconds);
	
	if (UIDIsEqual(uid_signal, UIDGetBad()))
	{
		perror("Bad uid! (SendSignal)\n");
		
		exit(EXIT_FAILURE);
	}
	
	/*add counter check task (revive if bad counter)*/
	uid_counter = Heap_PQ_SchedulerAddTask(g_scheduler, CounterCheck, 
		DummyClean, NULL, g_watch_dog_arg_struct.interval_in_seconds);
	
	if (UIDIsEqual(uid_counter, UIDGetBad()))
	{
		perror("Bad uid! (CounterCheck)\n");
		
		exit(EXIT_FAILURE);
	}
	
	/*add dnr check task*/
	uid_dnr = Heap_PQ_SchedulerAddTask(g_scheduler, DNRCheck, 
	DummyClean, NULL, g_watch_dog_arg_struct.interval_in_seconds);
	
	if (UIDIsEqual(uid_dnr, UIDGetBad()))
	{
		perror("Bad uid! (DNRCheck)\n");
		
		exit(EXIT_FAILURE);
	}
	
	return 0;
}

int RunCommunicationScheduler()
{
	/*open communication semaphore*/
	sem_t *revivel_sem = sem_open(REVIVAL_SEM_NAME, O_CREAT, SEM_PERMISSIONS, 0);
	if (SEM_FAILED == revivel_sem)
	{
		perror("Failed open semaphore (RunCommunicationScheduler)\n");
		
		exit(EXIT_FAILURE);
	}
	
	/*post communication semaphore*/
	sem_post(revivel_sem);
	
	/*run communication scheduler*/
	Heap_PQ_SchedulerRun(g_scheduler);
	
	/*destroy communication scheduler*/
	Heap_PQ_SchedulerDestroy(g_scheduler);
	
	/*close communication semaphore*/
	sem_close(revivel_sem);
	
	return 0;
}

void SetWatchDogStruct(char **argv, size_t interval_in_seconds, size_t repitition)
{
	char **argv_runner = argv;
	
	/*set g_watch_dog_arg_struct args*/
	g_watch_dog_arg_struct.client_argc = 0;
	g_watch_dog_arg_struct.repitition = repitition;
	g_watch_dog_arg_struct.interval_in_seconds = interval_in_seconds;
	
	while (NULL != *argv_runner)
	{
		++argv_runner;
		++g_watch_dog_arg_struct.client_argc;
	}
	
	memcpy(g_watch_dog_arg_struct.client_argv, argv, 
			(g_watch_dog_arg_struct.client_argc) * sizeof(char *));
}

void SetWatchDogArgv(void)
{
	size_t i = 0;
	
	/*set watch dog argument vector to execute*/
	sprintf(g_str_repetitions, "%ld", g_watch_dog_arg_struct.repitition);
	sprintf(g_str_interval, "%ld", g_watch_dog_arg_struct.interval_in_seconds);
    
	g_communication_arg_vector[0] = WATCH_DOG_EXEC;
	
	g_communication_arg_vector[REPITITION_INDEX] = g_str_repetitions;
	
	g_communication_arg_vector[INTERVAL_INDEX] = g_str_interval;
	
	while (NULL != g_watch_dog_arg_struct.client_argv[i])
	{
		/*3 for arguments taken (WATCH_DOG_EXEC, repitition and interval)*/
		g_communication_arg_vector[i + 3] = g_watch_dog_arg_struct.client_argv[i];
		
		++i;
	}
}

void SetClientArgv(void)
{
	size_t i = 0;
	
	/*set clent argument vector to execute*/
	while (NULL != g_watch_dog_arg_struct.client_argv[i])
	{
		g_communication_arg_vector[i] = g_watch_dog_arg_struct.client_argv[i];
		
		++i;
	}
}

int SetEnvVar(pid_t watch_dog_pid)
{
	char pid_buffer[16] = {0};

	/*set watch dog pid into envirement variable*/
    sprintf(pid_buffer, "%d", watch_dog_pid);
	
	/*set envirement variable*/
	if (0 != setenv(ENV_VAR_NAME, pid_buffer, 1))
	{
		perror("Failed setenv\n");
		
		exit(EXIT_FAILURE);
	}

	return 0;
}


/********************************************/
/*			 	Task Functions				*/
/********************************************/


int SendSignal(void *arg)
{	
	/*use printf to indicate progress. all prints can be remove*/
	if (0 == kill(g_communication_pid, SIGUSR1))
	{
		printf("%d send SIGUSR1 to: %d\n\n", getpid(), g_communication_pid);
	}
	else
	{
		printf("im %d and i failed to send to %d\n\n", getpid(), g_communication_pid);
	}
	
	printf("counter before: %d\n", atomic_load(&g_counter));
	
	/*increament global counter*/
	atomic_fetch_add(&g_counter, 1);
	
	printf("counter after: %d\n", atomic_load(&g_counter));
	
	if (NULL != getenv(ENV_VAR_NAME) && getpid() == atoi(getenv(ENV_VAR_NAME)))
	{
		printf("\n\t\t ---WatchDog process (%d)---\n", getpid());
	}
	else
	{
		printf("\n\t\t ---Client process (%d)---\n", getpid());
	}
	
	UNUSED(arg);
	
	return 0;
}

int CounterCheck(void *arg)
{
	sem_t *dnr_sem = sem_open(DNR_SEM_NAME, O_CREAT, SEM_PERMISSIONS, 0);
	sem_t *revivel_sem = sem_open(REVIVAL_SEM_NAME, 0);

	/*for cases DNR came first*/
	sem_post(dnr_sem);

	/*unable DNR to send signal*/
	sem_wait(dnr_sem);

	if (g_watch_dog_arg_struct.repitition <= (size_t)atomic_load(&g_counter))
	{
		/*print for progress indication. can be remove*/
		printf("\n\t\rim %d. reviving partner!!!\n\n", getpid());

		/*revive relevent process*/
		ProcessExec(g_communication_arg_vector);

		/*wait for process revival*/
		sem_wait(revivel_sem);
		
		/*reset counter*/
		atomic_store(&g_counter, 0);
	}
	
	UNUSED(arg);
	
	/*allow DNR to send signal*/
	sem_post(dnr_sem);
	
	/*close semaphoes*/
	sem_close(revivel_sem);
	sem_close(dnr_sem);
	
	return 0;
}

int DNRCheck(void *arg)
{
	if (1 == atomic_load(&g_dnr))
    {
		/*print for indication. can be remove*/
		printf("DNR recived in: %d\n", getpid());
		
        Heap_PQ_SchedulerStop(g_scheduler);
        
        return 0;
    }
	
	UNUSED(arg);
	
	return 0;
}

void DummyClean()
{
	return;
}


/********************************************/
/*			  Handler Functions				*/
/********************************************/


void Handler1(int signal, siginfo_t *info, void *ucontext)
{
	UNUSED(signal);
	UNUSED(ucontext);
	
	if (g_communication_pid == info->si_pid)
	{
		atomic_store(&g_counter, 0);
	}
}

void Handler2(int signal, siginfo_t *info, void *ucontext)
{	
	UNUSED(signal);
	UNUSED(ucontext);
	
	if (g_communication_pid == info->si_pid || getpid() == info->si_pid)
	{
		atomic_store(&g_dnr, 1);
	}
}


