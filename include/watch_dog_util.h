#ifndef __WATCH_DOG_UTIL_H__
#define __WATCH_DOG_UTIL_H__

#include <signal.h>		/*struct sigaction*/
#include <sys/types.h>	/*pid_t*/

/*read write permissions for user and group*/
#define SEM_PERMISSIONS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define WATCH_DOG_SEM_NAME "/watch_dog_sem"
#define REVIVAL_SEM_NAME "/revival_sem"
#define THREAD_SEM_NAME "/thread_sem"
#define DNR_SEM_NAME "/dnr_sem"
#define WATCH_DOG_EXEC "./watch_dog"
#define ENV_VAR_NAME "WD_PID"

#define CLIENT_PATH_INDEX 3
#define REPITITION_INDEX 1
#define INTERVAL_INDEX 2
#define NUM_OF_ARG 20
#define UNUSED(a) (void)a

typedef struct watch_dog_arg
{
	size_t interval_in_seconds;
	size_t repitition;
	char *client_argv[NUM_OF_ARG];
	int client_argc;
}watch_dog_arg_t;

/*extern variables;*/
extern char *g_communication_arg_vector[NUM_OF_ARG];
extern pid_t g_communication_pid;

/*API function declarations*/
int SignalHandle(struct sigaction *act_sig1, struct sigaction *act_sig2,
		 struct sigaction *old_sig_act1, struct sigaction *old_sig_act2);
void SetWatchDogStruct(char **argv, size_t interval_in_seconds, size_t repitition);
int SetEnvVar(pid_t watch_dog_pid);
void SetClientArgv(void);
void SetWatchDogArgv(void);
int BuildCommunicationScheduler(void);
int RunCommunicationScheduler(void);
int ProcessExec(char **argv);


#endif /*__WATCH_DOG_UTIL_H__*/
