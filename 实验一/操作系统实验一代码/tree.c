#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
int main ()
{
    pid_t fpid;
    int num = 0;
    for (int i=0; i<100; i++)
    {
        fpid = fork();
		
        if (fpid < 0)
		{
			printf("error\n");
		}
        else if (fpid == 0)
		{
			printf("I'm the process %d, and my parent process is %d\n",getpid(),getppid());
			sleep(50);
			return 0;
		}	
    }
    while(1){}
    return 0;
}