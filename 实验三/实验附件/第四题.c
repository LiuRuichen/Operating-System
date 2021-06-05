#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main ()
{
	printf("This is process %d\n",getpid());
	getchar();

	char *buf;
	for(long i=0; ; i++)
	{
		buf = (char *)malloc(1024*1024*1024*sizeof(char));  //1 Gigabit
		if (!buf)
		{
			printf("No memory left! Exiting...\n");
			getchar();
			break;
		}
		else
		{
			printf("This is the %ld-time-allocation! Going on...\n",i);
		}
	}
}