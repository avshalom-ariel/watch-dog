
#include <unistd.h>	/*sleep*/
#include <stdio.h>	/*printf*/

#include "watch_dog.h"

int main(int argc, char **argv)
{
	size_t i = 10;
	MMI(3, 5, argv);
	
	(void)argc;
	
	while(i)
	{
		printf("\n\nrepition %ld passed!\n\n", i);
		sleep(3);
		--i;
	}
	
	DNR();
	
	printf("JOB is done!\n");

	return 0;
}


