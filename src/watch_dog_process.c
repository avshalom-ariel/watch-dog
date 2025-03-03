#define _POSIX_C_SOURCE 200112L	/*unsetenv*/
#include <semaphore.h>			/*sem_t, semaphore functions*/
#include <stdio.h>				/*perror*/
#include <unistd.h>				/*getppid*/
#include <signal.h>				/*struct sigaction*/
#include <stddef.h>				/*size_t*/
#include <stdlib.h>				/*atoi, unsetenv exit*/

#include "watch_dog_util.h"
#include "watch_dog_process.h"

int WatchDog(char **argv)
{
	sem_t *watch_dog_sem = sem_open(WATCH_DOG_SEM_NAME, 0);
	struct sigaction act_sig1 = {0};
	struct sigaction act_sig2 = {0};
	
	/*set watch dogstruct*/
	SetWatchDogStruct(argv + CLIENT_PATH_INDEX, 
			(size_t)atoi(argv[INTERVAL_INDEX]),
			(size_t)atoi(argv[REPITITION_INDEX]));
	
	/*set client execute arguments (for revivel when needed)*/
	SetClientArgv();
	
	/*set signal handlers*/
	if (-1 == SignalHandle(&act_sig1, &act_sig2, NULL, NULL))
	{
		perror("watch dog signal handlers failed\n");
		
		exit(EXIT_FAILURE);
	}
	
	/*set communication pid to client pid*/
	g_communication_pid = getppid();
	
	/*build scheduler with alll relevent tasks*/
	BuildCommunicationScheduler();
	
	/*tell MMI stop waiting*/
	sem_post(watch_dog_sem);
	
	/*start communication scheduler*/
	RunCommunicationScheduler();
	
	/*unset envirement variable*/
	unsetenv(ENV_VAR_NAME);
	
	/*close watch dog semaphore*/
	sem_close(watch_dog_sem);
	
	return 0;
}


