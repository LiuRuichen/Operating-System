#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
	printf("This is process %d\n",getpid());
	getchar();

	char *buf = (char *)malloc(2500LL*1024*1024*sizeof(char));  //4 Gigabit

	if(!buf)
	{
		printf("This is null!\n");
	}
	
	printf("Initialization over and programme is going to run!\n");
	getchar();

	for(long long i=0; i<2500LL*1024*1024; i+=(4*1024))
	{
		buf[i] = '1';	
	}
}