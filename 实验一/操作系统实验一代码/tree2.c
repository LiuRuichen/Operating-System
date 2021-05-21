#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <stdio.h> 
#include <stdlib.h>
int main()
{
	for(int i=0; i<5; i++)            
	{
		pid_t fpid1 = fork();
		pid_t fpid2 = 0;
		//printf("this is %d\n",fpid1);
		printf("this is %d, and its parent is %d\n",getpid(),getppid());
		if (fpid1 > 0)
		{
			fpid2 = fork();
			printf("this is %d, and its parent is %d\n",getpid(),getppid());
		}
		if(fpid1>0 && fpid2>0)
		{
			break;
		}
	}
	while(1){}                            
}