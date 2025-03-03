
#include <stdio.h>		/*perror*/
#include <unistd.h>		/*getpid*/

#include "watch_dog_util.h"
#include "watch_dog_process.h"


int main(int argc, char **argv)
{
	if (-1 == SetEnvVar(getpid()))
	{
		perror("failed stting env var\n");
		
		return -1;
	}
	
	if (-1 == WatchDog(argv))
	{
		perror("watch_dog failed!\n");
	}
	
	UNUSED(argc);

	return 0;
}


