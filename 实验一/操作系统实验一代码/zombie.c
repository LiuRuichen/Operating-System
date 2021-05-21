#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <stdio.h> 
#include <stdlib.h>
int main(int argc, char **argv)
{
    	int fpid = fork();
	printf("this is %d, its parent is %d\n",getpid(),getppid());
	if (fpid < 0)
	{
		printf("Error!\n");
	}
	else if (fpid == 0)
	{
		sleep(5);
		exit(0);
	}
	else
	{
		sleep(30);
	}
}