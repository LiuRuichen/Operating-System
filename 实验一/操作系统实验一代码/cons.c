#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
int main()
{
	while(1)	
	{
		clock_t start = clock()/CLOCKS_PER_SEC;
		int i = 0;
		for (i=0; i<5; )
		{
			clock_t time = clock()/CLOCKS_PER_SEC;
			i = time - start;
		}
		sleep(5);
	}
}