#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <stdio.h> 
#include <stdlib.h>
int main()
{
	pid_t fpid = fork();
	if(fpid < 0)
	{
		printf("Error\n");
	}
	else if (fpid == 0)  //子进程
	{
		printf("This is a son process.\n");
		sleep(60);
	}
	else                 //父进程
	{
		printf("This is a parent process.\n");
		sleep(10);
	}	
}