/*
Name: Avshalom Ariel
Reveiwer: Shahar Marom
date: 28/05/2023
status: Approved
*/

#include <time.h> /*time*/
#include <unistd.h> /*getpid*/
#include <pthread.h> /* pthread_mutex_t pthread_mutex_lock pthread_mutex_unlock */

#include "uid.h" /*ilrd_uid_t*/

static size_t count = 0;
static pthread_mutex_t lock;

ilrd_uid_t UIDCreate(void)
{
	ilrd_uid_t result = {0};
	
	pthread_mutex_lock(&lock);
	
	result.timestamp = time(NULL);
	if((time_t)-1 == result.timestamp)
	{
		return UIDGetBad(); 
	}
	
	result.counter = count;
	result.pid = getpid();
	
	++count;
	
	pthread_mutex_unlock(&lock);
	
	return result;
}


int UIDIsEqual(ilrd_uid_t uid1, ilrd_uid_t uid2)
{
	int result = 1;
	
	result *= (uid1.timestamp == uid2.timestamp);
	result *= (uid1.pid == uid2.pid);
	result *= (uid1.counter == uid2.counter);
	
	return result;
}


ilrd_uid_t UIDGetBad(void)
{
	ilrd_uid_t uid = {(time_t)-1, 0, 0};
	
	return uid;
}
	
/*	printf("time is: %s", ctime(&uid.timestamp));*/
