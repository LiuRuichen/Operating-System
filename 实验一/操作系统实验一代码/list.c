#include <stdio.h>
#include <unistd.h>
int main ()
{
	pid_t pid;
	for (int i=0; i<100; i++)
	{
		pid = fork();
		if (pid < 0)
		{
			printf("error\n");
		}
		else if(pid != 0)
		{
			break;
		}
		else
		{
			printf("I am the process %d, and my parent is process %d\n",getpid(),getppid());
		}
	}
	while(1){sleep(1);}
	return 0;
}